// Screenshots for the book (kern/docs/buch/bilder) at print resolution (device scale 2).
//
//   node kern/tools/buch_bilder.mjs <base-url> <out-folder> [names...]
//
// Without names it makes all of them; names pick some (e.g. studio.png game.png). Software WebGL is slow:
// each picture waits for its scene, a full run takes one to two hours. A group that crashes (software
// WebGL under memory pressure) is tried once more; what still fails is listed and the exit code is 1.
//
// Language: KERN_BOOK_LANG=de or en opens the Studio and the packaged game with ?lang=de / ?lang=en and
// writes to <out-folder>/de/ or <out-folder>/en/ (German books: German interface, English books: English).
// Without it the pictures go to <out-folder> itself, in the browser's language.
// Studio page: KERN_STUDIO_PATH, default /generated/studio.html (local build); the published site has
// /editor/index.html.
//
// The pictures in kern/docs/buch/bilder/de and en come from the published build (branch gh-pages).
// From the repository root, one command per language:
//
//   git fetch origin +gh-pages:refs/remotes/origin/gh-pages
//   rm -rf /tmp/kern-site && mkdir /tmp/kern-site && git archive origin/gh-pages | tar -x -C /tmp/kern-site
//   (cd /tmp/kern-site && python3 -m http.server 8771 --bind 127.0.0.1 &)
//   KERN_STUDIO_PATH=/editor/index.html KERN_BOOK_LANG=de node kern/tools/buch_bilder.mjs http://127.0.0.1:8771 kern/docs/buch/bilder
//   KERN_STUDIO_PATH=/editor/index.html KERN_BOOK_LANG=en node kern/tools/buch_bilder.mjs http://127.0.0.1:8771 kern/docs/buch/bilder
//
// Pictures:
//   studio.png, studio-selected.png   Orb Run, the steps of kern/tests/e2e/studio_e2e.mjs (1280 x 800)
//   studio-simulate.png               Physics Lab simulating, collision shapes and contact points shown
//   studio-quad.png                   Orb Run at night (time of day 0.85), four viewports
//   studio-physics-lab.png            Physics Lab simulating
//   studio-play.png                   Orb Run played in the viewport
//   studio-multiplayer.png            two players: server in viewport 1, the local client in viewport 2
//                                     (a perspective view), front and right below
//   studio-narrow.png                 an 820 px wide window
//   phone-dock-390.png, phone-play-390.png   the Studio on a phone, portrait
//   game.png, game-phone.png          the packaged Orb Run (KERN_GAME_HTML names one; otherwise the
//                                     Studio packages it into the system's temp folder)
//   lehrer.png                        the Lehrer page training (KERN_LEHRER_URL, default <base-url>/lehrer/index.html;
//                                     ?lang= like the Studio, the page is German and English)
//   duck-level.png, duck-play.png, duck-close.png   only with KERN_BOOK_DUCK=own-design, see below
// Hidden in every picture, because they describe the machine the pictures are made on and not the
// Studio: the frame rate (#fps, SwiftShader draws 1 to 3 frames a second), the build tag and the site's
// legal links in the status bar, and the camera position line in the main viewport.
import {createRequire} from "node:module";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import {pathToFileURL} from "node:url";
const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const [base, outBase, ...only] = process.argv.slice(2);
const lang = process.env.KERN_BOOK_LANG || "";
if (lang && lang !== "de" && lang !== "en") throw new Error("KERN_BOOK_LANG must be de or en");
const out = lang ? path.join(outBase, lang) : outBase;
fs.mkdirSync(out, {recursive: true});
const query = lang ? "?lang=" + lang : "";
const studioUrl = base + (process.env.KERN_STUDIO_PATH || "/generated/studio.html") + query;
const duckAllowed = process.env.KERN_BOOK_DUCK === "own-design";
const isDuck = name => name.startsWith("duck-");
// The duck pictures only when asked for (own-design build), the others only without that build.
const want = name => (only.length === 0 ? !isDuck(name) : only.includes(name)) && (isDuck(name) ? duckAllowed : true);
const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist"]});
const wait = ms => new Promise(r => setTimeout(r, ms));
const failed = [];

