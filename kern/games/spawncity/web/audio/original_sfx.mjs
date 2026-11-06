// The sound functions of the original SpawnCity's gameplay modules (spawncity.de, sim/*.mjs, commit
// 9108537), cut out of their source text unchanged by tools/take_original_audio.py - do not edit, run
// the tool again. One block per module: its functions as they are and the few names they read (the
// module's own settings and helpers, or what Kern feeds through K: web/audio.mjs). The game says when
// each one sounds (the game's audio events, game_audio_web.cpp); web/audio.mjs calls them.
import {AUDIO_SETTINGS_EVENT,AUDIO_SETTINGS_KEY,loadAudioSettings,normalizeAudioSettings} from "./audio_settings.mjs";
import {getSharedCombatAudioContext,playCombatAudio,prepareCombatAudio} from "./combat_audio_bank.mjs";

/** The bit of THREE.Vector3 the blocks use (fighter_jets.mjs: listener, retarded time, Doppler). */
export class V3 {
  constructor(x = 0, y = 0, z = 0) { this.x = x; this.y = y; this.z = z; }
  set(x, y, z) { this.x = x; this.y = y; this.z = z; return this; }
  copy(v) { this.x = v.x; this.y = v.y; this.z = v.z; return this; }
  clone() { return new V3(this.x, this.y, this.z); }
  sub(v) { this.x -= v.x; this.y -= v.y; this.z -= v.z; return this; }
  length() { return Math.hypot(this.x, this.y, this.z); }
  normalize() { const l = this.length() || 1; this.x /= l; this.y /= l; this.z /= l; return this; }
  distanceTo(v) { return Math.hypot(this.x - v.x, this.y - v.y, this.z - v.z); }
}
/** What the blocks read from Kern (web/audio.mjs sets it): the presented camera's pose. */
export const K = {V3, pose: () => null};

// ---- sim/flight_fire_fx.mjs
export const flight_fire_fx = (() => {
let audioSettings=loadAudioSettings();addEventListener(AUDIO_SETTINGS_EVENT,e=>{audioSettings=normalizeAudioSettings(e?.detail||loadAudioSettings());});
function ensureAudio(){return getSharedCombatAudioContext({resume:true});}
function shotSound(){if(!audioSettings.soundEnabled||audioSettings.shotsVolume<=0)return false;const ctx=ensureAudio();return Boolean(ctx&&playCombatAudio(ctx,"shot",{gain:.26*audioSettings.shotsVolume/100,minIntervalMs:38}));}
function hitConfirmSound(){if(!audioSettings.soundEnabled||audioSettings.fxVolume<=0)return false;const ctx=ensureAudio();return Boolean(ctx&&playCombatAudio(ctx,"hit",{gain:.23*audioSettings.fxVolume/100,minIntervalMs:28}));}
function damageSound(){if(!audioSettings.soundEnabled||audioSettings.fxVolume<=0)return false;const ctx=ensureAudio();return Boolean(ctx&&playCombatAudio(ctx,"damage",{gain:.28*audioSettings.fxVolume/100,minIntervalMs:55}));}
return {shotSound, hitConfirmSound, damageSound};
})();

// ---- sim/world_action_feedback.mjs
export const world_action_feedback = (() => {
let audioCtx=null,audioMaster=null,noiseBuffer=null,audioSettings=loadAudioSettings();addEventListener(AUDIO_SETTINGS_EVENT,e=>{audioSettings=normalizeAudioSettings(e?.detail||loadAudioSettings());});
function ensureAudio(unlock=false){
  const Ctx=globalThis.AudioContext||globalThis.webkitAudioContext;if(!Ctx)return null;
  if(!audioCtx){
    try{audioCtx=(globalThis.__sharedAudioContext??=new Ctx({latencyHint:"interactive"}));}catch{audioCtx=(globalThis.__sharedAudioContext??=new Ctx());}
    audioMaster=audioCtx.createDynamicsCompressor();audioMaster.threshold.value=-10;audioMaster.knee.value=18;audioMaster.ratio.value=3.2;audioMaster.attack.value=.002;audioMaster.release.value=.14;audioMaster.connect(audioCtx.destination);
    noiseBuffer=audioCtx.createBuffer(1,Math.max(1,Math.floor(audioCtx.sampleRate*.24)),audioCtx.sampleRate);const data=noiseBuffer.getChannelData(0);let seed=0x45a39f1;
    for(let i=0;i<data.length;i++){seed^=seed<<13;seed^=seed>>>17;seed^=seed<<5;data[i]=(((seed>>>0)/2147483648)-1)*Math.pow(1-i/data.length,.55);}
  }
  if(unlock&&audioCtx.state==="suspended")audioCtx.resume().catch(()=>{});return audioCtx;
}
function audioLevel(kind){if(!audioSettings.soundEnabled)return 0;return Math.max(0,Math.min(1,(kind==="shots"?audioSettings.shotsVolume:audioSettings.fxVolume)/100));}
function playShotBody(scale=1){
  const level=audioLevel("shots"),ctx=ensureAudio(false);if(!ctx||ctx.state!=="running"||!audioMaster||!noiseBuffer||level<=0)return false;
  try{
    const t=ctx.currentTime,body=ctx.createOscillator(),bodyGain=ctx.createGain(),crack=ctx.createBufferSource(),crackFilter=ctx.createBiquadFilter(),crackGain=ctx.createGain(),tail=ctx.createBufferSource(),tailFilter=ctx.createBiquadFilter(),tailGain=ctx.createGain();
    body.type="triangle";body.frequency.setValueAtTime(138,t);body.frequency.exponentialRampToValueAtTime(54,t+.09);bodyGain.gain.setValueAtTime(.052*level*scale,t);bodyGain.gain.exponentialRampToValueAtTime(.0001,t+.105);
    crack.buffer=noiseBuffer;crackFilter.type="bandpass";crackFilter.frequency.setValueAtTime(2900,t);crackFilter.Q.value=.7;crackGain.gain.setValueAtTime(.036*level*scale,t);crackGain.gain.exponentialRampToValueAtTime(.0001,t+.038);
    tail.buffer=noiseBuffer;tailFilter.type="lowpass";tailFilter.frequency.setValueAtTime(980,t+.018);tailFilter.frequency.exponentialRampToValueAtTime(210,t+.18);tailGain.gain.setValueAtTime(.0001,t);tailGain.gain.exponentialRampToValueAtTime(.018*level*scale,t+.025);tailGain.gain.exponentialRampToValueAtTime(.0001,t+.19);
    body.connect(bodyGain).connect(audioMaster);crack.connect(crackFilter).connect(crackGain).connect(audioMaster);tail.connect(tailFilter).connect(tailGain).connect(audioMaster);
    body.start(t);body.stop(t+.11);crack.start(t);crack.stop(t+.045);tail.start(t);tail.stop(t+.20);return true;
  }catch{return false;}
}
function playImpactSound(kind="concrete"){
  const level=audioLevel("fx"),ctx=ensureAudio(false);if(!ctx||ctx.state!=="running"||!audioMaster||!noiseBuffer||level<=0)return false;
  try{
    const t=ctx.currentTime,src=ctx.createBufferSource(),filter=ctx.createBiquadFilter(),gain=ctx.createGain();src.buffer=noiseBuffer;filter.type="bandpass";
    const profile=kind==="metal"?{f:2600,q:1.6,g:.042,d:.12}:kind==="wood"?{f:520,q:.85,g:.052,d:.105}:kind==="flesh"?{f:280,q:.7,g:.040,d:.09}:{f:980,q:.75,g:.040,d:.105};
    filter.frequency.setValueAtTime(profile.f,t);filter.Q.value=profile.q;gain.gain.setValueAtTime(profile.g*level,t);gain.gain.exponentialRampToValueAtTime(.0001,t+profile.d);src.connect(filter).connect(gain).connect(audioMaster);src.start(t);src.stop(t+profile.d+.02);
    if(kind==="metal"){const ping=ctx.createOscillator(),pg=ctx.createGain();ping.type="sine";ping.frequency.setValueAtTime(1850,t);ping.frequency.exponentialRampToValueAtTime(1120,t+.14);pg.gain.setValueAtTime(.018*level,t);pg.gain.exponentialRampToValueAtTime(.0001,t+.15);ping.connect(pg).connect(audioMaster);ping.start(t);ping.stop(t+.16);}return true;
  }catch{return false;}
}
return {playShotBody, playImpactSound};
})();

