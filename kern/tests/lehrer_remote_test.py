#!/usr/bin/env python3
"""Exercise real HTTP and subprocess control without GPUs or systemd."""
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import tempfile
import threading
import time
import unittest
from unittest.mock import patch
import urllib.error
import urllib.request
from types import SimpleNamespace
import uuid

TOOLS=Path(__file__).resolve().parents[1]/"tools"
def load(name,path):
    spec=importlib.util.spec_from_file_location(name,path)
    mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod);return mod
remote=load("remote",TOOLS/"lehrer_remote.py")
exclusive=load("exclusive",TOOLS/"mag-exclusive.py")
BUILD="b"*40
ORIGIN="https://kernengine.de"
FAKE=r'''#!/usr/bin/env python3
import json,os,signal,sys,time,struct
from pathlib import Path
if "--hip-check" in sys.argv:
    print("HIP devices: 2",flush=True);sys.exit(0)
stop=False
def halt(*args):
    global stop
    stop=True
signal.signal(signal.SIGTERM,halt)
args=sys.argv
out=Path(args[args.index("--out")+1])
ckpt=Path(args[args.index("--checkpoint")+1])
total=int(args[args.index("--iterations")+1])
first=1
if "--resume" in args:
    first=struct.unpack_from("<Ii",Path(args[args.index("--resume")+1]).read_bytes())[1]+1
for it in range(first,total+1):
    time.sleep(.04)
    tmp=out.with_suffix(".tmp");tmp.write_bytes(b"onnx");os.replace(tmp,out)
    payload=struct.pack("<Ii",0x4b4c4e32,it)
    for part in (b"KERNPPO2fake",b"collectors",b"",b"onnx"):
        payload+=struct.pack("<Q",len(part))+part
    tmp=ckpt.with_suffix(".tmp");tmp.write_bytes(payload);os.replace(tmp,ckpt)
    print("KERN_PROGRESS "+json.dumps(dict(iteration=it,steps=it*20,episodes=1,meanReturn=.1,meanLength=20,meanStd=.1,collectSeconds=.02,learnSeconds=.02,sps=500,snapshot=True,report={"meanErrorDeg":2})),flush=True)
    if stop:break
'''
class RemoteTest(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.root=Path(self.tmp.name)
        binary=self.root/"fake";binary.write_text(FAKE);binary.chmod(0o700)
        lock=self.root/"gpu.lock";lock.touch();Path(str(lock)+".idol").touch()
        self.server=remote.Server(SimpleNamespace(jobs=str(self.root/"jobs"),binary=str(binary),build=BUILD,
            origin=[ORIGIN],gpu_lock=str(lock),hook="",port=0,tailnet_only=True))
        self.base="http://127.0.0.1:"+str(self.server.server_address[1])
        self.thread=threading.Thread(target=self.server.serve_forever);self.thread.start()
        self.id=str(uuid.uuid4())
        self.body=dict(id=self.id,build=BUILD,task="stand",iterations=300,envs=1,seed=1)
    def tearDown(self):
        job=self.server.job
        if job and job.state not in remote.TERMINAL:
            job.control("stop");job.thread.join(10)
        self.server.shutdown();self.thread.join(5);self.server.server_close()
        self.tmp.cleanup()
    def request(self,path,body=None,marker="1",origin=ORIGIN,method=None):
        data=None if body is None else body if isinstance(body,bytes) else json.dumps(body).encode()
        req=urllib.request.Request(self.base+path,data=data,headers={"X-Kern-Lehrer":marker,
            "Origin":origin,"Content-Type":"application/octet-stream" if isinstance(body,bytes) else "application/json"},method=method)
        try:
            with urllib.request.urlopen(req,timeout=3) as r:
                content=r.read()
                return r.status,r.headers, json.loads(content) if r.headers.get_content_type()=="application/json" else content
        except urllib.error.HTTPError as e:
            return e.code,e.headers,json.loads(e.read())
    def job(self):
        code,_,data=self.request("/v1/jobs/"+self.id)
        self.assertEqual(code,200);return data
    def wait(self,predicate):
        end=time.monotonic()+10
        while time.monotonic()<end:
            data=self.job()
            if predicate(data):return data
            time.sleep(.05)
        self.fail("job state did not arrive: "+repr(data))
    def test_auth_validation_and_control(self):
        self.assertEqual(self.request("/v1/info",marker="bad")[0],403)
        self.assertEqual(self.request("/v1/info")[0],200)
        self.assertEqual(self.request("/v1/jobs",self.body,marker="")[0],403)
        self.assertEqual(self.request("/v1/info",origin="https://bad.example",method="OPTIONS")[0],403)
        self.assertEqual(self.request("/v1/info",method="OPTIONS")[0],204)
        self.assertEqual(self.request("/v1/info",origin="https://bad.example")[0],403)
        self.assertEqual(self.request("/v1/jobs",{**self.body,"build":"0"*40})[0],400)
        self.assertEqual(self.request("/v1/jobs",{**self.body,"task":"--out /tmp/evil"})[0],400)
        self.assertEqual(self.request("/v1/jobs",{**self.body,"envs":0})[0],400)
        self.assertEqual(self.request("/v1/jobs",self.body)[0],200)
        self.assertEqual(self.request("/v1/jobs",self.body)[2]["id"],self.id)
        self.assertEqual(self.request("/v1/jobs",{**self.body,"id":str(uuid.uuid4())})[0],409)
        data=self.wait(lambda d:d["state"]=="running" and d["network"])
        self.assertEqual(data["gpus"],2)
        self.assertEqual(data["workers"],len(os.sched_getaffinity(0)))
        self.assertEqual(self.request("/v1/current")[2]["id"],self.id)
        self.assertEqual(self.request("/v1/jobs/"+self.id+"/pause",{})[2]["state"],"paused")
        # A buffered in-flight iteration can arrive just after the pause acknowledgment.
        time.sleep(.12);before=self.job()["stats"]["iteration"]
        time.sleep(.15);self.assertEqual(self.job()["stats"]["iteration"],before)
        self.assertEqual(self.request("/v1/jobs/"+self.id+"/resume",{})[2]["state"],"running")
        self.wait(lambda d:d["stats"]["iteration"]>before)
        self.assertEqual(self.request("/v1/jobs/"+self.id+"/stop",{})[0],200)
        data=self.wait(lambda d:d["state"] in remote.TERMINAL)
        self.assertEqual(data["state"],"stopped")
        self.assertEqual(self.request("/v1/jobs/"+self.id+"/network/"+data["network"])[2],b"onnx")
        self.assertTrue((self.root/"jobs"/self.id/"learner.ckpt").exists())
        # Both lock files must be available after the job ends.
        import fcntl
        for filename in ("gpu.lock","gpu.lock.idol"):
            with (self.root/filename).open("rb") as f:fcntl.flock(f,fcntl.LOCK_EX|fcntl.LOCK_NB)
    def test_cancel_waiting(self):
        import fcntl
        with (self.root/"gpu.lock.idol").open("rb") as f:
            fcntl.flock(f,fcntl.LOCK_SH)
            self.assertEqual(self.request("/v1/jobs",self.body)[0],200)
            self.assertEqual(self.request("/v1/jobs/"+self.id+"/stop",{})[0],200)
            data=self.wait(lambda d:d["state"] in remote.TERMINAL)
            self.assertEqual(data["state"],"stopped")
            self.assertIsNone(self.server.job.proc)
    def test_finish(self):
        self.assertEqual(self.request("/v1/jobs",{**self.body,"iterations":2})[0],200)
        data=self.wait(lambda d:d["state"] in remote.TERMINAL)
        self.assertEqual(data["state"],"done")
        self.assertEqual(data["stats"]["iteration"],2)
        self.assertEqual(data["config"]["task"],"stand")
        self.assertEqual(data["config"]["seed"],1)
        history=self.request("/v1/jobs/"+self.id+"/history")[2]["history"]
        self.assertEqual([p["it"] for p in history],[1,2])
        self.assertAlmostEqual(history[-1]["len"],.4)

    def test_checkpoint_roundtrip(self):
        self.assertEqual(self.request("/v1/jobs",self.body)[0],200)
        self.wait(lambda d:d["stats"] and d["stats"]["iteration"]>=2)
        self.assertEqual(self.request("/v1/jobs/"+self.id+"/pause",{})[0],200)
        code,_,archive=self.request("/v1/jobs/"+self.id+"/checkpoint")
        self.assertEqual(code,200)
        meta,payload=remote.decode_archive(archive)
        self.assertEqual(meta["backend"],"mag")
        self.assertEqual(meta["config"]["task"],"stand")
        self.assertEqual(meta["iteration"],remote.native_checkpoint_iteration(payload))
        self.assertEqual(remote.native_checkpoint_network(payload),b"onnx")
        target="/v1/checkpoints/"+str(uuid.uuid4())
        self.assertEqual(self.request(target,archive)[0],409)
        self.assertEqual(self.request("/v1/jobs/"+self.id+"/stop",{})[0],200)
        self.wait(lambda d:d["state"] in remote.TERMINAL)
        self.assertEqual(self.request(target,archive[:-1]+bytes([archive[-1]^1]))[0],400)
        bad=remote.encode_archive({**meta,"build":"0"*40},payload)
        self.assertEqual(self.request(target,bad)[0],400)
        self.assertEqual(self.request(target,b"onnx")[0],400)
        code,_,resumed=self.request(target,archive)
        self.assertEqual(code,200)
        self.id=resumed["id"]
        self.assertEqual(self.request(target,archive)[0],200)
        self.assertEqual((self.root/"jobs"/self.id/"resume.ckpt").read_bytes(),payload)
        self.wait(lambda d:d["stats"] and d["stats"]["iteration"]>meta["iteration"])
        history=self.request("/v1/jobs/"+self.id+"/history")[2]["history"]
        self.assertEqual(history[0]["it"],1)

    def reopen(self):
        old=self.server
        old.shutdown();self.thread.join(5);old.server_close()
        self.server=remote.Server(SimpleNamespace(jobs=str(self.root/"jobs"),binary=str(self.root/"fake"),build=BUILD,
            origin=[ORIGIN],gpu_lock=str(self.root/"gpu.lock"),hook="",port=0,tailnet_only=True))
        self.base="http://127.0.0.1:"+str(self.server.server_address[1])
        self.thread=threading.Thread(target=self.server.serve_forever);self.thread.start()

    def test_completed_job_survives_restart(self):
        body={**self.body,"iterations":2}
        self.assertEqual(self.request("/v1/jobs",body)[0],200)
        prior=self.wait(lambda d:d["state"] in remote.TERMINAL)
        self.server.job.thread.join(5)
        self.reopen()
        self.assertEqual(self.request("/v1/current")[2],prior)
        self.assertEqual(self.request("/v1/jobs",body)[2],prior)
        self.assertEqual(self.request("/v1/jobs/"+self.id+"/network/"+prior["network"])[2],b"onnx")
        self.assertIsNone(self.server.job.proc)

    def test_interrupted_job_is_not_restarted(self):
        self.assertEqual(self.request("/v1/jobs",{**self.body,"iterations":2})[0],200)
        data=self.wait(lambda d:d["state"] in remote.TERMINAL)
        self.server.job.thread.join(5)
        path=self.root/"jobs"/self.id/"job.json"
        meta=json.loads(path.read_text())
        meta["public"]["state"]="running"
        meta["public"]["message"]="Learning on MAG"
        remote.atomic_json(path,meta)
        self.reopen()
        recovered=self.request("/v1/current")[2]
        self.assertEqual(recovered["state"],"error")
        self.assertIn("interrupted",recovered["message"])
        self.assertEqual(recovered["id"],self.id)
        self.assertEqual(recovered["network"],data["network"])
        self.assertIsNone(self.server.job.proc)

    def test_shutdown_rejects_new_jobs(self):
        self.server.closing=True
        self.assertEqual(self.request("/v1/jobs",self.body)[0],503)
        self.assertIsNone(self.server.job)

class FakeSystemd:
    def __init__(self):
        self.active={unit:"active" for unit in (*exclusive.FREEZE,*exclusive.STOP)}
        self.frozen={unit:"running" for unit in exclusive.FREEZE}
        self.actions=[]
        self.fail=None
    def ctl(self,*args):
        op,unit=args[:2]
        if op=="show":
            return self.active[unit] if args[3]=="ActiveState" else self.frozen[unit]
        self.actions.append((op,unit))
        if self.fail==(op,unit):
            raise subprocess.CalledProcessError(1,args,stderr="test")
        if op=="freeze":self.frozen[unit]="frozen"
        elif op=="thaw":self.frozen[unit]="running"
        elif op=="stop":self.active[unit]="inactive"
        elif op=="start":self.active[unit]="active"
        else:raise AssertionError(args)
        return ""

class LeaseTest(unittest.TestCase):
    def test_restore_only_originally_running_units_in_reverse_order(self):
        with tempfile.TemporaryDirectory() as tmp:
            state=Path(tmp)/"lease.json";system=FakeSystemd()
            system.active["mag-model@noah.service"]="inactive"
            system.frozen["mag-model@lena.service"]="frozen"
            before=(system.active.copy(),system.frozen.copy())
            with patch.object(exclusive,"STATE",state),patch.object(exclusive,"ctl",system.ctl):
                exclusive.enter()
                actions=system.actions.copy()
                self.assertNotIn(("freeze","mag-model@lena.service"),actions)
                self.assertNotIn(("freeze","mag-model@noah.service"),actions)
                self.assertEqual(actions[0],("freeze","mag-modeld.service"))
                self.assertEqual(system.active["mag-h3d.service"],"inactive")
                self.assertTrue(all(system.frozen[u]=="frozen" for u in exclusive.FREEZE if system.active[u]=="active"))
                exclusive.leave()
                self.assertFalse(state.exists())
                self.assertEqual(system.actions[len(actions):],[("thaw" if op=="freeze" else "start",unit) for op,unit in reversed(actions)])
                self.assertEqual((system.active,system.frozen),before)
                exclusive.leave()  # idempotent
    def test_partial_enter_failure_restores(self):
        with tempfile.TemporaryDirectory() as tmp:
            state=Path(tmp)/"lease.json";system=FakeSystemd()
            system.fail=("freeze","mag-model@noah.service")
            before=(system.active.copy(),system.frozen.copy())
            with patch.object(exclusive,"STATE",state),patch.object(exclusive,"ctl",system.ctl):
                with self.assertRaises(subprocess.CalledProcessError):exclusive.enter()
                self.assertIn(("thaw","mag-modeld.service"),system.actions)
                self.assertFalse(state.exists())
                self.assertEqual((system.active,system.frozen),before)
    def test_failed_restore_is_reported_and_retryable(self):
        with tempfile.TemporaryDirectory() as tmp:
            state=Path(tmp)/"lease.json";system=FakeSystemd()
            unit="mag-model@qwen.service"
            with patch.object(exclusive,"STATE",state),patch.object(exclusive,"ctl",system.ctl):
                exclusive.enter()
                system.fail=("thaw",unit)
                with self.assertRaisesRegex(RuntimeError,"restoration incomplete"):exclusive.leave()
                self.assertEqual(json.loads(state.read_text()),[[unit,"freeze"]])
                with self.assertRaisesRegex(RuntimeError,"previous lease"):exclusive.enter()
                system.fail=None
                exclusive.leave()
                self.assertFalse(state.exists())
                self.assertTrue(all(v=="running" for v in system.frozen.values()))
                self.assertTrue(all(v=="active" for v in system.active.values()))

if __name__=="__main__":unittest.main()
