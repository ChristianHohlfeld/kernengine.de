// SpawnCity on Kern - service worker (like SpawnCity's oppenheimer-sw.js): the installed app opens
// without a network. Network first, so every start gets the newest build; what was loaded is kept
// and answers when the network is gone. Only this origin's files (the page, its modules, the wasm);
// map tiles and other services are not cached here.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
const CACHE = "spawncity-kern-v1";

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
      const res = await fetch(req);
      if (res.ok) { const copy = res.clone(); caches.open(CACHE).then(c => c.put(req, copy)).catch(() => {}); }
      return res;
    } catch (error) {
      const hit = await caches.match(req, {ignoreSearch: true});
      if (hit) return hit;
      throw error;
    }
  })());
});
