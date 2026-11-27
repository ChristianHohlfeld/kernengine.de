// Browser smoke test of SpawnCity on Kern, the small game - a drone over the real city (Playwright + Chromium,
// software WebGL): loads the page, waits for the world, flies the drone (arm, climb, forward, the cameras, landing),
// tries TON / MUSIK / RESET and takes screenshots.
//
//   node kern/games/spawncity/tests/smoke.mjs [shots-dir]      (SC_SMOKE=desktop|phone: one run only)
//
// (Until 26.11.2026 it also walked, drove, flew the jet and played two tabs of multiplayer: out of the small game with
// them - BLUEPRINTS.md, "Herausgenommen am 26.11.".)
//
// Serves the repository root itself (python3 -m http.server). Two runs:
//   * desktop with map data: the page's TileJSON and tile requests are answered by the test with
//     synthetic OpenMapTiles vector tiles (gzip, as OpenFreeMap serves them) - the real MVT path - and
//     a synthetic elevation (Terrarium PNG)
//   * phone (touch, landscape) offline: the procedural stand-in town, flown with the fingers
// Needs kern/games/spawncity/web/spawncity.wasm (kern/games/spawncity/build.sh).
import {createRequire} from "node:module";
import {spawn} from "node:child_process";
import fs from "node:fs";
import path from "node:path";
import zlib from "node:zlib";
import {fileURLToPath} from "node:url";

const {chromium} = await import("playwright").catch(() => createRequire(import.meta.url)("playwright"));
const here = path.dirname(fileURLToPath(import.meta.url));
const root = path.resolve(here, "../../../..");
// The smoke test checks Kern's own HUD (?view=kern); the original view has view_check / compare.
const kernView = q => (q && q.startsWith("?") ? `${q}&view=kern` : `?view=kern${q || ""}`);
const shots = path.resolve(process.argv[2] || process.env.SC_SHOTS || path.join(root, "generated/spawncity-shots"));
fs.mkdirSync(shots, {recursive: true});
const port = 8700 + Math.floor(Math.random() * 200);
const server = spawn("python3", ["-m", "http.server", String(port), "--bind", "127.0.0.1"], {cwd: root, stdio: "ignore"});
await new Promise(r => setTimeout(r, 800));
let failures = 0, checks = 0;
const check = (ok, what) => { checks++; if (!ok) { failures++; console.log("FAIL", what); } else console.log("ok  ", what); };
const cleanup = () => { try { server.kill(); } catch {} };
process.on("exit", cleanup);

