// Desktop mouse check of SpawnCity on Kern: aim with the right button and fire with the left, in
// both orders, with a real mouse (Playwright + Chromium).
//   node mouse_check.mjs <url>      e.g. http://127.0.0.1:8765/drone_simulator.html?offline=1&autostart=1
//
// What it guards (kern/web/app.mjs): the browser reports a second mouse button pressed while one
// is held as a pointermove that names it (chorded buttons), and its own synthetic moves do not
// carry the held buttons - neither may let go of the aiming right button or swallow the shot.
import {createRequire} from "node:module";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const url = process.argv[2];
if (!url) { console.error("usage: node mouse_check.mjs <url>"); process.exit(2); }
let failures = 0;
const check = (ok, what) => { if (!ok) failures++; console.log(ok ? "ok  " : "FAIL", what); };
const b = await chromium.launch({args: ["--use-gl=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist"]});
const p = await (await b.newContext({viewport: {width: 900, height: 420}})).newPage();
const errors = [];
p.on("pageerror", e => errors.push(e.message));
await p.goto(url, {waitUntil: "domcontentloaded", timeout: 120000});
const hud = () => p.evaluate(() => { try { return window.SpawnCity.hud(); } catch { return null; } });
/** Polls until `fn(hud)` holds (software WebGL draws about two frames a second). */
const until = async (fn, ms = 60000) => {
  const end = Date.now() + ms;
  for (;;) {
    const h = await hud();
    if (h && fn(h)) return true;
    if (Date.now() > end) return false;
    await p.waitForTimeout(700);
  }
};
check(await until(h => h.placed, 180000), "the world is placed");
await p.mouse.move(450, 260);
await p.mouse.down();  // the capture click: never fires
await p.mouse.up();
check(await until(h => h.ctl.locked), "a click captures the mouse");

await p.mouse.down({button: "right"});
check(await until(h => h.ctl.rmb && h.aiming), "the right button aims");
await p.mouse.down();
check(await until(h => h.ctl.lmb && h.ctl.trigger && h.aiming), "the left button fires while the right one aims");
await p.waitForTimeout(3000);
check(await until(h => h.ctl.rmb && h.aiming && h.ctl.trigger, 1), "both stay held (no synthetic move lets go of them)");
await p.mouse.up();
check(await until(h => !h.ctl.trigger && h.aiming), "letting go of the left button keeps aiming");
await p.mouse.up({button: "right"});
check(await until(h => !h.aiming && !h.ctl.rmb), "letting go of the right button stops aiming");

await p.mouse.down();
check(await until(h => h.ctl.trigger && !h.aiming), "the left button fires from the hip");
await p.mouse.down({button: "right"});
check(await until(h => h.ctl.trigger && h.aiming), "the right button aims while the left one fires");
await p.mouse.up({button: "right"});
await p.mouse.up();
check(await until(h => !h.ctl.trigger && !h.aiming && !h.ctl.lmb && !h.ctl.rmb), "nothing is held afterwards");
check(errors.length === 0, "no page errors" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
await b.close();
console.log(failures ? `${failures} FAILED` : "mouse check passed");
process.exit(failures ? 1 : 0);
