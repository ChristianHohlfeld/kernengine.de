// Quick check of SpawnCity's original view (?view=original) on a deployed page: page errors, the
// title, START, the drone after the start and its cameras - with the state of the page after each step and a
// screenshot. (Until 26.11.2026 it also switched to foot and car: out of the small game with them - BLUEPRINTS.md,
// "Herausgenommen am 26.11.".)
//   node kern/games/spawncity/tests/view_check.mjs <url> <out-dir>
import {createRequire} from "node:module";
import fs from "node:fs";
import path from "node:path";
const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const url = process.argv[2] || "https://kernengine.de/kern/games/spawncity/web/index.html?view=original&menu=1";
const out = path.resolve(process.argv[3] || "generated/spawncity-view");
fs.mkdirSync(out, {recursive: true});
const b = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader"]});
let fails = 0;
for (const [dev, opts] of [["desktop", {viewport: {width: 1280, height: 720}}], ["phone", {viewport: {width: 844, height: 390}, isMobile: true, hasTouch: true}]]) {
  const ctx = await b.newContext({...opts, geolocation: {latitude: 52.5208, longitude: 13.4094}, permissions: ["geolocation"]});
  const p = await ctx.newPage(), errs = [];
  p.on("pageerror", e => errs.push(String(e.stack || e).slice(0, 500)));
  p.on("console", m => { if (m.type() === "error" || m.type() === "warning") errs.push(`console ${m.type()}: ` + m.text().slice(0, 300)); });
  const state = () => p.evaluate(() => { const h = window.SpawnCity?.hud?.(); return JSON.stringify({mode: h?.mode, placed: h?.placed, agl: h?.agl, alt: h?.alt, fps: h?.fps, tiles: h?.tiles, playing: window.SpawnCity?.ui?.playing,
    scStart: document.getElementById("scStart")?.disabled, menu: document.getElementById("gameMenu")?.hidden, body: document.body.className}); }).catch(e => "state error " + e.message);
  const shot = async n => { await p.screenshot({path: path.join(out, `${dev}-${n}.png`)}).catch(() => {}); console.log(dev, n, await state()); };
  try {
    await p.goto(url, {timeout: 90000});
    const t0 = Date.now();
    await p.waitForFunction(() => window.SpawnCity?.hud?.()?.placed, null, {timeout: 180000, polling: 1000}).catch(async e => {
      console.log(dev, "never placed:", await p.evaluate(() => JSON.stringify(window.SpawnCity?.hud?.()?.tiles)).catch(() => "?"));
      console.log(dev, "terrain / jobs:", await p.evaluate(() => { const h = window.SpawnCity?.hud?.() || {}; return JSON.stringify({terrain: h.terrain, offline: h.offline, realData: h.realData, mode: h.mode, toast: h.toast, status: document.getElementById("status")?.textContent, mem: window.SpawnCity?.kern?.memory?.buffer?.byteLength}); }).catch(e => e.message));
      throw e;
    });
    console.log(dev, "placed after", Date.now() - t0, "ms", await p.evaluate(() => JSON.stringify(window.SpawnCity.hud().tiles)));
    await shot("title");
    await p.evaluate(() => document.getElementById("gameMenuStart").click());
    await p.waitForTimeout(6000);
    await shot("started");
    // where the drone stands right after the start (the original: on the ground)
    for (let i = 0; i < 6; i++) { console.log(dev, "after start", i, await p.evaluate(() => { const h = window.SpawnCity.hud(); return JSON.stringify({agl: h.agl, alt: h.alt, drone: window.SpawnCity.command("drone"), toast: h.toast, placed: h.placed, cam: window.SpawnCity.command("camera")}); })); await p.waitForTimeout(2000); }
    // the minimap (#worldLookHud): its box, and the big map after M
    const mini = () => p.evaluate(() => { const e = document.getElementById("worldLookHud"); if (!e) return "none"; const r = e.getBoundingClientRect(); return JSON.stringify({x: Math.round(r.x), y: Math.round(r.y), w: Math.round(r.width), h: Math.round(r.height), cls: e.className, display: getComputedStyle(e).display, vw: innerWidth, vh: innerHeight}); });
    console.log(dev, "minimap", await mini());
    console.log(dev, "city", await p.evaluate(() => JSON.stringify(window.SpawnCity.hud().tiles)));
    for (const c of ["cam fpv", "cam follow", "cam third"]) { console.log(dev, c, await p.evaluate(c => window.SpawnCity.command(c), c)); await p.waitForTimeout(3000); await shot(c.replace(" ", "-")); }
  } catch (e) { fails++; console.log("FAIL", dev, e.message.split("\n")[0]); await shot("fail"); }
  console.log(dev, "errors:", JSON.stringify(errs, null, 1));
  if (errs.length) fails++;
  await ctx.close();
}
await b.close();
process.exit(fails ? 1 : 0);