const HIDE = `#fps, #buildTag, #legalLinks { visibility: hidden !important; }
.vp[data-index="0"] .vp-info { visibility: hidden !important; }`;

// Playing, SwiftShader draws a frame in tens of seconds and the screenshot waits behind them (minutes at
// device scale 2). `freeze` stops the page's frame loop first: the last frame stays on screen and is captured.
const shot = async (page, name, freeze = false) => {
  if (freeze) await page.evaluate(() => { window.requestAnimationFrame = () => 0; });
  await page.screenshot({path: path.join(out, name), timeout: 1800000});
  console.log(path.join(out, name));
};
/** Polls `fn` until it returns truthy or `ms` pass; returns the last value. */
async function until(fn, ms = 60000) {
  const end = Date.now() + ms;
  let v;
  while (!(v = await fn()) && Date.now() < end) await wait(100);
  return v;
}
/** Waits until the Studio has drawn `n` more frames (software WebGL draws about one a second). */
const frames = async (page, n = 3) => {
  const f0 = await page.evaluate(() => window.KernStudio.frames());
  await until(() => page.evaluate(m => window.KernStudio.frames() >= m, f0 + n), 300000);
};

async function studio(viewport = {width: 1280, height: 800}, phone = false) {
  const ctx = await browser.newContext({viewport, deviceScaleFactor: 2, acceptDownloads: true, ...(phone ? {isMobile: true, hasTouch: true} : {})});
  const page = await ctx.newPage();
  page.setDefaultTimeout(600000);  // clicks wait for a stable element: a frame a second or slower here
  page.on("pageerror", e => console.log("pageerror", e.message));
  await page.goto(studioUrl);
  await page.waitForFunction(() => window.KernStudio && window.KernStudio.frames() > 5, null, {timeout: 300000});
  await page.addStyleTag({content: HIDE});
  await wait(1500);
  const cmd = c => page.evaluate(c => window.KernStudio.command(c), c);
  const state = () => page.evaluate(() => window.KernStudio.state);
  const status = () => page.evaluate(() => window.KernStudio.status);
  return {ctx, page, cmd, state, status};
}
/** Runs one group of pictures; a crash is tried once more, then reported. */
async function attempt(label, fn) {
  for (let i = 1; i <= 2; i++) {
    try { return await fn(); } catch (e) {
      console.log(`${label}: attempt ${i} failed: ${String(e.message).split("\n")[0]}`);
      for (const c of browser.contexts()) await c.close().catch(() => {});
    }
  }
  failed.push(label);
}
/** The viewCamera command that puts the camera at `p` looking at `t` (metres, Z up). */
const lookAt = (viewport, p, t) => {
  const d = [t[0] - p[0], t[1] - p[1], t[2] - p[2]];
  return {cmd: "viewCamera", viewport, position: p, yaw: Math.atan2(d[1], d[0]) * 180 / Math.PI, pitch: Math.atan2(d[2], Math.hypot(d[0], d[1])) * 180 / Math.PI};
};
const playing = page => page.waitForFunction(() => window.KernStudio.state.play.state === "playing", null, {timeout: 600000});
const simulating = page => page.waitForFunction(() => window.KernStudio.state.play.state === "simulating", null, {timeout: 600000});
/** Lets the simulation run until `steps` physics steps are done (a few per second here). */
const steps = (page, n, ms = 600000) => until(() => page.evaluate(n => (window.KernStudio.status.sim?.steps || 0) >= n, n), ms);

