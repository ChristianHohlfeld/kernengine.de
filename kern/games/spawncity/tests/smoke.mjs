// Browser smoke test of SpawnCity on Kern (Playwright + Chromium, software WebGL):
// loads the page, waits for the world, steers every mode for 3 s and takes screenshots.
//
//   node kern/games/spawncity/tests/smoke.mjs [shots-dir]      (SC_SMOKE=desktop|phone: one run only)
//
// Serves the repository root itself (python3 -m http.server). Two runs:
//   * desktop with map data: the page's TileJSON and tile requests are answered by the test with
//     synthetic OpenMapTiles vector tiles (gzip, as OpenFreeMap serves them) - the real MVT path
//   * phone (touch, landscape) offline: the procedural stand-in town
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
  let tileRequests = 0;
  if (mapped) {
    await page.route("https://tiles.test/planet", r => r.fulfill({contentType: "application/json", body: JSON.stringify({tiles: ["https://tiles.test/t/{z}/{x}/{y}.pbf"]})}));
    await page.route("https://dem.test/**", r => {
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
  // The level's rules run as their compiled C++ class (src/level_blueprints.cpp, written from the
  // level by the build): not in the interpreter, not in the JIT.
  const blueprints = JSON.parse(await cmd("blueprints"));
  const rules = blueprints.find(b => b.name === "Wanted");
  check(rules && rules.tier === "native C++" && !rules.disabled, `[${name}] the rules' Blueprint runs as its C++ class (${JSON.stringify(blueprints)})`);
  const shot = async file => {
    await page.waitForTimeout(400);
    const p = path.join(shots, `${name}-${file}.png`);
    await page.screenshot({path: p, timeout: 900000});
    console.log("   screenshot", p);
  };
  // Controls are held for 3 s of game time (software WebGL renders a few frames per second, and the
  // game clamps a frame to 0.1 s), at most `limit` ms of wall time.
  const holdFor = async (press, release, seconds = 3, limit = 400000) => {
    const gameTime = async () => { const t = (await hud()).t; if (!Number.isFinite(t)) throw Error("HUD without game time"); return t; };
    const t0 = await gameTime(), end = Date.now() + limit;
    await press();
    while ((await gameTime()) - t0 < seconds && Date.now() < end) await page.waitForTimeout(150);
    await release();
  };
  const hold = (keys, seconds = 3) => holdFor(async () => { for (const k of keys) await page.keyboard.down(k); },
    async () => { for (const k of keys) await page.keyboard.up(k); }, seconds);
  let cdp = null;
  const touchHold = async (x, y, seconds = 3) => {
    // A finger on a stick: down, dragged up 50 px, held, lifted (CDP touch events).
    cdp ||= await ctx.newCDPSession(page);
    await holdFor(async () => {
      await cdp.send("Input.dispatchTouchEvent", {type: "touchStart", touchPoints: [{x, y, id: 1}]});
      await cdp.send("Input.dispatchTouchEvent", {type: "touchMove", touchPoints: [{x, y: y - 50, id: 1}]});
    }, () => cdp.send("Input.dispatchTouchEvent", {type: "touchEnd", touchPoints: []}), seconds);
  };
  const vw = context.viewport.width, vh = context.viewport.height;
  let h = await hud();
  check(h.mode === "foot", `[${name}] starts on foot`);
  check(mapped ? h.realData && !h.offline : h.offline, `[${name}] ${mapped ? "map tiles loaded through the MVT path" : "offline stand-in town"} (tiles ${JSON.stringify(h.tiles)})`);
  // world_building_collisions.mjs: the collision is the working set around the player (<= 480 within 340 m).
  // The game starts before the city is there (like the original), so the houses stream in after the start.
  {
    const t1 = Date.now();
    await page.waitForFunction(() => { const t = window.SpawnCity?.hud()?.tiles; return t && t.colliders > 20 && (t.t?.built >= 0 || t.t?.request < 0); }  /* offline town: no tile milestones */, null, {timeout: 120000}).catch(() => {});
    h = await hud();
    console.log(`[${name}] city built ${((Date.now() - t1) / 1000).toFixed(1)} s after the start (tiles ${JSON.stringify(h.tiles)})`);
  }
  check(h.tiles.buildings > 50 && h.tiles.colliders > 20 && h.tiles.colliders <= 480, `[${name}] buildings with collision (${h.tiles.buildings} / ${h.tiles.colliders})`);
  if (mapped) check(tileRequests >= 1, `[${name}] tiles fetched by the page (${tileRequests})`);
  // The ground (terrain.h): real elevation through the Terrarium path, water basins, the player on it.
  if (mapped) check(h.terrain?.dem && h.terrain.max - h.terrain.min > 5, `[${name}] terrain elevation loaded (${JSON.stringify(h.terrain)})`);
  else check(h.terrain?.settled && !h.terrain.dem, `[${name}] flat ground offline (${JSON.stringify(h.terrain)})`);
  check(h.terrain?.water > 100 && h.terrain.collision > 4, `[${name}] water cells and ground collision (${JSON.stringify(h.terrain)})`);
  {
    const g = JSON.parse(await cmd("ground"));
    check(Math.abs(g.playerZ - 0.73 - g.z) < 0.3, `[${name}] the player stands on the terrain (${JSON.stringify(g)})`);
  }
  await shot("0-start");

  // On foot: walk 3 s.
  const p0 = [h.lon, h.lat];
  if (context.hasTouch) await touchHold(110, vh - 110);
  else await hold(["KeyW"]);
  h = await hud();
  check(Math.hypot(h.lon - p0[0], h.lat - p0[1]) > 1e-6, `[${name}] walked on foot`);
  await shot("1-foot");

  // Controls (CONTROLS.md, game_controls.cpp): keyboard, a synthetic Xbox pad, touch.
  {
    const gameWait = async s => { const t = (await hud()).t; await page.waitForFunction(([t, s]) => window.SpawnCity.hud().t > t + s, [t, s], {timeout: 300000}).catch(() => {}); };
    const ctl = async () => (await hud()).ctl || {};
    if (!context.hasTouch) {
      check((await ctl()).device === "desktop", `[${name}] desktop device (${JSON.stringify(await ctl())})`);
      check((await page.locator(".sc-help").textContent()).includes("WASD move"), `[${name}] the key line (desktop_controls.mjs HINTS)`);
      let w0 = (await hud()).weapon;
      await page.keyboard.press("KeyQ"); await gameWait(0.3);
      check((await hud()).weapon !== w0, `[${name}] Q: next weapon (${w0} -> ${(await hud()).weapon})`);
      await page.keyboard.press("Digit4"); await gameWait(0.2);
      check((await ctl()).aiming, `[${name}] 4: air strike aiming`);
      await page.keyboard.press("Escape"); await gameWait(0.2);
      check(!(await ctl()).aiming && await page.locator("#scQuit").isHidden(), `[${name}] Esc cancels the aiming (no quit dialog)`);
      const c0 = (await ctl()).nade.carry;
      await page.keyboard.down("KeyT"); await gameWait(0.6);
      check((await ctl()).nade.left > 2.5 && (await ctl()).nade.left < 4, `[${name}] T held: the grenade cooks (${JSON.stringify((await ctl()).nade)})`);
      await page.keyboard.up("KeyT"); await gameWait(0.2);
      check((await ctl()).nade.carry === c0 - 1 && (await ctl()).nade.left < 0, `[${name}] T released: thrown (${JSON.stringify((await ctl()).nade)})`);
      // A synthetic Xbox pad (standard mapping) through navigator.getGamepads.
      await page.evaluate(() => {
        const pad = {connected: true, mapping: "standard", id: "Xbox Wireless Controller (STANDARD GAMEPAD Vendor: 045e)", index: 0, axes: [0, 0, 0, 0],
          buttons: Array.from({length: 17}, () => ({pressed: false, value: 0}))};
        window.__pad = pad;
        window.__padSet = (i, on) => { pad.buttons[i] = {pressed: on, value: on ? 1 : 0}; };
        navigator.getGamepads = () => [pad];
      });
      const tap = async (i, s = 0.25) => { await page.evaluate(i => window.__padSet(i, true), i); await gameWait(s); await page.evaluate(i => window.__padSet(i, false), i); await gameWait(0.2); };
      w0 = (await hud()).weapon;
      await tap(3);
      check((await hud()).weapon !== w0 && (await ctl()).pad, `[${name}] pad Y: next weapon, pad-input (${w0} -> ${(await hud()).weapon})`);
      check(await page.locator(".sc-help").evaluate(e => !e.textContent), `[${name}] pad-input hides the key line`);
      const p1 = [(await hud()).lon, (await hud()).lat];
      await page.evaluate(() => { window.__pad.axes[1] = -1; });
      await gameWait(2);
      await page.evaluate(() => { window.__pad.axes[1] = 0; });
      check(Math.hypot((await hud()).lon - p1[0], (await hud()).lat - p1[1]) > 1e-6, `[${name}] pad LS walks`);
      await tap(12);
      check((await ctl()).aiming, `[${name}] D-pad up: strike aiming`);
      await tap(1);
      check(!(await ctl()).aiming, `[${name}] B cancels`);
      await tap(8, 0.7);
      check(await page.locator("#mpMap").count() === 1, `[${name}] VIEW held: the big map`);
      await tap(1);
      check(await page.locator("#mpMap").count() === 0, `[${name}] B closes the map`);
      await tap(13);
      check((await hud()).mode === "drone", `[${name}] D-pad down: drone`);
      await tap(13);
      check((await hud()).mode === "foot", `[${name}] D-pad down: back on foot`);
      await page.evaluate(() => { navigator.getGamepads = () => []; });
      await page.keyboard.press("KeyZ"); await gameWait(0.2);
      check(!(await ctl()).pad, `[${name}] a key ends pad-input`);
    } else {
      cdp ||= await ctx.newCDPSession(page);
      const touchTap = async (x, y) => {
        await cdp.send("Input.dispatchTouchEvent", {type: "touchStart", touchPoints: [{x, y, id: 7}]});
        await gameWait(0.15);
        await cdp.send("Input.dispatchTouchEvent", {type: "touchEnd", touchPoints: []});
        await gameWait(0.15);
      };
      const box = async id => page.locator(`.k-touch [data-touch="${id}"]`).boundingBox();
      check((await ctl()).device === "touch", `[${name}] touch device`);
      for (const id of ["jump", "wpn", "mode", "strike", "nade"]) check(!!(await box(id)), `[${name}] touch button ${id}`);
      const w0 = (await hud()).weapon, b = await box("wpn");
      if (b) await touchTap(b.x + b.width / 2, b.y + b.height / 2);
      check((await hud()).weapon !== w0, `[${name}] the thumb weapon button cycles (${w0} -> ${(await hud()).weapon})`);
      await cmd("weapon 1");  // the Glock (Kern fires it): a tap on the screen shoots
      await gameWait(0.4);
      const m0 = (await hud()).gaps?.footShots ?? 0;  // shots on foot (no magazines: gameplay_final_runtime_v2.mjs)
      await touchTap(vw * 0.55, vh * 0.35);
      await page.waitForFunction(m => (window.SpawnCity.hud().gaps?.footShots ?? 0) > m, m0, {timeout: 30000}).catch(() => {});
      check(((await hud()).gaps?.footShots ?? 0) > m0, `[${name}] a tap on the screen fires (${m0} -> ${(await hud()).gaps?.footShots})`);
      const s = await box("strike");
      if (s) await touchTap(s.x + s.width / 2, s.y + s.height / 2);
      check((await ctl()).aiming, `[${name}] STRIKE: aiming`);
      const c = await box("strike_cancel");
      if (c) await touchTap(c.x + c.width / 2, c.y + c.height / 2);
      check(!(await ctl()).aiming, `[${name}] ABBRECHEN`);
      await cmd("weapon 1");
    }
  }

  // The town: people on the pavements; a wanted level brings a police unit with its crew.
  h = await hud();
  check(h.people >= 10, `[${name}] people in the streets: ${h.people}`);
  check(await cmd("wanted 2") === "2", `[${name}] wanted level 2`);
  const tw = (await hud()).t;
  // wanted_dispatch_guard.mjs: the drones are dispatched first and show up after the dispatch delay
  await page.waitForFunction(() => { const h = window.SpawnCity.hud(); return h.police > 0 && (h.policeDrones > 0 || h.gaps?.inbound > 0); }, null, {timeout: 300000}).catch(() => {});
  h = await hud();
  check(h.police > 0 && (h.policeDrones > 0 || h.gaps?.inbound > 0) && h.stars > 0, `[${name}] police on its way (${h.police} cruisers, ${h.policeDrones} drones, ${h.gaps?.inbound} inbound, ${h.stars} stars)`);
  await page.waitForFunction(t => window.SpawnCity.hud().t > t + 6, tw, {timeout: 400000}).catch(() => {});
  await shot("1b-police");
  check(await cmd("wanted 0") === "0", `[${name}] wanted level cleared`);
  // Weapons: the rocket launcher fires a rocket that explodes; the minimap has streets.
  check(await cmd("weapon 4") === "Raketenwerfer", `[${name}] rocket launcher in hand`);
  const booms = (await hud()).explosions;
  { const t = (await hud()).t; await page.waitForFunction(t => window.SpawnCity.hud().t > t + 0.4, t, {timeout: 120000}).catch(() => {}); }  // the launcher comes up (0.25 s)
  check((await cmd("attack")).startsWith("1 "), `[${name}] a rocket is fired`);
  await page.waitForFunction(b => window.SpawnCity.hud().explosions > b, booms, {timeout: 300000}).catch(() => {});
  check((await hud()).explosions > booms, `[${name}] the rocket explodes`);
  // What blasts leave (terrain_craters.mjs, world_impacts.mjs, facade_marks.mjs): a crater in the
  // terrain, a scorch, soot and a breach on the wall, rubble as Box3D bodies.
  {
    const b = await cmd("blast wall");
    check(b.startsWith("{"), `[${name}] a blast at a wall (${b})`);
    if (b.startsWith("{")) {
      const at = JSON.parse(b), g = JSON.parse(await cmd(`ground ${at.x} ${at.y}`));
      // the marks' counts reach the HUD on the next frames (a slow phone: wait for them)
      await page.waitForFunction(() => { const m = window.SpawnCity.hud().marks; return m && m.soot >= 1 && m.breach >= 1 && m.rubble >= 2; }, null, {timeout: 60000}).catch(() => {});
      h = await hud();
      check(h.marks.craters >= 1 && g.z < at.ground - 0.4, `[${name}] a crater in the ground (${at.ground} -> ${g.z})`);
      check(h.marks.scorch >= 1 && h.marks.soot >= 1 && h.marks.breach >= 1 && h.marks.rubble >= 2, `[${name}] scorch, soot, breach and rubble (${JSON.stringify(h.marks)})`);
      await shot("1c-blast-marks");
    }
  }
  await cmd("weapon 1");
  const map = JSON.parse(await cmd("map"));
  check(map.lanes && map.lanes.length > 0, `[${name}] minimap streets (${map.lanes?.length})`);

  // Performance and look (game_look.cpp): frame times, quality level, day clock, lights, weapon models.
  {
    const frame = () => page.evaluate(() => new Promise(r => requestAnimationFrame(() => requestAnimationFrame(r))));
    h = await hud();
    check(h.perf && h.perf.ms && h.perf.tier >= 0 && h.renderScale > 0, `[${name}] frame times and quality in the HUD (${JSON.stringify(h.perf)})`);
    check(await cmd("perf 1") === "on", `[${name}] frame-time line on`);
    await frame();
    check(await page.locator(".sc-perf").isVisible(), `[${name}] frame-time line shown`);
    await cmd("perf 0");
    check(JSON.parse(await cmd("quality 2")).tier === 2, `[${name}] quality level 2 fixed`);
    await frame(); await frame();
    // the host's pixel-ratio ceiling (render_stability.mjs renderPlatformProfile: software 0.30) x the level's 0.85
    const cw = await page.evaluate(() => { const c = document.getElementById("game"), k = window.SpawnCity.platform?.ceiling ?? 1; return [c.width, Math.round(c.clientWidth * k * 0.85), Math.max(1, Math.round(c.clientWidth * k * 0.85))]; });
    check(Math.min(Math.abs(cw[0] - cw[1]), Math.abs(cw[0] - cw[2])) <= 1, `[${name}] the drawing buffer follows the level's scale (${cw})`);
    check(JSON.parse(await cmd("quality auto")).auto === true, `[${name}] quality back to automatic`);
    check((await cmd("time 22:00")).endsWith("22:00"), `[${name}] time of day set`);
    await frame(); await frame();
    h = await hud();
    check(h.night > 0.6 && /^22:0\d$/.test(h.clock), `[${name}] night after "time 22:00" (${h.night}, ${h.clock})`);  // the clock runs on (480 s a day)
    check(JSON.parse(await cmd("lights")).length >= 1, `[${name}] point lights at night (${await cmd("lights")})`);
    await shot("1c-night");
    await cmd("time 0.40");
    check(await cmd("weapon 6") === "Messer", `[${name}] knife in hand`);
    await frame(); await frame();
    await page.waitForFunction(() => window.SpawnCity.hud().look?.viewModel === "KNIFE", null, {timeout: 30000}).catch(() => {});
    { const hk = await hud(); check(hk.look?.viewModel === "KNIFE", `[${name}] own first-person knife model (${JSON.stringify(hk.look)}, mode ${hk.mode}, weapon ${hk.weapon}, health ${hk.health}, ${await cmd("viewmodel")})`); }
    await cmd("weapon 1");
  }

  // sc-m3-gaps (game_gaps.cpp, PARITY.md): lamps, critical smoke, jet damage, spawn gate.
  {
    let g = JSON.parse(await cmd("gaps"));
    check(g.lamps > 0, `[${name}] street lamps placed (${g.lamps})`);
    const lb = await cmd("lampbreak 2");
    g = JSON.parse(await cmd("gaps"));
    check(/^lamp /.test(lb) && g.lampsBroken >= 1 && g.lampsFallen >= 1, `[${name}] a lamp post knocked down (${lb})`);
    let burnt = "";
    for (let i = 0; i < 16 && burnt !== "burning"; i++) burnt = await cmd("burn " + i);
    await page.waitForFunction(() => window.SpawnCity.hud().gaps?.smoke >= 1, null, {timeout: 30000}).catch(() => {});  // smoke starts on the next frame
    g = JSON.parse(await cmd("gaps"));
    check(burnt === "burning" && g.smoke >= 1, `[${name}] critical smoke on a burning car (${JSON.stringify({smoke: g.smoke, puffs: g.puffs})})`);
    check(["yes", "no"].includes(await cmd("canspawn 0 0")), `[${name}] spawn gate answers`);
  }

  // Street life, ragdolls, interiors (game_life.cpp, M3 life).
  {
    check(JSON.parse(await cmd("cows")).cows >= 5, `[${name}] a herd of cows`);
    check(JSON.parse(await cmd("pigeons")).flocks >= 1, `[${name}] a pigeon flock`);
    const pets = JSON.parse(await cmd("pets"));
    check(pets.pets >= 12 && pets.cats >= 4, `[${name}] dogs and cats (${JSON.stringify(pets)})`);
    check(JSON.parse(await cmd("ragdoll")).ok, `[${name}] a ragdoll is thrown`);
    const knocked = await cmd("knock");
    const tl = (await hud()).t;
    await page.waitForFunction(t => window.SpawnCity.hud().t > t + 6, tl, {timeout: 400000}).catch(() => {});
    const rs = JSON.parse(await cmd("ragdoll state"));
    check(rs.pelvis && rs.pelvis[2] - rs.ground < 0.6, `[${name}] the ragdoll lies on the ground (${JSON.stringify(rs)})`);
    let L = JSON.parse(await cmd("life"));
    if (knocked !== "nobody") check(L.getups >= 1, `[${name}] a knocked-down passer-by gets up again (${JSON.stringify(L)})`);
    check(JSON.parse(await cmd("cows kill")).dead >= 1, `[${name}] a blast kills a cow`);
    check(L.birds > 0 && L.trees >= 0, `[${name}] birds over the roofs (${L.birds}), trees (${L.trees})`);
    const go = await cmd("interior go");
    if (go !== "no building") {
      const ti = (await hud()).t;
      await page.waitForFunction(t => window.SpawnCity.hud().t > t + 4, ti, {timeout: 400000}).catch(() => {});
      let s = JSON.parse(await cmd("interior"));
      check(s.active && s.levels >= 1, `[${name}] the building at the door is enterable (${JSON.stringify(s)})`);
      if (s.active && s.roof) {
        await cmd("interior roof");
        const tr = (await hud()).t;
        await page.waitForFunction(t => window.SpawnCity.hud().t > t + 2, tr, {timeout: 400000}).catch(() => {});
        s = JSON.parse(await cmd("interior"));
        check(Math.abs(s.z - s.top) < 0.6, `[${name}] standing on the roof by the stairs (${JSON.stringify(s)})`);
        await shot("1c-roof");
        await cmd("interior door");  // back out in front of the door
      }
    }
  }

  // Menus and settings (game_ui.cpp, ui.mjs): the table, a change, pause, the dialog, WORLD, RESET.
  {
    const schema = JSON.parse(await cmd("settings schema"));
    check(schema.some(o => o.key === "world.jetpack") && schema.some(o => o.key === "car.maxKmh"), `[${name}] settings table (${schema.length} rows)`);
    check(await cmd("set mouse.sensitivity 150") === "ok" && await cmd("get mouse.sensitivity") === "150", `[${name}] a setting changes`);
    await cmd("set mouse.sensitivity 100");
    await cmd("pause 1");
    const tp = (await hud()).t;
    await page.waitForTimeout(1500);
    h = await hud();
    check(h.ui?.paused && Math.abs(h.t - tp) < 0.05, `[${name}] paused: the game stands (${tp} -> ${h.t})`);
    await cmd("pause 0");
    await page.evaluate(() => window.SpawnCity.ui.open("settings"));
    check(await page.locator("#scSettings").isVisible(), `[${name}] settings dialog open`);
    await shot("0-settings");
    await page.keyboard.press("Escape");
    await page.evaluate(() => new Promise(r => requestAnimationFrame(() => requestAnimationFrame(r))));
    check(!(await page.locator("#scSettings").isVisible()) && !(await hud()).ui.paused, `[${name}] Esc closes it and the game runs`);
    await cmd("set world.people 0");
    await page.waitForFunction(() => window.SpawnCity.hud().people === 0, null, {timeout: 60000}).catch(() => {});
    check((await hud()).people === 0, `[${name}] WORLD: people switched off`);
    await cmd("set world.people 1");
    const resets = (await hud()).ui.resets;
    const resetOk = await cmd("reset") === "ok";
    await page.evaluate(() => new Promise(r => requestAnimationFrame(() => requestAnimationFrame(r))));
    h = await hud();
    check(resetOk && h.ui.resets === resets + 1 && h.mode === "foot", `[${name}] RESET back to the start (${h.ui.resets}, ${h.mode})`);
    // The suggestion box signs its Nostr event in the game (nostr.cpp).
    const ev = await cmd(`nostr event 1700000000 ${"0".repeat(63)}3 ${"0".repeat(64)} [["t","spawncity-suggestion"]] Mehr Jets bitte`);
    check(ev.startsWith("{") && /"sig":"[0-9a-f]{128}"/.test(ev), `[${name}] suggestion signed as a Nostr event`);
    check(await cmd("set view.grid 1") === "ok", `[${name}] 1 m grid on`);
    await page.waitForTimeout(500);
    await shot("0c-grid");
    await cmd("set view.grid 0");
    // LIVE GPS: two fixes 20 m apart move the player there.
    const o = (await hud()).ui.origin;
    check(await cmd("gps on") === "on", `[${name}] GPS live on`);
    await cmd(`gps fix ${o[0]} ${o[1]} 5`);
    await page.waitForTimeout(1200);
    await cmd(`gps fix ${o[0] + 0.00018} ${o[1]} 5`);
    await page.waitForTimeout(2500);
    h = await hud();
    check(h.gps?.fix && h.gps.accepted === 2, `[${name}] GPS fixes taken (${JSON.stringify(h.gps)})`);
    check(await cmd("gps view follow") === "follow", `[${name}] GPS follow view`);
    await shot("0b-gps-follow");
    await cmd("gps off");
    // Another start place: the world moves and is built there.
    const moves = h.ui.moves;
    check(await cmd(`start ${o[0] + 0.004} ${o[1] + 0.004} Testort`) === "moving", `[${name}] start place command`);
    await page.waitForFunction(m => { const h = window.SpawnCity.hud(); return h.placed && h.ui.moves > m; }, moves, {timeout: 400000}).catch(() => {});
    h = await hud();
    check(h.placed && Math.abs(h.ui.origin[0] - o[0] - 0.004) < 1e-5, `[${name}] the world moved to the new start place (${h.ui.origin})`);
  }

  // Jetpack (WORLD option): a jump with ignition lifts the player over 2 m.
  {
    check(await cmd("jetpack burn") === "ok", `[${name}] jetpack ignition`);
    const t0 = (await hud()).t;
    let top = 0;
    while ((await hud()).t < t0 + 1.2) { top = Math.max(top, (await hud()).agl); await page.waitForTimeout(100); }
    const jp = JSON.parse(await cmd("jetpack"));
    check(jp.ignitions >= 1 && top > 1.5, `[${name}] the jetpack lifts (${top.toFixed(1)} m, ${JSON.stringify(jp)})`);
  }

  // Combat and events (game_military.cpp, game_events.cpp).
  {
    const until = (f, ms = 240000, arg = null) => page.waitForFunction(f, arg, {timeout: ms}).catch(() => {});
    const before = (await hud()).explosions;
    check(await cmd("strike 0 150") === "ok", `[${name}] air strike called`);
    // an ambient pass may be in the sky as well: the strike adds its pair (fighter_jets.mjs)
    await until(() => window.SpawnCity.hud().combat.jets >= 2);
    check((await hud()).combat.jets >= 2, `[${name}] the strike pair is in the air (${(await hud()).combat.jets} jets)`);
    await until(b => window.SpawnCity.hud().explosions > b + 3, 300000, before);
    await shot("1c-airstrike");
    check((await hud()).explosions > before + 3, `[${name}] the bombs fall (${(await hud()).explosions - before} explosions)`);
    check(await cmd("military start") === "on", `[${name}] military called in`);
    // a player the police cannot see shakes the stars off in ~9 s (wanted_system_logic.mjs escape) and
    // the army turns back: keep five stars up while waiting for it (the phone stands on a roof here)
    for (let i = 0; i < 90; i++) {
      const hw = await hud();
      if (hw.combat.helis > 0 && hw.combat.soldiers > 0) break;
      if (hw.stars < 5 || hw.combat.mil !== "on") await cmd("military start");
      await page.waitForFunction(t => window.SpawnCity.hud().t > t + 2, hw.t, {timeout: 120000}).catch(() => {});
    }
    let c = (await hud()).combat;
    { const hm = await hud(); check(c.mil === "on" && c.helis > 0, `[${name}] army: ${c.helis} heli, ${c.tanks} tank, ${c.soldiers} soldiers (mode ${hm.mode}, health ${hm.health}, stars ${hm.stars}, mil ${c.mil}, spawn ${JSON.stringify({a: hm.gaps?.spawnAllowed, d: hm.gaps?.spawnDenied})})`); }
    await shot("1d-military");
    check(await cmd("military off") === "off" && (await hud()).combat.helis === 0, `[${name}] military switched off`);
    await cmd("military on");
    // the wanted level from the army run may already have sent enemies of its own: one more is the check
    const enemiesBefore = (await hud()).combat.enemies;
    const enemyCmd = await cmd("enemy swat");
    await until(b => window.SpawnCity.hud().combat.enemies >= b + 1, 30000, enemiesBefore);
    check(enemyCmd === "SWAT" && (await hud()).combat.enemies >= enemiesBefore + 1, `[${name}] a sandbox enemy (${enemyCmd}, ${enemiesBefore} -> ${(await hud()).combat.enemies})`);
    check(await cmd("zombies force") === "force", `[${name}] zombie night forced`);
    await until(() => window.SpawnCity.hud().combat.zombies.alive > 0);
    c = (await hud()).combat;
    check(c.zombies.night && c.zombies.round === 1 && c.zombies.alive > 0, `[${name}] zombies rise (${JSON.stringify(c.zombies)})`);
    await shot("1e-zombies");
    await cmd("zombies day");
    const prov = await cmd("provoke");
    check(/^(gun|knife|fists) /.test(prov), `[${name}] a passer-by squares up (${prov})`);
    check(await cmd("race") === "on" && (await cmd("race fly")).startsWith("0 lap 1"), `[${name}] race track: start gate passed`);
    check(!!(await hud()).combat.race, `[${name}] race HUD`);
    await cmd("race");
    check((await cmd("contract")).startsWith("AUFTRAG"), `[${name}] next contract`);
  }

  // Drone: wait for the firmware's calibration, then climb and fly forward for 3 s.
  // The fights above leave burning cars and a damaged jet behind: RESET first (game_reset.mjs), so
  // the vehicle checks start from a clean world as a player would after a restart.
  await cmd("military off");
  await cmd("zombies day");
  await cmd("reset");
  { const t = (await hud()).t; await page.waitForFunction(t => window.SpawnCity.hud().t > t + 1, t, {timeout: 300000}).catch(() => {}); }
  // the army test left 5 stars: helicopters around a pilot standing still kill him (the player's
  // touch rounds count as his since they carry their shooter) - the drone tests start unwanted
  await cmd("wanted 0");
  check(await cmd("mode drone") === "drone", `[${name}] drone mode`);
  await page.waitForFunction(() => { const d = window.SpawnCity.hud()?.drone; return d && !d.calibrating; }, null, {timeout: 240000});
  // touch: the altitude pad right of the move stick (simulator.mjs #soloClearance) pushed up
  if (context.hasTouch) await touchHold(168 + 8 + 24, vh - 10 - 66);
  else { await hold(["Space"], 2); await hold(["KeyW", "Space"], 1); }
  h = await hud();
  check(h.drone && h.drone.armed, `[${name}] drone armed (state ${JSON.stringify(h.drone)})`);
  check(h.agl > 1.0, `[${name}] drone climbed: ${h.agl} m AGL`);
  await shot("2-drone");
  check(await cmd("cam") === "third" && await cmd("cam") === "fpv", `[${name}] drone cameras follow -> third -> FPV`);
  await shot("2b-drone-fpv");
  // GUIDED (guided_missile.mjs): a press launches, a second one within 5 s is refused.
  const g1 = await cmd("guided");
  check(g1.startsWith("launched"), `[${name}] GUIDED missile launched (${g1}, pilot dead ${(await hud()).ctl?.dead}, health ${(await hud()).health})`);
  check(await cmd("guided") === "denied" && (await hud()).guided?.cd > 3, `[${name}] GUIDED reloads 5 s (${JSON.stringify((await hud()).guided)})`);
  {
    const tg = (await hud()).t, booms = (await hud()).explosions;
    await page.waitForFunction(([t, b]) => { const h = window.SpawnCity.hud(); return h.explosions > b || h.t > t + 25; }, [tg, booms], {timeout: 400000}).catch(() => {});
    check((await hud()).explosions > booms && (await hud()).guided.flying === 0, `[${name}] the GUIDED missile went off`);
  }
  // critical_damage_logic.mjs: the drone at 28 % smokes and blows 2.05 s later.
  // flak may have hit the drone on its climb: hurt it down to 25 % from where it is
  const droneHp = (await hud()).droneHp ?? 100;
  const hurtOut = await cmd(`hurt ${Math.max(1, droneHp - 25)}`);
  await page.waitForFunction(() => window.SpawnCity.hud().details?.droneCritical, null, {timeout: 30000}).catch(() => {});  // armed on the next frame
  check(hurtOut.startsWith("drone 25") && (await hud()).details?.droneCritical, `[${name}] drone critical (${hurtOut}, ${JSON.stringify((await hud()).details)})`);
  await cmd("mode foot");
  check(await cmd("ballot new zombies 0 1 1 2") === "pending" && await cmd("ballot vote 2 1 1 2") === "approved", `[${name}] rule ballot (rules_vote_engine.mjs)`);
  await page.waitForFunction(() => window.SpawnCity.hud().logbook?.flights >= 1, null, {timeout: 30000}).catch(() => {});  // the flight ends on the next frame
  check((await hud()).logbook?.flights >= 1, `[${name}] the flight is in the logbook (${JSON.stringify((await hud()).logbook)})`);

  // Car: carjack the nearest car, drive 3 s.
  // Traffic (world_traffic.h): junctions from the street graph, lights on the main roads.
  {
    const tr = JSON.parse(await cmd("traffic"));
    check(tr.junctions > 3 && (mapped ? tr.lights > 0 : true), `[${name}] junctions and traffic lights (${JSON.stringify(tr)})`);
  }
  const car = await cmd("mode car");
  check(car === "car", `[${name}] car mode (${car})`);
  check(JSON.parse(await cmd("traffic")).ejected >= 1 || car !== "car", `[${name}] the carjacked driver got out`);
  if (car === "car") {
    if (context.hasTouch) await touchHold(110, vh - 110);  // LENKEN / GAS: the left stick up
    else await hold(["KeyW"]);
    h = await hud();
    check(h.speed > 1.0, `[${name}] the car drives: ${(h.speed * 3.6).toFixed(0)} km/h (mode ${h.mode}, hp ${h.carHp}, toast ${h.toast}, car ${JSON.stringify(h.car)}, ctl ${(await cmd("controls")).slice(0, 160)})`);
    await shot("3-car");
    check(await cmd("radio next") !== "off" && (await hud()).radio?.on, `[${name}] car radio: next station (${JSON.stringify((await hud()).radio)})`);
    check(await cmd("cam") === "cockpit", `[${name}] car cockpit view`);
    await shot("3b-car-cockpit");
    await cmd("cam");
    // Water (world_water.mjs): the car floats on the lake (buoyancy), the pilot swims.
    const wet = await cmd("water car");
    check(wet.startsWith("{"), `[${name}] the car is put on the water (${wet})`);
    if (wet.startsWith("{")) {
      const tw0 = (await hud()).t;
      await page.waitForFunction(t => window.SpawnCity.hud().t > t + 2.5, tw0, {timeout: 400000}).catch(() => {});
      const g = JSON.parse(await cmd("ground")), lake = JSON.parse(wet);
      check(g.water && g.playerZ > lake.bed + 1.2 && g.playerZ < g.level + 1.5, `[${name}] the car floats (${JSON.stringify(g)})`);
      check((await hud()).splashes > 0, `[${name}] a splash when it hit the water`);
      await shot("3c-car-water");
      await cmd("mode foot");
      const tw1 = (await hud()).t;
      await page.waitForFunction(t => window.SpawnCity.hud().t > t + 1, tw1, {timeout: 400000}).catch(() => {});
      h = await hud();
      const gs = JSON.parse(await cmd("ground"));
      check(h.swimming && Math.abs(gs.playerZ - (gs.level - 0.53)) < 0.3, `[${name}] the pilot swims, head above the water (${JSON.stringify(gs)})`);
      await shot("3d-swim");
    }
    await cmd("mode foot");
  }

  // Jet: board, take off jet-borne for 3 s.
  const jet = await cmd("mode jet");
  check(jet === "jet", `[${name}] jet mode (${jet})`);
  if (jet === "jet") {
    if (context.hasTouch) await touchHold(110, vh - 110);
    else await hold(["KeyW"]);
    h = await hud();
    check(h.agl > 2, `[${name}] the jet lifts off: ${h.agl} m AGL (mode ${h.mode}, hp ${h.jetHp}, jet ${JSON.stringify(h.jet)}, toast ${h.toast})`);
    await shot("4-jet");
    check(await cmd("eject") === "ok", `[${name}] ejection seat`);
    const t0 = (await hud()).t;
    await page.waitForFunction(t0 => window.SpawnCity.hud().t > t0 + 1.5, t0, {timeout: 400000});
    h = await hud();
    check(h.mode === "parachute" && h.chute, `[${name}] the canopy is open (${h.mode}, ${h.agl} m)`);
    await shot("5-parachute");
  }
  check(errors.length === 0, `[${name}] no page errors ${errors.length ? JSON.stringify(errors.slice(0, 5)) : ""}`);
  await ctx.close();
}

// ---- multiplayer: two tabs of one browser (BroadcastChannel), host + client --------------------
async function runMultiplayer() {
  const ctx = await browser.newContext({viewport: {width: 640, height: 360}});
  const errors = [];
  const open = async q => {
    const p = await ctx.newPage();
    p.on("pageerror", e => errors.push(e.message));
    await p.goto(`http://127.0.0.1:${port}/kern/games/spawncity/web/index.html${kernView(q)}`);
    return p;
  };
  const room = "smoke" + Math.floor(Math.random() * 1e6);
  const A = await open(`?offline=1&mute=1&mp=host&room=${room}`);
  await A.waitForFunction(() => window.SpawnCity?.hud()?.placed, null, {timeout: 300000});
  // The client starts somewhere else: it must take the host's map origin.
  const B = await open(`?offline=1&mute=1&mp=join&room=${room}&lat=47.67&lon=9.18`);
  await B.waitForFunction(() => window.SpawnCity?.hud()?.net?.connected && window.SpawnCity.hud().placed, null, {timeout: 400000});
  const hud = p => p.evaluate(() => window.SpawnCity.hud());
  const cmd = (p, c) => p.evaluate(c => window.SpawnCity.command(c), c);
  const until = (p, fn, arg, ms = 120000) => p.waitForFunction(fn, arg, {timeout: ms}).then(() => true).catch(() => false);
  check(await until(A, () => window.SpawnCity.hud().mp?.players?.length === 1) && await until(B, () => window.SpawnCity.hud().mp?.players?.length === 1),
    "[mp] host and client see each other");
  const ra = JSON.parse(await cmd(B, "net remotes")), rb = JSON.parse(await cmd(A, "net remotes"));
  check(ra.length === 1 && rb.length === 1, `[mp] poses both ways (${JSON.stringify(ra)} / ${JSON.stringify(rb)})`);
  check(Math.hypot(rb[0].px - ra[0].px, rb[0].py - ra[0].py) < 40, "[mp] the client spawned near the host (spawn anchor)");
  const ha = await hud(A), hb = await hud(B);
  check(ha.mp.index === 1 && hb.mp.index === 2 && ha.mp.color !== hb.mp.color, `[mp] P1 / P2 in their colours (${ha.mp.color} ${hb.mp.color})`);
  check(await A.evaluate(() => document.querySelectorAll(".mp-plate").length) === 1, "[mp] nameplate of the mate");
  // Friendly fire is off (world_options.mjs default): a hit does nothing ...
  const hp0 = (await hud(B)).health;
  await cmd(A, "weapon 2");
  check(await cmd(A, "net hit 30") === "hit", "[mp] the round reaches the mate");
  await A.waitForTimeout(2500);
  check((await hud(B)).health === hp0, "[mp] friendly fire off: no damage");
  // ... until everybody agrees (unanimous vote): the client proposes, votes yes; the host proposed it.
  check(await cmd(B, "mp ff on") === "proposed", "[mp] rule change proposed");
  check(await until(B, () => window.SpawnCity.hud().mp.votePending), "[mp] the ballot reaches the client");
  await cmd(B, "mp vote yes");
  check(await until(A, () => window.SpawnCity.hud().mp.ff) && await until(B, () => window.SpawnCity.hud().mp.ff), "[mp] friendly fire on for both");
  await cmd(A, "net hit 30");
  check(await until(B, h0 => window.SpawnCity.hud().health < h0, hp0), "[mp] the authority approved the hit, the victim applied it");
  check(await until(A, () => window.SpawnCity.hud().mp.hit < 3000), "[mp] hit marker for the shooter");
  // A second sniper round: down. Kill feed and score board everywhere.
  await cmd(A, "net hit 30");
  check(await until(A, () => window.SpawnCity.hud().mp.scores.some(s => s.self && s.k === 1)), "[mp] the kill counts for the host");
  check(await until(B, () => window.SpawnCity.hud().mp.feed.length > 0), "[mp] kill feed at the victim");
  check(await until(B, () => !window.SpawnCity.hud().mp.respawnIn && window.SpawnCity.hud().health > 0, null, 300000), "[mp] the victim respawned");
  // Effects: an explosion of the host is seen by the client.
  const booms = (await hud(B)).explosions;
  await cmd(A, "net blast");
  check(await until(B, b => window.SpawnCity.hud().explosions > b, booms), "[mp] remote explosion");
  // Ping marker in the world, seen by the mate.
  // At a point low on the screen (the ground in front; the crosshair may look at the sky).
  check(await cmd(A, "ping 320 330") === "set", "[mp] ping set");
  check(await until(B, () => window.SpawnCity.hud().mp.pings.length === 1), "[mp] the mate sees the ping");
  // Voice: no microphone headless - the client "speaks" a test tone; the host hears it (in range).
  await cmd(B, "voice mic");
  await cmd(B, "voice tone 4");
  check(await until(A, () => JSON.parse(window.SpawnCity.command("voice state")).speaking > 0, null, 60000), "[mp] proximity voice reaches the host");
  // Share panel with the join QR code, the big map with a waypoint and guidance.
  await A.evaluate(() => document.querySelector('#mpBar [data-a="share"]').click());
  check(await A.evaluate(() => window.__scShareQr?.size >= 21), "[mp] join QR code");
  await A.screenshot({path: path.join(shots, "mp-A-share.png"), timeout: 900000});
  await A.evaluate(() => document.getElementById("mpShare")?.remove());
  await cmd(A, "bigmap open");
  await cmd(A, "nav set 60 80 Treffpunkt");
  check(await until(A, () => window.SpawnCity.hud().mp.nav?.name === "Treffpunkt" && !!document.getElementById("mpMap")), "[mp] big map and destination");
  await A.waitForTimeout(800);
  await A.screenshot({path: path.join(shots, "mp-A-bigmap.png"), timeout: 900000});
  await cmd(A, "bigmap close");
  await B.screenshot({path: path.join(shots, "mp-B.png"), timeout: 900000});
  check(errors.length === 0, `[mp] no page errors ${errors.length ? JSON.stringify(errors.slice(0, 5)) : ""}`);
  await ctx.close();
}

try {
  const only = process.env.SC_SMOKE || "";  // "desktop", "phone" or "mp" runs one of them
  if (!only || only === "mp") await runMultiplayer();
  if (!only || only === "desktop") await run({name: "desktop", url: "?tilejson=https://tiles.test/planet&demtiles=https://dem.test/{z}/{x}/{y}.png", mapped: true, context: {viewport: {width: 1280, height: 720}}});
  if (!only || only === "phone") await run({name: "phone", url: "?offline=1&mute=1", mapped: false,
    context: {viewport: {width: 844, height: 390}, deviceScaleFactor: 2, isMobile: true, hasTouch: true}});
} finally {
  await browser.close();
  cleanup();
}
console.log(`${checks} checks, ${failures} failures`);
process.exit(failures ? 1 : 0);
