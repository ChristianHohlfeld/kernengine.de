// Browser test of the native export (Playwright + Chromium):
//   node export_e2e.mjs <url of the exported game>
//
// The page is what tools/export_game.sh wrote for kern/tests/e2e/native_export.klevel: a
// standalone game whose engine module was built for that level, with its Blueprints compiled in
// as C++ classes. Both Blueprints of the level must run as those classes - not in the
// interpreter, not compiled by the browser - and do what the level says: the level's Begin Play
// shows its message, the cube's timer moves it up and scores.
import {createRequire} from "node:module";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const url = process.argv[2] || "";
if (!url) { console.error("usage: node export_e2e.mjs <url of the exported game>"); process.exit(2); }
let failures = 0;
const check = (ok, what) => { if (!ok) failures++; console.log(ok ? "ok  " : "FAIL", what); };
const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist"]});
const page = await (await browser.newContext({viewport: {width: 1280, height: 800}})).newPage();
const errors = [];
page.on("pageerror", e => errors.push(e.message));
await page.goto(url);
await page.waitForFunction(() => window.KernGame && window.KernGame.status?.logic, null, {timeout: 120000});
const status = () => page.evaluate(() => window.KernGame.status);
const until = async (fn, ms = 30000) => { for (const end = Date.now() + ms;;) { const v = await fn(); if (v) return v; if (Date.now() > end) return v; await page.waitForTimeout(100); } };

const logic = (await status()).logic;
check(logic.graphs === 2 && logic.native === 2, `both Blueprints run as their C++ classes (${JSON.stringify(logic)})`);
check((await status()).hud?.message === "Native export", "the level's Begin Play ran: its message is on the screen");
check(await until(async () => (await status()).hud?.score >= 3, 20000), `the cube's timer runs: it scores (${(await status()).hud?.score})`);
const again = (await status()).logic;
check(again.native === 2 && again.graphs === 2, "and they still run as classes");
check(errors.length === 0, "no page errors" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
await browser.close();
console.log(failures ? `${failures} FAILED` : "native export test passed");
process.exit(failures ? 1 : 0);
