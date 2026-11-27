// Kern Studio's Lehrer workspace: the panel beside the stage, and the training run (docs/lehrer.md).
//
// The stage - the Micro Duck in the viewport, posed by hand, and the keyframes of the motion being
// taught - is the engine's (kern/src/studio/trainer.cpp). This file draws the panel from the JSON
// the engine returns, like every Studio panel, and runs the training: collectors and one learner
// in Web Workers, each on its own instance of the headless Lehrer (generated/kern_lehrer.wasm,
// kern/src/lehrer), so the editor stays responsive while the duck learns. A trained network goes
// into the level: what the duck is taught is saved, played and exported with it (Level::motions).
import {tr, escHtml, toBase64} from "./game.mjs";

/** The Lehrer's module: beside the deployed editor and the single-file build; the module page reads the repository's build. */
const TRAINER_WASM = /\/kern\/web\/studio\//.test(location.pathname) ? "../../../generated/kern_lehrer.wasm" : "./kern_lehrer.wasm";
/** What the Lehrer's old page kept on this device: its learned motions {file: {name, file, duration}} here, their networks in IndexedDB "kern-lehrer". */
const TRAINER_LEGACY = "kern-lehrer.motion-skills.v1";
/** Set once taking those into a level was offered. */
const TRAINER_OFFERED = "kern-studio.lehrer-import.v1";

// Each committed checkpoint contains all workers at one common barrier.
// Store the exact WASM binary once, in the same transaction as the first state.
function localCheckpoint(mode, action) {
  return new Promise((resolve,reject)=>{
    const req=indexedDB.open("kern-lehrer-checkpoint",1);
    req.onupgradeneeded=()=>req.result.createObjectStore("state");
    req.onerror=()=>reject(req.error);
    req.onblocked=()=>reject(Error("Local checkpoint database is blocked"));
    req.onsuccess=()=>{
      const db=req.result;
      let tx, result;
      try {
        tx=db.transaction("state",mode,mode==="readwrite"?{durability:"strict"}:{});
        result=action(tx.objectStore("state"));
      }catch(error){db.close();reject(error);return;}
      tx.oncomplete=()=>{db.close();resolve(result());};
      tx.onabort=()=>{db.close();reject(tx.error||Error("Local checkpoint transaction aborted"));};
      tx.onerror=()=>{}; // abort is authoritative
    };
  });
}
function readLocalCheckpoint() {
  return localCheckpoint("readonly",store=>{
    const current=store.get("current"),wasm=store.get("wasm");
    return ()=>current.result?{record:current.result,wasm:wasm.result}:null;
  });
}
function writeLocalCheckpoint(record,wasm) {
  return localCheckpoint("readwrite",store=>{
    if(wasm)store.put(wasm,"wasm");
    store.put(record,"current");
    return ()=>undefined;
  });
}
function clearLocalCheckpoint() {
  return localCheckpoint("readwrite",store=>{store.clear();return ()=>undefined;});
}
const CHECKPOINT_MAGIC = new TextEncoder().encode("KERNCP1\n");
const CHECKPOINT_MAX = 512*1024*1024;
const hashBytes=async bytes=>new Uint8Array(await crypto.subtle.digest("SHA-256",bytes));
const hex=bytes=>Array.from(bytes,b=>b.toString(16).padStart(2,"0")).join("");
function frameCheckpoint(buffers){
  const total=buffers.reduce((sum,b)=>sum+8+b.byteLength,0);
  if(total>CHECKPOINT_MAX)throw Error("Checkpoint too large");
  const out=new Uint8Array(total),view=new DataView(out.buffer);
  let at=0;
  for(const b of buffers){view.setBigUint64(at,BigInt(b.byteLength),true);at+=8;out.set(new Uint8Array(b),at);at+=b.byteLength;}
  return out;
}
function unframeCheckpoint(payload,count){
  const view=new DataView(payload.buffer,payload.byteOffset,payload.byteLength),out=[];
  let at=0;
  for(let i=0;i<count;i++){
    if(at+8>payload.length)throw Error("Truncated checkpoint");
    const n=view.getBigUint64(at,true);at+=8;
    if(n>BigInt(payload.length-at))throw Error("Invalid checkpoint section");
    out.push(payload.slice(at,at+Number(n)).buffer);at+=Number(n);
  }
  if(at!==payload.length)throw Error("Unexpected checkpoint data");
  return out;
}
async function encodeCheckpoint(meta,payload){
  const header=new TextEncoder().encode(JSON.stringify({...meta,sha256:hex(await hashBytes(payload))}));
  const size=12+header.length+payload.length+32;
  if(size>CHECKPOINT_MAX||header.length>8*1024*1024)throw Error("Checkpoint too large");
  const out=new Uint8Array(size);
  out.set(CHECKPOINT_MAGIC);new DataView(out.buffer).setUint32(8,header.length,true);
  out.set(header,12);out.set(payload,12+header.length);
  out.set(await hashBytes(out.subarray(0,size-32)),size-32);
  return out;
}
async function decodeCheckpoint(bytes){
  if(bytes.length<44||bytes.length>CHECKPOINT_MAX||CHECKPOINT_MAGIC.some((v,i)=>bytes[i]!==v))
    throw Error("Select a complete .kernckpt file, not a standalone ONNX network.");
  const end=bytes.length-32,hash=await hashBytes(bytes.subarray(0,end));
  if(hash.some((v,i)=>v!==bytes[end+i]))throw Error("Checkpoint checksum mismatch");
  const n=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength).getUint32(8,true);
  if(n<1||n>8*1024*1024||12+n>=end)throw Error("Invalid checkpoint header");
  const meta=JSON.parse(new TextDecoder("utf-8",{fatal:true}).decode(bytes.subarray(12,12+n)));
  const payload=bytes.subarray(12+n,end);
  if(!meta||meta.schema!==1||meta.sha256!==hex(await hashBytes(payload)))throw Error("Invalid checkpoint metadata");
  return {meta,payload};
}
function validLocalRecord(r){
  const integer=(n,min,max)=>Number.isInteger(n)&&n>=min&&n<=max;
  return r && r.schema===1 && typeof r.token==="string" && r.token.length<=128 &&
    ["motion","jump","stand","scenario"].includes(r.task) && typeof r.name==="string" && r.name.length<=16384 &&
    (r.name===r.task||r.name.startsWith(r.task+":")) && typeof r.label==="string" && r.label.length<=256 &&
    integer(r.count,1,64)&&integer(r.envs,1,256)&&integer(r.total,1,100000)&&
    integer(r.seed,1,4294967295)&&integer(r.iteration,0,r.total)&&
    r.config && typeof r.config==="object" && Object.keys(r.config).every(k=>
      ["initStd","learningRate"].includes(k)&&Number.isFinite(r.config[k])&&r.config[k]>0&&r.config[k]<=1) &&
    Array.isArray(r.history)&&r.history.length<=r.iteration&&r.history.every(p=>integer(p.it,1,r.iteration)&&Number.isFinite(p.ret)&&Number.isFinite(p.len));
}
function acquireLocalLease() {
  if(!navigator.locks)throw Error("Local resumable training requires browser Web Locks support.");
  return new Promise((resolve,reject)=>{
    navigator.locks.request("kern-lehrer-local-training",{ifAvailable:true},lock=>{
      if(!lock){reject(Error("Local training is open in another tab."));return;}
      return new Promise(release=>resolve(release));
    }).catch(reject);
  });
}


// A Lehrer worker: one Kern module (kern_lehrer.wasm) per worker. A worker is a collector (its own
// ducks, the policy it is handed, returns rollouts) or the learner (PPO).
const TRAINER_WORKER = `
let kern = null;
const enc = new TextEncoder();

/** Copies bytes into the module's memory; the caller frees the pointer. */
function put(bytes) {
  const ptr = kern.exports.kern_alloc(bytes.byteLength);
  kern.u8.set(new Uint8Array(bytes.buffer, bytes.byteOffset, bytes.byteLength), ptr);
  return ptr;
}
function withBytes(bytes, fn) {
  const ptr = put(bytes);
  try { return fn(ptr, bytes.byteLength); } finally { kern.exports.kern_free(ptr); }
}
function withText(text, fn) { return withBytes(enc.encode(text), fn); }
function resultBytes(ptr) {
  if (!ptr) return null;
  return kern.u8.slice(ptr, ptr + kern.exports.kern_lehrer_result_length());
}
function policyBuffer() {
  const ptr = kern.exports.kern_lehrer_policy();
  return kern.u8.slice(ptr, ptr + kern.exports.kern_lehrer_result_length()).buffer;
}
/** Buffers one after the other, each with its byte size in front (u32). */
function framed(buffers) {
  let total = 0;
  for (const b of buffers) total += 4 + b.byteLength;
  const all = new Uint8Array(total), view = new DataView(all.buffer);
  let at = 0;
  for (const b of buffers) {
    view.setUint32(at, b.byteLength, true);
    all.set(new Uint8Array(b), at + 4);
    at += 4 + b.byteLength;
  }
  return all;
}

self.onmessage = async ({ data: m }) => {
  try {
    if (m.type === "load") {
      kern = await __loadKern({ module: m.module });
      self.postMessage({ type: "loaded" });
    } else if (m.type === "collector") {
      const ok = withText(m.task, (p, n) => kern.exports.kern_lehrer_create(p, n, m.envs, m.seed, m.first));
      self.postMessage({ type: "ready", ok: !!ok });
    } else if (m.type === "collect") {
      kern.exports.kern_lehrer_progress(m.iteration);
      const policy = new Float32Array(m.policy);
      const ok = withBytes(policy, (p, n) => kern.exports.kern_lehrer_set_policy(p, n >> 2));
      if (!ok) throw Error("the policy did not load");
      const rollout = resultBytes(kern.exports.kern_lehrer_collect(m.steps));
      if (!rollout) throw Error("collect failed");
      self.postMessage({ type: "rollout", rollout: rollout.buffer }, [rollout.buffer]);
    } else if (m.type === "learner") {
      const ok = withText(m.task, (tp, tn) => withText(JSON.stringify(m.config || {}), (cp, cn) => kern.exports.kern_lehrer_learner(tp, tn, cp, cn, m.seed)));
      if (!ok) throw Error("The Lehrer does not take this task");
      self.postMessage({ type: "policy", policy: policyBuffer(), steps: kern.exports.kern_lehrer_steps(), imitationLeft: kern.exports.kern_lehrer_imitation_left(), imitationEpisodes: kern.exports.kern_lehrer_imitation_episodes() });
    } else if (m.type === "dagger") {
      // A collector's share of an imitation round: its episodes, labelled by the demonstration.
      const policy = new Float32Array(m.policy);
      if (!withBytes(policy, (p, n) => kern.exports.kern_lehrer_set_policy(p, n >> 2))) throw Error("the policy did not load");
      const share = resultBytes(kern.exports.kern_lehrer_dagger(m.round, m.first, m.count));
      if (!share) throw Error("imitation episodes failed");
      self.postMessage({ type: "share", share: share.buffer }, [share.buffer]);
    } else if (m.type === "imitate") {
      // One round of imitation (the demonstration, docs/lehrer.md) from the collectors' shares, in
      // episode order; then the network as it is now.
      const p = withBytes(framed(m.shares), (q, n) => kern.exports.kern_lehrer_imitate(q, n, m.shares.length));
      if (!p) throw Error("imitation failed");
      const report = JSON.parse(kern.string(p, kern.exports.kern_lehrer_result_length()));
      const policy = policyBuffer();
      self.postMessage({ type: "imitated", report, policy, left: kern.exports.kern_lehrer_imitation_left() }, [policy]);
    } else if (m.type === "evaluate") {
      const p = kern.exports.kern_lehrer_evaluate(m.episodes || 5);
      self.postMessage({ type: "evaluated", report: p ? JSON.parse(kern.string(p, kern.exports.kern_lehrer_result_length())) : null });
    } else if (m.type === "learn") {
      const statsPtr = withBytes(framed(m.rollouts), (p, n) => kern.exports.kern_lehrer_learn(p, n, m.rollouts.length));
      if (!statsPtr) throw Error("learn failed");
      const stats = JSON.parse(kern.string(statsPtr, kern.exports.kern_lehrer_result_length()));
      const policy = policyBuffer();
      self.postMessage({ type: "learned", stats, policy }, [policy]);
    } else if (m.type === "save-state") {
      const ptr = m.learner ? kern.exports.kern_lehrer_save_learner() : kern.exports.kern_lehrer_save_collector();
      const state = resultBytes(ptr);
      if(!state)throw Error("Checkpoint capture failed");
      self.postMessage({type:"saved-state",state:state.buffer},[state.buffer]);
    } else if (m.type === "load-state") {
      const ok=withBytes(new Uint8Array(m.state),(p,n)=>m.learner?
        kern.exports.kern_lehrer_load_learner(p,n):kern.exports.kern_lehrer_load_collector(p,n));
      if(!ok)throw Error("Checkpoint could not be restored exactly");
      if(m.learner) {
        const policy=policyBuffer();
        self.postMessage({type:"restored-state",policy,left:kern.exports.kern_lehrer_imitation_left()},[policy]);
      }else self.postMessage({type:"restored-state"});
    } else if (m.type === "export") {
      const onnx = resultBytes(kern.exports.kern_lehrer_export_onnx());
      self.postMessage({ type: "onnx", onnx: onnx.buffer }, [onnx.buffer]);
    }
  } catch (error) {
    self.postMessage({ type: "error", message: String(error?.message || error) });
  }
};
`;

