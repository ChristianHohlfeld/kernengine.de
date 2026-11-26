// Murmel im Browser (Playwright, Software-WebGL): (1) die Seite des Spiels lädt das Modul, eine gedrückte Pfeiltaste
// rollt die Murmel über die erste Münze, der Punktestand der Anzeige steigt; (2) Kern Studio öffnet das Projekt und
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
  await page.goto(base + "/kern/games/murmel/web/index.html");
  const text = () => page.evaluate(() => document.getElementById("status").textContent);
  const punkte = async () => Number((/Punkte (\d+)/.exec(await text()) || [])[1] ?? -1);
  check(await until(async () => /Level 1\/2/.test(await text()), 90000), "the game page starts level 1 (" + (await text()).split("\n")[0] + ")");
  check(await punkte() === 0, "it starts with 0 points");
  await page.click("#game");
  await page.keyboard.down("ArrowUp");
  check(await until(async () => await punkte() >= 10, 90000), "an arrow key rolls the marble over a coin: the points go up (" + (await text()).split("\n")[0] + ")");
  await page.keyboard.up("ArrowUp");
  check(await until(async () => /Muenzen [1-3]\/3/.test(await text())), "the display counts the coin");
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
  const start = project.startLevel;
  check(await until(async () => (await state()).level?.name === "Wiese" || /Wiese/.test(JSON.stringify((await state()).level ?? {}).slice(0, 400))), "opening the project opens its start level " + start);
  await cmd({cmd: "play", mode: "play"});
  check(await until(async () => /Level 1/.test((await status()).hud?.message ?? "")), "Play runs the level's Blueprint (its message: " + ((await status()).hud?.message ?? "-") + ")");
  // (every mouse movement the page sees from here on, and whether the pointer was locked: printed with the state below)
  await page.evaluate(() => { window.__moves = []; document.addEventListener("mousemove", e => window.__moves.push([e.movementX, e.movementY, !!document.pointerLockElement])); });
  const box = await page.locator(".vp canvas, #gl").first().boundingBox();
  if (box) await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
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
  console.log((rolled ? "" : "(not rolled) ") + "editor before the key:", JSON.stringify(before), "\neditor after:", JSON.stringify(await seen()));
  check(rolled, "an arrow key rolls the marble over a coin in the editor (score " + ((await status()).hud?.score ?? "-") + ")");
  await page.keyboard.up("ArrowUp");
  check(errors.length === 0, "no page errors in the editor" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
  await page.close();
}
await browser.close();
console.log(`murmel e2e: ${checks - failures}/${checks}`);
process.exit(failures ? 1 : 0);
