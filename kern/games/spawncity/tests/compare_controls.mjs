// Controls measured with REAL inputs in both games (the original and SpawnCity on Kern), per device:
// desktop keyboard + mouse (pointer lock, movementX), an Xbox pad (navigator.getGamepads stub) and a
// phone (CDP touch events). Every input is followed by a reading of what it did - distance, rotation,
// height, speed, mode - and a screenshot. Writes controls.json and compare-controls.md
// (value original | Kern | deviation). Called by compare.mjs (COMPARE_PART=controls|all).
import fs from "node:fs";
import path from "node:path";

const pause = ms => new Promise(r => setTimeout(r, ms));
const wrap = a => { a = ((a + 180) % 360 + 360) % 360 - 180; return a; };

// What is steered, the camera and the state, the same quantities in both games:
// x/y/z metres (z up), yaw/pitch degrees counter-clockwise from +x, kmh, game time t (s).
const PROBE = {
  original: () => {
    const W = globalThis.__arondightWalkMode, V = globalThis.__arondightPlayerVehicleRuntime, C = globalThis.__arondightVehicleDrive, R = globalThis.__arondightRealWorld;
    const car = !!C?.active, mode = car ? "car" : document.body.classList.contains("jet-mode") ? "jet" : W?.mode || "drone";
    let x = null, y = null, z = null, yaw = null, pitch = null;
    if (car && C.pose?.position) [x, y, z] = C.pose.position;
    else if (mode === "foot" && W?.position) ({x, y, z} = W.position);
    else { const p = V?.dronePose; if (p) ({x, y, z} = p); }
    const cam = R?.presentedCamera?.() || R?.threeCamera;
    if (cam?.quaternion) {
      const d = cam.position.clone().set(0, 0, -1).applyQuaternion(cam.quaternion), up = cam.up;
      if (Math.abs(up?.z ?? 1) >= Math.abs(up?.y ?? 0)) { yaw = Math.atan2(d.y, d.x) * 180 / Math.PI; pitch = Math.asin(Math.max(-1, Math.min(1, d.z))) * 180 / Math.PI; }
      else { yaw = Math.atan2(-d.z, d.x) * 180 / Math.PI; pitch = Math.asin(Math.max(-1, Math.min(1, d.y))) * 180 / Math.PI; }
    }
    const footYaw = W?.yaw != null ? W.yaw * 180 / Math.PI : null;
    const jump = globalThis.__arondightWalkJump;
    const wpn = document.getElementById("mobileGameplayWeapon")?.textContent?.trim() || "";
    return {mode, x, y, z, yaw, pitch, footYaw, jumpZ: jump?.z ?? null, kmh: car ? Math.abs(C.speedMps) * 3.6 : null, carHeading: car ? C.heading * 180 / Math.PI : null,
      t: performance.now() / 1000, locked: !!document.pointerLockElement, weapon: wpn, body: document.body.className.slice(0, 300)};
  },
  kern: () => {
    const h = window.SpawnCity?.hud?.() || {}, v = h.ctl?.view || {}, p = v.probe;
    const e = v.eye || [null, null, null];
    return {mode: h.mode, x: p ? p.x : e[0], y: p ? p.y : e[1], z: p ? p.z : e[2], ground: p?.ground ?? null, yaw: p?.yaw ?? null, pitch: p?.pitch ?? null,
      kmh: h.mode === "car" ? (v.carKmh ?? (h.speed ?? 0) * 3.6) : null, agl: h.agl ?? null, fps: h.fps ?? null, t: p?.time ?? performance.now() / 1000, wall: performance.now() / 1000,
      locked: !!document.pointerLockElement, weapon: document.getElementById("mobileGameplayWeapon")?.textContent?.trim() || h.weapon || "", body: document.body.className.slice(0, 300)};
  },
};

