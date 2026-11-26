// Kern's audio output in the browser (kern/docs/app.md 3.): the engine's C++ mixer renders PCM, KernAudio moves it
// to the speakers - an AudioContext (suspended until the first gesture: a key, a pointer, a touch), an AudioWorklet
// (a ScriptProcessor where worklets are blocked), paused while the page is hidden. The one way for every Kern host:
// the app host (kern/web/app.mjs) and the editor's game host (kern/web/studio/game.mjs).

/** AudioWorklet processor: plays queued chunks, reports what it consumed (frames). When the queue runs
 *  dry (the main thread stalled) it never jumps to silence: the last sample glides to zero, and when
 *  data comes back it fades in - a gap without a click. */
const WORKLET_SOURCE = `
const RAMP = 96;  // samples (2 ms): a glide no one hears as a click
class KernOutput extends AudioWorkletProcessor {
  constructor() {
    super();
    this.queue = [];
    this.offset = 0;
    this.consumed = 0;
    this.underruns = 0;
    this.blocks = 0;
    this.lastL = 0;
    this.lastR = 0;
    this.gain = 1;     // fade-in after a gap
    this.dry = false;
    this.port.onmessage = e => { if (e.data === "flush") { this.queue.length = 0; this.offset = 0; } else this.queue.push(e.data); };
  }
  process(inputs, outputs) {
    const out = outputs[0], l = out[0], r = out[1] || out[0], n = l.length;
    let i = 0;
    while (i < n && this.queue.length) {
      const chunk = this.queue[0], frames = chunk.length >> 1;
      const take = Math.min(n - i, frames - this.offset);
      for (let k = 0; k < take; k++) {
        const g = this.gain;
        if (g < 1) this.gain = Math.min(1, g + 1 / RAMP);
        l[i + k] = chunk[(this.offset + k) * 2] * g;
        r[i + k] = chunk[(this.offset + k) * 2 + 1] * g;
      }
      i += take;
      this.offset += take;
      if (this.offset >= frames) { this.queue.shift(); this.offset = 0; }
    }
    if (i > 0) { this.lastL = l[i - 1]; this.lastR = r[i - 1]; this.dry = false; }
    if (i < n) {
      if (!this.dry) { this.underruns++; this.dry = true; }
      for (; i < n; i++) {
        this.lastL *= 1 - 1 / RAMP * 4;
        this.lastR *= 1 - 1 / RAMP * 4;
        if (Math.abs(this.lastL) < 1e-5) this.lastL = 0;
        if (Math.abs(this.lastR) < 1e-5) this.lastR = 0;
        l[i] = this.lastL;
        r[i] = this.lastR;
      }
      this.gain = 0;  // what comes next fades in
    }
    this.consumed += n;
    if (++this.blocks % 4 === 0) this.port.postMessage([this.consumed, this.underruns]);
    return true;
  }
}
registerProcessor("kern-output", KernOutput);
`;

/**
 * Speaker output for the engine mixer. Works everywhere Web Audio does: an
 * AudioWorklet where available (Blob or data: module), a ScriptProcessor
 * otherwise. Browsers start audio only after a user gesture, so the first
 * pointer / key / touch event resumes the context (iOS included; the audio
 * session is set to "playback" so the ring switch does not mute games).
 */
