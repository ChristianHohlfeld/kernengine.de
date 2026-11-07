// SpawnCity on Kern - a picture or a short clip of the game to share: taken from the game's canvas
// (its own frames, not the page around it), marked with the site, handed to the system's share
// sheet where there is one (phones), else saved as a file.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).

const canvasOf = () => document.getElementById("game");
const stamp = () => new Date().toISOString().slice(0, 19).replace(/[:T]/g, "-");

// a frame of the canvas as it is shown (a stream of it: the drawing buffer itself is cleared after
// every frame)
async function grabFrame(canvas) {
  const stream = canvas.captureStream(30);
  const video = document.createElement("video");
  video.muted = true; video.playsInline = true; video.srcObject = stream;
  try {
    await video.play();
    await new Promise(res => (video.requestVideoFrameCallback ? video.requestVideoFrameCallback(() => res()) : setTimeout(res, 120)));
    const c = document.createElement("canvas");
    c.width = video.videoWidth || canvas.width; c.height = video.videoHeight || canvas.height;
    c.getContext("2d").drawImage(video, 0, 0, c.width, c.height);
    return c;
  } finally {
    for (const t of stream.getTracks()) t.stop();
    video.srcObject = null;
  }
}

function mark(c, line) {
  const g = c.getContext("2d"), s = Math.max(1, c.height / 540);
  g.font = `900 ${Math.round(18 * s)}px Inter, system-ui, sans-serif`;
  g.textBaseline = "bottom";
  const text = line ? `${line}  ·  spawncity.de` : "spawncity.de";
  const w = g.measureText(text).width, pad = 10 * s;
  g.fillStyle = "#0b1118c0";
  g.fillRect(c.width - w - pad * 3, c.height - 34 * s - pad, w + pad * 2, 30 * s);
  g.fillStyle = "#ffd76a";
  g.fillText(text, c.width - w - pad * 2, c.height - pad - 8 * s);
}

async function hand(file, title) {
  // the share sheet (phones, some desktops), else the file to the downloads
  try {
    if (navigator.canShare?.({files: [file]})) { await navigator.share({files: [file], title, text: `${title} – spawncity.de`}); return "geteilt"; }
  } catch (e) { if (e?.name === "AbortError") return "abgebrochen"; }
  const a = document.createElement("a");
  a.href = URL.createObjectURL(file); a.download = file.name;
  document.body.appendChild(a); a.click(); a.remove();
  setTimeout(() => URL.revokeObjectURL(a.href), 4000);
  return "gespeichert";
}

export async function sharePicture(line = "") {
  const canvas = canvasOf();
  if (!canvas?.captureStream) return "nicht möglich";
  const c = await grabFrame(canvas);
  mark(c, line);
  const blob = await new Promise(res => c.toBlob(res, "image/jpeg", 0.9));
  if (!blob) return "nicht möglich";
  return hand(new File([blob], `spawncity-${stamp()}.jpg`, {type: "image/jpeg"}), "SpawnCity");
}

let recording = null;
export const clipRunning = () => !!recording;

// the next seconds of the game as a video (webm, or mp4 where only that is recorded)
export async function shareClip(seconds = 10, onTick = () => {}) {
  if (recording) return "läuft schon";
  const canvas = canvasOf();
  if (!canvas?.captureStream || typeof MediaRecorder === "undefined") return "nicht möglich";
  const type = ["video/mp4;codecs=avc1", "video/webm;codecs=vp9", "video/webm;codecs=vp8", "video/webm", "video/mp4"].find(t => MediaRecorder.isTypeSupported?.(t));
  if (!type) return "nicht möglich";
  const stream = canvas.captureStream(30);
  const rec = new MediaRecorder(stream, {mimeType: type, videoBitsPerSecond: 6_000_000});
  const parts = [];
  rec.ondataavailable = e => { if (e.data?.size) parts.push(e.data); };
  recording = rec;
  const done = new Promise(res => { rec.onstop = res; });
  rec.start(1000);
  const t0 = performance.now();
  const timer = setInterval(() => onTick(Math.max(0, seconds - (performance.now() - t0) / 1000)), 250);
  await new Promise(res => setTimeout(res, seconds * 1000));
  rec.stop();
  await done;
  clearInterval(timer);
  for (const t of stream.getTracks()) t.stop();
  recording = null;
  onTick(0);
  const ext = type.startsWith("video/mp4") ? "mp4" : "webm";
  const blob = new Blob(parts, {type: type.split(";")[0]});
  return hand(new File([blob], `spawncity-${stamp()}.${ext}`, {type: blob.type}), "SpawnCity Clip");
}
