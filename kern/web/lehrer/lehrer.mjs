// Kern Lehrer page: collectors in Web Workers, one learner worker, a chart (docs/lehrer.md).
const $ = id => document.getElementById(id);
const here = new URL(".", import.meta.url);
const wasmUrl = new URL("../../../generated/kern_lehrer.wasm", here);
const workerUrl = new URL("./worker.mjs", here);

/**
 * Language, as in Kern Studio (kern/web/studio/game.mjs): the browser's language (German for de*,
 * English otherwise); `?lang=de` or `?lang=en` overrides it and is remembered on this device under
 * the same key as the Studio's, so both pages speak the same language. The page is written in
 * German; in English its texts are swapped for LEHRER_EN (exact phrases, trimmed).
 */
const I18N_KEY = "kern.lang";
function pickLanguage() {
  try {
    const q = new URLSearchParams(location.search).get("lang");
    if (q === "de" || q === "en") { localStorage.setItem(I18N_KEY, q); return q; }
    const kept = localStorage.getItem(I18N_KEY);
    if (kept === "de" || kept === "en") return kept;
  } catch { /* storage blocked: follow the browser */ }
  const langs = (navigator.languages?.length ? navigator.languages : [navigator.language]) || [];
  return String(langs[0] || "en").toLowerCase().startsWith("de") ? "de" : "en";
}
const LANG = pickLanguage();
const EN = LANG === "en";
/** The text in the page's language: German as written, or its English version. */
const t = (de, en) => (EN ? en : de);

const LEHRER_EN = {
  "Kern Lehrer: bringt der Micro Duck auf diesem Gerät neue Bewegungen bei, in Kerns eigener Engine und Physik.":
    "Kern Lehrer: teaches the Micro Duck new motions on this device, in Kern's own engine and physics.",
  "Bringt der Micro Duck auf diesem Gerät neue Bewegungen bei: Kerns Engine und Box3D, PPO in C++.":
    "Teaches the Micro Duck new motions on this device: Kern's engine and Box3D, PPO in C++.",
  "Bewegung": "Motion",
  "Sprung": "Jump",
  "Stehen (Schnelltest)": "Stand (quick test)",
  "Worker": "Workers",
  "Enten je Worker": "Ducks per worker",
  "Iterationen": "Iterations",
  "Aktionsrauschen am Anfang": "Initial action noise",
  "Lernrate": "Learning rate",
  "Rezept": "Recipe",
  ".onnx herunterladen": "Download .onnx",
  "Die Engine lädt ...": "The engine is loading ...",
  "Entenschritte": "Duck steps",
  "Schritte / s": "Steps / s",
  "mittlere Episoden-Rückgabe": "Mean episode return",
  "Episodenlänge (s)": "Episode length (s)",
  "Aktionsrauschen (std)": "Action noise (std)",
  "echte Sprünge (sauberer Roboter)": "Real jumps (clean robot)",
  "Sprunghöhe (mm)": "Jump height (mm)",
  "Flugzeit (ms)": "Flight time (ms)",
  "Episoden-Rückgabe": "Episode return",
  "Episodenlänge": "Episode length",
  "Wie es funktioniert": "How it works",
  "Der Sprung lernt in zwei Schritten: Zuerst ahmt das Netz ein Vorbild nach, einen Sprung, den eine Suche (CMA-ES) in derselben Physik gefunden hat. Danach übt es mit PPO und allen Variationen (Massen, Schwerpunkte, Servos, Sensoren), bis es den Sprung selbst sicher kann. Zur Laufzeit gibt es nur das Netz.":
    "The jump is learned in two steps: first the network imitates a demonstration, a jump that a search (CMA-ES) found in the same physics. Then it practices with PPO and all the variations (masses, centers of mass, servos, sensors) until it can do the jump reliably on its own. At run time there is only the network.",
  "Jeder Worker führt seine eigenen Enten in Kerns Engine (dieselben Servos, Sensoren und derselbe 50-Hz-Regler wie im Kern Studio) und handelt mit der aktuellen Policy. Der Lerner sammelt ihre Schritte und verbessert die Policy mit PPO, dem Rezept, mit dem Pollen Robotics die Netze des Roboters trainiert. Heraus kommt eine ONNX-Datei im Format von Pollens Netzen; Kern Studio und der Roboter selbst können sie laden.":
    "Each worker runs its own ducks in Kern's engine (the same servos, sensors and the same 50 Hz controller as in Kern Studio) and acts with the current policy. The learner collects their steps and improves the policy with PPO, the recipe Pollen Robotics uses to train the robot's networks. The result is an ONNX file in the format of Pollen's networks; Kern Studio and the robot itself can load it.",
  "Diesen Tab offen lassen, solange er lernt; ein Handy ans Ladekabel. Mehr Worker nutzen mehr Kerne. Gleicher Seed und gleich viele Enten ergeben Bit für Bit dasselbe Netz, auf jedem Gerät.":
    "Keep this tab open while it learns; plug a phone into its charger. More workers use more cores. The same seed and the same number of ducks produce the same network bit for bit, on every device.",
  "Alle 25 Iterationen wird das Netz auf diesem Gerät für Kern Studio gespeichert: Die Bewegung „jump“ der Micro Duck (Logik-Knoten Robot Move, Bewegung „jump“) nutzt dann das neueste Sprung-Netz, in offenen Studio-Tabs sofort.":
    "Every 25 iterations the network is saved on this device for Kern Studio: the Micro Duck's “jump” move (logic node Robot Move, move “jump”) then uses the newest jump network, immediately in open Studio tabs.",
};

