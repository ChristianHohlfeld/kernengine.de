// SpawnCity on Kern - the big map's ground: OpenFreeMap's vector tiles (OpenMapTiles schema,
// (c) OpenStreetMap contributors) read and drawn here, so the map shows the whole world at every
// zoom - as the original's did (world_map_full.mjs on MapLibre with the "liberty" style; its
// colours are followed). Pure functions, used in a worker (world_map_worker.mjs) or on the page.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).

// ---- vector tile (protobuf) -----------------------------------------------------------------
const WANT = new Set(["water", "waterway", "landcover", "landuse", "park", "boundary", "transportation", "building", "aeroway", "place", "water_name", "transportation_name"]);
const KEEP = new Set(["class", "subclass", "brunnel", "admin_level", "maritime", "name", "name:de", "name:latin", "name_de", "name_en", "rank", "ref", "intermittent"]);

class Pbf {
  constructor(b, pos = 0, end = b.length) { this.b = b; this.pos = pos; this.end = end; }
  varint() {
    let r = 0, s = 0, v;
    do { v = this.b[this.pos++]; if (s < 28) r |= (v & 127) << s; else r += (v & 127) * 2 ** s; s += 7; } while (v & 128);
    return s > 28 ? r : r >>> 0;
  }
  skip(type) {
    if (type === 0) this.varint();
    else if (type === 1) this.pos += 8;
    else if (type === 2) this.pos += this.varint();
    else if (type === 5) this.pos += 4;
    else throw Error("pbf wire type " + type);
  }
}
const text = new TextDecoder();

function readValue(p, end) {
  let out = null;
  while (p.pos < end) {
    const k = p.varint(), f = k >> 3, t = k & 7;
    if (f === 1) { const n = p.varint(); out = text.decode(p.b.subarray(p.pos, p.pos + n)); p.pos += n; }
    else if (f === 2) { out = new DataView(p.b.buffer, p.b.byteOffset + p.pos, 4).getFloat32(0, true); p.pos += 4; }
    else if (f === 3) { out = new DataView(p.b.buffer, p.b.byteOffset + p.pos, 8).getFloat64(0, true); p.pos += 8; }
    else if (f === 4 || f === 5) out = p.varint();
    else if (f === 6) { const v = p.varint(); out = (v >>> 1) ^ -(v & 1); }
    else if (f === 7) out = p.varint() !== 0;
    else p.skip(t);
  }
  return out;
}

/// The tile's layers: {name: {extent, features: [{type (1 point, 2 line, 3 polygon), rings: [Int32Array x,y,...], p: {..}}]}}.
export function parseTile(bytes) {
  const layers = {};
  const p = new Pbf(bytes);
  while (p.pos < p.end) {
    const k = p.varint();
    if (k >> 3 !== 3 || (k & 7) !== 2) { p.skip(k & 7); continue; }
    const end = p.pos + p.varint(), start = p.pos;
    // first pass: the name, the keys, the values
    let name = "", extent = 4096;
    const keys = [], values = [], feats = [];
    while (p.pos < end) {
      const kk = p.varint(), f = kk >> 3, t = kk & 7;
      if (f === 1) { const n = p.varint(); name = text.decode(p.b.subarray(p.pos, p.pos + n)); p.pos += n; }
      else if (f === 2) { const n = p.varint(); feats.push([p.pos, p.pos + n]); p.pos += n; }
      else if (f === 3) { const n = p.varint(); keys.push(text.decode(p.b.subarray(p.pos, p.pos + n))); p.pos += n; }
      else if (f === 4) { const n = p.varint(), e = p.pos + n; values.push(readValue(p, e)); p.pos = e; }
      else if (f === 5) extent = p.varint();
      else p.skip(t);
    }
    p.pos = end;
    if (!WANT.has(name)) continue;
    const features = [];
    for (const [a, b] of feats) {
      const q = new Pbf(bytes, a, b);
      let type = 0, tags = null, geom = null;
      while (q.pos < b) {
        const kk = q.varint(), f = kk >> 3, t = kk & 7;
        if (f === 2) { const n = q.varint(), e = q.pos + n; tags = []; while (q.pos < e) tags.push(q.varint()); }
        else if (f === 3) type = q.varint();
        else if (f === 4) { const n = q.varint(); geom = [q.pos, q.pos + n]; q.pos += n; }
        else q.skip(t);
      }
      if (!geom || !type) continue;
      const props = {};
      if (tags) for (let i = 0; i + 1 < tags.length; i += 2) { const key = keys[tags[i]]; if (KEEP.has(key)) props[key] = values[tags[i + 1]]; }
      // geometry: MoveTo 1, LineTo 2, ClosePath 7 (zigzag deltas)
      const g = new Pbf(bytes, geom[0], geom[1]);
      const rings = [];
      let cur = null, x = 0, y = 0;
      while (g.pos < geom[1]) {
        const c = g.varint(), cmd = c & 7, n = c >> 3;
        if (cmd === 1 || cmd === 2) {
          for (let i = 0; i < n; i++) {
            const dx = g.varint(), dy = g.varint();
            x += (dx >>> 1) ^ -(dx & 1); y += (dy >>> 1) ^ -(dy & 1);
            if (cmd === 1) { if (cur && cur.length) rings.push(cur); cur = []; }
            cur.push(x, y);
          }
        } else if (cmd === 7 && cur) cur.closed = true;
      }
      if (cur && cur.length) rings.push(cur);
      features.push({type, rings, p: props});
    }
    layers[name] = {extent, features};
    void start;
  }
  return layers;
}

