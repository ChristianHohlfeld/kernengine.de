// SpawnCity on Kern - the page host. Everything the game decides runs in C++ (spawncity.wasm);
// this module only does what the browser forces into JavaScript:
//   * runs the Kern App host (kern/web/app.mjs: canvas, WebGL2, frame loop, keyboard / mouse)
//   * fetches the map tiles the game asks for (OpenFreeMap vector tiles, (c) OpenStreetMap
//     contributors) and hands the bytes over; no map service -> the game builds its stand-in town
//   * polls the gamepad and routes touch fingers (CSS px) to the game
//   * plays the samples Kern's mixer renders (KernAudio from kern/web/studio/game.mjs)
//   * draws the HUD from the game's JSON and the touch controls with Kern's TouchOverlay
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
import {runKernApp} from "../../../web/app.mjs";
import {KernAudio, TouchOverlay, TOUCH_STYLE} from "../../../web/studio/game.mjs";

const SC_TILEJSON = "https://tiles.openfreemap.org/planet";
const SC_MAX_FETCHES = 4;

/** Starts the game in the page (index.html). Module: url (dev) or base64 (single file). */
export async function startSpawnCity({url = null, base64 = null} = {}) {
  const params = new URLSearchParams(location.search);
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
  const str = ptr => kern.string(ptr, e.sc_result_length());
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
  const frame = () => {
    const c = getComputedStyle(probe), px = v => parseFloat(v) || 0;
    e.sc_touch_frame(canvas.clientWidth, canvas.clientHeight, px(c.paddingLeft), px(c.paddingTop), px(c.paddingRight), px(c.paddingBottom),
      Math.min(globalThis.devicePixelRatio || 1, mobile ? 1.5 : 2));
  };
  frame();
  addEventListener("resize", frame);
  addEventListener("orientationchange", frame);
  const local = ev => { const r = canvas.getBoundingClientRect(); return [ev.clientX - r.left, ev.clientY - r.top]; };
  const finger = type => ev => {
    if (ev.pointerType === "mouse") return;
    for (const c of (type === 1 && ev.getCoalescedEvents?.()) || [ev]) { const [x, y] = local(c); e.sc_touch(type, c.pointerId, x, y, c.timeStamp); }
    ev.preventDefault();
  };
  canvas.addEventListener("pointerdown", finger(0));
  canvas.addEventListener("pointermove", finger(1));
  canvas.addEventListener("pointerup", finger(2));
  canvas.addEventListener("pointercancel", finger(3));
  const overlay = new TouchOverlay(document.getElementById("stage"));

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

  // ---- HUD ----------------------------------------------------------------------------------
  const hud = new SpawnCityHud(hudEl);
  let frames = 0, last = null;
  const tick = () => {
    frames++;
    pollPad();
    pumpTiles();
    audio?.pump();
    try { last = JSON.parse(str(e.sc_hud())); } catch { /* keep the last one */ }
    if (last) hud.update(last, tiles);
    overlay.update(JSON.parse(str(e.sc_touch_view())));
    requestAnimationFrame(tick);
  };
  requestAnimationFrame(tick);
  window.SpawnCity = {app, command, hud: () => last, frames: () => frames, tiles};
  return window.SpawnCity;
}

const SC_MODE_LABEL = {foot: "ZU FUSS", drone: "DROHNE", car: "AUTO", jet: "JET", parachute: "FALLSCHIRM"};
const SC_HELP = {
  foot: "WASD laufen · Shift sprinten · Leertaste springen · Maus zielen/schießen · R nachladen · F einsteigen · V Drohne",
  drone: "R / Leertaste ARM · WASD fliegen · Leertaste/Shift Höhe · Maus drehen/neigen · Q/E drehen · C Kamera · G GAME/ANGLE · V zurück",
  car: "W Gas · S Bremse/rückwärts · A/D lenken · Leertaste Handbremse · C Cockpit · F aussteigen",
  jet: "W/S Schub · A/D Seitenruder · Maus nicken/rollen · V Schweben/Flug · C Cockpit · F aussteigen / Schleudersitz",
  parachute: "WASD steuern",
};

/** The HUD, drawn from the game's JSON (sc_hud): numbers only, no game logic. */
class SpawnCityHud {
  constructor(root) {
    this.root = root;
    this.root.innerHTML = `
      <div class="sc-top"><span class="sc-badge"></span><span class="sc-stars"></span><span class="sc-fps"></span></div>
      <div class="sc-toast" hidden></div>
      <div class="sc-offline" hidden>OFFLINE · prozedurale Ersatzstadt (keine echten Kartendaten)</div>
      <div class="sc-cross" hidden></div>
      <div class="sc-panel"><div class="sc-speed"><b>0</b><small>km/h</small></div><div class="sc-rows"></div></div>
      <div class="sc-health"><i></i></div>
      <div class="sc-ammo" hidden></div>
      <div class="sc-hint" hidden></div>
      <div class="sc-help"></div>
      <div class="sc-attrib"><a href="https://www.openstreetmap.org/copyright" target="_blank" rel="noopener">© OpenStreetMap-Mitwirkende</a> ·
        <a href="https://openfreemap.org" target="_blank" rel="noopener">OpenFreeMap</a> ·
        <a href="https://openmaptiles.org" target="_blank" rel="noopener">© OpenMapTiles</a> · Kern Engine</div>`;
    const q = s => this.root.querySelector(s);
    this.el = {badge: q(".sc-badge"), stars: q(".sc-stars"), fps: q(".sc-fps"), toast: q(".sc-toast"), offline: q(".sc-offline"), cross: q(".sc-cross"),
      speed: q(".sc-speed b"), rows: q(".sc-rows"), health: q(".sc-health"), healthBar: q(".sc-health i"), ammo: q(".sc-ammo"), hint: q(".sc-hint"),
      help: q(".sc-help"), panel: q(".sc-panel")};
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
      row("Fahrwerk", j.gear ? "AUS" : "EIN");
      row("Treibstoff", `${Math.round(j.fuel * 100)} %`);
    }
    if (h.chute !== undefined) row("Schirm", h.chute ? "OFFEN" : "FREIFALL");
    const key = rows.join("");
    if (key !== this.key) { el.rows.innerHTML = key; this.key = key; }
    const hp = h.health >= 0 ? h.health / (h.maxHealth || 100) : 1;
    el.health.hidden = h.health < 0;
    el.healthBar.style.width = `${Math.max(0, Math.min(1, hp)) * 100}%`;
    el.health.classList.toggle("low", hp < 0.3);
    el.ammo.hidden = h.mode !== "foot" || h.ammo < 0;
    if (!el.ammo.hidden) el.ammo.innerHTML = `<b>${h.mag}</b> / ${h.reserve}${h.reload >= 0 ? " · LADEN" : ""}`;
    el.hint.hidden = !h.hint;
    if (h.hint) el.hint.textContent = h.hint;
    el.help.textContent = h.touch ? "" : SC_HELP[h.mode] || "";
  }
}