/** In English: swaps the page's German texts (text nodes, placeholders, the description) once. */
function localizePage() {
  document.documentElement.lang = LANG;
  if (!EN) return;
  const en = s => {
    const key = s.trim();
    const out = LEHRER_EN[key];
    return out === undefined ? s : s.replace(key, out);
  };
  const it = document.createTreeWalker(document.body, NodeFilter.SHOW_TEXT);
  for (let n = it.nextNode(); n; n = it.nextNode()) {
    if (n.parentElement?.closest("script, style")) continue;
    const out = en(n.nodeValue);
    if (out !== n.nodeValue) n.nodeValue = out;
  }
  for (const el of document.querySelectorAll("[placeholder], [title], [aria-label], meta[name=description]")) {
    for (const a of ["placeholder", "title", "aria-label", "content"]) {
      const v = el.getAttribute(a);
      if (v) { const out = en(v); if (out !== v) el.setAttribute(a, out); }
    }
  }
}

/** The DE | EN switch, bottom right as in Kern Studio's status bar: remembers the choice and reloads. */
function languageSwitch() {
  const b = document.createElement("button");
  b.id = "langBtn";
  b.type = "button";
  const de = !EN;
  b.innerHTML = `<span class="${de ? "on" : ""}">DE</span><span class="${de ? "" : "on"}">EN</span>`;
  b.title = de ? "Sprache: Deutsch (tippen für English)" : "Language: English (tap for Deutsch)";
  b.setAttribute("aria-label", b.title);
  b.addEventListener("click", () => {
    try { localStorage.setItem(I18N_KEY, de ? "en" : "de"); } catch { /* storage blocked */ }
    const u = new URL(location.href);
    u.searchParams.delete("lang");
    // Without storage the address carries the choice.
    try { if (localStorage.getItem(I18N_KEY) !== (de ? "en" : "de")) throw 0; } catch { u.searchParams.set("lang", de ? "en" : "de"); }
    location.replace(u.href);
  });
  document.body.append(b);
}

localizePage();
languageSwitch();

const cores = Math.max(1, (navigator.hardwareConcurrency || 2) - 1);
$("workers").value = Math.min(cores, 8);

let module = null;
let running = false, paused = false;
let learner = null, collectors = [];
let policy = null, iteration = 0, history = [], onnx = null, wakeLock = null, steps = 24;

function status(text, error = false) {
  $("status").textContent = text;
  $("status").classList.toggle("err", error);
}

function call(worker, message, expect, transfer = []) {
  return new Promise((resolve, reject) => {
    worker.onmessage = ({ data }) => {
      if (data.type === "error") reject(Error(data.message));
      else if (data.type === expect) resolve(data);
    };
    worker.onerror = e => reject(Error(e.message || "worker failed"));
    worker.postMessage(message, transfer);
  });
}

async function spawn() {
  const w = new Worker(workerUrl, { type: "module" });
  await call(w, { type: "load", module }, "loaded");
  return w;
}


