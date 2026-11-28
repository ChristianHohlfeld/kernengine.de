// SpawnCity on Kern - the big map's tiles off the page's thread: fetched, read and drawn here
// (world_map_tiles.mjs), handed back as bitmaps with their names.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
import {parseTile, renderTile, labelsOf} from "./world_map_tiles.mjs";

const vts = new Map();  // "z/x/y" -> Promise<{layers, labels}> (the last 24 vector tiles)

async function vectorTile(url, zv, xv, yv) {
  const key = `${zv}/${xv}/${yv}`;
  let hit = vts.get(key);
  if (hit) { vts.delete(key); vts.set(key, hit); return hit; }
  hit = (async () => {
    const res = await fetch(url);
    let bytes;
    if (res.status === 204 || res.status === 404) bytes = new Uint8Array(0);  // no data there
    else if (!res.ok) throw Error("tile " + res.status);
    else bytes = new Uint8Array(await res.arrayBuffer());
    if (bytes.length > 2 && bytes[0] === 0x1f && bytes[1] === 0x8b) bytes = new Uint8Array(await new Response(new Blob([bytes]).stream().pipeThrough(new DecompressionStream("gzip"))).arrayBuffer());
    const layers = bytes.length ? parseTile(bytes) : {};
    return {layers, labels: labelsOf(layers, zv, xv, yv), sent: false};
  })();
  vts.set(key, hit);
  hit.catch(() => vts.delete(key));
  while (vts.size > 24) vts.delete(vts.keys().next().value);
  return hit;
}

onmessage = async ev => {
  const m = ev.data;
  if (m.op !== "tile") return;
  try {
    const vt = await vectorTile(m.url, m.zv, m.xv, m.yv);
    const c = new OffscreenCanvas(m.size, m.size);
    renderTile(c.getContext("2d"), m.size, vt.layers, m.zd, m.x, m.y, m.zv);
    const bitmap = c.transferToImageBitmap();
    postMessage({id: m.id, ok: true, bitmap, labels: vt.labels}, [bitmap]);
  } catch (error) {
    postMessage({id: m.id, ok: false, error: String(error?.message || error)});
  }
};
