// SpawnCity on Kern - the original's sound. Everything you hear is spawncity.de's own: its music
// file (sim/audio/8_bits_only.m4a), its audio modules unchanged (web/audio/*.mjs: settings and the
// effects bus, the combat sound bank, the title music, the drone's motors, footsteps and the car's
// engine, sirens, jetpack, animals, the car radio) and the sound functions of its gameplay modules
// cut out of their source (web/audio/original_sfx.mjs). Kern's own mixer is not heard on this page
// ("audio web 1", game_audio_web.cpp); it only mixes the other players' voices.
//
// This module feeds them what the original's modules read: the game's audio state every frame (the
// listener, the walk / drive / jetpack / drone state, police, jets, missiles) under the globals the
// original uses, the settings under its storage key, and the game's audio events - each names the
// original function that sounds at that moment (game_audio_web.cpp osfx) and is called here with
// the original's own arguments. Nothing here makes a sound of its own.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
import {AUDIO_SETTINGS_KEY, loadAudioSettings, normalizeAudioSettings, saveAudioSettings} from "./audio/audio_settings.mjs";
import {MUSIC_PLAYER_VERSION} from "./audio/music_player.mjs";
import {HybridMotorSound} from "./audio/motor_sound.mjs";
import {POLICE_SIRENS_VERSION} from "./audio/police_sirens.mjs";
import {JETPACK_AUDIO_VERSION} from "./audio/jetpack_audio.mjs";
import {playAnimal} from "./audio/animal_audio.mjs";
import {installImmersiveSoundscape} from "./audio/immersive_soundscape.mjs";
import {createAmbience} from "./audio/ambience.mjs";
import {CAR_RADIO_VERSION} from "./audio/car_radio.mjs";
import {ORGANIC_HIT_AUDIO_VERSION} from "./audio/world_organic_hit_audio.mjs";
import {announceHeadshot} from "./audio/headshot_feedback.mjs";
import {getSharedCombatAudioContext} from "./audio/combat_audio_bank.mjs";

// Kern: a zombie's voice - a low rasping groan (a sawtooth throat through two vowel formants, a
// tremolo rattle, a breath of noise), a different one every time; "shriek" when hit
function zombieVoice(at, listener, kind) {
  const ctx = getSharedCombatAudioContext?.();
  if (!ctx || ctx.state !== "running" || !listener) return;
  const set = loadAudioSettings();
  if (!set.soundEnabled) return;
  const d = Math.hypot(at[0] - listener.x, at[1] - listener.y, at[2] - listener.z);
  if (d > 40) return;
  const vol = Math.min(0.5, 0.42 / (1 + d * 0.1)) * Math.max(0, Math.min(1.4, (Number(set.fxVolume) || 0) / 72));
  if (vol < 0.004) return;
  const t = ctx.currentTime, shriek = kind === "shriek", dur = shriek ? 0.55 + Math.random() * 0.3 : 0.9 + Math.random() * 1.0;
  const f0 = (shriek ? 210 : 72) * (0.85 + Math.random() * 0.35);
  const out = ctx.createGain(); out.gain.setValueAtTime(0.0001, t); out.gain.exponentialRampToValueAtTime(vol, t + 0.08); out.gain.setValueAtTime(vol, t + dur * 0.6); out.gain.exponentialRampToValueAtTime(0.0001, t + dur);
  const pan = ctx.createStereoPanner ? ctx.createStereoPanner() : null;
  if (pan) { pan.pan.value = Math.max(-0.8, Math.min(0.8, (at[0] - listener.x) / Math.max(4, d) * 0.6)); out.connect(pan).connect(ctx.destination); } else out.connect(ctx.destination);
  const f1 = ctx.createBiquadFilter(); f1.type = "bandpass"; f1.frequency.value = (shriek ? 1100 : 520) * (0.9 + Math.random() * 0.25); f1.Q.value = 5;
  const f2 = ctx.createBiquadFilter(); f2.type = "bandpass"; f2.frequency.value = (shriek ? 2300 : 1150) * (0.9 + Math.random() * 0.2); f2.Q.value = 6;
  const trem = ctx.createGain(); trem.gain.value = 0.55;
  const lfo = ctx.createOscillator(); lfo.frequency.value = 7 + Math.random() * 6; const lg = ctx.createGain(); lg.gain.value = 0.45; lfo.connect(lg).connect(trem.gain);
  const o = ctx.createOscillator(); o.type = "sawtooth"; o.frequency.setValueAtTime(f0, t); o.frequency.exponentialRampToValueAtTime(f0 * (shriek ? 0.7 : 0.82), t + dur);
  o.connect(trem); trem.connect(f1); trem.connect(f2); f1.connect(out); f2.connect(out);
  // the breath: noise through the throat
  const n = ctx.createBufferSource(), len = Math.floor(ctx.sampleRate * dur), buf = ctx.createBuffer(1, len, ctx.sampleRate), ch = buf.getChannelData(0);
  for (let i = 0; i < len; i++) ch[i] = (Math.random() * 2 - 1) * 0.35;
  n.buffer = buf; const nf = ctx.createBiquadFilter(); nf.type = "bandpass"; nf.frequency.value = shriek ? 1800 : 800; nf.Q.value = 1.2; n.connect(nf).connect(out);
  o.start(t); lfo.start(t); n.start(t); o.stop(t + dur + 0.05); lfo.stop(t + dur + 0.05); n.stop(t + dur + 0.05);
}
import * as O from "./audio/original_sfx.mjs";
import {V3} from "./audio/original_sfx.mjs";