// The Studio iframe uses Kern's real Box3D MouseGrab, not a JS mock-up.
let motionReady = false, motionNextId = 0;
const motionPending = new Map();
let motionFrames = [];
let latestMotion = null;
const motionFrame = $("motionStudio");
function motionTell(type, data = {}) {
  if (!motionReady) return Promise.reject(Error("3D-Ente ist noch nicht bereit"));
  return new Promise((resolve, reject) => {
    const id = ++motionNextId;
    const timer = setTimeout(() => { motionPending.delete(id); reject(Error("Studio antwortet nicht")); }, 8000);
    motionPending.set(id, result => { clearTimeout(timer); result?.ok ? resolve(result) : reject(Error(result?.error || "Studio Fehler")); });
    motionFrame.contentWindow.postMessage({type: "kern-motion-" + type, id, ...data}, location.origin);
  });
}
function motionStatus(message) { $("motionStatus").textContent = message; }
function motionSlug() {
  const s = $("motionName").value.normalize("NFKD").replace(/[\u0300-\u036f]/g, "")
    .replace(/[^a-z0-9_-]+/gi, "_").replace(/^_+|_+$/g, "").toLowerCase().slice(0, 38);
  return "motion_" + (s || "bewegung");
}
function motionSpec() {
  const duration = Number($("motionDuration").value);
  if (!Number.isFinite(duration) || duration < .3 || duration > 15) throw Error("Dauer: 0,3 bis 15 Sekunden");
  const frames = [...motionFrames].sort((a,b)=>a.time-b.time);
  if (frames.length < 2 || Math.abs(frames[0].time) > .0001 ||
      Math.abs(frames.at(-1).time - duration) > .001)
    throw Error("Mindestens zwei Keyframes nötig: bei 0 und bei " + duration + " Sekunden");
  if (frames.length > 32) throw Error("Maximal 32 Keyframes");
  return {name: motionSlug(), duration, frames};
}
function renderMotionFrames() {
  const el = $("motionFrames");
  el.replaceChildren();
  for (const frame of motionFrames) {
    const row = document.createElement("div");
    const preview = document.createElement("button");
    preview.textContent = frame.time.toFixed(2) + " s · Pose ansehen";
    preview.onclick = () => motionTell("preview", {joints: frame.joints}).catch(e => motionStatus(e.message));
    const remove = document.createElement("button");
    remove.textContent = "Entfernen";
    remove.style.marginLeft = "6px";
    remove.onclick = () => { motionFrames = motionFrames.filter(f=>f!==frame); renderMotionFrames(); };
    row.append(preview, remove);
    el.append(row);
  }
  let valid = false, why = "";
  try { motionSpec(); valid = true; }
  catch (error) { why = error.message; }
  $("motionTrain").disabled = running || !valid;
  motionStatus(motionFrames.length + " Keyframes gespeichert. " +
    (valid ? "Fertig zum Trainieren – oder weitere Posen hinzufügen." :
      (why || "Weitere Posen hinzufügen.")));
}
window.addEventListener("message", event => {
  if (event.origin !== location.origin || event.source !== motionFrame.contentWindow) return;
  if (event.data?.type === "kern-motion-ready") {
    motionReady = !!event.data.ok;
    motionStatus(motionReady ? "Ente bereit: Kopf oder Gliedmaß mit der Maus ziehen, dann Pose speichern." :
      "Studio-Fehler: " + (event.data.error || "unbekannt"));
  } else if (event.data?.type === "kern-motion-reply") {
    const done = motionPending.get(event.data.id);
    if (done) { motionPending.delete(event.data.id); done(event.data.result); }
  }
});
$("motionCapture").onclick = async () => {
  try {
    const time = Number($("motionTime").value);
    const duration = Number($("motionDuration").value);
    if (!Number.isFinite(time) || time < 0 || time > duration) throw Error("Zeit muss zwischen 0 und Dauer liegen");
    const result = await motionTell("capture");
    const joints = result.joints;
    if (!Array.isArray(joints) || joints.length !== 14 || joints.some(x=>!Number.isFinite(x)))
      throw Error("Studio liefert keine gültigen 14 Gelenkwinkel");
    motionFrames = motionFrames.filter(f=>Math.abs(f.time-time)>.001);
    motionFrames.push({time,joints});
    motionFrames.sort((a,b)=>a.time-b.time);
    renderMotionFrames();
    $("motionTime").value = String(Math.min(duration, Math.round((time + .2)*100)/100));
  } catch (error) { motionStatus(error.message); }
};
$("motionReset").onclick = () => motionTell("restart").then(()=>motionStatus("Ente zurückgesetzt. Keyframes bleiben erhalten.")).catch(e=>motionStatus(e.message));
$("motionClear").onclick = () => { motionFrames = []; renderMotionFrames(); };
$("motionDuration").addEventListener("input", renderMotionFrames);
$("motionAnchor").onchange = () => {
  motionTell("anchor", {anchored: $("motionAnchor").checked})
    .then(() => motionStatus($("motionAnchor").checked ?
      "Rumpf fixiert: Kopf und Beine lassen sich ohne Servo-Gegenkraft ziehen." :
      "Ente frei schwebend: auch der Rumpf kann mit der Maus verschoben werden."))
    .catch(error => motionStatus(error.message));
};
$("motionTrain").onclick = () => start();
renderMotionFrames();
$("motionTest").onclick = () => {
  if (!latestMotion) return;
  motionTell("run", latestMotion).then(()=>motionStatus("Neues ONNX-Netz steuert die Ente mit Erdschwerkraft."))
    .catch(e=>motionStatus("Test fehlgeschlagen: "+e.message));
};
function motionMeta() {
  const spec = motionSpec();
  return {name: spec.name, file: spec.name + ".onnx", duration: spec.duration};
}