// ---- sim/gameplay_final_runtime_v2.mjs
export const gameplay_final_runtime_v2 = (() => {
const clamp=(v,a,b)=>Math.max(a,Math.min(b,Number(v)||0));let audioSettings=loadAudioSettings();addEventListener(AUDIO_SETTINGS_EVENT,e=>{audioSettings=normalizeAudioSettings(e?.detail||loadAudioSettings());});
function audioShot(gain=.22){if(!audioSettings.soundEnabled||audioSettings.shotsVolume<=0)return;const ctx=getSharedCombatAudioContext({resume:true});if(ctx)playCombatAudio(ctx,"shot",{gain:gain*audioSettings.shotsVolume/100,minIntervalMs:28});}
function audioGrenadeBounce(impactSpeed=8,bounceCount=0){if(!audioSettings.soundEnabled||audioSettings.fxVolume<=0)return;const ctx=getSharedCombatAudioContext({resume:true});if(ctx)playCombatAudio(ctx,"bounce",{gain:clamp(.16+impactSpeed*.018,.16,.44)*audioSettings.fxVolume/100,playbackRate:clamp(.92+bounceCount*.035+.02*Math.min(impactSpeed,10),.88,1.32),minIntervalMs:42});}
function fxVolume(){return audioSettings.soundEnabled?Math.max(0,Number(audioSettings.fxVolume)||0)/100:0;}
function knifeSwish(){const a=globalThis.__sharedAudioContext;if(!a||a.state!=="running")return;try{const t=a.currentTime,dur=.13,n=a.createBuffer(1,Math.ceil(a.sampleRate*dur),a.sampleRate),ch=n.getChannelData(0);for(let i=0;i<ch.length;i++){const u=i/ch.length;ch[i]=(Math.random()*2-1)*Math.sin(Math.PI*u)**2;}const s=a.createBufferSource(),bp=a.createBiquadFilter(),g=a.createGain();bp.type="bandpass";bp.Q.value=2.2;bp.frequency.setValueAtTime(2400,t);bp.frequency.exponentialRampToValueAtTime(6800,t+dur);g.gain.value=.34*fxVolume();s.buffer=n;s.connect(bp).connect(g).connect(a.destination);s.start(t);}catch{}}
function knifeHit(flesh){const a=globalThis.__sharedAudioContext;if(!a||a.state!=="running")return;try{const t=a.currentTime+.04,v=fxVolume();if(flesh){const o=a.createOscillator(),g=a.createGain();o.type="sine";o.frequency.setValueAtTime(120,t);o.frequency.exponentialRampToValueAtTime(50,t+.18);g.gain.setValueAtTime(.55*v,t);g.gain.exponentialRampToValueAtTime(.001,t+.22);o.connect(g).connect(a.destination);o.start(t);o.stop(t+.25);}
  else{for(const f of[2900,4300]){const o=a.createOscillator(),g=a.createGain();o.type="triangle";o.frequency.value=f;g.gain.setValueAtTime(.08*v,t);g.gain.exponentialRampToValueAtTime(.001,t+.25);o.connect(g).connect(a.destination);o.start(t);o.stop(t+.3);}}}catch{}}
function punchWhoosh(h,combo){const a=globalThis.__sharedAudioContext;if(!a||a.state!=="running")return;try{const t=a.currentTime,dur=h?.14:.2,n=a.createBuffer(1,Math.ceil(a.sampleRate*dur),a.sampleRate),ch=n.getChannelData(0);for(let i=0;i<ch.length;i++){const u=i/ch.length;ch[i]=(Math.random()*2-1)*Math.sin(Math.PI*u)**1.5;}
  const src=a.createBufferSource(),bp=a.createBiquadFilter(),g=a.createGain();src.buffer=n;bp.type="bandpass";bp.Q.value=1.6;bp.frequency.setValueAtTime(h?700:500,t);bp.frequency.exponentialRampToValueAtTime(h?2600:2000,t+dur*.6);bp.frequency.exponentialRampToValueAtTime(900,t+dur);g.gain.value=(h?.16:.22)*(combo?1.15:1);src.connect(bp).connect(g).connect(a.destination);src.start(t);}catch{}}
function punchThud(h,combo){const a=globalThis.__sharedAudioContext;if(!a||a.state!=="running")return;try{const t=a.currentTime,o=a.createOscillator(),g=a.createGain(),n=a.createBufferSource(),buf=a.createBuffer(1,a.sampleRate*.08,a.sampleRate),ch=buf.getChannelData(0);for(let i=0;i<ch.length;i++)ch[i]=(Math.random()*2-1)*(1-i/ch.length);n.buffer=buf;const lp=a.createBiquadFilter();lp.type="lowpass";lp.frequency.value=900;const ng=a.createGain();ng.gain.value=h?.18:.26;n.connect(lp).connect(ng).connect(a.destination);n.start(t);
  {const sm=a.createBufferSource(),sb=a.createBuffer(1,Math.ceil(a.sampleRate*.035),a.sampleRate),sd=sb.getChannelData(0);for(let i=0;i<sd.length;i++)sd[i]=(Math.random()*2-1)*Math.pow(1-i/sd.length,2);sm.buffer=sb;const hp=a.createBiquadFilter();hp.type="bandpass";hp.frequency.value=1400;hp.Q.value=.9;const sg=a.createGain();sg.gain.value=h?.32:.42;sm.connect(hp).connect(sg).connect(a.destination);sm.start(t);} // the slap of the knuckles
  o.type="sine";o.frequency.setValueAtTime(h?120:95,t);o.frequency.exponentialRampToValueAtTime(45,t+.12);g.gain.setValueAtTime((h?.35:.5)*(combo?1.2:1),t);g.gain.exponentialRampToValueAtTime(.001,t+.16);o.connect(g).connect(a.destination);o.start(t);o.stop(t+.18);}catch{}}
function glockShot(){if(audioSettings.soundEnabled&&audioSettings.shotsVolume>0){const ctx=getSharedCombatAudioContext({resume:true});if(ctx)playCombatAudio(ctx,"pistol",{gain:.72*audioSettings.shotsVolume/100,playbackRate:1,minIntervalMs:0});}}
function sniperShot(){if(audioSettings.soundEnabled&&audioSettings.shotsVolume>0){const ctx=getSharedCombatAudioContext({resume:true});if(ctx){playCombatAudio(ctx,"pistol",{gain:1.0*audioSettings.shotsVolume/100,playbackRate:.52,minIntervalMs:0});playCombatAudio(ctx,"pistol",{gain:.55*audioSettings.shotsVolume/100,playbackRate:.36,minIntervalMs:0});}}}
return {audioShot, audioGrenadeBounce, knifeSwish, knifeHit, punchWhoosh, punchThud, glockShot, sniperShot};
})();

// ---- sim/hand_grenade.mjs
export const hand_grenade = (() => {

function sfx(fn){const a=globalThis.__sharedAudioContext;if(!a||a.state!=="running")return;try{fn(a,a.currentTime);}catch{}}
function pinSound(){sfx((a,t)=>{for(const[f,d,g]of[[3100,.09,.16],[4700,.05,.08]]){const o=a.createOscillator(),k=a.createGain();o.type="triangle";o.frequency.setValueAtTime(f,t);o.frequency.exponentialRampToValueAtTime(f*.86,t+d);k.gain.setValueAtTime(g,t);k.gain.exponentialRampToValueAtTime(.001,t+d+.05);o.connect(k).connect(a.destination);o.start(t);o.stop(t+d+.08);}
  const o=a.createOscillator(),k=a.createGain();o.type="square";o.frequency.value=900;k.gain.setValueAtTime(.08,t+.16);k.gain.exponentialRampToValueAtTime(.001,t+.2);o.connect(k).connect(a.destination);o.start(t+.16);o.stop(t+.22);});}
function tickSound(urgent){sfx((a,t)=>{const o=a.createOscillator(),k=a.createGain();o.type="square";o.frequency.value=urgent?1500:1100;k.gain.setValueAtTime(urgent?.07:.045,t);k.gain.exponentialRampToValueAtTime(.001,t+.04);o.connect(k).connect(a.destination);o.start(t);o.stop(t+.05);});}
function whoosh(){sfx((a,t)=>{const n=a.createBuffer(1,a.sampleRate*.25,a.sampleRate),d=n.getChannelData(0);for(let i=0;i<d.length;i++)d[i]=(Math.random()*2-1)*Math.sin(Math.PI*i/d.length);const s=a.createBufferSource(),f=a.createBiquadFilter(),k=a.createGain();s.buffer=n;f.type="bandpass";f.frequency.setValueAtTime(600,t);f.frequency.exponentialRampToValueAtTime(1800,t+.2);k.gain.value=.22;s.connect(f).connect(k).connect(a.destination);s.start(t);});}
return {pinSound, tickSound, whoosh};
})();

