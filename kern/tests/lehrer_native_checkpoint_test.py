#!/usr/bin/env python3
"""Compare uninterrupted training and a new process restored from a complete checkpoint."""
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

binary=str(Path(sys.argv[1]).resolve())
def sections(path):
    data=path.read_bytes()
    magic,iteration=struct.unpack_from("<Ii",data)
    assert magic==0x4b4c4e33
    at=8
    result=[]
    for _ in range(5):
        n=struct.unpack_from("<Q",data,at)[0];at+=8
        assert n<=len(data)-at
        result.append(data[at:at+n]);at+=n
    assert at==len(data)
    return iteration,result

with tempfile.TemporaryDirectory() as directory:
    root=Path(directory)
    def run(name,iterations,resume=None):
        checkpoint=root/(name+".ckpt")
        cmd=[binary,"--full-checkpoint","--progress-json","--task","stand","--threads","2","--envs","4",
             "--steps","4","--epochs","1","--minibatches","1","--actor","16,16","--critic","16,16",
             "--warm-start","none","--seed","17","--iterations",str(iterations),"--save-every","25",
             "--out",str(root/(name+".onnx")),"--checkpoint",str(checkpoint)]
        if resume:cmd+=["--resume",str(resume)]
        subprocess.run(cmd,check=True)
        return checkpoint
    baseline=run("baseline",4)
    partial=run("partial",2)
    resumed=run("resumed",4,partial)
    i,a=sections(baseline);j,b=sections(resumed)
    assert i==j==4
    for index,name in enumerate(("PPO","collectors","imitation","ONNX")):
        assert a[index]==b[index],name+" differs after resume"
    assert (root/"baseline.onnx").read_bytes()==(root/"resumed.onnx").read_bytes()
    print("Native exact checkpoint continuation passed")