// studio.png, studio-selected.png: the steps of kern/tests/e2e/studio_e2e.mjs (same scene).
if (want("studio.png") || want("studio-selected.png")) await attempt("studio.png / studio-selected.png", async () => {
  const {ctx, page, state, status} = await studio();
  if (want("studio.png")) { await frames(page); await shot(page, "studio.png"); }
  if (want("studio-selected.png")) {
    const r = await page.evaluate(() => window.KernStudio.viewportRect(0));
    const c = {x: r.left + r.width / 2, y: r.top + r.height / 2};
    // Fly: right mouse button and W.
    const before = (await status()).camera.position;
    await page.mouse.move(c.x, c.y);
    await page.mouse.down({button: "right"});
    await page.keyboard.down("KeyW");
    await until(async () => { const p = (await status()).camera.position; return Math.hypot(p[0] - before[0], p[1] - before[1], p[2] - before[2]) > 1; }, 120000);
    await page.keyboard.up("KeyW");
    await page.mouse.move(c.x + 60, c.y, {steps: 4});
    await page.mouse.up({button: "right"});
    // Tools, then the hover car from the outliner, its X location typed and undone, F focuses.
    await page.keyboard.press("KeyE");
    await until(async () => (await state()).tool === "rotate");
    await page.click('[data-tool="scale"]');
    await until(async () => (await state()).tool === "scale");
    await page.keyboard.press("KeyW");
    await page.locator("#outliner .row", {hasText: "Hover Car"}).first().click();
    await until(async () => (await state()).selection.length === 1);
    const pos = async () => (await state()).details.sections.find(s => s.id === "transform").fields.find(f => f.path === "transform.position").value;
    const x = page.locator('#details input.num[data-path="transform.position"][data-part="0"]');
    await until(() => x.count());
    await x.click();
    await x.fill("12.5");
    await x.press("Enter");
    await until(async () => Math.abs((await pos())[0] - 12.5) < 1e-3);
    await page.keyboard.press("Control+z");
    await until(async () => Math.abs((await pos())[0] - 12.5) > 0.01);
    await page.mouse.move(c.x, c.y);
    await page.keyboard.press("KeyF");
    await frames(page);
    await shot(page, "studio-selected.png");
  }
  await ctx.close();
});

// studio-simulate.png: Physics Lab simulating, Show > Collision shapes and Contact points, the bodies moving.
if (want("studio-simulate.png")) await attempt("studio-simulate.png", async () => {
  const {ctx, page, cmd} = await studio();
  await cmd({cmd: "new", template: "physicsLab"});
  await frames(page);
  // From the level's own camera position (closer, the floor and the endless ground z-fight), a narrow lens
  // on the stack, the pendulum and the door.
  await cmd(lookAt(0, [-30, -30, 18], [-7, -9, 1]));
  await cmd({cmd: "fov", viewport: 0, value: 30});
  for (const flag of ["colliders", "contacts"]) await cmd({cmd: "showFlag", viewport: 0, flag, value: true});
  await page.click("#simBtn");
  await simulating(page);
  await steps(page, 150);
  await shot(page, "studio-simulate.png", true);
  await ctx.close();
});

// studio-quad.png: Orb Run at night (lit windows, headlights, bloom), four viewports.
if (want("studio-quad.png")) await attempt("studio-quad.png", async () => {
  const {ctx, page, cmd} = await studio();
  await cmd({cmd: "setWorld", path: "world.dayLength", value: 0});
  await cmd({cmd: "setWorld", path: "world.timeOfDay", value: 0.85});
  await cmd({cmd: "layout", layout: "quad"});
  await until(async () => (await page.locator(".vp:not(.hidden)").count()) === 4);
  await frames(page, 5);
  await shot(page, "studio-quad.png");
  await ctx.close();
});