// ---- sim/player_vehicle_runtime_v2.mjs
export const player_vehicle_runtime_v2 = (() => {
const clamp=(v,lo,hi)=>Math.max(lo,Math.min(hi,Number(v)||0));let audioCtx=null,fxMaster=null;const bridge=()=>globalThis.__arondightRealWorld||null,walk=()=>globalThis.__arondightWalkMode||null,viewport=()=>document.getElementById("viewport");
function currentMode(){return walk()?.mode==="foot"?"foot":"drone";}
function ensureAudio(unlock=false){const Ctx=globalThis.AudioContext||globalThis.webkitAudioContext;if(!Ctx)return null;if(!audioCtx){try{audioCtx=(globalThis.__sharedAudioContext??=new Ctx({latencyHint:"interactive"}));}catch{audioCtx=(globalThis.__sharedAudioContext??=new Ctx());}fxMaster=audioCtx.createDynamicsCompressor();fxMaster.threshold.value=-12;fxMaster.knee.value=18;fxMaster.ratio.value=3.5;fxMaster.attack.value=.002;fxMaster.release.value=.16;fxMaster.connect(audioCtx.destination);prepareCombatAudio(audioCtx);}if(unlock&&audioCtx.state==="suspended")audioCtx.resume().catch(()=>{});return audioCtx;}
function listenerPosition(){if(currentMode()==="foot"&&walk()?.position)return walk().position;return bridge()?.threeCamera?.position||null;}
function spatialGain(position){if(!position)return 1;const listener=listenerPosition();if(!listener)return 1;const p=Array.isArray(position)?position:[position.x,position.y,position.z],d=Math.hypot((p[0]||0)-listener.x,(p[1]||0)-listener.y,(p[2]||0)-listener.z);return 1/(1+.10*d+.018*d*d);}
function settingsGain(kind){const s=loadAudioSettings();if(!s.soundEnabled)return 0;return clamp((kind==="shots"?s.shotsVolume:s.fxVolume)/100,0,1);}
function synthPistol(volume){if(!audioCtx||audioCtx.state!=="running"||!fxMaster||volume<=0)return false;const played=playCombatAudio(audioCtx,"shot",{destination:fxMaster,gain:.44*volume,minIntervalMs:38}),v=viewport();if(played&&v)v.dataset.walkShotSamplePlays=String((Number(v.dataset.walkShotSamplePlays)||0)+1);return played;}
function playPistol(position=null,remote=false){const ctx=ensureAudio(true),volume=settingsGain("shots")*spatialGain(position)*(remote?.68:1),v=viewport();if(v)v.dataset.walkShotSoundEvents=String((Number(v.dataset.walkShotSoundEvents)||0)+1);if(!ctx||volume<=0)return false;if(ctx.state==="running")synthPistol(volume);else ctx.resume().then(()=>synthPistol(volume)).catch(()=>{});return true;}
function playStep(position=null,remote=false){const ctx=ensureAudio(false),volume=settingsGain("fx")*spatialGain(position)*(remote?.18:.12);if(!ctx||ctx.state!=="running"||!fxMaster||volume<=0)return false;return playCombatAudio(ctx,"step",{destination:fxMaster,gain:volume,minIntervalMs:55});}
function explosionSpatialGain(position){if(!position)return 1;const listener=listenerPosition();if(!listener)return 1;const p=Array.isArray(position)?position:[position.x,position.y,position.z],d=Math.hypot((p[0]||0)-listener.x,(p[1]||0)-listener.y,(p[2]||0)-listener.z);return Math.min(1,24/(12+d));}
function playExplosion(position=null,small=false){const ctx=ensureAudio(false),volume=settingsGain("fx")*explosionSpatialGain(position)*(small?.36:1),v=viewport();if(v)v.dataset.playerExplosionSoundEvents=String((Number(v.dataset.playerExplosionSoundEvents)||0)+1);if(!ctx||ctx.state!=="running"||!fxMaster||volume<=0)return false;return playCombatAudio(ctx,"explosion",{destination:fxMaster,gain:(small?.30:.52)*volume,minIntervalMs:70});}
function playRemoteWeapon(position,weapon){const ctx=ensureAudio(false),volume=settingsGain("shots")*spatialGain(position)*.75;if(!ctx||ctx.state!=="running"||!fxMaster||volume<=0)return false;return playCombatAudio(ctx,weapon==="glock"||weapon==="pistol"?"pistol":"shot",{destination:fxMaster,gain:(weapon==="grenade"?.5:.42)*volume,playbackRate:weapon==="grenade"?.62:1,minIntervalMs:30});}
return {playStep, playExplosion, playRemoteWeapon, playPistol};
})();

// ---- sim/gangsters.mjs
export const gangsters = (() => {
const bridge=()=>globalThis.__arondightRealWorld||null;const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));
function cam(){const b=bridge();return b?.presentedCamera?.()||b?.threeCamera||null;}
function audioGain(p,base){const c=cam();const d=c?Math.hypot(p[0]-c.position.x,p[1]-c.position.y,p[2]-c.position.z):20;return clamp(base/(1+d*.06),.03,base);}
function bang(p){try{const ctx=getSharedCombatAudioContext();if(!ctx||ctx.state!=="running")return;playCombatAudio(ctx,"pistol",{gain:audioGain(p,.6),playbackRate:.92+Math.random()*.1,minIntervalMs:25});}catch{}}
function stabSound(p){const c=globalThis.__sharedAudioContext;if(!c||c.state!=="running")return;try{const t=c.currentTime,out=c.createGain();out.gain.value=audioGain(p,.7);out.connect(c.destination);
  const n=c.createBuffer(1,c.sampleRate*.22,c.sampleRate),ch=n.getChannelData(0);for(let i=0;i<ch.length;i++)ch[i]=(Math.random()*2-1)*Math.sin(Math.PI*i/ch.length);const s=c.createBufferSource(),bp=c.createBiquadFilter();bp.type="bandpass";bp.Q.value=1.4;bp.frequency.setValueAtTime(1800,t);bp.frequency.exponentialRampToValueAtTime(5200,t+.16);s.buffer=n;s.connect(bp).connect(out);s.start(t);
  const o=c.createOscillator(),g=c.createGain();o.type="sine";o.frequency.setValueAtTime(130,t+.12);o.frequency.exponentialRampToValueAtTime(55,t+.3);g.gain.setValueAtTime(0,t);g.gain.setValueAtTime(.8,t+.12);g.gain.exponentialRampToValueAtTime(.001,t+.34);o.connect(g).connect(out);o.start(t+.1);o.stop(t+.36);}catch{}}
return {bang, stabSound};
})();

// ---- sim/police_ground_units.mjs
export const police_ground_units = (() => {
const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));
function playerTarget(){const t=globalThis.__arondightPlayerVitals?.damageTargets?.()?.find(x=>x.kind==="player");return t&&Number(t.hp)>0?t:null;}
function playShot(x,y,z){try{const ctx=getSharedCombatAudioContext();if(!ctx||ctx.state!=="running")return;const pl=playerTarget()?.position,d=pl?Math.hypot(pl.x-x,pl.y-y,(pl.z||0)-z):30;playCombatAudio(ctx,"pistol",{gain:clamp(.55/(1+d*.06),.04,.5),minIntervalMs:25});}catch{}}
return {playShot};
})();

// ---- sim/military_response.mjs
export const military_response = (() => {
const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));
function playerTarget(){return globalThis.__arondightPlayerVitals?.damageTargets?.()?.find?.(t=>t.kind==="drone")||globalThis.__arondightPlayerVitals?.damageTargets?.()?.find?.(t=>t.kind==="player")||null;}
function sound(kind,x,y,z,gain=.5,rate=1){try{const c=getSharedCombatAudioContext();if(!c||c.state!=="running")return;const pl=playerTarget()?.position,d=pl?Math.hypot(pl.x-x,pl.y-y,(pl.z||0)-z):60;playCombatAudio(c,kind,{gain:clamp(gain*40/(25+d),.03,gain),playbackRate:rate,minIntervalMs:30});}catch{}}
return {sound};
})();