// Mode changes and helpers that are not the input under test (getting to the place where it is measured).
const SETUP = {
  original: {
    mode: (page, m) => page.evaluate(m => {
      if (m === "car") { globalThis.__arondightWalkMode?.setMode?.("foot", {persist: false, reason: "compare"}); return globalThis.__arondightVehicleDrive?.enterNearest?.() ?? "no car api"; }
      if (globalThis.__arondightVehicleDrive?.active) globalThis.__arondightVehicleDrive.exit?.();
      return globalThis.__arondightWalkMode?.setMode?.(m, {persist: false, reason: "compare"}) ?? "no walk mode";
    }, m),
    wanted: page => page.evaluate(() => { const a = globalThis.__arondightWantedSystem; a?.clear?.("compare"); a?.reportCrime?.({id: "compare-" + Date.now(), kind: "car", severity: 5}); return !!a; }),
  },
  kern: {
    mode: (page, m) => page.evaluate(m => window.SpawnCity?.command?.(`mode ${m}`), m),
    wanted: page => page.evaluate(() => window.SpawnCity?.command?.("wanted 2")),
  },
};

const PAD_STUB = () => {
  const pad = globalThis.__cmpPad = {axes: [0, 0, 0, 0], buttons: Array(17).fill(0), on: false};
  const real = navigator.getGamepads?.bind(navigator);
  navigator.getGamepads = () => pad.on ? [{id: "Xbox Wireless Controller (STANDARD GAMEPAD Vendor: 045e Product: 0b13)", index: 0, connected: true, mapping: "standard",
    timestamp: performance.now(), axes: pad.axes.slice(), buttons: pad.buttons.map(v => ({pressed: v > 0.5, touched: v > 0, value: v})),
    vibrationActuator: {type: "dual-rumble", playEffect: () => Promise.resolve("complete"), reset: () => Promise.resolve("complete")}}, null, null, null] : (real ? real() : []);
};

