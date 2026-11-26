// Browser test of the Blueprint editor's functions, formulas and C++ view, and of a game's
// level file in the editor (Playwright + Chromium):
//   node blueprint_e2e.mjs <base url of the repository root> [<a game's level file, relative to it>]
//
//  1. a function made with the mouse and the keyboard in My Blueprint and Details: its name, an
//     input and an output, a Formula node between the entry and the Return, a call from Begin
//     Play - compiled, played, and the result on the screen; the same Blueprint in the C++ view,
//     changed there and applied;
//     then a list: a variable made a Float List in Details, its items typed in, nodes offered
//     in the list's type from its pin, For Each over it - as C++, changed there, and played;
//     then an enum and a struct, each made in My Blueprint and edited in Details, a variable of
//     its type, its nodes from the action menu - as C++, changed there, and played;
//     then the number types: an f32 and an i64 made in Details (values read exactly in their type, a bad one
//     refused at the field), an Add wired by hand - its Cast appears on the wire, one undo step -, compiled, played;
//  2. (with a game's level file named) it opens as a level: its actors are there, their
//     Blueprints compile without errors with the editor's own node library, and Play runs them
//     as machine code. (The game's project file must stay a file the editor reads.)
import {createRequire} from "node:module";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const base = (process.argv[2] || "").replace(/\/$/, "");
const gameLevel = process.argv[3] || "";   // (a game's level file, relative to the base: its cases - section 2 - run only then)
if (!base) { console.error("usage: node blueprint_e2e.mjs <base url>"); process.exit(2); }
let failures = 0;
const check = (ok, what) => { if (!ok) failures++; console.log(ok ? "ok  " : "FAIL", what); };
const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist"]});
const page = await (await browser.newContext({viewport: {width: 1280, height: 800}})).newPage();
// A click waits until its target stands still for two animation frames. With software WebGL the
// browser now and then delivers no frame for seconds (the page's own thread is idle meanwhile -
// measured: 97 % idle, two frames in twelve seconds): a click then needs longer than Playwright's
// 30 s on a slow machine. The page has two minutes.
page.setDefaultTimeout(120000);
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
// (every Blueprint is shown sheet by sheet: the new function opens on its own sheet - its body, nothing else)
check(l.sheet === l.functions[0].name && l.sheets?.length === 1 && l.nodes.length === 2, `the new function's sheet is open (${l.sheet})`);
// Details shows the function with its name ready to type over
const name = page.locator('.lg-dbody [data-fk="name"], [data-fk="name"]').first();
check(await until(async () => await name.count() === 1), "Details shows the function");
await name.fill("Twice");
await name.press("Enter");
check(await until(async () => (await logic()).functions[0].name === "Twice"), "typing renames it");
check((await logic()).sheet === "Twice", "its sheet stays open under the new name");
check(await until(async () => await page.locator('.lg-fn[data-fn="Twice"]').count() === 1), "My Blueprint lists it by its name");
await page.click('[data-act="fn-in"]');
await page.click('[data-act="fn-out"]');
l = await until(async () => { const x = await logic(); return x.functions[0].inputs.length === 1 && x.functions[0].outputs.length === 1 && x; });
check(!!l && l.functions[0].inputs[0].name === "value" && l.functions[0].outputs[0].name === "result", "+ Input and + Output add its pins");
const pin = page.locator('.lg-frow[data-side="in"] [data-pf="name"]');
// (Details has drawn both pins: a field filled while the panel is still being redrawn is gone with the old panel)
await until(async () => await page.locator('.lg-dbody .lg-frow[data-side="out"]').count() === 1 && await pin.count() === 1 && await pin.inputValue() === "value");
await pin.fill("x");
await pin.press("Enter");
check(await until(async () => (await logic()).functions[0].inputs[0].name === "x"), "a pin is renamed in place");
l = await logic();
const entry = l.nodes.find(n => n.type === "func.entry"), ret = l.nodes.find(n => n.type === "func.return");
check(entry.title === "Function Twice" && entry.outputs.some(p => p.id === "x") && ret.inputs.some(p => p.id === "result"), "the entry gives the input, the Return takes the output");
// a Formula between them: its names are its pins
const formula = (await cmd({cmd: "logicAdd", type: "math.formula", x: entry.x + 220, y: entry.y + 90, params: {formula: "x * 2.0f + bonus", types: "f32 f32"}})).id;
l = await logic();
const fnode = l.nodes.find(n => n.id === formula);
check(fnode.title === "x * 2.0f + bonus" && fnode.inputs.map(p => p.id).join() === "x,bonus", "a Formula node takes its inputs from the formula");
await cmd({cmd: "logicConnect", from: entry.id, out: "x", to: formula, in: "x"});
await cmd({cmd: "logicConnect", from: formula, out: "result", to: ret.id, in: "result"});
await cmd({cmd: "logicValue", node: formula, pin: "bonus", value: 0.5});
// a formula that cannot be read is an error that says why; fixed, the error is gone
await cmd({cmd: "logicParam", node: formula, key: "formula", value: "x * 2 +"});
check(errorsOf(await logic()).some(t => /ends too early/.test(t)), "a broken formula is a compile error that says why");
await cmd({cmd: "logicParam", node: formula, key: "formula", value: "x * 2.0f + bonus"});
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
// (the function gives an f32, Show Message takes a Text: the wire gets its To Text, in the open - no wire converts)
const toText = await cmd({cmd: "logicConnect", from: call.id, out: "result", to: msg, in: "text"});
check(toText.ok && toText.title === "To Text" && (await logic()).nodes.find(n => n.id === toText.inserted)?.type === "num.text", "a number into a Text pin gets its To Text on the wire");
await cmd({cmd: "logicValue", node: call.id, pin: "x", value: 20});
check(errorsOf(await logic()).length === 0, "the graph compiles");

// ---- 1b. the same Blueprint as C++: read it, change it, apply it ----------------------------------
await page.click(".lg-cpp");
const cppText = page.locator(".lg-cpptext");
check(await until(async () => (await cppText.inputValue()).includes("TwiceResult Twice(f32 x = 0.0f) {")), "the C++ button shows the Blueprint as C++");
let code = await cppText.inputValue();
check(code.includes("return {kern::formula<f32, f32>([](auto x, auto bonus) { return x * 2.0f + bonus; }, x, 0.5f)};"), "the Formula node is the expression itself");
check(/n\d+ = Twice\(20\.0f\);/.test(code) && code.includes("game_message(num_text<f32>(n") && code.includes("KERN_EVENT(on") && !code.includes("KERN_LAYOUT") && !/call_node|read_pin/.test(code), "the call is a call, the event a method; no layout, no callbacks");
// (an event, a call, Show Message - a gameplay node: a compiler runs this text as it stands, and the view says so)
const nativeNote = page.locator(".lg-cppnative");
check((await nativeNote.textContent()) === "runs as compiled C++" && /every node is a function/.test(await nativeNote.getAttribute("title")), "the view says that a compiler runs the text as it stands");
// a change typed into the text becomes the graph (Ctrl+Enter), and the nodes stay where they are
const places = async () => JSON.stringify((await logic()).nodes.map(n => [n.id, n.x, n.y]).sort((a, b) => a[0] - b[0]));
const placesBefore = await places();
await cppText.fill(code.replace("x, 0.5f)};", "x, 1.5f)};"));
check(await page.locator(".lg-cppstate.dirty").count() === 1, "a changed text says so");
await cppText.press("Control+Enter");
check(await until(async () => (await logic()).nodes.find(n => n.id === formula)?.inputs.find(p => p.id === "bonus")?.value === 1.5), "Apply changes the Blueprint: the Formula's bonus is 1.5");
check(await places() === placesBefore, "every node kept its id and its place");
check(await page.locator(".lg-cppstate.dirty").count() === 0 && (await cppText.inputValue()).includes("x, 1.5f)};"), "the view shows the Blueprint as it now is");
// what is not a Blueprint is refused with its line, and nothing changes
await cppText.fill((await cppText.inputValue()).replace("Twice(20.0f);", "Twice(20.0f, 1.0f);"));
await cppText.press("Control+Enter");
check(await until(async () => /^line \d+, column \d+: /.test(await page.locator(".lg-cpperr").innerText().catch(() => ""))), `a text that is no Blueprint is refused with line and reason (${await page.locator(".lg-cpperr").innerText().catch(() => "")})`);
check((await logic()).nodes.find(n => n.id === formula)?.inputs.find(p => p.id === "bonus")?.value === 1.5, "and the Blueprint is untouched");
await page.click(".lg-cppreset");
check(await page.locator(".lg-cpperr").isHidden() && (await cppText.inputValue()).includes("Twice(20.0f);"), "Reset shows the Blueprint's text again");
// back to 0.5 through the text, then back to the graph
await cppText.fill((await cppText.inputValue()).replace("x, 1.5f)};", "x, 0.5f)};"));
await page.click(".lg-cppapply");
check(await until(async () => (await logic()).nodes.find(n => n.id === formula)?.inputs.find(p => p.id === "bonus")?.value === 0.5), "the Apply button does the same");
await page.click(".lg-cppclose");
check(await page.locator(".lg-cppview").isHidden() && errorsOf(await logic()).length === 0, "Graph closes the view; the Blueprint compiles");
await cmd({cmd: "logicClose"});
await cmd({cmd: "play"});
check(await until(async () => (await status()).hud?.message === "40.5", 60000), `Play calls the function: 20 * 2 + 0.5 on the screen (${(await status()).hud?.message})`);
check(await until(async () => (await status()).logic?.compiled >= 1, 30000), "as machine code (JIT)");
await cmd({cmd: "stop"});

