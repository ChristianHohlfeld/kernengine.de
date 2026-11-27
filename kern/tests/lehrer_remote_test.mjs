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
console.log("Lehrer local/MAG selection and remote result passed");