// ---- sim/sandbox_game.mjs
export const sandbox_game = (() => {
const bridge=()=>globalThis.__arondightRealWorld||null;const clamp=(v,a,b)=>Math.max(a,Math.min(b,Number(v)||0));let settings=normalizeAudioSettings(loadAudioSettings());addEventListener(AUDIO_SETTINGS_EVENT,e=>{settings=normalizeAudioSettings(e?.detail||loadAudioSettings());});
function sfx(kind,{gain=1,rate=1,at=null}={}){
  if(settings.soundEnabled===false)return;let att=1;const c=bridge()?.threeCamera;if(at&&c)att=clamp(1-c.position.distanceTo(at)/140,.05,1);
  const ctx=getSharedCombatAudioContext();if(ctx)playCombatAudio(ctx,kind,{gain:gain*att*(Number(settings.fxVolume)||70)/100,playbackRate:rate,minIntervalMs:28});
}
return {sfx};
})();

// ---- sim/zombie_nights.mjs
export const zombie_nights = (() => {
const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));
function playerTarget(){const t=globalThis.__arondightPlayerVitals?.damageTargets?.()?.find(x=>x.kind==="player");return t&&Number(t.hp)>0?t:null;}
function groan(z,now){if(now-lastGroan<900)return;const pl=playerTarget()?.position;if(!pl)return;const d=Math.hypot(pl.x-z.x,pl.y-z.y);if(d>30)return;lastGroan=now;try{const c=getSharedCombatAudioContext();if(c?.state==="running")playCombatAudio(c,"scream",{gain:clamp(.25/(1+d*.12),.02,.22),playbackRate:.42+Math.random()*.12,minIntervalMs:400});}catch{}}
function repairKnock(){try{const c=getSharedCombatAudioContext();if(c?.state==="running")playCombatAudio(c,"bounce",{gain:.4,playbackRate:.7});}catch{}}
return {groan, repairKnock};
})();

// ---- sim/flak_battery.mjs
export const flak_battery = (() => {
const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));let settings=loadAudioSettings();addEventListener(AUDIO_SETTINGS_EVENT,e=>{settings={...settings,...(e?.detail||loadAudioSettings())};});
function audioShot(gun,dist){if(!settings.soundEnabled)return;const ctx=getSharedCombatAudioContext();if(!ctx||ctx.state!=="running")return;const level=clamp((Number(settings.fxVolume)||70)/72,0,1.4)*clamp(1.4/(1+dist/90),0,1.2);if(level<.01)return;const t=ctx.currentTime;
  const out=ctx.createGain();out.gain.value=level*.55;out.connect(ctx.destination);
  // thump: a falling sine kick
  const o=ctx.createOscillator(),og=ctx.createGain();o.type="sine";o.frequency.setValueAtTime(95,t);o.frequency.exponentialRampToValueAtTime(38,t+.11);og.gain.setValueAtTime(1,t);og.gain.exponentialRampToValueAtTime(.001,t+.16);o.connect(og).connect(out);o.start(t);o.stop(t+.18);
  // crack: short filtered noise, duller far away
  const n=ctx.createBufferSource(),buf=audioShot.buf||(audioShot.buf=(()=>{const b=ctx.createBuffer(1,ctx.sampleRate*.12|0,ctx.sampleRate),d=b.getChannelData(0);for(let i=0;i<d.length;i++)d[i]=(Math.random()*2-1)*Math.exp(-i/(d.length*.18));return b;})());n.buffer=buf;const f=ctx.createBiquadFilter();f.type="lowpass";f.frequency.value=clamp(9000-dist*7,900,9000);const ng=ctx.createGain();ng.gain.value=.9;n.connect(f).connect(ng).connect(out);n.start(t);}
function audioBurst(at,dist){if(!settings.soundEnabled)return;const ctx=getSharedCombatAudioContext();if(!ctx||ctx.state!=="running")return;const level=clamp((Number(settings.fxVolume)||70)/72,0,1.4)*clamp(1/(1+dist/60),0,1);if(level<.02)return;const t=ctx.currentTime+dist/343;const o=ctx.createOscillator(),g=ctx.createGain();o.type="triangle";o.frequency.setValueAtTime(70,t);o.frequency.exponentialRampToValueAtTime(30,t+.25);g.gain.setValueAtTime(level*.6,t);g.gain.exponentialRampToValueAtTime(.001,t+.3);o.connect(g).connect(ctx.destination);o.start(t);o.stop(t+.32);}
return {audioShot, audioBurst};
})();

// ---- sim/guided_missile.mjs
export const guided_missile = (() => {
let noiseBuf=null;
function ac(){const a=globalThis.__sharedAudioContext;return a&&a.state==="running"?a:null;}
function lockTone(){const a=ac();if(!a)return;try{const t=a.currentTime;for(let k=0;k<2;k++){const o=a.createOscillator(),g=a.createGain();o.type="square";o.frequency.value=1760;const s=t+k*.09;g.gain.setValueAtTime(.05,s);g.gain.setValueAtTime(0,s+.05);o.connect(g).connect(a.destination);o.start(s);o.stop(s+.06);}}catch{}}
function noise(a){if(noiseBuf&&noiseBuf.sampleRate===a.sampleRate)return noiseBuf;const n=a.createBuffer(1,a.sampleRate*2,a.sampleRate),d=n.getChannelData(0);let b=0;for(let i=0;i<d.length;i++){const w=Math.random()*2-1;b=.96*b+.04*w;d[i]=w*.5+b*2;}return noiseBuf=n;}
function motorVoice(){const a=ac();if(!a)return null;try{const t=a.currentTime,src=a.createBufferSource();src.buffer=noise(a);src.loop=true;const bp=a.createBiquadFilter();bp.type="bandpass";bp.frequency.value=700;bp.Q.value=.7;const whine=a.createOscillator();whine.type="sawtooth";whine.frequency.value=1300;const wg=a.createGain();wg.gain.value=.05;const g=a.createGain();g.gain.setValueAtTime(0,t);g.gain.linearRampToValueAtTime(.5,t+.06);g.gain.exponentialRampToValueAtTime(.28,t+.6);const pan=a.createPanner();pan.panningModel="equalpower";pan.distanceModel="inverse";pan.refDistance=6;pan.rolloffFactor=1.05;
    src.connect(bp).connect(g);whine.connect(wg).connect(g);g.connect(pan).connect(a.destination);src.start(t);whine.start(t);
    // ignition bang
    const o=a.createOscillator(),og=a.createGain();o.type="sine";o.frequency.setValueAtTime(110,t);o.frequency.exponentialRampToValueAtTime(40,t+.3);og.gain.setValueAtTime(.6,t);og.gain.exponentialRampToValueAtTime(.001,t+.35);o.connect(og).connect(a.destination);o.start(t);o.stop(t+.4);
    return{src,whine,g,pan,bp};}catch{return null;}}
function stopVoice(v){if(!v)return;const a=ac();try{const t=a?a.currentTime:0;v.g.gain.cancelScheduledValues(t);v.g.gain.setTargetAtTime(0,t,.05);setTimeout(()=>{try{v.src.stop();v.whine.stop();}catch{}},400);}catch{}}
function denied(){const a=ac();if(a)try{const t=a.currentTime,o=a.createOscillator(),g=a.createGain();o.type="square";o.frequency.setValueAtTime(330,t);o.frequency.setValueAtTime(220,t+.07);g.gain.setValueAtTime(.05,t);g.gain.exponentialRampToValueAtTime(.001,t+.16);o.connect(g).connect(a.destination);o.start(t);o.stop(t+.18);}catch{}
  const v=viewport();if(v){v.dataset.guidedDenied=String(performance.now()|0);document.body.classList.add("guided-denied");clearTimeout(denied.t);denied.t=setTimeout(()=>document.body.classList.remove("guided-denied"),900);}}
return {lockTone, motorVoice, stopVoice, denied};
})();

