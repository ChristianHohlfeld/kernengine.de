// Kern application host: runs a game written in C++ against kern::App
// (kern/include/kern/app/app.h) in a canvas. Owns only what the browser
// forces into JavaScript: the WebGL2 context, the frame callback, the
// drawing-buffer size, input events and the sound device (the engine's mixer,
// kern/docs/app.md 3.), all forwarded to the module.
// The input events themselves are read by kern.mjs' BrowserInput (the same
// for every Kern host); this file says what they are to kern::Input.
import {loadKern, BrowserInput, readGamepad} from "./kern.mjs";
import {KernAudio} from "./audio.mjs";

export const KERN_APP_VERSION = "kern-app-host-1";

/** KeyboardEvent.code values in kern::Key order (index = enum value). */
export const KEY_CODES = Object.freeze([
  "",
  ..."ABCDEFGHIJKLMNOPQRSTUVWXYZ".split("").map(c => "Key" + c),
  ..."0123456789".split("").map(c => "Digit" + c),
  "ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown", "Space", "Enter", "Escape", "Tab", "Backspace",
  "ShiftLeft", "ShiftRight", "ControlLeft", "ControlRight", "AltLeft", "AltRight",
]);
const KEY_INDEX = new Map(KEY_CODES.map((code, i) => [code, i]));
const GAME_KEYS = new Set(["ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown", "Space", "Tab"]);

const UI_STYLE = `
.kern-ui { position: fixed; pointer-events: none; overflow: hidden; font: 15px system-ui, sans-serif; color: #eef3f8; z-index: 5; }
.kern-ui > * { position: absolute; max-width: 90%; box-sizing: border-box; }
.kern-ui-text { padding: 6px 10px; background: rgba(0,0,0,.5); border-radius: 6px; white-space: pre; }
.kern-ui-bar { width: 25%; padding: 4px 8px; background: rgba(0,0,0,.5); border-radius: 6px; }
.kern-ui-bar > div { height: 8px; margin-top: 3px; background: rgba(255,255,255,.2); border-radius: 4px; overflow: hidden; }
.kern-ui-bar > div > div { height: 100%; background: #f2c230; }
.kern-ui button { pointer-events: auto; font: inherit; color: inherit; padding: 8px 16px; margin: 2px 0; background: rgba(20,40,70,.85);
  border: 1px solid rgba(255,255,255,.35); border-radius: 8px; cursor: pointer; display: block; width: 100%; }
.kern-ui button:hover, .kern-ui button:focus-visible { background: rgba(40,80,140,.95); outline: none; }
.kern-ui-menu { padding: 6px; background: rgba(0,0,0,.55); border-radius: 10px; min-width: 12em; }
`;

/** The widgets' layer over `canvas`: `show(list)` draws the list the engine handed over, `event(id, index)` goes back. */
/**
 * The app's widgets (kern/docs/app.md 6.) drawn over `canvas` (class kern-ui): show(list) with the list the engine hands
 * over; `event(id, index)` when one is clicked (a button 0, a menu's entry). The page of a game and the editor's Play
 * Project (kern/web/studio) draw them alike.
 */
export function mountUi(canvas, event, onStatus) {
  const doc = canvas.ownerDocument;
  if (!doc.getElementById("kern-ui-style")) {
    const st = doc.createElement("style");
    st.id = "kern-ui-style";
    st.textContent = UI_STYLE;
    doc.head.appendChild(st);
  }
  const layer = doc.createElement("div");
  layer.className = "kern-ui";
  doc.body.appendChild(layer);
  const fit = () => {
    const r = canvas.getBoundingClientRect();
    Object.assign(layer.style, {left: r.left + "px", top: r.top + "px", width: r.width + "px", height: r.height + "px"});
  };
  fit();
  const observer = typeof ResizeObserver === "function" ? new ResizeObserver(fit) : null;
  observer?.observe(canvas);
  addEventListener("resize", fit);
  let status = null;
  return {
    layer,
    show(list) {
      fit();
      layer.replaceChildren();
      let st = "";
      for (const w of list) {
        if (w.status) { st = w.text; if (onStatus) continue; }
        let el;
        if (w.kind === "text") { el = doc.createElement("div"); el.className = "kern-ui-text"; el.textContent = w.text; }
        else if (w.kind === "bar") {
          el = doc.createElement("div");
          el.className = "kern-ui-bar";
          el.style.width = (w.width * 100) + "%";
          el.textContent = w.label;
          const track = doc.createElement("div"), fill = doc.createElement("div");
          fill.style.width = (w.value * 100) + "%";
          track.appendChild(fill);
          el.appendChild(track);
        } else if (w.kind === "button") {
          el = doc.createElement("button");
          el.textContent = w.label;
          el.style.width = "auto";
          el.addEventListener("click", ev => { ev.stopPropagation(); event(w.id, 0); });
        } else if (w.kind === "menu") {
          el = doc.createElement("div");
          el.className = "kern-ui-menu";
          w.items.forEach((item, i) => {
            const b = doc.createElement("button");
            b.textContent = item;
            b.addEventListener("click", ev => { ev.stopPropagation(); event(w.id, i); });
            el.appendChild(b);
          });
        } else continue;
        el.dataset.id = w.id;
        el.dataset.kind = w.kind;
        // the widget hangs by the same point of itself: x = 1 right-aligned, 0.5 centred
        Object.assign(el.style, {left: (w.x * 100) + "%", top: (w.y * 100) + "%", transform: `translate(${-w.x * 100}%, ${-w.y * 100}%)`});
        layer.appendChild(el);
      }
      if (st !== status) { status = st; onStatus?.(st); }
    },
    dispose() { observer?.disconnect(); removeEventListener("resize", fit); layer.remove(); },
  };
}

