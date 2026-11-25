// Kern application host: runs a game written in C++ against kern::App
// (kern/include/kern/app/app.h) in a canvas. Owns only what the browser
// forces into JavaScript: the WebGL2 context, the frame callback, the
// drawing-buffer size and input events, all forwarded to the module.
// The input events themselves are read by kern.mjs' BrowserInput (the same
// for every Kern host); this file says what they are to kern::Input.
import {loadKern, BrowserInput} from "./kern.mjs";

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

/**
 * Starts the app in `canvas`. Module source: one of bytes / base64 / url / module.
 * `onStatus(text)` receives kern::Engine::set_status text.
 * `maxPixelRatio` caps the drawing-buffer density (default: full device ratio).
 * The page's own share of the input (all optional, after the app had the event; BrowserInput):
 * `onKey(event, down)`, `onLook(dx, dy)` - the locked pointer's movement in css px -,
 * `onLock(locked)`, `onDevice(kind, event)`, `onFingersGone()`, `onVisibility(hidden)`.
 */
export async function runKernApp({canvas, bytes = null, base64 = null, url = null, module = null, onStatus = null, onError = null,
  maxPixelRatio = Infinity, contextAttributes = {}, args = globalThis.location?.search ?? "", onKey = null, onLook = null, onLock = null, onDevice = null, onFingersGone = null,
  onVisibility = null} = {}) {
  if (!canvas?.getContext) throw Error("canvas required");
  const kern = await loadKern({bytes, base64, url, module});
  const e = kern.exports;
  if (!e.kern_app_start) throw Error("module was not built with --app");
  const gl = canvas.getContext("webgl2", {alpha: false, antialias: false, depth: true, powerPreference: "high-performance", ...contextAttributes});
  if (!gl) throw Error("WebGL2 required");
  kern.attachContext(canvas, gl);
  kern.statusSink = text => onStatus?.(text);
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
  if (!e.kern_app_start(width, height)) throw kern.fail("app start");

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
    stop() {
      running = false;
      xr?.session.end().catch(() => {});
      cancelAnimationFrame(handle);
      observer?.disconnect();
      for (const off of listeners) off();
      e.kern_app_stop();
    },
  };
}
