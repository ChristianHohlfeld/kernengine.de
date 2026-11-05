// Unchanged from the original SpawnCity (spawncity.de, sim/headshot_feedback.mjs, commit 9108537):
// the headshot click, run on Kern by web/audio.mjs.
// Headshot detection remains unchanged; feedback is a neutral short click only.
// No headshot title, announcer, dedicated MP3 request or speech.
import {loadAudioSettings,AUDIO_SETTINGS_EVENT,normalizeAudioSettings} from "./audio_settings.mjs";
import {getSharedCombatAudioContext,playCombatAudio} from "./combat_audio_bank.mjs";

export const HEADSHOT_FEEDBACK_VERSION="headshot-neutral-crack-no-label-v2";
const HEAD_PROXIES=new Set(["PERSON_HIT_PROXY","ZOMBIE_HIT_PROXY","POLICE_OFFICER_HIT_PROXY","MILITARY_SOLDIER_HIT_PROXY"]);
let installed=false,audioSettings=null,count=0,streak=0,lastHeadAt=-Infinity;
const viewport=()=>document.getElementById("viewport");
const clamp=(v,a,b)=>Math.max(a,Math.min(b,Number(v)||0));

// ---------------------------------------------------------------- zones
function zoneOf(hit){for(let n=hit?.object;n;n=n.parent){const z=n.userData?.vsHitZone;if(z)return String(z);}return"";}
function proxyOf(hit){for(let n=hit?.object,i=0;n&&i<4;n=n.parent,i++)if(HEAD_PROXIES.has(n.name))return n;return null;}
// is this object drawn in the live scene (not detached, not hidden by itself or a parent)?
export function liveInScene(object){for(let n=object;n;n=n.parent){if(n.visible===false)return false;if(n.isScene)return true;}return false;}
const _min={x:0,y:0,z:0},_max={x:0,y:0,z:0};
function worldZRange(mesh){const g=mesh?.geometry;if(!g)return null;if(!g.boundingBox)g.computeBoundingBox?.();const b=g.boundingBox;if(!b)return null;mesh.updateWorldMatrix?.(true,false);const e=mesh.matrixWorld.elements;let lo=Infinity,hi=-Infinity;
  for(let i=0;i<8;i++){const x=i&1?b.max.x:b.min.x,y=i&2?b.max.y:b.min.y,z=i&4?b.max.z:b.min.z,wz=e[2]*x+e[6]*y+e[10]*z+e[14];if(wz<lo)lo=wz;if(wz>hi)hi=wz;}return{lo,hi};}
export function headZone(hit){if(!hit)return"";const zone=zoneOf(hit);if(zone)return zone==="head"?"head":"";
  const proxy=proxyOf(hit),point=hit.point;if(!proxy||!point||!Number.isFinite(Number(point.z)))return"";const r=worldZRange(proxy);if(!r)return"";const h=r.hi-r.lo;
  if(h<1.2||h>2.6)return""; /* lying, crouched into a heap, or not a standing human */
  const head=clamp(h*.14,.21,.3);return Number(point.z)>=r.hi-head?"head":"";}
// ---------------------------------------------------------------- vehicles
// the car / bus a hit belongs to: the top-most vehicle node above the struck mesh (or the Box3D body id)
export function vehicleHit(hit){let root=null,kind="",police=false;for(let n=hit?.object;n&&!n.isScene;n=n.parent){const u=n.userData||{},k=String(u.worldPopulationKind||u.worldLifeKind||"");if(u.policeCruiser){root=n;kind="car";police=true;}else if(k==="car"||k==="bus"){root=n;kind=k;}else if(u.gtaDrivableVehicle&&!kind){root=n;kind="car";}}
  if(!kind){const pk=String(hit?.physicsKind||"");if(pk==="car"||pk==="bus")kind=pk;else return null;}
  const key=String(hit?.physicsId||root?.userData?.worldPopulationId||root?.userData?.policeCruiser||root?.uuid||"");let engine=false;
  if(root&&hit?.point){/* the engine sits in the nose (local +X, where the critical smoke comes out): a hit there does more */root.updateWorldMatrix?.(true,false);const inv=root.matrixWorld.clone().invert(),p=hit.point.clone().applyMatrix4(inv),s=root.scale?.x||1,nose=kind==="bus"?2.1:.85;engine=p.x*s>=nose&&p.z*s<1.35;}
  return{root,kind,police,key,engine};}

// ---------------------------------------------------------------- neutral feedback
// Preserve head detection, kill/damage and event/API contracts. A short dry
// percussive click is reused from the existing combat PCM bank (no dedicated
// headshot sample, no voice, no overlay, no fetch/decoding or visual allocation).
function settings(){return audioSettings||(audioSettings=loadAudioSettings());}
function click(){const a=settings();if(!a.soundEnabled||a.fxVolume<=0)return false;
  const ctx=getSharedCombatAudioContext({resume:true});
  return Boolean(ctx&&playCombatAudio(ctx,"crack",{gain:.32*clamp(a.fxVolume,0,100)/100,minIntervalMs:65}));}
export function announceHeadshot({weapon="",target="",remote=false}={}){
  const now=performance.now();count++;streak=now-lastHeadAt<4000?streak+1:1;lastHeadAt=now;
  const sound={crack:click(),voice:false},v=viewport();
  if(v){v.dataset.headshots=String(count);v.dataset.headshotLast=`${weapon}:${target}${remote?":remote":""}`;v.dataset.headshotVoice="0";v.dataset.headshotFeedback=HEADSHOT_FEEDBACK_VERSION;}
  try{window.dispatchEvent(new CustomEvent("arondight:headshot",{detail:{weapon,target,remote,streak,voice:false}}));}catch{}
  return{shown:false,voice:false,sound};}
export function installHeadshotFeedback(){if(installed||typeof window==="undefined")return;installed=true;
  addEventListener(AUDIO_SETTINGS_EVENT,e=>{audioSettings=normalizeAudioSettings(e.detail||loadAudioSettings());});
  globalThis.__arondightHeadshot={zone:headZone,vehicle:vehicleHit,announce:announceHeadshot,get count(){return count;},get voiceReady(){return false;},version:HEADSHOT_FEEDBACK_VERSION};}
installHeadshotFeedback();
