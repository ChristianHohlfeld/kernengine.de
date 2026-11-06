// Touch-only comparison of the original (spawncity.de) and SpawnCity on Kern on a phone: the same
// emulated touch phone (isMobile + hasTouch, coarse pointer), landscape and portrait, every step
// driven by taps (CDP touch events) - no keyboard, no mouse, so the original stays in its touch
// layout (desktop_controls.mjs switches to body.desktop-input on any key or mouse). States a finger
// cannot reach in a short run (car, jet) are entered through the games' own mode APIs, which send
// no input events either. Per step: a screenshot and a dump of every visible HUD element (id,
// class, rect, text, computed display), plus diff.md - element by element, original vs Kern.
//
//   node kern/games/spawncity/tests/touch_compare.mjs [out-dir] [original-url] [kern-url]
import {createRequire} from "node:module";
import fs from "node:fs";
import path from "node:path";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const out = path.resolve(process.argv[2] || "generated/spawncity-touch");
const ORIGINAL = process.argv[3] || "https://spawncity.de/drone_simulator.html";
const KERN = process.argv[4] || "https://kernengine.de/kern/games/spawncity/web/index.html";
const ONLY = process.env.TOUCH_ONLY || "";  // e.g. "kern" to run just one game
const GPS = {latitude: 52.5208, longitude: 13.4094, accuracy: 5};  // Berlin, Alexanderplatz
fs.mkdirSync(out, {recursive: true});
const pause = ms => new Promise(r => setTimeout(r, ms));
const UA = "Mozilla/5.0 (iPhone; CPU iPhone OS 18_6 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.6 Mobile/15E148 Safari/604.1";
const DEVICES = {
  landscape: {viewport: {width: 844, height: 390}, screen: {width: 844, height: 390}, deviceScaleFactor: 2, isMobile: true, hasTouch: true, userAgent: UA},
  small: {viewport: {width: 770, height: 353}, screen: {width: 770, height: 353}, deviceScaleFactor: 2.6, isMobile: true, hasTouch: true, userAgent: UA},  // a real Android phone
  portrait: {viewport: {width: 390, height: 844}, screen: {width: 390, height: 844}, deviceScaleFactor: 2, isMobile: true, hasTouch: true, userAgent: UA},
};

// Every visible element that a player can see as a HUD piece: has an id, is a button, has own text
// or a visible box. Key: id, else the nearest id'd ancestor + tag.class path.
const dumpHud = () => {
  const items = [];
  const vis = e => { const s = getComputedStyle(e); if (s.display === "none" || s.visibility === "hidden" || +s.opacity < 0.02) return false; const r = e.getBoundingClientRect(); return r.width > 1 && r.height > 1 && r.bottom > 0 && r.right > 0 && r.top < innerHeight && r.left < innerWidth; };
  const shown = e => { for (let p = e; p && p !== document.body; p = p.parentElement) { const s = getComputedStyle(p); if (s.display === "none" || s.visibility === "hidden" || +s.opacity < 0.02) return false; } return true; };
  const keyOf = e => {
    if (e.id) return "#" + e.id;
    let p = e.parentElement, chain = [];
    const c = e.className && typeof e.className === "string" ? "." + e.className.trim().split(/\s+/).slice(0, 2).join(".") : "";
    const da = [...e.attributes].filter(a => a.name.startsWith("data-") && a.value.length < 20).slice(0, 1).map(a => `[${a.name}=${a.value}]`).join("");
    chain.push(e.tagName.toLowerCase() + c + da);
    while (p && !p.id && p !== document.body) p = p.parentElement;
    return (p && p.id ? "#" + p.id + " " : "") + chain.join(" ");
  };
  for (const e of document.querySelectorAll("body *")) {
    if (/^(SCRIPT|STYLE|svg|path|OPTION)$/i.test(e.tagName)) continue;
    if (!vis(e) || !shown(e)) continue;
    let own = ""; for (const n of e.childNodes) if (n.nodeType === 3) own += n.textContent;
    own = own.replace(/\s+/g, " ").trim();
    const s = getComputedStyle(e), r = e.getBoundingClientRect();
    const boxy = e.tagName === "BUTTON" || e.tagName === "CANVAS" || e.tagName === "INPUT" || (s.backgroundColor !== "rgba(0, 0, 0, 0)" && r.width * r.height > 300) || (s.borderStyle !== "none" && parseFloat(s.borderWidth) > 0);
    if (!own && !boxy && !e.id) continue;
    if (r.width >= innerWidth - 2 && r.height >= innerHeight - 2 && !own) continue;  // full-screen layers
    const before = getComputedStyle(e, "::before").content, after = getComputedStyle(e, "::after").content;
    items.push({key: keyOf(e), tag: e.tagName.toLowerCase(), id: e.id || undefined, cls: typeof e.className === "string" ? e.className.slice(0, 80) || undefined : undefined,
      text: own.slice(0, 80) || undefined, x: Math.round(r.left), y: Math.round(r.top), w: Math.round(r.width), h: Math.round(r.height), display: s.display,
      font: `${s.fontWeight} ${s.fontSize}`, color: s.color, bg: s.backgroundColor !== "rgba(0, 0, 0, 0)" ? s.backgroundColor : undefined,
      radius: s.borderRadius !== "0px" ? s.borderRadius : undefined,
      pseudo: [before, after].filter(v => v && v !== "none" && v !== "normal").join(" | ") || undefined});
  }
  const ds = {}; for (const [k, v] of Object.entries(document.body.dataset)) ds[k] = v;
  return {w: innerWidth, h: innerHeight, bodyClass: document.body.className, htmlClass: document.documentElement.className, bodyData: ds, items};
};

