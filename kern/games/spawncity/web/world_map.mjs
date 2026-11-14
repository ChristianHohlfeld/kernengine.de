// SpawnCity on Kern - the big map's ground: the whole world, from OpenFreeMap's vector tiles
// (the same service the city is built from), at every zoom from the continents down to the house.
// The tiles are fetched, read and drawn in a worker (world_map_worker.mjs) and kept as bitmaps;
// the names are set on top every frame so they stay sharp and upright. Web Mercator, zoom numbers
// as MapLibre's (a tile is 512 px at its own zoom) - the original's map (world_map_full.mjs).
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
const TILE = 512, MAX_VT = 14, LAND = "#f8f4f0";

export const mercX = lon => (lon + 180) / 360;
export const mercY = lat => { const s = Math.sin(Math.max(-85.0511, Math.min(85.0511, lat)) * Math.PI / 180); return 0.5 - Math.log((1 + s) / (1 - s)) / (4 * Math.PI); };
export const lonOf = x => x * 360 - 180;
export const latOf = y => Math.atan(Math.sinh(Math.PI * (1 - 2 * y))) * 180 / Math.PI;

export class WorldBasemap {
  constructor(templateOf) {
    this.templateOf = templateOf;
    this.tiles = new Map();    // "zd/x/y" -> ImageBitmap | canvas (the last ones drawn stay)
    this.labels = new Map();   // "zv/xv/yv" -> names
    this.pending = new Map();  // id -> key
    this.failed = new Map();   // key -> when to ask again
    this.seq = 0;
    this.size = Math.round(TILE * Math.min(1.5, Math.max(1, globalThis.devicePixelRatio || 1)));
    this.cap = /Mobi|Android|iPhone|iPad/i.test(navigator.userAgent || "") ? 28 : 48;
    try {
      if (globalThis.__scMapTiles) throw Error("single file: no worker file beside it");
      this.worker = new Worker(new URL("./world_map_worker.mjs", import.meta.url), {type: "module"});
      this.worker.onmessage = ev => this.took(ev.data);
      this.worker.onerror = () => { this.worker = null; this.pending.clear(); };  // (the page draws them itself then)
    } catch { this.worker = null; }
  }
  dispose() {
    this.worker?.terminate();
    for (const b of this.tiles.values()) b.close?.();
    this.tiles.clear();
  }
  took(m) {
    const key = this.pending.get(m.id);
    this.pending.delete(m.id);
    if (!key) { m.bitmap?.close?.(); return; }
    if (!m.ok) { this.failed.set(key, performance.now() + 4000); return; }
    this.tiles.set(key, m.bitmap);
    if (m.labels) { const [zd, x, y] = key.split("/").map(Number), d = Math.max(0, zd - MAX_VT); this.labels.set(`${zd - d}/${x >> d}/${y >> d}`, m.labels); }
    while (this.tiles.size > this.cap) { const k = this.tiles.keys().next().value; this.tiles.get(k).close?.(); this.tiles.delete(k); }
    while (this.labels.size > 60) this.labels.delete(this.labels.keys().next().value);
  }
  async inline(job) {
    // no worker (an old browser, a single-file build): the same work on the page, a tile at a time
    try {
      const {parseTile, renderTile, labelsOf} = this.lib ||= globalThis.__scMapTiles || await import("./world_map_tiles.mjs");
      const res = await fetch(job.url);
      let bytes = res.status === 204 || res.status === 404 ? new Uint8Array(0) : new Uint8Array(await res.arrayBuffer());
      if (!res.ok && bytes.length) throw Error("tile " + res.status);
      if (bytes.length > 2 && bytes[0] === 0x1f && bytes[1] === 0x8b) bytes = new Uint8Array(await new Response(new Blob([bytes]).stream().pipeThrough(new DecompressionStream("gzip"))).arrayBuffer());
      const layers = bytes.length ? parseTile(bytes) : {};
      const c = document.createElement("canvas"); c.width = c.height = job.size;
      renderTile(c.getContext("2d"), job.size, layers, job.zd, job.x, job.y, job.zv);
      this.took({id: job.id, ok: true, bitmap: c, labels: labelsOf(layers, job.zv, job.xv, job.yv)});
    } catch { this.took({id: job.id, ok: false}); }
  }
  ask(zd, x, y) {
    const key = `${zd}/${x}/${y}`, template = this.templateOf();
    if (!template || this.pending.size >= (this.worker ? 8 : 1)) return;
    for (const k of this.pending.values()) if (k === key) return;
    if ((this.failed.get(key) || 0) > performance.now()) return;
    const d = Math.max(0, zd - MAX_VT), zv = zd - d, xv = x >> d, yv = y >> d;
    const job = {op: "tile", id: ++this.seq, url: template.replace("{z}", zv).replace("{x}", xv).replace("{y}", yv), zd, x, y, zv, xv, yv, size: this.size};
    this.pending.set(job.id, key);
    if (this.worker) this.worker.postMessage(job); else this.inline(job);
  }
  /// The ground of the view (lon / lat at the centre, zoom) on a w x h canvas (CSS pixels).
  draw(g, w, h, view) {
    g.fillStyle = LAND;
    g.fillRect(0, 0, w, h);
    const zd = Math.max(0, Math.min(18, Math.round(view.zoom))), n = 2 ** zd, scale = TILE * 2 ** view.zoom, ts = scale / n;
    const cx = mercX(view.lon), cy = mercY(view.lat);
    const x0 = Math.floor((cx - w / 2 / scale) * n), x1 = Math.floor((cx + w / 2 / scale) * n);
    const y0 = Math.max(0, Math.floor((cy - h / 2 / scale) * n)), y1 = Math.min(n - 1, Math.floor((cy + h / 2 / scale) * n));
    const want = [];
    g.imageSmoothingEnabled = true;
    for (let y = y0; y <= y1; y++)
      for (let x = x0; x <= x1; x++) {
        const xx = ((x % n) + n) % n, px = w / 2 + (x / n - cx) * scale, py = h / 2 + (y / n - cy) * scale;
        const key = `${zd}/${xx}/${y}`, bmp = this.tiles.get(key);
        if (bmp) { this.tiles.delete(key); this.tiles.set(key, bmp); g.drawImage(bmp, px, py, ts + 0.5, ts + 0.5); continue; }
        want.push([Math.hypot(px + ts / 2 - w / 2, py + ts / 2 - h / 2), xx, y]);
        // meanwhile: what a coarser tile shows there
        for (let up = 1; up <= 6 && zd - up >= 0; up++) {
          const pk = `${zd - up}/${xx >> up}/${y >> up}`, pb = this.tiles.get(pk);
          if (!pb) continue;
          const f = 2 ** up, s = pb.width / f;
          g.drawImage(pb, (xx % f) * s, (y % f) * s, s, s, px, py, ts + 0.5, ts + 0.5);
          break;
        }
      }
    want.sort((a, b) => a[0] - b[0]);
    for (const [, x, y] of want) this.ask(zd, x, y);
    this.zd = zd;
  }
  /// The names over the ground (after the game's own lines, under its markers).
  drawLabels(g, w, h, view) {
    const zoom = view.zoom, zv = Math.min(MAX_VT, this.zd ?? Math.round(zoom)), n = 2 ** zv, scale = TILE * 2 ** zoom;
    const cx = mercX(view.lon), cy = mercY(view.lat);
    const x0 = Math.floor((cx - w / 2 / scale) * n), x1 = Math.floor((cx + w / 2 / scale) * n), y0 = Math.floor((cy - h / 2 / scale) * n), y1 = Math.floor((cy + h / 2 / scale) * n);
    const list = [];
    const show = l => {
      if (l.k === "road") return zoom >= 15.3 ? 30 : 0;
      if (l.k === "water") return (l.c === "ocean" || l.c === "sea") ? (zoom < 9 ? 40 : 0) : zoom >= 9 ? 35 : 0;
      switch (l.c) {
        case "continent": return zoom < 3.2 ? 100 : 0;
        case "country": return zoom < 7.5 ? 95 - l.rank : 0;
        case "state": case "province": return zoom >= 4.5 && zoom < 8.5 ? 60 - l.rank : 0;
        case "city": return zoom >= 3.5 && l.rank <= zoom * 2 - 3 ? 90 - l.rank : 0;
        case "town": return zoom >= 8.5 ? 70 - l.rank : 0;
        case "village": return zoom >= 11 ? 55 - l.rank : 0;
        case "suburb": case "quarter": return zoom >= 12 ? 45 : 0;
        case "hamlet": case "neighbourhood": return zoom >= 13.5 ? 40 : 0;
        default: return zoom >= 14.5 ? 20 : 0;
      }
    };
    for (let y = y0; y <= y1; y++)
      for (let x = x0; x <= x1; x++) {
        const set = this.labels.get(`${zv}/${((x % n) + n) % n}/${y}`);
        if (!set) continue;
        for (const l of set) { const p = show(l); if (p > 0) list.push([p, l]); }
      }
    list.sort((a, b) => b[0] - a[0]);
    const boxes = [];
    g.textAlign = "center"; g.textBaseline = "middle"; g.lineJoin = "round";
    let drawn = 0;
    for (const [, l] of list) {
      if (drawn >= 90) break;
      const px = w / 2 + (l.x - cx) * scale, py = h / 2 + (l.y - cy) * scale;
      if (px < -60 || py < -20 || px > w + 60 || py > h + 20) continue;
      let size = 12, weight = 600, color = "#333", name = l.name;
      if (l.k === "water") { color = "#4a6fb0"; size = 12; }
      else if (l.k === "road") { size = 11; color = "#555"; }
      else if (l.c === "country" || l.c === "continent") { size = 15; weight = 800; name = name.toUpperCase(); }
      else if (l.c === "city") { size = l.rank <= 3 ? 17 : 15; weight = 800; }
      else if (l.c === "town") { size = 13.5; weight = 700; }
      else if (l.c === "suburb" || l.c === "quarter" || l.c === "neighbourhood") { size = 11; color = "#666"; name = name.toUpperCase(); }
      g.font = `${l.k === "water" ? "italic " : ""}${weight} ${size}px Inter, system-ui, sans-serif`;
      const tw = g.measureText(name).width, r = l.k === "road" ? Math.max(tw, size) / 2 + 2 : 0;
      if (l.k === "road" && l.len * scale < tw * 0.9 + 20) continue;  // the street is shorter than its name here
      const box = l.k === "road" ? [px - r, py - r, px + r, py + r] : [px - tw / 2 - 4, py - size / 2 - 3, px + tw / 2 + 4, py + size / 2 + 3];
      if (boxes.some(b => box[0] < b[2] && box[2] > b[0] && box[1] < b[3] && box[3] > b[1])) continue;
      boxes.push(box);
      g.save();
      g.translate(px, py);
      if (l.k === "road") g.rotate(l.a);
      g.strokeStyle = "#ffffffe6"; g.lineWidth = 3.2; g.strokeText(name, 0, 0);
      g.fillStyle = color; g.fillText(name, 0, 0);
      g.restore();
      drawn++;
    }
    g.textAlign = "start"; g.textBaseline = "alphabetic";
  }
}
