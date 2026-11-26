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
  await cmd({cmd: "projectOpen", text: fs.readFileSync(path.join(game, "murmel.kproject"), "utf8")});
  check(await until(async () => (await state()).project?.name === "Murmel"), "the editor opens murmel.kproject");
  // the project's input actions in its settings (kern/docs/app.md 4.): shown, and a wrong name is refused with its place
  const input = (await state()).project?.settings?.input;
  check(input?.actions?.some(x => x.name === "jump") && input?.axes?.some(x => x.name === "moveX"), "the editor shows the project's actions and axes (" + JSON.stringify(input ?? null).slice(0, 120) + ")");
  check(await page.evaluate(() => !!document.querySelector('#projectBody [data-kind="actions"][data-name="jump"] input.binding')), "the project panel has a row for the action jump");
  const wrong = await cmd({cmd: "projectSet", settings: {...(await state()).project.settings, input: {actions: [{name: "jump", keys: ["spcae"]}]}}});
  check(wrong?.ok === false && /input\.actions\[0\]\.keys\[0\]: unknown name "spcae"/.test(wrong.error ?? ""), "a key the engine has not is refused with its place (" + (wrong?.error ?? "-") + ")");
  const start = JSON.parse(fs.readFileSync(path.join(game, "murmel.kproject"), "utf8")).startLevel;
  await cmd({cmd: "open", text: fs.readFileSync(path.join(game, start), "utf8"), name: "wiese"});
  check(await until(async () => (await state()).level?.name === "Wiese" || /Wiese/.test(JSON.stringify((await state()).level ?? {}).slice(0, 400))), "it opens the start level " + start);
  await cmd({cmd: "play", mode: "play"});
  check(await until(async () => /Level 1/.test((await status()).hud?.message ?? "")), "Play runs the level's Blueprint (its message: " + ((await status()).hud?.message ?? "-") + ")");
  const box = await page.locator(".vp canvas, #gl").first().boundingBox();
  if (box) await page.mouse.click(box.x + box.width / 2, box.y + box.height / 2);
  await page.keyboard.down("ArrowUp");
  check(await until(async () => ((await status()).hud?.score ?? 0) >= 10, 90000), "an arrow key rolls the marble over a coin in the editor (score " + ((await status()).hud?.score ?? "-") + ")");
  await page.keyboard.up("ArrowUp");
  check(errors.length === 0, "no page errors in the editor" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
  await page.close();
}
await browser.close();
console.log(`murmel e2e: ${checks - failures}/${checks}`);
process.exit(failures ? 1 : 0);