const GAMES = {
  original: {
    url: ORIGINAL,
    boot: page => page.waitForFunction(() => document.getElementById("gameMenuStart")?.textContent?.trim() === "START", null, {timeout: 120000}),
    afterStart: page => page.waitForFunction(() => document.getElementById("gameMenu")?.hidden === true, null, {timeout: 60000}),
    mode: (page, m) => page.evaluate(m => {
      const walk = globalThis.__arondightWalkMode;
      if (m === "car") { walk?.setMode?.("foot", {persist: false, reason: "compare"}); const d = globalThis.__arondightVehicleDrive; let r = d?.enterNearest?.(); if (!d?.active) r = `nearest ${r}; gta ${globalThis.__arondightGtaRuntime?.setMode?.("vehicle")}`; return String(r); }
      if (m === "jet") { try { globalThis.__arondightVehicleDrive?.exit?.(); } catch {} walk?.setMode?.("foot", {persist: false, reason: "compare"}); return String(globalThis.__jetMode?.enter?.() ?? "no jet api"); }
      if (m === "unjet") { try { globalThis.__jetMode?.exit?.(); } catch {} return walk?.setMode?.("foot", {persist: false, reason: "compare"}); }
      return walk?.setMode?.(m, {persist: false, reason: "compare"}) ?? "no walk mode";
    }, m),
    state: page => page.evaluate(() => ({mode: globalThis.__arondightWalkMode?.mode, car: !!globalThis.__arondightVehicleDrive?.active, jet: !!globalThis.__jetMode?.active, desktop: document.body.classList.contains("desktop-input"), coarse: matchMedia("(pointer: coarse)").matches, touch: navigator.maxTouchPoints})),
  },
  kern: {
    url: KERN,
    // the snapshot's START is there before the game has loaded: wait for the game to say placed
    boot: page => page.waitForFunction(() => { let placed = false; try { placed = !!window.SpawnCity?.hud?.()?.placed; } catch {} const b = document.querySelector("html.sc-original-view #gameMenuStart"); return placed && b && !b.disabled && b.textContent.trim() === "START"; }, null, {timeout: 180000, polling: 500}),
    afterStart: page => page.waitForFunction(() => document.getElementById("gameMenu")?.hidden === true, null, {timeout: 60000}),
    mode: (page, m) => page.evaluate(m => window.SpawnCity?.command?.(m === "unjet" ? "mode foot" : `mode ${m}`), m),
    state: page => page.evaluate(() => { try { const h = window.SpawnCity.hud(); return {mode: h.mode, device: h.ctl?.device, desktop: document.body.classList.contains("desktop-input"), coarse: matchMedia("(pointer: coarse)").matches, touch: navigator.maxTouchPoints}; } catch (e) { return {error: String(e)}; } }),
  },
};

