// Touch-only check of the phone HUD on a page (Kern or the original): taps only, no keys or mouse.
//   node touch_check.mjs <url> <out dir> <label>
// Logs the boxes of the drone's thumb buttons (overlaps), what one tap on ARM and on the camera does, and what
// TON / MUSIK / RESET do; screenshots of each state. (Until 26.11.2026 it also tapped WEAPON, DRONE and the sniper
// scope: out of the small game with them - BLUEPRINTS.md, "Herausgenommen am 26.11.".)
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
const state = () => p.evaluate(() => { const h = window.SpawnCity?.hud?.(); return {body: document.body.className, mode: h?.mode || document.getElementById("viewport")?.dataset.walkMode || globalThis.__arondightWalkMode?.mode,
  armed: h ? h.drone?.armed : document.getElementById("soloArm")?.textContent?.trim(), cam: h ? h.drone?.cam : document.getElementById("viewport")?.dataset.cameraMode, resets: h?.ui?.resets,
  sound: document.getElementById("soundSwitch")?.className, music: document.getElementById("musicSwitch")?.className}; });
log(`# ${label} (kern ${kern})`);
await tap("#gameMenuStart");
if (kern) await p.waitForFunction(() => window.SpawnCity.hud()?.placed, null, {timeout: 180000}).catch(() => log("never placed"));
await p.waitForTimeout(9000);
log(`started: ${JSON.stringify(await state())}`);
const ids = ["#soloArm", "#soloKill", "#soloCamera", "#camFpv", "#camFollow", "#camThird", "#soloClearance", "#soloLeft", "#soloRight", "#soloReset", "#musicSwitch", "#soundSwitch",
  "#mobileGameplayReset", "#mobileGameplaySettings", "#worldLookHud", "#gameExitButton", "#mobileGameplayDock", ".phone-settings-button"];
const boxes = {};
for (const id of ids) boxes[id] = await box(id);
log("```\n" + Object.entries(boxes).map(([k, v]) => `${k.padEnd(28)} ${v ? `${v.x},${v.y} ${v.w}x${v.h} z${v.z} ${v.text}` : "-"}`).join("\n") + "\n```");
// overlaps between visible buttons
const ov = [];
const keys = Object.keys(boxes).filter(k => boxes[k] && !/soloLeft|soloRight|Dock/.test(k));
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
await shot("drone");
// one tap on ARM (the flight controller first calibrates: a few seconds after the start)
const a0 = await state();
await tap("#soloArm"); await p.waitForTimeout(2500);
log(`arm tap: ${a0.armed} -> ${(await state()).armed}`);
await shot("armed");
// one tap on the camera
const c0 = await state();
await tap("#soloCamera"); await p.waitForTimeout(1500);
log(`camera tap: ${c0.cam} -> ${(await state()).cam}`);
await shot("camera");
// TON / MUSIK twice each (on and back), RESET once
for (const id of ["#soundSwitch", "#musicSwitch"]) {
  const k = id === "#soundSwitch" ? "sound" : "music", s0 = (await state())[k];
  await tap(id); await p.waitForTimeout(900); const s1 = (await state())[k];
  await tap(id); await p.waitForTimeout(900);
  log(`${id} taps: ${s0} -> ${s1} -> ${(await state())[k]}`);
}
const r0 = await state();
if (!(await tap("#mobileGameplayReset"))) await tap("#soloReset");
await p.waitForTimeout(2500);
const r1 = await state();
log(`reset tap: resets ${r0.resets} -> ${r1.resets}, armed ${r0.armed} -> ${r1.armed}`);
await shot("reset");
fs.writeFileSync(path.join(outDir, `${label}.md`), out.join("\n") + "\n");
await b.close();
