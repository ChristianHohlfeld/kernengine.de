#!/usr/bin/env python3
"""Private, one-job remote Lehrer. HTTP never runs a shell or accepts a command/path."""
import argparse
import contextlib
import fcntl
import hmac
import json
import math
import os
from pathlib import Path
import re
import signal
import subprocess
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

TERMINAL = {"done", "stopped", "error"}
MAX_BODY = 65536

def validate(data, build, workers):
    if not isinstance(data, dict) or set(data) - {"id","build","task","iterations","envs","seed","initStd","learningRate"}:
        raise ValueError("invalid job fields")
    if data.get("build") != build:
        raise ValueError("Studio and MAG builds differ; update one before training")
    if not isinstance(data.get("id"),str) or not re.fullmatch(r"[a-f0-9-]{36}",data["id"]):
        raise ValueError("invalid job id")
    task = data.get("task")
    if not isinstance(task,str) or len(task.encode()) > 16384:
        raise ValueError("invalid task")
    if task not in ("jump","stand"):
        kind,sep,spec = task.partition(":")
        if not sep or kind not in ("motion","scenario") or not isinstance(json.loads(spec),dict):
            raise ValueError("invalid task")
    def integer(key, lo, hi):
        value = data.get(key)
        if type(value) is not int or not lo <= value <= hi:
            raise ValueError("invalid "+key)
        return value
    cfg = {"task":task,"iterations":integer("iterations",1,100000),
           "envs":integer("envs",1,min(64,4096//workers)),"seed":integer("seed",1,4294967295)}
    for key in ("initStd","learningRate"):
        val = data.get(key)
        if val is not None:
            if type(val) not in (int,float) or not math.isfinite(val) or not 0 < val <= 1:
                raise ValueError("invalid "+key)
            cfg[key] = val
    return cfg

class Job:
    def __init__(self, server, job_id, config):
        self.server, self.id, self.config = server, job_id, config
        self.dir = server.root / job_id
        self.dir.mkdir(mode=0o700,exist_ok=False)
        self.state, self.message = "waiting", "Waiting for MAG's GPU lease"
        self.proc = None
        self.stop = threading.Event()
        self.lock = threading.RLock()
        self.stats, self.report = None,None
        self.gpus = 0
        self.network = None
        self.thread = threading.Thread(target=self.run, name="lehrer-"+job_id, daemon=False)
    def public(self):
        with self.lock:
            return dict(id=self.id,state=self.state,message=self.message,stats=self.stats,
                        report=self.report,network=self.network,workers=self.server.workers,
                        gpus=self.gpus,total=self.config["iterations"],envs=self.config["envs"])
    def change(self,state,message):
        with self.lock:
            self.state,self.message = state,message
    def control(self, action):
        with self.lock:
            if self.state in TERMINAL:
                return
            if action == "stop":
                self.stop.set()
                if self.proc and self.proc.poll() is None:
                    # Continue a stopped process before asking it to checkpoint and finish.
                    with contextlib.suppress(ProcessLookupError):
                        os.killpg(self.proc.pid,signal.SIGCONT)
                        os.killpg(self.proc.pid,signal.SIGTERM)
                self.change("stopping","Saving the last training iteration")
            elif action in ("pause","resume") and self.state in ("running","paused") and self.proc and self.proc.poll() is None:
                with contextlib.suppress(ProcessLookupError):
                    os.killpg(self.proc.pid,signal.SIGSTOP if action=="pause" else signal.SIGCONT)
                self.change("paused" if action=="pause" else "running", "Paused on MAG" if action=="pause" else "Learning on MAG")
            else:
                raise ValueError("training is not ready to pause or resume")
    def acquire(self, stack, filename):
        file = stack.enter_context(open(filename,"rb"))
        while not self.stop.is_set():
            try:
                fcntl.flock(file,fcntl.LOCK_EX|fcntl.LOCK_NB)
                return file
            except BlockingIOError:
                self.stop.wait(0.25)
        return None
    def snapshot(self):
        path = self.dir/"network.onnx"
        if path.is_file() and path.stat().st_size:
            with self.lock:
                self.network = str(path.stat().st_mtime_ns)
    def run(self):
        server = self.server
        entered = False
        state,message = "error","Training failed"
        try:
            with contextlib.ExitStack() as stack:
                # The existing MAG GPU guard yields when .idol is locked; active
                # Idol work holds it shared. Drain that work, never kill it.
                if server.gpu_lock:
                    if not self.acquire(stack,server.gpu_lock+".idol") or not self.acquire(stack,server.gpu_lock):
                        self.change("stopped","Stopped before GPU allocation")
                        return
                if self.stop.is_set():
                    self.change("stopped","Stopped before training")
                    return
                try:
                    if server.hook:
                        entered = True  # restore even when enter partially failed
                        subprocess.run(server.hook+["enter"],check=True,timeout=120)
                    if self.stop.is_set():
                        state,message = "stopped","Stopped before training"
                    else:
                        self.change("checking","Checking native HIP on every visible GPU")
                        check = subprocess.run([server.binary,"--hip-check"],capture_output=True,text=True,timeout=90,check=True)
                        match = re.search(r"HIP devices: ([1-9][0-9]*)",check.stdout)
                        if not match:
                            raise RuntimeError("native trainer has not verified a GPU")
                        with self.lock:
                            self.gpus = int(match.group(1))
                        if self.stop.is_set():
                            state,message = "stopped","Stopped before training"
                        else:
                            cfg = self.config
                            cmd = [server.binary,"--progress-json","--task",cfg["task"],
                                   "--threads",str(server.workers),"--envs",str(server.workers*cfg["envs"]),
                                   "--iterations",str(cfg["iterations"]),"--seed",str(cfg["seed"]),
                                   "--save-every","25","--eval-every","25","--out",str(self.dir/"network.onnx"),
                                   "--checkpoint",str(self.dir/"learner.ckpt")]
                            for field,flag in (("initStd","--init-std"),("learningRate","--lr")):
                                if field in cfg:
                                    cmd += [flag,str(cfg[field])]
                            with open(self.dir/"training.log","w",encoding="utf-8") as log:
                                with self.lock:
                                    if self.stop.is_set():
                                        state,message = "stopped","Stopped before training"
                                    else:
                                        self.proc = subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,
                                            text=True,bufsize=1,start_new_session=True)
                                        self.change("running","Learning on MAG")
                                if self.proc:
                                    for line in self.proc.stdout:
                                        log.write(line); log.flush()
                                        if line.startswith("KERN_PROGRESS "):
                                            stats = json.loads(line[len("KERN_PROGRESS "):])
                                            with self.lock:
                                                self.report = stats.get("report",self.report)
                                                self.stats = stats
                                            if stats.get("snapshot"):
                                                self.snapshot()
                                    code = self.proc.wait()
                                    self.snapshot()
                                    if code != 0:
                                        state,message = "error","Native training exited with code "+str(code)
                                    elif self.stop.is_set():
                                        state,message = "stopped","Stopped; latest network and checkpoint kept"
                                    elif self.network:
                                        state,message = "done","Training complete"
                                    else:
                                        state,message = "error","Trainer completed without an ONNX network"
                finally:
                    if self.proc and self.proc.poll() is None:
                        with contextlib.suppress(ProcessLookupError):
                            os.killpg(self.proc.pid,signal.SIGCONT)
                            os.killpg(self.proc.pid,signal.SIGTERM)
                        self.proc.wait()
                    if entered:
                        # Keep GPU locks until restoration is requested. The GPU guard
                        # resumes only when the locks are released.
                        subprocess.run(server.hook+["leave"],check=True,timeout=120)
        except Exception as exc:
            with open(self.dir/"error.log","a",encoding="utf-8") as f:
                f.write(repr(exc)+"\n")
            state,message = "error","Training or MAG restoration failed; see server logs"
        finally:
            # Early cancelled lease already has its terminal state.
            with self.lock:
                if self.state not in TERMINAL:
                    self.change(state,message)

class Server(ThreadingHTTPServer):
    daemon_threads = True
    def __init__(self, args):
        self.root = Path(args.jobs).resolve()
        self.root.mkdir(mode=0o700,parents=True,exist_ok=True)
        self.binary = str(Path(args.binary).resolve())
        self.build = args.build
        self.token = os.environ.get("KERN_LEHRER_TOKEN","")
        if len(self.token) < 32 or not re.fullmatch(r"[A-Za-z0-9_-]+", self.token):
            raise ValueError("KERN_LEHRER_TOKEN must be at least 32 URL-safe characters")
        if not re.fullmatch(r"[a-f0-9]{40}",self.build):
            raise ValueError("full Git commit required")
        self.origins = set(args.origin)
        self.workers = len(os.sched_getaffinity(0))
        self.gpu_lock = args.gpu_lock
        self.hook = ["/usr/bin/sudo","-n",args.hook] if args.hook else []
        self.job,self.job_lock = None,threading.RLock()
        super().__init__(("127.0.0.1",args.port),Handler)

class Handler(BaseHTTPRequestHandler):
    def log_message(self, fmt,*args):
        # Do not log request data or bearer tokens.
        pass
    def allowed_origin(self):
        origin = self.headers.get("Origin")
        return not origin or origin in self.server.origins
    def cors(self):
        origin = self.headers.get("Origin")
        if origin in self.server.origins:
            self.send_header("Access-Control-Allow-Origin",origin)
            self.send_header("Vary","Origin")
            self.send_header("Access-Control-Allow-Private-Network","true")
    def reply(self,status,data,ctype="application/json"):
        if ctype=="application/json":
            data = json.dumps(data,allow_nan=False).encode()
        self.send_response(status); self.cors()
        self.send_header("Content-Type",ctype)
        self.send_header("Cache-Control","no-store")
        self.send_header("Content-Length",str(len(data)))
        self.end_headers()
        with contextlib.suppress(BrokenPipeError,ConnectionResetError):
            self.wfile.write(data)
    def authorized(self):
        if not self.allowed_origin() or not hmac.compare_digest(self.headers.get("Authorization",""),"Bearer "+self.server.token):
            self.reply(403,{"error":"forbidden"})
            return False
        return True
    def do_OPTIONS(self):
        if not self.allowed_origin():
            self.reply(403,{"error":"forbidden"});return
        self.send_response(204);self.cors()
        self.send_header("Access-Control-Allow-Methods","GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers","Authorization, Content-Type")
        self.send_header("Content-Length","0");self.end_headers()
    def do_GET(self):
        if not self.authorized():return
        server=self.server
        if self.path=="/v1/info":
            self.reply(200,dict(build=server.build,workers=server.workers,gpuRequired=True));return
        with server.job_lock:
            job=server.job
        if not job or not self.path.startswith("/v1/jobs/"+job.id):
            self.reply(404,{"error":"job not found"});return
        suffix=self.path[len("/v1/jobs/"+job.id):]
        if suffix=="":
            self.reply(200,job.public());return
        if suffix.startswith("/network/"):
            rev=suffix.split("/")[-1];path=job.dir/"network.onnx"
            try:
                with path.open("rb") as f:
                    if str(os.fstat(f.fileno()).st_mtime_ns)!=rev:
                        self.reply(409,{"error":"newer snapshot available"});return
                    data=f.read(32*1024*1024+1)
                if len(data)>32*1024*1024:raise ValueError("network too large")
                self.reply(200,data,"application/octet-stream");return
            except (OSError,ValueError):
                self.reply(404,{"error":"network unavailable"});return
        self.reply(404,{"error":"not found"})
    def do_POST(self):
        if not self.authorized():return
        server=self.server
        try:
            length=int(self.headers.get("Content-Length","0"))
            if not 0<=length<=MAX_BODY:raise ValueError("request too large")
            data=json.loads(self.rfile.read(length) or b"{}")
            if self.path=="/v1/jobs":
                cfg=validate(data,server.build,server.workers)
                with server.job_lock:
                    job=server.job
                    if job and (job.id==data["id"] or job.state not in TERMINAL):
                        if job.config!=cfg:
                            self.reply(409,{"error":"MAG already has a different training job"});return
                    else:
                        job=Job(server,data["id"],cfg)
                        server.job=job;job.thread.start()
                self.reply(200,job.public());return
            with server.job_lock:
                job=server.job
            if job:
                prefix="/v1/jobs/"+job.id+"/"
                if self.path.startswith(prefix) and self.path[len(prefix):] in ("pause","resume","stop"):
                    job.control(self.path[len(prefix):]);self.reply(200,job.public());return
            self.reply(404,{"error":"not found"})
        except (ValueError,TypeError,KeyError,FileExistsError) as exc:
            self.reply(400,{"error":str(exc)})

def main():
    p=argparse.ArgumentParser()
    p.add_argument("--binary",required=True)
    p.add_argument("--build",required=True)
    p.add_argument("--jobs",required=True)
    p.add_argument("--port",type=int,default=8799)
    p.add_argument("--origin",action="append",default=["https://kernengine.de","https://www.kernengine.de"])
    p.add_argument("--gpu-lock",default="/var/lib/mag-idol/gpu.lock")
    p.add_argument("--hook",default="")
    args=p.parse_args()
    server=Server(args)
    def shutdown(_sig,_frame):
        threading.Thread(target=server.shutdown,daemon=True).start()
    signal.signal(signal.SIGTERM,shutdown);signal.signal(signal.SIGINT,shutdown)
    try:server.serve_forever()
    finally:
        if server.job and server.job.state not in TERMINAL:
            server.job.control("stop")
            server.job.thread.join()
        server.server_close()
if __name__=="__main__":main()
