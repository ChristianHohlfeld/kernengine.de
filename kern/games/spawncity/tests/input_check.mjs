// Input check of SpawnCity on Kern with a real mouse and keyboard, a synthetic gamepad and a
// synthetic finger (Playwright + Chromium): aim with the right button and fire with the left, in
// both orders; the locked mouse looks around; the keyboard walks; the pad the game reads is the
// pad the big map reads; a finger switches to the touch layout and is let go when the browser's
// own list of touches is empty.
//   node input_check.mjs <url>      e.g. http://127.0.0.1:8765/drone_simulator.html?offline=1&autostart=1
//
// What it guards (kern/web/kern.mjs BrowserInput and readGamepad, the one input reader of every
// Kern host, by way of kern/web/app.mjs): the browser reports a second mouse button pressed while
// one is held as a pointermove that names it (chorded buttons), and its own synthetic moves do
// not carry the held buttons - neither may let go of the aiming right button or swallow the shot;
// the locked pointer's movement and the lock itself reach the game; so do the keys, the device in
// use, the gamepad and the fingers' ground truth.
import {createRequire} from "node:module";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const url = process.argv[2];
if (!url) { console.error("usage: node input_check.mjs <url>"); process.exit(2); }
let failures = 0;
const check = (ok, what) => { if (!ok) failures++; console.log(ok ? "ok  " : "FAIL", what); };
// WebGL in software, the page itself composited by the CPU (with --use-angle=swiftshader the whole
// page is composited through the software GPU, and this page's HUD takes minutes a frame).
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
const began = Date.now();
const placed = await until(h => h.placed, 180000);
check(placed, `the world is placed (after ${((Date.now() - began) / 1000).toFixed(0)} s)`);
if (!placed) { await b.close(); console.log("1 FAILED"); process.exit(1); }  // nothing to drive
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

// ---- the device in use, the gamepad, the fingers -------------------------------------------------
check((await hud()).ctl.device === "desktop", "the mouse makes it a desktop");
// a synthetic Xbox pad (standard mapping), listed by the browser behind a device that is none:
// every reader takes the Xbox pad
await p.evaluate(() => {
  const make = (mapping, id) => ({connected: true, mapping, id, index: 0, axes: [0, 0, 0, 0], buttons: Array.from({length: 17}, () => ({pressed: false, value: 0}))});
  const other = make("", "USB Receiver"), pad = make("standard", "Xbox Wireless Controller (STANDARD GAMEPAD Vendor: 045e)");
  window.__padSet = (i, on) => { pad.buttons[i] = {pressed: on, value: on ? 1 : 0}; };
  navigator.getGamepads = () => [other, pad];
});
const pad = (i, on) => p.evaluate(([i, on]) => window.__padSet(i, on), [i, on]);
const weapon = (await hud()).weapon;
await pad(3, true);
check(await until(h => h.ctl.pad && h.weapon !== weapon), "the pad's Y: the next weapon, and the pad is the device in use");
await pad(3, false);
await p.waitForTimeout(1500);
await pad(8, true);
const mapOpen = async want => { for (let i = 0; i < 40; ++i) { if ((await p.locator("#mpMap").count() === 1) === want) return true; await p.waitForTimeout(700); } return false; };
check(await mapOpen(true), "the pad's VIEW held: the big map opens");
await pad(8, false);
await p.waitForTimeout(1500);
await pad(1, true);
check(await mapOpen(false), "the pad's B closes the map (the map reads the same pad)");
await pad(1, false);
await p.evaluate(() => { navigator.getGamepads = () => []; });
await p.keyboard.press("KeyZ");
check(await until(h => !h.ctl.pad), "a key ends the pad's turn");

// a finger: the touch layout (the finger that brings it up is not a control's yet); the next one on
// the left stick is held; nothing left on the glass: the game lets go
const finger = (type, id, x, y) => p.evaluate(([type, id, x, y]) => (document.elementFromPoint(x, y) || document.body).dispatchEvent(
  new PointerEvent(type, {pointerType: "touch", pointerId: id, isPrimary: true, clientX: x, clientY: y, bubbles: true, cancelable: true})), [type, id, x, y]);
await finger("pointerdown", 21, 450, 150);
await finger("pointerup", 21, 450, 150);
check(await until(h => h.ctl.device === "touch" && !h.ctl.fingers), "a finger: the touch layout");
await p.waitForTimeout(1500);
await finger("pointerdown", 22, 84, 336);  // the left stick's centre (168 px, bottom left)
check(await until(h => h.ctl.fingers), "a finger on the stick: the game holds it");
await p.evaluate(() => { const e = new Event("touchend", {bubbles: true}); Object.defineProperty(e, "touches", {value: []}); document.dispatchEvent(e); });
check(await until(h => !h.ctl.fingers), "no touch left on the glass: the game lets go of the finger");
await p.mouse.move(460, 270);
await p.mouse.down();
await p.mouse.up();
check(await until(h => h.ctl.device === "desktop"), "the mouse again: back to the desktop");
check(errors.length === 0, "no page errors" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
await b.close();
console.log(failures ? `${failures} FAILED` : "input check passed");
process.exit(failures ? 1 : 0);
