// The Lehrer's WebAssembly build (kern/build.sh --lehrer): a collector and the learner run one PPO
// iteration through the C ABI the browser's workers use, and the exported ONNX reads back.
//   node kern/tests/lehrer_wasm_test.mjs generated/kern_lehrer.wasm
import fs from "fs";
import { loadKern } from "../web/kern.mjs";

const file = process.argv[2];
const kern = await loadKern({ bytes: fs.readFileSync(file) });
const e = kern.exports, enc = new TextEncoder();
const put = bytes => { const p = e.kern_alloc(bytes.length); kern.u8.set(bytes, p); return p; };
const take = ptr => kern.u8.slice(ptr, ptr + e.kern_lehrer_result_length());
const check = (ok, what) => { if (!ok) { console.error("FAIL " + what); process.exit(1); } };

const task = enc.encode("jump"), config = enc.encode('{"steps":8}');
const tp = put(task), cp = put(config);
check(e.kern_lehrer_create(tp, task.length, 2, 5, 0) === 1, "collector");
check(e.kern_lehrer_learner(tp, task.length, cp, config.length, 3) === 1, "learner");
let policy = take(e.kern_lehrer_policy());
check(policy.length > 100000, "policy");
check(e.kern_lehrer_set_policy(put(policy), policy.length >> 2) === 1, "set policy");
// Imitation (the jump's recipe): a collector's share of a round, then the learner fits it.
check(e.kern_lehrer_imitation_left() === 8 && e.kern_lehrer_imitation_episodes() === 96, "imitation rounds");
const share = take(e.kern_lehrer_dagger(0, 0, 1));
const ticks = new DataView(share.buffer).getUint32(0, true);
check(ticks > 10 && share.length === 4 + ticks * 75 * 4, "imitation share");
const sframed = new Uint8Array(4 + share.length);
new DataView(sframed.buffer).setUint32(0, share.length, true);
sframed.set(share, 4);
const ip = e.kern_lehrer_imitate(put(sframed), sframed.length, 1);
check(ip !== 0, "imitate");
const im = JSON.parse(kern.string(ip, e.kern_lehrer_result_length()));
check(im.round === 1 && im.samples === ticks && im.episodes === 5, "imitation report " + JSON.stringify(im));
check(e.kern_lehrer_imitation_left() === 7, "imitation left");
policy = take(e.kern_lehrer_policy());
check(e.kern_lehrer_set_policy(put(policy), policy.length >> 2) === 1, "set imitated policy");
e.kern_lehrer_progress(1);
const t0 = performance.now();
const rollout = take(e.kern_lehrer_collect(8));
const ms = performance.now() - t0;
check(rollout.length > 1000, "rollout");
const framed = new Uint8Array(4 + rollout.length);
new DataView(framed.buffer).setUint32(0, rollout.length, true);
framed.set(rollout, 4);
const sp = e.kern_lehrer_learn(put(framed), framed.length, 1);
check(sp !== 0, "learn");
const stats = JSON.parse(kern.string(sp, e.kern_lehrer_result_length()));
check(stats.iteration === 1 && stats.steps === 16, "stats " + JSON.stringify(stats));
const onnx = take(e.kern_lehrer_export_onnx());
check(onnx.length > 50000 && new TextDecoder().decode(onnx).includes("actions"), "onnx");
// Bit-identical with every other build (tests/determinism_fingerprint.txt).
{
  const p = e.kern_lehrer_fingerprint(400), n = e.kern_lehrer_result_length() / 8;
  const got = new BigUint64Array(kern.memory.buffer, p, n);
  const want = fs.readFileSync(new URL("./determinism_fingerprint.txt", import.meta.url), "utf8").trim().split("\n").map(l => BigInt("0x" + l.split(" ")[1]));
  check(n === want.length, "fingerprint length");
  const first = want.findIndex((w, i) => w !== got[i]);
  check(first < 0, "deterministic: differs from tick " + first);
}
// A new browser scenario must use the SAME C ABI and PPO rollout as built-in tasks.
{
  const manifest = JSON.stringify({
    name: "WASM scenario", duration: 2, commands: { forward: 0.1 },
    waypoints: [{ x: 0.4, y: 0.0, radius: 0.15 }],
    objects: [{ kind: "box", x: 0.5, y: 0.2, width: 0.15, depth: 0.15, height: 0.15 }],
    rewards: { progress: 12, waypoint: 8, finish: 20, upright: 2 }
  });
  const sc = enc.encode("scenario:" + manifest), sp = put(sc);
  const config = enc.encode('{"steps":4}'), cp = put(config);
  check(e.kern_lehrer_create(sp, sc.length, 1, 9, 0) === 1, "scenario collector");
  check(e.kern_lehrer_learner(sp, sc.length, cp, config.length, 9) === 1, "scenario learner");
  const current = take(e.kern_lehrer_policy());
  check(e.kern_lehrer_set_policy(put(current), current.length >> 2) === 1, "scenario policy");
  const steps = take(e.kern_lehrer_collect(4));
  check(steps.length > 0, "scenario rollout");
  const evaluation = e.kern_lehrer_evaluate(2);
  check(evaluation !== 0, "scenario evaluation");
  const metrics = JSON.parse(kern.string(evaluation, e.kern_lehrer_result_length()));
  check(metrics.episodes === 2 && metrics.successes >= 0, "scenario metrics " + JSON.stringify(metrics));
  const model = take(e.kern_lehrer_export_onnx());
  check(model.length > 50000, "scenario ONNX");
}

