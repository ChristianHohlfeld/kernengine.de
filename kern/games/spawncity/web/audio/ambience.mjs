// SpawnCity on Kern - the town's quiet ambience (Kern's addition to the original's sound): a low
// street hum, birdsong by day, a dog barking somewhere now and then, crickets and the odd owl at
// night, and the wind up high - all far back, a bed under the game, never in front of it. Every
// voice is synthesized (WebAudio), panned somewhere around the listener, and follows the AMBIENT
// volume (Settings: Ton) and the master switch. In a menu, a car or the jet the street sounds
// fade out (the wind stays for the flight).
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
import {getSharedCombatAudioContext} from "./combat_audio_bank.mjs";
import {loadAudioSettings} from "./audio_settings.mjs";

const rnd = (a, b) => a + Math.random() * (b - a);

export function createAmbience() {
  let ctx = null, out = null, hum = null, wind = null, cricket = null, noise = null;
  let nextBird = 0, nextDog = 0, nextOwl = 0, lastSettings = 0, vol = 0.6, on = true;

  const ensure = () => {
    if (ctx) return true;
    ctx = getSharedCombatAudioContext({resume: false});
    if (!ctx) return false;
    out = ctx.createGain(); out.gain.value = 0; out.connect(ctx.destination);
    // brown noise (a soft rumble) as the source of the hum and the wind
    noise = ctx.createBuffer(1, ctx.sampleRate * 4, ctx.sampleRate);
    const d = noise.getChannelData(0);
    let last = 0;
    for (let i = 0; i < d.length; i++) { last = (last + 0.02 * (Math.random() * 2 - 1)) / 1.02; d[i] = last * 3.5; }
    const loop = (freq, q, type) => {
      const src = ctx.createBufferSource(); src.buffer = noise; src.loop = true;
      const f = ctx.createBiquadFilter(); f.type = type; f.frequency.value = freq; f.Q.value = q;
      const g = ctx.createGain(); g.gain.value = 0;
      src.connect(f).connect(g).connect(out); src.start();
      return {f, g};
    };
    hum = loop(180, 0.4, "lowpass");      // the town far off: traffic, air
    wind = loop(520, 0.7, "bandpass");    // up high
    // crickets: a 4.6 kHz tone, pulsed in quick trains
    const osc = ctx.createOscillator(); osc.frequency.value = 4600;
    const am = ctx.createGain(); am.gain.value = 0;
    const lfo = ctx.createOscillator(); lfo.type = "square"; lfo.frequency.value = 28;
    const lfoGain = ctx.createGain(); lfoGain.gain.value = 0.5;
    lfo.connect(lfoGain).connect(am.gain);
    const cg = ctx.createGain(); cg.gain.value = 0;
    osc.connect(am).connect(cg).connect(out); osc.start(); lfo.start();
    cricket = {g: cg};
    return true;
  };
  const panned = gain => {
    const p = ctx.createStereoPanner ? ctx.createStereoPanner() : null;
    const g = ctx.createGain(); g.gain.value = gain;
    if (p) { p.pan.value = rnd(-0.9, 0.9); g.connect(p).connect(out); } else g.connect(out);
    return g;
  };
  // a songbird: 3-6 quick notes, a falling or rising sweep each
  const bird = t => {
    const g = panned(0.05), n = Math.floor(rnd(3, 7)), base = rnd(2600, 4200);
    for (let i = 0; i < n; i++) {
      const o = ctx.createOscillator(), e = ctx.createGain(), t0 = t + i * rnd(0.09, 0.16);
      const f0 = base * rnd(0.85, 1.2);
      o.frequency.setValueAtTime(f0, t0); o.frequency.exponentialRampToValueAtTime(f0 * rnd(0.7, 1.35), t0 + 0.07);
      e.gain.setValueAtTime(0, t0); e.gain.linearRampToValueAtTime(1, t0 + 0.01); e.gain.exponentialRampToValueAtTime(0.001, t0 + 0.08);
      o.connect(e).connect(g); o.start(t0); o.stop(t0 + 0.1);
    }
  };
  // a dog a few gardens away: two or three barks, a low formant burst each
  const dog = t => {
    const g = panned(0.035), n = Math.floor(rnd(2, 4)), pitch = rnd(0.8, 1.25);
    for (let i = 0; i < n; i++) {
      const t0 = t + i * rnd(0.28, 0.42);
      const o = ctx.createOscillator(); o.type = "sawtooth";
      o.frequency.setValueAtTime(420 * pitch, t0); o.frequency.exponentialRampToValueAtTime(230 * pitch, t0 + 0.14);
      const f = ctx.createBiquadFilter(); f.type = "bandpass"; f.frequency.value = 900 * pitch; f.Q.value = 2.5;
      const lp = ctx.createBiquadFilter(); lp.type = "lowpass"; lp.frequency.value = 1400;  // distance
      const e = ctx.createGain();
      e.gain.setValueAtTime(0, t0); e.gain.linearRampToValueAtTime(1, t0 + 0.015); e.gain.exponentialRampToValueAtTime(0.001, t0 + 0.17);
      o.connect(f).connect(lp).connect(e).connect(g); o.start(t0); o.stop(t0 + 0.2);
    }
  };
  // a tawny owl: "hoo ... hu-hoooo"
  const owl = t => {
    const g = panned(0.04);
    for (const [dt, len, f0] of [[0, 0.32, 520], [0.9, 0.14, 500], [1.12, 0.6, 540]]) {
      const o = ctx.createOscillator(), e = ctx.createGain(), t0 = t + dt;
      o.frequency.setValueAtTime(f0, t0); o.frequency.linearRampToValueAtTime(f0 * 0.92, t0 + len);
      e.gain.setValueAtTime(0, t0); e.gain.linearRampToValueAtTime(1, t0 + 0.06); e.gain.linearRampToValueAtTime(0, t0 + len);
      o.connect(e).connect(g); o.start(t0); o.stop(t0 + len + 0.05);
    }
  };

  return {
    step(h) {
      if (!h) return;
      const now = performance.now();
      if (now - lastSettings > 1000) {
        lastSettings = now;
        const s = loadAudioSettings();
        on = s.soundEnabled !== false;
        vol = Math.max(0, Math.min(1, Number(s.ambientVolume ?? 60) / 100));
      }
      if (!ensure() || ctx.state !== "running") return;
      const t = ctx.currentTime;
      const paused = !!h.ui?.paused, mode = h.mode;
      const ground = (mode === "foot" || mode === "drone") && !paused;
      const night = Math.max(0, Math.min(1, Number(h.night) || 0));
      const agl = Math.max(0, Number(h.agl) || 0);
      const high = mode === "jet" || mode === "drone" ? Math.max(0, Math.min(1, (agl - 25) / 120)) : 0;
      const low = ground ? 1 - Math.max(0, Math.min(1, (agl - 15) / 60)) : 0;
      const master = on && !paused ? vol : 0;
      out.gain.setTargetAtTime(master, t, 0.4);
      hum.g.gain.setTargetAtTime(0.05 * low * (1 - 0.45 * night), t, 0.8);
      wind.g.gain.setTargetAtTime(0.07 * high, t, 0.6);
      wind.f.frequency.setTargetAtTime(420 + 500 * high + 120 * Math.sin(now / 2300), t, 0.5);
      cricket.g.gain.setTargetAtTime(0.006 * low * night * (0.6 + 0.4 * Math.sin(now / 1700)), t, 0.5);
      if (!(master > 0) || low < 0.3) return;
      if (now > nextBird) { nextBird = now + rnd(3500, 11000) * (1 + 3 * night); if (night < 0.6) bird(t + 0.05); }
      if (now > nextDog) { nextDog = now + rnd(35000, 80000); dog(t + 0.05); }
      if (now > nextOwl) { nextOwl = now + rnd(30000, 70000); if (night > 0.6) owl(t + 0.05); }
    },
  };
}