/** One request to a worker: resolves with its `expect` answer, rejects with its error. */
function trainerCall(worker, message, expect, transfer = []) {
  return new Promise((resolve, reject) => {
    worker.onmessage = ({data}) => {
      if (data.type === "error") reject(Error(data.message));
      else if (data.type === expect) resolve(data);
    };
    worker.onerror = e => reject(Error(e.message || "worker failed"));
    worker.postMessage(message, transfer);
  });
}

/** The networks the Lehrer's old page kept on this device (IndexedDB "kern-lehrer", store "policies", key = file name): [[file, Uint8Array], ...]. */
function trainerLegacyNets() {
  return new Promise(resolve => {
    let open;
    try { open = indexedDB.open("kern-lehrer", 1); } catch { resolve([]); return; }
    open.onupgradeneeded = () => open.transaction.abort();  // this device has none: none is made
    open.onerror = e => { e.preventDefault(); resolve([]); };
    open.onsuccess = () => {
      const db = open.result, out = [];
      const done = () => { db.close(); resolve(out); };
      try {
        const cursor = db.transaction("policies").objectStore("policies").openCursor();
        cursor.onsuccess = () => {
          const c = cursor.result;
          if (!c) { done(); return; }
          out.push([String(c.key), new Uint8Array(c.value)]);
          c.continue();
        };
        cursor.onerror = done;
      } catch { done(); }
    };
  });
}

/** The motions the Lehrer's old page taught on this device: [{name, duration, bytes}]. Its jump was the robot's move "jump". */
async function trainerLegacy() {
  let learned = {};
  try { learned = JSON.parse(localStorage.getItem(TRAINER_LEGACY) || "{}") || {}; } catch { /* storage blocked */ }
  const out = [];
  for (const [file, bytes] of await trainerLegacyNets()) {
    const m = learned[file];
    if (m && typeof m.name === "string" && Number.isFinite(m.duration)) out.push({name: m.name, duration: m.duration, bytes});
    else if (file === "jump.onnx") out.push({name: "jump", duration: 2, bytes});
  }
  return out;
}

function trainerDownload(name, data, type) {
  const url = URL.createObjectURL(new Blob([data], {type}));
  const a = document.createElement("a");
  a.href = url;
  a.download = name;
  document.body.append(a);
  a.click();
  a.remove();
  setTimeout(() => URL.revokeObjectURL(url), 30000);
}

const trainerNumber = text => Number(String(text).trim().replace(",", "."));
/** A time as the panel shows it: milliseconds at most, no zeros at its end. */
const trainerTime = t => +(+t).toFixed(3);
/** The first step of a motion nobody has set a pace for yet: a quarter of its length, on a grid of 0.05 s. */
const trainerFirstStep = duration => Math.max(0.05, Math.round(duration / 4 * 20) / 20);
/** Where the next keyframe most likely goes when one was taken at `time`: on by the same step as
 *  from the keyframe before it - after 0.3 and 0.6 comes 0.9 - and never past the motion's end.
 *  `keys` are the motion's keyframes [{time}]. */
function trainerNextTime(keys, duration, time) {
  let before = null;
  for (const k of keys) if (k.time < time - 1e-3 && (before === null || k.time > before)) before = k.time;
  const next = time + (before === null ? trainerFirstStep(duration) : time - before);
  return next > duration - 1e-3 ? duration : trainerTime(next);
}
/** The same for a motion as it stands: one step after its last keyframe before the end. */
function trainerProposedTime(motion) {
  let last = 0;
  for (const k of motion.keys) if (k.time < motion.duration - 1e-3 && k.time > last) last = k.time;
  return trainerNextTime(motion.keys, motion.duration, last);
}

// A scenario is data, not a generated C++ task. Both the collectors and the PPO learner receive
// the exact same canonical JSON; the native trainer accepts the same schema with --scenario.
const TRAINER_SCENARIO = {
  name: "My course", duration: 8,
  commands: {forward: 0.15, lateral: 0, turn: 0},
  start: {yaw: 0, yawSpread: 0},
  waypoints: [{x: 0.4, y: 0, radius: 0.15}, {x: 0.85, y: 0, radius: 0.15}],
  objects: [{kind: "box", x: 0.62, y: 0.25, width: 0.15, depth: 0.15, height: 0.20}],
  rewards: {progress: 12, waypoint: 8, finish: 20, upright: 2, fall: -12, actionRate: -0.1, contact: -2, height: 0, speed: 0, pose: 0},
};

const TRAINER_TASKS = [["motion", "Motion from keyframes"], ["jump", "Jump"], ["stand", "Stand (quick test)"], ["scenario", "Free training scenario"]];
const TRAINER_STATS = [["it", "Iteration"], ["steps", "Duck steps"], ["sps", "Steps / s"], ["ret", "Mean episode return"], ["len", "Episode length (s)"],
                       ["std", "Action noise (std)"], ["goal", ""], ["height", "Jump height (mm)"], ["flight", "Flight time (ms)"]];
const TRAINER_GOAL = {motion: "Pose error (degrees)", jump: "Real jumps (clean robot)", scenario: "Goals reached (evaluation)"};

export class Trainer {
  /**
   * `engine`: the Studio's engine; `icon(name)`: the Studio's icons; `toast(text, actions, ms)`;
   * `kernSource()`: the text of kern.mjs on single-file pages (the workers run it), else null.
   */
  constructor({engine, icon, toast, kernSource}) {
    this.engine = engine;
    this.icon = icon;
    this.toast = toast;
    this.kernSource = kernSource;
    this.host = document.getElementById("trainer");
    this.bar = document.getElementById("trainerBar");
    this.s = null;            // the engine's side while the workspace is open (state().trainer)
    this.motions = [];        // the level's motions (state().motions)
    this.editable = true;     // the level takes changes (it is not being played)
    this.task = "motion";
    this.run = null;          // the training run
    this.onnx = null;         // the newest network of the last run: {bytes, name, task, move}
    this.handover = null;     // a network on its way into the level: {name, onnx (Base64), run, iteration}
    this.legacy = [];         // what the Lehrer's old page taught on this device
    this.module = null;       // the compiled Lehrer
    this.note = "";
    this.statusText = "";
    this.statusError = false;
    this.keysKey = this.learnedKey = "";
    this.timeFor = 0;         // the motion length the bar's time was last set to
    this.history = [];        // the curve of the last run
    this.texts = new WeakMap();
    this.el = null;
    this.remoteProbeTimer = null;
    this.localSaved = null;
    this.localBusy = false;
  }

  /** Sets an element's text when it changed (the page's translation rewrites text nodes: no needless writes). */
  text(node, value) {
    value = String(value);
    if (this.texts.get(node) === value) return;
    this.texts.set(node, value);
    node.textContent = value;
  }

  get running() { return !!this.run; }

