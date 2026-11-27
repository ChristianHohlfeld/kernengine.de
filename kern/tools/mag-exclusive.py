#!/usr/bin/env python3
"""Root-owned MAG lease hook. Fixed units only; no arguments from the browser."""
import fcntl
import json
import os
from pathlib import Path
import subprocess
import sys

STATE=Path("/run/mag-lehrer-exclusive.json")
LOCK=Path("/run/mag-lehrer-exclusive.lock")
# Supervisor first, then its workloads. Never freeze network, storage or MAG's chat UI.
FREEZE=("mag-modeld.service","mag-model@lena.service","mag-model@noah.service",
        "mag-model@qwen.service","mag-idol.service","mag-stt.service","mag-scene.service")
STOP=("mag-h3d.service",)

def ctl(*args):
    return subprocess.run(["/usr/bin/systemctl",*args],check=True,text=True,capture_output=True).stdout.strip()

def save(entries):
    tmp=STATE.with_suffix(".tmp")
    tmp.write_text(json.dumps(entries),encoding="utf-8")
    os.chmod(tmp,0o600);os.replace(tmp,STATE)

def leave():
    if not STATE.exists():return
    entries=json.loads(STATE.read_text())
    errors=[]
    for unit,op in reversed(entries):
        try:
            ctl("thaw" if op=="freeze" else "start",unit)
        except subprocess.CalledProcessError as e:
            errors.append((unit,op))
            print(unit,e.stderr,file=sys.stderr)
    if errors:
        save(list(reversed(errors)))
        raise RuntimeError("MAG restoration incomplete")
    STATE.unlink()

def enter():
    if STATE.exists():
        raise RuntimeError("previous lease needs restoration")
    entries=[]
    save(entries)
    try:
        for unit in FREEZE:
            if ctl("show",unit,"-p","ActiveState","--value")!="active":continue
            if ctl("show",unit,"-p","FreezerState","--value")!="running":continue
            entries.append([unit,"freeze"]);save(entries)
            ctl("freeze",unit)
        for unit in STOP:
            if ctl("show",unit,"-p","ActiveState","--value")!="active":continue
            entries.append([unit,"stop"]);save(entries)
            ctl("stop",unit)
    except BaseException:
        leave();raise

if __name__=="__main__":
    if os.geteuid()!=0 or len(sys.argv)!=2 or sys.argv[1] not in ("enter","leave"):
        sys.exit("root: mag-lehrer-exclusive enter|leave")
    os.umask(0o077)
    with LOCK.open("a") as lock:
        fcntl.flock(lock,fcntl.LOCK_EX)
        (enter if sys.argv[1]=="enter" else leave)()
