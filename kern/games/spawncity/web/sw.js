// SpawnCity on Kern - service worker (like SpawnCity's oppenheimer-sw.js): the installed app opens
// without a network. Network first, so every start gets the newest build; what was loaded is kept
// and answers when the network is gone. Only this origin's files (the page, its modules, the wasm);
// map tiles and other services are not cached here.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
const CACHE = "spawncity-kern-v2";

self.addEventListener("install", () => self.skipWaiting());
self.addEventListener("activate", event => {
  event.waitUntil((async () => {
    for (const key of await caches.keys()) if (key !== CACHE) await caches.delete(key);
    await self.clients.claim();
  })());
});
self.addEventListener("fetch", event => {
  const req = event.request;
  if (req.method !== "GET" || new URL(req.url).origin !== self.location.origin) return;
  event.respondWith((async () => {
    try {
      // past the browser's HTTP cache too (the host keeps pages 10 min): the installed app asks the
      // server every start (an unchanged file costs a 304) and so always opens the newest build
      const res = await fetch(req.mode === "navigate" ? new Request(req.url, {cache: "no-cache", credentials: "same-origin"}) : new Request(req, {cache: "no-cache"}));
      // a whole answer only (the music file is streamed in ranges: 206 cannot be cached)
      if (res.status === 200) { const copy = res.clone(); caches.open(CACHE).then(c => c.put(req, copy)).catch(() => {}); }
      return res;
    } catch (error) {
      const hit = await caches.match(req, {ignoreSearch: true});
      if (hit) return hit;
      throw error;
    }
  })());
});
