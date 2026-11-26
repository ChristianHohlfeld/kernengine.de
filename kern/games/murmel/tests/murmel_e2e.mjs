// Murmel im Browser (Playwright, Software-WebGL): (1) die Seite des Spiels lädt das Modul, eine gedrückte Pfeiltaste
// rollt die Murmel über die erste Münze, der Punktestand der Anzeige steigt, der Ton läuft (der Mixer der Engine); (2) Kern Studio öffnet das Projekt und
// sein Start-Level, Play spielt es: der Level-Graph meldet sich, die Pfeiltaste rollt, die Punkte steigen.
//
//   node kern/games/murmel/tests/murmel_e2e.mjs <base-url>     (die Wurzel des Repos, mit generated/studio.html und
//                                                                kern/games/murmel/web/murmel.wasm)
import fs from "node:fs";
import path from "node:path";
import {createRequire} from "node:module";
import {fileURLToPath} from "node:url";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const base = process.argv[2] || "http://127.0.0.1:8773";
const game = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..");
let failures = 0, checks = 0;
const check = (ok, what) => {
  checks++;
  if (!ok) { failures++; console.log("FAIL", what); if (process.env.GITHUB_ACTIONS) console.log("::error::murmel e2e: " + what); }
  else console.log("ok  ", what);
};
async function until(fn, ms = 30000) {
  const end = Date.now() + ms;
  let v;
  while (!(v = await fn()) && Date.now() < end) await new Promise(r => setTimeout(r, 100));
  return v;
}
const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist"]});

