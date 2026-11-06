#!/usr/bin/env python3
"""Takes the sound of the original SpawnCity (spawncity.de) for SpawnCity on Kern: its audio files,
its audio modules unchanged and, from the gameplay modules, the functions that make a sound - as
they are, cut out of the source text (tools/jsextract.py), not rewritten.

  kern/games/spawncity/tools/take_original_audio.py <spawncity-de checkout> [<web dir>]

Writes into <web dir> (default: kern/games/spawncity/web):
  sim/audio/8_bits_only.m4a   the title music (drone_simulator.html / music_player.mjs load it from
                              ./sim/audio/ next to the page, so it keeps that path)
  audio/<module>.mjs          the original's audio modules, unchanged except for an import of a
                              module that is not here (named in the file's header)
  audio/menu_music.js         drone_simulator.html's inline script that starts the menu music
  audio/menu_blip.js          drone_simulator.html's inline script of the menu buttons' click blip
  audio/original_sfx.mjs      the sound functions of the gameplay modules, one block per module
                              with the few names each block reads (its settings, its camera)
The page (web/audio.mjs) feeds all of it from the game's state and events.
"""
import pathlib
import re
import shutil
import subprocess
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import jsextract  # noqa: E402

orig = pathlib.Path(sys.argv[1]).resolve()
web = pathlib.Path(sys.argv[2] if len(sys.argv) > 2 else pathlib.Path(__file__).resolve().parent.parent / "web").resolve()
sim = orig / "sim"
commit = subprocess.run(["git", "-C", str(orig), "rev-parse", "--short", "HEAD"], capture_output=True, text=True).stdout.strip() or "?"

(web / "sim/audio").mkdir(parents=True, exist_ok=True)
(web / "audio").mkdir(parents=True, exist_ok=True)
shutil.copyfile(sim / "audio/8_bits_only.m4a", web / "sim/audio/8_bits_only.m4a")

# ---- the audio modules, unchanged ------------------------------------------------------------------
# (file, what it is, imports of modules that are not here -> dropped)
MODULES = [
    ("audio_settings.mjs", "the audio settings (EFFECTS / MUSIC / the mixer sliders) and the effects master bus", ['import "./impact_explosion_overlay.mjs";\n']),
    ("combat_audio_bank.mjs", "the prebaked combat sound bank", []),
    ("site_config.mjs", "the site origin (music_player.mjs's fallback base URL)", []),
    ("music_player.mjs", "the title music on its own track", []),
    ("motor_sound.mjs", "the drone's motors", []),
    ("police_sirens.mjs", "the police sirens", []),
    ("jetpack_audio.mjs", "the jetpack", []),
    ("animal_audio.mjs", "the animal voices", []),
    ("immersive_soundscape.mjs", "footsteps, the car's engine and its impacts",
     ['import "./gameplay_final_runtime_v2.mjs";\n', 'import "./first_person_fire_contract_v1.mjs";\n', 'import "./gameplay_final_ui_dock.mjs";\n',
      'import "./gameplay_final_realism_patch.mjs";\n', 'import "./first_person_weapon_runtime_v3.mjs";\n', 'import "./first_person_controller_v5.mjs";\n',
      'import "./world_traffic_motion_guard_v1.mjs";\n', 'import "./world_crowd_density_v1.mjs";\n']),
    ("car_radio.mjs", "the car radio (the region's real stations)", []),
    ("world_organic_hit_audio.mjs", "the knock when a person or an animal is killed", []),
    ("headshot_feedback.mjs", "the headshot click", []),
]
for name, what, drop in MODULES:
    text = (sim / name).read_text()
    for line in drop:
        if line not in text:
            sys.exit(f"{name}: expected import not found: {line!r}")
        text = text.replace(line, "")
    note = f" except for the imports of {', '.join(re.findall(r'\"./([^\"]+)\"', ''.join(drop)))} (modules that are not on Kern)" if drop else ""
    header = (f"// Unchanged from the original SpawnCity (spawncity.de, sim/{name}, commit {commit}){note}:\n"
              f"// {what}, run on Kern by web/audio.mjs.\n")
    (web / "audio" / name).write_text(header + text)

# ---- drone_simulator.html: the menu music starts on load / the first tap ---------------------------
html = (orig / "drone_simulator.html").read_text()
start = html.index("// Menu music starts right away")
script_start = html.rindex("<script>", 0, start) + len("<script>\n")
script_end = html.index("</script>", start)
(web / "audio/menu_music.js").write_text(
    f"// Unchanged from the original SpawnCity (spawncity.de, drone_simulator.html inline script, commit\n// {commit}): the title music starts right away (or on the first tap) and the shared AudioContext with it.\n"
    + html[script_start:script_end])