// ---- style (OpenFreeMap "liberty", reduced) ---------------------------------------------------
const C = {
  land: "#f8f4f0", water: "#a0c8f0", wood: "#d0e4c0", grass: "#d8e8c8", park: "#d8e8c8", sand: "#f2e9c4", ice: "#ffffff", farmland: "#f0ecdc",
  residential: "#eeeae6", industrial: "#ebe4dc", building: "#dfdbd7", buildingLine: "#cfcac4", boundary: "#9e9cab", rail: "#b9b6b0", runway: "#e2e0de",
  casing: "#cfcdca", road: "#ffffff", major: "#ffeeaa", majorCasing: "#e9ac77", motorway: "#ffcc88", path: "#cbb9a5",
};
// class -> [min zoom, width in metres at the equator-less scale used below, kind]
const ROADS = {
  motorway: [5, 16, "motorway"], trunk: [6, 14, "major"], primary: [8, 12, "major"], secondary: [10, 10, "road"], tertiary: [11, 9, "road"],
  minor: [13, 7, "road"], service: [14, 4.5, "road"], track: [14, 3, "path"], path: [15, 2, "path"], pedestrian: [14, 4, "path"], raceway: [12, 8, "road"], busway: [13, 7, "road"],
};

function trace(g, ring) {
  g.moveTo(ring[0], ring[1]);
  for (let i = 2; i < ring.length; i += 2) g.lineTo(ring[i], ring[i + 1]);
}