// ---- sim/wanted_police_drones.mjs
export const wanted_police_drones = (() => {
let audioUnlocked=false,audioContext=null,audioSettings=loadAudioSettings();addEventListener(AUDIO_SETTINGS_EVENT,e=>{audioSettings=normalizeAudioSettings(e?.detail||loadAudioSettings());});for(const t of["pointerdown","keydown"])addEventListener(t,()=>{audioUnlocked=true;},{capture:true,passive:true});
function ensureAudio(){
  const Ctx=globalThis.AudioContext||globalThis.webkitAudioContext;
  if(!audioUnlocked||!Ctx)return null;
  try{audioContext??=(globalThis.__sharedAudioContext??=new Ctx({latencyHint:"interactive"}));if(audioContext.state==="suspended")audioContext.resume().catch(()=>{});}catch{return null;}
  return audioContext;
}
function playTone({frequency=700,endFrequency=frequency,duration=.12,gain=.02,type="sine"}={}){
  if(!audioSettings.soundEnabled||audioSettings.fxVolume<=0)return false;
  const context=ensureAudio();
  if(!context||context.state!=="running")return false;
  try{
    const now=context.currentTime,oscillator=context.createOscillator(),volume=context.createGain();
    oscillator.type=type;oscillator.frequency.setValueAtTime(Math.max(30,frequency),now);oscillator.frequency.exponentialRampToValueAtTime(Math.max(30,endFrequency),now+duration);
    volume.gain.setValueAtTime(Math.max(.0001,gain*audioSettings.fxVolume/100),now);volume.gain.exponentialRampToValueAtTime(.0001,now+duration);
    oscillator.connect(volume).connect(context.destination);oscillator.start(now);oscillator.stop(now+duration+.02);return true;
  }catch{return false;}
}
return {playTone};
})();

// ---- sim/walk_world_experience_hotfix.mjs
export const walk_world_experience_hotfix = (() => {
const clamp=(v,lo,hi)=>Math.max(lo,Math.min(hi,Number(v)||0)),viewport=()=>document.getElementById("viewport");let audioUnlocked=false,audioCtx=null;for(const t of["pointerdown","keydown"])addEventListener(t,()=>{audioUnlocked=true;},{capture:true,passive:true});
function audioSettings(){try{const raw=localStorage.getItem(AUDIO_SETTINGS_KEY),v=raw?JSON.parse(raw):{};return{enabled:v.soundEnabled!==false,volume:clamp(v.fxVolume??100,0,100)/100};}catch{return{enabled:true,volume:1};}}
function scream(strength=1){const settings=audioSettings();if(!audioUnlocked||!settings.enabled||settings.volume<=0)return false;audioCtx??=getSharedCombatAudioContext({resume:true});if(!audioCtx)return false;const played=playCombatAudio(audioCtx,"scream",{gain:.30*settings.volume*clamp(strength,.35,1.2),minIntervalMs:105}),v=viewport();if(played&&v)v.dataset.worldPersonScreams=String((Number(v.dataset.worldPersonScreams)||0)+1);return played;}
return {scream};
})();

// ---- sim/gameplay_polish_lite.mjs
export const gameplay_polish_lite = (() => {
const viewport=()=>document.getElementById("viewport");
function clamp(v,lo,hi){return Math.max(lo,Math.min(hi,Number(v)||0));}
function fxSettings(){try{const raw=localStorage.getItem(AUDIO_SETTINGS_KEY),v=raw?JSON.parse(raw):{};return{enabled:v.soundEnabled!==false,volume:clamp(v.fxVolume??100,0,100)/100};}catch{return{enabled:true,volume:1};}}
function ensureAudio(){return getSharedCombatAudioContext({resume:true});}
function playCarExplosionSound(){const s=fxSettings(),ctx=ensureAudio();if(!s.enabled||s.volume<=0||!ctx)return false;const played=playCombatAudio(ctx,"explosion",{gain:.45*s.volume,minIntervalMs:90}),v=viewport();if(played&&v)v.dataset.worldCarExplosionSounds=String((Number(v.dataset.worldCarExplosionSounds)||0)+1);return played;}
function playPersonGroan(){const s=fxSettings(),ctx=ensureAudio();if(!s.enabled||s.volume<=0||!ctx)return false;const played=playCombatAudio(ctx,"scream",{gain:.30*s.volume,minIntervalMs:105}),v=viewport();if(played&&v){v.dataset.worldPersonGroans=String((Number(v.dataset.worldPersonGroans)||0)+1);v.dataset.worldPersonScreams=String((Number(v.dataset.worldPersonScreams)||0)+1);}return played;}
return {playCarExplosionSound, playPersonGroan};
})();

// ---- sim/people_impacts.mjs
export const people_impacts = (() => {

function thump(gain){try{const c=getSharedCombatAudioContext();if(c?.state==="running")playCombatAudio(c,"bounce",{gain,playbackRate:.62,minIntervalMs:60});}catch{}}
return {thump};
})();

// ---- sim/street_lamps.mjs
export const street_lamps = (() => {

function sfx(pos,fn){const c=globalThis.__sharedAudioContext;if(!c||c.state!=="running")return;try{const cam=globalThis.__arondightRealWorld?.presentedCamera?.()||globalThis.__arondightRealWorld?.threeCamera,d=cam?Math.hypot(pos[0]-cam.position.x,pos[1]-cam.position.y,pos[2]-cam.position.z):10,g=c.createGain();g.gain.value=Math.max(0,1-d/90)*.5;g.connect(c.destination);fn(c,g,c.currentTime);}catch{}}
function glass(pos){sfx(pos,(c,out,t)=>{const n=c.createBuffer(1,c.sampleRate*.4,c.sampleRate),ch=n.getChannelData(0);for(let i=0;i<ch.length;i++)ch[i]=(Math.random()*2-1)*Math.pow(1-i/ch.length,3)*(Math.random()<.04?3:1);const s=c.createBufferSource(),hp=c.createBiquadFilter();hp.type="highpass";hp.frequency.value=3200;s.buffer=n;s.connect(hp).connect(out);s.start(t);});}
function clang(pos,k=1){sfx(pos,(c,out,t)=>{for(const f of[180,412,733]){const o=c.createOscillator(),g=c.createGain();o.type="triangle";o.frequency.value=f*(.97+Math.random()*.06);g.gain.setValueAtTime(.32*k,t);g.gain.exponentialRampToValueAtTime(.001,t+.9*k+.2);o.connect(g).connect(out);o.start(t);o.stop(t+1.2);}});}
return {glass, clang};
})();

// ---- sim/gameplay_fun_runtime.mjs
export const gameplay_fun_runtime = (() => {
let audioSettings=loadAudioSettings();addEventListener(AUDIO_SETTINGS_EVENT,e=>{audioSettings=normalizeAudioSettings(e?.detail||loadAudioSettings());});
function playReward(kind,gain){if(!audioSettings.soundEnabled||audioSettings.fxVolume<=0)return false;const context=getSharedCombatAudioContext({resume:true});return Boolean(context&&playCombatAudio(context,kind,{gain:gain*audioSettings.fxVolume/100,minIntervalMs:60}));}
return {playReward};
})();

// ---- sim/cows.mjs
export const cows = (() => {
const bridge=()=>globalThis.__arondightRealWorld||null;let lastMoo=0;
function thud(pos,soft){const a=globalThis.__sharedAudioContext;if(!a||a.state!=="running")return;try{const t=a.currentTime,o=a.createOscillator(),g=a.createGain();o.type="sine";o.frequency.setValueAtTime(soft?90:140,t);o.frequency.exponentialRampToValueAtTime(45,t+.12);g.gain.setValueAtTime(soft?.4:.28,t);g.gain.exponentialRampToValueAtTime(.001,t+.16);o.connect(g).connect(a.destination);o.start(t);o.stop(t+.18);}catch{}}
function moo(pos,k=1,{pain=false}={}){const a=globalThis.__sharedAudioContext;if(!a||a.state!=="running")return;const now=performance.now();if(!pain&&now-lastMoo<2500&&k<.7)return;if(pain&&now-lastMoo<350)return;lastMoo=now;
  try{const cam=bridge()?.presentedCamera?.()||bridge()?.threeCamera;const t=a.currentTime,pan=a.createPanner();pan.panningModel="equalpower";pan.distanceModel="inverse";pan.refDistance=5;pan.rolloffFactor=1.2;if(pan.positionX){pan.positionX.value=pos[0];pan.positionY.value=pos[1];pan.positionZ.value=pos[2];}
    if(cam&&a.listener.positionX){a.listener.positionX.value=cam.position.x;a.listener.positionY.value=cam.position.y;a.listener.positionZ.value=cam.position.z;}
    const g=a.createGain(),dur=pain?.55+Math.random()*.25:1.1+Math.random()*.6,f0=(105+Math.random()*30)*(pain?1.45:1);g.gain.setValueAtTime(0,t);g.gain.linearRampToValueAtTime(.32*k,t+.18);g.gain.setValueAtTime(.3*k,t+dur*.7);g.gain.linearRampToValueAtTime(0,t+dur);
    const f1=a.createBiquadFilter(),f2=a.createBiquadFilter();f1.type="bandpass";f1.frequency.setValueAtTime(380,t);f1.frequency.linearRampToValueAtTime(720,t+dur*.4);f1.Q.value=4;f2.type="lowpass";f2.frequency.value=1400;
    for(const det of[0,7]){const o=a.createOscillator();o.type="sawtooth";o.frequency.setValueAtTime(f0*.9,t);o.frequency.linearRampToValueAtTime(f0,t+.25);o.frequency.linearRampToValueAtTime(f0*.82,t+dur);o.detune.value=det;o.connect(f1);o.start(t);o.stop(t+dur+.05);}
    f1.connect(f2).connect(g).connect(pan).connect(a.destination);}catch{}}
return {thud, moo};
})();