const report = {original: ORIGINAL, kern: KERN, runs: []};
const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist", "--autoplay-policy=no-user-gesture-required"]});
try {
  for (const [dev, profile] of Object.entries(DEVICES)) {
    if (process.env.TOUCH_DEVICE && process.env.TOUCH_DEVICE !== dev) continue;
    for (const [name, g] of Object.entries(GAMES)) {
      if (ONLY && ONLY !== name) continue;
      const ctx = await browser.newContext({...profile, geolocation: GPS, permissions: ["geolocation"], locale: "de-DE"});
      const page = await ctx.newPage();
      const run = {device: dev, game: name, steps: [], errors: []};
      report.runs.push(run);
      page.on("pageerror", e => run.errors.push(String(e.message || e).slice(0, 300)));
      page.on("console", m => { if (m.type() === "error") run.errors.push(`console: ${m.text().slice(0, 240)}`); });
      // a finger on the element's centre (CDP Input.dispatchTouchEvent through Playwright's touchscreen)
      const tap = async sel => {
        const r = await page.evaluate(sel => { const e = document.querySelector(sel); if (!e) return null; const b = e.getBoundingClientRect(); return b.width > 0 ? {x: b.left + b.width / 2, y: b.top + b.height / 2} : null; }, sel);
        if (!r) return `no ${sel}`;
        await page.touchscreen.tap(r.x, r.y);
        return `tap ${sel} ${Math.round(r.x)},${Math.round(r.y)}`;
      };
      const step = async (label, fn, wait = 2500) => {
        let note;
        try { note = await fn?.(); } catch (e) { run.errors.push(`${label}: ${String(e.message || e).slice(0, 300)}`); }
        await pause(wait);
        const file = `${dev}-${String(run.steps.length).padStart(2, "0")}-${label}-${name}.png`;
        try { await page.screenshot({path: path.join(out, file)}); } catch (e) { run.errors.push(`shot ${label}: ${e.message}`); }
        let hud = null, state = null;
        try { hud = await page.evaluate(dumpHud); } catch (e) { hud = {error: String(e)}; }
        try { state = await g.state(page); } catch (e) { state = {error: String(e)}; }
        run.steps.push({label, note, shot: file, state, hud});
        console.log(dev, name, label, JSON.stringify(note ?? ""), JSON.stringify(state).slice(0, 200));
      };
      const url = new URL(g.url); url.searchParams.set("menu", "1");
      await step("title", async () => { await page.goto(url.href, {waitUntil: "domcontentloaded", timeout: 90000}); await g.boot(page); }, 2500);
      await step("start", async () => { const n = await tap("#gameMenuStart"); await g.afterStart(page).catch(() => {}); return n; }, 7000);
      await step("minimap-expanded", () => tap("#worldLookHud [data-map-expand]"), 2500);
      // close the big map: the original's ✕ (#worldMapFull .wm-close), else the expand button again
      if ((await tap("#worldMapFull .wm-close")).startsWith("no ")) await tap("#worldLookHud [data-map-expand]");
      await pause(1000);
      await step("mode-tap", () => tap("#mobileGameplayMode"), 3500);
      await step("drone", () => g.mode(page, "drone"), 2500);
      await step("foot", () => g.mode(page, "foot"), 2500);
      await step("car", () => g.mode(page, "car"), 3500);
      await step("jet", () => g.mode(page, "jet"), 4000);
      await g.mode(page, "unjet").catch(() => {}); await pause(1500);
      await step("menu", () => tap("#mobileGameplaySettings"), 2000);
      await ctx.close();
    }
  }
} finally {
  fs.writeFileSync(path.join(out, "touch.json"), JSON.stringify(report, null, 1));
  // element by element: visible only in one game, and geometry / text differences of the shared ones
  const lines = [];
  for (const dev of Object.keys(DEVICES)) {
    const o = report.runs.find(r => r.device === dev && r.game === "original"), k = report.runs.find(r => r.device === dev && r.game === "kern");
    if (!o || !k) continue;
    for (const st of o.steps) {
      const ks = k.steps.find(x => x.label === st.label);
      lines.push(`## ${dev} / ${st.label}`, `original state ${JSON.stringify(st.state)}`, `kern state ${JSON.stringify(ks?.state)}`);
      lines.push(`original body: ${st.hud?.bodyClass}`, `kern body: ${ks?.hud?.bodyClass}`);
      const om = new Map(), km = new Map();
      for (const i of st.hud?.items || []) if (!om.has(i.key)) om.set(i.key, i);
      for (const i of ks?.hud?.items || []) if (!km.has(i.key)) km.set(i.key, i);
      const fmt = i => `${i.x},${i.y} ${i.w}x${i.h}${i.text ? ` "${i.text}"` : ""}${i.pseudo ? ` {${i.pseudo}}` : ""}`;
      for (const [key, i] of om) if (!km.has(key)) lines.push(`- only original ${key}: ${fmt(i)}`);
      for (const [key, i] of km) if (!om.has(key)) lines.push(`+ only kern ${key}: ${fmt(i)}`);
      for (const [key, i] of om) {
        const j = km.get(key); if (!j) continue;
        const geo = Math.abs(i.x - j.x) > 3 || Math.abs(i.y - j.y) > 3 || Math.abs(i.w - j.w) > 3 || Math.abs(i.h - j.h) > 3;
        if (geo || i.text !== j.text || i.pseudo !== j.pseudo) lines.push(`~ ${key}: original ${fmt(i)}  |  kern ${fmt(j)}`);
      }
      lines.push("");
    }
    lines.push(`errors original: ${o.errors.join(" | ")}`, `errors kern: ${k.errors.join(" | ")}`, "");
  }
  fs.writeFileSync(path.join(out, "diff.md"), lines.join("\n"));
  await browser.close();
}
