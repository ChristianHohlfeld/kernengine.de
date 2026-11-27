// Test the existing Trainer's transport selection without opening a viewport.
import assert from "node:assert/strict";
import fs from "node:fs";
globalThis.location = {pathname: "/editor/", search:""};
globalThis.window = {KERN_TRAINER_BUILD:"b".repeat(40)};
if (!globalThis.navigator) globalThis.navigator = {hardwareConcurrency: 8};
globalThis.localStorage = {getItem:()=>null,setItem:()=>{}};
const src=fs.readFileSync(new URL("../web/studio/trainer.mjs",import.meta.url),"utf8")
  .replace(/^import \{.*\} from .*;\n/m,'const tr=x=>x, escHtml=x=>x, toBase64=x=>Buffer.from(x).toString("base64");\n');
const {Trainer}=await import("data:text/javascript;base64,"+Buffer.from(src).toString("base64"));
const p=Trainer.prototype;
const field=value=>({value:String(value)});
function trainer(backend){
  const t=Object.create(p);
  Object.assign(t,{task:"stand",run:null,module:{},el:{
    trCompute:field(backend),trRemoteUrl:field("https://mag.test:8443"),
    trWorkers:field(8),trEnvs:field(4),trIterations:field(2),trSeed:field(7),trInitStd:field(""),trLr:field("")
  },host:{querySelectorAll:()=>[]},draw(){},sync(){},status(s){this.message=s},end(run){assert.equal(this.run,run);this.run=null;}});
  return t;
}
const local=trainer("local");
local.remotePractice=()=>{throw Error("local must never contact MAG")};
local.spawn=()=>{throw Error("local worker path")};
await local.start();
assert.equal(local.message,"local worker path");
const mag=trainer("mag");
mag.spawn=()=>{throw Error("MAG must never spawn a local worker")};
mag.remotePractice=async run=>{
  assert.equal(run.remote.body.build,"b".repeat(40));
  assert.equal(run.remote.body.envs,4);
  assert.equal(run.remote.body.seed,7);
  assert.equal(run.remote.url,"https://mag.test:8443");
  assert.equal("token" in run.remote,false);
  assert.equal(run.remote.body.task,"stand");
  mag.end(run);
};
await mag.start();
assert.equal(mag.run,null);
// Existing progress and network delivery are used for remote results.
const t=trainer("mag"),id="a".repeat(8)+"-aaaa-aaaa-aaaa-"+"a".repeat(12);
const run=t.run={remote:{id:null,body:{id}},history:[],iteration:0,total:2};
let imported=0, stats=0, requests=[];
t.stats=(_,s)=>{stats=s.iteration};
t.goal=()=>{};
t.acceptNetwork=(_,bytes,iteration)=>{assert.deepEqual([...bytes],[1,2]);imported=iteration;};
t.remoteRequest=async (_,path)=>{
  requests.push(path);
  if(path.includes("/network/"))return new Uint8Array([1,2]);
  return {id,state:"done",message:"Training complete",workers:56,gpus:2,stats:{iteration:2,collectSeconds:1,learnSeconds:1},network:"1",networkIteration:2};
};
await p.remotePractice.call(t,run);
assert.equal(stats,2);assert.equal(imported,2);assert.equal(t.run,null);
assert.deepEqual(requests,["/v1/jobs","/v1/jobs/"+id+"/network/1"]);
// Only the documented idle response is a successful connection.
for (const [http,message,expected,isError] of [
  [404,"no MAG training job","Connected to MAG. Ready to start training.",false],
  [404,"not found","not found",true],
  [403,"forbidden","forbidden",true],
  [undefined,"Failed to fetch","Failed to fetch",true],
]) {
  const t=trainer("mag");
  t.status=(message,error=false)=>{t.message=message;t.statusError=error;};
  t.remoteRequest=async (_,path)=>{
    assert.equal(path,"/v1/current");
    throw Object.assign(Error(message),{http});
  };
  await t.reconnectRemote();
  assert.equal(t.run,null);
  assert.equal(t.message,expected);
  assert.equal(t.statusError,isError);
}
// A fresh page with the saved address queries the current job, never starts one.
const store = new Map();
globalThis.localStorage = {getItem:k=>store.get(k)??null,setItem:(k,v)=>store.set(k,v),removeItem:k=>store.delete(k)};
const active = {id,taskKind:"motion",state:"running",total:1500,envs:8,workers:56,gpus:1,
  stats:{iteration:34,collectSeconds:1,learnSeconds:1},network:null};
for (const state of ["running","paused","waiting","checking","stopping"]) {
  const recovered=trainer("local"), paths=[];
  recovered.remoteRequest=async (_,path)=>{paths.push(path);return {...active,state};};
  recovered.remotePractice=async run=>{
    assert.equal(run.remote.id,id);
    assert.equal(run.remote.body,undefined);
    assert.equal(run.remote.state,state);
    assert.equal(run.count,56);
    assert.equal(run.paused,state==="paused");
    assert.equal(run.move,null); // no automatic import into another level
    assert.equal(recovered.el.trCompute.value,"mag");
    recovered.end(run);
  };
  await recovered.reconnectRemote({automatic:true});
  assert.deepEqual(paths,["/v1/current"]);
}
const idle=trainer("local");
store.clear();
idle.remoteRequest=async ()=>({...active,state:"done"});
idle.remotePractice=()=>{throw Error("do not adopt unrelated finished job on opening");};
await idle.reconnectRemote({automatic:true});
assert.equal(idle.el.trCompute.value,"local");
assert.equal(idle.message,"Connected to MAG. Ready to start training.");
// Recover a completed job belonging to this browser, and a pending stop
// only if the server still reports the exact same job.
const url="https://mag.test:8443";
for (const [serverId,state,stopping] of [[id,"running",true],[id,"done",false],["b".repeat(8)+"-bbbb-bbbb-bbbb-"+"b".repeat(12),"running",false]]) {
  store.set("kern-lehrer.remote-job",JSON.stringify({url,id,stopping:true}));
  const recovered=trainer("local");
  recovered.remoteRequest=async (_,path)=>{assert.equal(path,"/v1/current");return {...active,id:serverId,state};};
  recovered.remotePractice=async run=>{assert.equal(run.remote.id,serverId);assert.equal(run.remote.stopping,stopping);recovered.end(run);};
  await recovered.reconnectRemote({automatic:true});
}
// A real timeout must not be interpreted as idle; local mode remains usable
// when there is no known remote job.
store.clear();
const offline=trainer("local");
offline.remoteRequest=async()=>{throw Error("timeout");};
await offline.reconnectRemote({automatic:true});
assert.equal(offline.el.trCompute.value,"local");
assert.equal(offline.run,null);
assert.equal(offline.message,"Local training ready. MAG unavailable.");
const opening=trainer("local");
opening.s={};
let discovered=0;
opening.reconnectRemote=options=>{assert.equal(options.automatic,true);discovered++;};
opening.discoverRemote();
assert.equal(discovered,1);
opening.el.trRemoteUrl.value="";
opening.discoverRemote();
assert.equal(discovered,1);
console.log("Lehrer local/MAG selection and remote result passed");
