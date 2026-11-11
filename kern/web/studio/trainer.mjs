// Kern Studio's Lehrer workspace: the panel beside the stage, and the training run (docs/lehrer.md).
//
// The stage - the Micro Duck in the viewport, posed by hand, and the keyframes of the motion being
// taught - is the engine's (kern/src/studio/trainer.cpp). This file draws the panel from the JSON
// the engine returns, like every Studio panel, and runs the training: collectors and one learner
// in Web Workers, each on its own instance of the headless Lehrer (generated/kern_lehrer.wasm,
// kern/src/lehrer), so the editor stays responsive while the duck learns. A trained network goes
// straight to the engine of this page.
import {tr, escHtml} from "./game.mjs";

/** The Lehrer's module: beside the deployed editor and the single-file build; the module page reads the repository's build. */
const TRAINER_WASM = /\/kern\/web\/studio\//.test(location.pathname) ? "../../../generated/kern_lehrer.wasm" : "./kern_lehrer.wasm";
/** Learned moves kept on this device: {file: {name, file, duration}}; their networks are in IndexedDB "kern-lehrer". */
const TRAINER_SKILLS = "kern-lehrer.motion-skills.v1";

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

/** A network on this device (IndexedDB "kern-lehrer", store "policies", key = file name): kept, or dropped with `bytes` null. */
function trainerStore(file, bytes) {
  return new Promise((resolve, reject) => {
    const open = indexedDB.open("kern-lehrer", 1);
    open.onupgradeneeded = () => open.result.createObjectStore("policies");
    open.onerror = () => reject(open.error);
    open.onsuccess = () => {
      const tx = open.result.transaction("policies", "readwrite");
      if (bytes) tx.objectStore("policies").put(bytes, file);
      else tx.objectStore("policies").delete(file);
      tx.oncomplete = () => resolve();
      tx.onerror = () => reject(tx.error);
    };
  });
}