// studio-physics-lab.png: File > New Level > Physics Lab, simulated, from a camera that shows every group
// (stack, pendulum, door, rope bridge, turntable, lift, rover).
if (want("studio-physics-lab.png")) await attempt("studio-physics-lab.png", async () => {
  const {ctx, page, cmd} = await studio();
  await cmd({cmd: "new", template: "physicsLab"});
  await cmd({cmd: "fov", viewport: 0, value: 50});  // the level's camera, a longer lens: the lift and the rover larger
  await frames(page);
  await page.click("#simBtn");
  await simulating(page);
  await steps(page, 60);
  await shot(page, "studio-physics-lab.png", true);
  await ctx.close();
});

if (want("studio-play.png")) await attempt("studio-play.png", async () => {
  const {ctx, page} = await studio();
  await page.click("#playBtn");
  await playing(page);
  await wait(6000);
  await shot(page, "studio-play.png", true);
  await ctx.close();
});

// studio-multiplayer.png: a listen server and a local client. The client draws into viewport 2, which is
// switched to a perspective view (its bar would say "Top"); its bar is hidden like the server's while
// playing, so it does not cover the client's HUD.
if (want("studio-multiplayer.png")) await attempt("studio-multiplayer.png", async () => {
  const {ctx, page, cmd} = await studio();
  await cmd({cmd: "viewType", viewport: 1, type: "perspective"});
  await cmd({cmd: "viewMode", viewport: 1, mode: "lit"});
  await page.addStyleTag({content: '.vp[data-index="1"] .vp-bar, .vp[data-index="1"] .vp-info { visibility: hidden !important; }'});
  await cmd({cmd: "netPlayers", players: 2});
  await cmd({cmd: "play", mode: "play"});
  await page.waitForFunction(() => window.KernStudio.status.net?.clients?.[0]?.hud?.status === "playing", null, {timeout: 300000}).catch(() => {});
  await wait(3000);
  await shot(page, "studio-multiplayer.png", true);
  await ctx.close();
});

if (want("studio-narrow.png")) await attempt("studio-narrow.png", async () => {
  const {ctx, page} = await studio({width: 820, height: 900});
  await frames(page);
  await shot(page, "studio-narrow.png");
  await ctx.close();
});

if (want("phone-dock-390.png") || want("phone-play-390.png")) await attempt("phone pictures", async () => {
  const {ctx, page} = await studio({width: 390, height: 844}, true);
  if (want("phone-dock-390.png")) { await frames(page); await shot(page, "phone-dock-390.png"); }
  if (want("phone-play-390.png")) {
    await page.tap("#playBtn");
    await playing(page);
    await wait(5000);
    await shot(page, "phone-play-390.png", true);
  }
  await ctx.close();
});

// The duck pictures: Kern Studio draws the duck with Pollen Robotics' meshes (CC BY-NC-SA 4.0,
// non-commercial), and a book that is sold must not show them. They are only made with
// KERN_BOOK_DUCK=own-design, on a local build whose duck is the author's own design (tools/kern_duck.py,
// tag buch-ente-eigenes-design): revert 8353d43 for kern/src/game (builtin_meshes.cpp includes
// duck_meshes.inc), run tools/build_studio.sh, serve generated/studio.html as editor/index.html of a copy
// of the site. That build is never committed or published.
if (want("duck-level.png") || want("duck-play.png") || want("duck-close.png")) await attempt("duck pictures", async () => {
  const {ctx, page, cmd} = await studio();
  await cmd({cmd: "new", template: "microDuck"});
  await frames(page, 5);
  if (want("duck-level.png")) await shot(page, "duck-level.png");
  if (want("duck-close.png")) {  // the duck up close (the cover), without the viewport's buttons and status line
    const level = JSON.parse(await page.evaluate(() => window.KernStudio.command({cmd: "packageText"}).text));
    const p = level.actors.find(a => a.name && a.name.startsWith("Micro Duck")).transform.position;
    const [ox, oy, oz] = [1.25, 0.5, 0.62];
    await cmd({cmd: "viewCamera", position: [p[0] + ox, p[1] + oy, p[2] + oz],
               yaw: Math.atan2(-oy, -ox) * 180 / Math.PI, pitch: -Math.atan2(oz - 0.3, Math.hypot(ox, oy)) * 180 / Math.PI});
    await cmd({cmd: "select", ids: []});
    await frames(page, 4);
    const r = await page.evaluate(() => window.KernStudio.viewportRect(0));
    await page.screenshot({path: path.join(out, "duck-close.png"), clip: {x: r.x, y: r.y + 40, width: r.width, height: r.height - 76}, timeout: 1800000});
    console.log(path.join(out, "duck-close.png"));
    await cmd({cmd: "new", template: "microDuck"});
    await frames(page, 4);
  }
  if (want("duck-play.png")) {  // Play: the duck plays on its own, its head camera and ToF grid in the corner
    await cmd({cmd: "play", mode: "play"});
    // The duck level plays with the observer camera; its policies load first.
    await page.waitForFunction(() => window.KernStudio.state.play.state !== "editing", null, {timeout: 600000});
    await until(async () => (await page.evaluate(() => window.KernStudio.status.robot?.zones?.length)) === 64, 900000);
    console.log("duck play:", JSON.stringify(await page.evaluate(() => window.KernStudio.state.play)));
    await wait(30000);
    await shot(page, "duck-play.png", true);
  }
  await ctx.close();
});

