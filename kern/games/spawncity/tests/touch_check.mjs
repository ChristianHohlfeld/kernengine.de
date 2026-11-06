// Touch-only check of the phone HUD on a page (Kern or the original): taps only, no keys or mouse.
//   node touch_check.mjs <url> <out dir> <label>
// Logs the boxes of the thumb buttons (overlaps), what one tap on DRONE / WEAPON does, and whether
// the sniper scope can be left again; screenshots of each state.
import fs from "node:fs";
import path from "node:path";
import {chromium} from "playwright";

const [url, outDir, label = "page"] = process.argv.slice(2);
fs.mkdirSync(outDir, {recursive: true});
const out = [];
const log = s => { console.log(s); out.push(s); };
const b = await chromium.launch({args: ["--use-angle=swiftshader", "--enable-unsafe-swiftshader"]});
const ctx = await b.newContext({viewport: {width: 844, height: 390}, isMobile: true, hasTouch: true, deviceScaleFactor: 2,
  geolocation: {latitude: 52.52, longitude: 13.405}, permissions: ["geolocation"]});
const p = await ctx.newPage();
p.on("pageerror", e => log(`pageerror ${e.message}`));
await p.goto(url, {waitUntil: "domcontentloaded", timeout: 120000});
await p.waitForTimeout(8000);
const kern = await p.evaluate(() => !!window.SpawnCity);
const shot = async name => p.screenshot({path: path.join(outDir, `${label}-${name}.png`)});
const box = sel => p.evaluate(s => { const e = document.querySelector(s); if (!e) return null; const r = e.getBoundingClientRect(), cs = getComputedStyle(e);
  return r.width > 0 && cs.display !== "none" && cs.visibility !== "hidden" ? {x: Math.round(r.x), y: Math.round(r.y), w: Math.round(r.width), h: Math.round(r.height), z: cs.zIndex, text: (e.textContent || "").trim().slice(0, 24)} : null; }, sel);
const tap = async sel => { const r = await box(sel); if (!r) { log(`tap ${sel}: not visible`); return false; } await p.touchscreen.tap(r.x + r.w / 2, r.y + r.h / 2); return true; };
const state = () => p.evaluate(() => ({body: document.body.className, mode: window.SpawnCity?.hud?.()?.mode || document.getElementById("viewport")?.dataset.walkMode || globalThis.__arondightWalkMode?.mode,
  weapon: document.getElementById("mobileGameplayWeapon")?.textContent?.trim() || globalThis.__arondightFootWeapons?.mode}));
log(`# ${label} (kern ${kern})`);
await tap("#gameMenuStart");
if (kern) await p.waitForFunction(() => window.SpawnCity.hud()?.placed, null, {timeout: 180000}).catch(() => log("never placed"));
await p.waitForTimeout(9000);
log(`started: ${JSON.stringify(await state())}`);
const ids = ["#thumbWeapon", "#footJump", "#mobileGameplayMode", "#airStrikeButton", "#handGrenadeBtn", "#scopeButton", "#footMove", "#footLook", "#musicSwitch", "#soundSwitch",
  "#proximityVoiceBar", "#mobileGameplayReset", "#gpsLiveButton", "#worldLookHud", "#gameExitButton", "#mobileGameplayMultiplayer", "#mobileGameplayDock", ".phone-settings-button"];
const boxes = {};
for (const id of ids) boxes[id] = await box(id);
log("```\n" + Object.entries(boxes).map(([k, v]) => `${k.padEnd(28)} ${v ? `${v.x},${v.y} ${v.w}x${v.h} z${v.z} ${v.text}` : "-"}`).join("\n") + "\n```");
// overlaps between visible buttons
const ov = [];
const keys = Object.keys(boxes).filter(k => boxes[k] && !/footMove|footLook|Dock/.test(k));
for (let i = 0; i < keys.length; i++) for (let j = i + 1; j < keys.length; j++) {
  const a = boxes[keys[i]], c = boxes[keys[j]];
  const w = Math.min(a.x + a.w, c.x + c.w) - Math.max(a.x, c.x), h = Math.min(a.y + a.h, c.y + c.h) - Math.max(a.y, c.y);
  if (w > 2 && h > 2) ov.push(`${keys[i]} x ${keys[j]} (${w}x${h})`);
}
log(`overlaps: ${ov.join(", ") || "none"}`);
// every visible button / control on the screen (what the player sees, whatever its id)
const all = await p.evaluate(() => [...document.querySelectorAll("button,[role=button],.k-audio,.scui-corner,svg")].filter(e => {
  const r = e.getBoundingClientRect(), cs = getComputedStyle(e);
  return r.width > 4 && r.height > 4 && cs.visibility !== "hidden" && cs.display !== "none" && Number(cs.opacity) > 0.05 && r.right > 0 && r.bottom > 0 && r.left < innerWidth && r.top < innerHeight && !e.closest("svg:not(:scope)") && e.checkVisibility?.({checkOpacity: true, checkVisibilityCSS: true}) !== false;
}).map(e => { const r = e.getBoundingClientRect(); return `${e.tagName.toLowerCase()}#${e.id || "-"}.${String(e.className?.baseVal ?? e.className).split(" ")[0] || "-"} ${Math.round(r.x)},${Math.round(r.y)} ${Math.round(r.width)}x${Math.round(r.height)} "${(e.textContent || e.getAttribute("aria-label") || "").trim().slice(0, 14)}"`; }));
log("visible controls:\n```\n" + all.join("\n") + "\n```");
await shot("foot");
// one tap on WEAPON
const w0 = await state();
await tap("#thumbWeapon"); await p.waitForTimeout(1200);
log(`weapon tap: ${w0.weapon} -> ${(await state()).weapon}`);
// one tap on DRONE
await tap("#mobileGameplayMode"); await p.waitForTimeout(400); const m1 = await state(); await p.waitForTimeout(2000); const m2 = await state();
log(`drone tap: after 0.4 s ${m1.mode} · after 2.4 s ${m2.mode}`);
await shot("drone");
await tap("#mobileGameplayMode"); await p.waitForTimeout(2500);
log(`back on foot: ${(await state()).mode}`);
// sniper: weapon until SNIPER, scope in, scope out
for (let i = 0; i < 8 && !/SNIPER|sniper/i.test((await state()).weapon || ""); i++) { await tap("#thumbWeapon"); await p.waitForTimeout(900); }
log(`weapon now ${(await state()).weapon}`);
const sb = await box("#scopeButton");
log(`scope button ${JSON.stringify(sb)}`);
if (sb) {
  await tap("#scopeButton"); await p.waitForTimeout(1500);
  log(`scoped: ${/scope-active/.test((await state()).body)} · scope button now ${JSON.stringify(await box("#scopeButton"))}`);
  await shot("scope");
  await tap("#scopeButton"); await p.waitForTimeout(1500);
  log(`after second tap scoped: ${/scope-active/.test((await state()).body)}`);
}
fs.writeFileSync(path.join(outDir, `${label}.md`), out.join("\n") + "\n");
await b.close();