const v3 = a => new V3(a[0], a[1], a[2]);

// The Kern settings the page keeps (view.mjs / ui.mjs) in the original's names, for the start (before
// the game runs: the menu music decides at once whether it plays).
const KERN_SETTINGS_KEY = "spawnCityKernSettingsV1";
const KERN_TO_ORIGINAL = {"audio.on": "soundEnabled", "music.on": "musicEnabled", "music.volume": "musicVolume", "audio.drone": "droneVolume", "audio.shots": "shotsVolume",
  "audio.fx": "fxVolume", "audio.steps": "footstepsVolume", "audio.vehicle": "vehicleVolume", "audio.ambient": "ambientVolume"};
function startSettings() {
  let kern = {};
  try { kern = JSON.parse(localStorage.getItem(KERN_SETTINGS_KEY) || "{}") || {}; } catch { /* none */ }
  const next = {...loadAudioSettings()};
  for (const [k, o] of Object.entries(KERN_TO_ORIGINAL)) if (kern[k] !== undefined) next[o] = o.endsWith("Enabled") ? Number(kern[k]) > 0.5 : Number(kern[k]);
  try { localStorage.setItem(AUDIO_SETTINGS_KEY, JSON.stringify(normalizeAudioSettings(next))); } catch { /* none */ }
}

// LifeOut::Sound (life.h) -> the original's function (cows.mjs, pigeons.mjs, jogger_chain.mjs,
// street_voices.mjs, animal_audio.mjs); the horns come as their own event (street_voices.honk).

/** Starts the original's sound on the page (the title music at once). attach(command) once the game
 *  runs, then update(hud, ui) every frame. */