/**
 * Starts the app in `canvas`. Module source: one of bytes / base64 / url / module.
 * The app's widgets (kern::Engine::ui_text, ui_bar, ui_button, ui_menu and the status line set_status) are drawn
 * over the canvas (class kern-ui); `onStatus(text)`, when given, receives the status line instead (a page that
 * draws it itself).
 * `maxPixelRatio` caps the drawing-buffer density (default: full device ratio).
 * `audio` (default true): the engine's mixer (kern::Engine::mixer) is heard - after the page's first gesture, paused
 * while it is hidden (kern/web/audio.mjs); false: no sound device (a page that plays its own).
 * The page's own share of the input (all optional, after the app had the event; BrowserInput):
 * `onKey(event, down)`, `onLook(dx, dy)` - the locked pointer's movement in css px -,
 * `onLock(locked)`, `onDevice(kind, event)`, `onFingersGone()`, `onVisibility(hidden)`.
 */
export async function runKernApp({canvas, bytes = null, base64 = null, url = null, module = null, onStatus = null, onError = null,
  maxPixelRatio = Infinity, contextAttributes = {}, audio = true, args = globalThis.location?.search ?? "", onKey = null, onLook = null, onLock = null, onDevice = null, onFingersGone = null,
  onVisibility = null} = {}) {
  if (!canvas?.getContext) throw Error("canvas required");
  const kern = await loadKern({bytes, base64, url, module});
  const e = kern.exports;
  if (!e.kern_app_start) throw Error("module was not built with --app");
  // the program's one entry (kern/docs/project.md 4.): it hands the engine its App (kern::run) - once, first
  if (!e.kern_main) throw Error("module has no kern_main: the program's entry, which hands the engine its App (kern::run)");
  const gl = canvas.getContext("webgl2", {alpha: false, antialias: false, depth: true, powerPreference: "high-performance", ...contextAttributes});
  if (!gl) throw Error("WebGL2 required");
  kern.attachContext(canvas, gl);
  // the app's widgets (kern/docs/app.md 6.): drawn over the canvas; the status line goes to onStatus when the page
  // draws it itself
  const ui = mountUi(canvas, (id, index) => {
    const text = kern.encoder.encode(id), ptr = e.kern_alloc(text.length + 1);
    kern.u8.set(text, ptr);
    e.kern_app_ui_event(ptr, text.length, index);
    e.kern_free(ptr);
  }, onStatus);
  kern.uiSink = json => ui.show(JSON.parse(json));
  let input = null;
  kern.pointerLockSink = lock => { if (lock) input?.lock(); else input?.unlock(); };  // kern::Engine::request_pointer_lock
  const ratio = () => Math.min(globalThis.devicePixelRatio || 1, maxPixelRatio);
  const size = () => {
    const r = ratio();
    return [Math.max(1, Math.round((canvas.clientWidth || canvas.width) * r)), Math.max(1, Math.round((canvas.clientHeight || canvas.height) * r))];
  };
  let [width, height] = size();
  canvas.width = width;
  canvas.height = height;
  if (args) {
    const text = kern.encoder.encode(String(args)), ptr = e.kern_alloc(text.length + 1);
    kern.u8.set(text, ptr);
    e.kern_app_set_args(ptr, text.length);
    e.kern_free(ptr);
  }
  if (e.kern_main() !== 0) throw kern.fail("kern_main (kern::run)");
  // the sound device, before the start: the App finds its mixer ready at the device's rate (kern/docs/app.md 3.)
  let sound = null;
  const heard = {frames: 0, peak: 0};
  if (audio && KernAudio.supported()) {
    const lenAt = e.kern_alloc(4);
    sound = new KernAudio(frames => {
      const ptr = e.kern_app_audio(frames, lenAt), n = new Uint32Array(kern.memory.buffer, lenAt, 1)[0];
      const pcm = new Float32Array(kern.memory.buffer, ptr, n).slice();
      heard.frames += n >> 1;
      for (let i = 0; i < n; i++) heard.peak = Math.max(heard.peak, Math.abs(pcm[i]));
      return pcm;
    });
    const rate = sound.create();
    if (!rate || !e.kern_app_audio_init(rate)) { console.warn(rate ? kern.fail("audio") : "kern app: no sound device - the app runs without sound"); sound = null; }
  } else if (audio) console.warn("kern app: this browser has no Web Audio - the app runs without sound");
  if (!e.kern_app_start(width, height)) throw kern.fail("app start");
  const pump = sound ? setInterval(() => sound.pump(), 20) : 0;

  const listeners = [];
  const on = (target, type, fn, options) => { target.addEventListener(type, fn, options); listeners.push(() => target.removeEventListener(type, fn, options)); };
  const local = event => {
    const rect = canvas.getBoundingClientRect(), r = ratio();
    return [(event.clientX - rect.left) * r, (event.clientY - rect.top) * r];
  };
  canvas.style.touchAction = "none";
  // kern::Input holds state: the keys, the pointer's place, its movement and the buttons held.
  input = new BrowserInput({
    surface: canvas,
    position: local,
    onKey(event, down) {
      const k = KEY_INDEX.get(event.code);
      if (k !== undefined) {
        if (down && GAME_KEYS.has(event.code)) event.preventDefault();
        if (!down || !event.repeat) e.kern_app_key(k, down ? 1 : 0);
      }
      onKey?.(event, down);
    },
    onMouseButton(button, down, m) { e.kern_app_pointer(m.x, m.y, 0, 0, m.buttons); },
    onMouseMove(m) { const r = ratio(); e.kern_app_pointer(m.x, m.y, m.dx * r, m.dy * r, m.buttons); },
    onMouseCancel(m) { e.kern_app_pointer(m.x, m.y, 0, 0, 0); },
    onTouch(phase, event) { const [x, y] = local(event); e.kern_app_touch(event.pointerId, Math.min(phase, 2), x, y); },
    onWheel(pixels) { e.kern_app_wheel(pixels); },
    onFocus(focused) { e.kern_app_focus(focused ? 1 : 0); },
    onLook: onLook ? (dx, dy) => onLook(dx, dy) : null,
    onLock: locked => onLock?.(locked),
    onDevice,
    onFingersGone,
    onVisibility,
  });
  listeners.push(() => input.dispose());
  on(canvas, "webglcontextlost", event => { event.preventDefault(); e.kern_context_lost(); kern.resetTables(); });
  on(canvas, "webglcontextrestored", () => {
    kern.attachContext(canvas, gl);
    if (!e.kern_context_restored()) onError?.(kern.fail("context restore"));
  });
  const observer = typeof ResizeObserver === "function" ? new ResizeObserver(() => {
    if (xr) return;  // the XR layer sets the size while a session runs
    const [w, h] = size();
    if (w === width && h === height) return;
    width = w; height = h;
    canvas.width = w; canvas.height = h;
    e.kern_app_resize(w, h);
  }) : null;
  observer?.observe(canvas);

  let running = true, handle = 0;
  const frame = now => {
    if (!running) return;
    sound?.pump();
    // the gamepad (standard mapping) for kern::Input::gamepad - read each frame, the browser has no events for it
    const pad = readGamepad();
    if (pad) e.kern_app_gamepad(1, pad.lx, pad.ly, pad.rx, pad.ry, pad.lt, pad.rt, pad.bits >>> 0);
    else e.kern_app_gamepad(0, 0, 0, 0, 0, 0, 0, 0);
    if (!e.kern_app_frame(now, document.hidden ? 1 : 0)) {
      running = false;
      const error = kern.fail("frame");
      if (onError) onError(error); else console.error(error);
      return;
    }
    handle = requestAnimationFrame(frame);
  };
  handle = requestAnimationFrame(frame);
  // ---- WebXR (Meta Quest and the like): an immersive session draws every eye into the XR layer;
  // the app updates once per XR frame (kern_app_frame_xr). `onFrame(frame, session, space)` runs
  // first each frame (the page's own per-frame work and its controllers: window rAF sleeps in XR).
  let xr = null;
  const enterXR = async ({onFrame = null, onEnd = null, mode = "immersive-vr", scale = 0.8} = {}) => {
    if (xr || !navigator.xr || !e.kern_app_frame_xr) return false;
    const session = await navigator.xr.requestSession(mode, {optionalFeatures: ["local-floor", "bounded-floor", "hand-tracking"]});
    await gl.makeXRCompatible?.();
    // a lighter layer than the panel's native size (Quest: 0.8 of ~1.8k x 1.9k per eye) with fixed
    // foveation - a frame that misses the headset's 72 / 90 Hz too often is a frozen headset
    const layer = new XRWebGLLayer(session, gl, {antialias: false, depth: true, alpha: false, framebufferScaleFactor: scale});
    try { if ("fixedFoveation" in layer) layer.fixedFoveation = 1; } catch { /* not offered */ }
    session.updateRenderState({baseLayer: layer, depthNear: 0.05, depthFar: 1800});
    const space = await session.requestReferenceSpace("local").catch(() => session.requestReferenceSpace("viewer"));
    const ptr = e.kern_alloc(4 * 27 * 4);
    xr = {session, layer, space, ptr, w: layer.framebufferWidth, h: layer.framebufferHeight, fails: 0};
    cancelAnimationFrame(handle);
    kern.framebufferOverride = layer.framebuffer;
    e.kern_app_resize(layer.framebufferWidth, layer.framebufferHeight);
    const step = (now, frame) => {
      if (!xr || xr.session !== session) return;
      session.requestAnimationFrame(step);
      try { onFrame?.(frame, session, space); } catch (err) { console.error(err); }
      const pose = frame.getViewerPose(space);
      if (!pose) return;
      const views = pose.views.slice(0, 4), f = new Float32Array(kern.memory.buffer, ptr, views.length * 27);
      views.forEach((v, i) => {
        const vp = layer.getViewport(v), o = i * 27, p = v.transform.position, q = v.transform.orientation;
        f.set(v.projectionMatrix, o);
        f[o + 16] = p.x; f[o + 17] = p.y; f[o + 18] = p.z;
        f[o + 19] = q.x; f[o + 20] = q.y; f[o + 21] = q.z; f[o + 22] = q.w;
        f[o + 23] = vp.x; f[o + 24] = vp.y; f[o + 25] = vp.width; f[o + 26] = vp.height;
      });
      gl.bindFramebuffer(gl.FRAMEBUFFER, layer.framebuffer);
      if (xr.w !== layer.framebufferWidth || xr.h !== layer.framebufferHeight) {
        xr.w = layer.framebufferWidth; xr.h = layer.framebufferHeight;
        e.kern_app_resize(xr.w, xr.h);
      }
      if (!e.kern_app_frame_xr(now, views.length, ptr)) {
        // a failed frame is never a black headset for long: the size is put right and it goes on
        if (++xr.fails % 30 === 1) console.warn(kern.fail("xr frame"));
        xr.w = 0;
      } else xr.fails = 0;
    };
    session.requestAnimationFrame(step);
    session.addEventListener("end", () => {
      if (!xr || xr.session !== session) return;
      e.kern_free(ptr);
      xr = null;
      kern.framebufferOverride = null;
      gl.bindFramebuffer(gl.FRAMEBUFFER, null);
      e.kern_app_resize(width, height);
      if (running) handle = requestAnimationFrame(frame);
      onEnd?.();
    });
    return true;
  };
  return {
    kern,
    enterXR,
    exitXR() { xr?.session.end().catch(() => {}); },
    get inXR() { return !!xr; },
    get xrSpace() { return xr?.space || null; },
    /** The sound device: its state ("none" without one, else the AudioContext's: "suspended" until a gesture,
     *  "running"), how it plays ("worklet" / "script"), the frames it has pulled from the mixer and their peak. */
    get audio() { return {state: sound?.context?.state ?? "none", mode: sound?.mode ?? "none", frames: heard.frames, peak: heard.peak}; },
    stop() {
      running = false;
      clearInterval(pump);
      sound?.close();
      xr?.session.end().catch(() => {});
      cancelAnimationFrame(handle);
      observer?.disconnect();
      for (const off of listeners) off();
      ui.dispose();
      e.kern_app_stop();
    },
  };
}
