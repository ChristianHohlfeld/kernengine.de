// SpawnCity on Kern - the original's view layer, part 2: the DOM writes of the original's mode and
// combat HUD modules (car, jet, wanted, sniper, zombies, contract, multiplayer, VS, GUIDED, speed
// lines, hit / damage feedback, jetpack, air strike, hand grenade, foot reticle). Every element names
// the spawncity.de module whose writes it repeats (texts, classes, data attributes, inline styles),
// fed from the game's JSON (sc_hud: "ctl", "ctl.view", "combat", "guided", "mp", ...). The game
// decides; this only shows it. Registered with view.mjs (registerViewBinder).
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
import {registerViewBinder} from "./view.mjs";

function installViewModes() {
  const $ = id => document.getElementById(id);
  const q = sel => document.querySelector(sel);
  const text = (el, t) => { if (el && el.textContent !== t) el.textContent = t; };
  const hide = (el, h) => { if (el && el.hidden !== h) el.hidden = h; };
  const data = (el, k, v) => { if (el && el.dataset[k] !== v) el.dataset[k] = v; };
  const attr = (el, k, v) => { if (el && el.getAttribute(k) !== v) el.setAttribute(k, v); };
  const body = () => document.body;
  // gameplay_fun_runtime.mjs formatPoints
  const fmtPts = v => { const n = Math.max(0, Math.round(Number(v) || 0)); return n >= 100000 ? `${(n / 1000).toFixed(n >= 1000000 ? 0 : 1)}k` : n.toLocaleString("en-US"); };
  // re-run a CSS animation (void el.offsetWidth between remove and add, as the originals do)
  const pulse = (el, cls) => { if (!el) return; el.classList.remove(cls); void el.offsetWidth; el.classList.add(cls); };
  const S = {
    claimed: null, radioMounted: false, bannerKey: "", bannerTimer: 0, zombie: {night: false, round: 0, alive: 0}, zBannerTimer: 0,
    toastSeen: 0, toastQueue: [], toastTimer: 0, dmg: -1, hits: -1, impactSeen: 0, impactCursor: 0, localDead: false, peerDead: false,
    speed: 0, last: performance.now(), lastEye: null, streaks: [], city: null, cityBusy: false, cityAt: -Infinity, cityFrom: null,
    cityShot: null, cityCueAt: -Infinity, jetHudAt: -Infinity, strikeOverlay: null, feedKey: "", boardKey: "", mpHitClass: "",
  };

  // ---------------------------------------------------------------------------------------------
  // shared_sticks.mjs claimSticks / releaseSticks: the car and the jet take over the foot sticks
  function sticks(h) {
    const owner = h.mode === "car" ? "car" : h.mode === "jet" || h.mode === "parachute" ? "jet" : null;
    const label = (id, t) => {
      const s = $(id)?.querySelector("span");
      if (!s) return;
      if (s.dataset.base === undefined) s.dataset.base = s.textContent;
      text(s, t ?? s.dataset.base);
    };
    if (owner !== S.claimed) {
      S.claimed = owner;
      body().classList.toggle("sticks-claimed", !!owner);
      if (owner) body().dataset.sticksOwner = owner; else delete body().dataset.sticksOwner;
      const L = owner === "car" ? {move: "LENKEN / GAS", look: "UMSCHAUEN"} : owner === "jet" ? {move: "STEIGEN / SCHUB", look: "KNÜPPEL", fire: "KANONE"} : {};
      label("footMove", L.move);
      label("footLook", L.look);
      const f = $("footFire");
      if (f) { if (f.dataset.base === undefined) f.dataset.base = f.textContent; text(f, L.fire || f.dataset.base); }
    }
    // the knobs themselves: view.mjs sticks() (shared_sticks.mjs knob)
  }

  // ---------------------------------------------------------------------------------------------
  // player_car_mode.mjs renderUi / toggleCamera; car_radio.mjs ensureButton / showBanner
  function car(h, view) {
    const v = h.ctl?.view || {}, active = h.mode === "car";
    if (active) text($("vehicleReadout"), `CAR · ${Math.round(v.carKmh ?? h.speed * 3.6)} km/h · E EXIT`);
    text($("vehicleCamButton"), v.cockpit ? "🎥 AUSSEN" : "🎥 INNEN");
    const b = $("driveModeButton");
    if (b) {
      const near = !active && !!h.ctl?.nearCar;
      data(b, "near", near ? "1" : "0");
      b.disabled = !!h.ctl?.dead || (!active && !near);
      text(b, active ? "EXIT CAR" : near ? "CAR" : "CAR —");
      attr(b, "aria-label", active ? "Exit vehicle" : near ? "Enter nearby car" : "No drivable car nearby");
    }
    // the radio button (prepended to #vehicleButtons the first time a car is entered): tap = next station, hold 550 ms = off
    // (with the original's sound, audio.mjs, car_radio.mjs itself builds its button and banner)
    if (active && !S.radioMounted && !globalThis.__arondightCarRadio) {
      const row = $("vehicleButtons");
      if (row && !$("carRadioButton")) {
        const r = document.createElement("button");
        r.id = "carRadioButton"; r.type = "button"; r.textContent = "📻";
        r.title = "Radio: tippen = nächster Sender, halten = aus";
        r.setAttribute("aria-label", "Car radio");
        let holdT = 0, held = false;
        r.addEventListener("pointerdown", e => { if (e.pointerType !== "mouse") return; e.preventDefault(); e.stopPropagation(); held = false; holdT = setTimeout(() => { held = true; view.command("radio toggle"); }, 550); });
        r.addEventListener("pointerup", e => { if (e.pointerType !== "mouse") return; e.preventDefault(); e.stopPropagation(); clearTimeout(holdT); if (!held) view.command("radio next"); });
        r.addEventListener("pointercancel", () => clearTimeout(holdT));
        row.prepend(r);
      }
      S.radioMounted = true;
    }
    // the station banner (car_radio.mjs showBanner): 2.6 s, fades .35 s
    const key = active && String(h.banner || "").startsWith("📻") ? `${h.banner}|${h.bannerSub || ""}|${h.radio?.index ?? ""}` : "";
    if (key && key !== S.bannerKey) {
      const vp = $("viewport");
      let bn = $("carRadioBanner");
      if (vp && !bn) {
        bn = document.createElement("div");
        bn.id = "carRadioBanner";
        bn.style.cssText = "position:absolute;left:50%;top:calc(max(10px,env(safe-area-inset-top)) + 54px);transform:translateX(-50%);z-index:31;padding:8px 16px 7px;border-radius:12px;background:#0d1117d9;border:1px solid #ffffff2a;color:#fff;text-align:center;pointer-events:none;transition:opacity .35s;opacity:0;max-width:min(80vw,420px)";
        vp.appendChild(bn);
      }
      if (bn) {
        bn.innerHTML = `<div style="font:900 15px/1.15 'Nunito',system-ui,sans-serif;letter-spacing:.05em;color:#ffd36a;white-space:nowrap;overflow:hidden;text-overflow:ellipsis"></div><div style="font:700 11px/1.3 system-ui,sans-serif;color:#b8c2cc;margin-top:2px"></div>`;
        bn.children[0].textContent = h.banner;
        bn.children[1].textContent = h.bannerSub || "";
        bn.style.opacity = "1";
        clearTimeout(S.bannerTimer);
        S.bannerTimer = setTimeout(() => { bn.style.opacity = "0"; }, 2600);
      }
    }
    S.bannerKey = key;
  }

  // ---------------------------------------------------------------------------------------------
  // jet_mode.mjs renderHud (every 80 ms), renderCityCue (240 ms), renderButtons; body.jet-chute
  function jet(h, view) {
    const hud = $("jetHud");
    const chute = h.mode === "parachute", active = h.mode === "jet" || chute;
    body().classList.toggle("jet-chute", chute);
    if (!hud || !active) return;
    const k = name => hud.querySelector(`[data-k=${name}]`);
    const now = performance.now(), j = h.ctl?.view?.jet;
    if (now - S.jetHudAt >= 80) {
      S.jetHudAt = now;
      if (now - S.cityCueAt >= 240) { S.cityCueAt = now; cityCue(h, hud, view); }
      if (chute) text(k("mode"), "FALLSCHIRM");
      else if (j) {
        text(k("spd"), String(Math.round(j.kmh)));
        const mb = k("mach");
        if (mb) { text(mb, `M ${j.mach.toFixed(2)}`); data(mb, "super", j.mach >= 1 ? "1" : "0"); }
        text(k("alt"), String(Math.round(j.agl)));
        text(k("g"), `${(j.nz || 1).toFixed(1)} G`);
        const aoa = k("aoa");
        if (aoa) { text(aoa, `α ${Math.round(j.alpha)}°`); data(aoa, "high", j.alpha > 19 ? "1" : "0"); }
        const ab = j.flight && j.lever > 0.86;
        text(k("mode"), j.landed ? "GELANDET" : j.flight ? (j.nozzleCos < 0.95 ? "ÜBERGANG" : ab ? "FLUG · AB" : "FLUG") : (j.kmh / 3.6 > 30 ? "BREMSEN" : "SCHWEBEN"));
        const hp = k("hp");
        if (hp) { text(hp, j.burning ? `FEUER ${j.hp}` : `HP ${j.hp}`); data(hp, "low", j.hp < 40 ? "1" : "0"); }
        const thr = hud.querySelector(".jet-thr i");
        if (thr) thr.style.height = `${Math.round((j.flight ? j.lever : j.thrust) * 100)}%`;
      }
    }
    // the jet's buttons (renderButtons)
    const rocket = hud.querySelector("[data-a=rocket]");
    if (rocket && j) {
      const left = j.rocketLeft, armed = !!h.ctl?.rocket;
      text(rocket, left > 0 ? `RELOAD ${(left / 1000).toFixed(1)}s` : armed ? "2 · FIRE" : "1 · ROCKET");
      data(rocket, "armed", armed ? "1" : "0");
      data(rocket, "ready", left > 0 ? "0" : "1");
      attr(rocket, "aria-label", armed ? "Rocket selected: tap gameplay to fire" : left > 0 ? "Rocket recharging" : "Select rocket; tap gameplay to fire");
    }
    const mb = hud.querySelector("[data-a=mode]");
    if (mb && j) { text(mb, j.flight ? "⇄ SCHWEBEN" : "⇄ FLUG"); data(mb, "flight", j.flight ? "1" : "0"); }
    const ex = hud.querySelector("[data-a=exit]");
    if (ex) { text(ex, !j || j.landed ? "AUSSTEIGEN" : "EJECT"); hide(ex, chute); }
  }
  // the flight path marker and the town ahead (Nominatim reverse lookup, as the original does)
  function cityCue(h, hud, view) {
    const vp = $("viewport"), svg = hud.querySelector(".jet-svg");
    if (!vp || !svg) return;
    const W = vp.clientWidth, H = vp.clientHeight;
    if (svg.dataset.vb !== `${W}x${H}`) { svg.dataset.vb = `${W}x${H}`; svg.setAttribute("viewBox", `0 0 ${W} ${H}`); }
    const line = svg.querySelector(".jet-city-line"), dot = svg.querySelector(".jet-city-dot"), fpm = svg.querySelector(".jet-fpm");
    const nameEl = hud.querySelector("[data-k=city]"), dEl = hud.querySelector("[data-k=cityd]"), j = h.ctl?.view?.jet;
    if (fpm) {
      if (j && h.mode === "jet" && j.fpm) { fpm.setAttribute("transform", `translate(${j.fx.toFixed(1)},${j.fy.toFixed(1)})`); fpm.style.display = ""; }
      else fpm.style.display = "none";
    }
    if (!Number.isFinite(h.lon) || !Number.isFinite(h.lat)) { line.style.display = dot.style.display = "none"; text(nameEl, ""); text(dEl, ""); return; }
    const now = performance.now(), pos = {x: h.lon, y: h.lat};
    const moved = S.cityFrom ? Math.hypot((pos.x - S.cityFrom.x) * 75000, (pos.y - S.cityFrom.y) * 111000) : Infinity;
    if (!S.cityBusy && (now - S.cityAt > 25000 || moved > 1500)) {
      S.cityFrom = pos; S.cityBusy = true; S.cityAt = now;
      fetch(`https://nominatim.openstreetmap.org/reverse?format=jsonv2&zoom=10&accept-language=de&lat=${h.lat.toFixed(5)}&lon=${h.lon.toFixed(5)}`, {signal: AbortSignal.timeout?.(8000)})
        .then(r => r.ok ? r.json() : null).then(jv => {
          const a = jv?.address || {}, name = jv?.name || a.city || a.town || a.village || a.municipality;
          if (name && Number.isFinite(+jv.lon) && Number.isFinite(+jv.lat)) S.city = {name: String(name).toUpperCase(), lon: +jv.lon, lat: +jv.lat};
        }).catch(() => {}).finally(() => { S.cityBusy = false; });
    }
    if (!S.city) { line.style.display = dot.style.display = "none"; return; }
    let t = null;
    try { t = JSON.parse(view.command(`project ${S.city.lon} ${S.city.lat}`)); } catch { /* no projection */ }
    if (!t) { line.style.display = dot.style.display = "none"; return; }
    text(nameEl, S.city.name);
    text(dEl, t.dist < 600 ? "ÜBER DEM ZENTRUM" : `${(t.dist / 1000).toFixed(t.dist >= 10000 ? 0 : 1).replace(".", ",")} KM`);
    if (t.dist < 600) { line.style.display = dot.style.display = "none"; return; }
    let x = t.x, y = t.y;
    const cx = W / 2, cy = H / 2;
    if (!t.front || x < 0 || x > W || y < 0 || y > H) {
      const dx = x - cx, dy = y - cy, kk = Math.min((W * 0.46) / Math.max(1, Math.abs(dx)), (H * 0.42) / Math.max(1, Math.abs(dy)));
      x = cx + dx * kk; y = cy + dy * kk;
    }
    const L = Math.hypot(x - cx, y - cy) || 1, sx = cx + (x - cx) / L * 26, sy = cy + (y - cy) / L * 26;
    line.setAttribute("x1", sx.toFixed(1)); line.setAttribute("y1", sy.toFixed(1)); line.setAttribute("x2", x.toFixed(1)); line.setAttribute("y2", y.toFixed(1));
    dot.setAttribute("cx", x.toFixed(1)); dot.setAttribute("cy", y.toFixed(1));
    line.style.display = L > 34 ? "" : "none"; dot.style.display = "";
  }

  // ---------------------------------------------------------------------------------------------
  // wanted_police_drones.mjs renderHud / renderEmpControl
  function wanted(h) {
    const w = h.ctl?.view?.wanted, hud = $("wantedHud"), emp = $("wantedEmpButton");
    if (!w) return;
    if (emp) {
      const show = w.stars > 0;
      hide(emp, !show);
      if (show) {
        emp.disabled = w.empMs > 0;
        const label = emp.querySelector("small");
        if (w.empMs > 0) { data(emp, "state", "cooldown"); text(label, `COOLDOWN ${(w.empMs / 1000).toFixed(1)} s`); }
        else if (w.inRange) { data(emp, "state", "ready"); text(label, `${w.inRange} IN RANGE · 30 m`); }
        else if (w.miss) { data(emp, "state", "miss"); text(label, "NO TARGET IN RANGE"); }
        else { data(emp, "state", "empty"); text(label, "0 IN RANGE · 30 m"); }
        attr(emp, "aria-label", w.empMs > 0 ? `Emergency EMP recharging. ${Math.ceil(w.empMs / 1000)} seconds remaining.` : w.inRange ? `Fire emergency EMP at ${w.inRange} police ${w.inRange === 1 ? "drone" : "drones"} in range.` : "Emergency EMP ready. Move within 30 meters of a police drone.");
      }
    }
    if (!hud) return;
    const showWanted = w.stars > 0, showBanner = !!w.banner;
    hide(hud, !showWanted && !showBanner);
    if (hud.hidden) return;
    hud.querySelectorAll(".wanted-stars i").forEach((n, i) => n.classList.toggle("hot", showWanted && i < w.stars));
    const title = hud.querySelector("strong"), detail = hud.querySelector("small");
    if (showBanner) {
      data(hud, "phase", "escaped");
      text(title, w.busted ? "BUSTED" : "ESCAPED");
      text(detail, w.busted ? "POLICE PURSUIT ENDED" : "WANTED LEVEL CLEARED");
      attr(hud, "aria-label", title.textContent);
    } else if (w.active === 0 && w.nextWaveMs > 0) {
      data(hud, "phase", "searching");
      text(title, "WAVE CLEARED");
      text(detail, `HIDE NOW · NEXT ${(w.nextWaveMs / 1000).toFixed(1)} s`);
      attr(hud, "aria-label", `Police wave cleared. Hide now. Next wave in ${Math.ceil(w.nextWaveMs / 1000)} seconds.`);
    } else if (w.searching) {
      data(hud, "phase", "searching");
      text(title, "SEARCHING");
      text(detail, `HIDE ${Math.max(0, w.escapeMs / 1000).toFixed(1)} s · ${w.active} DRONES`);
      attr(hud, "aria-label", `${w.stars} wanted stars. Police searching. Escape in ${Math.ceil(w.escapeMs / 1000)} seconds.`);
    } else {
      const inbound = w.inboundMs;
      data(hud, "phase", "pursuit");
      text(title, inbound > 0 ? "POLICE INBOUND" : "POLICE PURSUIT");
      text(detail, inbound > 0 ? `${w.active} DRONES · ${Math.ceil(inbound / 100) / 10} s WARNING` : `${w.active} DRONES · BREAK LINE OF SIGHT`);
      attr(hud, "aria-label", inbound > 0 ? `${w.stars} wanted stars. Police drones inbound in ${Math.ceil(inbound / 1000)} seconds.` : `${w.stars} wanted stars. ${w.active} police drones pursuing.`);
    }
  }

  // ---------------------------------------------------------------------------------------------
  // sniper_scope.mjs sync / readout; first_person_controller_v5.mjs body.ads-active / scope-active
  function sniper(h) {
    const scoped = !!h.combat?.scope, ads = (h.ads ?? (h.aiming ? 1 : 0)) > 0.6 && h.mode === "foot";  // adsT > .6
    body().classList.toggle("scope-active", scoped);
    body().classList.toggle("ads-active", ads);
    if (scoped) text(q("#sniperScope .ss-range"), `${Math.round(h.ctl?.zoom || 8)}×`);
    const b = $("scopeButton");
    if (b) {
      const touchPlay = h.ctl?.device !== "desktop" && !h.ctl?.pad && h.mode === "foot";
      // sniper_scope.mjs place(): straight above the thumb weapon switch (66 px up); shown only
      // where it could be placed
      const w = $("thumbWeapon");
      let placed = false;
      if (touchPlay && /^Sniper/.test(h.weapon || "") && !h.ctl?.dead && w?.classList.contains("show")) {
        const l = parseFloat(w.style.left), t = parseFloat(w.style.top);
        if (Number.isFinite(l) && Number.isFinite(t)) {
          const L = `${Math.round(l)}px`, T = `${Math.round(t - 66)}px`;
          if (b.style.left !== L) b.style.setProperty("left", L, "important");
          if (b.style.top !== T) b.style.setProperty("top", T, "important");
          placed = true;
        }
      }
      b.classList.toggle("show", placed);
      const html = "<b>◎</b>SCOPE";
      if (b.dataset.html !== html) { b.dataset.html = html; b.innerHTML = html; }
      b.classList.toggle("on", !!h.ctl?.scope);
    }
  }

  // ---------------------------------------------------------------------------------------------
  // zombie_nights.mjs renderHud / banner
  function zombies(h) {
    const z = h.combat?.zombies, hud = $("zombieHud");
    if (!z || !hud) return;
    const banner = (t, ms) => {
      const b = hud.querySelector(".zh-banner");
      if (!b) return;
      b.textContent = t;
      b.dataset.on = "1";
      clearTimeout(S.zBannerTimer);
      S.zBannerTimer = setTimeout(() => { b.dataset.on = "0"; }, ms);
    };
    const was = S.zombie;
    if (z.night && !was.night) banner("DIE NACHT BRICHT HEREIN", 3600);
    else if (z.night && z.round > was.round && was.round > 0) banner(`RUNDE ${z.round}`, 2600);
    else if (z.night && was.alive > 0 && z.alive === 0 && z.round === was.round) banner(`RUNDE ${z.round} ÜBERSTANDEN`, 2400);
    if (!z.night && was.night && was.round) banner(`MORGENGRAUEN — ${was.round} ${was.round === 1 ? "RUNDE" : "RUNDEN"} ÜBERLEBT`, 4200);
    S.zombie = {night: !!z.night, round: z.round, alive: z.alive};
    const show = !!z.night || z.points > 0;
    hide(hud, !show);
    text(hud.querySelector(".zh-round"), z.night ? "I".repeat(Math.min(5, z.round)) + (z.round > 5 ? ` ${z.round}` : "") : "");
    text(hud.querySelector(".zh-points"), `${z.points}`);
    const btn = $("zombieRepair");
    if (btn) data(btn, "show", "0");  // the boarded-up doors of the original are not in this town yet
  }

  // ---------------------------------------------------------------------------------------------
  // gameplay_fun_runtime.mjs render / drainToast / toastFor
  function contract(h) {
    const f = h.ctl?.view?.fun, hud = $("gameplayContractHud");
    if (!f || !hud) return;
    data(hud, "complete", f.complete ? "1" : "0");
    data(hud, "overdrive", f.overdrive ? "1" : "0");
    text($("gameplayContractName"), f.name);
    text($("gameplayContractProgress"), `${f.progress}/${f.target}`);
    text($("gameplayContractHint"), f.complete ? `COMPLETE · +${fmtPts(f.reward)}` : f.hint);
    const bar = q("#gameplayContractBar>i");
    if (bar) bar.style.width = `${Math.max(0, Math.min(1, f.progress / Math.max(1, f.target))) * 100}%`;
    text($("gameplayRiskHint"), f.risk > 0 && f.stars > 0 ? `ESCAPE TO BANK ${fmtPts(f.risk)}` : f.mercy > 0 ? `COMEBACK SHIELD · ${Math.round((1 - f.damageScale) * 100)}% LESS DAMAGE` : f.high > 0 ? `BEST ${fmtPts(f.high)}` : "BUILD MOMENTUM · SURVIVE · BANK");
    document.querySelectorAll("#gameplayMomentum>i").forEach((n, i) => n.classList.toggle("hot", f.overdrive || i < f.momentum));
    const pill = $("gameplayScorePill");
    if (pill) {
      text(pill, `S ${fmtPts(f.score)} · R ${fmtPts(f.risk)} · ×${f.mult.toFixed(2)}`);
      data(pill, "risk", f.risk > 0 ? "1" : "0");
      attr(pill, "aria-label", `Banked score ${f.score}. ${f.risk} points at risk. Multiplier ${f.mult.toFixed(2)}.`);
    }
    for (const t of f.toasts || []) {
      if (t.id <= S.toastSeen) continue;
      S.toastSeen = t.id;
      const item = toastFor(t);
      if (item) S.toastQueue.push(item);
    }
    if (S.toastQueue.length > 5) S.toastQueue = S.toastQueue.slice(-5);
    drainToast();
  }
  function toastFor(e) {
    if (e.type === "contractComplete") return {title: "CONTRACT COMPLETE", detail: `${e.name} · +${fmtPts(e.reward)}`, kind: "bank", sound: "reward", gain: .36};
    if (e.type === "bank") return {title: `BANKED +${fmtPts(e.points)}`, detail: `ESCAPED ${e.peakStars}★ · SCORE ${fmtPts(e.score)}`, kind: "bank", sound: "reward", gain: .34};
    if (e.type === "overdrive") return {title: "OVERDRIVE!", detail: `1.75× FOR ${(e.durationMs / 1000).toFixed(0)}s · +${fmtPts(e.bonus)}`, kind: "bank", sound: "reward", gain: .40};
    if (e.type === "award" && e.combo >= 2) return {title: `+${fmtPts(e.points)}`, detail: `${e.label} · ${e.combo} COMBO · ×${e.multiplier.toFixed(2)}`, kind: "award", sound: "reward", gain: .19};
    if (e.type === "comboBreak") return {title: "COMBO LOST", detail: `${e.combo} STREAK ENDED`, kind: "fail", sound: "fail", gain: .17};
    if (e.type === "setback") return {title: e.label || "SETBACK", detail: `-${fmtPts(e.lost)} · COMEBACK SHIELD ${e.mercy}/2`, kind: "fail", sound: "fail", gain: .25};
    if (e.type === "insurance") return {title: "SECOND CHANCE", detail: `SAVED ${fmtPts(e.insured)} · LOST ${fmtPts(e.lost)}`, kind: "fail", sound: "fail", gain: .29};
    return null;
  }
  function drainToast() {
    const toast = $("gameplayToast");
    if (S.toastTimer || !S.toastQueue.length || !toast) return;
    const item = S.toastQueue.shift();
    toast.classList.remove("active");
    void toast.offsetWidth;
    toast.dataset.kind = item.kind;
    toast.querySelector("strong").textContent = item.title;
    toast.querySelector("small").textContent = item.detail;
    toast.classList.add("active");
    globalThis.__scPlayReward?.(item.sound, item.gain);  // playReward (the original's sound, audio.mjs)
    S.toastTimer = setTimeout(() => { S.toastTimer = 0; toast.classList.remove("active"); drainToast(); }, 820);
  }

  // ---------------------------------------------------------------------------------------------
  // mp_fun_hud.mjs onKill / onHit / onHurt / onProtected / renderBoard
  const MP_WEAPON = {MP: "MP", GLOCK: "GLOCK", SNIPER: "SNIPER", GRANATE: "GRANATE", RAKETE: "RAKETE", "FÄUSTE": "FÄUSTE", MESSER: "MESSER", DROHNE: "DROHNE", EXPLOSION: "EXPLOSION"};
  function mpFun(h) {
    const mp = h.mp;
    const feed = $("mpKillFeed"), board = $("mpScore"), marker = $("mpHitMarker"), hurt = $("mpHurt"), shield = $("mpShield");
    if (!mp) return;
    const nm = (index, color, self) => `<b style="color:${color}">${self ? "DU" : `P${index}`}</b>`;
    if (feed) {
      const rows = (mp.feed || []).filter(k => k.age < 5650).slice(0, 5);
      const key = rows.map(k => `${k.by}-${k.victim}-${Math.round(k.age / 450)}`).join("|");
      if (key !== S.feedKey) {
        S.feedKey = key;
        feed.innerHTML = rows.map(k => {
          const w = MP_WEAPON[String(k.weapon || "").toUpperCase()] || (k.weapon ? String(k.weapon).toUpperCase().slice(0, 10) : "");
          const me = k.bySelf || k.victimSelf;
          const inner = k.accident ? `${nm(k.victim, k.victimColor, k.victimSelf)}<i>✕</i>${w || "UNFALL"}` : `${nm(k.by, k.byColor, k.bySelf)}<i>${w || "✕"}</i>${nm(k.victim, k.victimColor, k.victimSelf)}${k.head ? "<em>KOPF</em>" : ""}`;
          return `<div class="${me ? "me" : ""}"${k.age > 5200 ? ' style="opacity: 0;"' : ""}>${inner}</div>`;
        }).join("");
      }
    }
    if (board) {
      const on = !!mp.connected;
      if (!on) hide(board, true);
      else {
        const list = (mp.scores || []).slice().sort((a, b) => b.k - a.k || a.d - b.d);
        const html = list.map(p => `<span class="${p.self ? "me" : ""}"><span><u style="background:${p.color}"></u>${p.self ? "DU" : `P${p.index}`}</span><span>${p.k} / ${p.d}</span></span>`).join("");
        if (html !== S.boardKey) { S.boardKey = html; board.innerHTML = html; }
        hide(board, false);
      }
    }
    if (marker) {
      const c = mp.hitShow ? `show${mp.hitKill ? " kill" : ""}${mp.hitHead ? " head" : ""}` : mp.hitKill && mp.hit < 400 ? "kill" : "";
      if (marker.className !== c) marker.className = c;
    }
    if (hurt) {
      if (mp.hurt) hurt.style.setProperty("--a", `${(mp.hurtAngle || 0).toFixed(0)}deg`);
      const c = mp.hurt ? "show" : "";
      if (hurt.className !== c) hurt.className = c;
    }
    if (shield) { const c = mp.shield ? "show" : ""; if (shield.className !== c) shield.className = c; }
  }

  // ---------------------------------------------------------------------------------------------
  // vs_combat_presentation.mjs updateEnemyMarker / updateRespawnHud / flashExplosion
  function vs(h) {
    const mp = h.mp, marker = $("vsEnemyMarker"), hud = $("vsRespawnHud"), flash = $("vsExplosionFlash"), vp = $("viewport");
    const connected = !!mp?.connected, peer = connected ? (mp.players || [])[0] : null;
    const localDead = connected && !!h.ctl?.dead, peerDead = !!peer?.dead;
    if (flash) {
      if (localDead && !S.localDead) { flash.classList.remove("pulse", "local"); void flash.offsetWidth; flash.classList.add("local"); flash.classList.add("pulse"); }
      if (peerDead && !S.peerDead) { flash.classList.remove("pulse", "local"); void flash.offsetWidth; flash.classList.add("pulse"); }
    }
    S.localDead = localDead; S.peerDead = peerDead;
    if (marker) {
      // the mates are drawn by mp_host.mjs (every mate: in view over his head, out of view at the
      // edge with the direction, distance, above / below) - the original's single marker stays off
      if (true) hide(marker, true);
      else {
        hide(marker, false);
        marker.classList.toggle("offscreen", !peer.on);
        marker.classList.toggle("dead", peerDead);
        marker.style.transform = `translate3d(${peer.x.toFixed(1)}px,${peer.y.toFixed(1)}px,0) translate(-50%,-50%)`;
        const sx = peer.x - vp.clientWidth / 2, sy = peer.y - vp.clientHeight / 2;
        const pointer = marker.querySelector(".vs-enemy-pointer");
        if (pointer) pointer.style.transform = `rotate(${(Math.atan2(sy, sx) * 180 / Math.PI + 90).toFixed(1)}deg)`;
        const hp = Math.max(0, Math.min(100, Math.round(peer.hp || 0))), d = peer.d || 0;
        text(marker.querySelector("strong"), peerDead ? "DOWN" : "ENEMY");
        text(marker.querySelector("small"), peerDead ? "WAITING RESET" : `${hp} HP · ${d < 100 ? d.toFixed(0) : Math.round(d)} m`);
        const fill = marker.querySelector(".vs-enemy-hp>b");
        if (fill) fill.style.transform = `scaleX(${hp / 100})`;
      }
    }
    if (hud) {
      const enemy = !localDead && peerDead, state = localDead ? "local" : enemy ? "enemy" : "none";
      hide(hud, state === "none");
      hud.classList.toggle("enemy", enemy);
      hud.classList.toggle("local", localDead);
      const strong = hud.querySelector("strong"), small = hud.querySelector("span"), button = hud.querySelector("button");
      const wait = localDead ? Math.max(0, (mp?.respawnIn ?? h.respawn ?? 0) * 1000) : 0;
      if (localDead) {
        text(strong, "DESTROYED");
        text(small, wait > 0 ? `RESPAWN IN ${Math.ceil(wait / 1000)}` : body().classList.contains("pad-input") ? "PRESS Ⓐ TO RESPAWN" : body().classList.contains("desktop-input") ? "PRESS R TO RESPAWN" : "TAP TO RESPAWN");
      } else if (enemy) { text(strong, "ENEMY DESTROYED"); text(small, "WAITING FOR RESET"); }
      if (button) {
        hide(button, !localDead); button.disabled = wait > 0; text(button, "RESPAWN");
        if (!button.dataset.kern) { button.dataset.kern = "1"; button.addEventListener("click", () => view.command("respawn")); }
      }
    }
  }

  // ---------------------------------------------------------------------------------------------
  // guided_missile.mjs renderLock / renderCooldown; gameplay_final_runtime_v2.mjs updateDroneAim
  function guided(h) {
    const g = h.ctl?.view?.guidedView, drone = h.mode === "drone", isGuided = drone && h.weapon === "GUIDED";
    const box = $("guidedLockBox");
    if (box) {
      if (!g?.lock) box.style.display = "none";
      else {
        box.style.display = "block";
        box.style.transform = `translate(${g.x}px,${g.y}px)`;
        box.style.opacity = String(0.85 + 0.15 * Math.sin(performance.now() / 90));
        if (box.firstChild) box.firstChild.textContent = `LOCK · ${g.label} · ${Math.round(g.d)} m`;
      }
    }
    const left = isGuided && g ? g.cdMs : 0;
    body().classList.toggle("guided-denied", !!(drone && g?.denied));
    text($("guidedCooldownNear"), left > 0 ? `LENKRAKETE LÄDT · ${(left / 1000).toFixed(1)} s` : "LENKRAKETE BEREIT");
    body().style.setProperty("--guided-cd", isGuided ? (left / 5000).toFixed(3) : "0");
    body().classList.toggle("guided-reloading", isGuided && left > 0);
    for (const el of document.querySelectorAll('#droneWeaponToggle,#mobileGameplayWeapon,[data-drone-weapon="guided"]')) {
      if (isGuided && left > 0) data(el, "guidedCd", `${(left / 1000).toFixed(1)} s`);
      else if (el.dataset.guidedCd !== undefined) delete el.dataset.guidedCd;
    }
    const reticle = $("droneGuidedLock"), vp = $("viewport");
    if (reticle) {
      hide(reticle, !isGuided);
      if (isGuided && g) {
        data(reticle, "locked", g.actor ? "1" : "0");
        text(reticle, g.actor ? "◈ ZIEL" : "◇ LENK");
        if (vp) { reticle.style.left = `${vp.clientWidth / 2}px`; reticle.style.top = `${vp.clientHeight / 2}px`; }
      }
    }
  }

  // ---------------------------------------------------------------------------------------------
  // speed_lines.mjs: neon streaks from the centre above 13 m/s (full at 40), half resolution
  function speedLines(h) {
    const canvas = $("speedLines"), eye = h.ctl?.view?.eye;
    if (!canvas || !eye) return;
    const ctx = canvas.getContext("2d"), now = performance.now(), dt = Math.min(0.1, (now - S.last) / 1000);
    S.last = now;
    if (S.lastEye && dt > 0) { const d = Math.hypot(eye[0] - S.lastEye[0], eye[1] - S.lastEye[1], eye[2] - S.lastEye[2]) / dt; S.speed += (Math.min(d, 90) - S.speed) * Math.min(1, dt * 4); }
    S.lastEye = eye;
    const k = Math.max(0, Math.min(1, (S.speed - 13) / 27));
    const w = canvas.clientWidth >> 1, hh = canvas.clientHeight >> 1;
    if (!w || !hh) return;
    if (k <= 0) { if (canvas.dataset.on === "1") { ctx.clearRect(0, 0, canvas.width, canvas.height); canvas.dataset.on = "0"; } return; }
    if (canvas.width !== w || canvas.height !== hh) { canvas.width = w; canvas.height = hh; }
    canvas.dataset.on = "1";
    ctx.clearRect(0, 0, w, hh); ctx.globalCompositeOperation = "lighter"; ctx.lineCap = "round";
    const spawn = i => ({a: Math.random() * Math.PI * 2, r: 0.15 + Math.random() * 0.5, len: 0.08 + Math.random() * 0.22, v: 0.9 + Math.random() * 1.4, c: i % 3 === 0 ? "255,236,200" : "255,255,255"});
    while (S.streaks.length < 26) S.streaks.push(spawn(S.streaks.length));
    const cx = w / 2, cy = hh / 2, R = Math.hypot(cx, cy);
    for (let i = 0; i < S.streaks.length; i++) {
      const s = S.streaks[i];
      s.r += s.v * dt * (0.6 + 1.8 * k);
      if (s.r > 1.15) { S.streaks[i] = spawn(i); continue; }
      const r0 = s.r * R, r1 = (s.r + s.len * (0.5 + k)) * R, ca = Math.cos(s.a), sa = Math.sin(s.a), alpha = k * Math.min(1, s.r * 1.6) * 0.55;
      ctx.strokeStyle = `rgba(${s.c},${alpha.toFixed(3)})`; ctx.lineWidth = 0.8 + 1.6 * k * s.r;
      ctx.beginPath(); ctx.moveTo(cx + ca * r0, cy + sa * r0); ctx.lineTo(cx + ca * r1, cy + sa * r1); ctx.stroke();
    }
    ctx.globalCompositeOperation = "source-over";
  }

  // ---------------------------------------------------------------------------------------------
  // flight_fire_fx.mjs damage vignette, crosshair hit confirm, screen impacts; xbox_control_mode.mjs crosshair
  function feedback(h) {
    const v = h.ctl?.view || {};
    const vig = q(".combat-damage-vignette"), cross = q(".xbox-crosshair");
    if (S.dmg >= 0 && v.dmg > S.dmg) pulse(vig, "active");
    S.dmg = v.dmg ?? S.dmg;
    if (S.hits >= 0 && v.droneHits > S.hits) pulse(cross, "hit-confirm");
    S.hits = v.droneHits ?? S.hits;
    body().classList.toggle("xbox-aim-active", !!v.xboxAim);
    cross?.classList.toggle("active", !!v.xboxAim);
    const marks = document.querySelectorAll(".flight-fire-impact");
    for (const [id, x, y] of v.impacts || []) {
      if (id <= S.impactSeen) continue;
      S.impactSeen = id;
      const el = marks[S.impactCursor++ % Math.max(1, marks.length)];
      if (!el) continue;
      el.classList.remove("active"); el.style.left = `${x}px`; el.style.top = `${y}px`; void el.offsetWidth; el.classList.add("active");
    }
  }

  // ---------------------------------------------------------------------------------------------
  // player_walk_mode_v4.mjs jetHud / syncDeathState / #footReticle; hand_grenade.mjs render / place;
  // fighter_jets.mjs renderButton / setTargeting
  function foot(h, view) {
    const ctl = h.ctl || {}, foot = h.mode === "foot";
    const jp = h.jetpack, fuel = $("jetpackFuel");
    if (fuel && jp) {
      fuel.classList.toggle("show", foot && (jp.fuel < 0.999 || jp.thrusting));
      fuel.classList.toggle("thrust", !!jp.thrusting);
      if (fuel.firstChild) fuel.firstChild.style.width = `${Math.round(jp.fuel * 100)}%`;
    }
    text(q("#footReadout b"), ctl.dead ? "PILOT DOWN · RESET" : "WALK READY");
    $("footReticle")?.classList.toggle("aim-assisted", !!ctl.view?.assist);
    // the hand grenade
    const g = $("handGrenadeBtn");
    if (g) {
      const touchPlay = ctl.device !== "desktop" && foot;
      g.classList.toggle("show", touchPlay && !ctl.dead);
      const nade = ctl.nade || {carry: 3, left: -1}, cooking = nade.left >= 0, left = cooking ? nade.left * 1000 : 0;
      g.classList.toggle("cooking", cooking);
      g.classList.toggle("urgent", cooking && left < 1200);
      g.classList.toggle("empty", !cooking && nade.carry <= 0);
      text(g.querySelector("b"), cooking ? (left / 1000).toFixed(1) : "●");
      text(g.querySelector("i"), String(nade.carry));
      const fuse = g.querySelector(".fuse");
      if (fuse) fuse.style.strokeDashoffset = String(cooking ? 182.2 * (1 - left / 4000) : 182.2);
    }
    // the air strike button and its aiming overlay
    const b = $("airStrikeButton"), k = ctl.view?.strikeK ?? 1;
    if (b) {
      if (b.disabled !== !!ctl.dead) b.disabled = !!ctl.dead;
      data(b, "ready", k >= 1 ? "1" : "0");
      b.style.setProperty("--cool", `${Math.round(k * 100)}%`);
    }
    const aiming = !!ctl.aiming;
    body().classList.toggle("air-strike-targeting", aiming);
    if (aiming && !S.strikeOverlay) {
      const o = document.createElement("dialog");
      o.id = "airStrikeTargeting";
      o.innerHTML = '<b>ZIEL MARKIEREN</b><small data-as-hint>Tippen = Laser auf das Ziel · die Jets kommen</small><div class="as-actions"><button type="button" data-a="cancel">✕ ABBRECHEN</button></div>';
      ($("viewport") || document.body).appendChild(o);
      const stop = e => { e.preventDefault(); e.stopPropagation(); e.stopImmediatePropagation?.(); };
      for (const btn of o.querySelectorAll(".as-actions button")) {
        btn.addEventListener("pointerdown", stop);
        btn.addEventListener("pointerup", e => { stop(e); view.command("strike cancel"); });
        btn.addEventListener("click", stop);
      }
      o.addEventListener("pointerdown", e => { if (e.target.closest?.(".as-actions")) return; stop(e); o.dataset.down = "1"; });
      o.addEventListener("pointerup", e => {
        if (e.target.closest?.(".as-actions")) return;
        stop(e);
        if (o.dataset.down !== "1") return;
        o.dataset.down = "0";
        // the game's frame (host.mjs local(): CSS px of the canvas, the quarter turn undone)
        const c = $("game")?.getBoundingClientRect(), rot = document.documentElement.classList.contains("sc-css-landscape");
        const x = rot ? e.clientY : e.clientX - (c?.left || 0), y = rot ? innerWidth - e.clientX : e.clientY - (c?.top || 0);
        view.command(`strike at ${x.toFixed(0)} ${y.toFixed(0)}`);
      });
      o.addEventListener("click", stop);
      o.addEventListener("contextmenu", stop);
      S.strikeOverlay = o;
    }
    const o = S.strikeOverlay;
    if (o) {
      if (aiming && !o.open) {
        o.classList.remove("placed");
        const hint = o.querySelector("[data-as-hint]");
        if (hint) hint.textContent = document.pointerLockElement ? "Fadenkreuz aufs Ziel · KLICK / J = Laser + Jets · ESC abbrechen" : "Tippen = Laser auf das Ziel · die Jets kommen";
        o.show?.();
        o.setAttribute("open", "");
      } else if (!aiming && o.open) { o.close?.(); o.removeAttribute("open"); }
    }
  }

  // the radio in the jet and the drone too (car_radio.mjs plays in every vehicle, audio.mjs): a 📻
  // button in the jet's button row and the drone's top bar - tap = next station, hold = off
  // (page buttons: view.mjs PAGE_BUTTONS ".sc-radio-btn"); the car keeps the original's own
  const radioBtn = where => {
    const b = document.createElement("button");
    b.type = "button"; b.className = "sc-radio-btn"; b.textContent = "📻"; b.dataset.where = where;
    b.title = "Radio: tippen = nächster Sender, halten = aus"; b.setAttribute("aria-label", "Radio");
    let holdT = 0, held = false;
    b.addEventListener("pointerdown", e => { e.preventDefault(); e.stopPropagation(); held = false; holdT = setTimeout(() => { held = true; globalThis.__arondightCarRadio?.toggleOff?.(); }, 550); });
    b.addEventListener("pointerup", e => { e.preventDefault(); e.stopPropagation(); clearTimeout(holdT); if (held) return; if (!globalThis.__scRadioAny) { globalThis.__scRadioAny = true; return; } globalThis.__arondightCarRadio?.next?.(1); });
    b.addEventListener("pointercancel", () => clearTimeout(holdT));
    b.addEventListener("click", e => { e.preventDefault(); e.stopPropagation(); });
    return b;
  };
  function radios(h) {
    const jetRow = q("#jetHud .jet-btns"), vp = $("viewport");
    if (jetRow && !jetRow.querySelector(".sc-radio-btn")) jetRow.appendChild(radioBtn("jet"));
    // the drone: its own small button left of the minimap (the drone's top bar is hidden on phones)
    if (vp && !vp.querySelector('.sc-radio-btn[data-where="drone"]')) {
      const b = radioBtn("drone");
      b.style.cssText = "position:absolute;z-index:12;top:calc(max(48px,calc(var(--solo-safe-top,env(safe-area-inset-top)) + 42px)) + 4px);right:calc(max(10px,var(--solo-safe-right,env(safe-area-inset-right))) + 128px);width:44px;height:44px;border-radius:12px;border:1px solid rgba(127,240,197,.55);background:rgba(7,21,34,.86);color:#fff;font-size:20px;padding:0";
      vp.appendChild(b);
    }
    for (const b of document.querySelectorAll(".sc-radio-btn")) hide(b, b.dataset.where !== h.mode);
  }

  registerViewBinder("modes", (h, touch, view) => {
    document.body.classList.toggle("sc-mp-on", !!h.net);  // the voice buttons only in MULTI (index.html)
    radios(h);
    sticks(h);
    car(h, view);
    jet(h, view);
    wanted(h);
    sniper(h);
    zombies(h);
    contract(h);
    mpFun(h);
    vs(h);
    guided(h);
    speedLines(h);
    feedback(h);
    foot(h, view);
  });
}

installViewModes();
export const VIEW_MODES = true;
