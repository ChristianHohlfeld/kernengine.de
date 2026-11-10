// Browser test of the one input reader every Kern host uses (kern/web/kern.mjs BrowserInput), and
// of every host on it - with a real mouse and keyboard (Playwright + Chromium):
//   node input_e2e.mjs <base url of the repository root> [url of a SpawnCity page]
//
//  1. the contract (input_page.html): what a host is told, event by event - mouse buttons from
//     the button events alone (a second button while one is held comes as a move; a plain move
//     never lets go of a held one), positions, the wheel in pixels, the pointer lock, the locked
//     pointer's movement held to +-420 px, keys, focus, fingers and their ground truth, the
//     device in use, the gamepad in use;
//  2. the App host's examples (kern/examples/hello, orbrun): the keyboard, a drag, a button
//     pressed while another is held, the wheel and a finger reach the C++ app, in its units;
//  3. SpawnCity (when its url is given): tests/input_check.mjs of the game.
// The Studio and the packaged games are in studio_e2e.mjs.
import {createRequire} from "node:module";
import {spawn} from "node:child_process";
import readline from "node:readline";
import {fileURLToPath} from "node:url";
import path from "node:path";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const base = (process.argv[2] || "").replace(/\/$/, "");
if (!base) { console.error("usage: node input_e2e.mjs <base url> [spawncity url]"); process.exit(2); }
let failures = 0;
const check = (ok, what) => { if (!ok) failures++; console.log(ok ? "ok  " : "FAIL", what); };
const same = (a, b) => JSON.stringify(a) === JSON.stringify(b);
const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist"]});

