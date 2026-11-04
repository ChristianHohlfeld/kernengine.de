// SpawnCity on Kern - menus (DOM only, no game logic): the title screen (START, WELT, EINSTELLUNGEN,
// NEU / CHANGELOG, IDEE), the pause menu, the settings dialog built from the game's own settings
// table (C++ `settings schema`), the WORLD options, "Zurueck zum Hauptmenue?", RESET, the corner
// buttons of the HUD, a suggestion card and the installed-app / fullscreen policy.
// Every change goes to the game as a command ("set <key> <value>"); the page keeps a copy in
// localStorage only as a convenience and replays it at start.
// Sources (spawncity-de/sim): settings_help.mjs, settings_controller_nav.mjs, player_settings_sections.mjs,
// control_settings.mjs, mouse_settings.mjs, camera_settings.mjs, audio_settings.mjs, world_options.mjs,
// world_options_menu.mjs, changelog_menu.mjs, quit_confirm.mjs, game_exit_button.mjs, game_reset.mjs,
// suggestion_box.mjs, neon_ui_theme.mjs, edge_hud_layout.mjs, mobile_gameplay_ui.mjs, pwa_fullscreen_policy.mjs.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).

const SCUI_STORE = "spawnCityKernSettingsV1";
const SCUI_SUGGEST = "spawnCityKernSuggestV1";
const SCUI_PLACE = "spawnCityKernStartPlaceV1", SCUI_RECENT = "spawnCityKernStartPlaceRecentV1";
const SCUI_LOGBOOK = "spawnCityKernLogbookV1";
const SCUI_PHOTON = "https://photon.komoot.io/api/";
// world_location_selector.mjs WORLD_LOCATION_PRESETS (+ Konstanz, the game's home)
const SCUI_PRESETS = [["Konstanz", 47.6600, 9.1766], ["New York", 40.7128, -74.0060], ["Berlin", 52.5200, 13.4050], ["Zürich", 47.3769, 8.5417],
  ["London", 51.5074, -0.1278], ["Paris", 48.8566, 2.3522], ["Tokyo", 35.6762, 139.6503], ["Los Angeles", 34.0522, -118.2437], ["Singapore", 1.3521, 103.8198],
  ["Dubai", 25.2048, 55.2708], ["Sydney", -33.8688, 151.2093], ["Rio de Janeiro", -22.9068, -43.1729]];
const SCUI_SECTIONS = [["maus", "MAUS"], ["fuss", "ZU FUSS"], ["drohne", "DROHNE"], ["auto", "AUTO"], ["kamera", "KAMERA"], ["ton", "TON"]];

// changelog_menu.mjs: newest first, one line a player understands, only what is new in that update.
const SCUI_CHANGELOG = [
  {date: "2026-11-04", title: "Meilenstein 3: Menues und Ausstattung", items: [
    "Titelbildschirm mit START, WELT, EINSTELLUNGEN, NEU / CHANGELOG; ESC oder MENUE pausiert.",
    "Einstellungen wie auf spawncity.de: Maus, zu Fuss, Drohne, Auto, Kamera, Ton – sofort wirksam.",
    "WELT: Leute, Verkehr, Polizei, Jetpack und Friendly Fire an- und ausschalten.",
    "RESET bringt alles zurueck an den Start; EXIT fragt vorher nach."]},
  {date: "2026-11-04", title: "Meilenstein 2: Stadt, Polizei, Waffen, Mehrspieler", items: [
    "Passanten, Streifenwagen mit Beamten, Polizeidrohnen und Fahndung mit Fluchtuhr.",
    "Das ganze Waffenset mit Granaten, Raketen, Explosionen und Fahrzeugschaden; EMP auf X.",
    "Mehrspieler ueber Kern Client/Server, Minimap und Strassenlaternen."]},
  {date: "2026-10-29", title: "Meilenstein 1: SpawnCity laeuft nativ auf Kern", items: [
    "Drohne mit der echten Arondight45-Firmware, Jet, Autos, Bus und Fallschirm auf Box3D.",
    "Die echte Stadt aus OpenStreetMap-Vektorkacheln, ohne Netz eine prozedurale Ersatzstadt.",
    "Tastatur, Maus, Xbox-Pad und Touch."]},
];