# ---- drone_simulator.html: the menu's UI blip (START, HOME SCREEN, the switches, EXIT; pings) ------
start = html.index("// Menu UI click:")
script_start = html.rindex("<script>", 0, start) + len("<script>\n")
script_end = html.index("</script>", start)
(web / "audio/menu_blip.js").write_text(
    f"// Unchanged from the original SpawnCity (spawncity.de, drone_simulator.html inline script, commit\n// {commit}): the menu's click blip (window.__oppUiBlip).\n"
    + html[script_start:script_end])

# ---- the sound functions of the gameplay modules -------------------------------------------------------
# Each block: (module, prelude - the module's own names the functions read, as the module has them or
# fed from Kern (K, web/audio.mjs) - the functions / statements cut out of the source, the exports).
SETTINGS = lambda var: f"addEventListener(AUDIO_SETTINGS_EVENT,e=>{{{var}=normalizeAudioSettings(e?.detail||loadAudioSettings());}});"
UNLOCK = 'for(const t of["pointerdown","keydown"])addEventListener(t,()=>{audioUnlocked=true;},{capture:true,passive:true});'
BLOCKS = [
    ("flight_fire_fx.mjs", "let audioSettings=loadAudioSettings();" + SETTINGS("audioSettings"),
     ["ensureAudio", "shotSound", "hitConfirmSound", "damageSound"], [], ["shotSound", "hitConfirmSound", "damageSound"]),
    ("world_action_feedback.mjs", "let audioCtx=null,audioMaster=null,noiseBuffer=null,audioSettings=loadAudioSettings();" + SETTINGS("audioSettings"),
     ["ensureAudio", "audioLevel", "playShotBody", "playImpactSound"], [], ["playShotBody", "playImpactSound"]),
    ("gameplay_final_runtime_v2.mjs", "const clamp=(v,a,b)=>Math.max(a,Math.min(b,Number(v)||0));let audioSettings=loadAudioSettings();" + SETTINGS("audioSettings"),
     ["audioShot", "audioGrenadeBounce", "fxVolume", "knifeSwish", "knifeHit", "punchWhoosh", "punchThud"],
     [("glockShot", 'if(audioSettings.soundEnabled&&audioSettings.shotsVolume>0){const ctx=getSharedCombatAudioContext({resume:true});if(ctx)playCombatAudio(ctx,"pistol",{gain:.72'),
      ("sniperShot", 'if(audioSettings.soundEnabled&&audioSettings.shotsVolume>0){const ctx=getSharedCombatAudioContext({resume:true});if(ctx){playCombatAudio(ctx,"pistol",{gain:1.0')],
     ["audioShot", "audioGrenadeBounce", "knifeSwish", "knifeHit", "punchWhoosh", "punchThud", "glockShot", "sniperShot"]),
    ("hand_grenade.mjs", "", ["sfx", "pinSound", "tickSound", "whoosh"], [], ["pinSound", "tickSound", "whoosh"]),
    ("player_vehicle_runtime_v2.mjs",
     "const clamp=(v,lo,hi)=>Math.max(lo,Math.min(hi,Number(v)||0));let audioCtx=null,fxMaster=null;const bridge=()=>globalThis.__arondightRealWorld||null,walk=()=>globalThis.__arondightWalkMode||null,viewport=()=>document.getElementById(\"viewport\");",
     ["currentMode", "ensureAudio", "listenerPosition", "spatialGain", "settingsGain", "synthPistol", "playPistol", "playStep", "explosionSpatialGain", "playExplosion", "playRemoteWeapon"], [],
     ["playStep", "playExplosion", "playRemoteWeapon", "playPistol"]),
    ("gangsters.mjs", "const bridge=()=>globalThis.__arondightRealWorld||null;const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));", ["cam", "audioGain", "bang", "stabSound"], [], ["bang", "stabSound"]),
    ("police_ground_units.mjs", "const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));", ["playerTarget", "playShot"], [], ["playShot"]),
    ("military_response.mjs", "const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));", ["playerTarget", "sound"], [], ["sound"]),
    ("sandbox_game.mjs", "const bridge=()=>globalThis.__arondightRealWorld||null;const clamp=(v,a,b)=>Math.max(a,Math.min(b,Number(v)||0));let settings=normalizeAudioSettings(loadAudioSettings());" + SETTINGS("settings"),
     ["sfx"], [], ["sfx"]),
    ("zombie_nights.mjs", "const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));", ["playerTarget", "groan"],
     [("repairKnock", 'try{const c=getSharedCombatAudioContext();if(c?.state==="running")playCombatAudio(c,"bounce",{gain:.4,playbackRate:.7});}catch{}')], ["groan", "repairKnock"]),
    ("flak_battery.mjs", "const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));let settings=loadAudioSettings();addEventListener(AUDIO_SETTINGS_EVENT,e=>{settings={...settings,...(e?.detail||loadAudioSettings())};});",
     ["audioShot", "audioBurst"], [], ["audioShot", "audioBurst"]),
    ("guided_missile.mjs", "let noiseBuf=null;", ["ac", "lockTone", "noise", "motorVoice", "stopVoice", "denied"], [], ["lockTone", "motorVoice", "stopVoice", "denied"]),
    ("wanted_police_drones.mjs", "let audioUnlocked=false,audioContext=null,audioSettings=loadAudioSettings();" + SETTINGS("audioSettings") + UNLOCK, ["ensureAudio", "playTone"], [], ["playTone"]),
    ("walk_world_experience_hotfix.mjs", "const clamp=(v,lo,hi)=>Math.max(lo,Math.min(hi,Number(v)||0)),viewport=()=>document.getElementById(\"viewport\");let audioUnlocked=false,audioCtx=null;" + UNLOCK,
     ["audioSettings", "scream"], [], ["scream"]),
    ("gameplay_polish_lite.mjs", "const viewport=()=>document.getElementById(\"viewport\");", ["clamp", "fxSettings", "ensureAudio", "playCarExplosionSound", "playPersonGroan"], [],
     ["playCarExplosionSound", "playPersonGroan"]),
    ("people_impacts.mjs", "", ["thump"], [], ["thump"]),
    ("street_lamps.mjs", "", ["sfx", "glass", "clang"], [], ["glass", "clang"]),
    ("gameplay_fun_runtime.mjs", "let audioSettings=loadAudioSettings();" + SETTINGS("audioSettings"), ["playReward"], [], ["playReward"]),
    ("cows.mjs", "const bridge=()=>globalThis.__arondightRealWorld||null;let lastMoo=0;", ["thud", "moo"], [], ["thud", "moo"]),
    ("pigeons.mjs", "const bridge=()=>globalThis.__arondightRealWorld||null,rand=(a,b)=>a+Math.random()*(b-a);let audio=loadAudioSettings(),lastCoo=0;addEventListener(AUDIO_SETTINGS_EVENT,()=>{audio=loadAudioSettings();});",
     ["focus", "ctx", "level", "coo", "flapSound"], [], ["coo", "flapSound"]),
    ("jogger_chain.mjs", "const bridge=()=>globalThis.__arondightRealWorld||null;const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));", ["cam", "distGain", "stomp", "jingle"], [], ["stomp", "jingle"]),
    # street_voices.mjs: worldCameraPose(camera()) is the presented camera's position and view direction (K)
    ("street_voices.mjs",
     "const worldCameraPose=()=>K.pose();const HONK_GAP_MS=1200;let settings=loadAudioSettings(),ctx=null,bus=null,lastHonk=-1e9;const lastBySource=new Map();const camera=()=>null;addEventListener(AUDIO_SETTINGS_EVENT,()=>{settings=loadAudioSettings();});",
     ["hash", "audio", "placeListener", "hearing", "distanceTo", "honk"], [("screamBlock", "let lastScream=-1e9;"), ("scream", "export function scream(")],
     ["honk", "scream"]),
    ("vs_combat_presentation.mjs", "const viewport=()=>document.getElementById(\"viewport\");let audioContext=null,explosionNoiseBuffer=null,soundEventCount=0;",
     ["prepareExplosionNoise", "ensureAudio", "playExplosion"], [], ["playExplosion"]),
    ("fighter_jets.mjs",
     "const SPEED=255,SOUND=343;const bridge=()=>globalThis.__arondightRealWorld||null,walk=()=>globalThis.__arondightWalkMode||null;let ctx=null,master=null,jetBuffer=null;"
     "const listener=new K.V3(),tmp=new K.V3(),tmp2=new K.V3();const beepCtx=()=>{try{return getSharedCombatAudioContext({resume:true});}catch{return null;}};",
     ["ensureAudio", "jetEngineAudio", "startJetVoice", "stopJetVoice", "listenerPos", "jetPosAt", "updateJetAudio", "beep"],
     [("blastSound", 'try{const c=ensureAudio(),L=listenerPos(),d=L?L.distanceTo(p):100;if(c)setTimeout(()=>playCombatAudio(c,"explosion",{destination:master,gain:Math.min(1.6,40/(10+d)),playbackRate:.7+Math.random()*.15}),d/SOUND*1000);}catch{}')],
     ["jetEngineAudio", "startJetVoice", "stopJetVoice", "updateJetAudio", "beep", "blastSound"]),
    ("jet_mode.mjs", "const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));let voice=null,jet=null,camMode=\"cockpit\",JET_SPEC={thrustDry:1};const jetEngineAudio=(...a)=>fighter_jets.jetEngineAudio(...a);",
     ["sfx", "startVoice", "stopVoice", "updateVoice"], [], ["sfx", "startVoice", "stopVoice", "updateVoice", "setJet"]),
]
# the cut-out statements become functions of their block (their free names are the block's own)
VERBATIM = {"blastSound", "repairKnock"}
STATEMENT_ARGS = {"blastSound": "p", "repairKnock": "", "glockShot": "", "sniperShot": ""}
EXTRA = {
    "jet_mode.mjs": "function setJet(j,cam,spec){jet=j;camMode=cam;JET_SPEC=spec;}",
}

