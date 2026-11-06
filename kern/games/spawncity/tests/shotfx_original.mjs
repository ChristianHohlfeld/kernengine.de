// Shot effects of the original (spawncity.de): fires every weapon with the page's clock slowed down
// (performance.now, requestAnimationFrame, setTimeout and CSS animations at a fraction of real time)
// and takes screenshots of muzzle flashes, tracers, impacts and blasts - the reference for
// SpawnCity on Kern's shot_fx.h and Kern's soldier art (compare with "fxslow" on the Kern side).
//
//   node kern/games/spawncity/tests/shotfx_original.mjs [out-dir] [original-url]
import {createRequire} from "node:module";
import fs from "node:fs";
import path from "node:path";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const out = path.resolve(process.argv[2] || "generated/shotfx-original");
const URL = process.argv[3] || "https://spawncity.de/drone_simulator.html";
fs.mkdirSync(out, {recursive: true});
const log = (...a) => console.log(new Date().toISOString().slice(11, 19), ...a);

// The page's own clock, slowed on demand: window.__slowmo(k).
const slowmo = () => {
  const realNow = performance.now.bind(performance);
  let k = 1, base = 0, vbase = 0;
  const vnow = () => vbase + (realNow() - base) * k;
  window.__slowmo = s => { vbase = vnow(); base = realNow(); k = s; for (const a of document.getAnimations?.() || []) a.playbackRate = s; };
  performance.now = vnow;
  const raf = window.requestAnimationFrame.bind(window);
  window.requestAnimationFrame = cb => raf(() => cb(vnow()));
  const st = window.setTimeout.bind(window);
  window.setTimeout = (f, d, ...a) => st(f, (Number(d) || 0) / k, ...a);
  // CSS animations started while slowed play slowed too
  const watch = () => new MutationObserver(() => { if (k !== 1) for (const a of document.getAnimations?.() || []) a.playbackRate = k; })
    .observe(document.documentElement, {subtree: true, attributes: true, attributeFilter: ["class"]});
  if (document.documentElement) watch(); else addEventListener("DOMContentLoaded", watch, {once: true});
};

const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist", "--autoplay-policy=no-user-gesture-required"]});
const ctx = await browser.newContext({viewport: {width: 960, height: 540}, geolocation: {latitude: 52.5208, longitude: 13.4094, accuracy: 5}, permissions: ["geolocation"]});
await ctx.addInitScript(slowmo);
const page = await ctx.newPage();
page.on("pageerror", e => log("pageerror", e.message));
await page.goto(URL);
// the page may start the game by itself (the menu hides, START stays "LOADING"); else press START
await page.waitForFunction(() => document.getElementById("gameMenuStart")?.textContent?.trim() === "START" || (document.getElementById("gameMenu")?.hidden && globalThis.__arondightFootWeapons), null, {timeout: 240000})
  .catch(async e => { await page.screenshot({path: path.join(out, "orig-boot-failed.png")}); log("boot", await page.evaluate(() => document.getElementById("gameMenuStart")?.outerHTML || document.title)); throw e; });
if (await page.evaluate(() => document.getElementById("gameMenuStart")?.textContent?.trim() === "START" && !document.getElementById("gameMenu")?.hidden)) {
  await page.click("#gameMenuStart");
  await page.waitForFunction(() => document.getElementById("gameMenu")?.hidden === true, null, {timeout: 60000});
}
await page.waitForFunction(() => globalThis.__arondightFootWeapons && globalThis.__arondightWalkMode, null, {timeout: 120000});
await page.waitForFunction(() => Number(document.querySelector("#viewport")?.dataset.worldCityBuildings) > 20, null, {timeout: 90000}).catch(() => {});
const frames = async n => { for (let i = 0; i < n; i++) await page.evaluate(() => new Promise(r => requestAnimationFrame(() => r()))); };
const shot = async name => { await page.screenshot({path: path.join(out, `${name}.png`), timeout: 600000}); log("shot", name); };
const slow = k => page.evaluate(k => window.__slowmo(k), k);
const vp = await page.evaluate(() => { const r = document.getElementById("viewport").getBoundingClientRect(); return {x: r.x, y: r.y, w: r.width, h: r.height}; });
const aim = {x: vp.x + vp.w * 0.5, y: vp.y + vp.h * 0.62};
await frames(30);

