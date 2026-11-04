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
  await page.goto(`http://127.0.0.1:${port}/kern/games/spawncity/web/index.html${url}`);
  await page.waitForFunction(() => window.SpawnCity?.hud()?.placed, null, {timeout: 300000});
  console.log(`[${name}] world placed after ${((Date.now() - t0) / 1000).toFixed(1)} s`);
  const hud = () => page.evaluate(() => window.SpawnCity.hud());
  const cmd = c => page.evaluate(c => window.SpawnCity.command(c), c);
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
  check(h.tiles.buildings > 50 && h.tiles.colliders > 50, `[${name}] buildings with collision (${h.tiles.buildings} / ${h.tiles.colliders})`);
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

  // The town: people on the pavements; a wanted level brings a police unit with its crew.
  h = await hud();
  check(h.people >= 10, `[${name}] people in the streets: ${h.people}`);
  check(await cmd("wanted 2") === "2", `[${name}] wanted level 2`);
  const tw = (await hud()).t;
  await page.waitForFunction(() => { const h = window.SpawnCity.hud(); return h.police > 0 && h.policeDrones > 0; }, null, {timeout: 300000}).catch(() => {});
  h = await hud();
  check(h.police > 0 && h.policeDrones > 0 && h.stars > 0, `[${name}] police on its way (${h.police} cruisers, ${h.policeDrones} drones, ${h.stars} stars)`);
  await page.waitForFunction(t => window.SpawnCity.hud().t > t + 6, tw, {timeout: 400000}).catch(() => {});
  await shot("1b-police");
  check(await cmd("wanted 0") === "0", `[${name}] wanted level cleared`);
  // Weapons: the rocket launcher fires a rocket that explodes; the minimap has streets.
  check(await cmd("weapon 4") === "Raketenwerfer", `[${name}] rocket launcher in hand`);
  const booms = (await hud()).explosions;
  check((await cmd("attack")).startsWith("1 "), `[${name}] a rocket is fired`);
  await page.waitForFunction(b => window.SpawnCity.hud().explosions > b, booms, {timeout: 300000}).catch(() => {});
  check((await hud()).explosions > booms, `[${name}] the rocket explodes`);
  await cmd("weapon 1");
  const map = JSON.parse(await cmd("map"));
  check(map.lanes && map.lanes.length > 0, `[${name}] minimap streets (${map.lanes?.length})`);

  // Drone: wait for the firmware's calibration, then climb and fly forward for 3 s.
  check(await cmd("mode drone") === "drone", `[${name}] drone mode`);
  await page.waitForFunction(() => { const d = window.SpawnCity.hud()?.drone; return d && !d.calibrating; }, null, {timeout: 240000});
  if (context.hasTouch) await touchHold(vw - 110, vh - 110);
  else { await hold(["Space"], 2); await hold(["KeyW", "Space"], 1); }
  h = await hud();
  check(h.drone && h.drone.armed, `[${name}] drone armed (state ${JSON.stringify(h.drone)})`);
  check(h.agl > 1.0, `[${name}] drone climbed: ${h.agl} m AGL`);
  await shot("2-drone");
  check(await cmd("cam") === "third" && await cmd("cam") === "fpv", `[${name}] drone cameras follow -> third -> FPV`);
  await shot("2b-drone-fpv");
  page.evaluate(() => window.SpawnCity.command("mode foot"));

  // Car: carjack the nearest car, drive 3 s.
  const car = await cmd("mode car");
  check(car === "car", `[${name}] car mode (${car})`);
  if (car === "car") {
    if (context.hasTouch) await touchHold(vw - 110, vh - 110);
    else await hold(["KeyW"]);
    h = await hud();
    check(h.speed > 1.0, `[${name}] the car drives: ${(h.speed * 3.6).toFixed(0)} km/h`);
    await shot("3-car");
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
    check(h.agl > 2, `[${name}] the jet lifts off: ${h.agl} m AGL`);
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

try {
  const only = process.env.SC_SMOKE || "";  // "desktop" or "phone" runs one of the two
  if (!only || only === "desktop") await run({name: "desktop", url: "?tilejson=https://tiles.test/planet&demtiles=https://dem.test/{z}/{x}/{y}.png", mapped: true, context: {viewport: {width: 1280, height: 720}}});
  if (!only || only === "phone") await run({name: "phone", url: "?offline=1&mute=1", mapped: false,
    context: {viewport: {width: 844, height: 390}, deviceScaleFactor: 2, isMobile: true, hasTouch: true}});
} finally {
  await browser.close();
  cleanup();
}
console.log(`${checks} checks, ${failures} failures`);
process.exit(failures ? 1 : 0);
