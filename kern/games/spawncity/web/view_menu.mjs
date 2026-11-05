// SpawnCity on Kern - the original's menus on the original's DOM (view.mjs): the in-game MENU with
// its settings (control_settings.mjs and the settings modules: player_settings_sections.mjs,
// world_options_menu.mjs, camera_settings.mjs, audio_settings.mjs, mouse_settings.mjs, ...), and
// the quit question (quit_confirm.mjs). ui.mjs keeps deciding when a menu is open (the panel
// stack, pause, pad navigation); this module shows the original's dialog for it and writes the
// settings into the game (the same "set key value" and the same stored copy ui.mjs uses).
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
import {registerViewBinder} from "./view.mjs";

const VM_STORE = "spawnCityKernSettingsV1";  // ui.mjs SCUI_STORE
const load = () => { try { return JSON.parse(localStorage.getItem(VM_STORE) || "{}"); } catch { return {}; } };
const save = v => { try { localStorage.setItem(VM_STORE, JSON.stringify(v)); } catch { /* private mode */ } };

// The original's control -> the game's setting, and how the original prints the value.
const pct = v => `${Math.round(v)}%`, km = v => `${Math.round(v)} km/h`, m1 = v => `${(+v).toFixed(1)} m`, of10 = v => `${Math.round(v)}/10`, deg = v => `${Math.round(v)}°`;
const MAP = [
  ["[data-stereo-sbs]", "cam.stereo"],
  ...["people", "traffic", "dogs", "cats", "birds", "zombies", "police", "military", "flak", "jetpack", "friendlyFire"].map(k => [`[data-world-option="${k}"]`, `world.${k}`]),
  // player_settings_sections.mjs: drone, first person and vehicle profiles
  ['[data-setting="leftFineness"],[data-slider="left"]', "drone.leftFineness", of10],
  ['[data-setting="rightFineness"],[data-slider="right"]', "drone.rightFineness", of10],
  ['[data-setting="maxHorizontalSpeedKmh"],[data-slider="speed"]', "drone.maxSpeedKmh", km],
  ['[data-setting="defaultHoverAgl"],[data-slider="hover"]', "drone.hoverAgl", m1],
  ['[data-setting="invertLeftHorizontal"],[data-invert-left-horizontal]', "drone.invertRoll"],
  ['[data-setting="invertRightHorizontal"],[data-invert-right-horizontal]', "drone.invertYaw"],
  ['[data-setting="invertRightVertical"],[data-invert-right-vertical]', "drone.invertPitch"],
  ['[data-setting="moveFineness"]', "foot.moveFineness", of10],
  ['[data-setting="lookFineness"]', "foot.lookFineness", of10],
  ['[data-setting="horizontalLookSensitivityPercent"],[data-slider="look-horizontal"]', "foot.lookH", pct],
  ['[data-setting="verticalLookSensitivityPercent"],[data-slider="look-vertical"]', "foot.lookV", pct],
  ['[data-setting="lookDeadzonePercent"],[data-slider="look-deadzone"]', "foot.deadzone", pct],
  ['[data-setting="aimAssistStrengthPercent"],[data-slider="aim-assist"]', "foot.aimAssist", pct],
  ['[data-setting="invertMoveHorizontal"]', "foot.invertMoveH"],
  ['[data-setting="invertLookHorizontal"]', "foot.invertLookH"],
  ['[data-setting="invertLookVertical"]', "foot.invertLookV"],
  ['[data-setting="steeringSensitivityPercent"]', "car.steering", pct],
  ['[data-setting="throttleSensitivityPercent"]', "car.throttle", pct],
  ['[data-setting="maxSpeedKmh"]', "car.maxKmh", km],
  ['[data-setting="reverseSpeedKmh"]', "car.reverseKmh", km],
  ['[data-setting="cameraDistanceM"]', "car.camDistance", m1],
  ['[data-setting="cameraHeightM"]', "car.camHeight", v => `${(+v).toFixed(2)} m`],
  // camera_settings.mjs
  ['[data-camera-slider="tilt"]', "cam.fpvTilt", deg],
  ['[data-camera-slider="fov"]', "cam.fov", deg],
  ['[data-camera-slider="third"]', "cam.thirdDistance", m1],
  // audio_settings.mjs
  ["[data-audio-enabled]", "audio.on"],
  ["[data-music-enabled]", "music.on"],
  ['[data-audio-slider="music"]', "music.volume", pct],
  ['[data-audio-slider="drone"]', "audio.drone", pct],
  ['[data-audio-slider="shots"]', "audio.shots", pct],
  ['[data-audio-slider="fx"]', "audio.fx", pct],
  ['[data-audio-slider="footsteps"]', "audio.steps", pct],
  ['[data-audio-slider="vehicle"]', "audio.vehicle", pct],
  ['[data-audio-slider="ambient"]', "audio.ambient", pct],
  // real_world_bootstrap.mjs world settings, settings_help.mjs
  ["[data-world-grid],[data-debug-grid]", "view.grid"],
  ["[data-world-keep-look]", "view.keepLook"],
  ["[data-world-minimap-axis-lock]", "view.minimapNorth"],
  ["[data-box3d-collider-debug]", "debug.colliders"],
  // mouse_settings.mjs
  ["[data-mouse-sens]", "mouse.sensitivity", pct],
  ["[data-mouse-invert]", "mouse.invertY"],
  // xbox_control_mode.mjs
  ["[data-xbox-scheme]", "pad.aimScheme"],
];