const SCUI_CSS = `
/* neon_ui_theme.mjs: chunky rounded buttons, translucent navy panels, white bold text, one warm accent */
.scui-btn{min-height:40px;padding:10px 18px;border-radius:10px;border:1px solid #ffffff40;background:#ffffff14;color:#eef0f2;font:800 13px/1 Inter,system-ui,sans-serif;letter-spacing:.2em;cursor:pointer;text-transform:uppercase}
.scui-btn:hover,.scui-btn:focus-visible{background:#ffffff26;outline:none}
.scui-btn:focus-visible,body.scui-pad .scui-btn:focus,body.scui-pad .scui-row:focus,body.scui-pad input:focus{outline:3px solid #ffd76a;outline-offset:2px}
.scui-hot{background:linear-gradient(180deg,#ffb02e,#ff7a1a);color:#16120c;border:0}
#scMenu{position:fixed;inset:0;z-index:50;display:flex;align-items:safe center;justify-content:safe center;overflow-y:auto;background:radial-gradient(ellipse at 50% 40%,#0b0d10a8,#0b0d10f0);color:#eef0f2;font-family:Inter,system-ui,sans-serif;touch-action:manipulation}
#scMenu[hidden],.scui-panel[hidden],#scQuit[hidden]{display:none!important}
#scMenu .gm-center{display:flex;flex-direction:column;align-items:center;gap:clamp(8px,2vh,16px);padding:24px;margin:auto;text-align:center}
#scMenu .gm-title{position:relative;display:flex;flex-direction:column;align-items:center;font-weight:900;font-style:italic;font-size:clamp(34px,min(10vw,15vh),120px);letter-spacing:.02em;line-height:.9;color:#f4f5f6;text-shadow:0 3px 0 rgba(0,0,0,.55),0 10px 30px rgba(0,0,0,.6);padding:0 .08em .16em}
#scMenu .gm-title::after{content:"";position:absolute;left:4%;right:4%;bottom:.02em;height:.09em;background:linear-gradient(90deg,transparent,#ff7a1a 18%,#ffb02e 60%,transparent);transform:skewX(-24deg);box-shadow:0 0 18px rgba(255,122,26,.55)}
#scMenu .gm-sub{font-weight:700;font-size:clamp(11px,1.6vw,15px);letter-spacing:.42em;color:rgba(238,240,242,.78)}
#scStart{min-width:min(64vw,300px);padding:15px 28px;font:700 clamp(17px,2.4vw,22px) Inter,system-ui,sans-serif;letter-spacing:.32em;color:#16120c;background:linear-gradient(180deg,#ffb02e,#ff7a1a);border:0;border-radius:4px;box-shadow:0 6px 18px rgba(0,0,0,.55),inset 0 -3px 0 rgba(0,0,0,.18);cursor:pointer;clip-path:polygon(4% 0,100% 0,96% 100%,0 100%)}
#scStart[disabled]{opacity:.7;cursor:default;filter:saturate(.4)}
#scMenu .gm-wide{min-width:min(64vw,300px)}
#scMenuStatus{min-height:1.2em;font-size:11px;font-weight:700;letter-spacing:.3em;color:rgba(238,240,242,.62)}
.scui-panel{position:fixed;inset:0;z-index:60;display:flex;align-items:center;justify-content:center;background:#0b0d10d0;backdrop-filter:blur(4px);font-family:Inter,system-ui,sans-serif;color:#eef0f2}
.scui-card{width:min(94vw,540px);max-height:calc(100dvh - 32px);overflow:auto;padding:16px 16px 12px;border:1px solid #ffffff2e;border-radius:14px;background:#12161ce8;text-align:left;box-shadow:0 12px 40px #000a}
.scui-card h2{margin:0 0 4px;font:900 italic 22px/1 Inter,system-ui,sans-serif;letter-spacing:.06em;color:#ffb02e}
.scui-card .scui-sub{margin:0 0 12px;font:600 12px/1.35 system-ui,sans-serif;color:#b9c3cc}
.scui-tabs{display:flex;flex-wrap:wrap;gap:6px;margin:0 0 10px}
.scui-tabs button{padding:7px 10px;border-radius:8px;border:1px solid #ffffff30;background:#162437;color:#fff;font:850 11px/1 system-ui;letter-spacing:.1em;cursor:pointer}
.scui-tabs button[aria-selected="true"]{border-color:#ffb02e;background:#4a3212}
.scui-row{display:grid;grid-template-columns:1fr auto;align-items:center;gap:4px 10px;width:100%;margin:0 0 6px;padding:9px 11px;border:1px solid #ffffff1f;border-radius:10px;background:#ffffff0a;color:#eef0f2;box-sizing:border-box;font:inherit;text-align:left}
.scui-row b{font:850 13px/1.1 system-ui,sans-serif;letter-spacing:.06em}
.scui-row small{grid-column:1/-1;font:500 11.5px/1.3 system-ui,sans-serif;color:#aab4be}
.scui-row output{font:900 12px ui-monospace,SFMono-Regular,Menlo,monospace;color:#ffd36a}
.scui-row input[type=range]{grid-column:1/-1;width:100%;accent-color:#ff8a1f}
.scui-row.scui-toggle{cursor:pointer}
.scui-row.scui-toggle i{width:44px;height:24px;border-radius:12px;background:#3a414b;position:relative}
.scui-row.scui-toggle i::after{content:"";position:absolute;top:3px;left:3px;width:18px;height:18px;border-radius:50%;background:#dfe5ea}
.scui-row.scui-toggle[aria-checked="true"] i{background:#ff8a1f}.scui-row.scui-toggle[aria-checked="true"] i::after{left:23px}
.scui-row.scui-soon{opacity:.55}
.scui-actions{display:flex;flex-wrap:wrap;gap:8px;margin-top:10px}.scui-actions .scui-btn{flex:1}
.scui-log article{padding:9px 11px;margin:0 0 8px;border:1px solid #ffffff1f;border-radius:10px;background:#ffffff0a}
.scui-log time{font:750 10px ui-monospace,monospace;color:#91a7bc}.scui-log b{display:block;margin:2px 0 4px;font:850 13px system-ui}
.scui-log li{font:500 12px/1.4 system-ui;color:#d5dde5}
#scQuit{position:fixed;inset:0;z-index:70;display:flex;align-items:center;justify-content:center;background:#0006}
#scQuit .qc-box{background:#11151bf2;border:1px solid #ffffff33;border-radius:14px;padding:18px 22px;color:#fff;font:600 14px/1.4 Inter,system-ui,sans-serif;min-width:260px;text-align:center;box-shadow:0 12px 40px #000a}
#scQuit b{display:block;font:900 17px/1.2 Inter,system-ui,sans-serif;letter-spacing:.08em;margin-bottom:12px}
#scQuit .qc-row{display:flex;gap:12px;justify-content:center}#scQuit button{min-width:104px}
#scQuit small{display:block;margin-top:10px;opacity:.7;font-size:11px}
/* game_exit_button.mjs / mobile_gameplay_ui.mjs: EXIT, MENU, RESET top left, in reach and off the picture */
.scui-corner{position:fixed;z-index:20;left:max(10px,env(safe-area-inset-left));top:max(10px,env(safe-area-inset-top));display:flex;gap:6px}
.scui-corner button{min-height:30px;padding:6px 10px;border-radius:8px;border:1px solid #ffffff2a;background:#0a0d12a0;color:#fff;font:800 11px/1 Inter,system-ui,sans-serif;letter-spacing:.12em;cursor:pointer}
.scui-corner[hidden]{display:none}
body.scui-playing .sc-top{top:calc(max(12px,env(safe-area-inset-top)) + 38px)}
#scPlace{display:flex;align-items:center;gap:8px;max-width:min(86vw,420px);padding:8px 14px;border-radius:999px;border:1px solid #ffffff40;background:#0a0d12b0;color:#fff;font:700 12px/1.2 Inter,system-ui,sans-serif;letter-spacing:.06em;cursor:pointer}
#scPlace b{overflow:hidden;text-overflow:ellipsis;white-space:nowrap}#scPlace i{font-style:normal;color:#ffb02e;font-size:10px;letter-spacing:.14em}
#scPlacePanel input{width:100%;box-sizing:border-box;margin:0 0 10px;padding:11px 12px;border-radius:10px;border:1px solid #ffffff38;background:#0c1118;color:#fff;font:600 14px system-ui}
#scPlacePanel .sp-item{display:block;width:100%;margin:0 0 6px;padding:9px 12px;border-radius:10px;border:1px solid #ffffff1f;background:#ffffff0a;color:#eef0f2;text-align:left;cursor:pointer;font:inherit}
#scPlacePanel .sp-item b{display:block;font:800 13px/1.2 system-ui}#scPlacePanel .sp-item small{color:#aab4be;font:500 11.5px/1.3 system-ui}
#scPlacePanel .sp-on{border-color:#ffb02e}#scPlacePanel h4{margin:10px 0 6px;font:900 10px system-ui;letter-spacing:.14em;color:#8fe8ff}
#scGps{position:fixed;z-index:21;left:50%;top:calc(max(10px,env(safe-area-inset-top)) + 46px);transform:translateX(-50%);display:flex;gap:12px;align-items:center;padding:8px 12px;border-radius:12px;background:#0d1117d9;border:1px solid #4285f4aa;color:#fff;font:700 12px/1.2 Inter,system-ui,sans-serif}
#scGps[hidden]{display:none}#scGps b{font-size:15px;color:#9cc2ff}#scGps span{color:#b8c2cc}#scGps button{padding:6px 9px;border-radius:8px;border:1px solid #ffffff38;background:#162437;color:#fff;font:800 10px system-ui;letter-spacing:.1em;cursor:pointer}
#scBanner{position:fixed;z-index:21;left:50%;top:calc(max(10px,env(safe-area-inset-top)) + 54px);transform:translateX(-50%);padding:8px 16px 7px;border-radius:12px;background:#0d1117d9;border:1px solid #ffffff2a;color:#fff;text-align:center;pointer-events:none;transition:opacity .35s;max-width:min(80vw,420px)}
#scBanner b{display:block;font:900 15px/1.15 Inter,system-ui,sans-serif;letter-spacing:.05em;color:#ffd36a;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}#scBanner small{font:700 11px/1.3 system-ui;color:#b8c2cc}
#scRadio{position:fixed;z-index:21;right:max(14px,env(safe-area-inset-right));top:calc(max(12px,env(safe-area-inset-top)) + 160px);width:44px;height:44px;border-radius:50%;border:1px solid #ffffff38;background:#0a0d12b0;font-size:20px;cursor:pointer}
#scRadio[hidden],#scJet[hidden]{display:none}
#scJet{position:fixed;z-index:19;left:50%;bottom:calc(max(34px,env(safe-area-inset-bottom) + 24px) + 12px);transform:translateX(-50%);width:min(160px,30vw);height:4px;border-radius:2px;background:#ffffff26;overflow:hidden}
#scJet i{display:block;height:100%;background:linear-gradient(90deg,#ff8a2a,#ffd36a)}#scJet.burn i{background:#ff5a2a}
.scui-log .lb-stats{font:600 11.5px/1.4 ui-monospace,monospace;color:#c7d2dc}
#scIdea textarea{width:100%;min-height:70px;box-sizing:border-box;border-radius:8px;border:1px solid #ffffff33;background:#0c1118;color:#fff;font:500 13px/1.4 system-ui;padding:8px}
`;

