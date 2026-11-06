// Where a frame's time goes - Kern and the original, same machine, same steps (CI: no GPU, so the
// GPU raster is SwiftShader on the CPU; the JS / wasm main-thread profile is what counts here).
//   node profile.mjs <url> <out dir> <label> [mode ...]
// Per mode: frame times (p50 / p95 / p99 / max), draw calls and WebGL calls per frame, and a CPU
// profile of the main thread (self time per function, wasm functions by name) -> <label>.md
import fs from "node:fs";
import path from "node:path";
import {chromium} from "playwright";

const [url, outDir, label = "page", ...modes] = process.argv.slice(2);
fs.mkdirSync(outDir, {recursive: true});
const out = [];
const log = s => { console.log(s); out.push(s); };
const b = await chromium.launch({args: ["--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist"]});
const ctx = await b.newContext({viewport: {width: 1280, height: 720}, geolocation: {latitude: 52.5200, longitude: 13.4050}, permissions: ["geolocation"]});
const p = await ctx.newPage();
p.on("pageerror", e => log(`pageerror ${e.message}`));
p.on("crash", () => log("CRASH"));
await p.addInitScript(() => {
  // every WebGL call: count and time per name, per frame; the long frames keep their slowest calls
  const st = window.__gl = {draws: 0, calls: 0, frames: 0, time: {}, count: {}, cur: {}, long: []};
  const proto = WebGL2RenderingContext.prototype;
  for (const k of Object.getOwnPropertyNames(proto)) {
    const d = Object.getOwnPropertyDescriptor(proto, k);
    if (typeof d.value !== "function") continue;
    const o = d.value, draw = k.startsWith("draw");
    proto[k] = function (...a) {
      if (draw) st.draws++;
      st.calls++;
      const t = performance.now(), r = o.apply(this, a), dt = performance.now() - t;
      st.time[k] = (st.time[k] || 0) + dt; st.count[k] = (st.count[k] || 0) + 1; st.cur[k] = (st.cur[k] || 0) + dt;
      return r;
    };
  }
  const ft = window.__ft = [];
  let last = 0;
  const loop = t => {
    if (last) {
      const d = t - last;
      ft.push(d);
      if (d > 150) st.long.push({ms: Math.round(d), gl: Object.entries(st.cur).sort((a, c) => c[1] - a[1]).slice(0, 5).map(([k, v]) => `${k} ${v.toFixed(0)}`).join(", ")});
    }
    st.cur = {};
    last = t; st.frames++; requestAnimationFrame(loop);
  };
  requestAnimationFrame(loop);
});
const t0 = Date.now();
await p.goto(url, {waitUntil: "domcontentloaded", timeout: 120000});
await p.waitForTimeout(6000);
const kern = await p.evaluate(() => !!window.SpawnCity);
log(`# ${label}\n\nurl ${url} · kern ${kern}`);
// START: the original's title button (both pages have it)
await p.evaluate(() => (document.getElementById("gameMenuStart") || document.getElementById("scStart"))?.click());
if (kern) await p.waitForFunction(() => window.SpawnCity.hud()?.placed, null, {timeout: 180000}).catch(() => log("never placed"));
else await p.waitForTimeout(15000);
log(`started after ${((Date.now() - t0) / 1000).toFixed(1)} s`);
await p.waitForTimeout(8000);
const cdp = await ctx.newCDPSession(p);
await cdp.send("Profiler.enable");
await cdp.send("Profiler.setSamplingInterval", {interval: 250});
for (const mode of modes.length ? modes : ["foot"]) {
  if (kern && mode !== "foot") await p.evaluate(m => window.SpawnCity.command?.(`mode ${m}`), mode);
  if (!kern && mode !== "foot") await p.keyboard.press(mode === "drone" ? "v" : "f").catch(() => {});
  await p.waitForTimeout(5000);
  await p.evaluate(() => { window.__ft.length = 0; Object.assign(window.__gl, {draws: 0, calls: 0, frames: 0, time: {}, count: {}, long: []}); });
  await cdp.send("Profiler.start");
  await p.waitForTimeout(10000);
  const {profile} = await cdp.send("Profiler.stop");
  const r = await p.evaluate(() => ({ft: window.__ft.slice(), gl: {...window.__gl}, hud: window.SpawnCity?.hud?.() || null}));
  fs.writeFileSync(path.join(outDir, `${label}-${mode}.cpuprofile`), JSON.stringify(profile));
  const ft = r.ft.sort((a, c) => a - c), q = x => ft.length ? ft[Math.floor(x * (ft.length - 1))].toFixed(1) : "-";
  log(`\n## ${mode}\n\nframes ${ft.length} in 10 s · frame ms p50 ${q(.5)} · p95 ${q(.95)} · p99 ${q(.99)} · max ${q(1)} · draws/frame ${(r.gl.draws / Math.max(1, r.gl.frames)).toFixed(0)} · GL calls/frame ${(r.gl.calls / Math.max(1, r.gl.frames)).toFixed(0)}`);
  log(`GL time per call name (ms in 10 s, calls): ${Object.entries(r.gl.time).sort((x, y) => y[1] - x[1]).slice(0, 12).map(([k, v]) => `${k} ${v.toFixed(0)} (${r.gl.count[k]})`).join(" · ")}`);
  log(`long frames (> 150 ms) and their slowest GL calls: ${r.gl.long.slice(0, 12).map(l => `${l.ms} ms [${l.gl}]`).join(" | ") || "none"}`);
  if (r.hud) log(`hud mode ${r.hud.mode} fps ${r.hud.fps} tiles ${JSON.stringify(r.hud.tiles?.t || {})} perf ${JSON.stringify(r.hud.perf || r.hud.look?.perf || {})}`);
  // self time per function
  const nodes = new Map(profile.nodes.map(n => [n.id, n]));
  const self = new Map();
  let total = 0;
  profile.samples.forEach((id, i) => {
    const cf = nodes.get(id).callFrame, dt = profile.timeDeltas[i] || 0;
    const key = `${cf.functionName || "(anonymous)"} · ${(cf.url || "").split("/").pop().slice(0, 40)}`;
    self.set(key, (self.get(key) || 0) + dt);
    total += dt;
  });
  log(`\n| self % | ms/s | function |\n|---:|---:|---|`);
  for (const [k, v] of [...self].sort((a, c) => c[1] - a[1]).slice(0, 45)) log(`| ${(v / total * 100).toFixed(1)} | ${(v / 10000).toFixed(1)} | ${k} |`);
}
fs.writeFileSync(path.join(outDir, `${label}.md`), out.join("\n") + "\n");
await p.screenshot({path: path.join(outDir, `${label}.png`)});
await b.close();