// ---- 1c. a list: its items in Details, nodes in its type, a loop over it, played -------------------
await cmd({cmd: "new", template: "basic"});
await cmd({cmd: "logicOpen", owner: 0});
await until(async () => await page.locator("#logicEditor:not([hidden]) .lg-canvas").count() === 1);
await cmd({cmd: "logicVariable", op: "add", name: "Scores", type: "f32"});
await page.click('.lg-var[data-var="Scores"]');
const typeSelect = page.locator('.lg-dbody [data-vk="type"]');
check(await until(async () => await typeSelect.count() === 1) && (await typeSelect.locator("option").count()) === 30 && (await typeSelect.locator('option[value="u32"]').textContent()) === "u32",
      "Details offers the ten number types, bool, Vector, Text, Actor, a list of each, a map and a set");
check((await typeSelect.locator("option").allTextContents()).slice(0, 3).join("|") === "Float (f32)|Int (i64)|f64" && (await typeSelect.locator("option:checked").textContent()) === "Float (f32)",
      "first the defaults Float (f32) and Int (i64), the other widths below them");
await typeSelect.selectOption("f32_list");
check(await until(async () => (await logic()).variables.find(v => v.name === "Scores")?.type === "f32_list"), "a variable becomes a Float List (f32)");
const scores = async () => JSON.stringify((await logic()).variables.find(v => v.name === "Scores")?.value);
check(await until(async () => await page.locator(".lg-dbody .lg-list [data-ladd]").count() === 1) && await scores() === "[]", "its default is the empty list, with + Item below");
// (the rows are drawn from the Blueprint: each click waits for its row)
const rows = n => until(async () => await page.locator(".lg-dbody .lg-li").count() === n);
for (const n of [1, 2, 3]) {
  await page.click(".lg-dbody .lg-list [data-ladd]");
  await rows(n);
}
check(await scores() === "[0,0,0]" && await rows(3), `+ Item adds items (${await scores()})`);
for (const [i, v] of [[0, "1.5"], [1, "40"], [2, "7"]]) {
  const field = page.locator(`.lg-dbody .lg-list [data-li="${i}"]`);
  await field.fill(v);
  await field.press("Enter");
  await until(async () => JSON.parse(await scores())[i] === +v);
}
check(await scores() === "[1.5,40,7]", `typing sets an item (${await scores()})`);
await page.click('.lg-dbody .lg-list [data-lx="2"]');
check(await rows(2) && await scores() === "[1.5,40]", "x takes an item out");
// Get Scores; from its pin the menu's nodes come in the list's type; a new Length takes the wire's type
const getScores = (await cmd({cmd: "logicAdd", type: "var.get", x: 0, y: 320, params: {variable: "Scores"}})).id;
const offered = (await cmd({cmd: "logicPalette", query: "", from: {node: getScores, pin: "value", output: true}})).items;
check(offered.some(e => e.type === "flow.for_each" && e.params?.type === "f32") && offered.some(e => e.type === "list.get" && e.params?.type === "f32") && !offered.some(e => e.type === "math.add"),
      "from an f32 List pin the menu offers For Each and Get Item of f32, and nothing a list does not go into");