const scuiEsc = s => String(s ?? "").replace(/[&<>"]/g, c => ({"&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;"}[c]));
function scuiLoad(key, fallback) { try { return JSON.parse(localStorage.getItem(key) || "null") ?? fallback; } catch { return fallback; } }
function scuiSave(key, value) { try { localStorage.setItem(key, JSON.stringify(value)); } catch { /* private mode */ } }

/** Installs the menus. `command` sends a command to the game and returns its answer. */
export function installSpawnCityUi({command, params, canvas, statusEl}) {
  const style = document.createElement("style");
  style.textContent = SCUI_CSS;
  document.head.appendChild(style);
  const coarse = matchMedia("(pointer: coarse)").matches;
  const ui = {open: [], playing: false, started: false, schema: [], tab: "maus", lockWanted: false, playSeconds: 0};

  // ---- settings: replay the stored copy, then read the table back -------------------------
  const stored = scuiLoad(SCUI_STORE, {});
  const pairs = Object.entries(stored).filter(([k, v]) => /^[a-zA-Z.]+$/.test(k) && Number.isFinite(+v)).map(([k, v]) => `${k} ${+v}`);
  if (pairs.length) command("set " + pairs.join(";"));
  const readSchema = () => { try { ui.schema = JSON.parse(command("settings schema")); } catch { ui.schema = []; } };
  readSchema();
  const setOpt = (key, value) => {
    command(`set ${key} ${value}`);
    const row = ui.schema.find(o => o.key === key);
    if (row) row.value = Number(command(`get ${key}`));
    const copy = scuiLoad(SCUI_STORE, {});
    copy[key] = row ? row.value : value;
    scuiSave(SCUI_STORE, copy);
    return row ? row.value : value;
  };

  // ---- DOM ---------------------------------------------------------------------------------
  const el = (html) => { const t = document.createElement("template"); t.innerHTML = html.trim(); return t.content.firstElementChild; };
  const menu = el(`<div id="scMenu" role="dialog" aria-label="SpawnCity">
    <div class="gm-center">
      <div class="gm-title" aria-label="SpawnCity"><span>SPAWN</span><span>CITY</span></div>
      <div class="gm-sub">DROHNE · AUTOS · JET</div>
      <button id="scStart" type="button" disabled>LADEN</button>
      <button id="scPlace" type="button" data-act="place"><span>📍</span><b>MEIN STANDORT (GPS)</b><i>ÄNDERN</i></button>
      <div id="scMenuStatus">STADT WIRD VORBEREITET</div>
      <button class="scui-btn gm-wide" data-act="world" type="button">WELT</button>
      <button class="scui-btn gm-wide" data-act="settings" type="button">EINSTELLUNGEN</button>
      <button class="scui-btn gm-wide" data-act="changelog" type="button">NEU / CHANGELOG</button>
    </div></div>`);
  const pause = el(`<div class="scui-panel" id="scPause" hidden><div class="scui-card"><h2>PAUSE</h2><p class="scui-sub">Das Spiel steht, solange dieses Menue offen ist.</p>
    <div class="scui-actions" style="flex-direction:column">
      <button class="scui-btn scui-hot" data-act="resume" type="button">WEITER</button>
      <button class="scui-btn" data-act="settings" type="button">EINSTELLUNGEN</button>
      <button class="scui-btn" data-act="world" type="button">WELT</button>
      <button class="scui-btn" data-act="reset" type="button">RESET</button>
      <button class="scui-btn" data-act="gps" type="button">GPS LIVE</button>
      <button class="scui-btn" data-act="logbook" type="button">LOGBUCH</button>
      <button class="scui-btn" data-act="exit" type="button">HAUPTMENUE</button>
    </div></div></div>`);
  const settings = el(`<div class="scui-panel" id="scSettings" hidden><div class="scui-card"><h2>EINSTELLUNGEN</h2>
    <p class="scui-sub">Jede Aenderung wirkt sofort. Unter jeder Zeile steht, wofuer sie gut ist.</p>
    <div class="scui-tabs" role="tablist"></div><div class="scui-rows"></div>
    <div class="scui-actions"><button class="scui-btn" data-act="reset-section" type="button">STANDARD</button><button class="scui-btn scui-hot" data-act="close" type="button">FERTIG</button></div></div></div>`);
  const world = el(`<div class="scui-panel" id="scWorld" hidden><div class="scui-card"><h2>WELT</h2>
    <p class="scui-sub">Wer in der Stadt lebt und was du kannst. Ausgeschaltetes gibt es nicht: keine Koerper, keine KI, nichts zu zeichnen.</p>
    <div class="scui-rows"></div><div class="scui-actions"><button class="scui-btn scui-hot" data-act="close" type="button">FERTIG</button></div></div></div>`);
  const log = el(`<div class="scui-panel" id="scChangelog" hidden><div class="scui-card"><h2>NEU</h2><p class="scui-sub">Die wichtigsten Punkte jedes Updates, das neueste zuerst.</p>
    <div class="scui-log">${SCUI_CHANGELOG.map(e => `<article><time>${e.date}</time><b>${scuiEsc(e.title)}</b><ul>${e.items.map(i => `<li>${scuiEsc(i)}</li>`).join("")}</ul></article>`).join("")}</div>
    <div class="scui-actions"><button class="scui-btn scui-hot" data-act="close" type="button">SCHLIESSEN</button></div></div></div>`);
  const quit = el(`<div id="scQuit" role="dialog" aria-label="Zurueck zum Hauptmenue" hidden><div class="qc-box"><b>ZURUECK ZUM HAUPTMENUE?</b>
    <div class="qc-row"><button class="scui-btn scui-hot" data-yes type="button">JA</button><button class="scui-btn" data-no type="button">NEIN</button></div>
    <small>Enter / Y = ja · Esc / N = nein</small></div></div>`);
  const idea = el(`<div class="scui-panel" id="scIdea" hidden><div class="scui-card"><h2>DEINE IDEE?</h2>
    <p class="scui-sub">Was soll das naechste Update bringen? Eine kurze Zeile (hoechstens 240 Zeichen). Sie wird nicht automatisch verschickt: KOPIEREN legt sie in die Zwischenablage, GITHUB oeffnet einen Vorschlag im oeffentlichen Repository.</p>
    <textarea maxlength="240" aria-label="Deine Idee"></textarea>
    <div class="scui-actions"><button class="scui-btn" data-act="copy" type="button">KOPIEREN</button><button class="scui-btn" data-act="github" type="button">GITHUB</button><button class="scui-btn scui-hot" data-act="close" type="button">SPAETER</button></div></div></div>`);
  const corner = el(`<div class="scui-corner" hidden><button type="button" data-act="exit" aria-label="Zurueck zum Hauptmenue">EXIT</button>
    <button type="button" data-act="pause" aria-label="Menue">MENUE</button><button type="button" data-act="reset" aria-label="Zurueck zum Start">RESET</button></div>`);
  const placePanel = el(`<div class="scui-panel" id="scPlacePanel" role="dialog" aria-label="Startort waehlen" hidden><div class="scui-card"><h2>WO WILLST DU SPIELEN?</h2>
    <input type="search" placeholder="Stadt, Strasse oder Adresse ..." autocomplete="off" enterkeyhint="search" aria-label="Ort suchen">
    <div class="sp-list"></div><small class="scui-sub">Suche: © OpenStreetMap-Mitwirkende (Photon von komoot)</small>
    <div class="scui-actions"><button class="scui-btn scui-hot" data-act="close" type="button">SCHLIESSEN</button></div></div></div>`);
  const gpsCard = el(`<div id="scGps" hidden><b data-k="act">🧍 STEHT</b><span data-k="speed">0 km/h</span><span data-k="dist">0 m</span><span data-k="time">0:00</span>
    <span data-k="acc">± – m</span><button type="button" data-act="gps-view">AUGE</button><button type="button" data-act="gps">STOP</button></div>`);
  const logbook = el(`<div class="scui-panel" id="scLogbook" hidden><div class="scui-card"><h2>LOGBUCH</h2>
    <p class="scui-sub">Jeder Drohnenflug von ARM bis DISARM und die Netzereignisse. Nur auf diesem Geraet gespeichert; fliesst nie in Flugregler oder Physik.</p>
    <div class="scui-log" data-k="flights"></div><h4 class="scui-sub">NETZ / MEHRSPIELER</h4><div class="scui-log" data-k="events"></div>
    <div class="scui-actions"><button class="scui-btn" data-act="lb-export" type="button">EXPORT JSON</button><button class="scui-btn" data-act="lb-clear" type="button">LEEREN</button>
    <button class="scui-btn scui-hot" data-act="close" type="button">SCHLIESSEN</button></div></div></div>`);
  const banner = el(`<div id="scBanner" style="opacity:0"><b></b><small></small></div>`);
  const radioBtn = el(`<button id="scRadio" type="button" data-act="radio" aria-label="Autoradio: naechster Sender" hidden>📻</button>`);
  const jetBar = el(`<div id="scJet" hidden><i></i></div>`);
  for (const n of [menu, pause, settings, world, log, quit, idea, corner, placePanel, gpsCard, logbook, banner, radioBtn, jetBar]) document.body.appendChild(n);
  const startBtn = menu.querySelector("#scStart"), menuStatus = menu.querySelector("#scMenuStatus");

  // ---- open / close: a stack of panels; the game pauses while anything is open ---------------
  const visible = () => !menu.hidden || ui.open.length > 0 || !quit.hidden;
  function sync() {
    const paused = visible();
    command(`pause ${paused ? 1 : 0}`);
    corner.hidden = !ui.started || !menu.hidden;
    document.body.classList.toggle("scui-playing", ui.started && menu.hidden);
    document.body.classList.toggle("scui-menu", paused);
  }
  function openPanel(p) {
    if (!ui.open.includes(p)) ui.open.push(p);
    p.hidden = false;
    focusFirst(p);
    sync();
  }
  function closeTop() {
    const p = ui.open.pop();
    if (p) p.hidden = true;
    if (ui.open.length) focusFirst(ui.open[ui.open.length - 1]);
    else if (!menu.hidden) focusFirst(menu);
    sync();
    if (!visible()) relock();
  }
  function relock() { if (!coarse && ui.lockWanted) try { canvas.requestPointerLock?.(); } catch { /* no lock */ } canvas.focus?.(); }
  function focusables(root) {
    return Array.from(root.querySelectorAll("button:not([disabled]),input,textarea,[tabindex='0']")).filter(n => n.offsetParent !== null);
  }
  function focusFirst(root) { const f = focusables(root)[0]; f?.focus?.({preventScroll: false}); }

  // ---- settings dialog -----------------------------------------------------------------------
  function rowHtml(o) {
    const value = o.toggle ? "" : `<output>${fmt(o)}</output>`;
    if (o.toggle) return `<div class="scui-row scui-toggle${o.live ? "" : " scui-soon"}" role="switch" tabindex="0" data-key="${o.key}" aria-checked="${o.value > 0.5}">
      <b>${scuiEsc(o.label)}${o.live ? "" : " · BALD"}</b><i></i><small>${scuiEsc(o.help)}${o.live ? "" : " (Noch nicht auf Kern: der Schalter wird gespeichert.)"}</small></div>`;
    return `<div class="scui-row" data-key="${o.key}"><b>${scuiEsc(o.label)}</b>${value}
      <input type="range" min="${o.min}" max="${o.max}" step="${o.step}" value="${o.value}" aria-label="${scuiEsc(o.label)}"><small>${scuiEsc(o.help)}</small></div>`;
  }
  function fmt(o) {
    const k = o.key, v = o.value;
    if (/sensitivity|look[HV]|steering|throttle|deadzone/.test(k) || o.section === "ton") return `${Math.round(v)} %`;
    if (/Kmh/.test(k)) return `${Math.round(v)} km/h`;
    if (/fov|Tilt/.test(k)) return `${Math.round(v)}°`;
    if (/Distance|Height|hoverAgl/.test(k)) return `${(+v).toFixed(1)} m`;
    return String(Math.round(v * 10) / 10);
  }
  function renderSettings() {
    const tabs = settings.querySelector(".scui-tabs"), rows = settings.querySelector(".scui-rows");
    tabs.innerHTML = SCUI_SECTIONS.map(([id, label]) => `<button type="button" role="tab" data-tab="${id}" aria-selected="${ui.tab === id}">${label}</button>`).join("");
    rows.innerHTML = ui.schema.filter(o => o.section === ui.tab).map(rowHtml).join("");
  }
  function renderWorld() { world.querySelector(".scui-rows").innerHTML = ui.schema.filter(o => o.section === "welt").map(rowHtml).join(""); }
  function bindRows(root, render) {
    root.addEventListener("input", ev => {
      const input = ev.target.closest("input[type=range]"), row = ev.target.closest(".scui-row");
      if (!input || !row) return;
      const o = ui.schema.find(x => x.key === row.dataset.key);
      if (!o) return;
      o.value = setOpt(o.key, input.value);
      row.querySelector("output").textContent = fmt(o);
    });
    const flip = row => {
      const o = ui.schema.find(x => x.key === row.dataset.key);
      if (!o || !o.toggle) return;
      o.value = setOpt(o.key, o.value > 0.5 ? 0 : 1);
      row.setAttribute("aria-checked", String(o.value > 0.5));
    };
    root.addEventListener("click", ev => {
      const tab = ev.target.closest("[data-tab]");
      if (tab) { ui.tab = tab.dataset.tab; render(); focusFirst(root.querySelector(".scui-rows") || root); return; }
      const row = ev.target.closest(".scui-toggle");
      if (row) flip(row);
    });
    root.addEventListener("keydown", ev => {
      const row = ev.target.closest?.(".scui-toggle");
      if (row && (ev.key === "Enter" || ev.key === " ")) { ev.preventDefault(); flip(row); }
    });
  }
  bindRows(settings, renderSettings);
  bindRows(world, renderWorld);

  // ---- quit confirm (quit_confirm.mjs) --------------------------------------------------------
  function askQuit() { quit.hidden = false; focusFirst(quit); sync(); }
  function answerQuit(yes) {
    quit.hidden = true;
    if (yes) { while (ui.open.length) closeTop(); showTitle(); }
    sync();
    if (!yes && !visible()) relock();
  }
  quit.querySelector("[data-yes]").onclick = () => answerQuit(true);
  quit.querySelector("[data-no]").onclick = () => answerQuit(false);

  // ---- actions -------------------------------------------------------------------------------
  function act(name) {
    switch (name) {
      case "world": renderWorld(); openPanel(world); break;
      case "settings": readSchema(); renderSettings(); openPanel(settings); break;
      case "changelog": openPanel(log); break;
      case "pause": openPanel(pause); break;
      case "resume": closeTop(); break;
      case "close": closeTop(); break;
      case "exit": askQuit(); break;
      case "reset": while (ui.open.length) closeTop(); command("reset"); relock(); break;
      case "reset-section": command(`settings reset ${ui.tab}`); { const copy = scuiLoad(SCUI_STORE, {}); for (const o of ui.schema) if (o.section === ui.tab) delete copy[o.key]; scuiSave(SCUI_STORE, copy); } readSchema(); renderSettings(); break;
      case "place": renderPlaces([]); openPanel(placePanel); placePanel.querySelector("input").focus(); break;
      case "gps": if (gpsLive.on) stopGps(); else { while (ui.open.length) closeTop(); startGps(); } break;
      case "gps-view": command("gps view next"); break;
      case "logbook": syncLogbook(); renderLogbook(); openPanel(logbook); break;
      case "radio": command("radio next"); break;
      case "lb-export": {
        const blob = new Blob([JSON.stringify({version: 1, exportedAt: new Date().toISOString(), ...scuiLoad(SCUI_LOGBOOK, {flights: [], events: []})}, null, 2)], {type: "application/json"});
        const a = document.createElement("a");
        a.href = URL.createObjectURL(blob);
        a.download = `spawncity-logbuch-${new Date().toISOString().replaceAll(":", "-")}.json`;
        document.body.appendChild(a); a.click(); a.remove();
        setTimeout(() => URL.revokeObjectURL(a.href), 1000);
        break;
      }
      case "lb-clear": scuiSave(SCUI_LOGBOOK, {flights: [], events: []}); renderLogbook(); break;
      case "copy": navigator.clipboard?.writeText(idea.querySelector("textarea").value.slice(0, 240)).catch(() => {}); break;
      case "github": {
        const text = idea.querySelector("textarea").value.trim().slice(0, 240);
        if (text) open(`https://github.com/ChristianHohlfeld/kernengine.de/issues/new?title=${encodeURIComponent("SpawnCity-Idee: " + text.slice(0, 60))}&body=${encodeURIComponent(text)}`, "_blank", "noopener");
        markIdeaDone();
        closeTop();
        break;
      }
    }
  }
  for (const root of [menu, pause, settings, world, log, idea, corner, placePanel, gpsCard, logbook, radioBtn])
    root.addEventListener("click", ev => { const b = ev.target.closest("[data-act]"); if (b) { ev.stopPropagation(); act(b.dataset.act); } });

  // ---- title screen --------------------------------------------------------------------------
  function showTitle() { menu.hidden = false; ui.playing = false; try { document.exitPointerLock?.(); } catch { /* none */ } focusFirst(menu); sync(); }
  function start() {
    if (startBtn.disabled) return;
    startPlace();
    menu.hidden = true;
    ui.started = ui.playing = true;
    ui.lockWanted = !coarse;
    fullscreenForGame();
    sync();
    relock();
  }
  startBtn.onclick = start;
  // pwa_fullscreen_policy.mjs: an installed (fullscreen / standalone) app is already an app window.
  function fullscreenForGame() {
    const installed = matchMedia("(display-mode: fullscreen)").matches || matchMedia("(display-mode: standalone)").matches || navigator.standalone === true;
    if (!coarse || installed || document.fullscreenElement || !document.documentElement.requestFullscreen) return;
    document.documentElement.requestFullscreen({navigationUI: "hide"}).catch(() => {});
  }
  // Automated browsers and ?autostart=1 skip the title (auto_flight_start.mjs); ?menu=1 forces it.
  const skipTitle = params.get("menu") !== "1" && (navigator.webdriver || params.get("autostart") === "1");
  if (skipTitle) { menu.hidden = true; ui.started = ui.playing = true; }
  sync();

  // ---- keyboard and pad: menus first --------------------------------------------------------
  addEventListener("keydown", ev => {
    const typing = ev.target instanceof HTMLElement && (ev.target.tagName === "TEXTAREA" || (ev.target.tagName === "INPUT" && ev.target.type !== "range"));
    if (!quit.hidden) {
      if (ev.key === "Enter" || ev.code === "KeyY" || ev.code === "KeyZ") answerQuit(true);
      else if (ev.key === "Escape" || ev.code === "KeyN") answerQuit(false);
      ev.stopImmediatePropagation(); ev.preventDefault();
      return;
    }
    if (ev.key === "Escape") {
      ev.stopImmediatePropagation(); ev.preventDefault();
      if (ui.open.length) closeTop();
      else if (menu.hidden && ui.started) openPanel(pause);
      return;
    }
    if (visible()) {
      // Inside a menu the keys belong to the menu, not to the game.
      if (!typing && (ev.key === "ArrowDown" || ev.key === "ArrowUp")) { moveFocus(ev.key === "ArrowDown" ? 1 : -1); ev.preventDefault(); }
      ev.stopImmediatePropagation();
    }
  }, {capture: true});
  // Leaving the pointer lock while playing (Esc on desktop) opens the pause menu.
  let hadLock = false;
  document.addEventListener("pointerlockchange", () => {
    const locked = document.pointerLockElement === canvas;
    if (!locked && hadLock && ui.playing && !visible()) openPanel(pause);
    hadLock = locked;
  });
  function topRoot() { return !quit.hidden ? quit : ui.open.length ? ui.open[ui.open.length - 1] : !menu.hidden ? menu : null; }
  function moveFocus(dir) {
    const root = topRoot();
    if (!root) return;
    const list = focusables(root), i = list.indexOf(document.activeElement);
    list[(i + dir + list.length) % list.length]?.focus();
  }
  // settings_controller_nav.mjs: D-pad up/down moves, left/right changes a slider, A selects, B closes; MENU opens the pause menu.
  const padPrev = new Set();
  function pollPad() {
    const pads = navigator.getGamepads ? Array.from(navigator.getGamepads()).filter(p => p && p.connected) : [];
    const p = pads.find(g => g.mapping === "standard") || pads[0];
    if (!p) return;
    const hit = i => { const on = !!p.buttons[i]?.pressed, was = padPrev.has(i); if (on) padPrev.add(i); else padPrev.delete(i); return on && !was; };
    const a = hit(0), b = hit(1), y = hit(3), menuBtn = hit(9), up = hit(12), down = hit(13), left = hit(14), right = hit(15);
    if (menuBtn && ui.started && menu.hidden) { if (ui.open.length) closeTop(); else openPanel(pause); }
    if (!visible()) return;
    document.body.classList.add("scui-pad");
    if (!quit.hidden) { if (a || y) answerQuit(true); else if (b) answerQuit(false); return; }
    if (up || down) moveFocus(down ? 1 : -1);
    const f = document.activeElement;
    if ((left || right) && f?.closest?.(".scui-row")) {
      const range = f.closest(".scui-row").querySelector("input[type=range]");
      if (range) { range.value = String(+range.value + (right ? 1 : -1) * (+range.step || 1)); range.dispatchEvent(new Event("input", {bubbles: true})); }
    }
    if (a && f && f !== document.body) f.click();
    if (b) { if (ui.open.length) closeTop(); }
  }
  addEventListener("pointerdown", () => document.body.classList.remove("scui-pad"), {capture: true});

  // ---- start place (start_location.mjs, world_location_selector.mjs, auto_flight_start.mjs) -----
  // The chosen place lives only for this browser session: every new start of the app is at your GPS
  // position. Search: Photon (OpenStreetMap), live suggestions.
  const validPlace = p => p && Number.isFinite(+p.lat) && Number.isFinite(+p.lon) && Math.abs(+p.lat) <= 85 && Math.abs(+p.lon) <= 180;
  let place = null;
  try { place = JSON.parse(sessionStorage.getItem(SCUI_PLACE) || "null"); } catch { place = null; }
  if (!validPlace(place)) place = null;
  const explicitStart = params.has("lat") && params.has("lon");  // the URL names the place: no GPS
  function renderPill() {
    const b = menu.querySelector("#scPlace b");
    b.textContent = place ? place.name : explicitStart ? "STARTORT AUS DER ADRESSE" : "MEIN STANDORT (GPS)";
    menu.querySelector("#scPlace").title = place ? `${place.name} · ${place.sub || ""}` : "Start an deinem GPS-Standort";
  }
  function choosePlace(p) {
    place = p && validPlace(p) ? {name: String(p.name || "").slice(0, 80), sub: String(p.sub || "").slice(0, 120), lat: +p.lat, lon: +p.lon} : null;
    try { sessionStorage.setItem(SCUI_PLACE, JSON.stringify(place)); } catch { /* private */ }
    if (place) scuiSave(SCUI_RECENT, [place, ...scuiLoad(SCUI_RECENT, []).filter(q => Math.abs(q.lat - place.lat) > 1e-4 || Math.abs(q.lon - place.lon) > 1e-4)].slice(0, 5));
    renderPill();
    // Picked while a game runs: the world moves there now.
    if (place && ui.started) moveTo(place.lat, place.lon, place.name);
  }
  const placeRow = p => `<button type="button" class="sp-item${place && p && Math.abs(place.lat - p.lat) < 1e-5 && Math.abs(place.lon - p.lon) < 1e-5 ? " sp-on" : ""}" data-lat="${p ? p.lat : ""}" data-lon="${p ? p.lon : ""}" data-name="${scuiEsc(p ? p.name : "")}" data-sub="${scuiEsc(p ? p.sub : "")}"><b>${scuiEsc(p ? p.name : "📡 MEIN STANDORT (GPS)")}</b><small>${scuiEsc(p ? p.sub : "wo du gerade bist")}</small></button>`;
  function renderPlaces(results) {
    const recent = scuiLoad(SCUI_RECENT, []).filter(validPlace);
    const presets = SCUI_PRESETS.map(([name, lat, lon]) => ({name, sub: `${lat.toFixed(4)}, ${lon.toFixed(4)}`, lat, lon}));
    placePanel.querySelector(".sp-list").innerHTML = (results.length ? `<h4>TREFFER</h4>${results.map(placeRow).join("")}` : "") + placeRow(null) +
      (recent.length ? `<h4>ZULETZT</h4>${recent.map(placeRow).join("")}` : "") + `<h4>STAEDTE</h4>${presets.map(placeRow).join("")}`;
  }
  function labelOf(pr) {
    const street = pr.street ? `${pr.street}${pr.housenumber ? " " + pr.housenumber : ""}` : "", head = pr.name || street || pr.city || pr.state || "";
    const tail = [pr.name && street ? street : "", [pr.postcode, pr.city || pr.town || pr.village].filter(Boolean).join(" "), pr.country].filter(x => x && x !== head);
    return {name: head, sub: tail.join(", ")};
  }
  let searchTimer = 0, searchSeq = 0;
  async function search(q) {
    const seq = ++searchSeq;
    if (q.length < 2) { renderPlaces([]); return; }
    try {
      const res = await fetch(`${SCUI_PHOTON}?q=${encodeURIComponent(q)}&limit=6&lang=${(navigator.language || "de").startsWith("de") ? "de" : "en"}`);
      const json = await res.json();
      if (seq !== searchSeq) return;
      renderPlaces((json.features || []).map(f => ({...labelOf(f.properties || {}), lon: f.geometry?.coordinates?.[0], lat: f.geometry?.coordinates?.[1]})).filter(validPlace));
    } catch { if (seq === searchSeq) placePanel.querySelector(".sp-list").insertAdjacentHTML("afterbegin", "<h4>SUCHE NICHT ERREICHBAR</h4>"); }
  }
  placePanel.querySelector("input").addEventListener("input", ev => { clearTimeout(searchTimer); const q = ev.target.value.trim(); searchTimer = setTimeout(() => search(q), 280); });
  placePanel.addEventListener("click", ev => {
    const it = ev.target.closest(".sp-item");
    if (!it) return;
    choosePlace(it.dataset.lat ? {lat: it.dataset.lat, lon: it.dataset.lon, name: it.dataset.name, sub: it.dataset.sub} : null);
    closeTop();
    if (!it.dataset.lat && ui.started) gpsStart();
  });
  renderPill();
  const metres = (a, b) => { const k = Math.PI / 180, x = (b.lon - a.lon) * k * Math.cos((a.lat + b.lat) / 2 * k), y = (b.lat - a.lat) * k; return Math.hypot(x, y) * 6371000; };
  function origin() { const o = last?.ui?.origin; return o ? {lat: o[0], lon: o[1]} : null; }
  let last = null;
  function moveTo(lat, lon, name = "") {
    const o = origin();
    if (o && metres(o, {lat, lon}) < 150) return false;  // already there
    command(`start ${lat.toFixed(6)} ${lon.toFixed(6)} ${name}`.trim());
    return true;
  }
  function startPlace() {
    if (place) { moveTo(place.lat, place.lon, place.name); return; }
    if (!explicitStart) gpsStart();
  }
  // Every START begins where you really are: a fresh fix (never a cached one); after 12 s the network
  // position; a precise fix that comes within a minute and is far from the rough one moves the world.
  function gpsStart() {
    if (!navigator.geolocation) return;
    let used = null, done = false;
    const take = (pos, precise) => {
      const p = {lat: pos.coords.latitude, lon: pos.coords.longitude, acc: pos.coords.accuracy || 0};
      if (!used) { used = p; moveTo(p.lat, p.lon); return; }
      if (precise && p.acc < 50 && metres(used, p) > 300) { used = p; moveTo(p.lat, p.lon); }
    };
    const fallback = setTimeout(() => { if (!used) navigator.geolocation.getCurrentPosition(pos => take(pos, false), () => {}, {enableHighAccuracy: false, maximumAge: 60000, timeout: 20000}); }, 12000);
    const watch = navigator.geolocation.watchPosition(pos => { take(pos, true); if (pos.coords.accuracy < 50) finish(); }, () => {}, {enableHighAccuracy: true, maximumAge: 0, timeout: 60000});
    const finish = () => { if (done) return; done = true; clearTimeout(fallback); navigator.geolocation.clearWatch(watch); };
    setTimeout(finish, 60000);
  }

  // ---- LIVE GPS (gps_live_mode.mjs): the phone's fixes go to the game, which moves you ---------
  const gpsLive = {on: false, watch: null, coarse: null, wake: null};
  const gpsFix = pos => {
    const c = pos.coords;
    command(`gps fix ${c.latitude.toFixed(7)} ${c.longitude.toFixed(7)} ${(c.accuracy || 0).toFixed(1)} ${Number.isFinite(c.speed) && c.speed !== null ? c.speed.toFixed(2) : -1} ${Number.isFinite(c.heading) && c.heading !== null ? c.heading.toFixed(1) : -1}`);
  };
  const onCompass = e => {
    const h = Number.isFinite(e.webkitCompassHeading) ? e.webkitCompassHeading : (e.absolute && Number.isFinite(e.alpha) ? (360 - e.alpha) % 360 : null);
    if (h !== null) command(`gps compass ${(h * Math.PI / 180).toFixed(4)}`);
  };
  async function startGps() {
    if (!navigator.geolocation) { gpsCard.hidden = false; gpsCard.querySelector('[data-k="act"]').textContent = "KEIN GPS IN DIESEM BROWSER"; return; }
    gpsLive.on = true;
    command("gps on");
    // Ask for the position first, still inside the tap (Android shows no prompt after an await).
    gpsLive.watch = navigator.geolocation.watchPosition(gpsFix, err => { gpsCard.querySelector('[data-k="acc"]').textContent = err?.code === 1 ? "STANDORT BLOCKIERT" : "KEIN GPS-SIGNAL"; },
      {enableHighAccuracy: true, maximumAge: 0, timeout: 20000});
    setTimeout(() => { if (gpsLive.on && !last?.gps?.fix && gpsLive.coarse === null) gpsLive.coarse = navigator.geolocation.watchPosition(gpsFix, () => {}, {enableHighAccuracy: false, maximumAge: 60000, timeout: 30000}); }, 9000);
    try { if (typeof DeviceOrientationEvent !== "undefined" && typeof DeviceOrientationEvent.requestPermission === "function") await DeviceOrientationEvent.requestPermission().catch(() => {}); } catch { /* none */ }
    addEventListener("deviceorientationabsolute", onCompass, true);
    addEventListener("deviceorientation", onCompass, true);
    try { gpsLive.wake = await navigator.wakeLock?.request?.("screen"); } catch { /* none */ }
  }
  function stopGps() {
    gpsLive.on = false;
    command("gps off");
    if (gpsLive.watch !== null) navigator.geolocation.clearWatch(gpsLive.watch);
    if (gpsLive.coarse !== null) navigator.geolocation.clearWatch(gpsLive.coarse);
    gpsLive.watch = gpsLive.coarse = null;
    removeEventListener("deviceorientationabsolute", onCompass, true);
    removeEventListener("deviceorientation", onCompass, true);
    try { gpsLive.wake?.release?.(); } catch { /* none */ }
    gpsLive.wake = null;
  }
  const GPS_ICON = {STEHT: "🧍", GEHEN: "🚶", LAUFEN: "🏃", RAD: "🚴", FAHRT: "🚗"};
  function renderGps(g) {
    gpsCard.hidden = !g;
    if (!g) return;
    const q = k => gpsCard.querySelector(`[data-k="${k}"]`);
    q("act").textContent = g.fix ? `${GPS_ICON[g.activity] || ""} ${g.activity}` : "GPS SUCHT ...";
    const pace = g.speed > 0.8 && (g.activity === "LAUFEN" || g.activity === "GEHEN") ? 1000 / g.speed / 60 : 0;
    q("speed").textContent = pace ? `${Math.floor(pace)}:${String(Math.round(pace % 1 * 60)).padStart(2, "0")} /km` : `${(g.speed * 3.6).toFixed(0)} km/h`;
    q("dist").textContent = g.distance >= 1000 ? `${(g.distance / 1000).toFixed(2)} km` : `${Math.round(g.distance)} m`;
    q("time").textContent = `${Math.floor(g.time / 60)}:${String(Math.floor(g.time % 60)).padStart(2, "0")}`;
    if (g.fix) q("acc").textContent = `± ${g.accuracy} m`;
    gpsCard.querySelector('[data-act="gps-view"]').textContent = g.view;
  }

  // ---- flight logbook (flight_logbook.mjs): the game records, the page keeps the history ----------
  const session = Date.now().toString(36);
  let logSerial = -1;
  function syncLogbook() {
    let fresh;
    try { fresh = JSON.parse(command("logbook")); } catch { return; }
    const book = scuiLoad(SCUI_LOGBOOK, {flights: [], events: []});
    const known = new Set(book.flights.map(f => f.key));
    for (const f of fresh.flights.slice().reverse()) {
      const key = `${session}-${f.id}`;
      if (known.has(key)) continue;
      book.flights.unshift({key, at: new Date().toISOString(), origin: last?.ui?.origin || null, ...f});
    }
    const ekeys = new Set(book.events.map(e => e.key));
    for (const ev of fresh.events.slice().reverse()) {
      const key = `${session}-${ev.t}-${ev.stage}`;
      if (!ekeys.has(key)) book.events.unshift({key, at: new Date().toISOString(), ...ev});
    }
    book.flights = book.flights.slice(0, 50);
    book.events = book.events.slice(0, 120);
    scuiSave(SCUI_LOGBOOK, book);
  }
  const dur = s => { const m = Math.floor(s / 60), r = s - m * 60; return m ? `${m}m ${r.toFixed(1)}s` : `${r.toFixed(1)}s`; };
  function renderLogbook() {
    const book = scuiLoad(SCUI_LOGBOOK, {flights: [], events: []});
    logbook.querySelector('[data-k="flights"]').innerHTML = book.flights.length ? book.flights.map(f => `<article><time>${scuiEsc(new Date(f.at).toLocaleString())}</time>
      <b>${last?.offline ? "ERSATZSTADT" : "WELT"} · ${dur(f.duration)} · ${scuiEsc(String(f.reason).replaceAll("_", " "))}</b>
      <div class="lb-stats">STRECKE ${f.distance.toFixed(1)} m · MAX ${(f.maxSpeed * 3.6).toFixed(0)} km/h · AGL ${f.maxAgl.toFixed(1)} m · AKKU ${f.voltsStart.toFixed(1)} → ${f.voltsMin.toFixed(1)} V · ${f.path.length} Punkte</div></article>`).join("")
      : `<article><b>NOCH KEINE FLUEGE</b></article>`;
    logbook.querySelector('[data-k="events"]').innerHTML = book.events.length ? book.events.slice(0, 30).map(e => `<article><time>${scuiEsc(new Date(e.at).toLocaleString())}</time><b>${scuiEsc(e.stage)}</b></article>`).join("")
      : `<article><b>KEINE NETZEREIGNISSE</b></article>`;
  }

  // ---- suggestion card (suggestion_box.mjs): after 15 min of play, once, snooze 10 min -------
  function markIdeaDone() { const s = scuiLoad(SCUI_SUGGEST, {}); s.done = true; scuiSave(SCUI_SUGGEST, s); }
  setInterval(() => {
    if (!ui.playing || visible() || document.hidden) return;
    const s = scuiLoad(SCUI_SUGGEST, {played: 0, done: false, snoozeUntil: 0});
    if (s.done) return;
    s.played = (s.played || 0) + 5;
    if (s.played >= 15 * 60 && s.played >= (s.snoozeUntil || 0)) { s.snoozeUntil = s.played + 10 * 60; openPanel(idea); }
    scuiSave(SCUI_SUGGEST, s);
  }, 5000);

  return {
    /** Called every frame with the game's HUD JSON. */
    update(h) {
      pollPad();
      if (!h) return;
      last = h;
      renderGps(h.gps);
      if (h.logbook && h.logbook.serial !== logSerial) { logSerial = h.logbook.serial; if (logSerial > 0) syncLogbook(); }
      // car radio banner and the touch button (car_radio.mjs)
      if (h.banner) { banner.querySelector("b").textContent = h.banner; banner.querySelector("small").textContent = h.bannerSub || ""; }
      banner.style.opacity = h.banner ? "1" : "0";
      radioBtn.hidden = !(h.mode === "car" && h.touch);
      // jetpack fuel (player_walk_mode_v4.mjs jetHud): only while it is not full
      const jp = h.jetpack;
      jetBar.hidden = !jp || (jp.fuel > 0.995 && !jp.thrusting) || !!h.ui?.paused;
      if (!jetBar.hidden) { jetBar.querySelector("i").style.width = `${Math.round(jp.fuel * 100)}%`; jetBar.classList.toggle("burn", jp.thrusting); }
      const ready = !!h.placed;
      if (startBtn.disabled === ready) { startBtn.disabled = !ready; startBtn.textContent = ready ? "START" : "LADEN"; }
      const status = ready ? (h.offline ? "OFFLINE · PROZEDURALE ERSATZSTADT" : "BEREIT") : (statusEl?.textContent || "STADT WIRD VORBEREITET").toUpperCase();
      if (menuStatus.textContent !== status) menuStatus.textContent = status;
    },
    get playing() { return ui.playing; },
    open: name => act(name),
    title: showTitle,
    start,
  };
}