export function installOriginalAudio() {
  startSettings();
  let command = null;
  globalThis.__scOriginalAudio = true;
  // the shims of the original's globals (filled every frame from the game's audio state)
  const bridge = globalThis.__arondightRealWorld ??= {active: false, originLat: NaN, originLon: NaN};
  const camera = {position: new V3()};
  bridge.threeCamera = camera;
  bridge.presentedCamera = () => camera;
  const walk = globalThis.__arondightWalkMode ??= {mode: "drone"};
  walk.position = new V3();
  const jump = globalThis.__arondightWalkJump = {airborne: false};
  const drive = globalThis.__arondightVehicleDrive = {active: false, speedMps: 0, throttle: 0, vehicleId: ""};
  const jetpack = globalThis.__arondightJetpack = {thrusting: false, acc: 0};
  const remoteJets = globalThis.__arondightRemoteJetPos = new Map();
  const police = globalThis.__policeGroundUnits = {units: []};
  const wanted = globalThis.__arondightWantedSystem = {drones: []};
  const gps = globalThis.__arondightGpsLive = {active: false};
  let targets = [];
  globalThis.__arondightPlayerVitals = {damageTargets: () => targets};
  let pose = {position: [0, 0, 0], direction: [0, 1, 0]};
  O.K.pose = () => pose;

  // the menu music and the menu buttons' click blip (drone_simulator.html's inline scripts), unchanged
  for (const [fn, file] of [["__scMenuMusic", "menu_music.js"], ["__scMenuBlip", "menu_blip.js"]]) {
    if (globalThis[fn]) globalThis[fn]();
    else { const sc = document.createElement("script"); sc.src = new URL(`audio/${file}`, import.meta.url).href; document.body.appendChild(sc); }
  }
  // a switch the page flips (view.mjs) is in the original's settings at once, as its inline script
  // writes it (the blip after EFFECTS reads it in the same click)
  const KEY_OF = Object.fromEntries(Object.entries(KERN_TO_ORIGINAL));
  globalThis.__scAudioSetting = (kernKey, value) => { const o = KEY_OF[kernKey]; if (o) saveAudioSettings({...loadAudioSettings(), [o]: o.endsWith("Enabled") ? Number(value) > 0.5 : Number(value)}); };
  installImmersiveSoundscape();  // (already installed by its import: footsteps, the car's engine, its knocks)

  // simulator.mjs: the drone's motors (one HybridMotorSound on #viewport, unlocked by a tap)
  const viewport = document.getElementById("viewport") || document.body;
  const motorSound = new HybridMotorSound(viewport);
  document.addEventListener("pointerdown", event => { if (event.target?.id !== "soundToggle" && motorSound.enabled && !motorSound.isRunning()) motorSound.unlock(); }, {passive: true});
  document.addEventListener("visibilitychange", () => motorSound.syncState(), {passive: true});
  let lastMotorAt = 0;
  const still = [0, 0, 0, 0];

  const fire = (type, detail) => { try { window.dispatchEvent(new CustomEvent(type, {detail})); } catch { /* none */ } };
  const MATERIAL = ["", "concrete", "metal", "flesh", "wood"];  // game_audio_web.cpp hit_material (world_action_feedback.mjs materialKind)
  const LIFE = [
    (p, k) => O.cows.moo(p, k),                                  // moo
    (p, k) => O.cows.moo(p, k, {pain: true}),                    // moo_pain
    p => O.pigeons.coo(p[0], p[1], Math.hypot(p[0] - walk.position.x, p[1] - walk.position.y)),  // coo
    p => O.pigeons.flapSound(p[0], p[1]),                        // flap
    (p, k, pitch) => playAnimal("bird", pitch ?? 1),             // squawk (a hit bird: its own pitch, game life.cpp)
    p => O.cows.thud(p, false),                                  // thud
    p => O.jogger_chain.stomp(p[0], p[1], false),                // stomp
    p => O.jogger_chain.stomp(p[0], p[1], true),                 // stomp_accent
    () => O.jogger_chain.jingle(),                               // jingle
    null, null,                                                  // horn, horn_bus: street_voices.honk
    null, null,                                                  // scream, scream_f: the original screams when the arms fly up (street_voices.scream below)
    () => playAnimal("dog"), () => playAnimal("cat"), () => playAnimal("hiss"), () => playAnimal("yowl"), () => playAnimal("screech"),
  ];
  // the game's audio events: "<module>.<function>" with a place and three numbers (game_audio_web.cpp)
  const EVENTS = {
    // flight_fire_fx.mjs fire(): shotSound; world_action_feedback.mjs: playShotBody(1) for the new
    // round, playImpactSound(kind) where it struck (acknowledgeSceneHit)
    "flight_fire_fx.fire": (p, material) => { O.flight_fire_fx.shotSound(); O.world_action_feedback.playShotBody(1); if (material > 0) O.world_action_feedback.playImpactSound(MATERIAL[material]); },
    // the weapons' own launch sound (weapons.h Id): grenade launcher audioShot(.34), rocket launcher
    // audioShot(.4) (gameplay_final_runtime_v2.mjs), hand grenade whoosh (hand_grenade.mjs), the
    // drone's missiles audioShot(.34), the jet's rocket sfx("explosion", .25, 1.8) and bomb
    // sfx("bounce", .4, .5) (jet_mode.mjs); GUIDED: its motor lights later (guided_missile.mjs)
    "launch": (p, id, byPlayer) => {
      if (!byPlayer) return;
      if (id === 3) O.gameplay_final_runtime_v2.audioShot(.34);
      else if (id === 4) O.gameplay_final_runtime_v2.audioShot(.4);
      else if (id === 7) O.hand_grenade.whoosh();
      else if (id === 9) O.gameplay_final_runtime_v2.audioShot(.34);
      else if (id === 12) O.jet_mode.sfx("explosion", {gain: .25, playbackRate: 1.8});
      else if (id === 13) O.jet_mode.sfx("bounce", {gain: .4, playbackRate: .5});
    },
    "jet_mode.gun": () => O.jet_mode.sfx("shot", {gain: .32, playbackRate: .7, minIntervalMs: 40}),
    "jet_mode.boom": () => { O.jet_mode.sfx("explosion", {gain: .9, playbackRate: .55}); setTimeout(() => O.jet_mode.sfx("explosion", {gain: .6, playbackRate: .62}), 120); },
    // on foot (gameplay_final_runtime_v2.mjs): MP audioShot(.18), Glock / sniper their reports
    "foot.shot": (p, w) => { if (w === 0) O.gameplay_final_runtime_v2.audioShot(.18); else if (w === 1) O.gameplay_final_runtime_v2.glockShot(); else if (w === 2) O.gameplay_final_runtime_v2.sniperShot(); },
    "gameplay_final_runtime_v2.audioGrenadeBounce": (p, speed, bounces) => O.gameplay_final_runtime_v2.audioGrenadeBounce(speed, bounces),
    "gameplay_final_runtime_v2.knifeSwish": () => O.gameplay_final_runtime_v2.knifeSwish(),
    "gameplay_final_runtime_v2.knifeHit": (p, flesh) => O.gameplay_final_runtime_v2.knifeHit(!!flesh),
    "gameplay_final_runtime_v2.punchWhoosh": (p, h, combo) => O.gameplay_final_runtime_v2.punchWhoosh(h, !!combo),
    "gameplay_final_runtime_v2.punchThud": (p, h, combo) => O.gameplay_final_runtime_v2.punchThud(h, !!combo),
    "hand_grenade.pinSound": () => O.hand_grenade.pinSound(),
    "hand_grenade.tickSound": (p, urgent) => O.hand_grenade.tickSound(!!urgent),
    "gangsters.bang": p => O.gangsters.bang(p),
    "gangsters.stabSound": p => O.gangsters.stabSound(p),
    "zombie_nights.groan": p => zombieVoice(p, camera.position, "groan"),
    "zombie.shriek": p => zombieVoice(p, camera.position, "shriek"),
    "zombie_nights.repairKnock": () => O.zombie_nights.repairKnock(),
    "sandbox_game.sfx": (p, explosion, gain, rate) => O.sandbox_game.sfx(explosion ? "explosion" : "shot", {gain, rate, at: v3(p)}),
    "military_response.sound": (p, explosion, gain, rate) => O.military_response.sound(explosion ? "explosion" : "shot", p[0], p[1], p[2], gain, rate),
    "police_ground_units.playShot": p => O.police_ground_units.playShot(p[0], p[1], p[2]),
    "flak_battery.audioShot": p => O.flak_battery.audioShot(null, Math.hypot(camera.position.x - p[0], camera.position.y - p[1], camera.position.z - p[2])),
    "flak_battery.audioBurst": p => O.flak_battery.audioBurst(p, Math.hypot(camera.position.x - p[0], camera.position.y - p[1], camera.position.z - p[2])),
    "fighter_jets.blastSound": p => O.fighter_jets.blastSound(v3(p)),
    "player_vehicle_runtime_v2.playExplosion": p => O.player_vehicle_runtime_v2.playExplosion(p, false),
    "player_vehicle_runtime_v2.playPistol": p => O.player_vehicle_runtime_v2.playPistol(p, true),  // onFx: a mate's shot
    "headshot_feedback.announceHeadshot": () => announceHeadshot({}),
    "guided_missile.lockTone": () => O.guided_missile.lockTone(),
    "guided_missile.denied": () => O.guided_missile.denied(),
    // wanted_police_drones.mjs playTone calls (lines 367, 387, 398, 473)
    "wanted_police_drones.down": () => O.wanted_police_drones.playTone({frequency: 150, endFrequency: 48, duration: .28, gain: .055, type: "sawtooth"}),
    "wanted_police_drones.emp": () => O.wanted_police_drones.playTone({frequency: 1120, endFrequency: 170, duration: .34, gain: .052, type: "sawtooth"}),
    "wanted_police_drones.star": (p, stars) => O.wanted_police_drones.playTone({frequency: 520 + stars * 90, endFrequency: 760 + stars * 100, duration: .16, gain: .022, type: "square"}),
    "wanted_police_drones.shot": () => O.wanted_police_drones.playTone({frequency: 310, endFrequency: 145, duration: .075, gain: .015, type: "square"}),
    "street_lamps.glass": p => O.street_lamps.glass(p),
    "street_lamps.clang": (p, k) => O.street_lamps.clang(p, k),
    "people_impacts.thump": (p, gain) => O.people_impacts.thump(gain),
    "gameplay_polish_lite.playPersonGroan": () => O.gameplay_polish_lite.playPersonGroan(),
    "ping_marker.set": () => globalThis.__oppUiBlip?.(false),  // ping_marker.mjs ping("set")
    "fighter_jets.beep": (p, kind) => kind === 0 ? O.fighter_jets.beep([740, 990], .06, .11) : kind === 1 ? O.fighter_jets.beep([220], .16, .1) : kind === 2 ? O.fighter_jets.beep([220], .18, .1) : kind === 3 ? O.fighter_jets.beep([330], .08, .1)
      : kind === 4 ? O.fighter_jets.beep([660, 990], .07, .1) : O.fighter_jets.beep([880, 1175, 1568], .08, .12),
    "street_voices.honk": (p, bus, style) => O.street_voices.honk(p, {kind: bus ? "bus" : "car", style: ["toot", "double", "long"][style] || "toot", source: ""}),
    // world_procedural_population.mjs: arms flung up = a scream (seed: the person)
    "street_voices.scream": (p, seed) => O.street_voices.scream(p, {seed: `p${seed}`}),
    "jet_mode.knock": () => O.jet_mode.sfx("bounce", {gain: .3, playbackRate: .6, minIntervalMs: 60}),
    "wanted_police_drones.escaped": () => O.wanted_police_drones.playTone({frequency: 660, endFrequency: 1040, duration: .22, gain: .023, type: "sine"}),
    "life": (p, sound, volume, pitch) => LIFE[sound]?.(p, volume, pitch),
    // the original's own events, for its unchanged modules
    "world-kill": () => fire("arondight:world-kill", {kind: "person"}),                       // world_organic_hit_audio.mjs
    "world-physics-impact": (p, dv) => fire("arondight:world-physics-impact", {id: drive.vehicleId, deltaVelocityMps: dv}),  // immersive_soundscape.mjs
    "jetpack.ignition": () => fire("arondight:player-jetpack", {}),                             // jetpack_audio.mjs
  };

  // continuous voices the gameplay modules keep: the fighter jets (fighter_jets.mjs), the player's
  // jet (jet_mode.mjs), the GUIDED missiles' motors (guided_missile.mjs)
  const jets = new Map(), missiles = new Map(), rockets = new Map(), mates = new Map();
  let jetVoice = false, settingsKey = "", was = {mode: "", playing: false, car: false, stars: 0, resets: 0, dmg: -1, hits: -1, me: null, peers: new Map(), flight: null};
  const SPEED = 255;
  const state = {}, events = [];  // the last events (tests)

  const ambience = createAmbience();
  function update(h, ui) {
    if (!command) return;
    let a;
    try { a = JSON.parse(command("audio frame")); } catch { return; }
    // no sound before the page's first gesture: an AudioContext made then starts suspended and the
    // browser warns ("not allowed to start"); the frame is still taken so the queue never grows
    if (globalThis.navigator?.userActivation && !navigator.userActivation.hasBeenActive) return;
    Object.assign(state, a);
    try { ambience.step(h); } catch { /* no ambience */ }  // Kern: the town's quiet bed (ambience.mjs)
    const now = performance.now();
    // ---- settings: the game's (Kern's settings) under the original's key, every module follows
    const key = JSON.stringify(a.set);
    if (key !== settingsKey) {
      settingsKey = key;
      saveAudioSettings({...a.set, soundEnabled: !!a.set.soundEnabled, musicEnabled: !!a.set.musicEnabled});
    }
    // ---- the globals the original's modules read
    camera.position.set(a.L[0], a.L[1], a.L[2]);
    pose = {position: a.L, direction: a.fwd};
    walk.position.set(a.walk.p[0], a.walk.p[1], a.walk.p[2]);
    walk.dead = !!a.walk.dead;
    walk.mode = a.mode === "drone" ? "drone" : "foot";
    jump.airborne = !!a.walk.air;
    gps.active = !!a.gps;
    drive.active = !!a.car && !a.paused;
    drive.speedMps = a.car?.v || 0;
    drive.throttle = a.car?.th || 0;
    drive.vehicleId = a.car ? `car-${a.car.id}` : drive.vehicleId;
    jetpack.thrusting = !!a.jp[0];
    jetpack.acc = a.jp[1];
    for (const [id, x, y, z] of a.rj) remoteJets.set(id, {x, y, z, at: now});
    police.units = a.paused ? [] : a.pc.map(p => ({lights: true, state: "pursuit", pose: {position: [p[0], p[1], p[2] - .9]}}));
    wanted.drones = a.paused ? [] : a.pd.map(p => ({active: true, root: {position: {x: p[0], y: p[1], z: p[2]}}}));
    targets = [{kind: a.mode === "drone" ? "drone" : "player", hp: a.walk.dead ? 0 : 100, position: a.mode === "drone" && a.drone ? v3(a.drone.p) : walk.position}];
    if (a.mode === "drone") targets.push({kind: "player", hp: a.walk.dead ? 0 : 100, position: walk.position});
    // ---- the original's own events at the moments it fires them
    const playing = !!ui?.playing;
    if (playing && !was.playing) fire("arondight:game-start", {});                                // auto_flight_start.mjs START
    was.playing = playing;
    // the radio: on by itself in the car (the original), in the jet and the drone once you switch it
    // on there (its 📻, the VR menu: globalThis.__scRadioAny) - car_radio.mjs "vehicle-mode"
    const inCar = (a.mode === "car" || ((a.mode === "jet" || a.mode === "drone") && globalThis.__scRadioAny)) && !a.paused;
    if (inCar !== was.car) fire("arondight:vehicle-mode", {active: inCar});                      // player_car_mode.mjs (car_radio.mjs)
    was.car = inCar;
    // ---- the drone's motors (simulator.mjs render: not while the menu covers the game)
    if (playing) {
      const d = a.drone;
      motorSound.syncFcState(d ? d.st : 0, d ? !!d.arm : false);
      if (now - lastMotorAt >= 50) {  // PRESENTATION_AUDIO_INTERVAL_MS
        lastMotorAt = now;
        const p = d?.p || [0, 0, -1e4];
        motorSound.update({motorOmega: d?.w || still, motorTorque: d?.mt || still, propTorque: d?.pt || still, p: {propD: d?.d || .127}, position: () => p}, camera.position);
      }
    }
    // ---- events
    for (const [name, x, y, z, e1, e2, e3] of a.e) {
      events.push(name);
      if (events.length > 64) events.shift();
      try { EVENTS[name]?.([x, y, z], e1, e2, e3); } catch (err) { console.warn("[spawncity] sound", name, err); }
    }
    // ---- flight_fire_fx.mjs: the damage vignette's damageSound, the crosshair's hitConfirmSound
    const v = h?.ctl?.view || {};
    if (was.dmg >= 0 && v.dmg > was.dmg) O.flight_fire_fx.damageSound();
    if (v.dmg !== undefined) was.dmg = v.dmg;
    if (was.hits >= 0 && v.droneHits > was.hits) O.flight_fire_fx.hitConfirmSound();
    if (v.droneHits !== undefined) was.hits = v.droneHits;
    // ---- vs_combat_presentation.mjs playExplosion: you went down (local) / a mate did
    if (h?.net) {
      const me = !!h.ctl?.dead;
      if (me && was.me === false) O.vs_combat_presentation.playExplosion(true);
      was.me = me;
      for (const p of h.mp?.players || []) { if (p.dead && was.peers.get(p.slot) === false) O.vs_combat_presentation.playExplosion(false); was.peers.set(p.slot, !!p.dead); }
    }
    // ---- player_vehicle_runtime_v2.mjs remote avatars: a mate walking on foot steps every
    // max(270, min(480, 500 - speed * 22)) ms (walking: on foot, not airborne, faster than .22 m/s)
    for (const [slot, x, y, z, jet] of a.rp || []) {
      let r = mates.get(slot);
      if (!r) { r = {x, y, at: now, lastStep: 0, speed: 0}; mates.set(slot, r); }
      const dt = (now - r.at) / 1000;
      if (dt > 0.02) { r.speed = Math.hypot(x - r.x, y - r.y) / dt; r.x = x; r.y = y; r.at = now; }
      if (!jet && r.speed > .22 && now - r.lastStep > Math.max(270, Math.min(480, 500 - r.speed * 22))) { r.lastStep = now; O.player_vehicle_runtime_v2.playStep([x, y, z], true); }
    }
    // ---- fighter_jets.mjs: a voice per jet, its retarded time and Doppler (updateJetAudio)
    const seen = new Set();
    for (const [x, y0, z, age] of a.fj) {
      const id = `${x.toFixed(1)}:${y0.toFixed(1)}`;
      seen.add(id);
      let j = jets.get(id);
      if (!j) { j = {x, y0, z, t0: now - age * 1000, pitch: Math.random(), voice: null}; jets.set(id, j); }
      if (!j.voice) O.fighter_jets.startJetVoice(j);
      O.fighter_jets.updateJetAudio(j, now);
    }
    for (const [id, j] of jets) if (!seen.has(id)) { O.fighter_jets.stopJetVoice(j); jets.delete(id); }
    // ---- jet_mode.mjs: the engine from boarding to leaving (startVoice / updateVoice / stopVoice),
    // toggleMode's knock, EJECT's bang
    if (a.jet && !a.paused) {
      O.jet_mode.setJet({af: {mode: a.jet.flight ? "flight" : "hover", lever: a.jet.lever, nozzle: a.jet.nozzle, thrust: a.jet.thrust}}, a.jet.cockpit ? "cockpit" : "chase", {thrustDry: a.jet.dry});
      if (!jetVoice) { O.jet_mode.startVoice(); jetVoice = true; }
      O.jet_mode.updateVoice();
      if (was.flight !== null && was.flight !== a.jet.flight) O.jet_mode.sfx("bounce", {gain: .25, playbackRate: .4});
      was.flight = a.jet.flight;
    } else {
      if (jetVoice) { O.jet_mode.stopVoice(); jetVoice = false; }
      was.flight = null;
    }
    if (was.mode === "jet" && a.mode === "parachute") { O.jet_mode.sfx("explosion", {gain: .35, playbackRate: 1.6}); }
    was.mode = a.mode;
    // ---- guided_missile.mjs: the motor's voice while it burns (panned at the missile)
    const burning = new Set();
    for (const [born, x, y, z] of a.gm) {
      burning.add(born);
      let m = missiles.get(born);
      if (!m) { m = {voice: O.guided_missile.motorVoice()}; missiles.set(born, m); }
      const pan = m.voice?.pan;
      if (pan?.positionX) { pan.positionX.value = x; pan.positionY.value = y; pan.positionZ.value = z; }
    }
    for (const [born, m] of missiles) if (!burning.has(born)) { O.guided_missile.stopVoice(m.voice); missiles.delete(born); }
    // ---- rockets in flight: the same motor voice, panned at the rocket (a drone rocket a touch
    // quieter, the launcher's rocket and the jet's missile full)
    const flying = new Set();
    for (const [born, x, y, z, kind] of a.rk || []) {
      flying.add(born);
      let r = rockets.get(born);
      if (!r) {
        r = {voice: O.guided_missile.motorVoice()};
        if (kind === 2 && r.voice?.g?.gain) try { r.voice.g.gain.value *= 0.7; } catch { /* fixed */ }
        rockets.set(born, r);
      }
      const pan = r.voice?.pan;
      if (pan?.positionX) { pan.positionX.value = x; pan.positionY.value = y; pan.positionZ.value = z; }
    }
    for (const [born, r] of rockets) if (!flying.has(born)) { O.guided_missile.stopVoice(r.voice); rockets.delete(born); }
  }

  globalThis.__scPlayReward = (kind, gain) => O.gameplay_fun_runtime.playReward(kind, gain);
  // a hidden page is silent: the effects' context sleeps (music_player.mjs pauses its track itself,
  // wanted() = !document.hidden), and wakes when the page is seen again
  const sleep = hidden => { const c = globalThis.__sharedAudioContext; if (!c) return; try { if (hidden && c.state === "running") c.suspend(); else if (!hidden && c.state === "suspended") c.resume(); } catch { /* none */ } };
  document.addEventListener("visibilitychange", () => sleep(document.hidden));
  addEventListener("pagehide", () => sleep(true));
  addEventListener("pageshow", () => sleep(document.hidden));
  const attach = cmd => { command = cmd; command("audio web 1"); };
  const versions = {music: MUSIC_PLAYER_VERSION, sirens: POLICE_SIRENS_VERSION, jetpack: JETPACK_AUDIO_VERSION, radio: CAR_RADIO_VERSION, organic: ORGANIC_HIT_AUDIO_VERSION};
  return {attach, update, state, events, versions, motorSound};
}

/** Voice chat stays out of the effects bus (proximity_voice.mjs marks its output exempt the same way). */
export function exemptVoice(kernAudio) {
  const node = kernAudio?.node;
  if (!node || node.__audioBusExempt) return !!node;
  node.__audioBusExempt = true;
  try { node.disconnect(); node.connect(kernAudio.context.destination); } catch { /* none */ }
  return true;
}
