// spawncity.de drone_simulator.html (commit 9108537): the update check, unchanged but for two
// lines - the tag reuses the page's #buildTag, and the page names the build Kern's.
// Strict versioning for the installed app (home screen). The deployed page carries its build
// SHA (first comment of the document); every deploy also publishes ./build.json with the same
// SHA. We ask for build.json with no-store + a unique query (no CDN, no HTTP cache, no service
// worker cache can answer) on start, on every return to the app (visibility, focus, pageshow,
// back online) and every 30 s. A newer build:
//  * before the first START (title screen, app launch / resume) → reload right away,
//  * once a game has been started in this page, never on its own: a session in progress (also in
//    the pause menu, after an app switch, in multiplayer) is never thrown out — an UPDATE pill
//    (tap / U = update now) waits until the player chooses.
// Before reloading, the service worker is told to update, every Cache Storage entry is dropped,
// and the reload URL carries the new SHA so nothing on the way can serve the old page.
(function(){
  var first=document.firstChild,m=first&&first.nodeType===8&&/BUILD_SHA:([0-9a-f]{7,40})/.exec(first.nodeValue||""),mine=m&&m[1];
  // bottom right: this build vs the online build, green when equal
  var tag=document.getElementById("buildTag")||document.createElement("div");tag.id="buildTag";tag.style.cssText="position:fixed;z-index:2147482000;right:max(6px,env(safe-area-inset-right));bottom:max(4px,env(safe-area-inset-bottom));padding:2px 6px;border-radius:4px;background:#000a;color:#9fb3c8;font:600 10px/1.3 ui-monospace,Menlo,monospace;pointer-events:none;letter-spacing:.02em";
  function showTag(online){if(!tag.isConnected&&document.body)document.body.appendChild(tag);var a=mine?mine.slice(0,7):"dev",b=online?online.slice(0,7):"…";tag.textContent="v "+a+" · online "+b;tag.style.color=online&&mine&&online===mine?"#7ff0c5":online?"#ffcf6a":"#9fb3c8";}
  if(document.body)showTag(null);else document.addEventListener("DOMContentLoaded",function(){showTag(null);});
  if(!mine){fetch("./build.json?t="+Date.now(),{cache:"no-store"}).then(function(r){return r.ok?r.json():null;}).then(function(j){showTag(j&&j.sha||null);}).catch(function(){});return;}
  var pending=null,reloading=false,lastCheck=0,pill=null;
  function menu(){return document.getElementById("gameMenu");}
  function inMenu(){var el=menu();return !el||!el.hidden;}
  var played=false;function playing(){if(!played&&document.body&&document.body.classList.contains("solo-flight"))played=true;return played;}setInterval(playing,1000);
  function purge(){var jobs=[];/* the offline copy and map data stay (the worker serves the page network-first anyway); legacy caches go */try{if(window.caches&&caches.keys)jobs.push(caches.keys().then(function(k){return Promise.all(k.filter(function(n){return n.indexOf("spawncity-")!==0&&n!=="rush-tiles-v1"/* the worker moves the old map cache over */;}).map(function(n){return caches.delete(n);}));}));}catch(e){}
    try{if(navigator.serviceWorker&&navigator.serviceWorker.getRegistrations)jobs.push(navigator.serviceWorker.getRegistrations().then(function(rs){return Promise.all(rs.map(function(r){return r.update().catch(function(){});}));}));}catch(e){}
    return Promise.race([Promise.all(jobs),new Promise(function(r){setTimeout(r,1500);})]);}
  function fresh(sha){if(reloading)return;
    // loop guard: if the CDN still hands out the old page after two reloads for this SHA, wait for a tap
    var key="spawnCityUpdateTries:"+sha,tries=0;try{tries=+sessionStorage.getItem(key)||0;sessionStorage.setItem(key,String(tries+1));}catch(e){}
    if(tries>=2){showPill();return;}
    reloading=true;purge().then(function(){var u=new URL(location.href);u.searchParams.set("v",sha.slice(0,12));location.replace(u.toString());});}
  // in-game live notice: a new build is online → banner with a RELOAD button (tap / click / U key);
  // the game keeps running until you choose, "LATER" hides it until the next check finds it again
  function showPill(){if(pill&&pill.isConnected){if(Date.now()-(+pill.dataset.later||0)<600000)return;/* LATER = quiet for 10 min */pill.style.display="flex";return;}if(!document.body)return;pill=document.createElement("div");pill.id="appUpdatePill";pill.setAttribute("role","status");
    pill.innerHTML='<span style="font:800 12px/1.2 system-ui,sans-serif;letter-spacing:.08em">⬆ UPDATE AVAILABLE</span><button type="button" data-u="go" style="padding:7px 12px;border-radius:14px;border:0;background:linear-gradient(180deg,#7ff0c5,#3fbf8a);color:#06261a;font:900 12px/1 system-ui,sans-serif;letter-spacing:.1em;cursor:pointer">RELOAD · U</button><button type="button" data-u="later" style="padding:7px 10px;border-radius:14px;border:1px solid #ffffff55;background:transparent;color:#cfe;font:700 11px/1 system-ui,sans-serif;cursor:pointer">LATER</button>';
    pill.style.cssText="position:fixed;z-index:2147483000;left:50%;top:calc(max(8px,env(safe-area-inset-top)) + 56px);transform:translateX(-50%);display:flex;align-items:center;gap:10px;padding:8px 10px 8px 14px;border-radius:22px;border:2px solid #7ff0c5;background:#0d1118f0;color:#eafff7;box-shadow:0 8px 24px #000a;animation:appUpdIn .35s ease-out";
    var st=document.createElement("style");st.textContent="@keyframes appUpdIn{from{opacity:0;transform:translate(-50%,-14px)}to{opacity:1;transform:translate(-50%,0)}}";document.head.appendChild(st);
    pill.addEventListener("pointerdown",function(e){e.stopPropagation();},true);
    pill.addEventListener("click",function(e){var b=e.target.closest&&e.target.closest("[data-u]");if(!b)return;e.preventDefault();e.stopPropagation();if(b.dataset.u==="later"){pill.style.display="none";pill.dataset.later=String(Date.now());return;}if(!pending)return;try{sessionStorage.removeItem("spawnCityUpdateTries:"+pending);}catch(_){}fresh(pending);});
    window.addEventListener("keydown",function(e){if(e.code==="KeyU"&&pending&&pill&&pill.style.display!=="none"&&!e.repeat){try{sessionStorage.removeItem("spawnCityUpdateTries:"+pending);}catch(_){}fresh(pending);}});
    document.body.appendChild(pill);}
  function seen(sha,reason){if(sha)showTag(sha);if(!sha||sha===mine)return;pending=sha;if(!playing()&&(inMenu()||reason==="resume"||reason==="load"))fresh(sha);else showPill();}
  function viaPage(reason){return fetch(location.pathname+"?vcheck="+Date.now(),{cache:"no-store",credentials:"same-origin"}).then(function(r){return r.ok?r.text():"";}).then(function(t){var n=/BUILD_SHA:([0-9a-f]{7,40})/.exec(t.slice(0,300));if(n)seen(n[1],reason);});}
  function check(reason){var now=Date.now();if(reason!=="load"&&reason!=="resume"&&now-lastCheck<10000)return;lastCheck=now;
    fetch("./build.json?t="+now,{cache:"no-store",credentials:"same-origin"}).then(function(r){if(!r.ok)throw 0;return r.json();}).then(function(j){seen(String(j&&j.sha||""),reason);}).catch(function(){viaPage(reason).catch(function(){});});}
  function resume(){if(pending&&!playing())fresh(pending);else check("resume");}
  document.addEventListener("visibilitychange",function(){if(document.visibilityState==="visible")resume();});
  window.addEventListener("pageshow",function(e){if(e.persisted)resume();});
  window.addEventListener("focus",function(){check("focus");});
  window.addEventListener("online",function(){check("resume");});
  setInterval(function(){if(pending&&inMenu()&&!playing())fresh(pending);else check("interval");},30000);
  check("load");
})();
