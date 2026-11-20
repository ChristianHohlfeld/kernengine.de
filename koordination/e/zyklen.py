import re,glob,sys
src={}
for f in glob.glob(sys.argv[1]+'/game*.cpp'):
    t=open(f).read()
    for m in re.finditer(r'^[^\n;{}]*?\bGame::(\w+)\s*\([^;{]*?\)\s*(const\s*)?\{',t,re.M):
        i=m.end();d=1
        while d and i<len(t):
            d+= (t[i]=='{')-(t[i]=='}'); i+=1
        src.setdefault(m.group(1),'')
        src[m.group(1)]+=t[m.end():i]
names=set(src)
calls={n:set(c for c in re.findall(r'(?<![\w.>:])(\w+)\s*\(',b) if c in names) for n,b in src.items()}
# SCC via Tarjan
idx={};low={};st=[];on=set();out=[];c=[0]
sys.setrecursionlimit(10000)
def sc(v):
    idx[v]=low[v]=c[0];c[0]+=1;st.append(v);on.add(v)
    for w in calls[v]:
        if w not in idx: sc(w);low[v]=min(low[v],low[w])
        elif w in on: low[v]=min(low[v],idx[w])
    if low[v]==idx[v]:
        s=[]
        while True:
            w=st.pop();on.discard(w);s.append(w)
            if w==v:break
        if len(s)>1 or v in calls[v]: out.append(s)
for v in names:
    if v not in idx: sc(v)
for s in out: print(len(s),sorted(s))
big=max(out,key=len); S=set(big)
for a in sorted(S):
    print(a,'->',sorted(calls[a]&S))