/// Draws display tile (zd, x, y) from the vector tile of zoom zv that holds it (zd >= zv) into a
/// 2D context of `size` pixels. `zoomNow`: the zoom the tile is made for (line widths, what shows).
export function renderTile(g, size, layers, zd, x, y, zv) {
  const d = zd - zv, n = 2 ** d, ext = 4096, sub = ext / n;
  const ox = (x - ((x >> d) << d)) * sub, oy = (y - ((y >> d) << d)) * sub;
  const k = size / sub;  // pixels per tile unit
  g.setTransform(1, 0, 0, 1, 0, 0);
  g.fillStyle = C.land;
  g.fillRect(0, 0, size, size);
  g.setTransform(k, 0, 0, k, -ox * k, -oy * k);
  g.lineJoin = g.lineCap = "round";
  const px = v => v / k;  // a width in pixels as tile units
  const L = name => layers[name]?.features || [];
  const fillAll = (feats, color, test) => {
    g.fillStyle = color;
    g.beginPath();
    let any = false;
    for (const f of feats) {
      if (f.type !== 3 || (test && !test(f.p))) continue;
      for (const r of f.rings) { trace(g, r); g.closePath(); any = true; }
    }
    if (any) g.fill("evenodd");
  };
  // ground cover
  if (zd >= 9) {
    fillAll(L("landuse"), C.residential, p => p.class === "residential" || p.class === "suburb" || p.class === "neighbourhood");
    fillAll(L("landuse"), C.industrial, p => p.class === "industrial" || p.class === "commercial" || p.class === "retail" || p.class === "railway");
  }
  fillAll(L("landcover"), C.farmland, p => p.class === "farmland");
  fillAll(L("landcover"), C.grass, p => p.class === "grass");
  fillAll(L("landcover"), C.wood, p => p.class === "wood");
  fillAll(L("landcover"), C.sand, p => p.class === "sand");
  fillAll(L("landcover"), C.ice, p => p.class === "ice");
  if (zd >= 8) fillAll(L("park"), C.park);
  if (zd >= 11) fillAll(L("landuse"), C.park, p => p.class === "cemetery" || p.class === "pitch" || p.class === "playground" || p.class === "garden" || p.class === "stadium");
  // water
  fillAll(L("water"), C.water);
  if (zd >= 8) {
    g.strokeStyle = C.water;
    for (const f of L("waterway")) {
      if (f.type !== 2) continue;
      const big = f.p.class === "river" || f.p.class === "canal";
      if (!big && zd < 12) continue;
      g.lineWidth = px(big ? Math.max(1.2, 2 ** (zd - 12)) : 1);
      g.beginPath();
      for (const r of f.rings) trace(g, r);
      g.stroke();
    }
  }
  // runways
  if (zd >= 10) {
    g.strokeStyle = C.runway;
    for (const f of L("aeroway")) {
      if (f.type !== 2) continue;
      g.lineWidth = px(f.p.class === "runway" ? Math.max(2, 2 ** (zd - 11)) : Math.max(1, 2 ** (zd - 13)));
      g.beginPath();
      for (const r of f.rings) trace(g, r);
      g.stroke();
    }
  }
  // buildings
  if (zd >= 14) {
    fillAll(L("building"), C.building);
    if (zd >= 15) {
      g.strokeStyle = C.buildingLine; g.lineWidth = px(0.8);
      g.beginPath();
      for (const f of L("building")) if (f.type === 3) for (const r of f.rings) { trace(g, r); g.closePath(); }
      g.stroke();
    }
  }
  // streets: casings first, then the fills, least important class first
  const mpu = 40075016.686 / 2 ** zv / ext;      // metres per tile unit at the equator
  const order = ["path", "track", "pedestrian", "service", "minor", "busway", "raceway", "tertiary", "secondary", "primary", "trunk", "motorway"];
  const byClass = {};
  const rails = [];
  for (const f of L("transportation")) {
    if (f.type !== 2) continue;
    const c = f.p.class;
    if (c === "rail" || c === "transit") { if (zd >= 11 && f.p.brunnel !== "tunnel") rails.push(f); continue; }
    const spec = ROADS[c];
    if (!spec || zd < spec[0]) continue;
    (byClass[c] ||= []).push(f);
  }
  const width = spec => Math.max(spec[2] === "path" ? 0.8 : zd >= 13 ? 1.6 : 1.1, (spec[1] / mpu) * k * 0.9);  // pixels
  const strokeClass = (c, color, extra, dash) => {
    const feats = byClass[c];
    if (!feats) return;
    const spec = ROADS[c];
    g.strokeStyle = color;
    g.lineWidth = px(width(spec) + extra);
    g.setLineDash(dash ? [px(4), px(3)] : []);
    for (const tunnel of [true, false]) {
      g.globalAlpha = tunnel ? 0.45 : 1;
      g.beginPath();
      let any = false;
      for (const f of feats) { if ((f.p.brunnel === "tunnel") !== tunnel) continue; for (const r of f.rings) trace(g, r); any = true; }
      if (any) g.stroke();
    }
    g.globalAlpha = 1;
    g.setLineDash([]);
  };
  if (zd >= 12) for (const c of order) { const kind = ROADS[c][2]; if (kind !== "path") strokeClass(c, kind === "road" ? C.casing : C.majorCasing, zd >= 14 ? 2 : 1.2, false); }
  for (const c of order) {
    const kind = ROADS[c][2];
    strokeClass(c, kind === "path" ? C.path : kind === "road" ? C.road : kind === "major" ? C.major : C.motorway, 0, kind === "path" && c !== "pedestrian");
  }
  if (rails.length) {
    g.strokeStyle = C.rail; g.lineWidth = px(zd >= 14 ? 1.6 : 1);
    g.beginPath();
    for (const f of rails) for (const r of f.rings) trace(g, r);
    g.stroke();
  }
  // borders
  for (const f of L("boundary")) {
    if (f.type !== 2 || f.p.maritime) continue;
    const lvl = +f.p.admin_level || 9;
    if (lvl > 4 || (lvl > 2 && zd < 5)) continue;
    g.strokeStyle = C.boundary;
    g.lineWidth = px(lvl <= 2 ? 1.6 : 1);
    g.setLineDash(lvl <= 2 ? [] : [px(5), px(3)]);
    g.beginPath();
    for (const r of f.rings) trace(g, r);
    g.stroke();
  }
  g.setLineDash([]);
  g.setTransform(1, 0, 0, 1, 0, 0);
}

