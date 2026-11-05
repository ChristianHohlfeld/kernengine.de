// From the original SpawnCity (spawncity.de, drone_simulator.html inline script, commit 9108537): the title's
// HOME SCREEN install button. Only change: the service worker is Kern's (ui.mjs registers sw.js).
// Install as a home-screen app (top left of the menu).
//  Android/Chrome: native install prompt (beforeinstallprompt).
//  iOS Safari: no API exists — show the two taps (Share → Add to Home Screen).
// Hidden when already running as an installed app.
(function(){
  var btn=document.getElementById("installApp"),hint=document.getElementById("installHint"),menu=document.getElementById("gameMenu"),deferred=null;
  var standalone=window.matchMedia("(display-mode: standalone)").matches||window.matchMedia("(display-mode: fullscreen)").matches||navigator.standalone===true;
  var ios=/iphone|ipad|ipod/i.test(navigator.userAgent)||(/macintosh/i.test(navigator.userAgent)&&navigator.maxTouchPoints>1);
  // (the service worker: Kern's own, registered by ui.mjs)
  function sync(){btn.hidden=standalone||!menu||menu.hidden||!(deferred||ios);if(btn.hidden)hint.hidden=true;setTimeout(sync,500);}
  window.addEventListener("beforeinstallprompt",function(e){e.preventDefault();deferred=e;});
  window.addEventListener("appinstalled",function(){standalone=true;deferred=null;});
  btn.addEventListener("pointerdown",function(e){e.stopPropagation();},true);
  btn.addEventListener("click",function(e){e.preventDefault();e.stopPropagation();
    if(deferred){deferred.prompt();deferred.userChoice.finally(function(){deferred=null;});return;}
    if(ios){hint.innerHTML='Tippe unten in Safari auf <b>Teilen</b> &#x2191; und dann auf <b>Zum Home-Bildschirm</b>. SpawnCity startet danach im Vollbild wie eine App.';hint.hidden=!hint.hidden;}});
  document.addEventListener("click",function(e){if(!hint.hidden&&e.target!==btn&&!btn.contains(e.target))hint.hidden=true;},true);
  sync();
})();