// ---- sim/pigeons.mjs
export const pigeons = (() => {
const bridge=()=>globalThis.__arondightRealWorld||null,rand=(a,b)=>a+Math.random()*(b-a);let audio=loadAudioSettings(),lastCoo=0;addEventListener(AUDIO_SETTINGS_EVENT,()=>{audio=loadAudioSettings();});
function focus(){const drive=globalThis.__arondightVehicleDrive,walk=globalThis.__arondightWalkMode;if(drive?.active&&drive.cameraAnchor)return drive.cameraAnchor;if(walk?.mode==="foot"&&walk.position)return walk.position;const b=bridge(),c=b?.presentedCamera?.()||b?.threeCamera;return c?.position||null;}
function ctx(){const a=globalThis.__sharedAudioContext;return a&&a.state==="running"?a:null;}
function level(){if(audio.soundEnabled===false)return 0;return Math.max(0,Math.min(1,(Number(audio.ambientVolume)||46)/100));}
function coo(x,y,dist){const a=ctx(),L=level();if(!a||!L)return;const now=performance.now();if(now-lastCoo<1600)return;lastCoo=now;
  try{const t=a.currentTime,out=a.createGain(),vol=.11*L*Math.max(.15,1-dist/30);out.gain.value=vol;out.connect(a.destination);
    // "gruu-gurr-ruu": two or three rolling notes, each a sine with a fast amplitude trill and a falling pitch
    const notes=Math.random()<.5?[[0,.32,330,270],[.38,.5,300,240]]:[[0,.26,320,280],[.3,.24,345,300],[.6,.55,310,235]];
    for(const[st,d,f0,f1]of notes){const o=a.createOscillator(),o2=a.createOscillator(),g=a.createGain(),trem=a.createOscillator(),tg=a.createGain(),lp=a.createBiquadFilter();
      o.type="sine";o2.type="triangle";o.frequency.setValueAtTime(f0,t+st);o.frequency.exponentialRampToValueAtTime(f1,t+st+d);o2.frequency.setValueAtTime(f0*2.01,t+st);o2.frequency.exponentialRampToValueAtTime(f1*2.01,t+st+d);
      lp.type="lowpass";lp.frequency.value=900;trem.frequency.value=rand(22,30);tg.gain.value=.45;trem.connect(tg).connect(g.gain);
      g.gain.setValueAtTime(0,t+st);g.gain.linearRampToValueAtTime(.55,t+st+.05);g.gain.setValueAtTime(.5,t+st+d*.7);g.gain.linearRampToValueAtTime(0,t+st+d);
      const g2=a.createGain();g2.gain.value=.25;o.connect(g);o2.connect(g2).connect(g);g.connect(lp).connect(out);
      for(const n of[o,o2,trem]){n.start(t+st);n.stop(t+st+d+.02);}}
  }catch{}}
function flapSound(x,y){const a=ctx(),L=level(),f=focus();if(!a||!L||!f)return;const dist=Math.hypot(f.x-x,f.y-y);if(dist>45)return;
  try{const t=a.currentTime,len=Math.floor(a.sampleRate*.9),buf=a.createBuffer(1,len,a.sampleRate),d=buf.getChannelData(0);
    // a burst of wing claps: many short noise pulses, dense at first, thinning out
    for(let i=0;i<len;i++){const s=i/a.sampleRate,rate=26-18*s,ph=(s*rate)%1;d[i]=(Math.random()*2-1)*Math.exp(-ph*28)*Math.exp(-s*2.2);}
    const src=a.createBufferSource(),bp=a.createBiquadFilter(),g=a.createGain();src.buffer=buf;bp.type="bandpass";bp.frequency.value=1400;bp.Q.value=.7;g.gain.value=.35*L*Math.max(.12,1-dist/45);src.connect(bp).connect(g).connect(a.destination);src.start(t);}catch{}}
return {coo, flapSound};
})();

// ---- sim/jogger_chain.mjs
export const jogger_chain = (() => {
const bridge=()=>globalThis.__arondightRealWorld||null;const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));
function cam(){const b=bridge();return b?.presentedCamera?.()||b?.threeCamera||null;}
function distGain(x,y,ref=26){const c=cam();if(!c)return 0;const d=Math.hypot(x-c.position.x,y-c.position.y);return clamp(1-d/ref/3,0,1);}
function stomp(x,y,accent){const a=globalThis.__sharedAudioContext;if(!a||a.state!=="running")return;const g0=distGain(x,y)*(accent?.55:.32);if(g0<.02)return;try{const t=a.currentTime,o=a.createOscillator(),g=a.createGain();o.type="sine";o.frequency.setValueAtTime(accent?92:78,t);o.frequency.exponentialRampToValueAtTime(42,t+.12);g.gain.setValueAtTime(g0,t);g.gain.exponentialRampToValueAtTime(.001,t+.16);o.connect(g).connect(a.destination);o.start(t);o.stop(t+.18);
  if(accent){const n=a.createBuffer(1,a.sampleRate*.06,a.sampleRate),ch=n.getChannelData(0);for(let i=0;i<ch.length;i++)ch[i]=(Math.random()*2-1)*(1-i/ch.length)**2;const s=a.createBufferSource(),bp=a.createBiquadFilter(),cg=a.createGain();bp.type="bandpass";bp.frequency.value=1800;bp.Q.value=.9;cg.gain.value=g0*.9;s.buffer=n;s.connect(bp).connect(cg).connect(a.destination);s.start(t+.01);}}catch{}}
function jingle(){const a=globalThis.__sharedAudioContext;if(!a||a.state!=="running")return;try{const t=a.currentTime;[523,659,784,1047,784,1047].forEach((f,i)=>{const o=a.createOscillator(),g=a.createGain();o.type=i%2?"square":"triangle";o.frequency.value=f;const s=t+i*.11;g.gain.setValueAtTime(0,s);g.gain.linearRampToValueAtTime(.12,s+.01);g.gain.exponentialRampToValueAtTime(.001,s+.22);o.connect(g).connect(a.destination);o.start(s);o.stop(s+.25);});}catch{}}
return {stomp, jingle};
})();

