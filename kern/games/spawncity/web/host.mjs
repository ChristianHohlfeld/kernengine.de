// SpawnCity on Kern - the page host. Everything the game decides runs in C++ (spawncity.wasm);
// this module only does what the browser forces into JavaScript:
//   * runs the Kern App host (kern/web/app.mjs: canvas, WebGL2, frame loop, keyboard / mouse)
//   * fetches the map tiles the game asks for (OpenFreeMap vector tiles, (c) OpenStreetMap
//     contributors) and hands the bytes over; no map service -> the game builds its stand-in town
//   * fetches the elevation tiles it asks for (Terrarium PNGs, AWS Open Data Terrain Tiles) and
//     hands over the decoded RGBA pixels (the browser decodes PNGs; the heights are the game's)
//   * polls the gamepad and routes touch fingers (CSS px) to the game
//   * plays the original's sound (audio.mjs: spawncity.de's music, audio modules and sound
//     functions, fed from the game's audio state and events); Kern's mixer (KernAudio from
//     kern/web/studio/game.mjs) only carries the other players' voices then (?audio=kern: Kern's own)
//   * draws the HUD from the game's JSON and the touch controls with Kern's TouchOverlay
//   * multiplayer (?mp=host|join&room=NAME[&relay=wss://...]): moves the game's datagrams with
//     Kern's KernNet (BroadcastChannel between tabs, or the Kern relay between machines)
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
import {runKernApp} from "../../../web/app.mjs";
import {KernAudio, KernNet, TouchOverlay, TOUCH_STYLE} from "../../../web/studio/game.mjs";
import {startKernJobs} from "../../../web/jobs.mjs";
import {MpPage} from "./mp_host.mjs";
import {installSpawnCityUi} from "./ui.mjs";
import {GapsHud} from "./gaps_hud.mjs";
import {installSpawnCityLife} from "./life.mjs";
import {mountOriginalView, OriginalView, fingerForGame} from "./view.mjs";
// the mode binders register themselves (view_modes.mjs)
import {VIEW_MODES} from "./view_modes.mjs";
import {installOriginalMenus} from "./view_menu.mjs";
import {installOriginalAudio, exemptVoice} from "./audio.mjs";

const SC_TILEJSON = "https://tiles.openfreemap.org/planet";
// Elevation: Terrain Tiles (Terrarium) on AWS Open Data, as on spawncity.de (terrain_elevation.mjs).
const SC_DEM = "https://s3.amazonaws.com/elevation-tiles-prod/terrarium/{z}/{x}/{y}.png";
const SC_MAX_FETCHES = 4;