const steps = (process.env.STEPS || "glock,smg,sniper,grenade,rocket,nade,drone,dronerocket").split(",");
for (const s of steps) {
  try {
    if (["glock", "smg", "sniper", "grenade", "rocket"].includes(s)) {
      log(s, await page.evaluate(m => { globalThis.__arondightWalkMode?.setMode?.("foot", {persist: false, reason: "shotfx"}); return globalThis.__arondightFootWeapons?.setMode?.(m) ?? globalThis.__arondightFootWeapons?.mode; }, s));
      await frames(10);
      if (s === "grenade" || s === "rocket") {
        await page.evaluate(a => globalThis.__arondightFootWeapons.fireAt({clientX: a.x, clientY: a.y, source: "shotfx"}), aim);
        await slow(0.05); await frames(1); await shot(`orig-${s}-launch`); await slow(1);
        for (let i = 0; i < 60; i++) { await frames(1); const n = await page.evaluate(() => globalThis.__arondightFootWeapons.projectiles().length); if (!n) break; }
        await slow(0.05); await frames(1); await shot(`orig-${s}-a`); await frames(3); await shot(`orig-${s}-b`); await slow(1);
      } else {
        await slow(0.02);
        await page.evaluate(a => globalThis.__arondightFootWeapons.fireAt({clientX: a.x, clientY: a.y, source: "shotfx"}), aim);
        await frames(1); await shot(`orig-${s}-a`); await frames(3); await shot(`orig-${s}-b`);
        await slow(1); await frames(6); await shot(`orig-${s}-after`);
      }
    } else if (s === "nade") {
      await page.evaluate(() => { globalThis.__arondightHandGrenade?.pull?.("shotfx"); });
      await frames(3);
      await page.evaluate(() => globalThis.__arondightHandGrenade?.release?.());
      for (let i = 0; i < 400; i++) { await frames(1); const n = await page.evaluate(() => globalThis.__arondightFootWeapons.projectiles().length); if (!n) break; }
      await slow(0.05); await frames(1); await shot("orig-nade-a"); await frames(3); await shot("orig-nade-b"); await slow(1);
    } else if (s === "drone" || s === "dronerocket") {
      log(s, await page.evaluate(() => globalThis.__arondightWalkMode?.setMode?.("drone", {persist: false, reason: "shotfx"})));
      await frames(30);
      if (s === "drone") {
        await page.evaluate(() => globalThis.__arondightDroneWeapons?.setMode?.("gun"));
        await slow(0.02);
        await page.mouse.move(aim.x, aim.y); await page.mouse.down(); await frames(1); await page.mouse.up();
        await frames(1); await shot("orig-drone-a"); await frames(3); await shot("orig-drone-b"); await slow(1);
      } else {
        await page.evaluate(() => globalThis.__arondightDroneWeapons?.setMode?.("missile"));
        await frames(3);
        log("missile", await page.evaluate(a => globalThis.__arondightDroneWeapons?.fireMissile?.({clientX: a.x, clientY: a.y, source: "shotfx"}), aim));
        for (let i = 0; i < 200; i++) { await frames(1); const n = Number(await page.evaluate(() => document.getElementById("viewport").dataset.droneActiveMissiles || 0)); if (!n) break; }
        await slow(0.05); await frames(1); await shot("orig-dronerocket-a"); await frames(3); await shot("orig-dronerocket-b"); await slow(1);
      }
    }
  } catch (e) { log(s, "failed", e.message); }
}
await browser.close();
