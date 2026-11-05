// Side-by-side comparison of the original (spawncity.de) and SpawnCity on Kern, same browser, same
// GPS position, same device profiles and the same steps. Writes for every step and both games a
// screenshot and a dump of everything visible on the page (text, position, size, colours), plus
// compare.json - the material for checking the translation screen by screen.
//
//   node kern/games/spawncity/tests/compare.mjs [out-dir] [original-url] [kern-url]
import {createRequire} from "node:module";
import fs from "node:fs";
import path from "node:path";
import {measureControls} from "./compare_controls.mjs";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const out = path.resolve(process.argv[2] || "generated/spawncity-compare");
const ORIGINAL = process.argv[3] || "https://spawncity.de/drone_simulator.html";
const KERN = process.argv[4] || "https://kernengine.de/kern/games/spawncity/web/index.html";
// COMPARE_PART: "all" (default), "ui" (the screens) or "controls" (compare_controls.mjs: real inputs, measured)
const PART = process.env.COMPARE_PART || "all";
const GPS = {latitude: 52.5208, longitude: 13.4094, accuracy: 5};  // Berlin, Alexanderplatz
fs.mkdirSync(out, {recursive: true});
const pause = ms => new Promise(r => setTimeout(r, ms));
const report = {original: ORIGINAL, kern: KERN, gps: GPS, runs: []};

const DEVICES = {
  desktop: {viewport: {width: 1280, height: 720}, deviceScaleFactor: 1},
  phone: {viewport: {width: 844, height: 390}, deviceScaleFactor: 1, isMobile: true, hasTouch: true,
    userAgent: "Mozilla/5.0 (iPhone; CPU iPhone OS 18_6 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/18.6 Mobile/15E148 Safari/604.1"},
};

// Everything a player sees: elements with own text or a visible box, with geometry and style.
const dumpUi = () => {
  const seen = [];
  const visible = e => { const s = getComputedStyle(e); if (s.display === "none" || s.visibility === "hidden" || +s.opacity === 0) return false; const r = e.getBoundingClientRect(); return r.width > 1 && r.height > 1 && r.bottom > 0 && r.right > 0 && r.top < innerHeight && r.left < innerWidth; };
  for (const e of document.querySelectorAll("body *")) {
    if (e.tagName === "SCRIPT" || e.tagName === "STYLE" || !visible(e)) continue;
    let own = ""; for (const n of e.childNodes) if (n.nodeType === 3) own += n.textContent;
    own = own.replace(/\s+/g, " ").trim();
    const s = getComputedStyle(e), r = e.getBoundingClientRect();
    const boxy = e.tagName === "BUTTON" || e.tagName === "CANVAS" || e.tagName === "INPUT" || (s.backgroundColor !== "rgba(0, 0, 0, 0)" && r.width * r.height > 400) || s.borderStyle !== "none";
    if (!own && !boxy) continue;
    seen.push({tag: e.tagName.toLowerCase(), id: e.id || undefined, cls: (e.className && typeof e.className === "string") ? e.className.slice(0, 60) : undefined,
      text: own.slice(0, 120) || undefined, x: Math.round(r.left), y: Math.round(r.top), w: Math.round(r.width), h: Math.round(r.height),
      font: `${s.fontWeight} ${s.fontSize} ${s.fontFamily.split(",")[0]}`, color: s.color, bg: s.backgroundColor !== "rgba(0, 0, 0, 0)" ? s.backgroundColor : undefined,
      radius: s.borderRadius !== "0px" ? s.borderRadius : undefined});
  }
  return {title: document.title, w: innerWidth, h: innerHeight, items: seen};
};