export async function measureControls({browser, out, games, devices, gps}) {
  const rows = [], runs = [];
  const profiles = {desktop: devices.desktop, pad: devices.desktop, phone: devices.phone};
  for (const [dev, profile] of Object.entries(profiles)) {
    for (const [name, g] of Object.entries(games)) {
      const run = {device: dev, game: name, results: {}, errors: []};
      runs.push(run);
      const ctx = await browser.newContext({...profile, geolocation: gps, permissions: ["geolocation"], locale: "de-DE"});
      if (dev === "pad") await ctx.addInitScript(PAD_STUB);
      const page = await ctx.newPage();
      page.on("pageerror", e => run.errors.push(String(e.message || e).slice(0, 300)));
      const probe = () => page.evaluate(PROBE[name]).catch(e => ({error: String(e)}));
      const S = SETUP[name];
      let shotN = 0;
      const shot = async label => { const f = `ctl-${dev}-${String(shotN++).padStart(2, "0")}-${label}-${name}.png`; await page.screenshot({path: path.join(out, f)}).catch(() => {}); return f; };
      // one measurement: act() while sampling the probe every 60 ms, then read(start, samples, end)
      const measure = async (label, act, read, settle = 600) => {
        try {
          const a = await probe(), samples = [], wall0 = performance.now() / 1000;
          let on = true;
          const sampler = (async () => { while (on) { samples.push(await probe()); await pause(60); } })();
          await act();
          const c = await probe();
          await pause(settle);
          on = false; await sampler;
          const b = await probe();
          const v = read(a, samples, b, c);
          // per second of the game's own clock while the input was held (slow frames on the CI runner
          // stretch the wall clock; the original runs on the wall clock, Kern on its sim time)
          const held = c.t - a.t;
          if (held > 0.05) {
            if (typeof v.m === "number") v.mps = +(Math.hypot(c.x - a.x, c.y - a.y) / held).toFixed(2);
            if (typeof v.deg === "number") v.degps = +(v.deg / held).toFixed(1);
            if (typeof v.dz === "number") v.dzps = +((c.z - a.z) / held).toFixed(2);
          }
          run.results[label] = {...v, shot: await shot(label), from: a.mode, to: b.mode, held: +held.toFixed(2), wall: +(performance.now() / 1000 - wall0).toFixed(2), fps: b.fps ?? undefined};
        } catch (e) { run.errors.push(`${label}: ${String(e.message || e).slice(0, 300)}`); run.results[label] = {error: String(e.message || e).slice(0, 200)}; }
        console.log("controls", dev, name, label, JSON.stringify(run.results[label]).slice(0, 220));
      };
      const dist = (a, b) => +Math.hypot(b.x - a.x, b.y - a.y).toFixed(2);
      const dyaw = (a, b) => +wrap(b.yaw - a.yaw).toFixed(1);
      const dpitch = (a, b) => +(b.pitch - a.pitch).toFixed(1);
      const peak = (a, s) => +Math.max(0, ...s.map(x => (x.jumpZ ?? x.z) - (a.jumpZ ?? a.z))).toFixed(2);
      const hold = async (keys, ms) => { for (const k of keys) await page.keyboard.down(k); await pause(ms); for (const k of [...keys].reverse()) await page.keyboard.up(k); };
      const centre = async () => { const vp = page.viewportSize(); return {x: vp.width / 2, y: vp.height / 2}; };
      const mouseMove = async (dx, dy, steps = 10) => {
        const c = await centre();
        await page.mouse.move(c.x, c.y);
        for (let i = 1; i <= steps; i++) { await page.mouse.move(c.x + dx * i / steps, c.y + dy * i / steps); await pause(16); }
      };
      const cdp = dev === "phone" ? await ctx.newCDPSession(page) : null;
      const touch = async (type, pts) => cdp.send("Input.dispatchTouchEvent", {type, touchPoints: pts.map((p, i) => ({x: p.x, y: p.y, id: p.id ?? i, radiusX: 8, radiusY: 8, force: 1}))});
      const rectOf = sel => page.evaluate(sel => { for (const s of sel.split(",")) { const e = document.querySelector(s); if (!e) continue; const r = e.getBoundingClientRect(), st = getComputedStyle(e); if (r.width > 2 && r.height > 2 && st.display !== "none" && st.visibility !== "hidden" && !e.hidden) return {sel: s, x: r.left + r.width / 2, y: r.top + r.height / 2, w: r.width, h: r.height}; } return null; }, sel);
      const drag = async (sel, dx, dy, ms, holdMs = 0) => {
        const r = await rectOf(sel);
        if (!r) throw new Error(`no ${sel}`);
        await touch("touchStart", [{x: r.x, y: r.y, id: 7}]);
        const n = Math.max(2, Math.round(ms / 33));
        for (let i = 1; i <= n; i++) { await touch("touchMove", [{x: r.x + dx * i / n, y: r.y + dy * i / n, id: 7}]); await pause(ms / n); }
        await pause(holdMs);
        await touch("touchEnd", []);
        return r.sel;
      };
      const tap = async sel => { const r = await rectOf(sel); if (!r) return null; await touch("touchStart", [{x: r.x, y: r.y, id: 9}]); await pause(90); await touch("touchEnd", []); return r.sel; };
      const pad = (o) => page.evaluate(o => { const p = globalThis.__cmpPad; p.on = true; if (o.axes) p.axes = o.axes; if (o.buttons) { p.buttons = Array(17).fill(0); for (const [i, v] of Object.entries(o.buttons)) p.buttons[i] = v; } }, o);
      const padIdle = () => pad({axes: [0, 0, 0, 0], buttons: {}});
      const toMode = async (m, wait = 2500) => { await S.mode(page, m).catch(e => run.errors.push(`mode ${m}: ${e.message}`)); await pause(wait); };

      try {
        const url = new URL(g.url); url.searchParams.set("menu", "1");
        await page.goto(url.href, {waitUntil: "domcontentloaded", timeout: 90000});
        await g.boot(page);
        await g.start(page);
        await pause(4000);
        if (dev === "pad") { await padIdle(); await page.evaluate(() => window.dispatchEvent(Object.assign(new Event("gamepadconnected"), {gamepad: navigator.getGamepads()[0]}))); await pause(500); }
        // ---- on foot
        await toMode("foot", 3500);
        run.results.start = {...await probe(), shot: await shot("foot")};
        if (dev === "desktop") {
          await page.keyboard.press("Shift"); await pause(300);
          const c = await centre();
          await page.mouse.click(c.x, c.y); await pause(800);   // the first click takes the pointer lock (desktop_controls.mjs)
          run.results.lock = {locked: (await probe()).locked};
          await measure("W 1 s", () => hold(["KeyW"], 1000), (a, s, b) => ({m: dist(a, b)}));
          await measure("S 1 s", () => hold(["KeyS"], 1000), (a, s, b) => ({m: dist(a, b)}));
          await measure("D 1 s", () => hold(["KeyD"], 1000), (a, s, b) => ({m: dist(a, b)}));
          await measure("Shift+W 1 s", () => hold(["ShiftLeft", "KeyW"], 1000), (a, s, b) => ({m: dist(a, b)}));
          await measure("Space (Sprung)", () => page.keyboard.press("Space"), (a, s) => ({m: peak(a, s)}), 1400);
          await measure("Maus 200 px rechts", () => mouseMove(200, 0), (a, s, b) => ({deg: dyaw(a, b), pitch: dpitch(a, b)}));
          await measure("Maus 100 px runter", () => mouseMove(0, 100), (a, s, b) => ({deg: dpitch(a, b), yaw: dyaw(a, b)}));
          await measure("Q (Waffe)", () => page.keyboard.press("KeyQ"), (a, s, b) => ({weapon: `${a.weapon} -> ${b.weapon}`}));
          await measure("RMB (zielen)", async () => { await page.mouse.down({button: "right"}); await pause(600); }, (a, s, b) => ({body: b.body.match(/ads-active|scope-active/g)?.join(" ") || ""}), 100);
          await page.mouse.up({button: "right"}).catch(() => {});
          // drone
          await toMode("drone", 3500);
          await measure("Drohne Space 1 s", () => hold(["Space"], 1000), (a, s, b) => ({m: +(b.z - a.z).toFixed(2)}), 300);
          await measure("Drohne W 1 s", () => hold(["KeyW"], 1000), (a, s, b) => ({m: dist(a, b), dz: +(b.z - a.z).toFixed(2)}), 300);
          await measure("Drohne Maus 200 px", () => mouseMove(200, 0), (a, s, b) => ({deg: dyaw(a, b)}));
          await measure("Drohne A 1 s", () => hold(["KeyA"], 1000), (a, s, b) => ({deg: dyaw(a, b), m: dist(a, b)}));
          // car: F near a car (the setup brings one close), then W
          await toMode("car", 3500);
          await measure("Auto W 2 s", () => hold(["KeyW"], 2000), (a, s, b) => ({kmh: +(Math.max(0, ...s.map(x => x.kmh ?? 0), b.kmh ?? 0)).toFixed(1), m: dist(a, b)}), 100);
          await measure("Auto W+A 1 s", () => hold(["KeyW", "KeyA"], 1000), (a, s, b) => ({deg: a.carHeading != null ? +wrap(b.carHeading - a.carHeading).toFixed(1) : dyaw(a, b)}), 100);
          await measure("Auto S 2 s (Bremse)", () => hold(["KeyS"], 2000), (a, s, b) => ({kmh: +(b.kmh ?? 0).toFixed(1)}), 100);
          await measure("Auto F (aussteigen)", () => page.keyboard.press("KeyF"), (a, s, b) => ({mode: `${a.mode} -> ${b.mode}`}), 1500);
          await measure("F (einsteigen)", () => page.keyboard.press("KeyF"), (a, s, b) => ({mode: `${a.mode} -> ${b.mode}`}), 1500);
          await toMode("foot", 2000);
          await measure("Wanted", () => S.wanted(page), (a, s, b) => ({}), 1500);
          await measure("Esc (Pause)", () => page.keyboard.press("Escape"), (a, s, b) => ({locked: `${a.locked} -> ${b.locked}`}), 1000);
          await page.keyboard.press("Escape").catch(() => {});
        }
        if (dev === "pad") {
          await measure("Pad links hoch 1 s", async () => { await pad({axes: [0, -1, 0, 0]}); await pause(1000); await padIdle(); }, (a, s, b) => ({m: dist(a, b)}));
          await measure("Pad links 50 % 1 s", async () => { await pad({axes: [0, -0.5, 0, 0]}); await pause(1000); await padIdle(); }, (a, s, b) => ({m: dist(a, b)}));
          await measure("Pad rechts 50 % 1 s", async () => { await pad({axes: [0, 0, 0.5, 0]}); await pause(1000); await padIdle(); }, (a, s, b) => ({deg: dyaw(a, b)}));
          await measure("Pad rechts 100 % 1 s", async () => { await pad({axes: [0, 0, 1, 0]}); await pause(1000); await padIdle(); }, (a, s, b) => ({deg: dyaw(a, b)}));
          await measure("Pad rechts 20 % 1 s", async () => { await pad({axes: [0, 0, 0.2, 0]}); await pause(1000); await padIdle(); }, (a, s, b) => ({deg: dyaw(a, b)}));
          await measure("Pad rechts hoch 50 % 1 s", async () => { await pad({axes: [0, 0, 0, -0.5]}); await pause(1000); await padIdle(); }, (a, s, b) => ({deg: dpitch(a, b)}));
          await measure("Pad A (Sprung)", async () => { await pad({buttons: {0: 1}}); await pause(150); await padIdle(); }, (a, s) => ({m: peak(a, s)}), 1300);
          await measure("Pad L3+links (Sprint) 1 s", async () => { await pad({axes: [0, -1, 0, 0], buttons: {10: 1}}); await pause(1000); await padIdle(); }, (a, s, b) => ({m: dist(a, b)}));
          await measure("Pad Y (Waffe)", async () => { await pad({buttons: {3: 1}}); await pause(150); await padIdle(); }, (a, s, b) => ({weapon: `${a.weapon} -> ${b.weapon}`}), 600);
          await toMode("drone", 3500);
          await measure("Drohne Pad links hoch 1 s", async () => { await pad({axes: [0, -1, 0, 0]}); await pause(1000); await padIdle(); }, (a, s, b) => ({m: dist(a, b), dz: +(b.z - a.z).toFixed(2)}), 300);
          await measure("Drohne Pad rechts 50 % 1 s", async () => { await pad({axes: [0, 0, 0.5, 0]}); await pause(1000); await padIdle(); }, (a, s, b) => ({deg: dyaw(a, b), m: dist(a, b)}), 300);
          await measure("Drohne Pad RT 1 s", async () => { await pad({buttons: {7: 1}}); await pause(1000); await padIdle(); }, (a, s, b) => ({dz: +(b.z - a.z).toFixed(2)}), 300);
          await toMode("car", 3500);
          await measure("Auto Pad RT 2 s", async () => { await pad({buttons: {7: 1}}); await pause(2000); await padIdle(); }, (a, s, b) => ({kmh: +(Math.max(0, ...s.map(x => x.kmh ?? 0), b.kmh ?? 0)).toFixed(1)}), 100);
          await measure("Auto Pad links 50 % 1 s", async () => { await pad({axes: [-0.5, 0, 0, 0], buttons: {7: 1}}); await pause(1000); await padIdle(); }, (a, s, b) => ({deg: a.carHeading != null ? +wrap(b.carHeading - a.carHeading).toFixed(1) : dyaw(a, b)}), 100);
          await measure("Auto Pad X (aussteigen)", async () => { await pad({buttons: {2: 1}}); await pause(150); await padIdle(); }, (a, s, b) => ({mode: `${a.mode} -> ${b.mode}`}), 1500);
        }
        if (dev === "phone") {
          await measure("Touch Blick 120 px", () => drag("#footLook", 120, 0, 500), (a, s, b) => ({deg: dyaw(a, b), pitch: dpitch(a, b)}));
          await measure("Touch Blick 60 px hoch", () => drag("#footLook", 0, -60, 400), (a, s, b) => ({deg: dpitch(a, b)}));
          await measure("Touch Stick hoch 1 s", () => drag("#footMove", 0, -45, 150, 1000), (a, s, b) => ({m: dist(a, b)}));
          await measure("Touch Stick halb 1 s", () => drag("#footMove", 0, -22, 150, 1000), (a, s, b) => ({m: dist(a, b)}));
          // taps on every button of the foot view: what changes
          for (const sel of ["#footJump", "#footWeaponToggle,#thumbWeapon", "#mobileGameplayWeapon", "#handGrenadeBtn", "#scopeButton", "#airStrikeButton", "#footFire", "#enterCarButton", "#enterJetButton", "#mobileGameplayMode"]) {
            await measure(`Tippen ${sel.split(",")[0]}`, () => tap(sel), (a, s, b) => ({m: peak(a, s), mode: `${a.mode} -> ${b.mode}`, weapon: a.weapon !== b.weapon ? `${a.weapon} -> ${b.weapon}` : "", body: b.body.match(/scope-active|ads-active|air-strike-targeting/g)?.join(" ") || ""}), 900);
            await page.evaluate(() => document.querySelector('#airStrikeTargeting [data-a="cancel"], #airStrikeTargeting button')?.click()).catch(() => {});
            if ((await probe()).mode !== "foot") await toMode("foot", 1500);
          }
          await toMode("drone", 3500);
          await measure("Drohne Touch links hoch 1 s", () => drag("#soloLeft", 0, -45, 150, 1000), (a, s, b) => ({dz: +(b.z - a.z).toFixed(2), m: dist(a, b)}), 300);
          await measure("Drohne Touch rechts 1 s", () => drag("#soloRight", 45, 0, 150, 1000), (a, s, b) => ({deg: dyaw(a, b), m: dist(a, b)}), 300);
          await toMode("car", 3500);
          await measure("Auto Touch Gas 2 s", () => drag("#vehicleMove,#footMove", 0, -45, 150, 2000), (a, s, b) => ({kmh: +(Math.max(0, ...s.map(x => x.kmh ?? 0), b.kmh ?? 0)).toFixed(1)}), 100);
          await measure("Auto Tippen Kamera", () => tap("#vehicleCamButton"), (a, s, b) => ({}), 800);
          await measure("Auto Tippen Aussteigen", () => tap("#driveModeButton,#enterCarButton"), (a, s, b) => ({mode: `${a.mode} -> ${b.mode}`}), 1500);
        }
      } catch (e) { run.errors.push(String(e.message || e).slice(0, 300)); }
      await ctx.close();
    }
  }
  // ---- table: value original | Kern | deviation
  const fmt = r => { if (!r) return "–"; if (r.error) return `Fehler: ${r.error.slice(0, 60)}`; const v = Object.entries(r).filter(([k]) => !["shot", "from", "to", "wall"].includes(k)).map(([k, x]) => `${k} ${x}`).join(", "); return v || `${r.from} → ${r.to}`; };
  const num = r => r && !r.error ? (r.mps ?? r.degps ?? r.dzps ?? r.m ?? r.deg ?? r.kmh ?? r.dz) : undefined;
  const md = ["# Steuerung: Original und Kern mit echten Eingaben", "", "Gemessen von tests/compare_controls.mjs (Workflow „SpawnCity Vergleich“). Werte: m = Meter, deg = Grad (Gierwinkel, gegen den Uhrzeigersinn positiv), kmh, dz = Höhenänderung; mps, degps, dzps = je Sekunde der Spielzeit, solange die Eingabe gehalten wurde (held s); die Abweichung vergleicht diese Raten.", ""];
  for (const dev of Object.keys(profiles)) {
    const o = runs.find(r => r.device === dev && r.game === "original"), k = runs.find(r => r.device === dev && r.game === "kern");
    if (!o || !k) continue;
    md.push(`## ${dev}`, "", "| Eingabe | Original | Kern | Abweichung |", "|---|---|---|---|");
    const labels = [...new Set([...Object.keys(o.results), ...Object.keys(k.results)])].filter(l => l !== "start");
    for (const l of labels) {
      const a = o.results[l], b = k.results[l], x = num(a), y = num(b);
      const dev2 = typeof x === "number" && typeof y === "number" ? (Math.abs(x) < 0.05 && Math.abs(y) < 0.05 ? "0" : Math.abs(x) < 0.05 ? `${y}` : `${(((y - x) / Math.abs(x)) * 100).toFixed(0)} %`) : "";
      md.push(`| ${l} | ${fmt(a)} | ${fmt(b)} | ${dev2} |`);
      rows.push({device: dev, input: l, original: a, kern: b, deviation: dev2});
    }
    md.push("", `Fehler Original: ${o.errors.join(" · ") || "keine"}`, "", `Fehler Kern: ${k.errors.join(" · ") || "keine"}`, "");
  }
  fs.writeFileSync(path.join(out, "controls.json"), JSON.stringify({runs, rows}, null, 1));
  fs.writeFileSync(path.join(out, "compare-controls.md"), md.join("\n") + "\n");
  return {runs, rows};
}
