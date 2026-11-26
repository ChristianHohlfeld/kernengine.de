// Input check of SpawnCity on Kern (the small game: a drone over the city) with a real mouse and keyboard, a synthetic
// gamepad and a synthetic finger (Playwright + Chromium): the click captures the mouse; R arms the drone, Space climbs;
// the locked mouse turns it (the right stick's yaw); the pad is the device the game reads (its Y the next camera); a
// finger switches to the touch layout and is let go when the browser's own list of touches is empty.
//   node input_check.mjs <url>      e.g. http://127.0.0.1:8765/drone_simulator.html?offline=1&autostart=1
//
// What it guards (kern/web/kern.mjs BrowserInput and readGamepad, the one input reader of every Kern host, by way of
// kern/web/app.mjs): the locked pointer's movement and the lock itself reach the game; so do the keys, the device in
// use, the gamepad and the fingers' ground truth. (Until 26.11.2026 it also aimed and fired on foot and opened the big
// map: out of the small game with them - BLUEPRINTS.md, "Herausgenommen am 26.11.".)
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

// the drone: R arms it once the flight controller has calibrated, Space climbs (the keys reach the game)
check(await until(h => h.drone && !h.drone.calibrating, 60000), "the flight controller has calibrated");
await p.keyboard.press("KeyR");
check(await until(h => h.drone?.armed, 30000), "R arms the drone");
const alt0 = (await hud()).alt;
await p.keyboard.down("Space");
const climbed = await until(h => h.alt > alt0 + 1.5, 30000);
await p.keyboard.up("Space");
check(climbed, "holding Space climbs");
// the locked mouse turns it (headless Chromium reports a locked pointer's moves as a jump and its way back - no net
// movement -, so the moves are given to the document as the events a real mouse sends)
const yaw = () => p.evaluate(() => { const c = JSON.parse(window.SpawnCity.command("camera")); return Math.atan2(c.target[1] - c.eye[1], c.target[0] - c.eye[0]); });
const y0 = await yaw();
let turned = false;
for (let i = 0; i < 40 && !turned; ++i) {
  await p.evaluate(() => document.dispatchEvent(new MouseEvent("mousemove", {movementX: 60, movementY: 0, bubbles: true})));
  await p.waitForTimeout(150);
  turned = Math.abs(Math.atan2(Math.sin(await yaw() - y0), Math.cos(await yaw() - y0))) > 0.2;
}
check(turned, "moving the locked mouse turns the drone");

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
const cam = (await hud()).drone.cam;
await pad(3, true);
check(await until(h => h.ctl.pad && h.drone.cam !== cam), "the pad's Y: the next camera, and the pad is the device the game reads");
await pad(3, false);
await p.evaluate(() => { navigator.getGamepads = () => []; });
check(await until(h => !h.ctl.pad), "the pad gone: the game reads none");

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