// Keyframed 3D pose authoring reaches the same WASM collector, PPO and ONNX exporter.
{
  const zero = Array(14).fill(0);
  const nod = [...zero]; nod[6] = .3;
  const spec = JSON.stringify({name:"motion_nod",duration:.6,frames:[
    {time:0,joints:zero},{time:.3,joints:nod},{time:.6,joints:zero}
  ]});
  const task = enc.encode("motion:" + spec), ptr = put(task);
  const cfg = enc.encode('{"steps":4}'), conf = put(cfg);
  check(e.kern_lehrer_create(ptr, task.length, 1, 13, 0) === 1, "motion collector");
  check(e.kern_lehrer_learner(ptr, task.length, conf, cfg.length, 13) === 1, "motion learner / imitation warm start");
  const actor = take(e.kern_lehrer_policy());
  check(e.kern_lehrer_set_policy(put(actor), actor.length >> 2) === 1, "motion policy");
  const experience = take(e.kern_lehrer_collect(4));
  check(experience.length > 0, "motion rollout");
  const r = e.kern_lehrer_evaluate(1);
  check(r !== 0, "motion evaluation");
  const metrics = JSON.parse(kern.string(r, e.kern_lehrer_result_length()));
  check(Number.isFinite(metrics.meanErrorDeg), "real 14-joint pose error");
  check(take(e.kern_lehrer_export_onnx()).length > 50000, "motion ONNX");
}

console.log(`Lehrer wasm ok (bit-identical with native): imitation round of ${ticks} ticks, 16 duck steps in ${ms.toFixed(0)} ms, learn ${stats.learnSeconds.toFixed(2)} s, onnx ${onnx.length} bytes`);