out = [f"""// The sound functions of the original SpawnCity's gameplay modules (spawncity.de, sim/*.mjs, commit
// {commit}), cut out of their source text unchanged by tools/take_original_audio.py - do not edit, run
// the tool again. One block per module: its functions as they are and the few names they read (the
// module's own settings and helpers, or what Kern feeds through K: web/audio.mjs). The game says when
// each one sounds (the game's audio events, game_audio_web.cpp); web/audio.mjs calls them.
import {{AUDIO_SETTINGS_EVENT,AUDIO_SETTINGS_KEY,loadAudioSettings,normalizeAudioSettings}} from "./audio_settings.mjs";
import {{getSharedCombatAudioContext,playCombatAudio,prepareCombatAudio}} from "./combat_audio_bank.mjs";

/** The bit of THREE.Vector3 the blocks use (fighter_jets.mjs: listener, retarded time, Doppler). */
export class V3 {{
  constructor(x = 0, y = 0, z = 0) {{ this.x = x; this.y = y; this.z = z; }}
  set(x, y, z) {{ this.x = x; this.y = y; this.z = z; return this; }}
  copy(v) {{ this.x = v.x; this.y = v.y; this.z = v.z; return this; }}
  clone() {{ return new V3(this.x, this.y, this.z); }}
  sub(v) {{ this.x -= v.x; this.y -= v.y; this.z -= v.z; return this; }}
  length() {{ return Math.hypot(this.x, this.y, this.z); }}
  normalize() {{ const l = this.length() || 1; this.x /= l; this.y /= l; this.z /= l; return this; }}
  distanceTo(v) {{ return Math.hypot(this.x - v.x, this.y - v.y, this.z - v.z); }}
}}
/** What the blocks read from Kern (web/audio.mjs sets it): the presented camera's pose. */
export const K = {{V3, pose: () => null}};
"""]
for module, prelude, fns, stmts, exports in BLOCKS:
    src = (sim / module).read_text()
    parts = []
    for f in fns:
        parts.append(jsextract.function(src, f))
    for name, prefix in stmts:
        if name == "screamBlock":
            parts.append(prefix)
            continue
        if name == "scream":
            parts.append(jsextract.function(src, "scream"))
            continue
        if name in VERBATIM:
            if src.count(prefix) != 1: sys.exit(f"{module}: statement {name} not found once")
            body = prefix
        else:
            body = jsextract.statement(src, prefix)
        parts.append(f"function {name}({STATEMENT_ARGS.get(name, '')}){{{body}}}")
    if module in EXTRA:
        parts.append(EXTRA[module])
    var = module[:-4]
    out.append(f"// ---- sim/{module}\nexport const {var} = (() => {{\n{prelude}\n" + "\n".join(parts) + f"\nreturn {{{', '.join(exports)}}};\n}})();\n")
text = "\n".join(out)
if "</script" in text:
    sys.exit("original_sfx.mjs would contain '</script'")
(web / "audio/original_sfx.mjs").write_text(text)
print(f"took the original's audio (commit {commit}) into {web}")