  // ---- the panel -------------------------------------------------------------------------
  build() {
    const icon = this.icon;
    const sec = (id, label, body, closed = false) => `<div class="sec${closed ? " closed" : ""}" data-tsec="${id}"><div class="sec-head"><span class="tw">${icon("chevron")}</span><span>${label}</span></div><div class="sec-body">${body}</div></div>`;
    const field = (label, editor, unit = "") => `<div class="f"><label>${label}${unit ? ` <span class="dim">(${unit})</span>` : ""}</label><div class="v">${editor}</div><span></span></div>`;
    const num = (id, value = "", extra = "") => `<input class="num" id="${id}" value="${value}" spellcheck="false" inputmode="decimal" ${extra}>`;
    const cores = Math.max(1, (navigator.hardwareConcurrency || 2) - 1);
    const stats = TRAINER_STATS.map(([id, label]) => `<div class="tr-stat" data-stat="${id}"><b>-</b><span>${label}</span></div>`).join("");
    this.host.innerHTML =
      sec("task", "Task", field("Teach", `<select class="sel" id="trTask">${TRAINER_TASKS.map(([id, label]) => `<option value="${id}">${escHtml(tr(label))}</option>`).join("")}</select>`) + '<p class="tr-note" id="trRecoveredTask" hidden></p>') +
      sec("motion", "Motion from keyframes",
        field("Name", `<input class="txt" id="trName" maxlength="38" spellcheck="false" autocomplete="off">`) +
        field("Length", num("trDuration"), "s") +
        `<p class="tr-note" id="trMotion"></p>` +
        `<div class="grp">Keyframes</div><div id="trKeys"></div>` +
        `<div class="tr-buttons"><button class="btn small" id="trSpread" type="button" title="All keyframes in equal steps over the motion's length">Spread evenly</button>` +
        `<button class="btn small" id="trClear" type="button">Remove all keyframes</button><button class="btn small" id="trNew" type="button">New motion</button></div>` +
        `<p class="tr-note">Drag a part of the duck in the viewport and let go: it holds the pose. Add it as a keyframe at its time. The first keyframe is the home pose at 0 s, the last one lies at the motion's end.</p>` +
        `<p class="tr-note">After each keyframe the time moves on by the same step: after 0.3 and 0.6 it stands at 0.9. Spread evenly sets all keyframes in equal steps over the length.</p>`) +
      sec("scenario", "Scenario",
        `<p class="tr-note">Waypoints and physical obstacles of your own, rewards to taste, and a new network learns the course. The order of the waypoints is the order of the task. All measures in metres from where the duck starts.</p>` +
        field("Name", `<input class="txt" id="scName" maxlength="80" spellcheck="false">`) +
        field("Episode length", num("scDuration"), "s") +
        field("Forward command", num("scForward"), "m/s") +
        `<div class="grp">Rewards</div>` +
        field("Progress", num("scProgress")) + field("Per waypoint", num("scWaypoint")) + field("On finish", num("scFinish")) +
        field("Staying upright", num("scUpright")) + field("Fall penalty", num("scFall")) +
        `<div class="grp">Waypoints: x y radius (one per line)</div><div class="tr-area"><textarea id="scWaypoints" rows="4" spellcheck="false"></textarea></div>` +
        `<div class="grp">Objects: box x y width depth height, or sphere x y radius</div><div class="tr-area"><textarea id="scObjects" rows="4" spellcheck="false"></textarea></div>` +
        `<div class="tr-buttons"><button class="btn small" id="scExample" type="button">Load example</button><button class="btn small" id="scSave" type="button">Save scenario as JSON</button>` +
        `<button class="btn small" id="scImport" type="button">Import scenario JSON</button><input type="file" id="scFile" accept=".json,application/json" hidden></div>` +
        `<div class="grp">More goals, joint poses, randomisation and weights as JSON</div><div class="tr-area"><textarea id="scJson" rows="8" spellcheck="false" aria-label="Scenario configuration as JSON"></textarea></div>` +
        `<div class="tr-buttons"><button class="btn small" id="scApply" type="button">Apply JSON</button></div>` +
        `<p class="tr-note">The direction to the goal is a training command in the free inputs 55/56, not an invented IMU sensor: the real robot needs an external position for it. A scenario's network is not a move of the duck; download it as .onnx.</p>`) +
      sec("run", "Training",
        field("Compute backend", '<select class="sel" id="trCompute"><option value="local" selected>Local</option><option value="mag">MAG · CPU + HIP GPUs</option></select>') +
        `<div id="trRemoteSettings" hidden>` +
        `<details id="trRemoteAddress"><summary>${tr("MAG connection settings")}</summary>` +
        field("MAG URL", '<input class="txt" id="trRemoteUrl" type="url" placeholder="https://mag.…ts.net:8444" spellcheck="false" autocomplete="off">') + `</details>` +
        `<div class="tr-buttons"><button class="btn small" id="trReconnect" type="button">Reconnect to MAG</button></div></div>` +
        `<div class="tr-buttons"><button class="btn go" id="trStart" type="button">Start</button><button class="btn" id="trPause" type="button">Pause</button><button class="btn" id="trStop" type="button">Stop</button></div>` +
        `<div class="tr-buttons" id="trLocalRecovery" hidden><button class="btn small" id="trLocalResume" type="button">Continue saved training</button><button class="btn small" id="trLocalDiscard" type="button">Discard checkpoint</button></div>` +
        `<p class="tr-note" id="trLocalSaved"></p>` +
        `<div class="tr-status" id="trStatus"></div><div class="tr-bar"><i id="trProgress"></i></div>` +
        `<div class="tr-buttons"><button class="btn small" id="trTest" type="button">Try the network on the duck</button><button class="btn small" id="trDownload" type="button">Download .onnx</button></div>` +
        `<div class="tr-buttons"><button class="btn small" id="trCheckpointDownload" type="button">Download checkpoint</button><button class="btn small" id="trCheckpointUpload" type="button">Upload checkpoint</button><input id="trCheckpointFile" type="file" accept=".kernckpt" hidden></div>` +
        `<p class="tr-note">ONNX is for playback; a checkpoint contains the complete training state.</p>` +
        `<div class="tr-status tr-saved" id="trNote"></div>` +
        `<div class="grp">Expert settings</div>` +
        field("Workers", num("trWorkers", Math.min(cores, 8))) + field("Ducks per worker", num("trEnvs", 8)) + field("Iterations", num("trIterations", 1500)) + field("Seed", num("trSeed", 1)) +
        field("Initial action noise", num("trInitStd", "", `placeholder="Recipe"`)) + field("Learning rate", num("trLr", "", `placeholder="Recipe"`))) +
      sec("stats", "Progress", `<div class="tr-stats">${stats}</div><canvas class="tr-chart" id="trChart"></canvas>` +
        `<div class="tr-legend"><span><i style="background:var(--play)"></i>Episode return</span><span><i style="background:var(--accent)"></i>Episode length</span></div>`) +
      sec("learned", "Motions of this level", `<div id="trLearned"></div>` +
        `<div id="trLegacy" hidden><p class="tr-note" id="trLegacyNote"></p><div class="tr-buttons"><button class="btn small" id="trImport" type="button">Import into this level</button></div></div>`) +
      sec("help", "How it works",
        `<p class="tr-note">A motion from keyframes is learned in two steps: first the network imitates the poses you set, played through the servos under gravity; then it practices with PPO and all the variations (masses, centers of mass, servos, sensors) until it holds the motion on its own. The jump imitates a demonstration that a search (CMA-ES) found in the same physics. At run time there is only the network.</p>` +
        `<p class="tr-note">Each worker runs its own ducks in Kern's engine (the same servos, sensors and the same 50 Hz controller as in the viewport) and acts with the current policy. The learner collects their steps and improves the policy with PPO, the recipe Pollen Robotics uses to train the robot's networks. The result is an ONNX file in the format of Pollen's networks; Kern Studio and the robot itself can load it.</p>` +
        `<p class="tr-note">Local training needs the tab open. MAG runs on the server and uses its available CPU threads and HIP GPUs. Local is the default for new training. If a MAG address was saved, the Lehrer checks for an existing job when opened; reloading the page never stops or starts a remote job. CPU determinism is tested; HIP is a separate native build.</p>` +
        `<p class="tr-note">Every 25 iterations the newest network goes into the level: a motion is a move the Blueprint node Robot Move plays by its name, a jump network is the level's own "jump". They are saved with the level, and a packaged or exported game has them.</p>`, true);
    this.bar.innerHTML = `<div class="tr-pose"><span class="dim">Keyframe at</span>${num("trTime")}<span class="dim">s</span>` +
      `<button class="btn primary" id="trCapture" type="button">+ Keyframe</button><button class="btn" id="trHome" type="button">Home pose</button></div>` +
      `<div class="tr-test"><span id="trTesting" data-i18n-skip></span><button class="btn primary" id="trAgain" type="button">Again</button><button class="btn" id="trPosing" type="button">Back to posing</button></div>`;
    const el = this.el = {};
    for (const node of [...this.host.querySelectorAll("[id]"), ...this.bar.querySelectorAll("[id]")]) el[node.id] = node;
    try { el.trRemoteUrl.value = localStorage.getItem("kern-lehrer.remote-url") || ""; } catch { /* storage blocked */ }
    el.trRemoteAddress.open = !el.trRemoteUrl.value;
    el.trCompute.onchange = () => {
      clearTimeout(this.remoteProbeTimer);
      this.remoteProbeTimer = null;
      this.sync();
      if (el.trCompute.value === "mag" && el.trRemoteUrl.value.trim()) void this.reconnectRemote({automatic:true});
    };
    el.trReconnect.onclick = () => void this.reconnectRemote();
    el.trLocalResume.onclick = () => void this.resumeLocal();
    el.trLocalDiscard.onclick = () => void this.discardLocal();
    this.setScenario(TRAINER_SCENARIO);
    el.scName.value = tr(TRAINER_SCENARIO.name);
    this.wire();
  }