// Recreate all WASM instances as after a hard reload. Match the next
// rollout (excluding wall-clock timing) and complete next learner state.
const binary=fs.readFileSync(file);
async function worker(task,seed,learner=false){
  const k=await loadKern({bytes:binary}), e=k.exports;
  const bytes=x=>typeof x==="string"?enc.encode(x):x;
  const input=(x,fn)=>{
    const b=bytes(x),p=e.kern_alloc(b.byteLength);
    try{k.u8.set(b,p);return fn(p,b.byteLength);}finally{e.kern_free(p);}
  };
  const output=p=>{check(p!==0,"checkpoint ABI output");return k.u8.slice(p,p+e.kern_lehrer_result_length());};
  check(input(task,(p,n)=>learner?input('{"steps":4}',(q,m)=>e.kern_lehrer_learner(p,n,q,m,seed)):e.kern_lehrer_create(p,n,2,seed,0))===1,"checkpoint worker setup");
  return {e,input,output,policy:()=>output(e.kern_lehrer_policy()),
    save:()=>output(learner?e.kern_lehrer_save_learner():e.kern_lehrer_save_collector()),
    restore:state=>input(state,(p,n)=>learner?e.kern_lehrer_load_learner(p,n):e.kern_lehrer_load_collector(p,n)),
    collect:(policy,iteration,steps)=>{
      e.kern_lehrer_progress(iteration);
      check(input(policy,(p,n)=>e.kern_lehrer_set_policy(p,n/4))===1,"checkpoint policy");
      return output(e.kern_lehrer_collect(steps));
    },
    learn:rollout=>{
      const frame=new Uint8Array(4+rollout.length);
      new DataView(frame.buffer).setUint32(0,rollout.length,true);frame.set(rollout,4);
      check(input(frame,(p,n)=>e.kern_lehrer_learn(p,n,1))!==0,"checkpoint learn");
    }};
}
function same(a,b,label){
  check(a.length===b.length && Buffer.from(a).equals(Buffer.from(b)),label);
}
function physicsOnly(rollout){
  const b=rollout.slice();
  b.fill(0,36,44); // Rollout::serialize includes one f64 wall-clock duration.
  return b;
}
const tasks=["stand","jump","motion:"+JSON.stringify({name:"checkpoint_nod",duration:.6,frames:[
  {time:0,joints:Array(14).fill(0)},{time:.6,joints:Array(14).fill(.05)}]}),
  "scenario:"+JSON.stringify({name:"checkpoint_course",duration:2,commands:{forward:.1},
    waypoints:[{x:.4,y:0,radius:.15}],objects:[{kind:"box",x:.5,y:.2,width:.15,depth:.15,height:.15,dynamic:true}],
    rewards:{progress:12,waypoint:8,finish:20,upright:2}})];
for(const task of tasks){
  const kind=task.split(":")[0];
  const a=await worker(task,17), learner=await worker(task,17,true);
  // A checkpoint in the middle of an episode, after curriculum changes.
  a.collect(learner.policy(),200,9);
  const batch=a.collect(learner.policy(),301,4);
  learner.learn(batch);
  const c=a.save(),l=learner.save();
  const b=await worker(task,17), resumed=await worker(task,17,true);
  check(b.restore(c)===1,kind+" restore collector");
  check(resumed.restore(l)===1,kind+" restore learner");
  same(b.save(),c,kind+" collector state roundtrip");
  same(resumed.save(),l,kind+" learner state roundtrip");
  const x=a.collect(learner.policy(),501,4),y=b.collect(resumed.policy(),501,4);
  same(physicsOnly(x),physicsOnly(y),kind+" next rollout exact");
  learner.learn(x);resumed.learn(y);
  same(learner.save(),resumed.save(),kind+" next PPO state exact");
  same(a.save(),b.save(),kind+" next collector state exact");
  console.log("checkpoint exact:",kind);
}
// Preserve the accumulated imitation dataset and round number as well.
{
  const a=await worker("jump",23,true), c=await worker("jump",23);
  function imitate(l,round){
    const policy=l.policy();
    check(c.input(policy,(p,n)=>c.e.kern_lehrer_set_policy(p,n/4))===1,"imitation policy");
    const share=c.output(c.e.kern_lehrer_dagger(round,0,1));
    const framed=new Uint8Array(share.length+4);
    new DataView(framed.buffer).setUint32(0,share.length,true);framed.set(share,4);
    check(l.input(framed,(p,n)=>l.e.kern_lehrer_imitate(p,n,1))!==0,"imitation checkpoint round");
  }
  imitate(a,0);
  const b=await worker("jump",23,true);
  check(b.restore(a.save())===1,"imitation restore");
  check(b.e.kern_lehrer_imitation_left()===7,"imitation round retained");
  imitate(a,1);imitate(b,1);
  same(a.save(),b.save(),"next imitation state exact");
}
console.log("Full local checkpoint continuation checks passed");