// ---- sim/street_voices.mjs
export const street_voices = (() => {
const worldCameraPose=()=>K.pose();const HONK_GAP_MS=1200;let settings=loadAudioSettings(),ctx=null,bus=null,lastHonk=-1e9;const lastBySource=new Map();const camera=()=>null;addEventListener(AUDIO_SETTINGS_EVENT,()=>{settings=loadAudioSettings();});
function hash(s){let h=2166136261;for(const ch of String(s))h=Math.imul(h^ch.charCodeAt(0),16777619);return(h>>>0)/4294967296;}
function audio(){if(ctx)return ctx;const C=globalThis.AudioContext||globalThis.webkitAudioContext;if(!C)return null;try{ctx=(globalThis.__sharedAudioContext??=new C({latencyHint:"interactive"}));}catch{return null;}bus=ctx.createGain();bus.gain.value=1;bus.connect(ctx.destination);return ctx;}
function placeListener(){const pose=worldCameraPose(camera()),a=ctx;if(!pose||!a)return;const L=a.listener,t=a.currentTime,[x,y,z]=pose.position,[dx,dy,dz]=pose.direction;
  if(L.positionX){L.positionX.setValueAtTime(x,t);L.positionY.setValueAtTime(y,t);L.positionZ.setValueAtTime(z,t);L.forwardX.setValueAtTime(dx,t);L.forwardY.setValueAtTime(dy,t);L.forwardZ.setValueAtTime(dz,t);L.upX.setValueAtTime(0,t);L.upY.setValueAtTime(0,t);L.upZ.setValueAtTime(1,t);}
  else{L.setPosition?.(x,y,z);L.setOrientation?.(dx,dy,dz,0,0,1);}}
function hearing(){return settings.soundEnabled?Math.max(0,Math.min(1,(Number(settings.fxVolume)||0)/100)):0;}
function distanceTo(p){const pose=worldCameraPose(camera());return pose?Math.hypot(p[0]-pose.position[0],p[1]-pose.position[1],(p[2]||0)-pose.position[2]):Infinity;}
function honk(position,{kind="car",style="toot",source=""}={}){
  const now=performance.now();if(!Array.isArray(position)||now-lastHonk<HONK_GAP_MS)return false;const key=`h:${source}`;if(source&&now-(lastBySource.get(key)||-1e9)<4000)return false;
  const vol=hearing(),d=distanceTo(position);if(vol<=0||d>160)return false;const a=audio();if(!a||a.state!=="running")return false;lastHonk=now;if(source)lastBySource.set(key,now);placeListener();
  const t0=a.currentTime+.01,bus2=kind==="bus",f1=bus2?233:415,f2=bus2?294:523,pan=a.createPanner();pan.panningModel="equalpower";pan.distanceModel="inverse";pan.refDistance=bus2?9:6;pan.maxDistance=400;pan.rolloffFactor=1.1;
  if(pan.positionX){pan.positionX.setValueAtTime(position[0],t0);pan.positionY.setValueAtTime(position[1],t0);pan.positionZ.setValueAtTime(position[2]||1,t0);}else pan.setPosition(position[0],position[1],position[2]||1);
  const band=a.createBiquadFilter();band.type="bandpass";band.frequency.value=bus2?900:1500;band.Q.value=.7;const shape=a.createWaveShaper(),curve=new Float32Array(256);for(let i=0;i<256;i++){const x=i/127.5-1;curve[i]=Math.tanh(2.2*x);}shape.curve=curve;
  const g=a.createGain();g.gain.value=0;shape.connect(band).connect(g).connect(pan).connect(bus);
  const parts=style==="long"?[[0,.75]]:style==="double"?[[0,.16],[.24,.2]]:[[0,.24]];const end=parts[parts.length-1][0]+parts[parts.length-1][1];
  for(const[f,detune]of[[f1,-6],[f2,5]]){const o=a.createOscillator();o.type="sawtooth";o.frequency.value=f;o.detune.value=detune;const og=a.createGain();og.gain.value=.5;o.connect(og).connect(shape);o.start(t0);o.stop(t0+end+.1);}
  const peak=.34*vol;for(const[s,len]of parts){g.gain.setValueAtTime(0,t0+s);g.gain.linearRampToValueAtTime(peak,t0+s+.012);g.gain.setValueAtTime(peak,t0+s+len-.03);g.gain.linearRampToValueAtTime(0,t0+s+len);}
  const v=document.getElementById("viewport");if(v){v.dataset.streetHonks=String((Number(v.dataset.streetHonks)||0)+1);v.dataset.streetVoices=STREET_VOICES_VERSION;}return true;}
let lastScream=-1e9;
function scream(position,{seed=""}={}){
  const now=performance.now();if(!Array.isArray(position)||now-lastScream<450)return false;const key=`x:${seed}`;if(seed&&now-(lastBySource.get(key)||-1e9)<3000)return false;
  const d=distanceTo(position),vol=hearing();if(vol<=0||d>55)return false;const a=audio();if(!a||a.state!=="running")return false;lastScream=now;if(seed)lastBySource.set(key,now);placeListener();
  const k=hash(seed||String(now)),female=k>.45,f0=(female?520:330)+k*160,dur=.55+hash(seed+"d")*.55,t0=a.currentTime+.01;
  const pan=a.createPanner();pan.panningModel="equalpower";pan.distanceModel="inverse";pan.refDistance=4;pan.maxDistance=200;pan.rolloffFactor=1.2;
  if(pan.positionX){pan.positionX.setValueAtTime(position[0],t0);pan.positionY.setValueAtTime(position[1],t0);pan.positionZ.setValueAtTime(position[2]||1.6,t0);}else pan.setPosition(position[0],position[1],position[2]||1.6);
  const out=a.createGain();out.gain.value=0;out.connect(pan).connect(bus);
  const peak=.42*vol;out.gain.setValueAtTime(0,t0);out.gain.linearRampToValueAtTime(peak,t0+.04);out.gain.setValueAtTime(peak*.9,t0+dur*.75);out.gain.exponentialRampToValueAtTime(.0008,t0+dur);
  const f1=a.createBiquadFilter(),f2=a.createBiquadFilter();f1.type="bandpass";f1.frequency.value=female?1050:850;f1.Q.value=5;f2.type="bandpass";f2.frequency.value=female?2600:2300;f2.Q.value=6;const g1=a.createGain(),g2=a.createGain();g1.gain.value=1.1;g2.gain.value=.6;f1.connect(g1).connect(out);f2.connect(g2).connect(out);
  const vib=a.createOscillator(),vg=a.createGain();vib.frequency.value=6+k*3;vg.gain.value=f0*.035;vib.connect(vg);
  for(const[type,mul,gain]of[["sawtooth",1,.55],["square",1.003,.18]]){const o=a.createOscillator();o.type=type;o.frequency.setValueAtTime(f0*.82*mul,t0);o.frequency.exponentialRampToValueAtTime(f0*1.18*mul,t0+.12);o.frequency.exponentialRampToValueAtTime(f0*.9*mul,t0+dur);vg.connect(o.frequency);const og=a.createGain();og.gain.value=gain;o.connect(og);og.connect(f1);og.connect(f2);o.start(t0);o.stop(t0+dur+.05);}
  const len=Math.floor(a.sampleRate*dur),buf=a.createBuffer(1,len,a.sampleRate),ch=buf.getChannelData(0);for(let i=0;i<len;i++)ch[i]=(Math.random()*2-1)*.35;const nz=a.createBufferSource();nz.buffer=buf;const ng=a.createGain();ng.gain.value=.25;nz.connect(ng).connect(f2);nz.start(t0);
  vib.start(t0);vib.stop(t0+dur+.05);
  const v=document.getElementById("viewport");if(v)v.dataset.streetScreams=String((Number(v.dataset.streetScreams)||0)+1);return true;}
return {honk, scream};
})();

// ---- sim/vs_combat_presentation.mjs
export const vs_combat_presentation = (() => {
const viewport=()=>document.getElementById("viewport");let audioContext=null,explosionNoiseBuffer=null,soundEventCount=0;
function prepareExplosionNoise(ctx){if(explosionNoiseBuffer||!ctx)return explosionNoiseBuffer;const duration=.82,samples=Math.max(1,Math.floor(ctx.sampleRate*duration)),buffer=ctx.createBuffer(1,samples,ctx.sampleRate),data=buffer.getChannelData(0);let seed=0x45a70d1;for(let i=0;i<samples;i++){seed=(Math.imul(seed,1664525)+1013904223)>>>0;const noise=(seed/0xffffffff)*2-1,t=i/samples;data[i]=noise*Math.pow(1-t,2.35);}explosionNoiseBuffer=buffer;const view=viewport();if(view)view.dataset.vsExplosionAudioReady="1";return buffer;}
function ensureAudio(){const AudioCtor=globalThis.AudioContext||globalThis.webkitAudioContext;if(!AudioCtor)return null;try{audioContext??=new AudioCtor({latencyHint:"interactive"});}catch{return null;}const ready=()=>{prepareExplosionNoise(audioContext);const view=viewport();if(view)view.dataset.vsExplosionAudioState=audioContext.state;};if(audioContext.state==="suspended")audioContext.resume().then(ready).catch(()=>{});else ready();return audioContext;}
function playExplosion(local=false){const view=viewport();soundEventCount++;if(view)view.dataset.vsExplosionSoundCount=String(soundEventCount);const ctx=ensureAudio();try{navigator.vibrate?.(local?[70,35,120]:[65]);}catch{}if(!ctx||ctx.state!=="running"||!explosionNoiseBuffer){if(view)view.dataset.vsExplosionAudioState=ctx?.state||"unavailable";return false;}try{const now=ctx.currentTime,duration=.82,noise=ctx.createBufferSource(),filter=ctx.createBiquadFilter(),noiseGain=ctx.createGain();noise.buffer=explosionNoiseBuffer;filter.type="lowpass";filter.frequency.setValueAtTime(local?1150:900,now);filter.frequency.exponentialRampToValueAtTime(120,now+duration);noiseGain.gain.setValueAtTime(.0001,now);noiseGain.gain.exponentialRampToValueAtTime(local?.52:.38,now+.012);noiseGain.gain.exponentialRampToValueAtTime(.0001,now+duration);noise.connect(filter).connect(noiseGain).connect(ctx.destination);const boom=ctx.createOscillator(),boomGain=ctx.createGain();boom.type="sine";boom.frequency.setValueAtTime(local?105:92,now);boom.frequency.exponentialRampToValueAtTime(34,now+.55);boomGain.gain.setValueAtTime(.0001,now);boomGain.gain.exponentialRampToValueAtTime(local?.58:.43,now+.008);boomGain.gain.exponentialRampToValueAtTime(.0001,now+.72);boom.connect(boomGain).connect(ctx.destination);const crack=ctx.createOscillator(),crackGain=ctx.createGain();crack.type="triangle";crack.frequency.setValueAtTime(210,now);crack.frequency.exponentialRampToValueAtTime(58,now+.18);crackGain.gain.setValueAtTime(.0001,now);crackGain.gain.exponentialRampToValueAtTime(.17,now+.004);crackGain.gain.exponentialRampToValueAtTime(.0001,now+.22);crack.connect(crackGain).connect(ctx.destination);noise.start(now);noise.stop(now+duration);boom.start(now);boom.stop(now+.74);crack.start(now);crack.stop(now+.24);audioStartedCount++;if(view){view.dataset.vsExplosionAudioStartedCount=String(audioStartedCount);view.dataset.vsExplosionAudioState=ctx.state;}return true;}catch{return false;}}
return {playExplosion};
})();