// ---- 1. the contract -------------------------------------------------------------------------
{
  const page = await (await browser.newContext({viewport: {width: 800, height: 500}})).newPage();
  const errors = [];
  page.on("pageerror", e => errors.push(e.message));
  await page.goto(base + "/kern/tests/e2e/input_page.html");
  await page.waitForFunction(() => window.ready, null, {timeout: 30000});
  const take = () => page.evaluate(() => window.log.splice(0));
  const only = (log, kind) => log.filter(e => e[0] === kind);
  // the surface lies at (40, 30): page (140, 130) is surface (100, 100)
  await page.mouse.move(140, 130);
  await take();

  await page.keyboard.down("KeyW");
  await page.keyboard.up("KeyW");
  check(same(await take(), [["key", "KeyW", true, false], ["key", "KeyW", false, false]]), "a key: down and up, by its code");

  await page.mouse.down();
  let log = await take();
  check(same(log, [["down", "mouse"], ["button", 0, true, 1, 100, 100, false, "pointerdown"]]), "the left button: pressed, held mask 1, at the surface's (100, 100)");
  await page.mouse.move(160, 130);
  log = await take();
  check(only(log, "move").length >= 1 && only(log, "move").every(e => e[3] === 1) && same(only(log, "move").at(-1).slice(1, 3), [120, 100]), "a drag: moves that carry the held button");
  await page.mouse.up();
  check(same(only(await take(), "button"), [["button", 0, false, 0, 120, 100, false, "pointerup"]]), "the left button: let go, nothing held");

  // a second button while one is held comes as a pointermove (chorded buttons) - told as a button
  await page.mouse.down({button: "right"});
  await page.mouse.down({button: "left"});
  log = only(await take(), "button");
  check(same(log, [["button", 2, true, 2, 120, 100, false, "pointerdown"], ["button", 0, true, 3, 120, 100, false, "pointermove"]]), "right, then left while it is held: two presses, held mask 3");
  // the browser's own synthetic moves carry no buttons: nothing is let go
  await page.evaluate(() => document.getElementById("surface").dispatchEvent(new PointerEvent("pointermove", {pointerType: "mouse", pointerId: 1, button: -1, buttons: 0, clientX: 160, clientY: 130, bubbles: true})));
  log = await take();
  check(same(log, [["move", 120, 100, 3, false]]) && await page.evaluate(() => window.input.buttons) === 3, "a move that reports no buttons lets go of nothing");
  await page.mouse.up({button: "left"});
  await page.mouse.up({button: "right"});
  log = only(await take(), "button");
  check(same(log, [["button", 0, false, 2, 120, 100, false, "pointermove"], ["button", 2, false, 0, 120, 100, false, "pointerup"]]), "left let go while right is held, then right: two releases");

  // the wheel in pixels, whatever unit the browser used
  await page.mouse.wheel(0, 100);
  check(same(await take(), [["wheel", 100, 120, 100]]), "the wheel: 100 px");
  const prevented = await page.evaluate(() => { const s = document.getElementById("surface"); const e = new WheelEvent("wheel", {deltaY: 3, deltaMode: 1, clientX: 160, clientY: 130, bubbles: true, cancelable: true}); s.dispatchEvent(e); return e.defaultPrevented; });
  check(same(await take(), [["wheel", 120, 120, 100]]) && prevented, "the wheel in lines: 40 px a line, the page does not scroll");
  check(await page.evaluate(() => { const e = new MouseEvent("contextmenu", {bubbles: true, cancelable: true}); document.getElementById("surface").dispatchEvent(e); return e.defaultPrevented; }), "the surface's context menu does not open");

  // fingers go to the host as they are
  await page.evaluate(() => { const s = document.getElementById("surface"); for (const type of ["pointerdown", "pointermove", "pointerup"]) s.dispatchEvent(new PointerEvent(type, {pointerType: "touch", pointerId: 7, clientX: 200, clientY: 200, bubbles: true})); s.dispatchEvent(new PointerEvent("pointercancel", {pointerType: "touch", pointerId: 8, bubbles: true})); });
  check(same(await take(), [["down", "touch"], ["touch", 0, 7], ["touch", 1, 7], ["touch", 2, 7], ["touch", 3, 8]]), "a finger: down, move, up, cancel");

  // the pointer lock: asked for on a click, told when taken; the locked pointer's movement
  // (the browser tells of the lock after it changed it: wait for the telling, not for the state)
  const told = (kind, value) => page.waitForFunction(([kind, value]) => window.log.some(e => e[0] === kind && e[1] === value), [kind, value], {timeout: 15000}).catch(() => {});
  await page.evaluate(() => { window.lockOnClick = true; });
  await page.mouse.down();
  await page.mouse.up();
  await told("lock", true);
  log = await take();
  check(same(only(log, "lock"), [["lock", true, true]]) && await page.evaluate(() => window.input.locked), "a click takes the pointer lock");
  // (headless Chromium reports a locked pointer's moves as a jump and its way back, and may send
  // moves of its own once the lock is taken: the moves are given to the document as the events a
  // real mouse sends, and only those are counted)
  await page.evaluate(() => { for (const dx of [25, -10, 5000, -5000]) document.dispatchEvent(new MouseEvent("mousemove", {movementX: dx, movementY: dx / 5, bubbles: true})); });
  const looks = only(await take(), "look").filter(e => !e[3]).map(e => e.slice(0, 3));
  check(same(looks, [["look", 25, 5], ["look", -10, -2], ["look", 420, 420], ["look", -420, -420]]), "the locked pointer's movement, every event held to +-420 px" + ` (${JSON.stringify(looks)})`);
  await page.mouse.down({button: "right"});
  await page.mouse.down({button: "left"});
  log = only(await take(), "button");
  check(log.length === 2 && log[0][1] === 2 && log[0][6] === true && log[1][1] === 0 && log[1][3] === 3, "locked: right, then left while it is held");
  await page.mouse.up({button: "left"});
  await page.mouse.up({button: "right"});
  await take();
  await page.evaluate(() => window.input.unlock());
  await told("lock", false);
  check(same(only(await take(), "lock").map(e => e[1]), [false]) && !(await page.evaluate(() => window.input.locked)), "giving the lock back is told");

  // the device in use, seen anywhere in the window (the surface or not); a locked mouse says nothing
  const taken = () => page.evaluate(() => window.seen.splice(0));
  await taken();
  await page.mouse.move(700, 40);  // over the button beside the surface
  let seen = await taken();
  check(seen.length >= 1 && seen.every(e => same(e.slice(0, 2), ["device", "mousemove"])), "a moving mouse is told as the device in use");
  await page.mouse.down();
  await page.mouse.up();
  check(same(await taken(), [["device", "mouse", "beside"]]), "a mouse button anywhere on the page: the mouse");
  await page.evaluate(() => document.getElementById("beside").dispatchEvent(new PointerEvent("pointerdown", {pointerType: "touch", pointerId: 9, bubbles: true})));
  await page.evaluate(() => document.getElementById("beside").dispatchEvent(new PointerEvent("pointerdown", {pointerType: "pen", pointerId: 10, bubbles: true})));
  check(same(await taken(), [["device", "touch", "beside"], ["device", "touch", "beside"]]), "a finger or a pen anywhere on the page: touch");
  await page.evaluate(() => document.dispatchEvent(new PointerEvent("pointermove", {pointerType: "mouse", pointerId: 1, movementX: 0, movementY: 0, bubbles: true})));
  check(same(await taken(), []), "a mouse event without movement says nothing");
  // the fingers' ground truth: the browser's own list of touches
  const touchEvent = (type, count) => page.evaluate(([type, count]) => { const e = new Event(type, {bubbles: true}); Object.defineProperty(e, "touches", {value: Array(count).fill({})}); document.body.dispatchEvent(e); }, [type, count]);
  await touchEvent("touchend", 1);
  check(same(await taken(), []), "a finger lifted while another stays: nothing");
  await touchEvent("touchend", 0);
  await touchEvent("touchcancel", 0);
  await page.evaluate(() => window.dispatchEvent(new Event("pagehide")));
  check(same(await taken(), [["fingers gone"], ["fingers gone"], ["fingers gone"]]), "no touch left on the glass (lifted or taken away), or the page left: the fingers are gone");

  // the gamepad in use: the first with the standard mapping, else the first connected
  const gamepad = pads => page.evaluate(pads => {
    const make = d => d && {connected: d.connected ?? true, mapping: d.mapping ?? "", id: d.id, axes: d.axes ?? [0, 0, 0, 0],
      buttons: Array.from({length: 17}, (_, i) => ({pressed: (d.pressed ?? []).includes(i), value: d.values?.[i] ?? ((d.pressed ?? []).includes(i) ? 1 : 0)}))};
    navigator.getGamepads = () => pads.map(make);
    const g = window.readGamepad();
    return g && {id: g.pad.id, lx: g.lx, ly: g.ly, rx: g.rx, ry: g.ry, lt: g.lt, rt: g.rt, bits: g.bits, active: g.active};
  }, pads);
  check(await gamepad([]) === null && await gamepad([null, {id: "gone", connected: false}]) === null, "no gamepad: none");
  check(same(await gamepad([null, {id: "stick"}, {id: "xbox", mapping: "standard", axes: [0.5, -0.25, 0.1, 1], pressed: [0, 9], values: {6: 0.3, 7: 0.9}}]),
    {id: "xbox", lx: 0.5, ly: 0.25, rx: 0.1, ry: -1, lt: 0.3, rt: 0.9, bits: 1 | 1 << 9, active: true}), "a standard pad behind another device: it is read - sticks up-positive, triggers, buttons as a mask");
  check(same(await gamepad([{id: "stick", axes: [0.2, 0, 0, 0]}]), {id: "stick", lx: 0.2, ly: 0, rx: 0, ry: 0, lt: 0, rt: 0, bits: 0, active: false}), "only another device: it is read; a stick inside .22 and no button is not in use");
  check((await gamepad([{id: "xbox", mapping: "standard", axes: [0, 0, 0.3, 0]}])).active && (await gamepad([{id: "xbox", mapping: "standard", pressed: [16]}])).bits === 1 << 16, "a stick past .22 or any of the 17 buttons: in use");
  check(await page.evaluate(() => window.wheelPixels({deltaY: 2, deltaMode: 0}) === 2 && window.wheelPixels({deltaY: 2, deltaMode: 1}) === 80 && window.wheelPixels({deltaY: -1, deltaMode: 2}) === -400), "the wheel's units: pixels, lines of 40 px, pages of 400 px");

  // losing the window's focus lets go of every button
  await page.evaluate(() => { window.lockOnClick = false; });
  await page.mouse.down();
  await take();
  await page.evaluate(() => window.dispatchEvent(new Event("blur")));
  check(same(await take(), [["focus", false]]) && await page.evaluate(() => window.input.buttons) === 0, "losing the focus lets go of every button");
  await page.mouse.up();
  // taken off again: nothing is told any more
  await page.evaluate(() => window.input.dispose());
  await take();
  await page.keyboard.press("KeyA");
  await page.mouse.down();
  await page.mouse.up();
  check(same(await take(), []), "disposed: no listener left");
  check(errors.length === 0, "no page errors on the contract page" + (errors.length ? ": " + errors.join(" | ") : ""));
  await page.context().close();
}

