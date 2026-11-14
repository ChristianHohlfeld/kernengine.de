// Unchanged from the original SpawnCity (spawncity.de, drone_simulator.html inline script, commit
// 9108537): the title music starts right away (or on the first tap) and the shared AudioContext with it.
// Menu music starts right away: try autoplay on load, and if the browser
// blocks it (iOS/Android do until the first touch), start on the very first
// tap/key anywhere. The game's music module adopts this same element later,
// so the track never restarts or doubles when START is pressed.
(function(){
  var cfg={};try{cfg=JSON.parse(localStorage.getItem("arondight45AudioSettingsV1")||"{}");}catch(e){}
  if(cfg.musicEnabled===false)return;
  var vol=Math.max(0,Math.min(1,(cfg.musicVolume==null?32:cfg.musicVolume)/100))*0.45;
  // Kern: one track only - a second call (or the game's player that came first) reuses the element
  var a=window.__oppMusicElement;
  if(!a){a=new Audio("./sim/audio/8_bits_only.m4a");a.loop=true;a.preload="auto";a.volume=vol;a.setAttribute("playsinline","");window.__oppMusicElement=a;}
  if(window.__scMenuMusicArmed)return;window.__scMenuMusicArmed=true;
  // One audio session for the whole game: the Web Audio context for all
  // effects is created on the same first tap that starts the music, so iOS
  // never switches/interrupts the session later when the game is running.
  function ctx(){try{if(!window.__sharedAudioContext){var AC=window.AudioContext||window.webkitAudioContext;if(AC)window.__sharedAudioContext=new AC({latencyHint:"interactive"});}var x=window.__sharedAudioContext;if(x&&x.state==="suspended")x.resume().catch(function(){});}catch(e){}}
  function go(e){if(e&&e.isTrusted===false)return;if(e)ctx();if(!a.paused){off();return;}a.play().then(function(){document.documentElement.dataset.menuMusic="playing";off();}).catch(function(){document.documentElement.dataset.menuMusic="waiting-for-tap";});}
  function off(){["pointerdown","touchend","keydown","click"].forEach(function(t){window.removeEventListener(t,go,true);});}
  ["pointerdown","touchend","keydown","click"].forEach(function(t){window.addEventListener(t,go,true);});
  go();
})();
