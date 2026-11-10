// Browser test of the Blueprint editor's functions, formulas and C++ view, and of SpawnCity's
// level file in the editor (Playwright + Chromium):
//   node blueprint_e2e.mjs <base url of the repository root>
//
//  1. a function made with the mouse and the keyboard in My Blueprint and Details: its name, an
//     input and an output, a Formula node between the entry and the Return, a call from Begin
//     Play - compiled, played, and the result on the screen; the same Blueprint in the C++ view,
//     changed there and applied;
//  2. kern/games/spawncity/level/spawncity.klevel opens as a level: its actors are there, their
//     Blueprints compile without errors with the editor's own node library, and Play runs them
//     as machine code. (The game's project file must stay a file the editor reads.)
import {createRequire} from "node:module";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const base = (process.argv[2] || "").replace(/\/$/, "");
if (!base) { console.error("usage: node blueprint_e2e.mjs <base url>"); process.exit(2); }
let failures = 0;
const check = (ok, what) => { if (!ok) failures++; console.log(ok ? "ok  " : "FAIL", what); };
const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist"]});
const page = await (await browser.newContext({viewport: {width: 1280, height: 800}})).newPage();
const errors = [];
page.on("pageerror", e => errors.push(e.message));
await page.goto(base + "/kern/web/studio/index.html");
await page.waitForFunction(() => window.KernStudio && window.KernStudio.frames() > 5, null, {timeout: 120000});
const state = () => page.evaluate(() => window.KernStudio.state);
const status = () => page.evaluate(() => window.KernStudio.status);
const cmd = c => page.evaluate(c => window.KernStudio.command(c), c);
const until = async (fn, ms = 30000) => { for (const end = Date.now() + ms;;) { const v = await fn(); if (v) return v; if (Date.now() > end) return v; await page.waitForTimeout(100); } };
const logic = async () => (await state()).logic;
const errorsOf = l => (l?.issues || []).filter(i => i.error).map(i => i.text);