export function installOriginalMenus({command, ui, mp}) {
  const dialog = document.querySelector("dialog.phone-settings-dialog");
  const quit = document.getElementById("quitConfirmDialog");
  quit?.remove();  // quit_confirm.mjs builds it when it asks and removes it after the answer
  const get = key => Number(command(`get ${key}`));
  const set = (key, value) => {
    command(`set ${key} ${value}`);
    const v = get(key), copy = load();
    copy[key] = v;
    save(copy);
    return v;
  };
  const outFor = input => {
    const row = input.closest(".player-control-row,.phone-settings-row");
    return row?.querySelector("output");
  };
  const fill = () => {
    if (!dialog) return;
    for (const [sel, key, fmt] of MAP)
      for (const input of dialog.querySelectorAll(sel)) {
        const v = get(key);
        if (!Number.isFinite(v)) continue;
        if (input.type === "checkbox") input.checked = v > 0.5;
        else if (input.tagName === "SELECT") input.selectedIndex = v > 0.5 ? 1 : 0;
        else input.value = String(v);
        const out = outFor(input);
        if (out && fmt) out.textContent = fmt(v);
      }
    const x = dialog.querySelectorAll('[data-setting="xboxControllerEnabled"],[data-xbox-controller]');
    let pad = false;
    try { pad = !!JSON.parse(command("controls")).padDrone; } catch { /* older game */ }
    for (const i of x) i.checked = pad;
  };
  if (dialog) {
    dialog.addEventListener("input", ev => {
      const input = ev.target;
      for (const [sel, key, fmt] of MAP)
        if (input.matches?.(sel)) {
          const v = set(key, input.type === "checkbox" ? (input.checked ? 1 : 0) : input.tagName === "SELECT" ? (input.selectedIndex > 0 ? 1 : 0) : input.value);
          const out = outFor(input);
          if (out && fmt) out.textContent = fmt(v);
          // the same setting shown twice (profile copies) follows
          for (const other of dialog.querySelectorAll(sel)) if (other !== input) { if (other.type === "checkbox") other.checked = v > 0.5; else other.value = String(v); }
          return;
        }
      if (input.matches?.('[data-setting="xboxControllerEnabled"],[data-xbox-controller]')) command(input.checked ? "xbox on" : "xbox off");
    });
    // control_settings.mjs / settings_help.mjs: the accordion sections open on a tap of their head
    dialog.addEventListener("click", ev => {
      const head = ev.target.closest(".sec-head");
      if (head) {
        const sec = head.closest(".collapsible"), open = head.getAttribute("aria-expanded") !== "true";
        head.setAttribute("aria-expanded", String(open));
        sec?.classList.toggle("open", open);
        return;
      }
      const a = ev.target.closest("[data-game-action],[data-close-top],[data-close],[data-mode],[data-reset]");
      if (!a) return;
      ev.preventDefault();
      const act = a.dataset.gameAction;
      if (a.hasAttribute("data-close-top") || a.hasAttribute("data-close") || act === "resume") closeAll();
      else if (act === "multi") { closeAll(); mp?.goMulti?.(); }
      else if (act === "reset") { closeAll(); command("reset"); }
      else if (act === "world") ui?.open?.("world");
      else if (act === "logbook") ui?.open?.("logbook");
      else if (act === "exit") { closeAll(); ui?.open?.("exit"); }
      else if (a.dataset.mode) { closeAll(); command(a.dataset.mode === "vehicle" ? "mode car" : `mode ${a.dataset.mode}`); }
      else if (a.hasAttribute("data-reset")) { for (const s of ["drohne", "fuss", "auto", "kamera", "ton", "maus"]) command(`settings reset ${s}`); save({}); fill(); }
    });
  }
  const kernPanels = () => ["scPause", "scSettings"].map(id => document.getElementById(id)).filter(Boolean);
  const closeAll = () => {
    for (let i = 0; i < 8 && kernPanels().some(p => !p.hidden); i++) ui?.open?.("close");
    if (dialog?.open) dialog.close();
  };
  // ui.mjs opens its PAUSE / EINSTELLUNGEN panel -> the original's MENU dialog
  const sync = () => {
    const want = kernPanels().some(p => !p.hidden);
    if (dialog) {
      if (want && !dialog.open) { fill(); try { dialog.showModal(); } catch { dialog.setAttribute("open", ""); } }
      else if (!want && dialog.open) dialog.close();
    }
    const q = document.getElementById("scQuit");
    const ask = q && !q.hidden;
    if (quit) {
      if (ask && !quit.isConnected) document.body.appendChild(quit);
      else if (!ask && quit.isConnected) quit.remove();
    }
  };
  dialog?.addEventListener("cancel", ev => { ev.preventDefault(); closeAll(); });  // Esc on the dialog
  quit?.querySelector("[data-yes]")?.addEventListener("click", () => document.querySelector("#scQuit [data-yes]")?.click());
  quit?.querySelector("[data-no]")?.addEventListener("click", () => document.querySelector("#scQuit [data-no]")?.click());
  for (const id of ["scPause", "scSettings", "scQuit"]) {
    const el = document.getElementById(id);
    if (el) new MutationObserver(sync).observe(el, {attributes: true, attributeFilter: ["hidden"]});
  }
  // the mode cards say what you control now (player_settings_sections.mjs renderModeCards)
  registerViewBinder("menus", h => {
    if (!dialog?.open) return;
    const cur = h.mode === "car" ? "vehicle" : h.mode === "drone" ? "drone" : "foot";
    for (const b of dialog.querySelectorAll(".player-mode-grid [data-mode]")) { const v = b.dataset.mode === cur ? "1" : "0"; if (b.dataset.active !== v) b.dataset.active = v; }
  });
  return {fill, closeAll};
}