// ---- sim/fighter_jets.mjs
export const fighter_jets = (() => {
const SPEED=255,SOUND=343;const bridge=()=>globalThis.__arondightRealWorld||null,walk=()=>globalThis.__arondightWalkMode||null;let ctx=null,master=null,jetBuffer=null;const listener=new K.V3(),tmp=new K.V3(),tmp2=new K.V3();const beepCtx=()=>{try{return getSharedCombatAudioContext({resume:true});}catch{return null;}};
function ensureAudio(){
  try{ctx=getSharedCombatAudioContext();if(!ctx)return null;if(!master){master=ctx.createGain();master.gain.value=1;const comp=ctx.createDynamicsCompressor();comp.threshold.value=-10;comp.ratio.value=4;master.connect(comp);comp.connect(ctx.destination);}
    if(!jetBuffer){const sr=ctx.sampleRate,n=Math.floor(sr*4),b=ctx.createBuffer(1,n,sr),d=b.getChannelData(0);let lp=0,lp2=0,br=0;
      for(let i=0;i<n;i++){const t=i/sr,w=Math.random()*2-1;lp+=(w-lp)*.08;lp2+=(lp-lp2)*.02;br+=(w-br)*.6;// rumble + roar + hiss
        const whine=Math.sin(2*Math.PI*1180*t+Math.sin(2*Math.PI*3*t)*2)*.05+Math.sin(2*Math.PI*2360*t)*.025;d[i]=lp2*2.4+lp*.9+(w-br)*.18+whine;}
      // loop-safe crossfade
      const f=Math.floor(sr*.05);for(let i=0;i<f;i++){const a=i/f;d[n-f+i]=d[n-f+i]*(1-a)+d[i]*a;}
      let peak=0;for(let i=0;i<n;i++)peak=Math.max(peak,Math.abs(d[i]));for(let i=0;i<n;i++)d[i]/=peak||1;jetBuffer=b;}
    return ctx;}catch{return null;}
}
function jetEngineAudio(){const c=ensureAudio();return c&&jetBuffer?{ctx:c,buffer:jetBuffer,master}:null;}
function startJetVoice(jet){const c=ensureAudio();if(!c||c.state!=="running"||jet.voice)return;const src=c.createBufferSource();src.buffer=jetBuffer;src.loop=true;src.loopStart=0;src.loopEnd=jetBuffer.duration;const lp=c.createBiquadFilter();lp.type="lowpass";lp.frequency.value=1500;const g=c.createGain();g.gain.value=0;src.connect(lp);lp.connect(g);g.connect(master);src.start(0,Math.random()*3);jet.voice={src,lp,g};}
function stopJetVoice(jet){const v=jet.voice;if(!v)return;try{v.g.gain.setTargetAtTime(0,ctx.currentTime,.3);v.src.stop(ctx.currentTime+1.2);}catch{}jet.voice=null;}
function listenerPos(){const w=walk();if(w?.mode==="foot"&&w.position)return listener.copy(w.position);const c=bridge()?.threeCamera;return c?listener.copy(c.position):null;}
function jetPosAt(jet,t,out){const k=(t-jet.t0)/1000;return out.set(jet.x,jet.y0+SPEED*k,jet.z);}
function updateJetAudio(jet,now){
  const L=listenerPos();if(!L||!jet.voice)return;
  // retarded time: the sound we hear now left the jet when |p(te)-L| = c (now-te)
  let te=now;for(let i=0;i<4;i++){jetPosAt(jet,te,tmp);te=now-tmp.distanceTo(L)/SOUND*1000;}
  jetPosAt(jet,te,tmp);const d=Math.max(8,tmp.distanceTo(L)),dir=tmp2.copy(L).sub(tmp).normalize(),vr=SPEED*dir.y,doppler=Math.max(.45,Math.min(2.6,SOUND/(SOUND-vr)));
  const started=te>=jet.t0-1500,gain=started?Math.min(1.8,(85/d)**1.35)*.9*.85:0,t=ctx.currentTime;
  jet.voice.g.gain.setTargetAtTime(gain,t,.05);jet.voice.src.playbackRate.setTargetAtTime(doppler*(.92+jet.pitch*.16),t,.05);jet.voice.lp.frequency.setTargetAtTime(Math.min(9000,900+60000/d),t,.06);
  jet.heardD=d;
}
function beep(freqs,dur=.09,gain=.12){const c=beepCtx();if(!c||c.state!=="running")return;let t=c.currentTime;for(const f of freqs){const o=c.createOscillator(),g=c.createGain();o.type="square";o.frequency.value=f;g.gain.setValueAtTime(0,t);g.gain.linearRampToValueAtTime(gain,t+.008);g.gain.setValueAtTime(gain,t+dur-.02);g.gain.linearRampToValueAtTime(0,t+dur);o.connect(g);g.connect(c.destination);o.start(t);o.stop(t+dur+.02);t+=dur+.035;}}
function blastSound(p){try{const c=ensureAudio(),L=listenerPos(),d=L?L.distanceTo(p):100;if(c)setTimeout(()=>playCombatAudio(c,"explosion",{destination:master,gain:Math.min(1.6,40/(10+d)),playbackRate:.7+Math.random()*.15}),d/SOUND*1000);}catch{}}
return {jetEngineAudio, startJetVoice, stopJetVoice, updateJetAudio, beep, blastSound};
})();

// ---- sim/jet_mode.mjs
export const jet_mode = (() => {
const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));let voice=null,jet=null,camMode="cockpit",JET_SPEC={thrustDry:1};const jetEngineAudio=(...a)=>fighter_jets.jetEngineAudio(...a);
function sfx(name,opts){try{const c=getSharedCombatAudioContext();if(c?.state==="running")playCombatAudio(c,name,opts);}catch{}}
function startVoice(){const a=jetEngineAudio();if(!a||voice)return;const c=a.ctx,src=c.createBufferSource();src.buffer=a.buffer;src.loop=true;const lp=c.createBiquadFilter();lp.type="lowpass";lp.frequency.value=1200;const g=c.createGain();g.gain.value=0;src.connect(lp);lp.connect(g);g.connect(a.master||c.destination);src.start();voice={src,lp,g,c};}
function stopVoice(){if(!voice)return;try{voice.g.gain.setTargetAtTime(0,voice.c.currentTime,.2);voice.src.stop(voice.c.currentTime+.8);}catch{}voice=null;}
function updateVoice(){if(!voice)startVoice();if(!voice||!jet)return;const af=jet.af,t=voice.c.currentTime,ab=af.mode==="flight"&&af.lever>.86&&Math.cos(af.nozzle)>.8,pw=clamp(af.thrust/JET_SPEC.thrustDry,0,1.3),cockpit=camMode==="cockpit";voice.g.gain.setTargetAtTime((.12+pw*.36+(ab?.22:0))*(cockpit?.55:1),t,.08);voice.src.playbackRate.setTargetAtTime(.45+pw*.6+(ab?.12:0),t,.08);voice.lp.frequency.setTargetAtTime((700+pw*2600+(ab?2500:0))*(cockpit?.6:1),t,.08);}
function setJet(j,cam,spec){jet=j;camMode=cam;JET_SPEC=spec;}
return {sfx, startVoice, stopVoice, updateVoice, setJet};
})();