/** Starts the game in the page (index.html). Module: url (dev) or base64 (single file). */
export async function startSpawnCity({url = null, base64 = null} = {}) {
  const params = new URLSearchParams(location.search);
  // The original's view layer (view.mjs): its DOM, CSS and layout; Kern's canvas takes the place of
  // the original's three.js canvas.
  // The original spawncity.de view is the default; ?view=kern shows Kern's own HUD (tests, debugging).
  const originalView = params.get("view") !== "kern" && await mountOriginalView();
  // The original's sound (audio.mjs): its music, its audio modules, its sound functions - the title
  // music starts now, before the game has loaded, as on spawncity.de.
  const originalAudio = originalView && params.get("mute") !== "1" && params.get("audio") !== "kern" ? installOriginalAudio() : null;
  const canvas = document.getElementById("game");
  const statusEl = document.getElementById("status");
  const hudEl = document.getElementById("hud");
  const coarse = matchMedia("(pointer: coarse)").matches;
  const mobile = params.has("mobile") ? params.get("mobile") === "1" : coarse && Math.min(screen.width, screen.height) < 900;
  let args = location.search || "?";
  if (!params.has("mobile")) args += (args.length > 1 ? "&" : "") + "mobile=" + (mobile ? 1 : 0);

  const style = document.createElement("style");
  style.textContent = TOUCH_STYLE;
  document.head.appendChild(style);

  let app;
  try {
    app = await runKernApp({canvas, url, base64, args, maxPixelRatio: mobile ? 1.5 : 2,
      onStatus: text => { statusEl.textContent = text; statusEl.hidden = !text; }});
  } catch (error) {
    statusEl.hidden = false;
    statusEl.textContent = "SpawnCity konnte nicht starten: " + (error?.message || error);
    window.__scError = String(error);
    throw error;
  }
  const kern = app.kern, e = kern.exports;
  // Kern jobs (kern/web/jobs.mjs): the city's tile decoding and building extrusion run in Web
  // Workers on the same module, like MapLibre's tile workers for the original.
  let jobs = null;
  (async () => {
    try {
      let mod = null;
      if (url) mod = await WebAssembly.compileStreaming(fetch(url, {cache: "no-cache"})).catch(async () => WebAssembly.compile(await (await fetch(url, {cache: "no-cache"})).arrayBuffer()));
      else if (base64) { const bin = atob(base64), b = new Uint8Array(bin.length); for (let i = 0; i < bin.length; i++) b[i] = bin.charCodeAt(i); mod = await WebAssembly.compile(b); }
      if (mod) jobs = await startKernJobs(kern, mod, {kernSource: globalThis.__kernSource || null});
    } catch (error) { console.warn("[spawncity] no job workers, the city builds in the frame:", error?.message || error); }
  })();
  // Fresh views each time: the module's memory may have grown (and detached older views).
  const decoder = new TextDecoder();
  const str = ptr => decoder.decode(new Uint8Array(kern.memory.buffer, ptr, e.sc_result_length()));
  const command = text => {
    const bytes = new TextEncoder().encode(text), ptr = e.kern_alloc(bytes.length + 1);
    new Uint8Array(kern.memory.buffer, ptr, bytes.length).set(bytes);
    const out = str(e.sc_command(ptr, bytes.length));
    e.kern_free(ptr);
    return out;
  };

  // ---- map tiles ------------------------------------------------------------------------
  const tiles = {template: params.get("tiles") || null, queue: [], active: 0, failed: false, done: 0};
  const offline = params.get("offline") === "1";
  async function fetchWithTimeout(u, ms) {
    const ctl = new AbortController(), t = setTimeout(() => ctl.abort(), ms);
    try { return await fetch(u, {signal: ctl.signal}); } finally { clearTimeout(t); }
  }
  async function gunzip(bytes) {
    if (typeof DecompressionStream !== "function") throw Error("gzip tile and no DecompressionStream");
    const stream = new Blob([bytes]).stream().pipeThrough(new DecompressionStream("gzip"));
    return new Uint8Array(await new Response(stream).arrayBuffer());
  }
  async function loadTemplate() {
    if (tiles.template) return;
    const res = await fetchWithTimeout(params.get("tilejson") || SC_TILEJSON, 8000);
    if (!res.ok) throw Error("TileJSON " + res.status);
    const json = await res.json();
    if (!json?.tiles?.[0]) throw Error("TileJSON without tiles");
    tiles.template = json.tiles[0];
  }
  async function fetchTile([z, x, y]) {
    tiles.active++;
    try {
      const u = tiles.template.replace("{z}", z).replace("{x}", x).replace("{y}", y);
      const res = await fetchWithTimeout(u, 15000);
      let bytes;
      if (res.status === 204 || res.status === 404) bytes = new Uint8Array(0);  // no data there (sea, outside the planet)
      else if (!res.ok) throw Error("tile " + res.status);
      else bytes = new Uint8Array(await res.arrayBuffer());
      if (bytes.length > 2 && bytes[0] === 0x1f && bytes[1] === 0x8b) bytes = await gunzip(bytes);
      const ptr = e.kern_alloc(Math.max(1, bytes.length));
      new Uint8Array(kern.memory.buffer, ptr, bytes.length).set(bytes);
      e.sc_tile_data(z, x, y, ptr, bytes.length);
      e.kern_free(ptr);
      tiles.done++;
    } catch (error) {
      console.warn("[spawncity] tile", z, x, y, error?.message || error);
      e.sc_tile_failed(z, x, y);
    } finally {
      tiles.active--;
    }
  }
  function pumpTiles() {
    const list = JSON.parse(str(e.sc_tile_requests()));
    if (list.length) tiles.queue.push(...list);
    if (!tiles.template || tiles.failed) return;
    while (tiles.active < SC_MAX_FETCHES && tiles.queue.length) fetchTile(tiles.queue.shift());
  }
  // ---- elevation tiles -------------------------------------------------------------------------
  const dem = {template: params.get("demtiles") || SC_DEM, queue: [], active: 0, done: 0};
  async function fetchDem([z, x, y]) {
    dem.active++;
    try {
      const res = await fetchWithTimeout(dem.template.replace("{z}", z).replace("{x}", x).replace("{y}", y), 12000);
      if (!res.ok) throw Error("dem " + res.status);
      const bitmap = await createImageBitmap(await res.blob(), {colorSpaceConversion: "none", premultiplyAlpha: "none"});
      const c = new OffscreenCanvas(bitmap.width, bitmap.height), g = c.getContext("2d", {willReadFrequently: true});
      g.drawImage(bitmap, 0, 0);
      const px = g.getImageData(0, 0, bitmap.width, bitmap.height).data;
      const ptr = e.kern_alloc(px.length);
      new Uint8Array(kern.memory.buffer, ptr, px.length).set(px);
      e.sc_dem_data(z, x, y, ptr, bitmap.width, bitmap.height);
      e.kern_free(ptr);
      dem.done++;
    } catch (error) {
      console.warn("[spawncity] elevation tile", z, x, y, error?.message || error);
      e.sc_dem_failed(z, x, y);
    } finally {
      dem.active--;
    }
  }
  function pumpDem() {
    const list = JSON.parse(str(e.sc_dem_requests()));
    if (list.length) dem.queue.push(...list);
    while (dem.active < SC_MAX_FETCHES && dem.queue.length) fetchDem(dem.queue.shift());
  }
  if (offline) e.sc_offline();
  else loadTemplate().catch(error => {
    console.warn("[spawncity] no map service, building the stand-in town:", error?.message || error);
    tiles.failed = true;
    e.sc_offline();
  });

  // ---- touch frame and fingers --------------------------------------------------------------
  const probe = document.createElement("div");
  probe.style.cssText = "position:fixed;left:0;top:0;visibility:hidden;pointer-events:none;padding:env(safe-area-inset-top,0px) env(safe-area-inset-right,0px) env(safe-area-inset-bottom,0px) env(safe-area-inset-left,0px)";
  document.body.appendChild(probe);
  // Portrait phones: the CSS-rotated landscape (index.html .sc-css-landscape); fingers map back.
  const rotated = () => document.documentElement.classList.contains("sc-css-landscape");
  const orient = () => {
    const turn = (coarse || params.get("rotate") === "1") && matchMedia("(orientation: portrait)").matches && params.get("rotate") !== "0";
    document.documentElement.classList.toggle("sc-css-landscape", turn);
  };
  orient();
  matchMedia("(orientation: portrait)").addEventListener?.("change", () => { orient(); frame(); });
  visualViewport?.addEventListener("resize", () => { orient(); frame(); });
  const frame = () => {
    const c = getComputedStyle(probe), px = v => parseFloat(v) || 0;
    e.sc_touch_frame(canvas.clientWidth, canvas.clientHeight, px(c.paddingLeft), px(c.paddingTop), px(c.paddingRight), px(c.paddingBottom),
      Math.min(globalThis.devicePixelRatio || 1, mobile ? 1.5 : 2));
  };
  frame();
  addEventListener("resize", frame);
  addEventListener("orientationchange", frame);
  const local = ev => {
    if (rotated()) return [ev.clientY, innerWidth - ev.clientX];  // control_semantics.mjs quarter turn
    const r = canvas.getBoundingClientRect();
    return [ev.clientX - r.left, ev.clientY - r.top];
  };
  const finger = type => ev => {
    if (ev.pointerType === "mouse") return;
    for (const c of (type === 1 && ev.getCoalescedEvents?.()) || [ev]) {
      const [x, y] = local(c);
      const used = e.sc_touch(type, c.pointerId, x, y, c.timeStamp);
      mpPage?.touch(type, c.pointerId, x, y, used);  // a long press on free ground pings
    }
    ev.preventDefault();
  };
  if (originalView) {
    // The original's buttons lie over the canvas: a finger on them is still the game's (the game
    // knows their rectangles, view.mjs), except on the page's own buttons (menu, settings, map).
    const vp = document.getElementById("viewport"), owned = new Set();
    const route = type => {
      const f = finger(type);
      return ev => {
        if (ev.pointerType === "mouse") return;
        if (type === 0) { if (!fingerForGame(ev)) return; owned.add(ev.pointerId); }
        else if (!owned.has(ev.pointerId)) return;
        if (type >= 2) owned.delete(ev.pointerId);
        f(ev);
      };
    };
    vp.addEventListener("pointerdown", route(0), {capture: true});
    addEventListener("pointermove", route(1), {capture: true});
    addEventListener("pointerup", route(2), {capture: true});
    addEventListener("pointercancel", route(3), {capture: true});
  } else {
    canvas.addEventListener("pointerdown", finger(0));
    canvas.addEventListener("pointermove", finger(1));
    canvas.addEventListener("pointerup", finger(2));
    canvas.addEventListener("pointercancel", finger(3));
  }
  // foot_touch_controller.mjs: every finger is let go when the page hides, on pagehide, and when a
  // touchend / touchcancel leaves no touch on the glass (the ground truth); blur keeps them.
  const releaseFingers = () => command("touch release");
  document.addEventListener("visibilitychange", () => { if (document.hidden) releaseFingers(); });
  addEventListener("pagehide", releaseFingers);
  for (const t of ["touchend", "touchcancel"]) addEventListener(t, ev => { if (ev.touches.length === 0) releaseFingers(); }, {capture: true, passive: true});
  const overlay = new TouchOverlay(document.getElementById("stage"));

  // ---- devices and the mouse (desktop_controls.mjs; the game decides, game_controls.cpp) ------------
  // The platform: a fine hovering primary pointer or no touch = desktop; ?coarse= forces it (tests).
  if (!params.has("coarse"))
    command(`device platform ${matchMedia("(hover: hover) and (pointer: fine)").matches ? 1 : 0} ${coarse ? 1 : 0} ${navigator.maxTouchPoints || 0}`);
  const UI_SELECTOR = "button,input,select,textarea,a,label,dialog,[role=button],.sc-map";
  addEventListener("pointerdown", ev => command(ev.pointerType === "mouse" ? "device mouse" : "device touch"), {capture: true});
  let mouseSeen = 0;
  addEventListener("pointermove", ev => {
    if (ev.pointerType !== "mouse" || document.pointerLockElement || !(ev.movementX || ev.movementY)) return;
    if (ev.target instanceof Element && ev.target.closest(UI_SELECTOR)) return;
    const t = performance.now();
    if (t - mouseSeen > 200) { mouseSeen = t; command("device mousemove"); }
  }, {capture: true});
  // One movement stream: mousemove under the lock (precision touchpads keep sending it while
  // pointermove stalls), each event clamped to +-420 px (MAX_EVENT_PX), never dropped.
  addEventListener("mousemove", ev => {
    if (document.pointerLockElement !== canvas) return;
    const c = v => Math.max(-420, Math.min(420, Number(v) || 0));
    e.sc_mouse(c(ev.movementX), c(ev.movementY));
  }, {capture: true});
  document.addEventListener("pointerlockchange", () => e.sc_pointer_lock(document.pointerLockElement === canvas ? 1 : 0));
  // Keys Kern's input does not carry, and page buttons: + / - car zoom, P multiplayer, L GPS LIVE
  addEventListener("keydown", ev => {
    if (ev.repeat || ev.metaKey || ev.ctrlKey || ev.altKey) return;
    if (ev.target instanceof Element && ev.target.closest("input,textarea,select,[contenteditable]")) return;
    if (ui?.playing === false) return;
    if (ev.code === "Equal" || ev.code === "NumpadAdd") command("zoom in");
    else if (ev.code === "Minus" || ev.code === "NumpadSubtract") command("zoom out");
    else if (ev.code === "KeyP" && !last?.ctl?.dead) { ev.preventDefault(); mpPage?.goMulti(); }  // pad_actions.mjs
    else if (ev.code === "KeyL") ui?.open("gps");  // gps_live_mode.mjs
    else if (ev.code === "NumpadEnter" && last?.ctl?.aiming) command("strike go");
  });

  // ---- gamepad (Xbox standard mapping) ------------------------------------------------------
  function pollPad() {
    const pads = navigator.getGamepads ? Array.from(navigator.getGamepads()).filter(p => p && p.connected) : [];
    const p = pads.find(g => g.mapping === "standard") || pads[0];
    if (!p) { e.sc_gamepad(0, 0, 0, 0, 0, 0, 0, 0, 0); return; }
    let bits = 0;
    for (let i = 0; i < 16; i++) if (p.buttons[i]?.pressed) bits |= 1 << i;
    const ax = i => p.axes[i] ?? 0, v = i => p.buttons[i]?.value ?? 0;
    const active = bits !== 0 || p.axes.slice(0, 4).some(a => Math.abs(a) > 0.22);
    e.sc_gamepad(1, ax(0), -ax(1), ax(2), -ax(3), v(6), v(7), bits >>> 0, active ? 1 : 0);
  }

  // ---- audio --------------------------------------------------------------------------------
  let audio = null;
  if (KernAudio.supported() && params.get("mute") !== "1") {
    audio = new KernAudio(frames => {
      const ptr = e.sc_audio_render(frames);
      return new Float32Array(kern.memory.buffer, ptr, frames * 2).slice();
    });
    const rate = audio.create();
    if (rate && e.sc_audio_init(rate)) setInterval(() => audio.pump(), 20);
    else audio = null;
  }
  // A hidden page is silent (music_player.mjs wanted(): !document.hidden; the original's effects
  // stop with the frame loop): minimised on a phone, no birds, no engine in the background.
  document.addEventListener("visibilitychange", () => {
    const c = audio?.context;
    if (!c) return;
    if (document.hidden) c.suspend?.().catch(() => {});
    else if (c.state !== "running") c.resume?.().catch(() => {});
  });
  addEventListener("pagehide", () => audio?.context?.suspend?.().catch(() => {}));
  originalAudio?.attach(command);
  let voiceExempt = !originalAudio;
  // With the original's sound every game sound is a Web Audio node scheduled on the audio thread
  // (audio.mjs): a long frame only delays a parameter change. Kern's own output stream (rendered in
  // wasm on the main thread, a short queue that underruns on every long frame) is then not used for
  // the game at all - it is only connected for the other players' voices in a multiplayer round.
  let netActive = false;
  if (originalAudio && audio) {
    const kernConnect = audio.connect.bind(audio);
    audio.connect = async () => { if (netActive) return kernConnect(); };
  }

  // ---- multiplayer, voice, pings, big map (web/mp_host.mjs) --------------------------------
  const mpPage = new MpPage({e, kern, command, params, canvas, statusEl});
  mpPage.connect(KernNet);

  // ---- HUD ----------------------------------------------------------------------------------
  const hud = new SpawnCityHud(hudEl);
  // Menus, settings, start place, equipment (ui.mjs): DOM only, the game decides.
  const ui = installSpawnCityUi({command, params, canvas, statusEl});
  // Street life (life.mjs): speech, speech bubbles, banners - the game decides, the page speaks.
  const life = installSpawnCityLife({root: hudEl, muted: params.get("mute") === "1"});
  const view = originalView ? new OriginalView({command, ui, mp: mpPage}) : null;
  if (originalView) installOriginalMenus({command, ui, mp: mpPage});
  let frames = 0, last = null;
  const vib = {me: null, peers: new Map()};
  // Drawing-buffer density from the game's quality level (perf.h render_scale, game_look.cpp): the
  // browser owns the canvas size, so the page applies it; app.mjs re-sizes on layout changes and the
  // next frame applies the scale again.
  // render_stability.mjs renderPlatformProfile: the pixel-ratio ceiling of the original -
  // software rasterizer 0.30 (?hq=1: 1), Android 1.15, iPhone / iPad 1.5, desktop 1.5
  // (PRESENTATION / WORLD_FLIGHT_PIXEL_RATIO 1.5); software also without shadows (simulator.mjs).
  const platform = (() => {
    let name = "";
    try { const gl = document.createElement("canvas").getContext("webgl2") || document.createElement("canvas").getContext("webgl"); const ext = gl?.getExtension("WEBGL_debug_renderer_info"); name = String(gl?.getParameter(ext?.UNMASKED_RENDERER_WEBGL || gl.RENDERER) || ""); gl?.getExtension("WEBGL_lose_context")?.loseContext(); } catch { /* none */ }
    const ua = navigator.userAgent, dpr = Math.max(.25, Number(globalThis.devicePixelRatio) || 1);
    const software = /(swiftshader|llvmpipe|software raster|software renderer)/i.test(name), android = /android/i.test(ua);
    const apple = /(?:iphone|ipad|ipod)|(?:macintosh.*mobile)/i.test(ua) || (/macintosh/i.test(ua) && navigator.maxTouchPoints > 1);
    const ceiling = software ? Math.min(dpr, params.get("hq") === "1" ? 1 : 0.30) : android ? Math.min(dpr, 1.15) : apple ? Math.min(dpr, 1.5) : Math.min(dpr, 1.5);
    return {software, ceiling: Math.min(ceiling, 1.5), name};
  })();
  if (platform.software) command("render software");
  const baseRatio = () => platform.ceiling;
  const applyRenderScale = h => {
    let s = h?.renderScale;
    if (!(s > 0) || !canvas.clientWidth) return;
    // the title covers the game view (the original draws only its 2D flyover there): no 3D pixels
    if (originalView && !ui.playing) s = 2 / Math.max(canvas.clientWidth, 1);
    const r = baseRatio() * s, w = Math.max(1, Math.round(canvas.clientWidth * r)), hh = Math.max(1, Math.round(canvas.clientHeight * r));
    if (canvas.width === w && canvas.height === hh) return;
    canvas.width = w; canvas.height = hh;
    e.kern_app_resize(w, hh);
  };
  const gapsHud = new GapsHud(hudEl, command);  // impact bursts, police dispatch (gaps_hud.mjs)
  // ?perf=1: where a frame's time goes, on the player's own device (screenshot it). Frame time from
  // rAF (p50 / p95 / max of the last 2 s, frames over 50 ms), the game's own CPU slots, the GPU
  // time of the frame (timer query, when the browser has it), the city's slowest streaming step,
  // the render scale and the drawing-buffer size.
  const perfView = params.get("perf") === "1" ? (() => {
    const el = document.createElement("div");
    el.style.cssText = "position:fixed;left:6px;bottom:6px;z-index:2147483647;max-width:96vw;padding:5px 7px;border-radius:6px;background:rgba(0,0,0,.72);color:#9ff;font:600 10px/1.35 ui-monospace,monospace;white-space:pre;pointer-events:none";
    document.body.appendChild(el);
    command("perf 1");
    const ft = [];
    let prev = 0, shownAt = 0, longs = 0, memBytes = kern.memory.buffer.byteLength, grows = 0, growFrames = 0, growMax = 0;
    return now => {
      // wasm memory growth: how often, and how long the frames were in which it grew
      const mb = kern.memory.buffer.byteLength;
      const grew = mb !== memBytes;
      if (grew) { grows++; memBytes = mb; }
      if (prev) { const d = now - prev; ft.push([now, d]); if (d > 50) longs++; if (grew) { growFrames++; growMax = Math.max(growMax, d); } }
      prev = now;
      while (ft.length && now - ft[0][0] > 2000) ft.shift();
      if (now - shownAt < 250 || !last) return;
      shownAt = now;
      const s = ft.map(f => f[1]).sort((a, b) => a - b), q = x => s.length ? s[Math.min(s.length - 1, Math.floor(x * s.length))].toFixed(1) : "-";
      const p = last.perf || {}, m = p.ms || {}, t = last.tiles?.t || {};
      el.textContent = `frame ms p50 ${q(.5)} p95 ${q(.95)} max ${q(1)} · >50ms ${longs} · ${p.fps?.toFixed?.(0) ?? "-"} fps · tier ${p.tierName || "-"} · scale ${last.renderScale ?? "-"} · ${canvas.width}x${canvas.height}\n`
        + `cpu logic ${m.logic?.toFixed(1)} physics ${m.physics?.toFixed(1)} stream ${m.stream?.toFixed(1)} draw ${m.draw?.toFixed(1)} render ${m.render?.toFixed(1)} · gpu ${m.gpu >= 0 ? m.gpu.toFixed(1) : "n/a"}\n`
        + `wasm memory ${(memBytes / 1048576).toFixed(0)} MB · grew ${grows}x · longest frame with a grow ${growMax.toFixed(0)} ms\n`
        + `stream worst ${t.worst ?? "-"} ms (${t.worstStep || "-"} ${t.worstStepMs ?? ""}) · city re-syncs ${t.rebuilds ?? "-"} · street layings ${t.relays ?? "-"} · uploads ${t.uploads ?? "-"} (${t.uploadKVerts ?? "-"}k verts) · mode ${last.mode}`;
    };
  })() : null;
  const tick = () => {
    frames++;
    perfView?.(performance.now());
    pollPad();
    pumpTiles();
    pumpDem();
    jobs?.pump();
    audio?.pump();
    try { last = JSON.parse(str(e.sc_hud())); } catch { /* keep the last one */ }
    if (last) { hud.update(last, tiles); mpPage?.update(last); gapsHud.update(last); }
    // vs_combat_presentation.mjs playExplosion: in a multiplayer round the phone buzzes when you
    // go down ([70, 35, 120]) and when a mate does ([65]); the pad never rumbles in SpawnCity.
    if (last?.net) {
      const me = !!last.ctl?.dead;
      if (me && vib.me === false) navigator.vibrate?.([70, 35, 120]);
      vib.me = me;
      for (const p of last.mp?.players || []) {
        if (p.dead && vib.peers.get(p.slot) === false) navigator.vibrate?.([65]);
        vib.peers.set(p.slot, !!p.dead);
      }
    }
    applyRenderScale(last);
    ui.update(last);
    originalAudio?.update(last, ui);
    netActive = !!last?.net;
    if (originalAudio && audio && netActive && !audio.node) audio.connect();
    if (!voiceExempt && audio) voiceExempt = exemptVoice(audio);
    life.update(last);
    const touchView = JSON.parse(str(e.sc_touch_view()));
    if (view) view.update(last, touchView);
    else overlay.update(touchView);
    requestAnimationFrame(tick);
  };

  // ---- minimap (the game sends streets, police and players around the player, 4 Hz) -------
  const mapCanvas = (originalView && document.querySelector("#worldLookHud .world-mini-canvas")) || hudEl.querySelector(".sc-map"), mctx = mapCanvas?.getContext("2d");
  // The minimap (real_world_bootstrap.mjs): a drag turns the head (0.85 deg/px, -0.62 deg/px,
  // -75..60), letting go springs back; a double tap within 360 ms opens the big map.
  if (mapCanvas) {
    mapCanvas.style.pointerEvents = "auto";
    mapCanvas.style.touchAction = "none";
    let drag = null, lastTap = 0;
    mapCanvas.addEventListener("pointerdown", ev => {
      ev.preventDefault(); ev.stopPropagation();
      const look = last?.ctl?.look || [0, 0];
      drag = {id: ev.pointerId, x: ev.clientX, y: ev.clientY, yaw: look[0], pitch: look[1], moved: false};
      mapCanvas.setPointerCapture?.(ev.pointerId);
    });
    mapCanvas.addEventListener("pointermove", ev => {
      if (!drag || ev.pointerId !== drag.id) return;
      const dx = ev.clientX - drag.x, dy = ev.clientY - drag.y;
      if (Math.hypot(dx, dy) >= 8) drag.moved = true;
      if (drag.moved) command(`look set ${(drag.yaw + dx * 0.85).toFixed(2)} ${(drag.pitch - dy * 0.62).toFixed(2)}`);
    });
    const up = ev => {
      if (!drag || ev.pointerId !== drag.id) return;
      if (!drag.moved) { const t = performance.now(); if (t - lastTap < 360) { lastTap = 0; command("bigmap toggle"); } else lastTap = t; }
      command("look release");
      drag = null;
    };
    mapCanvas.addEventListener("pointerup", up);
    mapCanvas.addEventListener("pointercancel", up);
  }
  setInterval(() => {
    if (!mctx || originalView) return;  // the original view draws its own (view.mjs minimap)
    let m;
    try { m = JSON.parse(command("map")); } catch { return; }
    const W = mapCanvas.width, H = mapCanvas.height, R = Math.min(W, H) / 2, scale = R / 150;  // 150 m to the rim
    mctx.clearRect(0, 0, W, H);
    if (!m.me) return;
    mctx.save();
    if (!originalView) { mctx.beginPath(); mctx.arc(R, R, R - 2, 0, Math.PI * 2); mctx.clip(); }
    // Heading up: rotate the world so the view direction points to the top.
    mctx.translate(W / 2, H / 2);
    const northUp = !!last?.ui?.northUp;  // settings: minimap north-up (settings_help.mjs worldMinimapAxisLock)
    if (!northUp) mctx.rotate(m.me[2] - Math.PI / 2);
    mctx.scale(scale, -scale);
    mctx.strokeStyle = "rgba(220, 226, 235, .75)"; mctx.lineWidth = 5 / scale * 0.9; mctx.lineCap = "round";
    for (const l of m.lanes) { mctx.beginPath(); for (let i = 0; i < l.length; i += 2) (i ? mctx.lineTo : mctx.moveTo).call(mctx, l[i], l[i + 1]); mctx.stroke(); }
    const dot = (p, color, r) => { mctx.fillStyle = color; mctx.beginPath(); mctx.arc(p[0], p[1], r / scale, 0, Math.PI * 2); mctx.fill(); };
    if (m.jet) dot(m.jet, "#7fd3ff", 5);
    for (const p of m.players) dot(p, "#3ee07a", 6);
    for (const p of m.police) dot(p, Math.floor(performance.now() / 250) % 2 ? "#ff4040" : "#4c7dff", 6);
    mctx.restore();
    // The player in the middle: an arrow pointing up.
    mctx.save(); mctx.translate(W / 2, H / 2); if (northUp) mctx.rotate(Math.PI / 2 - m.me[2]);
    mctx.fillStyle = "#ffd23e"; mctx.beginPath(); mctx.moveTo(0, -12); mctx.lineTo(8, 9); mctx.lineTo(0, 4); mctx.lineTo(-8, 9); mctx.closePath(); mctx.fill(); mctx.restore();
  }, 250);
  requestAnimationFrame(tick);
  // hud() reads the game's state now (not the copy of the last drawn frame): a test that sends a
  // command sees its effect at once, however slow the frames are.
  const hudNow = () => { try { return JSON.parse(str(e.sc_hud())); } catch { return last; } };
  window.SpawnCity = {app, command, hud: hudNow, platform, frames: () => frames, tiles, ui, audio: originalAudio};
  return window.SpawnCity;
}

