// SpawnCity on Kern - the original's view layer. The page shows exactly the DOM of spawncity.de
// (web/view/original.html and original.css, taken by tools/take_original_view.py) and the
// original's own layout modules place it (web/view/edge_hud_layout.mjs, thumb_layout.mjs). This
// module writes into it what the original's modules write - texts, classes, data attributes - from
// the game's JSON (sc_hud, sc_touch_view). The game decides; nothing here is game logic.
//
// Each binder names the original module whose DOM writes it repeats.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).

const $ = id => document.getElementById(id);
const setText = (el, t) => { if (el && el.textContent !== t) el.textContent = t; };
const setHidden = (el, h) => { if (el && el.hidden !== h) el.hidden = h; };
const setData = (el, k, v) => { if (el && el.dataset[k] !== v) el.dataset[k] = v; };
const cls = (k, on) => document.body.classList.toggle(k, !!on);

/** Puts the original's markup and CSS into the page. Returns false when they are not there. */
export async function mountOriginalView(base = "./view/") {
  let html = globalThis.__scViewHtml, css = globalThis.__scViewCss;
  try {
    if (!html) html = await (await fetch(base + "original.html")).text();
    if (!css) css = await (await fetch(base + "original.css")).text();
  } catch (e) {
    console.warn("[spawncity] original view not available:", e?.message || e);
    return false;
  }
  const style = document.createElement("style");
  style.dataset.scView = "original";
  // fonts/ relative to the view folder (single file: the build inlines them as data: URLs)
  style.textContent = css.replace(/url\("fonts\//g, `url("${globalThis.__scViewFonts || base + "fonts/"}`);
  document.head.prepend(style);
  // Kern's own page bits that the original does not have (stage, status, Kern HUD) give way.
  // Kern's stage (its canvas) goes; an empty one stays for Kern's (unused) touch overlay.
  { const st = $("stage"); if (st) { st.querySelector("canvas")?.remove(); st.style.display = "none"; } }
  // Kern's load status stays as the source of the menu's status line (view.mjs), not shown itself.
  const status = $("status");
  if (status) status.style.display = "none";
  const kernHud = $("hud");
  const tpl = document.createElement("template");
  tpl.innerHTML = html;
  document.body.prepend(tpl.content);
  // The Kern HUD root (speech bubbles, banners, perf line from life.mjs / gaps_hud.mjs) lives in
  // the game view, above the picture like the original's overlays.
  if (kernHud) { kernHud.classList.add("sc-kern-extra"); $("viewport").appendChild(kernHud); }
  document.documentElement.classList.add("sc-original-view");
  const extra = document.createElement("style");
  extra.textContent = `
    html.sc-original-view #hud{position:absolute;inset:0;pointer-events:none;z-index:20}
    html.sc-original-view #hud > :is(.sc-top,.sc-map,.sc-panel,.sc-health,.sc-ammo,.sc-attrib,.sc-help,.sc-toast,.sc-cross,.sc-offline){display:none!important}
    html.sc-original-view :is(#scMenu,#scPause,#scSettings,#scQuit),html.sc-original-view .k-touch{display:none!important}
    html.sc-original-view #game{position:absolute;inset:0;z-index:2;display:block;width:100%;height:100%;outline:none;touch-action:none}
    html.sc-original-view :is(.scui-corner,#scInstall,#scInstallHint){display:none!important}
    html.sc-original-view :is(.scui-switches,#mpBar){display:none!important}`;
  document.head.appendChild(extra);
  // The original's layout modules, unchanged (they read the walk mode through this name).
  const walk = {mode: "drone"};
  globalThis.__arondightWalkMode = walk;
  // start_location.mjs builds its own button and panel (it mounts only when they are not there)
  // (and so do world_options_menu.mjs / changelog_menu.mjs: their title buttons and panels)
  // (and thumb_layout.mjs its #thumbWeapon: a second one from the snapshot was the one the game and
  // edge_hud_layout.mjs found - hidden - so JUMP took the weapon's place under the module's button)
  for (const id of ["startPlaceButton", "startPlacePanel", "gameMenuWorld", "worldOptionsPanel", "gameMenuChangelog", "changelogPanel", "thumbWeapon"]) $(id)?.remove();
  globalThis.__arondightRealWorld = {active: false, originLat: NaN, originLon: NaN};
  if (globalThis.__scViewLayouts) globalThis.__scViewLayouts();  // single file (build.sh)
  else {
    for (const m of ["start_location.mjs", "world_options.mjs", "world_options_menu.mjs", "changelog_menu.mjs"])
      await import(base + m).catch(e => console.warn("[spawncity] " + m, e));
    await import(base + "edge_hud_layout.mjs").catch(e => console.warn("[spawncity] edge layout", e));
    await import(base + "thumb_layout.mjs").catch(e => console.warn("[spawncity] thumb layout", e));
  }
  // The title screen's flyover city (drone_simulator.html inline script, unchanged).
  if (globalThis.__scViewFlyover) globalThis.__scViewFlyover();
  else for (const f of ["menu_flyover.js", "install_app.js"]) { const sc = document.createElement("script"); sc.src = base + f; document.body.appendChild(sc); }
  return true;
}

// The game's touch buttons (touch_play.cpp ids) and the original's elements that show them.
const TOUCH_DOM = {
  jump: "footJump", wpn: "thumbWeapon", scope: "scopeButton", nade: "handGrenadeBtn", strike: "airStrikeButton",
  mode: "mobileGameplayMode", jet: "enterJetButton", emp: "wantedEmpButton", arm: "soloArm", stop: "soloKill",
  hb: "vehicleBrakeButton", radio: "carRadioButton", fire: "footFire",
};
const JET_DOM = {vtol: "mode", rkt: "rocket", bomb: "bomb", cam: "cam", eject: "exit"};
// Original elements a finger uses as page buttons (their own click), never as game input.
const PAGE_BUTTONS = ".sc-radio-btn,#soloCamera,#camFpv,#camFollow,#camThird,#gameExitButton,#mobileGameplayDock,#mobileGameplayMultiplayer,#mobileGameplayReset,#proximityVoiceBar,#worldLookHud,#gpsLiveButton,#musicSwitch,#soundSwitch,#gameplayContractHud,#vsRespawnHud,#gpsLiveHud,#zombieRepair,dialog,#gameMenu,#scExitAsk";

/** A finger on the game view goes to the game unless it lands on a page button. */
export function fingerForGame(ev) {
  const t = ev.target instanceof Element ? ev.target : null;
  if (!t || !t.closest("#viewport")) return false;
  const game = !t.closest(PAGE_BUTTONS);
  // the game presses this button itself (its touch rect): the click that follows the finger must not
  // press it a second time (DRONE toggled there and straight back, the weapon switched twice)
  if (game) { const b = t.closest("button,[role=button]"); if (b) { gameTouched.el = b; gameTouched.at = performance.now(); } }
  return game;
}
const gameTouched = {el: null, at: -1e9};
// a finger anywhere else (or the mouse) ends the claim; the game's own finger sets it again
addEventListener("pointerdown", () => { gameTouched.el = null; }, {capture: true});
/** True when this element was pressed by the game-owned finger that went down on it last (its
 *  click is a duplicate): once, however long the finger stayed (a slow frame, a held button - a
 *  time window let the click through after a long press: DRONE toggled there and straight back,
 *  ARM armed and disarmed, the sticks then did nothing). */
function pressedByGame(el) {
  if (!gameTouched.el || performance.now() - gameTouched.at > 20000) return false;
  // thumb_layout.mjs: the thumb weapon button clicks the dock's WEAPON button on pointerdown
  const hit = gameTouched.el === el || (el?.id === "mobileGameplayWeapon" && gameTouched.el.id === "thumbWeapon");
  if (hit) gameTouched.el = null;
  return hit;
}

const FOOT_NAMES = {MP: "MP", Glock: "GLOCK", Sniper: "SNIPER", "Sniper .50": "SNIPER", Granatwerfer: "WERFER", "Granatwerfer 40 mm": "WERFER", Raketenwerfer: "RAKETEN",
  "Fäuste": "FÄUSTE", Faeuste: "FÄUSTE", Messer: "MESSER"};  // the game's names (weapons.h) -> the original's labels
const FOOT_ORDER = ["MP", "GLOCK", "SNIPER", "WERFER", "RAKETEN", "FÄUSTE", "MESSER"];  // first_person_weapon_runtime_v3.mjs WEAPON_ORDER
const DRONE_NAMES = {MG: "GUN", RAKETEN: "ROCKETS", GUIDED: "GUIDED"};                  // drone_weapon_modes.mjs

// Binders of other areas (view_modes.mjs, ...): fn(h, touchView, view) runs every frame after the
// core bindings below; each repeats the DOM writes of the original modules it names.
const BINDERS = [];
export function registerViewBinder(name, fn) { BINDERS.push({name, fn}); }

export class OriginalView {
  constructor({command, ui, mp}) {
    this.command = command;
    this.ui = ui;
    this.mp = mp;
    this.rectKey = "";
    this.wire();
    setInterval(() => { try { this.minimap(); } catch (e) { if (!this.mapWarned) { this.mapWarned = true; console.warn("[spawncity] minimap", e); } } }, 125);  // WORLD_MINIMAP_DRAW_MS
  }

  wire() {
    // A clicked HUD button must not keep the keyboard focus: SPACE / ENTER would press it again
    // (SPACE = jump became WEAPON). The game's buttons are pointer targets; the keys belong to the
    // game (desktop_controls.mjs). Dialogs and the title menu keep normal focus for their keys.
    const hudButton = t => t instanceof Element && t.closest("button,[role=button]") && !t.closest("dialog,#gameMenu,#worldOptionsPanel,#changelogPanel,#startPlacePanel,input,select,textarea");
    document.addEventListener("mousedown", ev => { if (hudButton(ev.target)) ev.preventDefault(); }, {capture: true});
    const release = ev => {
      const a = document.activeElement;
      if ((ev.code === "Space" || ev.code === "Enter" || ev.code === "NumpadEnter") && hudButton(a)) { a.blur(); ev.preventDefault(); }
    };
    document.addEventListener("keydown", release, {capture: true});
    document.addEventListener("keyup", release, {capture: true});
    const click = (id, fn) => $(id)?.addEventListener("click", ev => {
      ev.preventDefault(); ev.stopPropagation();
      if (!pressedByGame(ev.currentTarget)) fn(ev);
      ev.currentTarget?.blur?.();
    });
    const kernAct = act => document.querySelector(`#scMenu [data-act="${act}"]`)?.click();
    // title menu (drone_simulator.html #gameMenu; auto_flight_start.mjs START)
    click("gameMenuStart", () => {
      // START is felt at once: the button answers (pressed, "STARTE …") and a short loading screen
      // covers the world until the ground and the city around the start point stand
      const b = $("gameMenuStart");
      if (b) { b.dataset.pressed = "1"; b.textContent = "STARTE …"; }
      this.loadingSince = performance.now();
      $("scStart")?.click();
    });
    // the title's own START (ui.mjs #scStart) answers the same way
    $("scStart")?.addEventListener("click", () => {
      const b = $("scStart");
      if (b) { b.dataset.pressed = "1"; b.textContent = "STARTE …"; }
      if (!(this.loadingSince > 0)) this.loadingSince = performance.now();
    }, true);
    click("gameMenuShare", () => kernAct("share"));
    // the game view's top bar and dock (mobile_gameplay_ui.mjs, game_exit_button.mjs)
    click("gameExitButton", () => this.ui?.open?.("exit"));
    click("mobileGameplaySettings", () => this.ui?.open?.("settings"));
    // the real-world topbar's SETTINGS (#soloTopbar .phone-settings-button, phone_control_settings):
    // the same MENU dialog
    for (const b of document.querySelectorAll(".phone-settings-button"))
      b.addEventListener("click", ev => { ev.preventDefault(); ev.stopPropagation(); this.ui?.open?.("settings"); b.blur(); });
    click("mobileGameplayReset", () => this.command("reset"));
    click("mobileGameplayWeapon", () => this.command("press wpn"));
    click("mobileGameplayMode", () => this.command("press mode"));
    click("mobileGameplayMultiplayer", () => { if (this.mp?.toggleMulti) this.mp.toggleMulti(this.last); else this.mp?.goMulti?.(); });
    click("enterCarButton", () => this.command("press enter"));
    click("enterJetButton", () => this.command("press jet"));
    click("soloArm", () => this.command("press arm"));
    // simulator.mjs: the drone camera (FPV / FOLLOW / THIRD), the button cycles, the three pick;
    // the choice is kept (localStorage arondight45CameraMode, as the original)
    const camStore = "arondight45CameraMode";
    const setCam = m => { this.command(`cam ${m}`); try { localStorage.setItem(camStore, m); } catch { /* private */ } };
    click("soloCamera", () => { const m = this.command("cam"); if (/^(fpv|follow|third)$/.test(m)) { try { localStorage.setItem(camStore, m); } catch { /* private */ } } });
    click("camFpv", () => setCam("fpv"));
    click("camFollow", () => setCam("follow"));
    click("camThird", () => setCam("third"));
    try { const saved = localStorage.getItem(camStore); if (/^(fpv|follow|third)$/.test(saved || "")) this.command(`cam ${saved}`); } catch { /* private */ }
    click("soloKill", () => this.command("press stop"));
    click("gpsLiveButton", () => kernAct("gps"));
    click("proximityVoicePtt", () => this.command("voice mic"));
    click("proximityVoiceMute", () => this.command("voice mute"));
    // EFFECTS / MUSIC toggles (drone_simulator.html inline script): the game's audio.on / music.on
    const toggle = (id, key) => {
      const b = $(id);
      if (!b) return;
      b.addEventListener("pointerdown", ev => ev.stopPropagation(), true);
      b.addEventListener("click", ev => {
        ev.preventDefault(); ev.stopPropagation();
        const on = Number(this.command(`get ${key}`)) > 0.5;
        this.command(`set ${key} ${on ? 0 : 1}`);
        globalThis.__scAudioSetting?.(key, on ? 0 : 1);  // the original's settings at once (audio.mjs)
        // the music switch also stops / starts the menu's element right away (audio.mjs plays it)
        const m = key === "music.on" && window.__oppMusicElement;
        if (m) { if (on) m.pause(); else m.play().catch(() => {}); }
        try { const k = "spawnCityKernSettingsV1", c = JSON.parse(localStorage.getItem(k) || "{}"); c[key] = on ? 0 : 1; localStorage.setItem(k, JSON.stringify(c)); } catch { /* none */ }
      });
    };
    toggle("soundSwitch", "audio.on");
    toggle("musicSwitch", "music.on");
    // dock(): in game just left of RESET, else the menu's top-right spot (every 400 ms, as the original)
    const dock = () => {
      const btn = $("soundSwitch"), mbtn = $("musicSwitch");
      if (btn && mbtn && !(document.body.classList.contains("edge-hud") || btn.dataset.edgeAdopted)) {
        let reset = null;
        for (const id of ["mobileGameplayReset", "soloReset"]) { const el = $(id); if (!el || !el.offsetParent) continue; const q = el.getBoundingClientRect(); if (q.width < 4 || q.height < 4) continue; if (!reset || q.top < reset.getBoundingClientRect().top - 2) reset = el; }
        const menu = $("gameMenu"), inGame = menu && menu.hidden && reset;
        if (inGame) { const r = reset.getBoundingClientRect(), right = innerWidth - r.left + 6; btn.style.top = r.top + "px"; btn.style.right = right + "px"; btn.style.height = r.height + "px"; mbtn.style.top = r.top + "px"; mbtn.style.right = (right + btn.offsetWidth + 6) + "px"; mbtn.style.height = r.height + "px"; }
        else for (const b of [btn, mbtn]) { b.style.top = ""; b.style.right = ""; b.style.height = ""; }
      }
      // gps_live_mode.mjs: the GPS button sits left of the music switch
      const g = $("gpsLiveButton"), m = $("musicSwitch")?.getBoundingClientRect?.();
      if (g && !g.dataset.edgeAdopted) { if (m && m.width) { g.style.top = `${Math.round(m.top)}px`; g.style.right = `${Math.round(innerWidth - m.left + 8)}px`; } else { g.style.top = "10px"; g.style.right = "150px"; } }
      setTimeout(dock, 400);
    };
    dock();
    for (const b of document.querySelectorAll("#worldLookHud [data-map-expand]")) b.addEventListener("click", ev => { ev.preventDefault(); ev.stopPropagation(); this.command("bigmap toggle"); });
  }

  /** The loading screen after START: until the start tile is built and collides and the ring around
   *  it has (nearly) arrived - at most 15 s; the bar follows the tiles. */
  loading(h, playing) {
    if (!(this.loadingSince > 0)) return;
    let el = $("scLoading");
    if (!el) {
      el = document.createElement("div");
      el.id = "scLoading";
      el.innerHTML = '<div class="scl-box"><b>SPAWNCITY</b><span class="scl-text">Lade die Stadt …</span><i class="scl-bar"><i></i></i><small class="scl-sub"></small></div>';
      const st = document.createElement("style");
      st.textContent = `#scLoading{position:fixed;inset:0;z-index:2147480000;display:flex;align-items:center;justify-content:center;background:radial-gradient(ellipse at 50% 45%,#141a22,#07090c);color:#eef0f2;font-family:Inter,system-ui,sans-serif;transition:opacity .35s ease;touch-action:none}
#scLoading.done{opacity:0;pointer-events:none}
#scLoading .scl-box{display:flex;flex-direction:column;align-items:center;gap:14px;min-width:min(70vw,340px)}
#scLoading b{font:900 italic clamp(28px,7vmin,54px)/1 Inter,system-ui,sans-serif;letter-spacing:.04em;text-shadow:0 3px 0 #0008}
#scLoading .scl-text{font:700 13px/1 Inter,system-ui,sans-serif;letter-spacing:.3em;text-transform:uppercase;color:#eef0f2cc}
#scLoading .scl-bar{display:block;width:100%;height:6px;border-radius:3px;background:#ffffff1c;overflow:hidden}
#scLoading .scl-bar i{display:block;height:100%;width:0;border-radius:3px;background:linear-gradient(90deg,#ff7a1a,#ffb02e);box-shadow:0 0 12px #ff7a1a88;transition:width .25s ease}
#scLoading .scl-sub{font:600 11px/1 Inter,system-ui,sans-serif;letter-spacing:.12em;color:#eef0f288}
#gameMenuStart[data-pressed="1"],#scStart[data-pressed="1"]{filter:brightness(1.15);transform:scale(.97);box-shadow:0 0 0 3px #ffd76a,0 0 26px #ff7a1a99!important}`;
      document.head.appendChild(st);
      (document.body).appendChild(el);
    }
    const t = h.tiles || {}, tt = t.t || {}, age = (performance.now() - this.loadingSince) / 1000;
    const ground = !!h.placed && (h.offline || (tt.built >= 0 && tt.collision >= 0));
    const total = Math.max(1, (t.ready || 0) + (t.loading || 0)), frac = Math.min(1, ((h.placed ? 0.2 : 0) + (ground ? 0.3 : 0) + 0.5 * (t.ready || 0) / total));
    const bar = el.querySelector(".scl-bar i");
    if (bar) bar.style.width = `${Math.round(Math.max(frac, Math.min(0.9, age / 15)) * 100)}%`;
    setText(el.querySelector(".scl-text"), !h.placed ? "Lade die Stadt …" : !ground ? "Boden und Gebäude …" : "Fast fertig …");
    setText(el.querySelector(".scl-sub"), `${t.ready || 0} / ${total} Kacheln`);
    const ready = playing && ground && age > 0.6 && ((t.loading || 0) <= 1 || age > 8);
    if (ready || age > 15) {
      if (bar) bar.style.width = "100%";
      el.classList.add("done");
      this.loadingSince = 0;
      setTimeout(() => el.remove(), 400);
    }
  }

  update(h, touch) {
    if (!h) return;
    this.last = h;
    const playing = !!this.ui?.playing, mode = h.mode, ctl = h.ctl || {};
    const desktop = ctl.device === "desktop";
    // ---- body classes (simulator.mjs solo-flight, player_walk_mode_v4.mjs on-foot-mode,
    // player_car_mode.mjs player-driving, jet_mode.mjs jet-mode, desktop_controls.mjs desktop-input /
    // pad-input, mobile_gameplay_ui.mjs mobile-gameplay-compact, player_vitals_runtime.mjs player-dead)
    cls("solo-flight", playing);
    // the walker stays the walker in the car and in the jet (and under the canopy): the original keeps
    // on-foot-mode there (the foot sticks are claimed, the drone's ARM / STOP stay hidden)
    cls("on-foot-mode", mode === "foot" || mode === "car" || mode === "jet" || mode === "parachute");
    cls("player-driving", mode === "car");
    cls("jet-mode", mode === "jet" || mode === "parachute");
    cls("desktop-input", desktop);
    cls("pad-input", !!ctl.pad);
    cls("mobile-gameplay-compact", true);
    cls("player-dead", !!ctl.dead);
    // real_world_bootstrap.mjs: #viewport[data-world-mode="real"] once the real world is mounted
    // ("training" without it). The real-world HUD (minimap, topbar, contract) hangs on it in the CSS.
    const vp = document.getElementById("viewport");
    if (vp) {
      const world = h.offline ? "training" : "real";
      if (vp.dataset.worldMode !== world) vp.dataset.worldMode = world;
    }
    globalThis.__arondightWalkMode.mode = mode === "drone" ? "drone" : "foot";
    // ---- title menu (drone_simulator.html, auto_flight_start.mjs, start_location.mjs)
    const menu = $("gameMenu");
    setHidden(menu, playing);
    const start = $("gameMenuStart");
    // the original's title: START and READY from the first frame (the city streams in after START)
    if (start && !(this.loadingSince > 0)) { start.disabled = false; setText(start, "START"); delete start.dataset.pressed; }
    const sc = $("scStart");
    if (sc && !(this.loadingSince > 0) && sc.dataset.pressed) { delete sc.dataset.pressed; sc.textContent = "START"; }
    this.loading(h, playing);
    setText($("gameMenuStatus"), h.placed && h.offline ? "OFFLINE" : "READY");
    // ---- the toggles' state (drone_simulator.html inline script render / renderMusic)
    for (const [id, on] of [["soundSwitch", h.ui?.audioOn], ["musicSwitch", h.ui?.musicOn]]) {
      const b = $(id);
      if (b && on !== undefined) { setData(b, "on", on ? "1" : "0"); b.setAttribute("aria-pressed", on ? "true" : "false"); }
    }
    // what start_location.mjs reads of the running world (search bias, "far from here")
    // (only while playing: on the title the original has no world yet - a search for "New York" from
    // the title must find New York, not a street of that name near the last place)
    { const o = h.ui?.origin, w = globalThis.__arondightRealWorld; if (w) { w.active = !!h.placed && playing; if (o && playing) { w.originLat = o[0]; w.originLon = o[1]; } else { w.originLat = NaN; w.originLon = NaN; } } }
    // ---- what is there while you play: simulator.mjs enterSolo (soloHud), game_exit_button.mjs
    // sync (EXIT, MULTI right after it), proximity_voice.mjs updateLayout (the voice bar)
    setHidden($("soloHud"), !playing);
    setHidden($("proximityVoiceBar"), !playing);
    const exit = $("gameExitButton"), multi = $("mobileGameplayMultiplayer");
    setHidden(exit, !playing);
    if (multi) {
      multi.classList.remove("split-left");
      multi.classList.toggle("hud-after-exit", playing);
      // mobile_gameplay_ui.mjs: the button tells what the multiplayer is doing (offline / finding /
      // waiting / connected / error) and the line under it says it in words
      const ms = this.mp?.uiState?.(h);
      if (ms) {
        if (multi.dataset.state !== ms.state) multi.dataset.state = ms.state;
        setText(multi, ms.label);
        const st = $("mobileGameplayMultiplayerStatus");
        if (st) { setText(st, ms.status); if (st.dataset.state !== ms.state) st.dataset.state = ms.state; st.style.display = playing && ms.status ? "block" : "none"; }
      }
      if (playing && exit?.offsetWidth) {
        const l = `${exit.offsetLeft + exit.offsetWidth + 6}px`;
        if (multi.style.getPropertyValue("left") !== l) multi.style.setProperty("left", l, "important");
        const st = $("mobileGameplayMultiplayerStatus");
        if (st && st.style.getPropertyValue("left") !== l) st.style.setProperty("left", l, "important");
      }
    }
    // ---- vitals (player_vitals_runtime.mjs)
    const vit = $("playerVitalsHud");
    if (vit) {
      const hp = Math.max(0, Math.round(h.health >= 0 ? h.health : 100)), dhp = Math.round(h.droneHp ?? 100);
      setData(vit, "playerDead", ctl.dead ? "1" : "0");
      setData(vit, "droneDestroyed", dhp <= 0 ? "1" : "0");
      setText(vit.querySelector('[data-vital="player"] b'), String(hp));
      setText(vit.querySelector('[data-vital="drone"] b'), String(Math.max(0, dhp)));
    }
    // ---- chaos score and FLOW (sandbox_game.mjs renderHud)
    const ch = h.combat?.chaos;
    if (ch) {
      setText(document.querySelector("#sandboxHud .sb-score"), ch.score.toLocaleString("de-DE"));
      setText(document.querySelector("#sandboxHud .sb-flow em"), `x${ch.combo}`);
      const bar = document.querySelector("#sandboxHud .sb-flow b");
      if (bar) bar.style.width = `${Math.round((ch.flow || 0) * 100)}%`;
    }
    // ---- mode switch (mobile_gameplay_ui.mjs renderModeSwitch)
    const ms = $("mobileGameplayMode");
    if (ms) {
      const inJet = mode === "jet" || mode === "parachute", driving = mode === "car", onFoot = mode === "foot";
      const text = inJet ? (mode === "parachute" ? "CHUTE" : h.agl > 3 ? "EJECT" : "FPS") : driving ? "FPS" : onFoot ? "DRONE" : "FPS";
      setText(ms, text);
      setData(ms, "current", inJet ? "jet" : driving ? "car" : onFoot ? "foot" : "drone");
      setData(ms, "target", inJet || driving || !onFoot ? "foot" : "drone");
      ms.disabled = mode === "parachute";
    }
    // ---- weapon (mobile_gameplay_ui.mjs dock label, first_person_weapon_runtime_v3.mjs foot toggle)
    const wname = mode === "drone" ? (DRONE_NAMES[h.weapon] || String(h.weapon || "").toUpperCase()) : (FOOT_NAMES[h.weapon] || String(h.weapon || "").toUpperCase());
    setText($("mobileGameplayWeapon"), `WEAPON · ${wname}`);
    const ft = $("footWeaponToggle");
    if (ft && mode === "foot") { const i = FOOT_ORDER.indexOf(wname); setText(ft, `${wname || "MP"} → ${FOOT_ORDER[(i + 1) % FOOT_ORDER.length]}`); }
    // ---- getting in (player_car_mode.mjs renderEnterButton, jet_mode.mjs renderButtons)
    const touchUi = !desktop, key = ctl.pad ? "Ⓧ  " : desktop ? "F · " : "";
    const car = $("enterCarButton");
    if (car) {
      const show = (mode === "foot" && ctl.nearCar) || mode === "car";
      setHidden(car, !show);
      setText(car, touchUi ? (mode === "car" ? "↗" : "🚗") : mode === "car" ? `${key}AUSSTEIGEN` : `${key}🚗 EINSTEIGEN`);
    }
    const jet = $("enterJetButton");
    if (jet) {
      setHidden(jet, !(mode === "foot" && ctl.nearJet));
      setText(jet, touchUi ? "✈" : ctl.pad ? "Ⓧ  ✈ JET EINSTEIGEN" : desktop ? "F · ✈ JET EINSTEIGEN" : "✈ JET EINSTEIGEN");
    }
    // ---- drone (simulator.mjs solo loop)
    const d = h.drone;
    if (d) {
      setText($("soloClearanceValue"), `${(d.clearance ?? 1.2).toFixed(1)} m`);
      const rs = $("soloRangeStatus");
      setText(rs, `AGL ${(h.agl ?? 0).toFixed(1)} m`);
      if (rs && rs.style.color !== "rgb(100, 224, 174)") rs.style.color = "#64e0ae";
      const camName = ["FPV", "FOLLOW", "THIRD"][d.cam] || "FPV";
      setText($("soloCamera"), camName);
      for (const [id, k] of [["camFpv", 0], ["camFollow", 1], ["camThird", 2]]) {
        const b = $(id);
        if (!b || b.dataset.active === String(d.cam === k ? 1 : 0)) continue;
        b.dataset.active = d.cam === k ? "1" : "0";
        b.style.background = d.cam === k ? "#17694f" : "rgba(17,29,43,.82)";
        b.style.borderColor = d.cam === k ? "#62d6aa" : "rgba(255,255,255,.3)";
      }
      const arm = $("soloArm");
      if (arm) {
        const st = d.calibrating ? "CALIBRATING" : d.armed ? "ARMED" : "DISARMED";
        arm.classList.toggle("armed", st === "ARMED");
        arm.classList.toggle("attention", !d.armed && !d.calibrating);
        setText(arm, d.landing ? "LANDING…" : st === "ARMED" ? "ARMED ✓" : st === "CALIBRATING" ? "CALIBRATING…" : "ARM");
      }
    }
    // ---- desktop key line (desktop_controls.mjs #desktopKeyHints)
    const hints = $("desktopKeyHints");
    if (hints) { setText(hints, desktop && !ctl.pad ? ctl.hint || "" : ""); hints.classList.toggle("faded", !!ctl.hintFaded); }
    // ---- sticks (foot_touch_controller.mjs paintKnob, simulator.mjs solo knobs)
    if (touch) this.sticks(touch, mode);
    for (const b of BINDERS) { try { b.fn(h, touch, this); } catch (e) { if (!b.warned) { b.warned = true; console.warn(`[spawncity] view ${b.name}`, e); } } }
    // ---- touch buttons: the game's hit areas follow what the original's layout drew
    this.sendRects(touch, mode);
  }

  /** real_world_bootstrap.mjs drawMinimap (every 125 ms) and renderLookHud's title / readout. */
  minimap() {
    const canvas = document.querySelector("#worldLookHud .world-mini-canvas"), h = this.last;
    if (!canvas || !h) return;
    let m, mates = [];
    try { m = JSON.parse(this.command("minimap")); } catch { return; }
    try { mates = JSON.parse(this.command("map")).players || []; } catch {}
    if (!m.me) return;
    const ctx = canvas.getContext("2d"), w = canvas.width, hh = canvas.height;
    const locked = !!h.ui?.northUp, miniBearing = !document.fullscreenElement && !locked ? (h.ctl?.look?.[0] || 0) : 0;
    const rad = miniBearing * Math.PI / 180, c = Math.cos(rad), si = Math.sin(rad);
    const expanded = $("worldLookHud")?.classList.contains("expanded"), zoom = this.mapZoom || 1, fovScale = 1;  // fpvFovDeg 105
    const clamp = (v, a, b) => Math.min(b, Math.max(a, v));
    const radius = clamp(((55 + (m.mode === "drone" ? Math.max(0, m.me[2]) : 0) * 2) * fovScale) / (expanded ? zoom : 1), expanded ? 15 : 42, expanded ? 380 : 190);
    const scale = w / (radius * 2), baseY = hh * 0.5;
    const P = (dx, dy) => { const rx = dx * c - dy * si, ry = dx * si + dy * c; return [w / 2 + rx * scale, baseY - ry * scale]; };
    ctx.fillStyle = "#2a3026"; ctx.fillRect(0, 0, w, hh);
    if (h.ui?.grid !== false) {
      ctx.lineWidth = 1;
      // the grid is anchored to the world (every 20 m), like the original's projectLocal of the subject's cell
      for (let v = -Math.ceil(radius / 20) * 20; v <= radius; v += 20)
        for (const axis of [0, 1]) {
          const a = axis ? P(-radius, v) : P(v, -radius), b = axis ? P(radius, v) : P(v, radius);
          ctx.strokeStyle = v === 0 ? "rgba(255,255,255,.3)" : "rgba(255,255,255,.1)";
          ctx.beginPath(); ctx.moveTo(...a); ctx.lineTo(...b); ctx.stroke();
        }
    }
    for (const f of m.f) {
      const kind = f[0], pts = [];
      for (let i = 1; i + 1 < f.length; i += 2) pts.push(P(f[i], f[i + 1]));
      if (pts.length < 2) continue;
      const poly = kind.length > 1, path = () => { ctx.beginPath(); pts.forEach((p, i) => i ? ctx.lineTo(...p) : ctx.moveTo(...p)); };
      if (kind[0] === "r") { ctx.strokeStyle = "#b9b9b4"; ctx.lineWidth = 2; path(); ctx.stroke(); continue; }
      if (kind[0] === "w") { ctx.fillStyle = "#2f5f86"; ctx.strokeStyle = "#3c76a3"; ctx.lineWidth = 1; path(); if (poly) { ctx.closePath(); ctx.fill(); } ctx.stroke(); continue; }
      if (kind[0] === "g") { ctx.fillStyle = "#34482a"; path(); ctx.closePath(); ctx.fill(); continue; }
      ctx.fillStyle = "#5b5e63"; ctx.strokeStyle = "#7b7e83"; ctx.lineWidth = 1; path(); ctx.closePath(); ctx.fill(); ctx.stroke();
    }
    // every remote player, clamped to the edge (vs_multiplayer.mjs minimapPlayers)
    ctx.save(); ctx.font = "bold 11px system-ui,sans-serif"; ctx.textAlign = "center";
    for (const mate of mates) {
      const [rx, ry] = P(mate[0], mate[1]);
      const mx = clamp(rx, 12, w - 12), my = clamp(ry, 13, hh - 14), off = mx !== rx || my !== ry;
      ctx.save(); ctx.translate(mx, my); ctx.rotate(((mate[3] ?? 0) * Math.PI / 180) - rad);
      ctx.fillStyle = mate[4] ? "#777777" : "#40dcff"; ctx.strokeStyle = "#081019"; ctx.lineWidth = 1.8;
      ctx.beginPath(); ctx.moveTo(0, -9); ctx.lineTo(6, 6); ctx.lineTo(0, 3); ctx.lineTo(-6, 6); ctx.closePath(); ctx.fill(); ctx.stroke();
      if (off) { ctx.beginPath(); ctx.arc(0, 0, 10, 0, Math.PI * 2); ctx.stroke(); }
      ctx.restore();
      ctx.fillStyle = "#ffffff"; ctx.strokeStyle = "#101010"; ctx.lineWidth = 3;
      const label = "P" + String(mate[2] ?? "?"); ctx.strokeText(label, mx, my + 20); ctx.fillText(label, mx, my + 20);
    }
    ctx.restore();
    const rel = (m.bearing - miniBearing) * Math.PI / 180;
    ctx.save(); ctx.translate(w / 2, baseY); ctx.rotate(rel); ctx.fillStyle = "#f5b301"; ctx.strokeStyle = "#111"; ctx.lineWidth = 1.4;
    ctx.beginPath(); ctx.moveTo(0, -9); ctx.lineTo(6, 7); ctx.lineTo(0, 4); ctx.lineTo(-6, 7); ctx.closePath(); ctx.fill(); ctx.stroke(); ctx.restore();
    ctx.fillStyle = "#ffffff"; ctx.font = "600 11px Inter,system-ui,sans-serif"; ctx.fillText(Math.abs(miniBearing) < .01 ? "N↑" : "VIEW↑", 7, 15);
    ctx.fillStyle = "rgba(255,255,255,.9)"; ctx.font = "600 9px Inter,system-ui,sans-serif";
    if (m.mode === "drone") ctx.fillText(`${Math.round(m.me[2])}m`, 7, hh - 7);
    setText(document.querySelector("#worldLookHud [data-world-minimap-title]"), Math.abs(miniBearing) < .01 ? "MINIMAP · N↑ · TOP" : "MINIMAP · VIEW↑ · TOP");
    const yaw = Math.round(miniBearing);
    setText(document.querySelector("#worldLookHud [data-world-look-readout]"), h.ctl?.pad && h.ctl?.lookActive ? `AIM · ${yaw}°` : this.lookDragging ? `${yaw}°` : h.ctl?.keepLook ? `KEEP · ${yaw}°` : h.ctl?.lookSnapping ? "SNAP ↺" : "SNAP");
  }

  sticks(t, mode) {
    const pairs = mode === "drone" ? [["soloLeft", t.move, ".solo-knob"], ["soloRight", t.look, ".solo-knob"]]
      // the car drives with the foot sticks it claimed (shared_sticks.mjs): their knobs move
      : mode === "car" ? [["footMove", t.move, ".knob"], ["footLook", t.look, ".knob"], ["vehicleMove", t.move, ".knob"], ["vehicleLook", t.look, ".knob"]]
      : [["footMove", t.move, ".knob"], ["footLook", t.look, ".knob"]];
    // the right stick stands where the game has it (touch_play.cpp: 76 px off the edge, in every
    // mode): the page's CSS layers (thumb_layout.mjs) pin it to the edge, the finger would miss it
    const fw = $("game")?.clientWidth || 0;
    for (const id of ["footLook", "soloRight", "vehicleLook"]) {
      const el = $(id);
      if (!el || !fw || !t.look) continue;
      const right = `${Math.max(0, Math.round(fw - t.look.x - t.look.size))}px`;
      if (el.style.getPropertyValue("right") !== right) { el.style.setProperty("right", right, "important"); el.style.setProperty("left", "auto", "important"); }
    }
    // the jet's KANONE right over that stick, where the thumb reaches it (the original's CSS put
    // it left of a stick at the edge - now the jet's button row is there)
    const fire = $("footFire");
    if (fire && fw && t.look) {
      const fh = $("game")?.clientHeight || 0;
      const right = `${Math.max(0, Math.round(fw - t.look.x - t.look.size / 2 - (fire.offsetWidth || 64) / 2))}px`, bottom = `${Math.max(0, Math.round(fh - t.look.y + 8))}px`;
      if (fire.style.getPropertyValue("right") !== right) fire.style.setProperty("right", right, "important");
      if (fh && fire.style.getPropertyValue("bottom") !== bottom) fire.style.setProperty("bottom", bottom, "important");
    }
    for (const [id, s, sel] of pairs) {
      const el = $(id), k = el?.querySelector(sel);
      if (!el || !s) continue;
      el.classList.toggle("stick-live", !!s.live);
      if (k) { k.style.left = `${50 + s.kx * 31}%`; k.style.top = `${50 + s.ky * 31}%`; }
    }
  }

  sendRects(t, mode) {
    const canvas = $("game");
    if (!t || !canvas) { if (this.rectKey) { this.rectKey = ""; this.command("touch rects"); } return; }
    const cr = canvas.getBoundingClientRect(), rot = document.documentElement.classList.contains("sc-css-landscape");
    const out = [];
    const put = (id, el) => {
      if (!el || el.hidden || !el.getClientRects().length) return;
      const r = el.getBoundingClientRect();
      if (r.width < 2 || r.height < 2) return;
      // in the quarter-turned landscape the canvas frame is the turned one (host.mjs local())
      const x = rot ? r.top : r.left - cr.left, y = rot ? innerWidth - r.right : r.top - cr.top, w = rot ? r.height : r.width, hh = rot ? r.width : r.height;
      out.push(`${id} ${x.toFixed(0)} ${y.toFixed(0)} ${w.toFixed(0)} ${hh.toFixed(0)}`);
    };
    for (const b of t.buttons || []) {
      if (b.id === "enter") put("enter", $("enterCarButton"));
      else if (b.id === "cam") put("cam", mode === "car" ? $("vehicleCamButton") : document.querySelector('#jetHud [data-a="cam"]'));
      else if (JET_DOM[b.id]) put(b.id, document.querySelector(`#jetHud [data-a="${JET_DOM[b.id]}"]`));
      else if (TOUCH_DOM[b.id]) put(b.id, $(TOUCH_DOM[b.id]));
    }
    if (mode === "drone") put("clearance", $("soloHeightPad"));
    const key = out.join(";");
    if (key !== this.rectKey) { this.rectKey = key; this.command("touch rects " + key); }
  }
}