// ---- 2. the App host's examples ----------------------------------------------------------------
// The examples say nothing about themselves but their picture and a status line; what the input
// did is read from the engine: the camera's eye (the screen ray through the middle of the canvas).
const eye = page => page.evaluate(() => {
  const e = window.__kernApp.kern.exports, c = document.querySelector("canvas"), p = e.kern_alloc(64);
  const ok = e.kern_screen_ray(c.width / 2, c.height / 2, p);
  const v = new DataView(e.memory.buffer), out = [0, 1, 2].map(k => v.getFloat64(p + 8 * k, true));
  e.kern_free(p);
  return ok ? out : null;
});
const settle = page => page.waitForTimeout(600);
const turn = (a, b) => { let d = Math.atan2(b[1], b[0]) - Math.atan2(a[1], a[0]); while (d > Math.PI) d -= 2 * Math.PI; while (d < -Math.PI) d += 2 * Math.PI; return d; };
const far = v => Math.hypot(v[0], v[1], v[2]);
const examples = [
  ["hello", async page => {
    // hello: the left button's drag turns the camera by 0.005 rad a pixel, the wheel moves it by
    // 0.1 % a pixel, the arrow keys turn it
    await page.mouse.move(300, 300);
    const e0 = await eye(page);
    await settle(page);
    const e1 = await eye(page);
    check(Math.abs(turn(e0, e1)) < 1e-4 && Math.abs(far(e1) - far(e0)) < 1e-3, "hello: without input the camera stays");
    await page.mouse.move(460, 300, {steps: 8});
    await settle(page);
    const e2 = await eye(page);
    check(Math.abs(turn(e1, e2)) < 1e-4, "hello: moving the mouse without a button does not turn");
    await page.mouse.down();
    await page.mouse.move(300, 300, {steps: 8});
    await page.mouse.up();
    await settle(page);
    const e3 = await eye(page);
    check(Math.abs(turn(e2, e3) - 0.8) < 0.02, `hello: dragging 160 px with the left button turns by 0.8 rad (${turn(e2, e3).toFixed(3)})`);
    await page.mouse.down({button: "right"});
    await page.mouse.move(380, 300, {steps: 4});
    await settle(page);
    const e4 = await eye(page);
    check(Math.abs(turn(e3, e4)) < 1e-4, "hello: dragging with the right button does not turn");
    await page.mouse.down();  // the left one while the right one is held
    await page.mouse.move(300, 300, {steps: 4});
    await page.mouse.up();
    await page.mouse.move(380, 300, {steps: 4});  // the left one let go: no more turning
    await page.mouse.up({button: "right"});
    await settle(page);
    const e5 = await eye(page);
    check(Math.abs(turn(e4, e5) - 0.4) < 0.02, `hello: the left button pressed while the right one is held drags, and stops when let go (${turn(e4, e5).toFixed(3)})`);
    await page.mouse.wheel(0, 600);
    await settle(page);
    const e6 = await eye(page);
    check(Math.abs(far(e6) / far(e5) - 1.6) < 0.02, `hello: 600 px of wheel move the camera away by 60 % (${(far(e6) / far(e5)).toFixed(3)})`);
    await page.keyboard.down("ArrowLeft");
    for (let i = 0; i < 100 && !(turn(e6, await eye(page)) > 0.25); ++i) await page.waitForTimeout(100);
    await page.keyboard.up("ArrowLeft");
    await settle(page);
    const e7 = await eye(page);
    await settle(page);
    const e8 = await eye(page);
    check(turn(e6, e7) > 0.25 && Math.abs(turn(e7, e8)) < 1e-4, `hello: the left arrow turns while it is held, and no longer (${turn(e6, e7).toFixed(2)} rad)`);
  }],
  ["orbrun", async page => {
    // orbrun: W drives, the wheel moves the camera, a finger on the left half is the stick
    // (software WebGL draws it slowly: every check waits for what it expects, up to 30 s)
    const speed = () => page.evaluate(() => Number((document.getElementById("status").textContent.match(/([\d.]+) km\/h/) || [])[1] ?? NaN));
    const until = async (fn, ms = 30000) => { for (const end = Date.now() + ms;;) { if (await fn()) return true; if (Date.now() > end) return false; await page.waitForTimeout(150); } };
    await page.mouse.move(400, 300);
    await settle(page);
    check(await speed() === 0, "orbrun: without input the car stands");
    const e0 = await eye(page);
    await settle(page);
    check(Math.abs((await eye(page))[2] - e0[2]) < 0.02, "orbrun: without input the camera stays");
    await page.mouse.wheel(0, 500);  // 0.12 % a pixel: 60 % further away, the eye 28 % of that higher
    check(await until(async () => (await eye(page))[2] - e0[2] > 1.5), `orbrun: the wheel moves the camera away (${((await eye(page))[2] - e0[2]).toFixed(2)} m higher)`);
    await page.keyboard.down("KeyW");
    check(await until(async () => await speed() > 25), `orbrun: holding W drives (${await speed()} km/h)`);
    await page.keyboard.up("KeyW");
    const rolling = await speed();
    check(await until(async () => await speed() < rolling * 0.7), `orbrun: letting go of W rolls out (${rolling} -> ${await speed()} km/h)`);
    const finger = (type, x, y) => page.evaluate(([type, x, y]) => document.querySelector("canvas").dispatchEvent(new PointerEvent(type, {pointerType: "touch", pointerId: 11, isPrimary: true, clientX: x, clientY: y, bubbles: true, cancelable: true})), [type, x, y]);
    const before = await speed();
    await finger("pointerdown", 150, 420);
    await finger("pointermove", 150, 320);
    check(await until(async () => await speed() > before + 15), `orbrun: a finger pushed up on the left half drives (${before} -> ${await speed()} km/h)`);
    await finger("pointerup", 150, 320);
    const pushed = await speed();
    check(await until(async () => await speed() < pushed * 0.7), `orbrun: the finger lifted rolls out (${pushed} -> ${await speed()} km/h)`);
  }],
];
for (const [name, act] of examples) {
  const page = await (await browser.newContext({viewport: {width: 800, height: 600}})).newPage();
  const errors = [];
  page.on("pageerror", e => errors.push(e.message));
  await page.goto(`${base}/kern/examples/${name}/index.html`);
  const started = await page.waitForFunction(() => window.__kernApp || window.__kernError, null, {timeout: 60000}).then(() => page.evaluate(() => !!window.__kernApp), () => false);
  check(started, `${name}: the app starts`);
  if (started) { await page.waitForTimeout(1500); await act(page); }
  check(errors.length === 0, `${name}: no page errors` + (errors.length ? ": " + errors.join(" | ") : ""));
  await page.context().close();
}
await browser.close();

// ---- 3. SpawnCity ------------------------------------------------------------------------------
// (its check's lines as they come, so a log's times say where a slow run spent them; ten minutes
// are the most it may take)
if (process.argv[3]) {
  const here = path.dirname(fileURLToPath(import.meta.url));
  const child = spawn(process.execPath, [path.join(here, "../../games/spawncity/tests/input_check.mjs"), process.argv[3]], {env: process.env, stdio: ["ignore", "pipe", "inherit"]});
  const late = setTimeout(() => { console.log("FAIL SpawnCity: the check did not finish within ten minutes"); child.kill("SIGKILL"); }, 600000);
  readline.createInterface({input: child.stdout}).on("line", line => console.log(line.replace(/^(ok  |FAIL) /, "$1 SpawnCity: ")));
  const code = await new Promise(resolve => child.on("close", resolve));
  clearTimeout(late);
  if (code !== 0) failures++;
}
console.log(failures ? `${failures} FAILED` : "input test passed");
process.exit(failures ? 1 : 0);
