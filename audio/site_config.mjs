// Unchanged from the original SpawnCity (spawncity.de, sim/site_config.mjs, commit 9108537):
// the site origin (music_player.mjs's fallback base URL), run on Kern by web/audio.mjs.
// SpawnCity public-site settings. One place for the product name and the live host.
//
// This repository is the GitHub Pages site for spawncity.de. pagesHost must match
// the single line in ./CNAME.
//
// multiplayerAppId is the Trystero room key. Simulator and controller are both
// bundled from this file, so they switch rooms in the same deploy. Changing it
// splits players from anyone who still has an older build open until they reload.

export const SITE=Object.freeze({
  productName:"SpawnCity",
  productHost:"spawncity.de",
  pagesHost:"spawncity.de",
  gamePath:"/drone_simulator.html",
  controllerPath:"/drone_controller.html",
  multiplayerAppId:"arondight45-spawncity-vs-v1"
});

export function pagesOrigin(){return `https://${SITE.pagesHost}`;}
export function productOrigin(){return `https://${SITE.productHost}`;}
export function gameUrl(){return `${pagesOrigin()}${SITE.gamePath}`;}
export function controllerUrl(){return `${pagesOrigin()}${SITE.controllerPath}`;}
export function canonicalGameUrl(){return gameUrl();}

// Site root opens the game (apex, www, and the project Pages path). Other paths stay.
export function productFrontDoor(_hostname,pathname){
  const path=String(pathname||"/");
  if(path==="/"||path==="/index.html"||/\/spawncity-de\/?$/.test(path)||/\/spawncity-de\/index\.html$/.test(path)){
    const base=/\/spawncity-de(\/|$)/.test(path)?"/spawncity-de":"";
    return `${base}${SITE.gamePath}`;
  }
  return "";
}
