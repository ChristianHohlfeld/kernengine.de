// Kern application host: runs a game written in C++ against kern::App
// (kern/include/kern/app/app.h) in a canvas. Owns only what the browser
// forces into JavaScript: the WebGL2 context, the frame callback, the
// drawing-buffer size and input events, all forwarded to the module.
import {loadKern} from "./kern.mjs";

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
 */
export async function runKernApp({canvas, bytes = null, base64 = null, url = null, module = null, onStatus = null, onError = null,
  maxPixelRatio = Infinity, contextAttributes = {}, args = globalThis.location?.search ?? ""} = {}) {
  if (!canvas?.getContext) throw Error("canvas required");
  const kern = await loadKern({bytes, base64, url, module});
  const e = kern.exports;
  if (!e.kern_app_start) throw Error("module was not built with --app");
  const gl = canvas.getContext("webgl2", {alpha: false, antialias: false, depth: true, powerPreference: "high-performance", ...contextAttributes});
  if (!gl) throw Error("WebGL2 required");
  kern.attachContext(canvas, gl);
  kern.statusSink = text => onStatus?.(text);
  kern.pointerLockSink = lock => {
    try {
      if (lock && document.pointerLockElement !== canvas) canvas.requestPointerLock?.();
      if (!lock && document.pointerLockElement === canvas) document.exitPointerLock?.();
    } catch {}
  };

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
  on(window, "keydown", event => {
    const k = KEY_INDEX.get(event.code);
    if (k === undefined) return;
    if (GAME_KEYS.has(event.code)) event.preventDefault();
    if (!event.repeat) e.kern_app_key(k, 1);
  });
  on(window, "keyup", event => { const k = KEY_INDEX.get(event.code); if (k !== undefined) e.kern_app_key(k, 0); });
  on(window, "blur", () => e.kern_app_focus(0));
  on(window, "focus", () => e.kern_app_focus(1));
  canvas.style.touchAction = "none";
  on(canvas, "pointerdown", event => {
    const [x, y] = local(event);
    if (event.pointerType === "touch") e.kern_app_touch(event.pointerId, 0, x, y);
    else e.kern_app_pointer(x, y, 0, 0, event.buttons);
    canvas.setPointerCapture?.(event.pointerId);
  });
  on(canvas, "pointermove", event => {
    const [x, y] = local(event), r = ratio();
    if (event.pointerType === "touch") e.kern_app_touch(event.pointerId, 1, x, y);
    else e.kern_app_pointer(x, y, (event.movementX || 0) * r, (event.movementY || 0) * r, event.buttons);
  });
  const up = event => {
    const [x, y] = local(event);
    if (event.pointerType === "touch") e.kern_app_touch(event.pointerId, 2, x, y);
    else e.kern_app_pointer(x, y, 0, 0, event.buttons);
  };
  on(canvas, "pointerup", up);
  on(canvas, "pointercancel", up);
  on(canvas, "wheel", event => { event.preventDefault(); e.kern_app_wheel(event.deltaY); }, {passive: false});
  on(canvas, "contextmenu", event => event.preventDefault());
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
  const enterXR = async ({onFrame = null, onEnd = null, mode = "immersive-vr"} = {}) => {
    if (xr || !navigator.xr || !e.kern_app_frame_xr) return false;
    const session = await navigator.xr.requestSession(mode, {optionalFeatures: ["local-floor", "bounded-floor", "hand-tracking"]});
    await gl.makeXRCompatible?.();
    const layer = new XRWebGLLayer(session, gl, {antialias: false, depth: true, alpha: false, framebufferScaleFactor: 1});
    session.updateRenderState({baseLayer: layer, depthNear: 0.05, depthFar: 1800});
    const space = await session.requestReferenceSpace("local").catch(() => session.requestReferenceSpace("viewer"));
    const ptr = e.kern_alloc(4 * 27 * 4);
    xr = {session, layer, space, ptr};
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
      if (!e.kern_app_frame_xr(now, views.length, ptr)) console.warn(kern.fail("xr frame"));
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