let gameHtml = process.env.KERN_GAME_HTML;  // a packaged game (Studio: File > Export), e.g. the e2e test's orb-run-game.html
if (!gameHtml && (want("game.png") || want("game-phone.png"))) await attempt("packaging Orb Run", async () => {
  const {ctx, page} = await studio();
  const html = await page.evaluate(() => window.KernStudio.packageHtml("Orb Run"));
  const file = path.join(fs.mkdtempSync(path.join(os.tmpdir(), "kern-book-")), "orb-run-game.html");
  fs.writeFileSync(file, html);
  gameHtml = pathToFileURL(file).href;
  await ctx.close();
});
for (const [name, viewport, phone] of [["game.png", {width: 1280, height: 800}, false], ["game-phone.png", {width: 844, height: 390}, true]]) {
  if (!want(name) || !gameHtml) continue;
  await attempt(name, async () => {
    const ctx = await browser.newContext({viewport, deviceScaleFactor: 2, ...(phone ? {isMobile: true, hasTouch: true} : {})});
    const page = await ctx.newPage();
    await page.goto(gameHtml + query);
    await page.waitForFunction(() => window.KernGame?.status?.hud?.status === "playing", null, {timeout: 600000});
    await wait(8000);
    await shot(page, name, true);
    await ctx.close();
  });
}

// lehrer.png: the Lehrer page while it trains (one worker, the default jump), with its curve and values,
// in the book's language (?lang=, the same rule as the Studio).
const lehrerUrl = process.env.KERN_LEHRER_URL || base + "/lehrer/index.html";
if (want("lehrer.png")) await attempt("lehrer.png", async () => {
  const ctx = await browser.newContext({viewport: {width: 1280, height: 800}, deviceScaleFactor: 2});
  const page = await ctx.newPage();
  page.setDefaultTimeout(600000);
  const res = await page.goto(lehrerUrl + query);
  if (!res || !res.ok()) throw new Error("no Lehrer page at " + lehrerUrl);
  await page.waitForFunction(() => !document.getElementById("start").disabled && /Bereit|Ready/.test(document.getElementById("status").textContent), null, {timeout: 600000});
  await page.fill("#workers", "1");
  await page.click("#start");
  // A curve needs a few dozen iterations.
  await page.waitForFunction(() => Number(document.getElementById("s-it").textContent) >= 30, null, {timeout: 3600000});
  await shot(page, "lehrer.png");
  await ctx.close();
});

await browser.close();
if (failed.length) { console.log("FAILED: " + failed.join(", ")); process.exitCode = 1; }
