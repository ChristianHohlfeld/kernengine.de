// Desktop input check of SpawnCity on Kern with a real mouse and keyboard (Playwright + Chromium):
// aim with the right button and fire with the left, in both orders; the locked mouse looks around;
// the keyboard walks.
//   node mouse_check.mjs <url>      e.g. http://127.0.0.1:8765/drone_simulator.html?offline=1&autostart=1
//
// What it guards (kern/web/kern.mjs BrowserInput, the one input reader of every Kern host, by way
// of kern/web/app.mjs): the browser reports a second mouse button pressed while one is held as a
// pointermove that names it (chorded buttons), and its own synthetic moves do not carry the held
// buttons - neither may let go of the aiming right button or swallow the shot; the locked
// pointer's movement and the lock itself reach the game; so do the keys.
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

// the locked mouse looks around (the document's mousemove under the lock), the keyboard walks
// (headless Chromium reports a locked pointer's moves as a jump and its way back - no net
// movement -, so the moves are given to the document as the events a real mouse sends)
const yaw = () => p.evaluate(() => { const c = JSON.parse(window.SpawnCity.command("camera")); return Math.atan2(c.target[1] - c.eye[1], c.target[0] - c.eye[0]); });
const yaw0 = await yaw();
await p.evaluate(() => { for (let i = 0; i < 12; ++i) document.dispatchEvent(new MouseEvent("mousemove", {movementX: 25, movementY: 0, bubbles: true})); });
let turned = false;
for (let i = 0; i < 20 && !turned; ++i) { await p.waitForTimeout(700); turned = Math.abs(await yaw() - yaw0) > 0.05; }
check(turned, "moving the locked mouse turns the view");
const eye = () => p.evaluate(() => JSON.parse(window.SpawnCity.command("camera")).eye);
const from = await eye();
await p.keyboard.down("KeyW");
let walked = false;
for (let i = 0; i < 40 && !walked; ++i) { await p.waitForTimeout(700); const to = await eye(); walked = Math.hypot(to[0] - from[0], to[1] - from[1]) > 0.5; }
await p.keyboard.up("KeyW");
check(walked, "holding W walks");
await p.waitForTimeout(1500);
const still = await eye();
await p.waitForTimeout(2500);
const later = await eye();
check(Math.hypot(later[0] - still[0], later[1] - still[1]) < 0.3, "letting go of W stops");

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