  wire() {
    const el = this.el;
    const say = r => { if (r && !r.ok && r.error) this.toast(r.error); return r; };
    const command = c => say(this.engine.command(c));
    // Keys typed into the panel are the panel's (the engine's shortcuts stay out).
    for (const root of [this.host, this.bar]) root.addEventListener("keydown", e => {
      if (!e.target.matches("input, textarea, select")) return;
      e.stopPropagation();
      if (e.key === "Enter" && e.target.matches("input")) e.target.blur();
    });
    this.host.addEventListener("click", e => {
      const head = e.target.closest(".sec-head");
      if (head) { head.parentElement.classList.toggle("closed"); if (!head.parentElement.classList.contains("closed")) this.draw(); return; }
      const key = e.target.closest("[data-key]");
      if (key && e.target.closest("[data-show]")) command({cmd: "trainerShow", index: +key.dataset.key});
      if (key && e.target.closest("[data-drop]")) command({cmd: "trainerKey", index: +key.dataset.key, remove: true});
      const row = e.target.closest("[data-move]");
      if (row && e.target.closest("[data-edit]") && command({cmd: "trainerSelect", name: row.dataset.move})?.ok && !this.run) { this.task = "motion"; this.sync(); }
      if (row && e.target.closest("[data-try]")) command({cmd: "trainerTest", move: row.dataset.move});
      if (row && e.target.closest("[data-level]")) command({cmd: "trainerPlay", move: row.dataset.move});
      if (row && e.target.closest("[data-net]")) this.download(row.dataset.move);
      if (row && e.target.closest("[data-forget]")) command({cmd: "trainerRemove", name: row.dataset.move});
    });
    this.host.addEventListener("change", e => {
      const key = e.target.closest("[data-key]");
      if (!key || !e.target.matches("input")) return;
      const i = +key.dataset.key;
      if (!command({cmd: "trainerKey", index: i, time: trainerNumber(e.target.value)})?.ok) e.target.value = this.s.motion.keys[i].time;
    });
    // The list is not redrawn under the cursor; it catches up when the cursor leaves it.
    el.trKeys.addEventListener("focusout", () => setTimeout(() => this.sync(), 0));
    el.trTask.onchange = () => { this.task = el.trTask.value; this.sync(); };
    el.trName.onchange = () => {
      // A move's name: a-z, 0-9, _ and - (what Blueprints and file names take).
      const name = el.trName.value.normalize("NFKD").replace(/[\u0300-\u036f]/g, "").toLowerCase().replace(/[^a-z0-9_-]+/g, "_").replace(/^_+|_+$/g, "").slice(0, 38);
      el.trName.value = name;
      if (!name || !command({cmd: "trainerMotion", name})?.ok) el.trName.value = this.s.motion.name;
    };
    el.trDuration.onchange = () => {
      if (!command({cmd: "trainerMotion", duration: trainerNumber(el.trDuration.value)})?.ok) el.trDuration.value = this.s.motion.duration;
    };
    el.trSpread.onclick = () => command({cmd: "trainerSpread"});
    el.trClear.onclick = () => command({cmd: "trainerClear"});
    el.trNew.onclick = () => command({cmd: "trainerNew"});
    el.trImport.onclick = () => this.importLegacy();
    el.trCapture.onclick = () => {
      const motion = this.s?.motion, time = trainerNumber(el.trTime.value);
      if (!motion) return;
      const keys = motion.keys.slice(), duration = motion.duration;
      // The next pose most likely follows at the same step: the time moves on to it.
      if (command({cmd: "trainerCapture", time})?.ok) el.trTime.value = trainerNextTime(keys, duration, time);
    };
    el.trHome.onclick = () => command({cmd: "trainerStage"});
    el.trPosing.onclick = () => command({cmd: "trainerStage"});
    el.trAgain.onclick = () => { if (this.s?.testing === "lehrer_preview") this.previewNetwork(); else if (this.s?.testing) command({cmd: "trainerTest", move: this.s.testing}); };
    el.trStart.onclick = () => this.start();
    el.trPause.onclick = () => this.pause();
    el.trStop.onclick = () => this.stop();
    el.trTest.onclick = () => this.previewNetwork();
    el.trDownload.onclick = () => { if (this.onnx) trainerDownload(`${this.onnx.name}_kern.onnx`, this.onnx.bytes, "application/octet-stream"); };
    el.trCheckpointDownload.onclick=()=>void this.downloadCheckpoint();
    el.trCheckpointUpload.onclick=()=>el.trCheckpointFile.click();
    el.trCheckpointFile.onchange=()=>{
      const file=el.trCheckpointFile.files?.[0];el.trCheckpointFile.value="";
      if(file)void this.uploadCheckpoint(file);
    };
    el.scExample.onclick = () => { this.setScenario(TRAINER_SCENARIO); el.scName.value = tr(TRAINER_SCENARIO.name); };
    el.scApply.onclick = () => {
      try { this.setScenario(JSON.parse(el.scJson.value)); this.status("Scenario loaded."); }
      catch (error) { this.status(error.message, true); }
    };
    el.scSave.onclick = () => {
      try {
        el.scJson.value = JSON.stringify(this.scenarioConfig(), null, 2);
        trainerDownload("kern-scenario.json", el.scJson.value, "application/json");
      } catch (error) { this.status(error.message, true); }
    };
    el.scImport.onclick = () => el.scFile.click();
    el.scFile.onchange = async () => {
      const file = el.scFile.files?.[0];
      el.scFile.value = "";
      if (!file) return;
      try {
        if (file.size > 16384) throw Error("The file is too large (16 KiB at most)");
        this.setScenario(JSON.parse(await file.text()));
        this.status("Scenario imported.");
      } catch (error) { this.status(error.message, true); }
    };
    addEventListener("resize", () => this.draw());
    // Android may suspend polling in the background. On return, discover
    // the server's current job again if no run is attached.
    addEventListener("online", () => this.discoverRemote());
    document.addEventListener("visibilitychange", () => {
      if (!document.hidden) this.discoverRemote();
    });
  }

  /** The engine's state changed (every Studio panel is drawn from it). */
  render(s) {
    const open = !!s.trainer, wasOpen = !!this.s;
    this.s = s.trainer || null;
    this.motions = s.motions || [];
    this.editable = open || s.play.state === "editing";
    // A network that waited for the level to stop playing goes in now.
    if (this.handover && this.editable) {
      const why = this.deliver();
      if (why) {
        if (this.run?.remote) this.note = "Network saved on MAG; level import failed: " + why;
        else { if (this.run) this.end(this.run); this.status(why, true); }
      }
    }
    this.covered = !!s.logic?.open;  // the Blueprint editor lies over the viewports, the stage with them
    document.getElementById("main").classList.toggle("trainer-open", open);
    document.getElementById("trainerPanel").hidden = !open;
    const button = document.getElementById("trainerBtn");
    button.classList.toggle("on", open);
    if (open && !this.el) { this.build(); this.statusText = `Ready. ${navigator.hardwareConcurrency || "?"} cores on this device.`; }
    if (this.el) this.sync();
    if (open && !wasOpen) {
      void this.refreshLocal();
      this.discoverRemote();
    }
  }

  /** Brings the panel and the bar in step with the engine's state and the run. */
  sync() {
    const el = this.el, s = this.s, run = this.run;
    if (!el) return;
    const open = !!s, task = run?.task ?? this.task, motion = s?.motion;
    el.trTask.value = task;
    el.trTask.disabled = !!run;
    this.host.querySelector('[data-tsec="motion"]').hidden = task !== "motion" || !!run?.recovered;
    if(el.trRecoveredTask) {
      el.trRecoveredTask.hidden=!run?.recovered;
      this.text(el.trRecoveredTask,run?.recovered ? "Recovered training: "+(run.label||run.task) : "");
    }
    this.host.querySelector('[data-tsec="scenario"]').hidden = task !== "scenario" || !!run?.recovered;
    if (motion) {
      if (document.activeElement !== el.trName) el.trName.value = motion.name;
      if (document.activeElement !== el.trDuration) el.trDuration.value = motion.duration;
      el.trName.disabled = run?.move === motion.name;  // the run's network is this motion's, by its name
      this.text(el.trMotion, !motion.saved ? "New: it is the level's with its first keyframe." : motion.trained ? "In the level, taught." : "In the level, not taught yet.");
      // Another motion, or another length: the time for its next keyframe, one step after its last.
      const timeFor = motion.name + "\n" + motion.duration;
      if (timeFor !== this.timeFor && document.activeElement !== el.trTime) { this.timeFor = timeFor; el.trTime.value = trainerProposedTime(motion); }
      const key = JSON.stringify(motion.keys.map(k => k.time));
      if (key !== this.keysKey && !el.trKeys.contains(document.activeElement)) {
        this.keysKey = key;
        el.trKeys.innerHTML = motion.keys.map((k, i) => `<div class="tr-key" data-key="${i}"><input class="num" value="${k.time}" inputmode="decimal" spellcheck="false" aria-label="Time of the keyframe (s)">` +
          `<span class="dim">s</span><button class="btn small" data-show type="button" title="The servos take the duck to this pose">Show</button>` +
          `<button class="ph" data-drop type="button" title="Remove this keyframe">${this.icon("trash")}</button></div>`).join("");
      }
    }
    // The run.
    const remoteSelected = el.trCompute.value === "mag";
    el.trRemoteSettings.hidden = !remoteSelected;
    el.trCompute.disabled = !!run || !!this.checkpointBusy;
    el.trRemoteUrl.disabled = !!run || !remoteSelected;
    el.trReconnect.disabled = !!run || !remoteSelected;
    el.trWorkers.disabled = !!run || remoteSelected;
    for(const id of ["trEnvs","trIterations","trSeed","trInitStd","trLr"])if(el[id])el[id].disabled=!!run;
    el.trStart.disabled = !!run || !!this.localBusy || !!this.checkpointBusy || (!remoteSelected && !!this.localSaved);
    if(el.trLocalRecovery){
      el.trLocalRecovery.hidden=remoteSelected || !this.localSaved || !!run;
      el.trLocalResume.disabled=!!run || !!this.localBusy || !!this.checkpointBusy;
      el.trLocalDiscard.disabled=!!run || !!this.localBusy || !!this.checkpointBusy;
      this.text(el.trLocalResume,this.localSaved?.iteration===this.localSaved?.total?"Open saved result":"Continue saved training");
      this.text(el.trLocalSaved,!remoteSelected && this.localSaved ? "Saved checkpoint: iteration "+this.localSaved.iteration+" / "+this.localSaved.total : "");
    }
    el.trPause.disabled = !run || !!run.localStopping || !!(run.remote && (!run.remote.id || run.remote.busy || run.remote.stopping || !["running", "paused"].includes(run.remote.state)));
    el.trStop.disabled = !run || !!run?.localStopping || !!run?.remote?.stopping || !!(run?.remote?.busy && !run.remote.id);
    this.text(el.trPause, run?.paused ? "Resume" : "Pause");
    el.trDownload.disabled = !this.onnx;
    if(el.trCheckpointDownload){
      el.trCheckpointDownload.disabled=!!this.checkpointBusy || (remoteSelected?!(run?.remote?.id||this.lastRemoteResult):!this.localSaved);
      el.trCheckpointUpload.disabled=!!run||!!this.localBusy||!!this.checkpointBusy;
    }
    el.trTest.disabled = !(this.onnx?.preview || this.onnx?.move) || !open;
    this.text(el.trStatus, this.statusText);
    el.trStatus.classList.toggle("err", this.statusError);
    this.text(el.trNote, this.note);
    el.trProgress.style.width = run ? `${Math.min(100, 100 * run.iteration / run.total).toFixed(1)}%` : this.lastProgress != null ? this.lastProgress + "%" : "0";
    const shown = run?.task ?? this.onnx?.task ?? task;
    for (const tile of this.host.querySelectorAll("[data-stat]")) {
      const id = tile.dataset.stat;
      if (id === "goal") { tile.hidden = !TRAINER_GOAL[shown]; this.text(tile.lastElementChild, TRAINER_GOAL[shown] || ""); }
      if (id === "height" || id === "flight") tile.hidden = shown !== "jump";
    }
    // What the level's ducks were taught, and what the Lehrer's old page left on this device.
    const motions = this.motions, current = motion?.saved ? motion.name : "";
    const lkey = JSON.stringify([motions, current, run?.move ?? ""]);
    if (lkey !== this.learnedKey) {
      this.learnedKey = lkey;
      const off = on => on ? "" : " disabled";
      el.trLearned.innerHTML = motions.length ? motions.map(m => `<div class="tr-learned${m.name === current ? " on" : ""}" data-move="${escHtml(m.name)}">` +
        `<span class="nm" data-i18n-skip title="${escHtml(m.name)}">${escHtml(m.name)}</span><span class="dim">${m.duration} s</span><span class="dim">${m.bytes ? "" : "Not taught yet"}</span>` +
        `<span class="tr-acts"><button class="btn small" data-edit type="button" title="Its keyframes on the stage"${off(m.keys > 0)}>Edit</button>` +
        `<button class="btn small" data-try type="button" title="The duck tries the move on the stage"${off(m.bytes > 0)}>Try</button>` +
        `<button class="btn small" data-level type="button" title="The level's Micro Duck plays the move"${off(m.bytes > 0)}>In the level</button>` +
        `<button class="ph" data-net type="button" title="Download the network (.onnx)"${off(m.bytes > 0)}>${this.icon("save")}</button>` +
        `<button class="ph" data-forget type="button" title="Remove from the level"${off(run?.move !== m.name)}>${this.icon("trash")}</button></span></div>`).join("")
        : `<p class="tr-note">This level's ducks were not taught anything yet. A motion you teach appears here; it is saved with the level, and the Blueprint node Robot Move plays it by its name.</p>`;
    }
    const left = this.legacy.filter(m => !motions.some(x => x.name === m.name));
    el.trLegacy.hidden = !left.length;
    if (left.length) this.text(el.trLegacyNote, `From the Lehrer's old page, on this device: ${left.map(m => m.name).join(", ")}`);
    // The bar on the stage.
    const posing = open && s.stage === "pose" && task === "motion", testing = open && s.stage === "test";
    this.bar.hidden = this.covered || (!posing && !testing);
    this.bar.classList.toggle("test", testing);
    if (testing) this.text(el.trTesting, s.testing || "");
    const badge = document.getElementById("trainerRun");
    badge.hidden = !run;
    if (run) this.text(badge, `${Math.floor(100 * run.iteration / run.total)}%`);
  }

