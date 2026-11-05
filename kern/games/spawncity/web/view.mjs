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
  for (const id of ["stage", "status"]) $(id)?.remove?.();
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
    html.sc-original-view #scMenu,html.sc-original-view .k-touch{display:none!important}
    html.sc-original-view #game{display:block;width:100%;height:100%;outline:none;touch-action:none}`;
  document.head.appendChild(extra);
  // The original's layout modules, unchanged (they read the walk mode through this name).
  const walk = {mode: "drone"};
  globalThis.__arondightWalkMode = walk;
  if (globalThis.__scViewLayouts) globalThis.__scViewLayouts();  // single file (build.sh)
  else {
    await import(base + "edge_hud_layout.mjs").catch(e => console.warn("[spawncity] edge layout", e));
    await import(base + "thumb_layout.mjs").catch(e => console.warn("[spawncity] thumb layout", e));
  }
  return true;
}

// The game's touch buttons (touch_play.cpp ids) and the original's elements that show them.
const TOUCH_DOM = {
  jump: "footJump", wpn: "thumbWeapon", scope: "scopeButton", nade: "handGrenadeBtn", strike: "airStrikeButton",
  mode: "mobileGameplayMode", jet: "enterJetButton", emp: "wantedEmpButton", arm: "soloArm", stop: "soloKill",
  hb: "vehicleBrakeButton", radio: "carRadioButton",
};
const JET_DOM = {vtol: "mode", rkt: "rocket", bomb: "bomb", cam: "cam", eject: "exit"};
// Original elements a finger uses as page buttons (their own click), never as game input.
const PAGE_BUTTONS = "#gameExitButton,#mobileGameplayDock,#mobileGameplayMultiplayer,#mobileGameplayReset,#proximityVoiceBar,#worldLookHud,#gpsLiveButton,#musicSwitch,#soundSwitch,#gameplayContractHud,#vsRespawnHud,#gpsLiveHud,#zombieRepair,dialog,#gameMenu,#scExitAsk";

/** A finger on the game view goes to the game unless it lands on a page button. */
export function fingerForGame(ev) {
  const t = ev.target instanceof Element ? ev.target : null;
  if (!t || !t.closest("#viewport")) return false;
  return !t.closest(PAGE_BUTTONS);
}

const FOOT_NAMES = {MP: "MP", Glock: "GLOCK", Sniper: "SNIPER", Granatwerfer: "WERFER", Raketenwerfer: "RAKETEN", "Fäuste": "FÄUSTE", Messer: "MESSER"};
const FOOT_ORDER = ["MP", "GLOCK", "SNIPER", "WERFER", "RAKETEN", "FÄUSTE", "MESSER"];  // first_person_weapon_runtime_v3.mjs WEAPON_ORDER
const DRONE_NAMES = {MG: "GUN", RAKETEN: "ROCKETS", GUIDED: "GUIDED"};                  // drone_weapon_modes.mjs

export class OriginalView {
  constructor({command, ui, mp}) {
    this.command = command;
    this.ui = ui;
    this.mp = mp;
    this.rectKey = "";
    this.wire();
  }

  wire() {
    const click = (id, fn) => $(id)?.addEventListener("click", ev => { ev.preventDefault(); ev.stopPropagation(); fn(ev); });
    const kernAct = act => document.querySelector(`#scMenu [data-act="${act}"]`)?.click();
    // title menu (drone_simulator.html #gameMenu; auto_flight_start.mjs START)
    click("gameMenuStart", () => $("scStart")?.click());
    click("gameMenuWorld", () => kernAct("world"));
    click("gameMenuChangelog", () => kernAct("changelog"));
    click("gameMenuShare", () => kernAct("share"));
    click("startPlaceButton", () => $("scPlace")?.click());
    // the game view's top bar and dock (mobile_gameplay_ui.mjs, game_exit_button.mjs)
    click("gameExitButton", () => this.ui?.open?.("exit"));
    click("mobileGameplaySettings", () => this.ui?.open?.("settings"));
    click("mobileGameplayReset", () => this.command("reset"));
    click("mobileGameplayWeapon", () => this.command("press wpn"));
    click("mobileGameplayMode", () => this.command("press mode"));
    click("mobileGameplayMultiplayer", () => this.mp?.goMulti?.());
    click("enterCarButton", () => this.command("press enter"));
    click("enterJetButton", () => this.command("press jet"));
    click("soloArm", () => this.command("press arm"));
    click("soloKill", () => this.command("press stop"));
    click("gpsLiveButton", () => kernAct("gps"));
    click("proximityVoicePtt", () => this.command("voice mic"));
    click("proximityVoiceMute", () => this.command("voice mute"));
    for (const b of document.querySelectorAll("#worldLookHud [data-map-expand]")) b.addEventListener("click", ev => { ev.preventDefault(); ev.stopPropagation(); this.command("bigmap toggle"); });
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
    cls("on-foot-mode", mode === "foot" || mode === "car");
    cls("player-driving", mode === "car");
    cls("jet-mode", mode === "jet" || mode === "parachute");
    cls("desktop-input", desktop);
    cls("pad-input", !!ctl.pad);
    cls("mobile-gameplay-compact", true);
    cls("player-dead", !!ctl.dead);
    globalThis.__arondightWalkMode.mode = mode === "drone" ? "drone" : "foot";
    // ---- title menu (drone_simulator.html, auto_flight_start.mjs, start_location.mjs)
    const menu = $("gameMenu");
    setHidden(menu, playing);
    const start = $("gameMenuStart");
    if (start) { const ready = !!h.placed; start.disabled = !ready; setText(start, ready ? "START" : "LOADING"); }
    setText($("gameMenuStatus"), h.placed ? (h.offline ? "OFFLINE" : "READY") : "");
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
    // ---- touch buttons: the game's hit areas follow what the original's layout drew
    this.sendRects(touch, mode);
  }

  sticks(t, mode) {
    const pairs = mode === "drone" ? [["soloLeft", t.move, ".solo-knob"], ["soloRight", t.look, ".solo-knob"]]
      : mode === "car" ? [["vehicleMove", t.move, ".knob"], ["vehicleLook", t.look, ".knob"]]
      : [["footMove", t.move, ".knob"], ["footLook", t.look, ".knob"]];
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
