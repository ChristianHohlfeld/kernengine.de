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

async function start() {
  if (running) return;
  const task = $("task").value;
  const workers = Math.max(1, Math.min(64, Number($("workers").value) || 1));
  const envs = Math.max(1, Math.min(256, Number($("envs").value) || 8));
  running = true;
  paused = false;
  iteration = 0;
  history = [];
  onnx = null;
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
    await Promise.all(collectors.map((w, i) => call(w, { type: "collector", task, envs, seed, first: i * envs }, "ready")));
    const config = {};
    if (Number($("initstd").value) > 0) config.initStd = Number($("initstd").value);
    if (Number($("lr").value) > 0) config.learningRate = Number($("lr").value);
    const made = await call(learner, { type: "learner", task, seed, config }, "policy");
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
      if (task === "jump") showJump((await call(learner, { type: "evaluate", episodes: 5 }, "evaluated")).report);
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
  onnx.task = task;
  $("download").disabled = false;
  await saveForStudio(`${task}.onnx`, d.onnx).catch(() => {});
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