const GAMES = {
  original: {
    url: ORIGINAL,
    async boot(page) {
      await page.waitForFunction(() => document.getElementById("gameMenuStart")?.textContent?.trim() === "START", null, {timeout: 90000});
    },
    async start(page) {
      await page.click("#gameMenuStart");
      await page.waitForFunction(() => document.getElementById("gameMenu")?.hidden === true, null, {timeout: 60000});
      await page.waitForFunction(() => Number(document.querySelector("#viewport")?.dataset.worldCityBuildings) > 20, null, {timeout: 60000}).catch(() => {});
    },
    mode: (page, m) => page.evaluate(m => {
      if (m === "car") return globalThis.__arondightGtaRuntime?.setMode?.("vehicle") ?? "no gta runtime";
      return globalThis.__arondightWalkMode?.setMode?.(m, {persist: false, reason: "compare"}) ?? "no walk mode";
    }, m),
    state: page => page.evaluate(() => { const d = document.querySelector("#viewport")?.dataset || {}; return {mode: globalThis.__arondightWalkMode?.mode, gta: globalThis.__arondightGtaRuntime?.mode, camera: d.cameraMode, buildings: d.worldCityBuildings, roads: d.worldCityRoads, bloom: d.neonBloom, style: d.visualStyle}; }),
    minimap: page => page.evaluate(() => globalThis.__arondightRealWorld?.toggleMinimapExpanded?.()),
    pauseMenu: page => page.keyboard.press("Escape"),
  },
  kern: {
    url: KERN,
    async boot(page) {
      await page.waitForFunction(() => { const b = document.querySelector("html.sc-original-view #gameMenuStart") || document.getElementById("scStart"); return b && !b.disabled; }, null, {timeout: 120000});
    },
    async start(page) {
      await page.waitForFunction(() => window.SpawnCity?.hud?.()?.placed, null, {timeout: 120000, polling: 1000}).catch(() => {});
      await page.evaluate(() => (document.querySelector("html.sc-original-view #gameMenuStart") || document.getElementById("scStart")).click());
      await page.waitForFunction(() => document.getElementById("scMenu")?.hidden === true || getComputedStyle(document.getElementById("scMenu")).display === "none", null, {timeout: 60000}).catch(() => {});
    },
    mode: (page, m) => page.evaluate(m => window.SpawnCity?.command?.(`mode ${m}`), m),
    state: page => page.evaluate(() => { try { const h = window.SpawnCity.hud(); return {mode: h.mode, hp: h.hp, buildings: h.buildings, fps: h.fps}; } catch (e) { return {error: String(e)}; } }),
    minimap: page => page.evaluate(() => window.SpawnCity?.command?.("map")),
    pauseMenu: page => page.keyboard.press("Escape"),
  },
};