// A scenario is data, not a generated C++ task. Both the Web Worker collector and the PPO learner
// receive the exact same canonical JSON; the native trainer accepts the same schema with --scenario.
const DEFAULT_SCENARIO = {
  name: "Mein Parcours", duration: 8,
  commands: { forward: 0.15, lateral: 0, turn: 0 },
  start: { yaw: 0, yawSpread: 0 },
  waypoints: [{ x: 0.4, y: 0, radius: 0.15 }, { x: 0.85, y: 0, radius: 0.15 }],
  objects: [{ kind: "box", x: 0.62, y: 0.25, width: 0.15, depth: 0.15, height: 0.20 }],
  rewards: { progress: 12, waypoint: 8, finish: 20, upright: 2, fall: -12,
    actionRate: -0.1, contact: -2, height: 0, speed: 0, pose: 0 }
};

function numField(id, lo, hi) {
  const raw = $(id).value.trim();
  const n = Number(raw);
  if (!raw || !Number.isFinite(n) || n < lo || n > hi) throw Error("Ungültiger Wert: " + id);
  return n;
}

function parseRows(id, type) {
  const result = [];
  const lines = $(id).value.split(/\r?\n/);
  for (let i = 0; i < lines.length; ++i) {
    const line = lines[i].replace(/#.*/, "").trim();
    if (!line) continue;
    const bits = line.split(/[\s;]+/);
    if (type === "waypoint") {
      if (bits.length !== 3 || bits.some(v => !Number.isFinite(Number(v))))
        throw Error("Zielpunkt Zeile " + (i + 1) + ": erwartet x y Radius");
      const [x, y, radius] = bits.map(Number);
      if (Math.abs(x) > 5 || Math.abs(y) > 5 || radius < 0.03 || radius > 1)
        throw Error("Zielpunkt Zeile " + (i + 1) + " liegt außerhalb der erlaubten Grenzen");
      result.push({ x, y, radius });
    } else {
      const kind = bits[0];
      if ((kind !== "box" && kind !== "sphere") ||
          bits.length !== (kind === "box" ? 6 : 4) ||
          bits.slice(1).some(v => !Number.isFinite(Number(v))))
        throw Error("Objekt Zeile " + (i + 1) + ": box x y Breite Tiefe Höhe / sphere x y Radius");
      const n = bits.slice(1).map(Number);
      const obj = kind === "box" ?
        { kind, x: n[0], y: n[1], width: n[2], depth: n[3], height: n[4] } :
        { kind, x: n[0], y: n[1], radius: n[2] };
      if (Math.abs(obj.x) > 5 || Math.abs(obj.y) > 5 ||
          n.slice(2).some(v => v < 0.01 || v > 2))
        throw Error("Objekt Zeile " + (i + 1) + " liegt außerhalb der erlaubten Grenzen");
      result.push(obj);
    }
  }
  if (result.length > (type === "waypoint" ? 16 : 32))
    throw Error("Zu viele " + (type === "waypoint" ? "Zielpunkte" : "Objekte"));
  return result;
}

function setScenario(cfg) {
  if (!cfg || typeof cfg !== "object" || Array.isArray(cfg)) throw Error("Ein JSON-Objekt wird erwartet");
  $("scName").value = cfg.name ?? "Szenario";
  $("scDuration").value = cfg.duration ?? 6;
  $("scForward").value = cfg.commands?.forward ?? 0;
  for (const [id, key, fallback] of [
    ["scProgress", "progress", 12], ["scWaypoint", "waypoint", 8],
    ["scFinish", "finish", 20], ["scUpright", "upright", 2], ["scFall", "fall", -12]
  ]) $(id).value = cfg.rewards?.[key] ?? fallback;
  $("scWaypoints").value = (cfg.waypoints || []).map(p => [p.x, p.y, p.radius ?? 0.15].join(" ")).join("\n");
  $("scObjects").value = (cfg.objects || []).map(o =>
    o.kind === "sphere" ? ["sphere", o.x, o.y, o.radius ?? 0.08].join(" ") :
    ["box", o.x, o.y, o.width ?? 0.15, o.depth ?? 0.15, o.height ?? 0.15].join(" ")
  ).join("\n");
  $("scJson").value = JSON.stringify(cfg, null, 2);
}

function scenarioConfig() {
  // The advanced JSON is the source for options without a visual field (14 joint angles,
  // actuation/speed terms, randomisation, dynamic objects). Fields overwrite only their own keys.
  const cfg = JSON.parse($("scJson").value);
  if (!cfg || typeof cfg !== "object" || Array.isArray(cfg)) throw Error("Szenario muss ein JSON-Objekt sein");
  cfg.name = $("scName").value.trim().slice(0, 80) || "Szenario";
  cfg.duration = numField("scDuration", 0.5, 60);
  cfg.commands = { ...(cfg.commands || {}), forward: numField("scForward", -0.4, 0.4) };
  const points = parseRows("scWaypoints", "waypoint");
  cfg.waypoints = points.map((p, i) => ({ ...(cfg.waypoints?.[i] || {}), ...p }));
  const objs = parseRows("scObjects", "object");
  cfg.objects = objs.map((o, i) => ({ ...(cfg.objects?.[i] || {}), ...o }));
  cfg.rewards = { ...(cfg.rewards || {}),
    progress: numField("scProgress", -100, 100),
    waypoint: numField("scWaypoint", -100, 100),
    finish: numField("scFinish", -100, 100),
    upright: numField("scUpright", -100, 100),
    fall: numField("scFall", -100, 100) };
  if (cfg.waypoints.length === 0 && !cfg.rewards.pose && !cfg.rewards.height && !cfg.rewards.speed &&
      !cfg.rewards.upright) throw Error("Mindestens ein Zielpunkt oder ein Haltungs-/Geschwindigkeitsziel ist nötig");
  if (JSON.stringify(cfg).length > 16384) throw Error("Szenario zu groß (maximal 16 KiB)");
  return cfg;
}

$("task").addEventListener("change", () => {
  const scenario = $("task").value === "scenario";
  const motion = $("task").value === "motion";
  $("motionPanel").hidden = !motion;
  $("scenarioPanel").hidden = !scenario;
  $("start").hidden = motion;  // the motion's Train button is after the keyframe timeline
  $("s-jump").nextElementSibling.textContent = motion ? "Bewegungsfehler (Grad)" : scenario ? t("Ziele erreicht (Prüfläufe)", "Goals reached (evaluation)") :
    t("echte Sprünge (sauberer Roboter)", "Real jumps (clean robot)");
  for (const id of ["s-height", "s-flight"]) $(id).parentElement.hidden = scenario || motion;
  $("s-jump").textContent = "–";
});
setScenario(DEFAULT_SCENARIO);
$("task").dispatchEvent(new Event("change"));
$("scExample").onclick = () => setScenario(DEFAULT_SCENARIO);
$("scApply").onclick = () => {
  try { setScenario(JSON.parse($("scJson").value)); status(t("Szenario geladen.", "Scenario loaded.")); }
  catch (error) { status(error.message, true); }
};
$("scSave").onclick = () => {
  try {
    const cfg = scenarioConfig();
    $("scJson").value = JSON.stringify(cfg, null, 2);
    const blob = new Blob([$("scJson").value], { type: "application/json" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a"); a.href = url; a.download = "kern-szenario.json"; a.click();
    setTimeout(() => URL.revokeObjectURL(url), 1000);
  } catch (error) { status(error.message, true); }
};
$("scFile").onchange = async () => {
  const file = $("scFile").files?.[0];
  if (!file) return;
  try {
    if (file.size > 16384) throw Error("Datei zu groß (maximal 16 KiB)");
    setScenario(JSON.parse(await file.text()));
    status(t("Szenario importiert.", "Scenario imported."));
  } catch (error) { status(error.message, true); }
  $("scFile").value = "";
};

async function start() {
  if (running) return;
  const task = $("task").value;
  let scenario = null;
  try { if (task === "scenario") scenario = scenarioConfig();
    if (task === "motion") scenario = motionSpec(); }
  catch (error) { status(error.message, true); return; }
  const taskName = scenario ? (task === "motion" ? "motion:" : "scenario:") + JSON.stringify(scenario) : task;
  const workers = Math.max(1, Math.min(64, Number($("workers").value) || 1));
  const envs = Math.max(1, Math.min(256, Number($("envs").value) || 8));
  running = true;
  paused = false;
  iteration = 0;
  history = [];
  onnx = null;
  latestMotion = null;
  $("motionTest").disabled = true;
  $("motionTrain").disabled = true;
  $("start").disabled = true;
  $("pause").disabled = false;
  $("download").disabled = true;
  status(t(`Starte ${workers} Worker mit je ${envs} Enten ...`, `Starting ${workers} ${workers === 1 ? "worker" : "workers"} with ${envs} ${envs === 1 ? "duck" : "ducks"} each ...`));
  try {
    wakeLock = await navigator.wakeLock?.request("screen").catch(() => null);
    const seed = Math.max(1, Number($("seed").value) || 1);
    learner = await spawn();
    collectors = await Promise.all(Array.from({ length: workers }, () => spawn()));
    // Worker i steps ducks i*envs .. (i+1)*envs-1 of one run: the same run as natively with as many
    // ducks in all (bit for bit, docs/lehrer.md).
    await Promise.all(collectors.map((w, i) => call(w, { type: "collector", task: taskName, envs, seed, first: i * envs }, "ready").then(d => { if (!d.ok) throw Error("Szenario-Validierung fehlgeschlagen"); return d; })));
    const config = {};
    if (Number($("initstd").value) > 0) config.initStd = Number($("initstd").value);
    if (Number($("lr").value) > 0) config.learningRate = Number($("lr").value);
    const made = await call(learner, { type: "learner", task: taskName, seed, config }, "policy");
    policy = made.policy;
    steps = made.steps || 24;
    // First imitation of the demonstration (the jump's recipe), round by round, then practice.
    let left = made.imitationLeft || 0;
    while (running && left > 0) {
      const round = made.imitationLeft - left;
      status(t(`Nachahmen: Runde ${round + 1} von ${made.imitationLeft} ...`, `Imitating: round ${round + 1} of ${made.imitationLeft} ...`));
      // The round's episodes spread over the collectors (worker i: its share, in episode order).
      const episodes = made.imitationEpisodes, k = collectors.length;
      const shares = await Promise.all(collectors.map((w, i) => {
        const first = Math.floor(i * episodes / k), count = Math.floor((i + 1) * episodes / k) - first;
        return call(w, { type: "dagger", policy: policy.slice(0), round, first, count }, "share").then(d => d.share);
      }));
      const d = await call(learner, { type: "imitate", shares }, "imitated", shares);
      policy = d.policy;
      left = d.left;
      const r = d.report;
      showJump(r);
      status(t(`Nachahmen: Runde ${r.round} von ${r.rounds}, ${r.samples.toLocaleString()} Beispiele, Fehler ${r.error.toFixed(4)}`,
        `Imitating: round ${r.round} of ${r.rounds}, ${r.samples.toLocaleString()} samples, error ${r.error.toFixed(4)}`));
    }
    if (made.imitationLeft && running) await exportOnnx(task);
    loop(task).catch(error => { status(error.message, true); stop(); });
  } catch (error) {
    status(error.message, true);
    stop();
  }
}

async function loop(task) {
  const total = Math.max(1, Number($("iterations").value) || 1);
  while (running && iteration < total) {
    if (paused) { await new Promise(r => setTimeout(r, 200)); continue; }
    const t0 = performance.now();
    const rollouts = await Promise.all(collectors.map(w =>
      call(w, { type: "collect", policy: policy.slice(0), iteration: iteration + 1, steps }, "rollout").then(d => d.rollout)));
    const learned = await call(learner, { type: "learn", rollouts }, "learned", rollouts);
    policy = learned.policy;
    iteration = learned.stats.iteration;
    const seconds = (performance.now() - t0) / 1000;
    show(learned.stats, seconds, collectors.length * Number($("envs").value) * steps);
    if (iteration % 25 === 0 || iteration === total) {
      if (task === "jump" || task === "scenario" || task === "motion") {
        const result = (await call(learner, { type: "evaluate", episodes: 5 }, "evaluated")).report;
        if (task === "jump") showJump(result);
        else if (task === "motion") showMotion(result);
        else showScenario(result);
      }
      await exportOnnx(task);
    }
  }
  if (running) {
    status(t(`Fertig: ${iteration} Iterationen. Das Netz herunterladen oder direkt in Kern Studio nutzen.`,
      `Done: ${iteration} iterations. Download the network or use it directly in Kern Studio.`));
    await exportOnnx(task);
    stop(true);
  }
}

async function exportOnnx(task) {
  const d = await call(learner, { type: "export" }, "onnx");
  onnx = new Blob([d.onnx], { type: "application/octet-stream" });
  onnx.task = task === "motion" ? motionSlug() : task === "scenario" ? "scenario_" + $("scName").value.replace(/[^a-z0-9_-]/gi, "_").slice(0, 48) : task;
  $("download").disabled = false;
  if (task === "motion") {
    const meta = motionMeta();
    try {
      const skills = JSON.parse(localStorage.getItem("kern-lehrer.motion-skills.v1") || "{}");
      skills[meta.file] = meta;
      localStorage.setItem("kern-lehrer.motion-skills.v1", JSON.stringify(skills));
    } catch (error) { status("Die Fähigkeiten-Metadaten konnten nicht gespeichert werden: " + error.message, true); return; }
    await saveForStudio(meta.file, d.onnx);
    latestMotion = meta;
    $("motionTest").disabled = false;
  } else if (task === "scenario") {
    $("saved").textContent = t("Szenario-Modell als ONNX bereit. Für die Nutzung im Studio ist eine passende Kommando-Integration nötig.",
      "Scenario ONNX ready. Studio use needs matching command integration.");
  } else {
    await saveForStudio(`${task}.onnx`, d.onnx).catch(() => {});
  }
}

/** Kern Studio on this device picks the network up (IndexedDB "kern-lehrer", key = file name), at
 *  once in open tabs (BroadcastChannel): the duck's "jump" move runs jump.onnx. */
function saveForStudio(file, bytes) {
  return new Promise((resolve, reject) => {
    const open = indexedDB.open("kern-lehrer", 1);
    open.onupgradeneeded = () => open.result.createObjectStore("policies");
    open.onerror = () => reject(open.error);
    open.onsuccess = () => {
      const tx = open.result.transaction("policies", "readwrite");
      tx.objectStore("policies").put(bytes, file);
      tx.oncomplete = () => {
        try { new BroadcastChannel("kern-lehrer").postMessage({ file }); } catch { /* no channel */ }
        $("saved").textContent = t(`Für Kern Studio gespeichert als ${file} (Iteration ${iteration}).`, `Saved for Kern Studio as ${file} (iteration ${iteration}).`);
        resolve();
      };
      tx.onerror = () => reject(tx.error);
    };
  });
}

function show(s, seconds, steps) {
  $("s-it").textContent = s.iteration;
  $("s-steps").textContent = Math.round(s.steps).toLocaleString();
  $("s-sps").textContent = Math.round(steps / seconds).toLocaleString();
  if (s.episodes) {
    $("s-ret").textContent = s.meanReturn.toFixed(2);
    $("s-len").textContent = (s.meanLength * 0.02).toFixed(2);
    history.push({ it: s.iteration, ret: s.meanReturn, len: s.meanLength * 0.02 });
  }
  $("s-std").textContent = s.meanStd.toFixed(3);
  status(t(`Lernt: Iteration ${s.iteration}, sammeln ${s.collectSeconds.toFixed(2)} s, lernen ${s.learnSeconds.toFixed(2)} s`,
    `Learning: iteration ${s.iteration}, collecting ${s.collectSeconds.toFixed(2)} s, learning ${s.learnSeconds.toFixed(2)} s`));
  draw();
}

/** How the network jumps now (5 episodes on the clean robot). */
function showJump(r) {
  if (!r) return;
  $("s-jump").textContent = `${r.jumps}/${r.episodes}`;
  $("s-height").textContent = r.heightMm.toFixed(1);
  $("s-flight").textContent = Math.round(r.flightS * 1000);
}

function showMotion(r) {
  if (!r) return;
  $("s-jump").textContent = Number.isFinite(r.meanErrorDeg) ? r.meanErrorDeg.toFixed(1) + "°" : "–";
}
function showScenario(r) {
  if (!r) return;
  $("s-jump").textContent = `${r.successes}/${r.episodes}`;
}
function draw() {
  const c = $("chart"), dpr = devicePixelRatio || 1;
  const w = c.clientWidth, h = c.clientHeight;
  c.width = w * dpr;
  c.height = h * dpr;
  const g = c.getContext("2d");
  g.scale(dpr, dpr);
  g.clearRect(0, 0, w, h);
  if (history.length < 2) return;
  const pad = 28;
  const css = getComputedStyle(document.documentElement);
  const line = (key, color) => {
    const v = history.map(p => p[key]);
    const lo = Math.min(...v), hi = Math.max(...v), span = hi - lo || 1;
    g.strokeStyle = color;
    g.lineWidth = 2;
    g.beginPath();
    history.forEach((p, i) => {
      const x = pad + (w - 2 * pad) * (i / (history.length - 1));
      const y = h - pad - (h - 2 * pad) * ((p[key] - lo) / span);
      i ? g.lineTo(x, y) : g.moveTo(x, y);
    });
    g.stroke();
    return [lo, hi];
  };
  g.strokeStyle = css.getPropertyValue("--line2");
  g.lineWidth = 1;
  g.strokeRect(pad, pad, w - 2 * pad, h - 2 * pad);
  const [lo, hi] = line("ret", css.getPropertyValue("--play").trim());
  line("len", css.getPropertyValue("--accent").trim());
  g.fillStyle = css.getPropertyValue("--dim").trim();
  g.font = "11px system-ui";
  g.fillText(hi.toFixed(1), 4, pad + 4);
  g.fillText(lo.toFixed(1), 4, h - pad);
  g.fillText(`Iteration ${history[history.length - 1].it}`, w - pad - 80, h - 8);
}

function stop(done = false) {
  running = false;
  for (const w of [learner, ...collectors]) w?.terminate();
  learner = null;
  collectors = [];
  wakeLock?.release?.().catch(() => {});
  wakeLock = null;
  $("start").disabled = false;
  if ($("task").value === "motion") renderMotionFrames();
  $("pause").disabled = true;
  $("pause").textContent = "Pause";
  if (!done && !$("status").classList.contains("err")) status(t("Angehalten.", "Stopped."));
}

$("start").onclick = start;
$("pause").onclick = () => {
  paused = !paused;
  $("pause").textContent = paused ? t("Weiter", "Resume") : "Pause";
  status(paused ? t("Pausiert.", "Paused.") : t("Lernt ...", "Learning ..."));
};
$("download").onclick = () => {
  if (!onnx) return;
  const a = document.createElement("a");
  a.href = URL.createObjectURL(onnx);
  a.download = `${onnx.task || "kern"}_kern.onnx`;
  a.click();
  setTimeout(() => URL.revokeObjectURL(a.href), 1000);
};
addEventListener("resize", draw);

(async () => {
  try {
    module = await WebAssembly.compileStreaming(fetch(wasmUrl)).catch(async () => WebAssembly.compile(await (await fetch(wasmUrl)).arrayBuffer()));
    status(t(`Bereit. ${navigator.hardwareConcurrency || "?"} Kerne auf diesem Gerät.`, `Ready. ${navigator.hardwareConcurrency || "?"} cores on this device.`));
  } catch (error) {
    status(t(`Der Lehrer konnte nicht laden: ${error.message}`, `The Lehrer could not load: ${error.message}`), true);
    $("start").disabled = true;
  }
})();