// ---- a synthetic OpenMapTiles tile (MVT 2.1): blocks of houses along streets, a park, water ----
function varint(out, v) { v = BigInt(v); while (v >= 0x80n) { out.push(Number(v & 0x7fn) | 0x80); v >>= 7n; } out.push(Number(v)); }
const key = (out, field, wire) => varint(out, (field << 3) | wire);
const bytes = (out, field, data) => { key(out, field, 2); varint(out, data.length); out.push(...data); };
const zz = v => (v << 1) ^ (v >> 31);
const utf8 = s => [...new TextEncoder().encode(s)];
function layer(name, features) {
  const keys = [], values = [], l = [];
  key(l, 15, 0); varint(l, 2);
  bytes(l, 1, utf8(name));
  for (const f of features) {
    const tags = [];
    for (const [k, v] of Object.entries(f.tags || {})) {
      let ki = keys.indexOf(k); if (ki < 0) { keys.push(k); ki = keys.length - 1; }
      const vk = JSON.stringify(v); let vi = values.findIndex(x => JSON.stringify(x) === vk); if (vi < 0) { values.push(v); vi = values.length - 1; }
      varint(tags, ki); varint(tags, vi);
    }
    const g = []; let x = 0, y = 0;
    for (const part of f.parts) {
      varint(g, (1 << 3) | 1); varint(g, zz(part[0][0] - x)); varint(g, zz(part[0][1] - y)); [x, y] = part[0];
      varint(g, ((part.length - 1) << 3) | 2);
      for (const p of part.slice(1)) { varint(g, zz(p[0] - x)); varint(g, zz(p[1] - y)); [x, y] = p; }
      if (f.type === 3) varint(g, (1 << 3) | 7);
    }
    const fb = [];
    if (f.id) { key(fb, 1, 0); varint(fb, f.id); }
    bytes(fb, 2, tags); key(fb, 3, 0); varint(fb, f.type); bytes(fb, 4, g);
    bytes(l, 2, fb);
  }
  for (const k of keys) bytes(l, 3, utf8(k));
  for (const v of values) {
    const e = [];
    if (typeof v === "string") bytes(e, 1, utf8(v));
    else { key(e, 3, 1); const b = Buffer.alloc(8); b.writeDoubleLE(v); e.push(...b); }
    bytes(l, 4, e);
  }
  key(l, 5, 0); varint(l, 4096);
  const t = []; bytes(t, 3, l);
  return t;
}
function syntheticTile(z, x, y) {
  const houses = [], roads = [];
  let id = 1;
  // A street grid every 400 units (~160 m), terraced houses along each block's edges.
  for (let gx = 0; gx < 4096; gx += 400) roads.push({type: 2, tags: {class: gx % 1200 === 0 ? "primary" : "minor"}, parts: [[[gx, -64], [gx, 4160]]]});
  for (let gy = 0; gy < 4096; gy += 400) roads.push({type: 2, tags: {class: gy % 1200 === 0 ? "secondary" : "minor"}, parts: [[[-64, gy], [4160, gy]]]});
  for (let bx = 0; bx < 4096; bx += 400)
    for (let by = 0; by < 4096; by += 400) {
      if ((bx / 400 + by / 400) % 7 === 3) continue;  // squares
      for (let i = 0; i < 4; i++) {
        const x0 = bx + 30 + i * 85, y0 = by + 30, h = 9 + ((x0 * 7 + y0 * 13 + x * 3 + y) % 5) * 3.2;
        houses.push({type: 3, id: id++, tags: {render_height: h}, parts: [[[x0, y0], [x0 + 75, y0], [x0 + 75, y0 + 70], [x0, y0 + 70]]]});
        const y1 = by + 300;
        houses.push({type: 3, id: id++, tags: {render_height: h + 3}, parts: [[[x0, y1], [x0 + 75, y1], [x0 + 75, y1 + 70], [x0, y1 + 70]]]});
      }
    }
  const water = [{type: 3, tags: {class: "lake"}, parts: [[[0, 0], [4096, 0], [4096, 120], [0, 120]]]}];
  const park = [{type: 3, tags: {class: "park"}, parts: [[[1230, 1230], [1570, 1230], [1570, 1570], [1230, 1570]]]}];
  return zlib.gzipSync(Buffer.from([...layer("building", houses), ...layer("transportation", roads), ...layer("water", water), ...layer("park", park)]));
}

// ---- a synthetic Terrarium elevation tile (PNG, RGB = (h + 32768) * 256): rolling hills ----
function crc32(buf) {
  let c = ~0;
  for (const b of buf) { c ^= b; for (let k = 0; k < 8; k++) c = (c >>> 1) ^ (0xedb88320 & -(c & 1)); }
  return ~c >>> 0;
}
function png(width, height, rgb) {
  const chunk = (type, data) => {
    const len = Buffer.alloc(4); len.writeUInt32BE(data.length);
    const td = Buffer.concat([Buffer.from(type), data]), crc = Buffer.alloc(4); crc.writeUInt32BE(crc32(td));
    return Buffer.concat([len, td, crc]);
  };
  const ihdr = Buffer.alloc(13); ihdr.writeUInt32BE(width, 0); ihdr.writeUInt32BE(height, 4); ihdr[8] = 8; ihdr[9] = 2;
  const raw = Buffer.alloc((width * 3 + 1) * height);
  for (let y = 0; y < height; y++) { raw[y * (width * 3 + 1)] = 0; rgb.copy(raw, y * (width * 3 + 1) + 1, y * width * 3, (y + 1) * width * 3); }
  return Buffer.concat([Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]), chunk("IHDR", ihdr), chunk("IDAT", zlib.deflateSync(raw)), chunk("IEND", Buffer.alloc(0))]);
}
function syntheticDem(z, x, y) {
  const rgb = Buffer.alloc(256 * 256 * 3);
  for (let j = 0; j < 256; j++)
    for (let i = 0; i < 256; i++) {
      const gx = x * 256 + i, gy = y * 256 + j;  // ~13 m per pixel at z13
      const v = 400 + 18 * Math.sin(gx / 60) + 12 * Math.cos(gy / 45) + 32768;
      const o = (j * 256 + i) * 3;
      rgb[o] = Math.floor(v / 256); rgb[o + 1] = Math.floor(v) % 256; rgb[o + 2] = Math.floor((v % 1) * 256);
    }
  return png(256, 256, rgb);
}

