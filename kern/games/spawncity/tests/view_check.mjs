// Quick check of SpawnCity's original view (?view=original) on a deployed page: page errors, the
// title, START, a few modes - with the state of the page after each step and a screenshot.
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
  p.on("console", m => { if (m.type() === "error") errs.push("console: " + m.text().slice(0, 300)); });
  const state = () => p.evaluate(() => { const h = window.SpawnCity?.hud?.(); return JSON.stringify({mode: h?.mode, placed: h?.placed, agl: h?.agl, alt: h?.alt, fps: h?.fps, tiles: h?.tiles, playing: window.SpawnCity?.ui?.playing,
    scStart: document.getElementById("scStart")?.disabled, menu: document.getElementById("gameMenu")?.hidden, body: document.body.className}); }).catch(e => "state error " + e.message);
  const shot = async n => { await p.screenshot({path: path.join(out, `${dev}-${n}.png`)}).catch(() => {}); console.log(dev, n, await state()); };
  try {
    await p.goto(url, {timeout: 90000});
    const t0 = Date.now();
    await p.waitForFunction(() => window.SpawnCity?.hud?.()?.placed, null, {timeout: 180000, polling: 1000}).catch(async e => {
      console.log(dev, "never placed:", await p.evaluate(() => JSON.stringify(window.SpawnCity?.hud?.()?.tiles)).catch(() => "?"));
      throw e;
    });
    console.log(dev, "placed after", Date.now() - t0, "ms", await p.evaluate(() => JSON.stringify(window.SpawnCity.hud().tiles)));
    await shot("title");
    await p.evaluate(() => document.getElementById("gameMenuStart").click());
    await p.waitForTimeout(6000);
    await shot("started");
    // where the player stands right after the start (the original: on the ground)
    for (let i = 0; i < 6; i++) { console.log(dev, "after start", i, await p.evaluate(() => { const h = window.SpawnCity.hud(); let m = {}; try { m = JSON.parse(window.SpawnCity.command("minimap")).me; } catch {} return JSON.stringify({agl: h.agl, alt: h.alt, me: m, toast: h.toast, placed: h.placed}); })); await p.waitForTimeout(2000); }
    for (const m of ["foot", "drone", "car"]) { console.log(dev, m, await p.evaluate(m => window.SpawnCity.command(`mode ${m}`), m)); await p.waitForTimeout(3000); await shot(m); }
  } catch (e) { fails++; console.log("FAIL", dev, e.message.split("\n")[0]); await shot("fail"); }
  console.log(dev, "errors:", JSON.stringify(errs, null, 1));
  if (errs.length) fails++;
  await ctx.close();
}
await b.close();
process.exit(fails ? 1 : 0);