// (1) das Spiel auf seiner Seite
{
  const page = await browser.newPage({viewport: {width: 640, height: 400}});
  const errors = [];
  page.on("pageerror", e => errors.push(e.message));
  page.on("console", m => { if (m.type() === "error") errors.push(m.text()); });
  // a best score the page's storage holds already (kern/docs/app.md 5.: localStorage, the page's folder, the slot)
  await page.addInitScript(() => localStorage.setItem("kern-save:/kern/games/murmel/web/:murmel", JSON.stringify({format: "kern-save", version: 1, values: {best: {int: "42"}}})));
  await page.goto(base + "/kern/games/murmel/web/index.html");
  // the game's display: the engine's widgets on the page (kern/docs/app.md 6.)
  const text = () => page.evaluate(() => document.querySelector('.kern-ui [data-id="stand"]')?.textContent ?? "");
  const coins = () => page.evaluate(() => document.querySelector('.kern-ui [data-id="muenzen"][data-kind="bar"]')?.textContent ?? "");
  const punkte = async () => Number((/Punkte (\d+)/.exec(await text()) || [])[1] ?? -1);
  check(await until(async () => /Level 1\/2/.test(await text()), 90000), "the game page starts level 1 (" + (await text()).split("\n")[0] + ")");
  check(await punkte() === 0, "it starts with 0 points");
  check(/Bestwert 42/.test(await text()), "the best score comes from the page's storage (" + (await text()).split("\n")[0] + ")");
  await page.click("#game");
  await page.keyboard.down("ArrowUp");
  check(await until(async () => await punkte() >= 10, 90000), "an arrow key rolls the marble over a coin: the points go up (" + (await text()).split("\n")[0] + ")");
  await page.keyboard.up("ArrowUp");
  check(await until(async () => /Muenzen [1-3]\/3/.test(await coins())), "the bar counts the coin (" + (await coins()) + ")");
  check(await page.evaluate(() => { const f = document.querySelector('.kern-ui [data-id="muenzen"] > div > div'); return f && parseFloat(f.style.width) > 30; }), "the bar is filled a third and more");
  // the sound (kern/docs/app.md 3.): the page's key was the gesture - the engine's mixer plays, the host pulls it
  const heard = () => page.evaluate(() => window.__kernApp?.audio);
  check(await until(async () => (await heard())?.state === "running"), "the key unlocked the sound device (" + JSON.stringify(await heard()) + ")");
  check(await until(async () => (await heard())?.frames > 4800 && (await heard())?.peak > 0.01, 30000), "the app runtime pulls the mixer, and the coin is heard (" + JSON.stringify(await heard()) + ")");
  check(!(await page.evaluate(() => window.__kernError)), "no error from the module");
  check(errors.length === 0, "no page errors" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
  await page.close();
}

// (2) im Editor: das Projekt, sein Start-Level, Play
{
  const page = await browser.newPage({viewport: {width: 1024, height: 640}});
  const errors = [];
  page.on("pageerror", e => errors.push(e.message));
  await page.goto(base + "/generated/studio.html");
  await page.waitForFunction(() => window.KernStudio && window.KernStudio.frames() > 5, null, {timeout: 90000});
  const cmd = c => page.evaluate(c => window.KernStudio.command(c), c);
  const state = () => page.evaluate(() => window.KernStudio.state);
  const status = () => page.evaluate(() => window.KernStudio.status);
  // (a project opens with its levels: each one it names; its start level is the open level)
  const project = JSON.parse(fs.readFileSync(path.join(game, "murmel.kproject"), "utf8"));
  const without = await cmd({cmd: "projectOpen", text: fs.readFileSync(path.join(game, "murmel.kproject"), "utf8")});
  check(without?.ok === false && /not given: levels\/wiese\.klevel, levels\/bruecke\.klevel/.test(without.error ?? ""), "a project without its levels is refused, the levels named (" + (without?.error ?? "-") + ")");
  const levels = Object.fromEntries(project.levels.map(l => [l, fs.readFileSync(path.join(game, l), "utf8")]));
  await cmd({cmd: "projectOpen", text: fs.readFileSync(path.join(game, "murmel.kproject"), "utf8"), levels});
  check(await until(async () => (await state()).project?.name === "Murmel"), "the editor opens murmel.kproject with its levels");
  // the project's input actions in its settings (kern/docs/app.md 4.): shown, and a wrong name is refused with its place
  const input = (await state()).project?.settings?.input;
  check(input?.actions?.some(x => x.name === "jump") && input?.axes?.some(x => x.name === "moveX"), "the editor shows the project's actions and axes (" + JSON.stringify(input ?? null).slice(0, 120) + ")");
  // (the panel is drawn with the next state the page reads: waited for, as the project's name above)
  check(await until(() => page.evaluate(() => !!document.querySelector('#projectBody [data-kind="actions"][data-name="jump"] input.binding'))), "the project panel has a row for the action jump");
  const wrong = await cmd({cmd: "projectSet", settings: {...(await state()).project.settings, input: {actions: [{name: "jump", keys: ["spcae"]}]}}});
  check(wrong?.ok === false && /input\.actions\[0\]\.keys\[0\]: unknown name "spcae"/.test(wrong.error ?? ""), "a key the engine has not is refused with its place (" + (wrong?.error ?? "-") + ")");
  const start = project.startLevel;
  check(await until(async () => (await state()).level?.name === "Wiese" || /Wiese/.test(JSON.stringify((await state()).level ?? {}).slice(0, 400))), "opening the project opens its start level " + start);
  // The player of this check uses the keyboard only (the keys reach the editor from the window, the page has the
  // focus). In Play the pointer is locked - headless Chromium grants it without a gesture -, every movement of a locked
  // mouse turns the camera, and a mouse event the test sends under the lock comes with a movement of its own (seen in
  // CI 45742664680 and 45743657968: the click's move, then -340/-341 px with each press and release), so the arrow
  // would roll the marble past the coin. So the test sends no mouse event in Play.
  await cmd({cmd: "play", mode: "play"});
  check(await until(async () => /Level 1/.test((await status()).hud?.message ?? "")), "Play runs the level's Blueprint (its message: " + ((await status()).hud?.message ?? "-") + ")");
  // (every mouse movement the page sees from here on, and whether the pointer was locked: printed with the state below)
  await page.evaluate(() => { window.__moves = []; document.addEventListener("mousemove", e => window.__moves.push([e.movementX, e.movementY, !!document.pointerLockElement])); });
  // (the world takes input once it steps: its first fixed step)
  check(await until(async () => ((await status()).sim?.steps ?? 0) > 0), "the world steps (" + ((await status()).sim?.steps ?? "-") + " steps)");
  // (what the editor knows while the key is held: printed when the marble does not reach the coin)
  const seen = () => page.evaluate(() => {
    const s = window.KernStudio.status, st = window.KernStudio.state, a = document.activeElement;
    return {play: s.play, paused: s.paused, captured: s.captured, fps: s.fps, steps: s.sim?.steps, hud: s.hud && {score: s.hud.score, status: s.hud.status, message: s.hud.message},
            focus: document.hasFocus(), visible: document.visibilityState, active: a ? a.tagName + (a.id ? "#" + a.id : "") + (a.className ? "." + String(a.className).split(" ")[0] : "") : null,
            locked: !!document.pointerLockElement, logicOpen: !!st?.logic?.open, layout: st?.prefs?.layout, activeView: (st?.viewports ?? []).findIndex(v => v.active), moves: (window.__moves ?? []).slice(0, 12), moveCount: (window.__moves ?? []).length};
  });
  const before = await seen();
  await page.keyboard.down("ArrowUp");
  const rolled = await until(async () => ((await status()).hud?.score ?? 0) >= 10, 90000);
  const moved = await page.evaluate(() => (window.__moves ?? []).some(([x, y]) => x || y));
  console.log((rolled ? "" : "(not rolled) ") + "editor before the key:", JSON.stringify(before), "\neditor after:", JSON.stringify(await seen()));
  check(!moved, "the mouse does not move while the arrow is held (a locked mouse looks around)");
  check(rolled, "an arrow key rolls the marble over a coin in the editor (score " + ((await status()).hud?.score ?? "-") + ")");
  await page.keyboard.up("ArrowUp");
  check(errors.length === 0, "no page errors in the editor" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
  await page.close();
}
// (3) the export of the project (tools/export_game.sh murmel.kproject): a page for itself starts the game
{
  const page = await browser.newPage({viewport: {width: 640, height: 400}});
  const errors = [];
  page.on("pageerror", e => errors.push(e.message));
  await page.goto(base + "/generated/murmel.html");
  const stand = () => page.evaluate(() => document.querySelector('.kern-ui [data-id="stand"]')?.textContent ?? "");
  check(await until(async () => /Level 1\/2/.test(await stand()), 90000), "the exported page starts level 1 (" + (await stand()) + ")");
  check(await page.title() === "Murmel", "it has the project's name as its title (" + (await page.title()) + ")");
  check(errors.length === 0 && !(await page.evaluate(() => window.__kernError)), "no errors on the exported page" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
  await page.close();
}
await browser.close();
console.log(`murmel e2e: ${checks - failures}/${checks}`);
process.exit(failures ? 1 : 0);