function trainerSkills() {
  try { return JSON.parse(localStorage.getItem(TRAINER_SKILLS) || "{}") || {}; } catch { return {}; }
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

const TRAINER_TASKS = [["motion", "New motion (keyframes)"], ["jump", "Jump"], ["stand", "Stand (quick test)"], ["scenario", "Free training scenario"]];
const TRAINER_STATS = [["it", "Iteration"], ["steps", "Duck steps"], ["sps", "Steps / s"], ["ret", "Mean episode return"], ["len", "Episode length (s)"],
                       ["std", "Action noise (std)"], ["goal", ""], ["height", "Jump height (mm)"], ["flight", "Flight time (ms)"]];
const TRAINER_GOAL = {motion: "Pose error (degrees)", jump: "Real jumps (clean robot)", scenario: "Goals reached (evaluation)"};

export class Trainer {
  /**
   * `engine`: the Studio's engine; `icon(name)`: the Studio's icons; `toast(text)`; `kernSource()`:
   * the text of kern.mjs on single-file pages (the workers run it), else null.
   */
  constructor({engine, icon, toast, kernSource}) {
    this.engine = engine;
    this.icon = icon;
    this.toast = toast;
    this.kernSource = kernSource;
    this.host = document.getElementById("trainer");
    this.bar = document.getElementById("trainerBar");
    this.s = null;            // the engine's side while the workspace is open (state().trainer)
    this.task = "motion";
    this.run = null;          // the training run
    this.onnx = null;         // the newest network of the last run: {bytes, name, move}
    this.module = null;       // the compiled Lehrer
    this.note = "";
    this.statusText = "";
    this.statusError = false;
    this.keysKey = this.learnedKey = "";
    this.timeFor = 0;         // the motion length the bar's time was last set to
    this.history = [];        // the curve of the last run
    this.texts = new WeakMap();
    this.el = null;
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
      sec("task", "Task", field("Teach", `<select class="sel" id="trTask">${TRAINER_TASKS.map(([id, label]) => `<option value="${id}">${escHtml(tr(label))}</option>`).join("")}</select>`)) +
      sec("motion", "New motion",
        field("Name", `<input class="txt" id="trName" maxlength="38" spellcheck="false" autocomplete="off">`) +
        field("Length", num("trDuration"), "s") +
        `<div class="grp">Keyframes</div><div id="trKeys"></div>` +
        `<div class="tr-buttons"><button class="btn small" id="trClear" type="button">Remove all keyframes</button></div>` +
        `<p class="tr-note">Drag a part of the duck in the viewport and let go: it holds the pose. Add it as a keyframe at its time. The first keyframe is the home pose at 0 s, the last one lies at the motion's end.</p>`) +
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
        `<div class="tr-buttons"><button class="btn go" id="trStart" type="button">Start</button><button class="btn" id="trPause" type="button">Pause</button><button class="btn" id="trStop" type="button">Stop</button></div>` +
        `<div class="tr-status" id="trStatus"></div><div class="tr-bar"><i id="trProgress"></i></div>` +
        `<div class="tr-buttons"><button class="btn small" id="trTest" type="button">Try the network on the duck</button><button class="btn small" id="trDownload" type="button">Download .onnx</button></div>` +
        `<div class="tr-status tr-saved" id="trNote"></div>` +
        `<div class="grp">Expert settings</div>` +
        field("Workers", num("trWorkers", Math.min(cores, 8))) + field("Ducks per worker", num("trEnvs", 8)) + field("Iterations", num("trIterations", 1500)) + field("Seed", num("trSeed", 1)) +
        field("Initial action noise", num("trInitStd", "", `placeholder="Recipe"`)) + field("Learning rate", num("trLr", "", `placeholder="Recipe"`))) +
      sec("stats", "Progress", `<div class="tr-stats">${stats}</div><canvas class="tr-chart" id="trChart"></canvas>` +
        `<div class="tr-legend"><span><i style="background:var(--play)"></i>Episode return</span><span><i style="background:var(--accent)"></i>Episode length</span></div>`) +
      sec("learned", "Learned motions", `<div id="trLearned"></div>`) +
      sec("help", "How it works",
        `<p class="tr-note">A motion from keyframes is learned in two steps: first the network imitates the poses you set, played through the servos under gravity; then it practices with PPO and all the variations (masses, centers of mass, servos, sensors) until it holds the motion on its own. The jump imitates a demonstration that a search (CMA-ES) found in the same physics. At run time there is only the network.</p>` +
        `<p class="tr-note">Each worker runs its own ducks in Kern's engine (the same servos, sensors and the same 50 Hz controller as in the viewport) and acts with the current policy. The learner collects their steps and improves the policy with PPO, the recipe Pollen Robotics uses to train the robot's networks. The result is an ONNX file in the format of Pollen's networks; Kern Studio and the robot itself can load it.</p>` +
        `<p class="tr-note">Training goes on while you edit, play or leave this workspace; keep the tab open, and a phone on its charger. More workers use more cores. The same seed and the same number of ducks produce the same network bit for bit, on every device.</p>` +
        `<p class="tr-note">Every 25 iterations the newest network goes to the duck: a motion is a move the Blueprint node Robot Move plays by its name, a jump network is the duck's move "jump". They are kept on this device.</p>`, true);
    this.bar.innerHTML = `<div class="tr-pose"><span class="dim">Keyframe at</span>${num("trTime", "1.2")}<span class="dim">s</span>` +
      `<button class="btn primary" id="trCapture" type="button">+ Keyframe</button><button class="btn" id="trHome" type="button">Home pose</button></div>` +
      `<div class="tr-test"><span id="trTesting" data-i18n-skip></span><button class="btn primary" id="trAgain" type="button">Again</button><button class="btn" id="trPosing" type="button">Back to posing</button></div>`;
    const el = this.el = {};
    for (const node of [...this.host.querySelectorAll("[id]"), ...this.bar.querySelectorAll("[id]")]) el[node.id] = node;
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
      if (row && e.target.closest("[data-try]")) command({cmd: "trainerTest", move: row.dataset.move});
      if (row && e.target.closest("[data-level]")) command({cmd: "trainerPlay", move: row.dataset.move});
      if (row && e.target.closest("[data-forget]")) this.forget(row.dataset.file);
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
    el.trClear.onclick = () => command({cmd: "trainerClear"});
    el.trCapture.onclick = () => {
      // The next pose is most likely the end of the motion.
      if (command({cmd: "trainerCapture", time: trainerNumber(el.trTime.value)})?.ok) el.trTime.value = this.s.motion.duration;
    };
    el.trHome.onclick = () => command({cmd: "trainerStage"});
    el.trPosing.onclick = () => command({cmd: "trainerStage"});
    el.trAgain.onclick = () => { if (this.s?.testing) command({cmd: "trainerTest", move: this.s.testing}); };
    el.trStart.onclick = () => this.start();
    el.trPause.onclick = () => this.pause();
    el.trStop.onclick = () => this.stop();
    el.trTest.onclick = () => { if (this.onnx?.move) command({cmd: "trainerTest", move: this.onnx.move}); };
    el.trDownload.onclick = () => { if (this.onnx) trainerDownload(`${this.onnx.name}_kern.onnx`, this.onnx.bytes, "application/octet-stream"); };
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
  }

  /** The engine's state changed (every Studio panel is drawn from it). */
  render(s) {
    const open = !!s.trainer;
    this.s = s.trainer || null;
    this.covered = !!s.logic?.open;  // the Blueprint editor lies over the viewports, the stage with them
    document.getElementById("main").classList.toggle("trainer-open", open);
    document.getElementById("trainerPanel").hidden = !open;
    const button = document.getElementById("trainerBtn");
    button.classList.toggle("on", open);
    if (open && !this.el) { this.build(); this.statusText = `Ready. ${navigator.hardwareConcurrency || "?"} cores on this device.`; }
    if (this.el) this.sync();
  }

  /** Brings the panel and the bar in step with the engine's state and the run. */
  sync() {
    const el = this.el, s = this.s, run = this.run;
    if (!el) return;
    const open = !!s, task = this.task, motion = s?.motion;
    el.trTask.value = task;
    el.trTask.disabled = !!run;
    this.host.querySelector('[data-tsec="motion"]').hidden = task !== "motion";
    this.host.querySelector('[data-tsec="scenario"]').hidden = task !== "scenario";
    if (motion) {
      if (document.activeElement !== el.trName) el.trName.value = motion.name;
      if (document.activeElement !== el.trDuration) el.trDuration.value = motion.duration;
      // A keyframe is most often taken for the motion's end.
      if (motion.duration !== this.timeFor && document.activeElement !== el.trTime) { this.timeFor = motion.duration; el.trTime.value = motion.duration; }
      const key = JSON.stringify(motion.keys.map(k => k.time));
      if (key !== this.keysKey && !el.trKeys.contains(document.activeElement)) {
        this.keysKey = key;
        el.trKeys.innerHTML = motion.keys.map((k, i) => `<div class="tr-key" data-key="${i}"><input class="num" value="${k.time}" inputmode="decimal" spellcheck="false" aria-label="Time of the keyframe (s)">` +
          `<span class="dim">s</span><button class="btn small" data-show type="button" title="The servos take the duck to this pose">Show</button>` +
          `<button class="ph" data-drop type="button" title="Remove this keyframe">${this.icon("trash")}</button></div>`).join("");
      }
    }
    // The run.
    el.trStart.disabled = !!run;
    el.trPause.disabled = !run;
    el.trStop.disabled = !run;
    this.text(el.trPause, run?.paused ? "Resume" : "Pause");
    el.trDownload.disabled = !this.onnx;
    el.trTest.disabled = !this.onnx?.move || !open;
    this.text(el.trStatus, this.statusText);
    el.trStatus.classList.toggle("err", this.statusError);
    this.text(el.trNote, this.note);
    el.trProgress.style.width = run ? `${Math.min(100, 100 * run.iteration / run.total).toFixed(1)}%` : this.onnx ? "100%" : "0";
    const shown = run?.task ?? this.onnx?.task ?? task;
    for (const tile of this.host.querySelectorAll("[data-stat]")) {
      const id = tile.dataset.stat;
      if (id === "goal") { tile.hidden = !TRAINER_GOAL[shown]; this.text(tile.lastElementChild, TRAINER_GOAL[shown] || ""); }
      if (id === "height" || id === "flight") tile.hidden = shown !== "jump";
    }
    // The learned moves on this device.
    const learned = Object.values(trainerSkills()).filter(m => m && typeof m.name === "string" && typeof m.file === "string" && Number.isFinite(m.duration));
    const lkey = JSON.stringify(learned);
    if (lkey !== this.learnedKey) {
      this.learnedKey = lkey;
      el.trLearned.innerHTML = learned.length ? learned.map(m => `<div class="tr-learned" data-move="${escHtml(m.name)}" data-file="${escHtml(m.file)}"><span class="nm" data-i18n-skip title="${escHtml(m.name)}">${escHtml(m.name)}</span>` +
        `<span class="dim">${m.duration} s</span><button class="btn small" data-try type="button" title="The duck tries the move on the stage">Try</button>` +
        `<button class="btn small" data-level type="button" title="The level's Micro Duck plays the move">In the level</button>` +
        `<button class="ph" data-forget type="button" title="Remove from this device">${this.icon("trash")}</button></div>`).join("")
        : `<p class="tr-note">Nothing learned on this device yet. A motion you train appears here, and the Blueprint node Robot Move plays it by its name.</p>`;
    }
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

  /** Removes a learned move from this device (the engine keeps it until the page reloads). */
  async forget(file) {
    const skills = trainerSkills();
    delete skills[file];
    try { localStorage.setItem(TRAINER_SKILLS, JSON.stringify(skills)); } catch { /* storage blocked */ }
    await trainerStore(file, null).catch(() => {});
    this.sync();
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
    await trainerCall(w, {type: "load", module: this.module}, "loaded");
    return w;
  }

  async start() {
    if (this.run) return;
    const el = this.el, task = this.task;
    let name = task, move = null, label = task;
    try {
      if (task === "motion") {
        const r = this.engine.command({cmd: "trainerTask"});
        if (!r.ok) throw Error(r.error);
        name = r.task;
        move = {name: r.move, file: r.file, duration: r.duration};
        label = r.move;
      } else if (task === "scenario") {
        const scenario = this.scenarioConfig();
        name = "scenario:" + JSON.stringify(scenario);
        label = "scenario_" + scenario.name.replace(/[^a-z0-9_-]/gi, "_").slice(0, 48);
      }
    } catch (error) { this.status(error.message, true); return; }
    const whole = (id, lo, hi, fallback) => Math.max(lo, Math.min(hi, Math.round(trainerNumber(el[id].value)) || fallback));
    const run = this.run = {task, name, move, label, workers: [], collectors: [], learner: null, workerUrl: null, wake: null, policy: null, steps: 24,
      count: whole("trWorkers", 1, 64, 1), envs: whole("trEnvs", 1, 256, 8), total: whole("trIterations", 1, 100000, 1), seed: whole("trSeed", 1, 4294967295, 1),
      iteration: 0, paused: false, history: []};
    this.onnx = null;
    this.note = "";
    this.history = run.history;
    for (const tile of this.host.querySelectorAll("[data-stat] b")) this.text(tile, "-");
    this.draw();
    const live = () => this.run === run;
    try {
      if (!this.module) {
        this.status("Loading the Lehrer ...");
        this.module = await WebAssembly.compileStreaming(fetch(TRAINER_WASM)).catch(async () => WebAssembly.compile(await (await fetch(TRAINER_WASM)).arrayBuffer()));
        if (!live()) return;
      }
      this.status(`Starting ${run.count} ${run.count === 1 ? "worker" : "workers"} with ${run.envs} ${run.envs === 1 ? "duck" : "ducks"} each ...`);
      run.wake = await navigator.wakeLock?.request("screen").catch(() => null);
      run.learner = await this.spawn(run);
      run.collectors = await Promise.all(Array.from({length: run.count}, () => this.spawn(run)));
      // Worker i steps ducks i*envs .. (i+1)*envs-1 of one run: the same run as natively with as many
      // ducks in all (bit for bit, docs/lehrer.md).
      await Promise.all(run.collectors.map((w, i) => trainerCall(w, {type: "collector", task: name, envs: run.envs, seed: run.seed, first: i * run.envs}, "ready")
        .then(d => { if (!d.ok) throw Error("The Lehrer does not take this task"); })));
      const config = {};
      if (trainerNumber(el.trInitStd.value) > 0) config.initStd = trainerNumber(el.trInitStd.value);
      if (trainerNumber(el.trLr.value) > 0) config.learningRate = trainerNumber(el.trLr.value);
      const made = await trainerCall(run.learner, {type: "learner", task: name, seed: run.seed, config}, "policy");
      if (!live()) return;
      run.policy = made.policy;
      run.steps = made.steps || 24;
      // First imitation of the demonstration (the jump's recipe), round by round, then practice.
      let left = made.imitationLeft || 0;
      while (live() && left > 0) {
        const round = made.imitationLeft - left;
        this.status(`Imitating: round ${round + 1} of ${made.imitationLeft} ...`);
        // The round's episodes spread over the collectors (worker i: its share, in episode order).
        const episodes = made.imitationEpisodes, k = run.collectors.length;
        const shares = await Promise.all(run.collectors.map((w, i) => {
          const first = Math.floor(i * episodes / k), count = Math.floor((i + 1) * episodes / k) - first;
          return trainerCall(w, {type: "dagger", policy: run.policy.slice(0), round, first, count}, "share").then(d => d.share);
        }));
        const d = await trainerCall(run.learner, {type: "imitate", shares}, "imitated", shares);
        if (!live()) return;
        run.policy = d.policy;
        left = d.left;
        this.goal(run, d.report);
        this.status(`Imitating: round ${d.report.round} of ${d.report.rounds}, ${d.report.samples.toLocaleString()} samples, error ${d.report.error.toFixed(4)}`);
      }
      if (made.imitationLeft && live()) await this.export(run);
      await this.practice(run);
    } catch (error) {
      if (!live()) return;
      this.end(run);
      this.status(error.message, true);
    }
  }

  async practice(run) {
    const live = () => this.run === run;
    while (live() && run.iteration < run.total) {
      if (run.paused) { await new Promise(r => setTimeout(r, 200)); continue; }
      const t0 = performance.now();
      const rollouts = await Promise.all(run.collectors.map(w =>
        trainerCall(w, {type: "collect", policy: run.policy.slice(0), iteration: run.iteration + 1, steps: run.steps}, "rollout").then(d => d.rollout)));
      const learned = await trainerCall(run.learner, {type: "learn", rollouts}, "learned", rollouts);
      if (!live()) return;
      run.policy = learned.policy;
      run.iteration = learned.stats.iteration;
      this.stats(run, learned.stats, (performance.now() - t0) / 1000);
      if (run.iteration % 25 === 0 || run.iteration === run.total) {
        if (TRAINER_GOAL[run.task]) this.goal(run, (await trainerCall(run.learner, {type: "evaluate", episodes: 5}, "evaluated")).report);
        if (!live()) return;
        await this.export(run);
      }
    }
    if (!live()) return;
    this.end(run);
    this.status(`Done: ${run.iteration} iterations. Download the network, or play the move in your level.`);
  }

  /** The learner's network as ONNX: to the engine of this page at once, and kept on this device. */
  async export(run) {
    const d = await trainerCall(run.learner, {type: "export"}, "onnx");
    if (this.run !== run) return;
    const bytes = new Uint8Array(d.onnx), task = run.task;
    this.onnx = {bytes, name: run.label, task, move: null};
    if (task === "motion") {
      const m = run.move;
      if (!this.engine.registerPolicy(m.file, bytes) || !this.engine.command({cmd: "trainerRegister", move: m.name, file: m.file, duration: m.duration}).ok)
        throw Error("The engine did not take the trained network");
      this.onnx.move = m.name;
      const skills = trainerSkills();
      skills[m.file] = m;
      try { localStorage.setItem(TRAINER_SKILLS, JSON.stringify(skills)); } catch { /* storage blocked: the move lasts until the page reloads */ }
      await trainerStore(m.file, bytes).catch(() => {});
      this.note = `The duck knows "${m.name}" now (iteration ${run.iteration}).`;
    } else if (task === "jump") {
      // Kern's jump: the duck's move "jump" runs this network from now on.
      if (!this.engine.registerPolicy("jump.onnx", bytes)) throw Error("The engine did not take the trained network");
      this.onnx.move = "jump";
      await trainerStore("jump.onnx", bytes).catch(() => {});
      this.note = `The duck's move "jump" runs this network now (iteration ${run.iteration}).`;
    } else {
      this.note = `The network is ready to download (iteration ${run.iteration}).`;
    }
    this.sync();
  }

  stats(run, s, seconds) {
    const set = (id, text) => this.text(this.host.querySelector(`[data-stat="${id}"] b`), text);
    set("it", s.iteration);
    set("steps", Math.round(s.steps).toLocaleString());
    set("sps", Math.round(run.collectors.length * run.envs * run.steps / seconds).toLocaleString());
    if (s.episodes) {
      set("ret", s.meanReturn.toFixed(2));
      set("len", (s.meanLength * 0.02).toFixed(2));
      run.history.push({it: s.iteration, ret: s.meanReturn, len: s.meanLength * 0.02});
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
    run.paused = !run.paused;
    this.status(run.paused ? "Paused." : "Learning ...");
  }

  stop() {
    const run = this.run;
    if (!run) return;
    this.end(run);
    this.status(`Stopped after ${run.iteration} iterations.`);
  }

  /** Ends the run: its workers go; the newest exported network stays. */
  end(run) {
    if (this.run === run) this.run = null;
    for (const w of run.workers) w.terminate();
    if (run.workerUrl) URL.revokeObjectURL(run.workerUrl);
    run.wake?.release?.().catch(() => {});
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
      const v = history.map(p => p[key]), lo = Math.min(...v), hi = Math.max(...v), span = hi - lo || 1;
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