// ---- 1. a function, made by hand ---------------------------------------------------------------
await cmd({cmd: "new", template: "basic"});
await cmd({cmd: "logicOpen", owner: 0});
check(await until(async () => await page.locator("#logicEditor:not([hidden]) .lg-canvas").count() === 1), "the Blueprint editor is open");
await page.click('.lg-sec[data-sec="fns"] [data-add]');
check(await until(async () => (await logic()).functions?.length === 1), "+ in Functions adds a function");
let l = await logic();
check(l.nodes.some(n => n.type === "func.entry") && l.nodes.some(n => n.type === "func.return") && l.links.length === 1, "with its entry and a Return wired to it");
// Details shows the function with its name ready to type over
const name = page.locator('.lg-dbody [data-fk="name"], [data-fk="name"]').first();
check(await until(async () => await name.count() === 1), "Details shows the function");
await name.fill("Twice");
await name.press("Enter");
check(await until(async () => (await logic()).functions[0].name === "Twice"), "typing renames it");
check(await until(async () => await page.locator('.lg-fn[data-fn="Twice"]').count() === 1), "My Blueprint lists it by its name");
await page.click('[data-act="fn-in"]');
await page.click('[data-act="fn-out"]');
l = await until(async () => { const x = await logic(); return x.functions[0].inputs.length === 1 && x.functions[0].outputs.length === 1 && x; });
check(!!l && l.functions[0].inputs[0].name === "value" && l.functions[0].outputs[0].name === "result", "+ Input and + Output add its pins");
const pin = page.locator('.lg-frow[data-side="in"] [data-pf="name"]');
await pin.fill("x");
await pin.press("Enter");
check(await until(async () => (await logic()).functions[0].inputs[0].name === "x"), "a pin is renamed in place");
l = await logic();
const entry = l.nodes.find(n => n.type === "func.entry"), ret = l.nodes.find(n => n.type === "func.return");
check(entry.title === "Function Twice" && entry.outputs.some(p => p.id === "x") && ret.inputs.some(p => p.id === "result"), "the entry gives the input, the Return takes the output");
// a Formula between them: its names are its pins
const formula = (await cmd({cmd: "logicAdd", type: "math.formula", x: entry.x + 220, y: entry.y + 90, params: {formula: "x * 2 + bonus"}})).id;
l = await logic();
const fnode = l.nodes.find(n => n.id === formula);
check(fnode.title === "x * 2 + bonus" && fnode.inputs.map(p => p.id).join() === "x,bonus", "a Formula node takes its inputs from the formula");
await cmd({cmd: "logicConnect", from: entry.id, out: "x", to: formula, in: "x"});
await cmd({cmd: "logicConnect", from: formula, out: "result", to: ret.id, in: "result"});
await cmd({cmd: "logicValue", node: formula, pin: "bonus", value: 0.5});
// a formula that cannot be read is an error that says why; fixed, the error is gone
await cmd({cmd: "logicParam", node: formula, key: "formula", value: "x * 2 +"});
check(errorsOf(await logic()).some(t => /ends too early/.test(t)), "a broken formula is a compile error that says why");
await cmd({cmd: "logicParam", node: formula, key: "formula", value: "x * 2 + bonus"});
await cmd({cmd: "logicConnect", from: entry.id, out: "x", to: formula, in: "x"});
await cmd({cmd: "logicValue", node: formula, pin: "bonus", value: 0.5});
check(errorsOf(await logic()).length === 0, "the function compiles");
// Begin Play -> Twice(20) -> Show Message: the call from My Blueprint's Call button
const begin = (await cmd({cmd: "logicAdd", type: "event.begin", x: entry.x, y: entry.y + 320})).id;
await page.hover('.lg-fn[data-fn="Twice"]');  // (a row's buttons show under the pointer)
await page.click('.lg-fn[data-fn="Twice"] [data-call]');
l = await until(async () => { const x = await logic(); return x.nodes.some(n => n.type === "func.call") && x; });
const call = l.nodes.find(n => n.type === "func.call");
check(!!call && call.inputs.some(p => p.id === "x") && call.outputs.some(p => p.id === "result"), "Call adds a call node with the function's pins");
const msg = (await cmd({cmd: "logicAdd", type: "game.message", x: call.x + 320, y: call.y})).id;
await cmd({cmd: "logicConnect", from: begin, out: "then", to: call.id, in: "exec"});
await cmd({cmd: "logicConnect", from: call.id, out: "then", to: msg, in: "exec"});
await cmd({cmd: "logicConnect", from: call.id, out: "result", to: msg, in: "text"});
await cmd({cmd: "logicValue", node: call.id, pin: "x", value: 20});
check(errorsOf(await logic()).length === 0, "the graph compiles");