const SC_MODE_LABEL = {foot: "ZU FUSS", drone: "DROHNE", car: "AUTO", jet: "JET", parachute: "FALLSCHIRM"};

/** The HUD, drawn from the game's JSON (sc_hud): numbers only, no game logic. */
class SpawnCityHud {
  constructor(root) {
    this.root = root;
    this.root.innerHTML = `
      <div class="sc-top"><span class="sc-badge"></span><span class="sc-stars"></span><span class="sc-fps"></span></div>
      <canvas class="sc-map" width="300" height="300"></canvas>
      <div class="sc-toast" hidden></div>
      <div class="sc-offline" hidden>OFFLINE · prozedurale Ersatzstadt (keine echten Kartendaten)</div>
      <div class="sc-cross" hidden></div>
      <div class="sc-panel"><div class="sc-speed"><b>0</b><small>km/h</small></div><div class="sc-rows"></div></div>
      <div class="sc-health"><i></i></div>
      <div class="sc-ammo" hidden></div>
      <div class="sc-hint" hidden></div>
      <div class="sc-banner" hidden></div><div class="sc-pop" hidden></div><div class="sc-scope" hidden><i></i></div><div class="sc-guided" hidden></div>
      <div class="sc-help"></div>
      <div class="sc-attrib"><a href="https://www.openstreetmap.org/copyright" target="_blank" rel="noopener">© OpenStreetMap-Mitwirkende</a> ·
        <a href="https://openfreemap.org" target="_blank" rel="noopener">OpenFreeMap</a> ·
        <a href="https://openmaptiles.org" target="_blank" rel="noopener">© OpenMapTiles</a> ·
        <a href="https://registry.opendata.aws/terrain-tiles/" target="_blank" rel="noopener">Höhen: Terrain Tiles (Mapzen, AWS Open Data)</a> · Kern Engine</div>`;
    const q = s => this.root.querySelector(s);
    this.el = {badge: q(".sc-badge"), stars: q(".sc-stars"), fps: q(".sc-fps"), toast: q(".sc-toast"), offline: q(".sc-offline"), cross: q(".sc-cross"),
      speed: q(".sc-speed b"), rows: q(".sc-rows"), health: q(".sc-health"), healthBar: q(".sc-health i"), ammo: q(".sc-ammo"), hint: q(".sc-hint"), banner: q(".sc-banner"), pop: q(".sc-pop"), scope: q(".sc-scope"), guided: q(".sc-guided"),
      help: q(".sc-help"), panel: q(".sc-panel")};
    this.el.perf = Object.assign(document.createElement("div"), {className: "sc-perf", hidden: true});
    this.el.perf.style.cssText = "position:absolute;left:max(14px,env(safe-area-inset-left));top:calc(max(12px,env(safe-area-inset-top)) + 30px);font:600 11px ui-monospace,monospace;color:#cfe3f5;background:rgba(0,0,0,.45);padding:3px 7px;border-radius:5px;white-space:nowrap";
    this.root.appendChild(this.el.perf);
    this.key = "";
  }
  update(h, tiles) {
    const el = this.el;
    document.body.dataset.mode = h.mode;
    document.body.classList.toggle("sc-touch-on", !!h.touch);
    el.badge.textContent = SC_MODE_LABEL[h.mode] || h.mode;
    el.stars.textContent = "★".repeat(h.stars || 0);
    el.fps.textContent = `${h.fps} fps · ${h.tiles.ready} Kacheln${h.tiles.loading ? ` (+${h.tiles.loading})` : ""} · ${h.tiles.buildings} Gebäude`;
    el.toast.hidden = !h.toast;
    if (h.toast) el.toast.textContent = h.toast;
    // Frame times (command "perf 1"): ms per part of the frame, quality level, point lights.
    const p = h.perf;
    el.perf.hidden = !p?.show;
    if (p?.show) {
      const m = p.ms;
      el.perf.textContent = `${p.fps.toFixed(0)} fps · Logik ${m.logic.toFixed(1)} · Physik ${m.physics.toFixed(1)} · Stadt ${m.stream.toFixed(1)} · Zeichnen ${m.draw.toFixed(1)}` +
        ` · Render ${m.render.toFixed(1)}${m.gpu >= 0 ? ` · GPU ${m.gpu.toFixed(1)}` : ""} ms · Grafik ${p.tierName}${p.auto ? " (auto)" : ""} · ${p.lights} Lichter`;
    }
    el.offline.hidden = !h.offline;
    el.cross.hidden = h.mode !== "foot";
    if (h.mode === "foot") el.cross.style.setProperty("--spread", `${8 + h.spread * 7}px`);
    el.speed.textContent = Math.round(h.speed * 3.6);
    const rows = [];
    const row = (k, v, cls = "") => rows.push(`<div class="${cls}"><span>${k}</span><b>${v}</b></div>`);
    row("Höhe", `${h.agl.toFixed(1)} m`);
    if (h.drone) {
      const d = h.drone;
      row("Status", d.calibrating ? "KALIBRIERT" : d.fault ? "FEHLER" : d.landing ? "LANDET" : d.armed ? "ARMED" : "DISARMED", d.armed ? "hot" : "");
      row("Modus", d.game ? `GAME · Ziel ${d.clearance.toFixed(1)} m` : "ANGLE");
      row("Akku", `${Math.round(d.battery * 100)} % · ${d.volts.toFixed(1)} V · ${d.amps.toFixed(0)} A`, d.battery < 0.2 ? "warn" : "");
      row("Kamera", ["FPV", "FOLLOW", "THIRD"][d.cam]);
    }
    if (h.car) row("Gang", h.car.gear);
    if (h.jet) {
      const j = h.jet;
      row("Modus", j.mode);
      row("Schub", `${Math.round(j.thrust * 100)} %${j.mode === "FLUG" ? ` · Hebel ${Math.round(j.lever * 100)} %` : ""}`);
      row("Mach", j.mach.toFixed(2));
      row("Kurs", `${Math.round(j.heading)}°`);
      row("G", j.nz.toFixed(1));
      row("Fahrwerk", j.gear ? "UNTEN" : "OBEN");
      row("Treibstoff", `${Math.round(j.fuel * 100)} %`);
    }
    if (h.chute !== undefined) row("Schirm", h.chute ? "OFFEN" : "FREIFALL");
    if (h.weapon && h.mode !== "car" && h.mode !== "parachute") row("Waffe", h.weapon);
    if (h.mode === "foot") row("Granaten", h.grenades);
    if (h.carHp !== undefined) row("Zustand", `${Math.round(h.carHp / 3)} %`, h.carHp < 75 ? "warn" : "");
    if (h.droneHp !== undefined) row("Zustand", `${Math.round(h.droneHp)} %`, h.droneHp < 30 ? "warn" : "");
    if (h.jetHp !== undefined) row("Zustand", `${Math.round(h.jetHp / 4)} %`, h.jetHp < 100 ? "warn" : "");  // jet_mode.mjs HP_MAX 400
    if (h.stars > 0) row("Polizei", `${h.police} Wagen · ${h.policeDrones} Drohnen · EMP ${h.emp > 0 ? h.emp.toFixed(0) + " s" : (h.ctl?.pad ? "bereit (B)" : "bereit (G)")}`, "hot");
    if (h.net) row("Mehrspieler", h.net.status, h.net.connected ? "" : "warn");
    const c = h.combat;  // combat and events (game_events.cpp): numbers and texts from C++
    if (c) {
      if (c.mil !== "off") row("Militär", c.mil === "alert" ? "ALARM" : `${c.helis} Hubschrauber · ${c.tanks} Panzer · ${c.soldiers} Soldaten`, "hot");
      if (c.zombies.night) row("Zombies", `Runde ${c.zombies.round} · ${c.zombies.alive} · ${c.zombies.points} P`, "hot");
      if (c.enemies) row("Gegner", c.enemies, "hot");
      if (c.flak && (h.mode === "drone" || h.mode === "jet")) row("Flak", `${c.flak} Geschütz(e)`, c.flakRounds ? "warn" : "");
      row("Punkte", `${c.fun.score.toLocaleString("de-DE")}${c.fun.risk ? ` · Risiko ${c.fun.risk}` : ""} · x${c.fun.mult.toFixed(2)}`);
      row("Chaos", `${c.chaos.score.toLocaleString("de-DE")}${c.chaos.combo > 1 ? ` · x${c.chaos.combo}` : ""}`);
      row("Auftrag", `${c.fun.contract} ${c.fun.done ? "✓" : `${c.fun.progress}/${c.fun.target}`}`);
      row("Luftschlag", c.strike > 0 ? `${Math.ceil(c.strike)} s` : (h.ctl?.pad ? "bereit (D-PAD ⬆)" : "bereit (4)"));
      if (c.race) row("Rennen", `Runde ${c.race.lap}/${c.race.laps} · ${c.race.next} · ${c.race.time} · Best ${c.race.best}`, "hot");
    }
    const key = rows.join("");
    if (key !== this.key) { el.rows.innerHTML = key; this.key = key; }
    const hp = h.health >= 0 ? h.health / (h.maxHealth || 100) : 1;
    el.health.hidden = h.health < 0;
    el.healthBar.style.width = `${Math.max(0, Math.min(1, hp)) * 100}%`;
    el.health.classList.toggle("low", hp < 0.3);
    el.ammo.hidden = h.mode !== "foot" || h.ammo < 0;
    if (!el.ammo.hidden) el.ammo.innerHTML = `<b>${h.ammo}</b> / ${h.reserve}${h.reload >= 0 ? " · LADEN" : ""}`;
    el.hint.hidden = !h.hint;
    if (h.hint) el.hint.textContent = h.hint;
    // desktop_controls.mjs key line: desktop only, not while the pad is in use; fades 12 s after a change
    const ctl = h.ctl || {};
    document.body.classList.toggle("pad-input", !!ctl.pad);
    const help = !h.touch && ctl.device === "desktop" && !ctl.pad ? ctl.hint || "" : "";
    if (el.help.textContent !== help) el.help.textContent = help;
    el.help.classList.toggle("faded", !!ctl.hintFaded);
    if (ctl.aiming) {
      // fighter_jets.mjs targeting overlay
      el.hint.hidden = false;
      el.hint.textContent = h.touch ? "ZIEL MARKIEREN · Tippen = Laser auf das Ziel · die Jets kommen" : "ZIEL MARKIEREN · LMB / J / ENTER = Luftschlag · RMB / ESC = abbrechen";
    }
    el.banner.hidden = !c?.banner;
    if (c?.banner) el.banner.textContent = c.banner;
    el.pop.hidden = !c?.pop;
    if (c?.pop) el.pop.textContent = c.pop;
    el.scope.hidden = !c?.scope;  // sniper_scope.mjs: black tube, mil-dot reticle, red dot
    // guided_missile.mjs: reload state under the crosshair, the target of the missile in flight
    const g = h.guided, gw = h.mode === "drone" && h.weapon === "GUIDED" && g;
    el.guided.hidden = !gw;
    if (gw) el.guided.textContent = g.flying > 0 ? `LOCK · ${g.lock} · ${Math.round(g.dist)} m` : g.cd > 0 ? `LENKRAKETE LÄDT · ${g.cd.toFixed(1)} s` : "LENKRAKETE BEREIT";
  }
}