  status(text, error = false) {
    this.statusText = text;
    this.statusError = error;
    if (this.el) this.sync();
  }

  /** A motion's network as a file. */
  download(name) {
    const r = this.engine.command({cmd: "trainerNet", name});
    if (!r.ok) { this.toast(r.error); return; }
    trainerDownload(`${name}_kern.onnx`, Uint8Array.from(atob(r.onnx), c => c.charCodeAt(0)), "application/octet-stream");
  }

  // ---- what the Lehrer's old page left on this device ---------------------------------------
  /** Looks for motions the old page taught here and offers, once, to take them into the open level. */
  async offerLegacy() {
    this.legacy = await trainerLegacy();
    if (!this.legacy.length) return;
    this.sync();
    try {
      if (localStorage.getItem(TRAINER_OFFERED)) return;
    } catch { return; }  // storage blocked: the offer would come back every time
    const offer = () => {
      if (!document.getElementById("toast").hidden) { setTimeout(offer, 2000); return; }  // another message first
      try { localStorage.setItem(TRAINER_OFFERED, "1"); } catch { /* storage blocked */ }
      this.toast(`The duck learned on the Lehrer's old page on this device: ${this.legacy.map(m => m.name).join(", ")}. Import into this level?`, [
        {label: "Import", primary: true, run: () => this.importLegacy()},
        {label: "Not now", run: () => {}},
      ], 30000);
    };
    offer();
  }

  /** Takes them into the open level: saved with it from then on. */
  importLegacy() {
    const r = this.engine.command({cmd: "trainerImport", motions: this.legacy.map(m => ({name: m.name, duration: m.duration, onnx: toBase64(m.bytes)}))});
    this.toast(r.ok ? `Imported into the level: ${r.names.join(", ")}` : r.error);
  }

  // ---- the free scenario's form ------------------------------------------------------------
  setScenario(cfg) {
    if (!cfg || typeof cfg !== "object" || Array.isArray(cfg)) throw Error("A scenario is a JSON object");
    const el = this.el;
    el.scName.value = cfg.name ?? "Scenario";
    el.scDuration.value = cfg.duration ?? 6;
    el.scForward.value = cfg.commands?.forward ?? 0;
    for (const [id, key, fallback] of [["scProgress", "progress", 12], ["scWaypoint", "waypoint", 8], ["scFinish", "finish", 20], ["scUpright", "upright", 2], ["scFall", "fall", -12]])
      el[id].value = cfg.rewards?.[key] ?? fallback;
    el.scWaypoints.value = (cfg.waypoints || []).map(p => [p.x, p.y, p.radius ?? 0.15].join(" ")).join("\n");
    el.scObjects.value = (cfg.objects || []).map(o => o.kind === "sphere" ? ["sphere", o.x, o.y, o.radius ?? 0.08].join(" ")
      : ["box", o.x, o.y, o.width ?? 0.15, o.depth ?? 0.15, o.height ?? 0.15].join(" ")).join("\n");
    el.scJson.value = JSON.stringify(cfg, null, 2);
  }