// ---- 1b. the same Blueprint as C++: read it, change it, apply it ----------------------------------
await page.click(".lg-cpp");
const cppText = page.locator(".lg-cpptext");
check(await until(async () => (await cppText.inputValue()).includes("TwiceResult Twice(Float x = 0.0) {")), "the C++ button shows the Blueprint as C++");
let code = await cppText.inputValue();
check(code.includes("return {kern::formula([](Float x, Float bonus) -> Float { return x * 2.0 + bonus; }, x, 0.5)};"), "the Formula node is the expression itself");
check(/n\d+ = Twice\(20\.0\);/.test(code) && code.includes("KERN_EVENT(on") && !code.includes("KERN_LAYOUT") && !/call_node|read_pin/.test(code), "the call is a call, the event a method; no layout, no callbacks");
// (this Blueprint has an event: a compiler does not run its text yet, and the view says what is missing)
const nativeNote = page.locator(".lg-cppnative");
check((await nativeNote.textContent()) === "runs in the interpreter for now" && /is not yet a function of blueprint\.h/.test(await nativeNote.getAttribute("title")), "the view says whether a compiler runs the text as it stands");
// a change typed into the text becomes the graph (Ctrl+Enter), and the nodes stay where they are
const places = async () => JSON.stringify((await logic()).nodes.map(n => [n.id, n.x, n.y]).sort((a, b) => a[0] - b[0]));
const placesBefore = await places();
await cppText.fill(code.replace("x, 0.5)};", "x, 1.5)};"));
check(await page.locator(".lg-cppstate.dirty").count() === 1, "a changed text says so");
await cppText.press("Control+Enter");
check(await until(async () => (await logic()).nodes.find(n => n.id === formula)?.inputs.find(p => p.id === "bonus")?.value === 1.5), "Apply changes the Blueprint: the Formula's bonus is 1.5");
check(await places() === placesBefore, "every node kept its id and its place");
check(await page.locator(".lg-cppstate.dirty").count() === 0 && (await cppText.inputValue()).includes("x, 1.5)};"), "the view shows the Blueprint as it now is");
// what is not a Blueprint is refused with its line, and nothing changes
await cppText.fill((await cppText.inputValue()).replace("Twice(20.0);", "Twice(20.0, 1.0);"));
await cppText.press("Control+Enter");
check(await until(async () => /^line \d+, column \d+: /.test(await page.locator(".lg-cpperr").innerText().catch(() => ""))), `a text that is no Blueprint is refused with line and reason (${await page.locator(".lg-cpperr").innerText().catch(() => "")})`);
check((await logic()).nodes.find(n => n.id === formula)?.inputs.find(p => p.id === "bonus")?.value === 1.5, "and the Blueprint is untouched");
await page.click(".lg-cppreset");
check(await page.locator(".lg-cpperr").isHidden() && (await cppText.inputValue()).includes("Twice(20.0);"), "Reset shows the Blueprint's text again");
// back to 0.5 through the text, then back to the graph
await cppText.fill((await cppText.inputValue()).replace("x, 1.5)};", "x, 0.5)};"));
await page.click(".lg-cppapply");
check(await until(async () => (await logic()).nodes.find(n => n.id === formula)?.inputs.find(p => p.id === "bonus")?.value === 0.5), "the Apply button does the same");
await page.click(".lg-cppclose");
check(await page.locator(".lg-cppview").isHidden() && errorsOf(await logic()).length === 0, "Graph closes the view; the Blueprint compiles");
await cmd({cmd: "logicClose"});
await cmd({cmd: "play"});
check(await until(async () => (await status()).hud?.message === "40.5", 60000), `Play calls the function: 20 * 2 + 0.5 on the screen (${(await status()).hud?.message})`);
check(await until(async () => (await status()).logic?.compiled >= 1, 30000), "as machine code (JIT)");
await cmd({cmd: "stop"});

// ---- 2. SpawnCity's level file -------------------------------------------------------------------
const text = await (await fetch(base + "/kern/games/spawncity/level/spawncity.klevel")).text();
const opened = await cmd({cmd: "open", text, name: "spawncity.klevel"});
check(opened?.ok !== false, "SpawnCity's level file opens in the editor" + (opened?.error ? ": " + opened.error : ""));
let st = await state();
check(st.level?.name === "SpawnCity", `it is the level SpawnCity (${st.level?.name})`);
// (the level file says which actors carry a Blueprint; the editor's state lists every actor as [id, name])
const carriers = JSON.parse(text).actors.filter(a => a.components?.logic).map(a => a.name);
const withBlueprint = (st.actors || []).filter(([, name]) => carriers.includes(name)).map(([id, name]) => ({id, name}));
check(carriers.length >= 1 && withBlueprint.length === carriers.length && carriers.includes("Wanted"), `its rule actors are there (${withBlueprint.map(a => a.name).join(", ")})`);
for (const a of withBlueprint) {
  await cmd({cmd: "logicOpen", owner: a.id});
  l = await logic();
  const bad = errorsOf(l);
  check(bad.length === 0 && l.nodes.every(n => n.title), `${a.name}: its Blueprint compiles in the editor (${l.functions.length} functions, ${l.nodes.length} nodes)` + (bad.length ? ": " + bad.slice(0, 3).join(" | ") : ""));
  await cmd({cmd: "logicClose"});
}
await cmd({cmd: "play"});
check(await until(async () => (await status()).logic?.compiled >= withBlueprint.length, 60000), `Play runs them as machine code (${(await status()).logic?.compiled} compiled)`);
await cmd({cmd: "stop"});
check(errors.length === 0, "no page errors" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
await browser.close();
console.log(failures ? `${failures} FAILED` : "blueprint test passed");
process.exit(failures ? 1 : 0);