const browser = await chromium.launch({args: ["--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader", "--ignore-gpu-blocklist", "--autoplay-policy=no-user-gesture-required"]});


async function run({name, url, context, mapped}) {
  const ctx = await browser.newContext(context);
  const page = await ctx.newPage();
  const errors = [];
  page.on("pageerror", e => errors.push(e.message));
  page.on("console", m => { if (m.type() === "error" && !/tile|Failed to load resource/i.test(m.text())) errors.push(m.text()); });
  let tileRequests = 0, demRequests = 0;
  if (mapped) {
    await page.route("https://tiles.test/planet", r => r.fulfill({contentType: "application/json", body: JSON.stringify({tiles: ["https://tiles.test/t/{z}/{x}/{y}.pbf"]})}));
    await page.route("https://dem.test/**", r => {
      demRequests++;
      const [z, x, y] = r.request().url().match(/(\d+)\/(\d+)\/(\d+)\.png/).slice(1).map(Number);
      return r.fulfill({body: syntheticDem(z, x, y), headers: {"content-type": "image/png"}});
    });
    await page.route("https://tiles.test/t/**", r => {
      tileRequests++;
      const [z, x, y] = r.request().url().match(/(\d+)\/(\d+)\/(\d+)\.pbf/).slice(1).map(Number);
      return r.fulfill({body: syntheticTile(z, x, y), headers: {"content-type": "application/x-protobuf"}});
    });
  }
  const t0 = Date.now();
  await page.goto(`http://127.0.0.1:${port}/kern/games/spawncity/web/index.html${kernView(url)}`);
  await page.waitForFunction(() => window.SpawnCity?.hud()?.placed, null, {timeout: 300000});
  console.log(`[${name}] world placed after ${((Date.now() - t0) / 1000).toFixed(1)} s`);
  const hud = () => page.evaluate(() => window.SpawnCity.hud());
  const cmd = c => page.evaluate(c => window.SpawnCity.command(c), c);
  /** Polls until `fn(hud)` holds (software WebGL draws a few frames a second). */
  const until = async (fn, ms = 120000) => {
    const end = Date.now() + ms;
    for (;;) {
      const h = await hud().catch(() => null);
      if (h && fn(h)) return true;
      if (Date.now() > end) return false;
      await page.waitForTimeout(400);
    }
  };
  // The game's time: its fixed steps (1/240 s each). Controls are held for `seconds` of it (software WebGL renders a
  // few frames per second, and the game takes at most 0.25 s a frame), at most `limit` ms of wall time.
  const gameTime = async () => (await hud()).steps / 240;
  const holdFor = async (press, release, seconds = 3, limit = 400000) => {
    const g0 = await gameTime(), end = Date.now() + limit;
    await press();
    while ((await gameTime()) - g0 < seconds && Date.now() < end) await page.waitForTimeout(150);
    await release();
  };
  const hold = (keys, seconds = 3) => holdFor(async () => { for (const k of keys) await page.keyboard.down(k); },
    async () => { for (const k of keys) await page.keyboard.up(k); }, seconds);
  let cdp = null;
  const finger = async (type, x, y) => {
    cdp ||= await ctx.newCDPSession(page);
    await cdp.send("Input.dispatchTouchEvent", {type, touchPoints: type === "touchEnd" ? [] : [{x, y, id: 1}]});
  };
  // a control of the game's touch layout, where the page drew it (kern/web/studio/game.mjs TouchOverlay)
  const touchBox = sel => page.evaluate(sel => { const e = document.querySelector(sel); if (!e) return null; const r = e.getBoundingClientRect(); return r.width > 2 ? {x: r.x + r.width / 2, y: r.y + r.height / 2} : null; }, sel);
  const tap = async sel => { const b = await touchBox(sel); if (!b) return false; await finger("touchStart", b.x, b.y); await page.waitForTimeout(250); await finger("touchEnd"); return true; };
  // a finger on a stick: down, dragged by (dx, dy) px, held for `seconds` of game time, lifted
  const touchHold = async (sel, dx, dy, seconds = 3) => {
    const b = await touchBox(sel);
    if (!b) return false;
    await holdFor(async () => { await finger("touchStart", b.x, b.y); await finger("touchMove", b.x + dx, b.y + dy); }, () => finger("touchEnd"), seconds);
    return true;
  };
  const vw = context.viewport.width, vh = context.viewport.height;
  const shot = async file => {
    await page.waitForTimeout(400);
    const p = path.join(shots, `${name}-${file}.png`);
    await page.screenshot({path: p, timeout: 900000});
    console.log("   screenshot", p);
  };

  let h = await hud();
  check(h.mode === "drone", `[${name}] the game is the drone (${h.mode})`);
  check(mapped ? h.realData && !h.offline : h.offline, `[${name}] ${mapped ? "map tiles loaded through the MVT path" : "offline stand-in town"} (tiles ${JSON.stringify(h.tiles)})`);
  // world_building_collisions.mjs: the collision is the working set around the drone (<= 480 within 340 m).
  // The game starts before the city is there (like the original), so the houses stream in after the start.
  {
    const t1 = Date.now();
    await until(h => h.tiles.colliders > 20 && (h.tiles.t.built >= 0 || h.tiles.t.request < 0) /* offline town: no tile milestones */, 120000);
    h = await hud();
    console.log(`[${name}] city built ${((Date.now() - t1) / 1000).toFixed(1)} s after the start (tiles ${JSON.stringify(h.tiles)})`);
  }
  check(h.tiles.buildings > 50 && h.tiles.colliders > 20 && h.tiles.colliders <= 480, `[${name}] buildings with collision (${h.tiles.buildings} / ${h.tiles.colliders})`);
  if (mapped) check(tileRequests >= 1 && demRequests >= 1, `[${name}] map tiles and elevation fetched by the page (${tileRequests} / ${demRequests})`);
  check(h.agl >= 0 && h.agl < 1.5, `[${name}] the drone stands on the ground (${h.agl} m above it)`);
  await shot("0-start");

  // Arm (R, the ARM button) once the flight controller has calibrated, climb (Space, the altitude pad), fly forward
  // (W, the left stick).
  check(await until(h => h.drone && !h.drone.calibrating, 120000), `[${name}] the flight controller has calibrated`);
  if (context.hasTouch) {
    // (the first finger on the glass brings up the touch layout - it is not a control's yet)
    await finger("touchStart", vw / 2, vh / 3); await page.waitForTimeout(250); await finger("touchEnd");
    check(await until(h => h.touch && h.ctl.device === "touch", 30000), `[${name}] the touch layout`);
    await until(() => false, 1500);   // (the page draws it)
    check(await tap('.k-touch [data-touch="arm"]'), `[${name}] the ARM button is there`);
  } else await page.keyboard.press("KeyR");
  check(await until(h => h.drone.armed, 60000), `[${name}] armed`);
  const alt0 = (await hud()).alt;
  if (context.hasTouch) {
    // (the altitude pad right of the move stick, simulator.mjs #soloClearance: a finger on its upper half)
    check(await touchHold('.k-touch [data-touch="clearance"]', 0, -40, 3), `[${name}] the altitude pad is there`);
  } else await hold(["Space"], 3);
  h = await hud();
  check(h.alt > alt0 + 1.5, `[${name}] it climbs (${alt0.toFixed(1)} -> ${h.alt.toFixed(1)} m)`);
  await shot("1-climbed");
  const p0 = [h.x, h.y];
  if (context.hasTouch) await touchHold('.k-touch [data-stick="move"]', 0, -50, 3);
  else await hold(["KeyW"], 3);
  h = await hud();
  check(Math.hypot(h.x - p0[0], h.y - p0[1]) > 3, `[${name}] it flies forward (${Math.hypot(h.x - p0[0], h.y - p0[1]).toFixed(1)} m)`);
  await shot("2-flight");

  // The cameras: FPV, follow, third (C; the touch layout of the drone has no camera button: the command) - each one a
  // view of its own.
  const eyes = [];
  for (let i = 0; i < 3; i++) {
    const c = (await hud()).drone.cam;
    if (context.hasTouch) await cmd("cam");
    else await page.keyboard.press("KeyC");
    check(await until(h => h.drone.cam === (c + 1) % 3, 30000), `[${name}] the next camera (${c} -> ${(await hud()).drone.cam})`);
    const s0 = (await hud()).steps;
    await until(h => h.steps > s0 + 60, 60000);   // (the view of a few frames on)
    eyes.push(JSON.parse(await cmd("camera")).eye);
    await shot(`3-cam${(c + 1) % 3}`);
  }
  check(new Set(eyes.map(e => e.map(v => v.toFixed(1)).join())).size === 3, `[${name}] three views (${JSON.stringify(eyes)})`);

  // TON / MUSIK: the page's switches change the game's settings; RESET: the drone back at the start, on the ground.
  const sw = async which => page.evaluate(w => { const b = document.querySelector(`[data-sw="${w}"]`); if (!b) return false; b.click(); return true; }, which);
  for (const [which, key, flag] of [["sound", "audio.on", "audioOn"], ["music", "music.on", "musicOn"]]) {
    const before = await cmd(`get ${key}`);
    check(await sw(which), `[${name}] the ${which} switch is there`);
    check(await until(h => String(h.ui[flag] ? 1 : 0) !== before, 30000), `[${name}] ${which === "sound" ? "TON" : "MUSIK"} switches ${key} (${before} -> ${await cmd(`get ${key}`)})`);
    await sw(which);
    check(await until(h => String(h.ui[flag] ? 1 : 0) === before, 30000), `[${name}] ... and back (${await cmd(`get ${key}`)})`);
  }
  const resets = (await hud()).ui.resets;
  check(await page.evaluate(() => { const b = [...document.querySelectorAll('[data-act="reset"]')].find(e => e.offsetParent); if (!b) return false; b.click(); return true; }), `[${name}] the RESET button is there`);
  check(await until(h => h.ui.resets === resets + 1 && !h.drone.armed && h.agl < 1.5, 60000), `[${name}] RESET: the drone back at the start, on the ground (${JSON.stringify((await hud()).drone)}, ${(await hud()).agl} m)`);
  await shot("4-reset");
  check(errors.length === 0, `[${name}] no page errors ${errors.length ? JSON.stringify(errors.slice(0, 5)) : ""}`);
  await ctx.close();
}

try {
  const only = process.env.SC_SMOKE || "";  // "desktop" or "phone" runs one of them
  if (!only || only === "desktop") await run({name: "desktop", url: "?tilejson=https://tiles.test/planet&demtiles=https://dem.test/{z}/{x}/{y}.png", mapped: true, context: {viewport: {width: 1280, height: 720}}});
  if (!only || only === "phone") await run({name: "phone", url: "?offline=1&mute=1", mapped: false,
    context: {viewport: {width: 844, height: 390}, deviceScaleFactor: 2, isMobile: true, hasTouch: true}});
} finally {
  await browser.close();
  cleanup();
}
console.log(`${checks} checks, ${failures} failures`);
process.exit(failures ? 1 : 0);