const lengthNode = (await cmd({cmd: "logicAdd", type: "list.length", x: 320, y: 420, params: {type: "text"}})).id;
await cmd({cmd: "logicConnect", from: getScores, out: "value", to: lengthNode, in: "list"});
l = await logic();
check(l.nodes.find(n => n.id === lengthNode).inputs[0].type === "f32_list" && l.links.some(k => k[0] === getScores && k[2] === lengthNode), "a Length of Texts with nothing wired to it becomes a Length of f32 with the wire");
check(await until(async () => await page.locator(`.lg-node[data-id="${getScores}"] .lg-pin.list`).count() === 1), "a list pin is drawn as a list");
// Begin Play: Add 0.5 to Scores, then for each item add it to Sum; then show Sum and the list
await cmd({cmd: "logicVariable", op: "add", name: "Sum", type: "f32"});
const lb = (await cmd({cmd: "logicAdd", type: "event.begin", x: 0, y: 0})).id;
const addStep = (await cmd({cmd: "logicAdd", type: "list.add", x: 280, y: 0, params: {variable: "Scores"}})).id;
const each = (await cmd({cmd: "logicAdd", type: "flow.for_each", x: 560, y: 0, from: {node: getScores, pin: "value", output: true}})).id;
const addSum = (await cmd({cmd: "logicAdd", type: "var.add", x: 840, y: 0, params: {variable: "Sum"}})).id;
const getSum = (await cmd({cmd: "logicAdd", type: "var.get", x: 840, y: 240, params: {variable: "Sum"}})).id;
const text3 = (await cmd({cmd: "logicAdd", type: "text.append", x: 1000, y: 240})).id;
const show = (await cmd({cmd: "logicAdd", type: "game.message", x: 1200, y: 120})).id;
l = await logic();
check(l.nodes.find(n => n.id === addStep).title === "Add to Scores" && l.nodes.find(n => n.id === addStep).inputs.find(p => p.id === "item").type === "f32", "Add to Scores takes an item of the list's type");
check(l.nodes.find(n => n.id === each).outputs.find(p => p.id === "item").type === "f32" && l.links.some(k => k[0] === getScores && k[2] === each), "For Each dropped from the list's pin is wired and gives f32");
await cmd({cmd: "logicValue", node: addStep, pin: "item", value: 0.5});
for (const [from, out, to, input] of [[lb, "then", addStep, "exec"], [addStep, "then", each, "exec"], [each, "body", addSum, "exec"], [each, "item", addSum, "amount"], [each, "completed", show, "exec"],
                                      [getSum, "value", text3, "a"], [getScores, "value", text3, "c"], [text3, "result", show, "text"]]) {
  const r = await cmd({cmd: "logicConnect", from, out, to, in: input});
  // (a number and a list into Append's Text pins: To Text and List to Text on the wires)
  if (input === "a") check(r.title === "To Text", "an f32 into a Text pin: To Text on the wire");
  if (input === "c") check(r.title === "List to Text", "a list into a Text pin: List to Text on the wire");
}
await cmd({cmd: "logicValue", node: text3, pin: "b", value: " of "});
check(errorsOf(await logic()).length === 0, "the list graph compiles" + (errorsOf(await logic()).length ? ": " + errorsOf(await logic()).join(" | ") : ""));
await page.click(".lg-cpp");
check(await until(async () => (await cppText.inputValue()).includes("List<f32> Scores = List<f32>{1.5f, 40.0f};")), "the C++ view has the list variable with its items");
code = await cppText.inputValue();
// (Get Scores is read by inputs of several bodies - the loop, the message, the Length nothing reads: a value with a name, n<id>())
check(code.includes("list_add(Scores, 0.5f);") && /for \(auto it\d+ = flow_for_each<f32>\(n\d+, n\d+\(\)\); it\d+\.next\(\);\) \{/.test(code) && /list_text<f32>\(n\d+\(\)\)/.test(code) && code.includes("num_text<f32>(Sum)") &&
      /auto n\d+\(\) \{ return Scores; \}/.test(code), "Add to List names its variable, For Each is a for loop, a list and a number become Text by their nodes");
check((await nativeNote.textContent()) === "runs as compiled C++", "a compiler runs it as it stands");
// an item added in the text is an item in Details
await cppText.fill(code.replace("List<f32>{1.5f, 40.0f};", "List<f32>{1.5f, 40.0f, 0.25f};"));
await cppText.press("Control+Enter");
check(await until(async () => await scores() === "[1.5,40,0.25]"), "a list changed in the text is the Blueprint's list");
await page.click(".lg-cppclose");
await cmd({cmd: "logicClose"});
await cmd({cmd: "play"});
check(await until(async () => (await status()).hud?.message === "42.25 of [1.5, 40, 0.25, 0.5]", 60000), `Play adds to the list and loops over it (${(await status()).hud?.message})`);
await cmd({cmd: "stop"});

// ---- 1d. an enum: its values in Details, a variable of its type, Switch on Enum, played -----------
await cmd({cmd: "new", template: "basic"});
await cmd({cmd: "logicOpen", owner: 0});
await until(async () => await page.locator("#logicEditor:not([hidden]) .lg-canvas").count() === 1);
const enums = async () => (await logic()).enums;
const enumValues = async () => ((await enums())[0]?.values || []).join(",");
await page.click('.lg-sec[data-sec="enums"] [data-add]');
check(await until(async () => (await enums()).length === 1 && await page.locator('.lg-dbody [data-ek="name"]').count() === 1) && await enumValues() === "First,Second", "+ in My Blueprint adds an enum with two values, shown in Details");
await page.locator('.lg-dbody [data-ek="name"]').fill("Mode");
await page.locator('.lg-dbody [data-ek="name"]').press("Enter");
check(await until(async () => (await enums())[0]?.name === "Mode" && await page.locator('.lg-enum[data-enum="Mode"].on').count() === 1), "the enum is renamed in Details");
const valueField = i => page.locator(`.lg-dbody .lg-erow[data-index="${i}"] [data-ev="name"]`);
for (const [i, name] of [[0, "Closed"], [1, "Open"]]) {
  await valueField(i).fill(name);
  await valueField(i).press("Enter");
  await until(async () => (await enumValues()).split(",")[i] === name);
}
await page.click('.lg-dbody [data-act="enum-value"]');
await until(async () => await page.locator(".lg-dbody .lg-erow").count() === 3);
await valueField(2).fill("Opening");
await valueField(2).press("Enter");
check(await until(async () => await enumValues() === "Closed,Open,Opening"), `values are renamed and added (${await enumValues()})`);
await valueField(2).fill("Open");
await valueField(2).press("Enter");
check(await until(async () => await valueField(2).inputValue() === "Opening") && await enumValues() === "Closed,Open,Opening", "a name another value has is refused");
// a variable of the enum, set to Open; then Opening moves up: the variable is still Open
await cmd({cmd: "logicVariable", op: "add", name: "State", type: "f32"});
await page.click('.lg-var[data-var="State"]');
const enumType = page.locator('.lg-dbody [data-vk="type"]');
check(await until(async () => await enumType.count() === 1) && (await enumType.locator("option").count()) === 32 && (await enumType.locator('option[value="Mode_list"]').textContent()) === "Mode List",
      "a variable's Type offers the enum and a list of it");
await enumType.selectOption("Mode");
const stateVar = async () => (await logic()).variables.find(v => v.name === "State");
check(await until(async () => (await stateVar())?.of === "Mode" && await page.locator('.lg-dbody select[data-vk="value"] option').count() === 3), "its default is chosen among the enum's values");
await page.locator('.lg-dbody select[data-vk="value"]').selectOption({label: "Open"});
check(await until(async () => (await stateVar())?.value === 1), "the default is Open");
await page.click('.lg-enum[data-enum="Mode"]');
await page.click('.lg-dbody .lg-erow[data-index="2"] [data-ev="up"]');
check(await until(async () => await enumValues() === "Closed,Opening,Open") && (await stateVar()).value === 2, "a value moves up, and what was set stays with its value");
// Begin Play: State = Opening (chosen on the node); Switch on Mode; at Opening the message says the value's name
await page.hover('.lg-enum[data-enum="Mode"]');  // (a row's buttons show under the cursor)
await page.click('.lg-enum[data-enum="Mode"] [data-switch]');
check(await until(async () => (await logic()).nodes.some(n => n.type === "flow.switch_enum")), "Switch in My Blueprint adds a Switch on the enum");
l = await logic();
const sw = l.nodes.find(n => n.type === "flow.switch_enum");
check(sw.title === "Switch on Mode" && sw.outputs.map(o => o.label).join(",") === "Closed,Opening,Open" && sw.inputs.find(q => q.id === "value").of === "Mode", "it has an exec output for each value");
const eb = (await cmd({cmd: "logicAdd", type: "event.begin", x: 0, y: 0})).id;
const setState = (await cmd({cmd: "logicAdd", type: "var.set", x: 240, y: 0, params: {variable: "State"}})).id;
const getState = (await cmd({cmd: "logicAdd", type: "var.get", x: 240, y: 260, params: {variable: "State"}})).id;
const fromPin = (await cmd({cmd: "logicPalette", query: "", from: {node: getState, pin: "value", output: true}})).items;
check(fromPin.some(e => e.type === "enum.name" && e.params?.enum === "Mode") && fromPin.some(e => e.type === "logic.select" && e.params?.type === "Mode") && !fromPin.some(e => e.type === "debug.print" || e.type === "math.add"),
      "from a Mode pin the menu offers the enum's nodes and Select of a Mode, and nothing an enum does not go into");
const nameOf = (await cmd({cmd: "logicAdd", type: "enum.name", x: 520, y: 260, params: {enum: "Mode"}})).id;
const join = (await cmd({cmd: "logicAdd", type: "text.append", x: 760, y: 260})).id;
const say = (await cmd({cmd: "logicAdd", type: "game.message", x: 1000, y: 100})).id;
const literal = page.locator(`.lg-node[data-id="${setState}"] select.lg-lit.enm`);
check(await until(async () => await literal.count() === 1) && (await literal.locator("option").count()) === 3, "an enum pin without a wire has a dropdown of the values");
await literal.selectOption({label: "Opening"});
check(await until(async () => (await logic()).nodes.find(n => n.id === setState).inputs.find(q => q.id === "value").value === 1), "choosing sets the pin's value");
check(await until(async () => await page.locator(`.lg-node[data-id="${getState}"] .lg-row[data-type="Mode"]`).count() === 1), "an enum pin is of its enum's type");
for (const [from, out, to, input] of [[eb, "then", setState, "exec"], [setState, "then", sw.id, "exec"], [getState, "value", sw.id, "value"], [sw.id, "case1", say, "exec"],
                                      [getState, "value", nameOf, "value"], [nameOf, "name", join, "b"], [join, "result", say, "text"]])
  await cmd({cmd: "logicConnect", from, out, to, in: input});
await cmd({cmd: "logicValue", node: join, pin: "a", value: "is "});
check(!(await cmd({cmd: "logicConnect", from: getState, out: "value", to: join, in: "c"})).ok, "an enum's value is no text: it does not go into a Text pin");
check(errorsOf(await logic()).length === 0, "the enum graph compiles" + (errorsOf(await logic()).length ? ": " + errorsOf(await logic()).join(" | ") : ""));
await page.click(".lg-cpp");
check(await until(async () => (await cppText.inputValue()).includes("KERN_ENUM(Mode, Closed, Opening, Open);")), "the C++ view declares the enum");
code = await cppText.inputValue();
check(code.includes("Mode State = Mode::Open;") && code.includes("State = Mode::Opening;") && /switch \(flow_switch_enum<Mode>\(n\d+\(\)\)\) \{\n\s+case Mode::Closed: \{\n\s+\} break;\n\s+case Mode::Opening: \{/.test(code) &&
      /enum_name<Mode>\(n\d+\(\)\)/.test(code), "a variable of the enum, its values by name, a switch with a case for every value");
check((await nativeNote.textContent()) === "runs as compiled C++", "a compiler runs it as it stands");
// a value renamed in the text is renamed in the Blueprint
await cppText.fill(code.replaceAll("Opening", "Ajar"));
await cppText.press("Control+Enter");
check(await until(async () => await enumValues() === "Closed,Ajar,Open"), "an enum changed in the text is the Blueprint's enum");
await page.click(".lg-cppclose");
check(await until(async () => await page.locator(`.lg-node[data-id="${sw.id}"]`).count() === 0 || (await logic()).nodes.some(n => n.type === "flow.switch_enum" && n.outputs[1].label === "Ajar")), "the Switch shows the new name");
await cmd({cmd: "logicClose"});
await cmd({cmd: "play"});
check(await until(async () => (await status()).hud?.message === "is Ajar", 60000), `Play switches on the enum and says the value's name (${(await status()).hud?.message})`);
await cmd({cmd: "stop"});

// ---- 1e. a struct: its fields in Details, a variable of its type, Make / Break / Set of a field, played ----
// (The editor draws a change with its next frame: what is asked of the page comes after it.)
const settle = async fn => {
  const ok = await until(fn);
  const frame = await page.evaluate(() => window.KernStudio.frames());
  await page.waitForFunction(f => window.KernStudio.frames() >= f + 2, frame);
  return ok;
};
await cmd({cmd: "new", template: "basic"});
await cmd({cmd: "logicOpen", owner: 0});
await settle(async () => await page.locator("#logicEditor:not([hidden]) .lg-canvas").count() === 1);
const structs = async () => (await logic()).structs;
const fieldsOf = async name => (((await structs()).find(s => s.name === name))?.fields || []).map(f => f.name + ":" + (f.of ? f.of + (f.type.endsWith("_list") ? "[]" : "") : f.type)).join(",");
await page.click('.lg-sec[data-sec="structs"] [data-add]');
check(await settle(async () => (await structs()).length === 1 && await page.locator('.lg-dbody [data-stk="name"]').count() === 1) && await fieldsOf("NewStruct") === "Field:f32",
      "+ in My Blueprint adds a struct with one field, shown in Details");
await page.locator('.lg-dbody [data-stk="name"]').fill("Weapon");
await page.locator('.lg-dbody [data-stk="name"]').press("Enter");
check(await settle(async () => (await structs())[0]?.name === "Weapon" && await page.locator('.lg-st[data-struct="Weapon"].on').count() === 1), "the struct is renamed in Details");
const fieldRow = name => page.locator(`.lg-dbody .lg-srow[data-field="${name}"]`);
const renameField = async (from, to) => { await fieldRow(from).locator('[data-sf="name"]').fill(to); await fieldRow(from).locator('[data-sf="name"]').press("Enter"); };
await renameField("Field", "Damage");
check(await settle(async () => await fieldsOf("Weapon") === "Damage:f32"), "a field is renamed in place");
await fieldRow("Damage").locator('[data-sf="value"]').fill("10");
await fieldRow("Damage").locator('[data-sf="value"]').press("Enter");
check(await settle(async () => (await structs())[0].fields[0].value === 10), "and given its default");
await page.click('.lg-dbody [data-act="struct-field"]');
await settle(async () => await page.locator(".lg-dbody .lg-srow").count() === 2);
await renameField("Field", "Label");
await settle(async () => await fieldRow("Label").count() === 1);
await fieldRow("Label").locator('[data-sf="type"]').selectOption("text");
await settle(async () => await fieldsOf("Weapon") === "Damage:f32,Label:text");
await fieldRow("Label").locator('[data-sf="value"]').fill("Fist");
await fieldRow("Label").locator('[data-sf="value"]').press("Enter");
check(await settle(async () => (await structs())[0].fields[1]?.value === "Fist") && await fieldsOf("Weapon") === "Damage:f32,Label:text", `+ Field adds a field; its name, type and default are set in its row (${await fieldsOf("Weapon")})`);
await renameField("Label", "Damage");
check(await settle(async () => await fieldRow("Label").locator('[data-sf="name"]').inputValue() === "Label") && await fieldsOf("Weapon") === "Damage:f32,Label:text", "a name another field has is refused");
// a field of an enum and one of another struct: the types a field can have, a struct inside a struct
await cmd({cmd: "logicEnum", op: "add", name: "Kind"});
await cmd({cmd: "logicStruct", op: "add", name: "Aim"});
await cmd({cmd: "logicStruct", op: "renameField", name: "Aim", field: "Field", to: "Tries"});
await cmd({cmd: "logicStruct", op: "fieldType", name: "Aim", field: "Tries", type: "i64"});
await cmd({cmd: "logicStruct", op: "fieldValue", name: "Aim", field: "Tries", value: 3});
await page.click('.lg-dbody [data-act="struct-field"]');
await settle(async () => await page.locator(".lg-dbody .lg-srow").count() === 3);
await renameField("Field", "Type");
await settle(async () => await fieldRow("Type").count() === 1);
const fieldType = fieldRow("Type").locator('[data-sf="type"]');
check((await fieldType.locator("option").count()) === 34 && (await fieldType.locator('option[value="Aim_list"]').textContent()) === "Aim List" && (await fieldType.locator('option[value="Weapon"]').count()) === 0,
      "a field's Type offers the value types, the enums, the other structs and lists of them - not the struct itself");
await fieldType.selectOption("Kind");
check(await settle(async () => await fieldsOf("Weapon") === "Damage:f32,Label:text,Type:Kind" && await fieldRow("Type").locator('select[data-sf="value"] option').count() === 2), "a field of an enum: its default is one of the values");
await page.click('.lg-dbody [data-act="struct-field"]');
await settle(async () => await page.locator(".lg-dbody .lg-srow").count() === 4);
await renameField("Field", "Sight");
await settle(async () => await fieldRow("Sight").count() === 1);
await fieldRow("Sight").locator('[data-sf="type"]').selectOption("Aim");
const sightDefault = page.locator(`.lg-dbody .lg-vsub[data-sfield="Sight"] [data-vp='["Tries"]']`);
check(await settle(async () => await fieldsOf("Weapon") === "Damage:f32,Label:text,Type:Kind,Sight:Aim" && await sightDefault.count() === 1) && (await sightDefault.inputValue()) === "3" &&
      (await structs()).map(s => s.name).join(",") === "Aim,Weapon", "a field of another struct: its default is edited field by field, and that struct stands above");
await fieldRow("Sight").locator('[data-sf="up"]').click();
check(await settle(async () => await fieldsOf("Weapon") === "Damage:f32,Label:text,Sight:Aim,Type:Kind"), "a field moves up");
// a variable of the struct: its default field by field
await cmd({cmd: "logicVariable", op: "add", name: "Held", type: "f32"});
await page.click('.lg-var[data-var="Held"]');
const heldType = page.locator('.lg-dbody [data-vk="type"]');
check(await settle(async () => await heldType.count() === 1) && (await heldType.locator("option").count()) === 36 && (await heldType.locator('option[value="Weapon_list"]').textContent()) === "Weapon List",
      "a variable's Type offers the structs and lists of them");
await heldType.selectOption("Weapon");
const held = async () => (await logic()).variables.find(v => v.name === "Held");
const heldField = path => page.locator(`.lg-dbody .lg-vsub[data-var="Held"] [data-vp='${JSON.stringify(path)}']`);
check(await settle(async () => (await held())?.of === "Weapon" && await heldField(["Damage"]).count() === 1) && (await held()).value.Damage === 10 && (await held()).value.Label === "Fist" && (await held()).value.Sight.Tries === 3,
      "a variable of the struct starts at the fields' defaults, shown field by field");
await heldField(["Damage"]).fill("25");
await heldField(["Damage"]).press("Enter");
await settle(async () => (await held()).value.Damage === 25);
await heldField(["Sight", "Tries"]).fill("7");
await heldField(["Sight", "Tries"]).press("Enter");
await settle(async () => (await held()).value.Sight.Tries === 7);
await heldField(["Type"]).selectOption({label: "Second"});
check(await settle(async () => (await held()).value.Type === 1) && (await held()).value.Damage === 25 && (await held()).value.Sight.Tries === 7 && (await held()).value.Label === "Fist",
      "a field of the default is edited by itself - also one of the struct inside");
// a field's new name: what the variable holds stays with the field
await page.click('.lg-st[data-struct="Weapon"]');
await settle(async () => await fieldRow("Damage").count() === 1);
await renameField("Damage", "Power");
check(await settle(async () => await fieldsOf("Weapon") === "Power:f32,Label:text,Sight:Aim,Type:Kind") && (await held()).value.Power === 25 && !("Damage" in (await held()).value), "a renamed field keeps its values");
// Begin Play: Set Held.Power (typed on the node); the message says Break's Label and Power
await page.hover('.lg-st[data-struct="Weapon"]');  // (a row's buttons show under the cursor)
await page.click('.lg-st[data-struct="Weapon"] [data-make]');
check(await settle(async () => (await logic()).nodes.some(n => n.type === "struct.make")), "Make in My Blueprint adds a Make node of the struct");
l = await logic();
const makeNode = l.nodes.find(n => n.type === "struct.make");
check(makeNode.title === "Make Weapon" && makeNode.inputs.map(q => q.id).join(",") === "Power,Label,Sight,Type" && makeNode.outputs[0].of === "Weapon" && makeNode.inputs[2].value.Tries === 3, "it has an input for each field, at the field's default");
check(await page.locator(`.lg-node[data-id="${makeNode.id}"] .lg-row[data-type="Aim"] .lg-scount`).count() === 1 && await page.locator(`.lg-node[data-id="${makeNode.id}"] .lg-row.out[data-type="Weapon"]`).count() === 1,
      "a struct's pin is of its struct's type; without a wire it shows what it holds in short");
// (a struct typed into a pin is edited in Details, field by field: the new node is the selection)
const makeSight = page.locator(`.lg-dbody .lg-vsub[data-node="${makeNode.id}"][data-pin="Sight"] [data-vp='["Tries"]']`);
check(await settle(async () => await makeSight.count() === 1), "Details shows the fields of a struct pin");
await makeSight.fill("9");
await makeSight.press("Enter");
check(await settle(async () => (await logic()).nodes.find(n => n.id === makeNode.id).inputs.find(q => q.id === "Sight").value.Tries === 9), "typing there sets the pin's value");
const ebs = (await cmd({cmd: "logicAdd", type: "event.begin", x: 0, y: 300})).id;
const tune = (await cmd({cmd: "logicAdd", type: "var.set", x: 240, y: 300, params: {variable: "Held", path: ".Power"}})).id;
const getHeld = (await cmd({cmd: "logicAdd", type: "var.get", x: 240, y: 520, params: {variable: "Held"}})).id;
const brk = (await cmd({cmd: "logicAdd", type: "struct.break", x: 480, y: 520, params: {struct: "Weapon"}})).id;
const joined = (await cmd({cmd: "logicAdd", type: "text.append", x: 760, y: 520})).id;
const says = (await cmd({cmd: "logicAdd", type: "game.message", x: 1000, y: 300})).id;
l = await logic();
const tuneNode = l.nodes.find(n => n.id === tune);
check(tuneNode.title === "Set Held.Power" && tuneNode.params.find(q => q.key === "path").value === ".Power" && tuneNode.params.find(q => q.key === "path").kind === "path" && tuneNode.inputs.find(q => q.id === "value").type === "f32",
      "a Set names a variable and the path to the place in it - one of its fields; its value is of the field's type");
const fromHeld = (await cmd({cmd: "logicPalette", query: "", from: {node: getHeld, pin: "value", output: true}})).items;
check(fromHeld.some(e => e.type === "struct.break" && e.params?.struct === "Weapon") && fromHeld.some(e => e.type === "logic.select" && e.params?.type === "Weapon") && !fromHeld.some(e => e.type === "debug.print" || e.type === "math.add" || e.params?.struct === "Aim"),
      "from a Weapon pin the menu offers Break Weapon and Select of a Weapon, and nothing a struct does not go into");
const power = page.locator(`.lg-node[data-id="${tune}"] input.lg-lit.num`);
await power.fill("12.5");
await power.press("Enter");
check(await settle(async () => (await logic()).nodes.find(n => n.id === tune).inputs.find(q => q.id === "value").value === 12.5), "the field's new value is typed on the node");
for (const [from, out, to, input] of [[ebs, "then", tune, "exec"], [tune, "then", says, "exec"], [getHeld, "value", brk, "value"], [brk, "Label", joined, "a"], [brk, "Power", joined, "c"], [joined, "result", says, "text"]])
  await cmd({cmd: "logicConnect", from, out, to, in: input});
await cmd({cmd: "logicValue", node: joined, pin: "b", value: ": "});
check(!(await cmd({cmd: "logicConnect", from: getHeld, out: "value", to: joined, in: "b"})).ok && !(await cmd({cmd: "logicConnect", from: brk, out: "Sight", to: tune, in: "value"})).ok,
      "a struct is no text and no other struct: it goes into its own type only");
check(errorsOf(await logic()).length === 0, "the struct graph compiles" + (errorsOf(await logic()).length ? ": " + errorsOf(await logic()).join(" | ") : ""));
await page.click(".lg-cpp");
check(await settle(async () => (await cppText.inputValue()).includes("KERN_STRUCT(Weapon) {")), "the C++ view declares the struct");
code = await cppText.inputValue();
check(code.includes("  KERN_STRUCT(Aim) {\n    i64 Tries = 3;\n  };\n  KERN_STRUCT(Weapon) {\n    f32 Power = 10.0f;\n    Text Label = \"Fist\";\n    Aim Sight = Aim{3};\n    Kind Type = Kind::First;\n  };\n") &&
      code.includes("Weapon Held = Weapon{25.0f, \"Fist\", Aim{7}, Kind::Second};") && code.includes("Held.Power = 12.5f;") && /struct_break<Weapon>\(Held\)/.test(code) && /n\d+\(\)\.Label/.test(code) &&
      /num_text<f32>\(n\d+\(\)\.Power\)/.test(code) && code.includes("struct_make<Weapon>(10.0f, \"Fist\", Aim{9}, Kind::First)"),
      "the structs with their fields, a variable written out with every field, the Set of a field as an assignment, Break read by the field's name");
check((await nativeNote.textContent()) === "runs as compiled C++", "a compiler runs it as it stands");
// a field renamed in the text is renamed in the Blueprint
await cppText.fill(code.replaceAll("Label", "Title"));
await cppText.press("Control+Enter");
check(await settle(async () => await fieldsOf("Weapon") === "Power:f32,Title:text,Sight:Aim,Type:Kind"), "a struct changed in the text is the Blueprint's struct");
await page.click(".lg-cppclose");
check(await settle(async () => (await logic()).nodes.some(n => n.type === "struct.break" && n.outputs[1].id === "Title")), "Break shows the new name");
await cmd({cmd: "logicClose"});
await cmd({cmd: "play"});
check(await settle(async () => (await status()).hud?.message === "Fist: 12.5", 60000), `Play sets the field and reads the struct's fields (${(await status()).hud?.message})`);
await cmd({cmd: "stop"});

// ---- 1f. a data table: a list of structs shown as a table, a row looked up by its key, played ------
await cmd({cmd: "new", template: "basic"});
await cmd({cmd: "logicOpen", owner: 0});
await until(async () => await page.locator("#logicEditor:not([hidden]) .lg-canvas").count() === 1);
check((await cmd({cmd: "logicCppApply", text: `struct Level : kern::Blueprint {
  KERN_STRUCT(Item) {
    Text Key = "";
    f32 Price = 1.0f;
    bool Fresh = true;
  };
  List<Item> Shop = List<Item>{Item{"apple", 2.5f, true}, Item{"pear", 4.0f, false}};

  KERN_EVENT(on1, event_begin)
  void on1() {
    game_message(text_append("pear costs ", num_text<f32>(struct_break<Item>(list_find_row<&Item::Key>(Shop, "pear").row).Price), ""));
  }
};
`})).ok, "a table and a lookup written as C++ are a Blueprint");
check(errorsOf(await logic()).length === 0, "the table graph compiles" + (errorsOf(await logic()).length ? ": " + errorsOf(await logic()).join(" | ") : ""));
await page.click('.lg-var[data-var="Shop"]');
const tableRows = n => until(async () => await page.locator(".lg-dbody .lg-table tbody tr[data-row]").count() === n);
check(await tableRows(2) && (await page.locator(".lg-dbody .lg-table thead th").allTextContents()).join("|").includes("Key|Price|Fresh"), "a list of structs is shown as a table: a row per item, a column per field");
const shop = async () => (await logic()).variables.find(v => v.name === "Shop")?.value;
const price = page.locator('.lg-dbody .lg-table tr[data-row="1"] td input:not([type=checkbox])').nth(1);
await price.fill("6.5");
await price.press("Enter");
check(await until(async () => (await shop())?.[1]?.Price === 6.5) && (await shop())[0].Key === "apple", "a cell sets that field of that row");
await page.click(".lg-dbody .lg-tablebox [data-vadd]");
check(await tableRows(3) && (await shop())[2]?.Price === 1, "+ Row adds a row with the struct's defaults");
await page.click('.lg-dbody .lg-table tr[data-row="0"] [data-vx]');
check(await tableRows(2) && (await shop())[0].Key === "pear", "x takes a row out");
await cmd({cmd: "logicClose"});
await cmd({cmd: "play"});
check(await until(async () => (await status()).hud?.message === "pear costs 6.5", 60000), `Play looks the row up by its key (${(await status()).hud?.message})`);
await cmd({cmd: "stop"});

// ---- 1g. local variables of a function: made in Details, used as nodes, played -------------------
await cmd({cmd: "new", template: "basic"});
await cmd({cmd: "logicOpen", owner: 0});
await until(async () => await page.locator("#logicEditor:not([hidden]) .lg-canvas").count() === 1);
check((await cmd({cmd: "logicCppApply", text: `struct Level : kern::Blueprint {
  struct SumToResult { f32 sum = 0.0f; };
  SumToResult SumTo(i64 n = 3) {
    f32 Total = 0.5f;
    for (auto it10 = flow_for(n10, 1, n); it10.next();) {
      Total += num_cast<f32, i64>(n10.index);
    }
    return {Total};
  }

  KERN_EVENT(on1, event_begin)
  void on1() {
    n2 = SumTo(3);
    n3 = SumTo(2);
    game_message(text_append(num_text<f32>(n2.sum), "/", num_text<f32>(n3.sum)));
  }

  struct : kern::Node<10> { i64 index = 0; } n10;
  SumToResult n2;
  SumToResult n3;
};
`})).ok, "a function with a local variable written as C++ is a Blueprint");
check(errorsOf(await logic()).length === 0, "it compiles" + (errorsOf(await logic()).length ? ": " + errorsOf(await logic()).join(" | ") : ""));
const sumTo = async () => (await logic()).functions.find(f => f.name === "SumTo");
await page.click('.lg-fn[data-fn="SumTo"]');
const localRows = page.locator('.lg-dbody .lg-frow[data-side="local"]');
check(await until(async () => await localRows.count() === 1) && await localRows.first().locator('[data-pf="name"]').inputValue() === "Total", "Details of the function lists its local variables");
await page.click('.lg-dbody [data-act="fn-local"]');
check(await until(async () => (await sumTo())?.locals?.length === 2) && (await sumTo()).locals[1].name === "local", "+ Local Variable adds one");
const renamed = page.locator('.lg-dbody .lg-frow[data-side="local"][data-pin="local"] [data-pf="name"]');
await renamed.fill("Count");
await renamed.press("Enter");
check(await until(async () => (await sumTo())?.locals?.[1]?.name === "Count"), "it takes a name");
await page.selectOption('.lg-dbody .lg-frow[data-side="local"][data-pin="Count"] [data-pf="type"]', "i64");
check(await until(async () => (await sumTo())?.locals?.[1]?.type === "i64"), "and a type");
const scoped = async () => (await logic()).nodes.filter(n => (n.params || []).some(p => p.key === "scope" && p.value === "SumTo"));
const scopedBefore = (await scoped()).length;
await page.click('.lg-dbody .lg-frow[data-side="local"][data-pin="Count"] [data-pf="set"]');
check(await until(async () => (await scoped()).length === scopedBefore + 1) && (await scoped()).some(n => n.type === "var.set" && n.params.some(p => p.key === "variable" && p.value === "Count")),
      "Set in its row puts the node of that local variable into the graph");
// (the Set is wired nowhere: it is no path of the function yet, and Compile says so at the node)
const loose = (await scoped()).find(n => n.type === "var.set" && n.params.some(p => p.key === "variable" && p.value === "Count"));
await cmd({cmd: "logicDelete", ids: [loose.id]});
await cmd({cmd: "logicFunction", name: "SumTo", op: "pinRemove", side: "local", pin: "Count"});
check(await until(async () => (await sumTo())?.locals?.length === 1) && errorsOf(await logic()).length === 0, "a local variable can be removed again");
check((await cmd({cmd: "logicCpp"})).text.includes("  SumToResult SumTo(i64 n = 3) {\n    f32 Total = 0.5f;\n"), "as C++ it is a local of the function's body");
await cmd({cmd: "logicClose"});
await cmd({cmd: "play"});
check(await until(async () => (await status()).hud?.message === "6.5/3.5", 60000), `every call starts with the local variable's default (${(await status()).hud?.message})`);
await cmd({cmd: "stop"});

// ---- 1h. a function of another Blueprint: declared here, called on an actor, played ---------------
await cmd({cmd: "new", template: "basic"});
await cmd({cmd: "logicOpen", owner: 0});
await until(async () => await page.locator("#logicEditor:not([hidden]) .lg-canvas").count() === 1);
check((await cmd({cmd: "logicCppApply", text: `struct Level : kern::Blueprint {
  struct TwiceResult { f32 out = 0.0f; };
  /// Two times x.
  TwiceResult Twice(f32 x = 1.0f) {
    return {math_mul<f32>(x, 2.0f)};
  }
};
`})).ok, "the Level's Blueprint has a function");
const asker = (await state()).actors?.[0]?.[0];
await cmd({cmd: "logicOpen", owner: asker});
await cmd({cmd: "logicVariable", op: "add", name: "Got", type: "f32"});
check(await until(async () => await page.locator('.lg-declare [data-declare] option[value="0:Twice"]').count() === 1), "an actor's Blueprint is offered the functions of the level's other Blueprints");
await page.selectOption(".lg-declare [data-declare]", "0:Twice");
const twice = async () => (await logic()).functions.find(f => f.name === "Twice");
check(await until(async () => (await twice())?.external === true) && (await twice()).inputs.length === 1 && (await twice()).help === "Two times x.", "chosen there, the function is declared here as it is there");
check(await until(async () => await page.locator('.lg-dbody [data-fk="external"]').isChecked().catch(() => false)) && await page.locator('.lg-dbody [data-act="fn-local"]').count() === 0,
      "Details shows it as a function of another Blueprint: no body, no local variables");
await page.click('.lg-dbody [data-act="fn-call-on"]');
check(await until(async () => (await logic()).nodes.some(n => n.type === "func.call_on")) && (await logic()).nodes.find(n => n.type === "func.call_on").inputs.some(p => p.id === "this" && p.label === "Target"),
      "Call on Actor puts its call into the graph, with a Target");
check((await cmd({cmd: "logicCppApply", text: `struct Asker : kern::Blueprint {
  f32 Got = 0.0f;

  struct TwiceResult { f32 out = 0.0f; };
  /// Two times x.
  KERN_EXTERN TwiceResult Twice(f32 x = 1.0f);

  KERN_EVENT(on1, event_begin)
  void on1() {
    func_call_on<"Twice">(Actor{}, kern::out(n2.out), 21.0f);
    game_message(num_text<f32>(n2.out));
  }

  struct { f32 out = 0.0f; } n2;
};
`})).ok, "the call written as C++ is the same Blueprint");
check(errorsOf(await logic()).length === 0 && !(await logic()).issues.some(i => i.text.includes("no other Blueprint")), "it compiles, and the level has that function" + (errorsOf(await logic()).length ? ": " + errorsOf(await logic()).join(" | ") : ""));
await cmd({cmd: "logicClose"});
await cmd({cmd: "play"});
check(await until(async () => (await status()).hud?.message === "42", 60000), `Play: the actor's Blueprint calls the Level's function (${(await status()).hud?.message})`);
await cmd({cmd: "stop"});

// ---- 1i. a type of the project: an enum two Blueprints have, the same in both -----------------------
await cmd({cmd: "new", template: "basic"});
await cmd({cmd: "logicOpen", owner: 0});
await until(async () => await page.locator("#logicEditor:not([hidden]) .lg-canvas").count() === 1);
check((await cmd({cmd: "logicCppApply", text: `struct Level : kern::Blueprint {
  KERN_ENUM(Mode, Closed, Open);
  Mode State = Mode::Closed;
};
`})).ok, "the Level's Blueprint has an enum");
const taker = (await state()).actors?.[0]?.[0];
await cmd({cmd: "logicOpen", owner: taker});
await cmd({cmd: "logicVariable", op: "add", name: "Seen", type: "f32"});
check(await until(async () => await page.locator('.lg-declare [data-adopt="enum"] option[value="0:Mode"]').count() === 1), "an actor's Blueprint is offered the enums of the level's other Blueprints");
await page.selectOption('.lg-declare [data-adopt="enum"]', "0:Mode");
const modeOf = async () => (await logic()).enums.find(e => e.name === "Mode");
check(await until(async () => (await modeOf())?.project === true) && (await modeOf()).values.join() === "Closed,Open", "chosen there, this Blueprint has the enum: a type of the project");
check(await until(async () => await page.locator('.lg-enum[data-enum="Mode"] .lg-proj').count() === 1) && await until(async () => await page.locator('.lg-dbody [data-ek="project"]').isChecked().catch(() => false)),
      "My Blueprint marks it and Details says it is the project's");
// a value more, added here: the Level's enum has it too
await page.click('.lg-dbody [data-act="enum-value"]');
check(await until(async () => (await modeOf())?.values.length === 3), "+ Value adds a value here");
await cmd({cmd: "logicOpen", owner: 0});
check(await until(async () => (await modeOf())?.values.length === 3 && (await modeOf()).project === true), "and in the Level's Blueprint: one change for all");
await page.click(".lg-cpp");
check(await until(async () => (await page.locator(".lg-cpptext").inputValue()).includes("KERN_PROJECT_ENUM(Mode, Closed, Open, Value);")), "the C++ view says it is an enum of the project");
await page.click(".lg-cppclose");
check(errorsOf(await logic()).length === 0, "both compile" + (errorsOf(await logic()).length ? ": " + errorsOf(await logic()).join(" | ") : ""));
await cmd({cmd: "logicClose"});

// ---- 1j. number types: f32 and i64 in one Blueprint - built in the page, wired by hand, its Cast in the open, played ----
await cmd({cmd: "new", template: "basic"});
await cmd({cmd: "logicOpen", owner: 0});
await until(async () => await page.locator("#logicEditor:not([hidden]) .lg-canvas").count() === 1);
const vars = async () => (await logic()).variables;
await page.click('.lg-sec[data-sec="vars"] [data-add]');
check(await until(async () => (await vars())?.length === 1 && (await vars())[0].type === "f32"), "+ in Variables adds a Float: an f32");
const varName = page.locator('.lg-dbody [data-vk="name"]');
await until(async () => await varName.count() === 1);
await varName.fill("Pace");
await varName.press("Enter");
// (Details is drawn anew for the renamed variable: a field is typed into once that is done - the editor draws with its next frames)
const drawn = async () => { const f = await page.evaluate(() => window.KernStudio.frames()); await page.waitForFunction(f => window.KernStudio.frames() >= f + 3, f); };
await until(async () => (await vars())[0].name === "Pace" && await page.locator('.lg-dbody [data-vk="name"]').inputValue() === "Pace");
await drawn();
check(await until(async () => (await vars())[0].name === "Pace" && await page.locator('.lg-dbody [data-vk="type"] option:checked').count() === 1) &&
      (await page.locator('.lg-dbody [data-vk="type"] option:checked').textContent()) === "Float (f32)", "Details says its type: Float (f32)");
const paceValue = page.locator('.lg-dbody [data-vk="value"]');
await paceValue.fill("0.1");
await paceValue.press("Enter");
check(await until(async () => (await vars())[0].value === Math.fround(0.1)), `0.1 typed in is the f32 nearest to it (0.1f in C++), not the double (${(await vars())[0].value})`);
await drawn();
check(await until(async () => await page.locator('.lg-dbody [data-vk="value"]').inputValue() === "0.1"), `and is shown in the shortest form that gives that f32 (${await page.locator('.lg-dbody [data-vk="value"]').inputValue()})`);
await paceValue.fill("abc");
await paceValue.press("Enter");
check(await until(async () => await page.locator('.lg-dbody [data-vk="value"].lg-bad').count() === 1) && /is no f32/.test(await paceValue.getAttribute("title")) && (await vars())[0].value === Math.fround(0.1),
      "a text that is no f32 is refused at the field, with the reason - the value stays");
await paceValue.fill("2.5");
await paceValue.press("Enter");
check(await until(async () => (await vars())[0].value === 2.5), "2.5 is taken");
await cmd({cmd: "logicVariable", op: "add", name: "Hits", type: "i64"});
await page.click('.lg-var[data-var="Hits"]');
const hitsValue = page.locator('.lg-dbody [data-vk="value"]');
await until(async () => await page.locator('.lg-dbody [data-vk="name"]').inputValue() === "Hits");
await hitsValue.fill("9007199254740993");
await hitsValue.press("Enter");
check(await until(async () => (await vars()).find(v => v.name === "Hits")?.value === "9007199254740993"), "an i64 beyond 2^53 is taken digit for digit (never by way of a JavaScript number)");
await hitsValue.fill("3");
await hitsValue.press("Enter");
await until(async () => (await vars()).find(v => v.name === "Hits")?.value === 3);
check(await until(async () => await page.locator('.lg-var[data-var="Hits"] .lg-pt').count() === 1 && await page.locator('.lg-var[data-var="Hits"] .lg-pt').textContent() === "i64" &&
      await page.locator('.lg-var[data-var="Pace"] .lg-pt').textContent() === "f32"), "My Blueprint says each variable's type");
// Begin Play: Hits = Hits + Pace (an Add of i64), shown
const nb = (await cmd({cmd: "logicAdd", type: "event.begin", x: 0, y: 0})).id;
const getHits = (await cmd({cmd: "logicAdd", type: "var.get", x: 0, y: 160, params: {variable: "Hits"}})).id;
const getPace = (await cmd({cmd: "logicAdd", type: "var.get", x: 0, y: 260, params: {variable: "Pace"}})).id;
const plus = (await cmd({cmd: "logicAdd", type: "math.add", x: 400, y: 200})).id;
const setHits = (await cmd({cmd: "logicAdd", type: "var.set", x: 640, y: 0, params: {variable: "Hits"}})).id;
const shown = (await cmd({cmd: "logicAdd", type: "game.message", x: 960, y: 0})).id;
check((await logic()).nodes.find(n => n.id === plus).numberType === "f32", "a new Add is an Add of f32 (the Float)");
await cmd({cmd: "logicConnect", from: getHits, out: "value", to: plus, in: "a"});
check(await until(async () => (await logic()).nodes.find(n => n.id === plus).numberType === "i64"), "the first wire, an i64, makes it an Add of i64");
await page.click(".lg-fit");
check(await until(async () => await page.locator(`.lg-node[data-id="${plus}"] .lg-symt`).count() === 1) && (await page.locator(`.lg-node[data-id="${plus}"] .lg-symt`).textContent()) === "i64" &&
      (await page.locator(`.lg-node[data-id="${getPace}"] .lg-pt`).textContent()) === "f32", "the node says its type under its symbol, a pin its type beside it");
// the f32 into the Add of i64, with the mouse: the pin lights as a conversion, and the Cast stands on the wire
const from = await page.locator(`.lg-node[data-id="${getPace}"] .lg-row.out[data-pin="value"] .lg-pin`).boundingBox();
const into = await page.locator(`.lg-node[data-id="${plus}"] .lg-row.in[data-pin="b"] .lg-pin`).boundingBox();
const nodesBefore = (await logic()).nodes.length;
await page.mouse.move(from.x + from.width / 2, from.y + from.height / 2);
await page.mouse.down();
await page.mouse.move(into.x + into.width / 2, into.y + into.height / 2, {steps: 8});
check(await page.locator(`.lg-node[data-id="${plus}"] .lg-row.in[data-pin="b"].conv`).count() === 1, "while dragging, the pin of another type lights as one a conversion reaches");
await page.mouse.up();
l = await until(async () => { const x = await logic(); return x.nodes.length === nodesBefore + 1 && x; });
const castNode = l && l.nodes.find(n => n.type === "num.cast");
check(!!castNode && castNode.title === "f32 → i64 (FloatingToIntegral)" && l.links.some(k => k[0] === getPace && k[2] === castNode.id) && l.links.some(k => k[0] === castNode.id && k[2] === plus && k[3] === "b"),
      `the wire got its Cast, in the open: f32 → i64 (${castNode?.title})`);
check(await until(async () => await page.locator(`.lg-node[data-id="${castNode?.id}"] .lg-loss`).count() === 1) && /toward zero/.test(await page.locator(`.lg-node[data-id="${castNode?.id}"] .lg-loss`).getAttribute("title")),
      "the Cast says that it can change a value, and how");
check(await until(async () => /On the wire: f32 → i64/.test(await page.locator("#toast").textContent())), "and the editor says what it put there");
await cmd({cmd: "undo"});
check(await until(async () => (await logic()).nodes.length === nodesBefore && !(await logic()).links.some(k => k[2] === plus && k[3] === "b")), "one Undo takes the Cast and its wires away");
await cmd({cmd: "redo"});
check(await until(async () => (await logic()).nodes.some(n => n.type === "num.cast")), "Redo brings them back");
// a node's Type does not change silently under wires of another type
await page.click(`.lg-node[data-id="${plus}"] .lg-sym`);
const plusType = page.locator('.lg-dbody select.lg-par[data-param="type"]');
check(await until(async () => await plusType.count() === 1) && (await plusType.locator("option").allTextContents()).slice(0, 2).join("|") === "Float (f32)|Int (i64)" &&
      !(await plusType.locator("option").allTextContents()).some(t => /Vector|Text/.test(t)) && (await plusType.locator("option").count()) === 10, "the Type of an Add offers the ten numbers, the defaults first");
await plusType.selectOption("f32");
check(await until(async () => /brings an i64/.test(await page.locator("#toast").textContent())) && (await logic()).nodes.find(n => n.id === plus).numberType === "i64" && await plusType.inputValue() === "i64",
      "another Type while an i64 is wired in is refused, and the wire named");
// a value its type does not hold, typed on a node
const small = (await cmd({cmd: "logicAdd", type: "math.add", x: 400, y: 420, params: {type: "u8"}})).id;
await page.click(".lg-fit");
const smallA = page.locator(`.lg-node[data-id="${small}"] .lg-row.in[data-pin="a"] input.lg-lit`);
check(await until(async () => await smallA.count() === 1), "an Add of u8 has its fields");
await smallA.fill("300");
await smallA.press("Enter");
check(await until(async () => await page.locator(`.lg-node[data-id="${small}"] .lg-row.in[data-pin="a"] input.lg-lit.lg-bad`).count() === 1) &&
      /0 to 255/.test(await page.locator(`.lg-node[data-id="${small}"] .lg-row.in[data-pin="a"] input.lg-lit`).getAttribute("title")) &&
      (await page.locator(`.lg-node[data-id="${small}"] .lg-row.in[data-pin="a"] input.lg-lit`).inputValue()) === "300", "300 typed into a u8 is refused at the field, with the reason");
await page.locator(`.lg-node[data-id="${small}"] .lg-row.in[data-pin="a"] input.lg-lit`).fill("255");
await page.locator(`.lg-node[data-id="${small}"] .lg-row.in[data-pin="a"] input.lg-lit`).press("Enter");
check(await until(async () => (await logic()).nodes.find(n => n.id === small).inputs.find(p => p.id === "a").value === 255), "255 is taken");
await cmd({cmd: "logicDelete", ids: [small]});
// no node turns a Vector into a number: refused, with the reason
await cmd({cmd: "logicVariable", op: "add", name: "Where", type: "vector"});
const getWhere = (await cmd({cmd: "logicAdd", type: "var.get", x: 0, y: 420, params: {variable: "Where"}})).id;
const refused = await cmd({cmd: "logicConnect", from: getWhere, out: "value", to: plus, in: "a"});
check(!refused.ok && /Break Vector/.test(refused.error), `a Vector into an i64 is refused with the reason (${refused.error})`);
await cmd({cmd: "logicDelete", ids: [getWhere]});
await cmd({cmd: "logicVariable", op: "remove", name: "Where"});
for (const [a, out, b, input] of [[plus, "result", setHits, "value"], [nb, "then", setHits, "exec"], [setHits, "then", shown, "exec"], [setHits, "value", shown, "text"]])
  await cmd({cmd: "logicConnect", from: a, out, to: b, in: input});
check(errorsOf(await logic()).length === 0, "the Blueprint of f32 and i64 compiles" + (errorsOf(await logic()).length ? ": " + errorsOf(await logic()).join(" | ") : ""));
const numbersCode = (await cmd({cmd: "logicCpp"})).text;
check(numbersCode.includes("f32 Pace = 2.5f;") && numbersCode.includes("i64 Hits = 3;") && numbersCode.includes("math_add<i64>(Hits, num_cast<i64, f32>(Pace))") && numbersCode.includes("game_message(num_text<i64>("),
      "as C++: the types themselves, the Cast a num_cast, the To Text a num_text");
await cmd({cmd: "logicClose"});
await cmd({cmd: "play"});
check(await until(async () => (await status()).hud?.message === "5", 60000), `Play: 3 + (i64) 2.5 = 5 (${(await status()).hud?.message})`);
check(await until(async () => (await status()).logic?.compiled >= 1, 30000), "as machine code (JIT)");
await cmd({cmd: "stop"});

if (gameLevel) {
  // ---- 2. a game's level file (only when one is named: the engine's cases above need no game) ------------
  // The whole project first: every Blueprint of the level file, the folder "Source" with it - the Blueprints written
  // from the game's C++ (the Blueprint Game has every function of ~40 files, tens of thousands of nodes). The editor
  // opens each one (the project loads in the editor): every Blueprint
  // is shown sheet by sheet - it opens on its events, and its largest function's sheet is opened too. Then the level as the
  // built game carries it (without the folder "Source"), and Play runs that.
  const levelText = await (await fetch(base + "/" + gameLevel)).text();
  let withBlueprint = [];
  for (const whole of [true, false]) {
    const file = JSON.parse(levelText);
    if (!whole) {
      // (but the libraries the Blueprints that stay use, through `uses`, one after the other - as tools/embed_level.py
      // --without keeps them: those of "Program" call libraries of "Source")
      const byName = new Map(file.actors.map(a => [a.name, a]));
      const kept = new Set(file.actors.filter(a => a.folder !== "Source").map(a => a.name));
      for (const todo = [...kept]; todo.length;)
        for (const used of byName.get(todo.pop())?.components?.logic?.uses || [])
          if (!kept.has(used) && byName.has(used)) { kept.add(used); todo.push(used); }
      file.actors = file.actors.filter(a => kept.has(a.name));
    }
    const text = JSON.stringify(file);
    const what = whole ? "the whole project" : "the level the game carries";
    const opened = await cmd({cmd: "open", text, name: gameLevel.split("/").pop()});
    check(opened?.ok !== false, `the level file ${gameLevel} opens in the editor (${what})` + (opened?.error ? ": " + opened.error : ""));
    const st = await state();
    check(st.level?.name === JSON.parse(levelText).name, `it is the level ${JSON.parse(levelText).name} (${st.level?.name})`);
    // (the level file says which actors carry a Blueprint; the editor's state lists every actor as [id, name])
    const carriers = file.actors.filter(a => a.components?.logic).map(a => a.name);
    withBlueprint = (st.actors || []).filter(([, name]) => carriers.includes(name)).map(([id, name]) => ({id, name}));
    check(carriers.length >= 1 && withBlueprint.length === carriers.length && carriers.includes("Wanted"), `its ${carriers.length} Blueprints are there (${what})`);
    let slowest = ["", 0], nodes = 0, bad = 0;
    for (const a of withBlueprint) {
      const start = Date.now();
      await cmd({cmd: "logicOpen", owner: a.id});
      let l = await logic();
      const issues = errorsOf(l);
      let ok = issues.length === 0 && l.nodes.every(n => n.title);
      // (it opens on the sheet of its events: what stands in no function, and nothing else)
      ok = ok && Array.isArray(l.sheets) && l.sheet === "" && l.nodes.length === l.sheetEvents;
      if (l.sheets.length) {
        // (sheets: what is shown is one function's body - the largest one is drawn too, and it is that function's)
        const largest = l.sheets.reduce((x, y) => (y.nodes > x.nodes ? y : x));
        await cmd({cmd: "logicSheet", function: largest.name});
        l = await logic();
        ok = ok && l.sheet === largest.name && l.nodes.length === largest.nodes && l.nodes.every(n => n.title) && l.nodes.some(n => n.id === largest.entry);
        ok = ok && await until(async () => await page.locator("#logicEditor .lg-node").count() === largest.nodes, 60000);
      }
      const ms = Date.now() - start;
      if (ms > slowest[1]) slowest = [a.name, ms];
      nodes += l.counts?.nodes || 0;
      if (!ok) bad++;
      // (one line for each of the rule Blueprints the game runs, and for every one that fails; the others are summed below)
      if (!ok || !whole) check(ok, `${a.name}: its Blueprint compiles in the editor (${l.functions.length} functions, ${l.counts?.nodes} nodes)` + (issues.length ? ": " + issues.slice(0, 3).join(" | ") : ""));
      await cmd({cmd: "logicClose"});
    }
    if (whole) {
      check(bad === 0, `every one of the ${withBlueprint.length} Blueprints opens and compiles in the editor (${nodes} nodes; the slowest, ${slowest[0]}, in ${slowest[1]} ms)`);
      check(slowest[1] < 20000, `none takes longer than 20 s to open (${slowest[0]}: ${slowest[1]} ms)`);
    }
  }
  await cmd({cmd: "play"});
  check(await until(async () => (await status()).logic?.compiled >= withBlueprint.length, 60000), `Play runs them as machine code (${(await status()).logic?.compiled} compiled)`);
  await cmd({cmd: "stop"});
}
check(errors.length === 0, "no page errors" + (errors.length ? ": " + errors.slice(0, 3).join(" | ") : ""));
await browser.close();
console.log(failures ? `${failures} FAILED` : "blueprint test passed");
process.exit(failures ? 1 : 0);