/// The names of a vector tile (zoom zv, tile xv / yv), in web-mercator world units (0..1):
/// places, waters, streets (the middle of their longest run, with its direction).
export function labelsOf(layers, zv, xv, yv) {
  const out = [], n = 2 ** zv, u = 1 / (n * 4096);
  const nameOf = p => p["name:de"] || p.name_de || p["name:latin"] || p.name || p.name_en || "";
  for (const f of layers.place?.features || []) {
    const name = nameOf(f.p);
    if (!name || f.type !== 1) continue;
    const r = f.rings[0];
    if (r[0] < 0 || r[1] < 0 || r[0] >= 4096 || r[1] >= 4096) continue;  // the neighbour tile's (buffer)
    out.push({k: "place", c: f.p.class || "", rank: +f.p.rank || 20, name, x: (xv * 4096 + r[0]) * u, y: (yv * 4096 + r[1]) * u});
  }
  for (const f of layers.water_name?.features || []) {
    const name = nameOf(f.p);
    if (!name) continue;
    const r = f.rings[0], i = f.type === 1 ? 0 : (r.length >> 2) << 1;
    if (r[i] < 0 || r[i + 1] < 0 || r[i] >= 4096 || r[i + 1] >= 4096) continue;
    out.push({k: "water", c: f.p.class || "", rank: 10, name, x: (xv * 4096 + r[i]) * u, y: (yv * 4096 + r[i + 1]) * u});
  }
  if (zv >= 13) {
    const seen = new Set();
    for (const f of layers.transportation_name?.features || []) {
      const name = nameOf(f.p);
      if (!name || f.type !== 2 || seen.has(name)) continue;
      // the longest straight run of the line
      let best = 0, bx = 0, by = 0, ba = 0;
      for (const r of f.rings)
        for (let i = 0; i + 3 < r.length; i += 2) {
          const dx = r[i + 2] - r[i], dy = r[i + 3] - r[i + 1], l = Math.hypot(dx, dy);
          if (l > best) { best = l; bx = (r[i] + r[i + 2]) / 2; by = (r[i + 1] + r[i + 3]) / 2; ba = Math.atan2(dy, dx); }
        }
      if (best < 60 || bx < 0 || by < 0 || bx >= 4096 || by >= 4096) continue;
      seen.add(name);
      if (ba > Math.PI / 2) ba -= Math.PI; else if (ba < -Math.PI / 2) ba += Math.PI;
      out.push({k: "road", c: f.p.class || "", rank: 30, name, x: (xv * 4096 + bx) * u, y: (yv * 4096 + by) * u, a: ba, len: best * u});
    }
  }
  return out;
}
