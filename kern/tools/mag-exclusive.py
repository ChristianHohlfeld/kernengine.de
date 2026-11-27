#!/usr/bin/env python3
"""Root-owned MAG lease hook. Fixed units only; no arguments from the browser."""
import fcntl
import json
import os
from pathlib import Path
import subprocess
import sys
import time

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

def state(unit, prop):
    return ctl("show",unit,"-p",prop,"--value")

def wait_state(unit, prop, accepted, timeout=15):
    deadline=time.monotonic()+timeout
    while True:
        value=state(unit,prop)
        if value in accepted:return value
        if time.monotonic()>=deadline:
            raise RuntimeError(f"{unit}: {prop}={value!r}, expected {accepted}")
        time.sleep(0.1)

def settled(unit):
    return wait_state(unit,"ActiveState",("active","inactive","failed"))

def thaw(unit):
    # A recorded operation is an intent, so recovery is idempotent even
    # after a crash between journalling and executing systemctl.
    current=settled(unit)
    if current!="active":
        ctl("start",unit)
        wait_state(unit,"ActiveState",("active",))
    frozen=wait_state(unit,"FreezerState",("running","frozen"))
    if frozen=="frozen":
        ctl("thaw",unit)
    wait_state(unit,"FreezerState",("running",))

def leave():
    if not STATE.exists():return
    entries=json.loads(STATE.read_text())
    # Refuse unknown operations/units, even in a damaged journal.
    if not isinstance(entries,list) or any(
        not isinstance(e,list) or len(e)!=2 or
        not ((e[1]=="freeze" and e[0] in FREEZE) or (e[1]=="stop" and e[0] in STOP))
        for e in entries):
        raise RuntimeError("invalid MAG restoration journal")
    errors=[]
    # Restore workloads first; supervisor last. The caller still owns both
    # GPU locks until this returns, so the GPU guard cannot race the trainer.
    for unit,op in reversed(entries):
        try:
            if op=="freeze":
                thaw(unit)
            else:
                if settled(unit)!="active":ctl("start",unit)
                wait_state(unit,"ActiveState",("active",))
        except (subprocess.CalledProcessError,RuntimeError) as e:
            errors.append([unit,op])
            print(unit,getattr(e,"stderr",None) or str(e),file=sys.stderr)
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
        # The remote job holds the priority and GPU locks before calling
        # enter. GPU inference has already unloaded via MAG's GPU guard.
        # Freeze the supervisor first to prevent new model starts.
        for unit in FREEZE:
            if settled(unit)!="active":continue
            frozen=wait_state(unit,"FreezerState",("running","frozen"))
            if frozen=="frozen":continue
            entries.append([unit,"freeze"]);save(entries)
            ctl("freeze",unit)
            wait_state(unit,"FreezerState",("frozen",))
        for unit in STOP:
            if settled(unit)!="active":continue
            entries.append([unit,"stop"]);save(entries)
            ctl("stop",unit)
            wait_state(unit,"ActiveState",("inactive",))
        # Verify the reservation after all transitions; never start HIP
        # while a known inference workload is still running.
        for unit in FREEZE:
            if settled(unit)=="active":
                wait_state(unit,"FreezerState",("frozen",))
        for unit in STOP:
            if settled(unit)=="active":
                raise RuntimeError(f"{unit}: still active during trainer reservation")
    except BaseException:
        leave();raise

if __name__=="__main__":
    if os.geteuid()!=0 or len(sys.argv)!=2 or sys.argv[1] not in ("enter","leave"):
        sys.exit("root: mag-lehrer-exclusive enter|leave")
    os.umask(0o077)
    with LOCK.open("a") as lock:
        fcntl.flock(lock,fcntl.LOCK_EX)
        (enter if sys.argv[1]=="enter" else leave)()