  scenarioRows(id, waypoint) {
    const out = [], lines = this.el[id].value.split(/\r?\n/);
    for (let i = 0; i < lines.length; ++i) {
      const line = lines[i].replace(/#.*/, "").trim();
      if (!line) continue;
      const bits = line.split(/[\s;]+/);
      if (waypoint) {
        if (bits.length !== 3 || bits.some(v => !Number.isFinite(Number(v)))) throw Error(`Waypoint line ${i + 1}: x y radius expected`);
        const [x, y, radius] = bits.map(Number);
        if (Math.abs(x) > 5 || Math.abs(y) > 5 || radius < 0.03 || radius > 1) throw Error(`Waypoint line ${i + 1} is outside the allowed range`);
        out.push({x, y, radius});
      } else {
        const kind = bits[0];
        if ((kind !== "box" && kind !== "sphere") || bits.length !== (kind === "box" ? 6 : 4) || bits.slice(1).some(v => !Number.isFinite(Number(v))))
          throw Error(`Object line ${i + 1}: box x y width depth height, or sphere x y radius`);
        const n = bits.slice(1).map(Number);
        const o = kind === "box" ? {kind, x: n[0], y: n[1], width: n[2], depth: n[3], height: n[4]} : {kind, x: n[0], y: n[1], radius: n[2]};
        if (Math.abs(o.x) > 5 || Math.abs(o.y) > 5 || n.slice(2).some(v => v < 0.01 || v > 2)) throw Error(`Object line ${i + 1} is outside the allowed range`);
        out.push(o);
      }
    }
    if (out.length > (waypoint ? 16 : 32)) throw Error(waypoint ? "Too many waypoints (16 at most)" : "Too many objects (32 at most)");
    return out;
  }

  scenarioConfig() {
    // The JSON is the source for what has no field of its own (the 14 joint angles, actuation and
    // speed terms, randomisation, dynamic objects). The fields overwrite only their own keys.
    const el = this.el;
    const field = (id, label, lo, hi) => {
      const n = trainerNumber(el[id].value);
      if (!el[id].value.trim() || !Number.isFinite(n) || n < lo || n > hi) throw Error(`${label}: a number from ${lo} to ${hi}`);
      return n;
    };
    const cfg = JSON.parse(el.scJson.value);
    if (!cfg || typeof cfg !== "object" || Array.isArray(cfg)) throw Error("A scenario is a JSON object");
    cfg.name = el.scName.value.trim().slice(0, 80) || "Scenario";
    cfg.duration = field("scDuration", "Episode length", 0.5, 60);
    cfg.commands = {...(cfg.commands || {}), forward: field("scForward", "Forward command", -0.4, 0.4)};
    cfg.waypoints = this.scenarioRows("scWaypoints", true).map((p, i) => ({...(cfg.waypoints?.[i] || {}), ...p}));
    cfg.objects = this.scenarioRows("scObjects", false).map((o, i) => ({...(cfg.objects?.[i] || {}), ...o}));
    cfg.rewards = {...(cfg.rewards || {}),
      progress: field("scProgress", "Progress", -100, 100), waypoint: field("scWaypoint", "Per waypoint", -100, 100), finish: field("scFinish", "On finish", -100, 100),
      upright: field("scUpright", "Staying upright", -100, 100), fall: field("scFall", "Fall penalty", -100, 100)};
    if (cfg.waypoints.length === 0 && !cfg.rewards.pose && !cfg.rewards.height && !cfg.rewards.speed && !cfg.rewards.upright)
      throw Error("A scenario needs a waypoint or a posture or speed goal");
    if (JSON.stringify(cfg).length > 16384) throw Error("The scenario is too large (16 KiB at most)");
    return cfg;
  }

  // ---- the training run ----------------------------------------------------------------------
  /** A worker with the Lehrer loaded: its code as a Blob, kern.mjs from its text or by URL (as kern/web/jobs.mjs). */
  async spawn(run) {
    if (!run.workerUrl) {
      const source = this.kernSource();
      const head = source ? source.replace(/^export\s+/gm, "") + "\nconst __loadKern = loadKern;\n"
        : `import {loadKern as __loadKern} from ${JSON.stringify(new URL("../kern.mjs", import.meta.url).href)};\n`;
      run.workerUrl = URL.createObjectURL(new Blob([head + TRAINER_WORKER], {type: "text/javascript"}));
    }
    const w = new Worker(run.workerUrl, {type: "module"});
    run.workers.push(w);
    await trainerCall(w, {type: "load", module: run.module || this.module}, "loaded");
    return w;
  }

  async start() {
    if (this.run || this.localBusy || this.checkpointBusy) return;
    const el = this.el, task = this.task;
    // `move`: the level's motion the run's network belongs to (Kern's jump: the level's own "jump").
    let name = task, move = task === "jump" ? "jump" : null, label = task;
    try {
      if (task === "motion") {
        const r = this.engine.command({cmd: "trainerTask"});
        if (!r.ok) throw Error(r.error);
        name = r.task;
        move = label = r.name;
      } else if (task === "scenario") {
        const scenario = this.scenarioConfig();
        name = "scenario:" + JSON.stringify(scenario);
        label = "scenario_" + scenario.name.replace(/[^a-z0-9_-]/gi, "_").slice(0, 48);
      }
    } catch (error) { this.status(error.message, true); return; }
    const whole = (id, lo, hi, fallback) => Math.max(lo, Math.min(hi, Math.round(trainerNumber(el[id].value)) || fallback));
    const run = this.run = {task, name, move, label, token: `run-${Date.now().toString(36)}`, workers: [], collectors: [], learner: null, workerUrl: null, wake: null, policy: null, steps: 24,
      count: whole("trWorkers", 1, 64, 1), envs: whole("trEnvs", 1, 256, 8), total: whole("trIterations", 1, 100000, 1), seed: whole("trSeed", 1, 4294967295, 1),
      iteration: 0, paused: false, history: []};
    if (el.trCompute.value === "mag") {
      try {
        const connection = this.remoteConnection();
        const build = window.KERN_TRAINER_BUILD;
        if (!/^[a-f0-9]{40}$/.test(build || "")) throw Error("Remote training needs a versioned Studio build.");
        const body = {id: crypto.randomUUID(), build, task: name, iterations: run.total, envs: run.envs, seed: run.seed};
        for (const [field,id] of [["initStd","trInitStd"],["learningRate","trLr"]]) {
          const value = trainerNumber(el[id].value);
          if (value > 0) body[field] = value;
        }
        run.remote = {...connection, body, id: null, state: "waiting", stopping: false, busy: false, network: null};
        // Save before sending POST: a reload may lose the response, not the job.
        this.rememberRemote(run.remote, body.id);
      } catch (error) {
        this.end(run); this.status(error.message, true); return;
      }
    }
    this.onnx = null;
    this.handover = null;
    this.note = "";
    this.lastProgress=null;
    this.history = run.history;
    for (const tile of this.host.querySelectorAll("[data-stat] b")) this.text(tile, "-");
    this.draw();
    const live = () => this.run === run;
    try {
      if (run.remote) { await this.remotePractice(run); return; }
      const config = {};
      if (trainerNumber(el.trInitStd.value) > 0) config.initStd = trainerNumber(el.trInitStd.value);
      if (trainerNumber(el.trLr.value) > 0) config.learningRate = trainerNumber(el.trLr.value);
      run.config=config;
      await this.runLocal(run);
    } catch (error) {
      if (!live()) return;
      this.end(run);
      this.status(error.message, true);
    }
  }


  async downloadCheckpoint(){
    if(this.checkpointBusy)return;
    this.checkpointBusy=true;this.sync();
    try{
      let bytes,name="training";
      if(this.el.trCompute.value==="mag"){
        const remote=this.run?.remote;
        const id=remote?.id||this.lastRemoteResult;
        if(!id)throw Error("No MAG checkpoint available");
        const connection=remote||this.remoteConnection();
        bytes=await this.remoteRequest({remote:connection},"/v1/jobs/"+id+"/checkpoint",undefined,true,120000);
        await decodeCheckpoint(bytes);
        name="mag-"+id.slice(0,8);
      }else{
        const saved=await readLocalCheckpoint();
        if(!saved)throw Error("No local checkpoint available");
        const {learner,collectors,onnx,...record}=saved.record;
        if(!validLocalRecord(record)||!(onnx instanceof ArrayBuffer))throw Error("Checkpoint predates portable export");
        const payload=frameCheckpoint([learner,...collectors,onnx]);
        bytes=await encodeCheckpoint({schema:1,backend:"local",record,
          wasmSha256:hex(await hashBytes(saved.wasm))},payload);
        name=record.label;
      }
      trainerDownload(name.replace(/[^a-zA-Z0-9_-]/g,"_")+".kernckpt",bytes,"application/octet-stream");
      this.note="Complete checkpoint downloaded.";
    }catch(error){this.status(error.message,true);}
    finally{this.checkpointBusy=false;this.sync();}
  }

  async uploadCheckpoint(file){
    if(this.run||this.localBusy||this.checkpointBusy)return;
    this.checkpointBusy=true;this.sync();
    let release,localReady=false,remoteReconnect=false;
    try{
      if(file.size>CHECKPOINT_MAX)throw Error("Checkpoint too large");
      const bytes=new Uint8Array(await file.arrayBuffer());
      const {meta,payload}=await decodeCheckpoint(bytes);
      if(meta.backend==="local"){
        const record=meta.record;
        if(!validLocalRecord(record))throw Error("Invalid local checkpoint metadata");
        const buffers=unframeCheckpoint(payload,record.count+2);
        if(buffers.some(b=>!b.byteLength))throw Error("Empty checkpoint section");
        release=await acquireLocalLease();
        const previous=await readLocalCheckpoint();
        if(previous)throw Error("A local checkpoint exists. Download or discard it before importing another.");
        // Never execute WASM supplied by an uploaded file. Only resume with
        // the trusted current binary if it matches the original hash.
        const response=await fetch(TRAINER_WASM);
        if(!response.ok)throw Error("Lehrer WASM could not be loaded");
        const wasm=await response.arrayBuffer();
        if(hex(await hashBytes(wasm))!==meta.wasmSha256)throw Error("Checkpoint requires the original Lehrer WASM build.");
        await writeLocalCheckpoint({...record,learner:buffers[0],collectors:buffers.slice(1,-1),onnx:buffers.at(-1)},wasm);
        this.localSaved={token:record.token,iteration:record.iteration,total:record.total};
        this.el.trCompute.value="local";
        localReady=true;
      }else if(meta.backend==="mag"){
        if(meta.build!==window.KERN_TRAINER_BUILD)throw Error("Checkpoint requires the original Studio/MAG build.");
        const connection=this.remoteConnection(),digest=hex(await hashBytes(bytes));
        const pendingKey="kern-lehrer.pending-upload";
        let pending=null;
        try{pending=JSON.parse(localStorage.getItem(pendingKey)||"null");}catch{}
        const id=pending?.url===connection.url&&pending.digest===digest&&/^[a-f0-9-]{36}$/.test(pending.id)?pending.id:crypto.randomUUID();
        const previous=this.remoteRecord(connection.url);
        this.el.trCompute.value="mag";
        const remote={...connection,id,config:meta.config,stopping:false};
        // A lost response must not create another training job on retry.
        try{localStorage.setItem(pendingKey,JSON.stringify({url:connection.url,digest,id}));}catch{}
        this.rememberRemote(remote);
        try{
          await this.remoteRequest({remote},"/v1/checkpoints/"+id,bytes,false,120000);
          remoteReconnect=true;
          try{localStorage.removeItem(pendingKey);}catch{}
        }catch(error){
          if(error.http && error.http<500){
            try{
              localStorage.removeItem(pendingKey);
              if(previous)localStorage.setItem("kern-lehrer.remote-job",JSON.stringify(previous));
              else this.forgetRemote(connection.url,id);
            }catch{}
          }else remoteReconnect=true;
          throw error;
        }
      }else throw Error("Unsupported checkpoint backend");
    }catch(error){
      this.status(error.message,true);
    }finally{release?.();this.checkpointBusy=false;this.sync();}
    if(localReady)await this.resumeLocal();
    else if(remoteReconnect)await this.reconnectRemote();
  }

  async refreshLocal() {
    try {
      const saved=await readLocalCheckpoint();
      if(!this.run?.localLease) {
        const r=saved?.record;
        this.localSaved=r?{token:r.token,iteration:r.iteration,total:r.total}:null;
      }
    } catch(error) {
      if(!this.run && this.el?.trCompute.value==="local")
        this.status("Local checkpoint storage unavailable: "+error.message,true);
    }
    this.sync();
  }

  async discardLocal() {
    if(this.run||this.localBusy||this.checkpointBusy||!this.localSaved)return;
    if(!confirm(tr("Discard the saved local training? This cannot be undone.")))return;
    this.localBusy=true;this.sync();
    let release;
    try{
      release=await acquireLocalLease();
      const saved=await readLocalCheckpoint();
      if(saved?.record.token!==this.localSaved.token)throw Error("Saved training changed in another tab.");
      await clearLocalCheckpoint();
      this.localSaved=null;
      this.status("Local checkpoint discarded.");
    }catch(error){this.status(error.message,true);}
    finally{release?.();this.localBusy=false;this.sync();}
  }

  async resumeLocal() {
    if(this.run||this.localBusy||this.checkpointBusy)return;
    this.localBusy=true;this.sync();
    let run,release;
    try{
      release=await acquireLocalLease();
      const saved=await readLocalCheckpoint(),r=saved?.record;
      if(!r||r.schema!==1||!(saved.wasm instanceof ArrayBuffer)||
         !Number.isInteger(r.count)||r.count<1||r.count>64||
         !Number.isInteger(r.envs)||r.envs<1||r.envs>256||
         !Number.isInteger(r.iteration)||r.iteration<0||r.iteration>r.total||
         !Array.isArray(r.collectors)||r.collectors.length!==r.count||
         !(r.learner instanceof ArrayBuffer)||
         r.collectors.some(b=>!(b instanceof ArrayBuffer))||
         !["motion","jump","stand","scenario"].includes(r.task))
        throw Error("Local checkpoint is missing or incompatible; it has not been deleted.");
      this.el.trCompute.value="local";
      const {learner:ignoredLearner,collectors:ignoredCollectors,...metadata}=r;
      run=this.run={...metadata,move:null,workers:[],collectors:[],learner:null,workerUrl:null,
        wake:null,policy:null,paused:false,recovered:true,localLease:release};
      release=null;
      this.history=run.history;
      this.restoreRunFields(run);
      this.onnx=null;this.handover=null;
      await this.runLocal(run,saved);
    }catch(error){
      if(run&&this.run===run)this.end(run);
      this.status(error.message,true);
    }finally{release?.();this.localBusy=false;this.sync();}
  }

  async saveLocal(run,initialWasm=null) {
    if(this.run!==run)throw Error("Local run no longer active");
    const states=await Promise.all([
      trainerCall(run.learner,{type:"save-state",learner:true},"saved-state"),
      ...run.collectors.map(w=>trainerCall(w,{type:"save-state",learner:false},"saved-state"))
    ]);
    const snapshot=await trainerCall(run.learner,{type:"export"},"onnx");
    if(this.run!==run)throw Error("Local run no longer active");
    const record={schema:1,token:run.token,task:run.task,name:run.name,label:run.label,
      count:run.count,envs:run.envs,total:run.total,seed:run.seed,config:run.config,
      iteration:run.iteration,history:run.history,stats:run.lastStats||null,report:run.lastReport||null,
      learner:states[0].state,collectors:states.slice(1).map(s=>s.state),onnx:snapshot.onnx};
    await writeLocalCheckpoint(record,initialWasm);
    this.localSaved={token:record.token,iteration:record.iteration,total:record.total};
    this.sync();
  }

  async runLocal(run,saved=null) {
    const live=()=>this.run===run;
    if(!run.localLease)run.localLease=await acquireLocalLease();
    if(!live()){run.localLease?.();return;}
    if(!saved) {
      const existing=await readLocalCheckpoint();
      if(existing){
        const r=existing.record;
        this.localSaved={token:r.token,iteration:r.iteration,total:r.total};
        throw Error("A local checkpoint exists. Continue it or explicitly discard it.");
      }
    }
    this.status(saved?"Restoring saved local training ...":"Loading the Lehrer ...");
    let wasm=saved?.wasm;
    if(!wasm){
      const response=await fetch(TRAINER_WASM);
      if(!response.ok)throw Error("Lehrer WASM could not be loaded");
      wasm=await response.arrayBuffer();
    }
    run.module=await WebAssembly.compile(wasm);
    if(!live())return;
    run.wake=await navigator.wakeLock?.request("screen").catch(()=>null);
    run.learner=await this.spawn(run);
    run.collectors=await Promise.all(Array.from({length:run.count},()=>this.spawn(run)));
    await Promise.all(run.collectors.map((w,i)=>trainerCall(w,{type:"collector",task:run.name,
      envs:run.envs,seed:run.seed,first:i*run.envs},"ready").then(d=>{
      if(!d.ok)throw Error("The Lehrer does not take this task");
    })));
    const made=await trainerCall(run.learner,{type:"learner",task:run.name,seed:run.seed,config:run.config},"policy");
    run.steps=made.steps||24;run.policy=made.policy;
    let left=made.imitationLeft||0;
    if(saved){
      const r=saved.record;
      const [learner]=await Promise.all([
        trainerCall(run.learner,{type:"load-state",learner:true,state:r.learner},"restored-state"),
        ...run.collectors.map((w,i)=>trainerCall(w,{type:"load-state",learner:false,state:r.collectors[i]},"restored-state"))
      ]);
      run.policy=learner.policy;left=learner.left;
      run.lastStats=r.stats;run.lastReport=r.report;
      if(r.stats)this.stats(run,r.stats,r.stats.collectSeconds+r.stats.learnSeconds,false);
      if(r.report)this.goal(run,r.report);
      // The exact recovered policy is previewable immediately, including
      // when the next imitation round has not yet finished.
      await this.export(run);
    }else{
      // Do not start any training step until the initial checkpoint is durable.
      await this.saveLocal(run,wasm);
    }
    this.localBusy=false;this.sync();
    while(live() && left>0 && !run.localStopping){
      if(run.paused){await new Promise(r=>setTimeout(r,200));continue;}
      const round=made.imitationLeft-left;
      this.status("Imitating: round "+(round+1)+" of "+made.imitationLeft+" ...");
      const episodes=made.imitationEpisodes,k=run.collectors.length;
      const shares=await Promise.all(run.collectors.map((w,i)=>{
        const first=Math.floor(i*episodes/k),count=Math.floor((i+1)*episodes/k)-first;
        return trainerCall(w,{type:"dagger",policy:run.policy.slice(0),round,first,count},"share").then(d=>d.share);
      }));
      const d=await trainerCall(run.learner,{type:"imitate",shares},"imitated",shares);
      if(!live())return;
      run.policy=d.policy;left=d.left;run.lastReport=d.report;
      await this.saveLocal(run);
      this.goal(run,d.report);
    }
    if(!live())return;
    if(run.localStopping){await this.finishLocal(run,true);return;}
    if(made.imitationLeft||saved)await this.export(run);
    await this.practice(run);
  }

  async finishLocal(run,stopped) {
    // At this point all worker states already share a committed boundary.
    // Keep the checkpoint even when stopped or completed; discard is explicit.
    try{await this.export(run);}
    catch(error){this.note="Checkpoint saved; network export failed: "+error.message;}
    this.end(run);
    this.status(stopped?"Local training saved. Continue whenever ready.":"Local training complete. Result and checkpoint saved.");
  }

  remoteConnection() {
    if (!this.el.trRemoteUrl.value.trim()) throw Error("Enter the MAG HTTPS address once in the connection settings.");
    const url = new URL(this.el.trRemoteUrl.value);
    if (url.protocol !== "https:" || url.username || url.password || url.search || url.hash || url.pathname !== "/")
      throw Error("MAG needs an HTTPS origin (without a path).");
    try { localStorage.setItem("kern-lehrer.remote-url", url.origin); } catch { /* storage blocked */ }
    if (this.el.trRemoteAddress) this.el.trRemoteAddress.open = false;
    return {url: url.origin};
  }

  restoreRunFields(run) {
    this.task=run.task;
    const cfg=run.config||{};
    for(const [id,value] of [["trTask",run.task],["trWorkers",run.count],["trEnvs",run.envs],
      ["trIterations",run.total],["trSeed",run.seed],["trInitStd",cfg.initStd??""],["trLr",cfg.learningRate??""]])
      if(this.el[id] && value!==undefined && value!==null)this.el[id].value=String(value);
  }

  recoveredConfig(run,data,record) {
    const cfg=data.config || record?.config || {};
    run.name=typeof cfg.task==="string"?cfg.task:data.taskKind;
    run.config={initStd:cfg.initStd,learningRate:cfg.learningRate};
    run.seed=cfg.seed??"";
    run.label=data.taskKind;
    if(run.name.startsWith("motion:") || run.name.startsWith("scenario:")){
      try{run.label=JSON.parse(run.name.slice(run.name.indexOf(":")+1)).name||run.label;}catch{}
    }
    run.recovered=true;
    this.restoreRunFields(run);
  }

  async restoreRemoteHistory(run) {
    try{
      const data=await this.remoteRequest(run,"/v1/jobs/"+run.remote.id+"/history");
      if(this.run!==run || !Array.isArray(data.history))return;
      const points=new Map(run.history.map(p=>[p.it,p]));
      for(const p of data.history)if(Number.isInteger(p.it)&&Number.isFinite(p.ret)&&Number.isFinite(p.len))points.set(p.it,p);
      run.history=[...points.values()].sort((a,b)=>a.it-b.it);
      this.history=run.history;
      this.draw();
    }catch(error){
      // Older MAG builds have no history endpoint. Live control still works.
      if(error.http!==404 && this.run===run)this.note="Progress history temporarily unavailable.";
    }
  }

  remoteRecord(url) {
    try {
      const record = JSON.parse(localStorage.getItem("kern-lehrer.remote-job") || "null");
      return record?.url === url && /^[a-f0-9-]{36}$/.test(record.id) ? record : null;
    } catch { return null; }
  }

  rememberRemote(remote, id = remote.id) {
    if (!id) return;
    try {
      localStorage.setItem("kern-lehrer.remote-job", JSON.stringify({url:remote.url, id, stopping:!!remote.stopping, config:remote.body || remote.config || (this.remoteRecord(remote.url)?.id===id?this.remoteRecord(remote.url)?.config:undefined)}));
    } catch { /* recovery can still discover the active job on the server */ }
  }

  forgetRemote(url, id) {
    try {
      const record = this.remoteRecord(url);
      if (record && (!id || record.id === id)) localStorage.removeItem("kern-lehrer.remote-job");
    } catch { /* storage blocked */ }
  }

  discoverRemote() {
    if (!this.s || !this.el || this.run || this.localBusy || this.checkpointBusy || !this.el.trRemoteUrl.value.trim()) return;
    if(this.lastRemoteResult && this.onnx && this.el.trCompute.value==="mag")return;
    clearTimeout(this.remoteProbeTimer);
    this.remoteProbeTimer = null;
    void this.reconnectRemote({automatic:true});
  }

  // Read-only discovery: a reload never POSTs a new job or imports into a
  // potentially different level. The existing job remains authoritative.
  async reconnectRemote({automatic=false} = {}) {
    if (this.run || this.localBusy || this.checkpointBusy) return;
    let run, connection, record;
    try {
      connection = this.remoteConnection();
      record = this.remoteRecord(connection.url);
      run = this.run = {task: "stand", label: "mag", move: null, workers: [], collectors: [],
        total: 1, iteration: 0, paused: false, history: [], remote: {...connection,
          state: "waiting", id: null, busy: true, stopping: false, network: null}};
      this.status("Reconnecting to MAG ...");
      let data;
      try {
        data = await this.remoteRequest(run, "/v1/current");
      } catch (error) {
        if (this.run !== run) return;
        // The known idle response is not a connection failure. Other 404s
        // (e.g. a wrong server/path) remain errors.
        if (error.http !== 404 || error.message !== "no MAG training job") throw error;
        this.forgetRemote(connection.url);
        this.end(run);
        this.status("Connected to MAG. Ready to start training.");
        return;
      }
      if (this.run !== run) return;
      if (!/^[a-f0-9-]{36}$/.test(data.id) || !["jump","stand","motion","scenario"].includes(data.taskKind)) throw Error("Invalid MAG job");
      const terminal = ["done","stopped","error"].includes(data.state);
      if (automatic && terminal && (record?.id !== data.id || this.lastRemoteResult===data.id)) {
        // A terminal result already displayed in this tab must not be
        // reopened (or forgotten) on every visibility/online event.
        this.end(run);
        if(this.lastRemoteResult!==data.id)this.status("Connected to MAG. Ready to start training.");
        return;
      }
      this.el.trCompute.value = "mag";
      run.remote.id = data.id; run.remote.busy = false;
      run.remote.state = data.state;
      run.remote.stopping = !terminal && record?.id === data.id && !!record.stopping;
      run.remote.config=data.config || (record?.id===data.id?record.config:undefined);
      this.rememberRemote(run.remote);
      run.task = data.taskKind; run.label = "mag-" + data.id; run.token = run.label;
      run.total = data.total; run.envs = data.envs;
      run.count = data.workers; run.paused = data.state === "paused";
      this.recoveredConfig(run,data,record?.id===data.id?record:null);
      this.history=run.history;
      this.lastProgress=null;
      for(const tile of this.host.querySelectorAll("[data-stat] b"))this.text(tile,"-");
      this.onnx = null; this.handover = null;
      this.note = "Reconnected: the latest saved network can be previewed without changing this level.";
      if(data.config)await this.restoreRemoteHistory(run);
      if(this.run===run)await this.remotePractice(run);
    } catch (error) {
      if (run && this.run !== run) return;
      if (run) this.end(run);
      if (automatic && !record) {
        // A stored address does not make MAG the default for new training.
        // Leave local mode usable if the optional host is offline.
        if (this.el.trCompute.value !== "mag") { this.status("Local training ready. MAG unavailable."); return; }
      }
      if (record) {
        this.el.trCompute.value = "mag";
        this.status("MAG connection interrupted — job may still be running. Reconnecting ...", true);
        if (this.s) {
          clearTimeout(this.remoteProbeTimer);
          this.remoteProbeTimer = setTimeout(() => {
            this.remoteProbeTimer = null;
            if (this.el.trCompute.value === "mag") this.discoverRemote();
          }, 5000);
        }
      } else this.status(error.message, true);
    }
  }

  // A second transport, not a second trainer. The existing run, stats, goal
  // and ONNX delivery are shared with the local worker path.
  async remoteRequest(run, path, body, binary = false, timeoutMs = 12000) {
    const remote = run.remote;
    const controller = new AbortController();
    const timeout = setTimeout(() => controller.abort(), timeoutMs);
    try {
      const headers = {"X-Kern-Lehrer": "1"};
      const raw=body instanceof Uint8Array;
      if (body !== undefined) headers["Content-Type"] = raw?"application/octet-stream":"application/json";
      const response = await fetch(remote.url + path, {method: body === undefined ? "GET" : "POST",
        headers, body: body === undefined ? undefined : raw?body:JSON.stringify(body), signal: controller.signal,
        cache: "no-store", credentials: "omit", redirect: "error", referrerPolicy: "no-referrer"});
      if (!response.ok) {
        const data = await response.json().catch(() => ({}));
        const error = Error(data.error || "MAG HTTP " + response.status);
        error.http = response.status;
        throw error;
      }
      return binary ? new Uint8Array(await response.arrayBuffer()) : await response.json();
    } catch (error) {
      if (controller.signal.aborted) throw Error(timeoutMs===12000?"MAG did not respond within 12 seconds. Check Tailscale and browser local network access.":"Checkpoint transfer interrupted. Check MAG before retrying.");
      throw error;
    } finally { clearTimeout(timeout); }
  }

  async remotePractice(run) {
    const remote = run.remote, live = () => this.run === run;
    const wait = ms => new Promise(resolve => setTimeout(resolve, ms));
    this.status("Connecting to MAG ...");
    while (live()) {
      try {
        let data;
        if (!remote.id) {
          data = await this.remoteRequest(run, "/v1/jobs", remote.body);
          if (data.id !== remote.body.id) throw Object.assign(Error("MAG returned a different job id"), {http: 400});
          remote.id = data.id;
          this.rememberRemote(remote);
          run.token = "mag-" + data.id;
        } else {
          data = await this.remoteRequest(run, "/v1/jobs/" + remote.id);
        }
        if (!live()) return;
        remote.state = data.state;
        run.count = data.workers;
        run.paused = data.state === "paused";
        if (data.stats && data.stats.iteration > run.iteration) {
          run.iteration = data.stats.iteration;
          this.stats(run, data.stats, data.stats.collectSeconds + data.stats.learnSeconds,
            !run.history?.some(p=>p.it===data.stats.iteration));
        }
        if (data.report) this.goal(run, data.report);
        // Stop is independent of model download (even an invalid ONNX cannot block it).
        if (remote.stopping && !["stopping", "done", "stopped", "error"].includes(data.state)) {
          await this.remoteRequest(run, "/v1/jobs/" + remote.id + "/stop", {});
          await wait(200); continue;
        }
        if (data.network && data.network !== remote.network) {
          try {
            const bytes = await this.remoteRequest(run, "/v1/jobs/" + remote.id + "/network/" + encodeURIComponent(data.network), undefined, true);
            if (!live()) return;
            try { this.acceptNetwork(run, bytes, data.networkIteration ?? run.iteration); }
            catch (error) { this.note = "Network saved on MAG; level import failed: " + error.message; }
            remote.network = data.network;
          } catch (error) {
            // A snapshot transfer can fail independently of the job. Keep
            // control attached and retry, rather than treating a download
            // 404 as a missing training job.
            if (!live()) return;
            this.note = "Network download interrupted; retrying.";
            this.sync();
            await wait(error.http === 409 ? 200 : 1500);
            continue;
          }
        }
        if (["done", "stopped", "error"].includes(data.state)) {
          if(data.config)await this.restoreRemoteHistory(run);
          remote.stopping=false;
          this.lastRemoteResult=remote.id;
          this.rememberRemote(remote);
          this.end(run); this.status(data.message, data.state === "error"); return;
        }
        this.status(remote.stopping ? "Stop requested; waiting for MAG to finish." :
          data.message + (data.gpus ? ` · ${data.workers} CPU threads · ${data.gpus} HIP GPU(s)` : ""));
        await wait(1000);
      } catch (error) {
        if (!live()) return;
        if (error.http && error.http < 500 && (!remote.id || error.http === 404 || error.http === 403)) {
          this.end(run); this.status(error.message + (remote.id ? " — check MAG if the job is still running." : ""), true); return;
        }
        this.status("MAG connection interrupted: " + error.message + " — job may still be running. Reconnecting ...", true);
        await wait(2000);
      }
    }
  }

  async remoteControl(run, action) {
    const remote = run.remote;
    if (!remote.id || remote.busy) return;
    remote.busy = true; this.sync();
    try {
      const data = await this.remoteRequest(run, "/v1/jobs/" + remote.id + "/" + action, {});
      if (this.run !== run) return;
      remote.state = data.state; run.paused = data.state === "paused";
      this.status(data.message);
    } catch (error) {
      if (this.run === run) this.status("MAG control not confirmed: " + error.message, true);
    } finally { remote.busy = false; if (this.run === run) this.sync(); }
  }

  async practice(run) {
    const live = () => this.run === run;
    while (live() && run.iteration < run.total && !run.localStopping) {
      if (run.paused) { await new Promise(r => setTimeout(r, 200)); continue; }
      const t0 = performance.now();
      const rollouts = await Promise.all(run.collectors.map(w =>
        trainerCall(w, {type: "collect", policy: run.policy.slice(0), iteration: run.iteration + 1, steps: run.steps}, "rollout").then(d => d.rollout)));
      const learned = await trainerCall(run.learner, {type: "learn", rollouts}, "learned", rollouts);
      if (!live()) return;
      run.policy = learned.policy;
      run.iteration = learned.stats.iteration;
      run.lastStats=learned.stats;
      const seconds=(performance.now()-t0)/1000;
      if(learned.stats.episodes)run.history.push({it:run.iteration,ret:learned.stats.meanReturn,len:learned.stats.meanLength*0.02});
      if (run.iteration % 25 === 0 || run.iteration === run.total) {
        if (TRAINER_GOAL[run.task]) run.lastReport=(await trainerCall(run.learner, {type:"evaluate",episodes:5},"evaluated")).report;
      }
      if(!live())return;
      await this.saveLocal(run);
      this.stats(run,learned.stats,seconds,false);
      if(run.lastReport)this.goal(run,run.lastReport);
      if(run.iteration%25===0||run.iteration===run.total)await this.export(run);
    }
    if (!live()) return;
    await this.finishLocal(run,!!run.localStopping);
  }

  /** The learner's network as ONNX: a motion's goes into the level (Level::motions), the others are for download. */
  async export(run) {
    const d = await trainerCall(run.learner, {type: "export"}, "onnx");
    if (this.run !== run) return;
    this.acceptNetwork(run, new Uint8Array(d.onnx), run.iteration);
  }

  acceptNetwork(run, bytes, iteration) {
    if (this.run !== run) return;
    let duration=null;
    if(run.task==="jump")duration=2;
    if(run.task==="motion" && typeof run.name==="string" && run.name.startsWith("motion:")){
      try{duration=JSON.parse(run.name.slice(7)).duration;}catch{}
    }
    const preview=Number.isFinite(duration)&&duration>0?{kind:run.task,duration}:null;
    this.onnx = {bytes, name: run.label, task: run.task, move: null, preview, iteration};
    if (run.move) {
      this.handover = {name: run.move, onnx: toBase64(bytes), run: run.token, iteration};
      const why = this.deliver();
      if (why) throw Error(why);
    } else {
      this.note = `The network is ready to download (iteration ${iteration}).`;
    }
    this.sync();
  }

  previewNetwork() {
    const net=this.onnx;
    if(!net)return;
    const r=net.move ? this.engine.command({cmd:"trainerTest",move:net.move})
      : net.preview ? this.engine.command({cmd:"trainerPreview",...net.preview,onnx:toBase64(net.bytes)}) : null;
    if(r&&!r.ok)this.toast(r.error);
  }

  /**
   * Hands the newest network to the level; one training run is one step of its history. While the
   * level is played it takes no changes: the network waits and goes in when it stops (render()).
   * Returns why the level did not take it, or "".
   */
  deliver() {
    const h = this.handover;
    const r = this.engine.command({cmd: "trainerLearned", name: h.name, onnx: h.onnx, run: h.run});
    if (r.ok) {
      this.handover = null;
      if (this.onnx) this.onnx.move = h.name;
      this.note = h.name === "jump" ? `The level's ducks jump with this network now (iteration ${h.iteration}).` : `The level's ducks know "${h.name}" now (iteration ${h.iteration}).`;
    } else if (r.later) {
      this.note = "The network goes into the level when it stops playing.";
    } else {
      this.handover = null;
      return r.error;
    }
    this.sync();
    return "";
  }

  stats(run, s, seconds, append=true) {
    const set = (id, text) => this.text(this.host.querySelector(`[data-stat="${id}"] b`), text);
    set("it", s.iteration);
    set("steps", Math.round(s.steps).toLocaleString());
    set("sps", Math.round(run.remote ? s.sps : run.collectors.length * run.envs * run.steps / seconds).toLocaleString());
    if (s.episodes) {
      set("ret", s.meanReturn.toFixed(2));
      set("len", (s.meanLength * 0.02).toFixed(2));
      if(append)run.history.push({it: s.iteration, ret: s.meanReturn, len: s.meanLength * 0.02});
    }
    set("std", s.meanStd.toFixed(3));
    this.status(`Learning: iteration ${s.iteration} of ${run.total}, collecting ${s.collectSeconds.toFixed(2)} s, learning ${s.learnSeconds.toFixed(2)} s`);
    this.draw();
  }

  /** How the network does now (evaluation episodes on the clean robot): the task's own measure. */
  goal(run, r) {
    if (!r) return;
    const set = (id, text) => this.text(this.host.querySelector(`[data-stat="${id}"] b`), text);
    if (run.task === "jump") {
      set("goal", `${r.jumps}/${r.episodes}`);
      set("height", r.heightMm.toFixed(1));
      set("flight", Math.round(r.flightS * 1000));
    } else if (run.task === "motion") set("goal", Number.isFinite(r.meanErrorDeg) ? r.meanErrorDeg.toFixed(1) + "°" : "-");
    else if (run.task === "scenario") set("goal", `${r.successes}/${r.episodes}`);
  }

  pause() {
    const run = this.run;
    if (!run) return;
    if (run.remote) { void this.remoteControl(run, run.paused ? "resume" : "pause"); return; }
    run.paused = !run.paused;
    this.status(run.paused ? "Paused." : "Learning ...");
  }

  stop() {
    const run = this.run;
    if (!run) return;
    if (run.remote) {
      run.remote.stopping = true;
      this.rememberRemote(run.remote, run.remote.id || run.remote.body?.id);
      this.status("Stop requested; waiting for MAG to save and release its resources.");
      return;
    }
    run.localStopping=true;
    run.paused=false;
    this.status("Stop requested; saving local training at the next complete checkpoint.");
  }

  /** Ends the run: its workers go; the newest exported network stays (one that waits for the level still goes in). */
  end(run) {
    if (this.run === run) {
      if(!run.remote || run.remote.id)this.lastProgress=Math.min(100,100*run.iteration/run.total);
      this.run = null;
    }
    for (const w of run.workers) w.terminate();
    if (run.workerUrl) URL.revokeObjectURL(run.workerUrl);
    run.wake?.release?.().catch(() => {});
    run.localLease?.();run.localLease=null;
    this.sync();
  }

  /** The curve of the run: mean episode return and episode length over the iterations. */
  draw() {
    const c = this.el?.trChart;
    if (!c || !c.clientWidth) return;
    const history = this.history || [], dpr = devicePixelRatio || 1, w = c.clientWidth, h = c.clientHeight;
    c.width = w * dpr;
    c.height = h * dpr;
    const g = c.getContext("2d");
    g.scale(dpr, dpr);
    g.clearRect(0, 0, w, h);
    const css = getComputedStyle(document.documentElement), pad = 26;
    g.strokeStyle = css.getPropertyValue("--line2");
    g.lineWidth = 1;
    g.strokeRect(pad, 8, w - 2 * pad, h - 8 - pad);
    if (history.length < 2) return;
    const line = (key, color) => {
      const v = history.map(p => p[key]), lo = v.reduce((a,b)=>Math.min(a,b),Infinity), hi = v.reduce((a,b)=>Math.max(a,b),-Infinity), span = hi - lo || 1;
      g.strokeStyle = color;
      g.lineWidth = 2;
      g.beginPath();
      history.forEach((p, i) => {
        const x = pad + (w - 2 * pad) * (i / (history.length - 1)), y = h - pad - (h - 8 - pad) * ((p[key] - lo) / span);
        if (i) g.lineTo(x, y); else g.moveTo(x, y);
      });
      g.stroke();
      return [lo, hi];
    };
    const [lo, hi] = line("ret", css.getPropertyValue("--play").trim());
    line("len", css.getPropertyValue("--accent").trim());
    g.fillStyle = css.getPropertyValue("--dim").trim();
    g.font = "10px system-ui";
    g.fillText(hi.toFixed(1), 2, 16);
    g.fillText(lo.toFixed(1), 2, h - pad);
    g.textAlign = "right";
    g.fillText(String(history[history.length - 1].it), w - pad, h - 8);
  }
}