export class KernAudio {
  constructor(render, {latency = 0.07, maxLatency = 0.2} = {}) {
    this.render = render;        // frames -> Float32Array (interleaved stereo)
    this.latency = latency;      // seconds kept queued ahead of the device (grows after a gap)
    this.baseLatency = latency;
    this.maxLatency = maxLatency;
    this.lastUnderruns = 0;
    this.calmSince = 0;
    this.context = null;
    this.node = null;
    this.mode = "none";
    this.written = 0;
    this.consumed = 0;
    this.underruns = 0;
    this.started = false;
  }
  static supported() { return typeof (globalThis.AudioContext || globalThis.webkitAudioContext) === "function"; }
  get rate() { return this.context?.sampleRate || 0; }
  get running() { return this.context?.state === "running"; }
  /** Creates the context (suspended until a gesture) and returns its sample rate. */
  create() {
    if (this.context) return this.rate;
    const Ctx = globalThis.AudioContext || globalThis.webkitAudioContext;
    if (!Ctx) return 0;
    try { if (navigator.audioSession) navigator.audioSession.type = "playback"; } catch { /* not supported */ }
    try { this.context = new Ctx({latencyHint: "interactive"}); } catch { try { this.context = new Ctx(); } catch { return 0; } }
    const unlock = () => {
      if (!this.context) return;
      if (this.context.state !== "running") this.context.resume().catch(() => {});
      if (!this.node) this.connect();
    };
    for (const type of ["pointerdown", "keydown", "touchend", "mousedown"]) addEventListener(type, unlock, {capture: true, passive: true});
    this.unlock = unlock;
    // a hidden page is silent (a phone in the pocket, a tab behind others); back in view it plays on - once the
    // first gesture has unlocked it
    this.visibility = () => {
      const c = this.context;
      if (!c) return;
      if (document.hidden) c.suspend?.().catch(() => {});
      else if (this.node && c.state !== "running") c.resume().catch(() => {});
    };
    document.addEventListener("visibilitychange", this.visibility);
    addEventListener("pagehide", this.visibility);
    return this.rate;
  }
  /** Ends the device: the gesture listeners go, the context closes. */
  close() {
    for (const type of ["pointerdown", "keydown", "touchend", "mousedown"]) removeEventListener(type, this.unlock, {capture: true});
    document.removeEventListener("visibilitychange", this.visibility);
    removeEventListener("pagehide", this.visibility);
    this.node?.disconnect?.();
    this.context?.close?.().catch(() => {});
    this.node = null;
    this.context = null;
    this.mode = "none";
  }
  async connect() {
    if (this.node || this.connecting || !this.context) return;
    this.connecting = true;
    const ctx = this.context;
    if (ctx.audioWorklet && globalThis.AudioWorkletNode) {
      const urls = [];
      try { urls.push(URL.createObjectURL(new Blob([WORKLET_SOURCE], {type: "text/javascript"}))); } catch { /* no blobs */ }
      urls.push("data:text/javascript;base64," + btoa(WORKLET_SOURCE));
      for (const url of urls) {
        try {
          await ctx.audioWorklet.addModule(url);
          const node = new AudioWorkletNode(ctx, "kern-output", {numberOfInputs: 0, numberOfOutputs: 1, outputChannelCount: [2]});
          node.port.onmessage = e => { this.consumed = e.data[0]; this.underruns = e.data[1]; };
          node.connect(ctx.destination);
          this.node = node;
          this.mode = "worklet";
          break;
        } catch { /* blocked by a content security policy: try the next way */ }
      }
    }
    if (!this.node && ctx.createScriptProcessor) {
      const node = ctx.createScriptProcessor(1024, 0, 2);
      node.onaudioprocess = e => {
        const l = e.outputBuffer.getChannelData(0), r = e.outputBuffer.getChannelData(1), n = l.length;
        const pcm = this.render(n);
        for (let i = 0; i < n; i++) { l[i] = pcm[i * 2]; r[i] = pcm[i * 2 + 1]; }
      };
      node.connect(ctx.destination);
      this.node = node;
      this.mode = "script";
    }
    this.connecting = false;
    this.started = !!this.node;
  }
  /** Keeps the worklet's queue `latency` seconds ahead; call every frame (and from a timer). */
  pump() {
    if (this.mode !== "worklet" || !this.running) return;
    // Adaptive lead: a gap (the main thread stalled longer than the queue) adds 30 ms, up to
    // maxLatency; after 15 s without one it eases back towards the base, 5 ms at a time.
    const now = this.context.currentTime;
    if (this.underruns > this.lastUnderruns) {
      this.latency = Math.min(this.maxLatency, this.latency + 0.03);
      this.lastUnderruns = this.underruns;
      this.calmSince = now;
    } else if (this.latency > this.baseLatency && now - this.calmSince > 15) {
      this.latency = Math.max(this.baseLatency, this.latency - 0.005);
      this.calmSince = now;
    }
    const rate = this.rate, target = Math.ceil(this.latency * rate / 128) * 128;
    let queued = this.written - this.consumed;
    if (queued < -rate) { this.written = this.consumed; queued = 0; }  // after a long stall: start over
    let guard = 0;
    while (queued < target && guard++ < 16) {
      const frames = Math.min(2048, Math.max(128, target - queued));
      const pcm = this.render(frames);
      this.node.port.postMessage(pcm, [pcm.buffer]);
      this.written += frames;
      queued += frames;
    }
  }
  /** Decodes a data: URL (any format the browser plays) to interleaved PCM (1 or 2 channels). */
  async decode(src) {
    const comma = src.indexOf(",");
    if (!src.startsWith("data:") || comma < 0) throw Error("not a data URL");
    const meta = src.slice(5, comma), body = src.slice(comma + 1);
    let bytes;
    if (/;base64$/i.test(meta)) {
      const bin = atob(body);
      bytes = new Uint8Array(bin.length);
      for (let i = 0; i < bin.length; i++) bytes[i] = bin.charCodeAt(i);
    } else bytes = new TextEncoder().encode(decodeURIComponent(body));
    const buffer = await new Promise((resolve, reject) => {
      const p = this.context.decodeAudioData(bytes.buffer, resolve, reject);
      if (p?.then) p.then(resolve, reject);
    });
    const channels = Math.min(2, buffer.numberOfChannels), frames = buffer.length;
    const pcm = new Float32Array(frames * channels);
    if (buffer.numberOfChannels > 2) {
      // Down-mix surround to stereo (even channels left, odd right).
      const l = new Float32Array(frames), r = new Float32Array(frames);
      for (let c = 0; c < buffer.numberOfChannels; c++) {
        const d = buffer.getChannelData(c), t = c % 2 ? r : l;
        for (let i = 0; i < frames; i++) t[i] += d[i] / Math.ceil(buffer.numberOfChannels / 2);
      }
      for (let i = 0; i < frames; i++) { pcm[i * 2] = l[i]; pcm[i * 2 + 1] = r[i]; }
    } else {
      for (let c = 0; c < channels; c++) {
        const d = buffer.getChannelData(c);
        for (let i = 0; i < frames; i++) pcm[i * channels + c] = d[i];
      }
    }
    return {pcm, frames, channels, rate: buffer.sampleRate, duration: buffer.duration};
  }
}