const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist", "--autoplay-policy=no-user-gesture-required"]});
try {
  for (const [dev, profile] of Object.entries(PART === "controls" ? {} : DEVICES)) {
    for (const [name, g] of Object.entries(GAMES)) {
      const ctx = await browser.newContext({...profile, geolocation: GPS, permissions: ["geolocation"], locale: "de-DE"});
      const page = await ctx.newPage();
      const run = {device: dev, game: name, steps: [], errors: []};
      report.runs.push(run);
      page.on("pageerror", e => run.errors.push(String(e.message || e).slice(0, 300)));
      page.on("console", m => { if (m.type() === "error") run.errors.push(`console: ${m.text().slice(0, 240)}`); });
      const step = async (label, fn, wait = 2500) => {
        const t = Date.now(); let note;
        try { note = await fn?.(); } catch (e) { run.errors.push(`${label}: ${String(e.message || e).slice(0, 300)}`); }
        await pause(wait);
        const file = `${dev}-${String(run.steps.length).padStart(2, "0")}-${label}-${name}.png`;
        try { await page.screenshot({path: path.join(out, file)}); } catch (e) { run.errors.push(`shot ${label}: ${e.message}`); }
        let ui = null, state = null;
        try { ui = await page.evaluate(dumpUi); } catch (e) { ui = {error: String(e)}; }
        try { state = await g.state(page); } catch (e) { state = {error: String(e)}; }
        // the same frame measurement in both games: requestAnimationFrame for 2 s
        try {
          state.raf = await page.evaluate(() => new Promise(res => { const t = []; let last = performance.now(); const t0 = last;
            const f = now => { t.push(now - last); last = now; if (now - t0 < 2000) requestAnimationFrame(f); else { t.sort((a, b) => a - b);
              res({fps: +(t.length / ((now - t0) / 1000)).toFixed(1), p95: +t[Math.floor(t.length * 0.95)].toFixed(1), max: +t[t.length - 1].toFixed(1)}); } };
            requestAnimationFrame(f); }));
        } catch (e) { state.raf = {error: String(e)}; }
        // The full page as the player sees it: markup (without canvases' pixels) and every CSS rule,
        // including the rules modules inject at run time - the source for translating the HUD 1:1.
        try {
          const snap = await page.evaluate(() => {
            const css = []; for (const sh of document.styleSheets) { try { for (const r of sh.cssRules) css.push(r.cssText); } catch { css.push(`/* ${sh.href} (cross-origin) */`); } }
            const vars = {}; const rs = getComputedStyle(document.documentElement); for (const n of rs) if (n.startsWith("--")) vars[n] = rs.getPropertyValue(n);
            return {html: document.body.outerHTML, bodyClass: document.body.className, css: css.join("\n"), vars};
          });
          fs.writeFileSync(path.join(out, file.replace(/\.png$/, ".html")), snap.html);
          fs.writeFileSync(path.join(out, file.replace(/\.png$/, ".css")), `/* body class: ${snap.bodyClass} */\n/* vars: ${JSON.stringify(snap.vars)} */\n${snap.css}`);
        } catch (e) { run.errors.push(`snap ${label}: ${e.message}`); }
        run.steps.push({label, ms: Date.now() - t, note: note === undefined ? undefined : note, shot: file, state, ui});
        console.log(dev, name, label, JSON.stringify(state).slice(0, 200));
      };
      const url = new URL(g.url); url.searchParams.set("menu", "1");
      await step("title", async () => { await page.goto(url.href, {waitUntil: "domcontentloaded", timeout: 90000}); await g.boot(page); }, 1500);
      // the title's panels (both games: the original's ids; Kern shows them with ?view=original)
      const clickIf = sel => page.evaluate(sel => { const b = document.querySelector(sel); if (!b) return "no " + sel; b.click(); return "ok"; }, sel);
      const escape = () => page.keyboard.press("Escape").catch(() => {});
      await step("title-changelog", () => clickIf("#gameMenuChangelog"), 1500); await escape(); await pause(600);
      await step("title-world", () => clickIf("#gameMenuWorld"), 1500); await escape(); await pause(600);
      await step("title-place", () => clickIf("#startPlaceButton"), 1500); await escape(); await pause(600);
      await step("start-drone", () => g.start(page), 6000);
      await step("foot", () => g.mode(page, "foot"), 3000);
      await step("car", () => g.mode(page, "car"), 3000);
      await step("drone-again", () => g.mode(page, "drone"), 3000);
      await step("minimap", () => g.minimap(page), 1200);
      await g.minimap(page).catch(() => {});
      await step("pause-menu", () => g.pauseMenu(page), 1200);
      await escape(); await pause(800);
      await step("settings-menu", () => clickIf("#mobileGameplaySettings"), 1500);
      await page.evaluate(() => { for (const h of document.querySelectorAll("dialog[open] .sec-head")) h.click(); }).catch(() => {});
      await step("settings-open-all", null, 1000);
      await ctx.close();
    }
  }
  if (PART !== "ui") await measureControls({browser, out, games: GAMES, devices: DEVICES, gps: GPS});
} finally {
  fs.writeFileSync(path.join(out, "compare.json"), JSON.stringify(report, null, 1));
  // A quick overview: which visible texts exist in one game but not in the other, per step.
  const texts = r => new Map(r.steps.map(s => [s.label, new Set((s.ui?.items || []).map(i => i.text).filter(Boolean))]));
  const lines = [];
  for (const dev of Object.keys(DEVICES)) {
    const o = report.runs.find(r => r.device === dev && r.game === "original"), k = report.runs.find(r => r.device === dev && r.game === "kern");
    if (!o || !k) continue;
    const ot = texts(o), kt = texts(k);
    for (const [label, set] of ot) {
      const ks = kt.get(label) || new Set();
      lines.push(`## ${dev} / ${label}`, `only original: ${[...set].filter(t => !ks.has(t)).join(" | ")}`, `only kern: ${[...ks].filter(t => !set.has(t)).join(" | ")}`, "");
    }
    lines.push(`errors original: ${o.errors.join(" | ")}`, `errors kern: ${k.errors.join(" | ")}`, "");
  }
  fs.writeFileSync(path.join(out, "texts.md"), lines.join("\n"));
  // frame rate side by side (fps, 95th percentile and worst frame in ms)
  const perf = ["| device | step | original fps / p95 / max | kern fps / p95 / max |", "|---|---|---|---|"];
  for (const dev of Object.keys(DEVICES)) {
    const o = report.runs.find(r => r.device === dev && r.game === "original"), k = report.runs.find(r => r.device === dev && r.game === "kern");
    if (!o || !k) continue;
    for (const st of o.steps) {
      const ks = k.steps.find(x => x.label === st.label), f = r => r ? `${r.fps} / ${r.p95} / ${r.max}` : "-";
      perf.push(`| ${dev} | ${st.label} | ${f(st.state?.raf)} | ${f(ks?.state?.raf)} |`);
    }
  }
  fs.writeFileSync(path.join(out, "perf.md"), perf.join("\n") + "\n");
  await browser.close();
}
