// Unchanged from the original SpawnCity (spawncity.de, drone_simulator.html inline script, commit
// 9108537): the menu's click blip (window.__oppUiBlip).
// Menu UI click: a short soft pulse blip in the soundtrack's key (B♭→E♭),
// for START, HOME SCREEN and turning sound on. Respects the sound switch.
(function(){
  var ctx=null;
  function soundOn(){try{return JSON.parse(localStorage.getItem("arondight45AudioSettingsV1")||"{}").soundEnabled!==false;}catch(e){return true;}}
  function blip(strong){
    if(!soundOn())return;var AC=window.AudioContext||window.webkitAudioContext;if(!AC)return;
    try{ctx=ctx||new AC();if(ctx.state==="suspended")ctx.resume();var t=ctx.currentTime,out=ctx.createGain(),lp=ctx.createBiquadFilter();lp.type="lowpass";lp.frequency.setValueAtTime(3200,t);lp.frequency.exponentialRampToValueAtTime(900,t+.25);out.gain.value=strong?.22:.14;lp.connect(out).connect(ctx.destination);
      [[932.33,0],[1244.51,.07]].slice(0,strong?2:1).forEach(function(n){var o=ctx.createOscillator(),g=ctx.createGain();o.type="square";o.frequency.value=n[0];g.gain.setValueAtTime(0,t+n[1]);g.gain.linearRampToValueAtTime(1,t+n[1]+.005);g.gain.exponentialRampToValueAtTime(.001,t+n[1]+.16);o.connect(g).connect(lp);o.start(t+n[1]);o.stop(t+n[1]+.2);});}catch(e){}
  }
  window.__oppUiBlip=blip;
  document.addEventListener("click",function(e){var el=e.target&&e.target.closest&&e.target.closest("#gameMenuStart,#installApp,#soundSwitch,#musicSwitch,#gameExitButton");if(!el)return;
    if(el.id==="soundSwitch"){setTimeout(function(){blip(false);},0);return;}blip(el.id==="gameMenuStart");},true);
})();
