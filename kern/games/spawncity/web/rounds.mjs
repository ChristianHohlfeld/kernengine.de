// SpawnCity on Kern - the rounds on the page (the game decides: game_rounds.cpp; this only shows
// it): the RUNDE button, the vote, the countdown, the goal and the clock while it runs, the
// marker to the next gate / the ring, the seeker's closed eyes, the results. Personal bests (the
// race alone) are kept in this browser.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
import {registerViewBinder} from "./view.mjs";
import {sharePicture, shareClip, clipRunning} from "./share.mjs";

const MODES = [
  [1, "🚓", "RÄUBER & POLIZEI", "Einer flieht, alle jagen"],
  [2, "🏁", "DROHNENRENNEN", "8 Tore durch die Straßen"],
  [3, "🧟", "ZOMBIENACHT", "Überlebt bis zum Morgen"],
  [4, "👑", "KING OF THE HILL", "Allein im Ring = Punkte"],
  [5, "🙈", "VERSTECKEN", "Einer sucht, alle verstecken sich"],
];
const CSS = `
#scRoundBtn{position:absolute;z-index:41;left:50%;transform:translateX(-50%);top:var(--sc-round-top,60px);padding:6px 12px;border-radius:999px;border:1.5px solid #ffd76a99;background:#0b1118c7;color:#ffe9a8;font:900 12px/1 Inter,system-ui,sans-serif;letter-spacing:.12em;cursor:pointer}
#scRoundBtn[hidden]{display:none}
#scRound{position:absolute;inset:0;z-index:40;overflow:hidden;pointer-events:none;font-family:Inter,system-ui,sans-serif;color:#fff}
#scRound .card{position:absolute;left:50%;top:50%;transform:translate(-50%,-50%);width:min(92vw,460px);padding:14px;border-radius:14px;background:#0b1118ee;border:1.5px solid #ffd76a88;pointer-events:auto;box-shadow:0 10px 30px #0009}
#scRound .card h3{margin:0 0 8px;font:900 16px/1.1 system-ui;letter-spacing:.14em;color:#ffd76a;display:flex;justify-content:space-between}
#scRound .modes{display:grid;grid-template-columns:1fr 1fr;gap:6px}
#scRound .modes button{display:flex;flex-direction:column;align-items:flex-start;gap:2px;padding:9px 10px;border-radius:10px;border:1px solid #ffffff2a;background:#ffffff0c;color:#fff;text-align:left;cursor:pointer;font:inherit}
#scRound .modes button.on{border-color:#7ff0c5;background:#7ff0c522}
#scRound .modes button b{font:900 13px/1.1 system-ui;letter-spacing:.04em}#scRound .modes button small{opacity:.7;font:600 11px/1.25 system-ui}
#scRound .modes button i{font-style:normal;font:800 11px system-ui;color:#7ff0c5}
#scRound .big{position:absolute;left:50%;top:38%;transform:translate(-50%,-50%);font:900 min(22vw,140px)/1 system-ui;color:#ffd76a;text-shadow:0 4px 18px #000c}
#scRound .bar{position:absolute;left:50%;top:var(--sc-round-top,60px);transform:translateX(-50%);display:flex;flex-direction:column;align-items:center;gap:3px;max-width:min(92vw,620px)}
#scRound .bar .row{display:flex;gap:8px;align-items:center;padding:5px 12px;border-radius:999px;background:#0b1118d9;border:1.5px solid #ffd76a88;font:900 12px/1 system-ui;letter-spacing:.1em;white-space:nowrap}
#scRound .bar .row em{font-style:normal;color:#ffd76a}#scRound .bar .row u{text-decoration:none;color:#7ff0c5}
#scRound .bar .goal{padding:4px 10px;border-radius:9px;background:#0b1118b8;font:700 12px/1.25 system-ui;text-align:center}
#scRound .board{position:absolute;right:max(10px,env(safe-area-inset-right));top:calc(var(--sc-round-top,60px) + 150px);display:flex;flex-direction:column;gap:3px;font:800 11px/1 system-ui}
#scRound .board div{display:flex;gap:6px;align-items:center;padding:3px 8px;border-radius:7px;background:#0b1118c4;border-left:3px solid var(--c)}
#scRound .board div.me{background:#16324acc}#scRound .board div.out{opacity:.5}
#scRound .mark{position:absolute;left:0;top:0;transform:translate(-50%,-50%);display:flex;flex-direction:column;align-items:center;gap:2px;font:900 12px/1 system-ui;color:#7ff0c5;text-shadow:0 1px 3px #000}
#scRound .mark i{width:0;height:0;border-left:10px solid transparent;border-right:10px solid transparent;border-top:14px solid #7ff0c5;filter:drop-shadow(0 0 4px #7ff0c5)}
#scRound .blind{position:absolute;inset:0;background:#000;display:flex;align-items:center;justify-content:center;flex-direction:column;gap:10px;font:900 22px/1.2 system-ui;letter-spacing:.12em}
#scRound .share{display:flex;gap:6px;margin-top:10px}#scRound .share button{flex:1;padding:8px;border-radius:9px;border:1px solid #7ff0c588;background:#7ff0c51a;color:#dffcf1;font:900 12px system-ui;letter-spacing:.08em;cursor:pointer}
#scRound .xp{margin-top:6px;font:900 12px system-ui;color:#7ff0c5;letter-spacing:.08em}
#scRoundBtn b{margin-left:6px;color:#7ff0c5}
#scToast{position:absolute;left:50%;bottom:90px;transform:translateX(-50%);z-index:42;padding:6px 12px;border-radius:999px;background:#0b1118e0;border:1.5px solid #7ff0c588;color:#dffcf1;font:900 12px/1 Inter,system-ui,sans-serif;letter-spacing:.08em;pointer-events:none}#scToast[hidden]{display:none}
#scRound .res ol{margin:6px 0 0;padding-left:22px;font:800 13px/1.6 system-ui}
`;

export function installRounds({command}) {
  const st = document.createElement("style"); st.textContent = CSS; document.head.appendChild(st);
  const btn = document.createElement("button");
  btn.id = "scRoundBtn"; btn.type = "button"; btn.textContent = "🏁 RUNDE"; btn.title = "Eine Runde starten (Taste O)";
  btn.addEventListener("pointerdown", ev => ev.stopPropagation(), true);
  btn.addEventListener("click", ev => { ev.preventDefault(); ev.stopPropagation(); command("round start"); });
  const root = document.createElement("div"); root.id = "scRound";
  // inside the game's viewport (it lies over everything on the page), under the voice bar
  const mount = () => {
    const vp = document.getElementById("viewport") || document.body;
    if (root.parentElement !== vp) { vp.appendChild(root); vp.appendChild(btn); }
    return vp;
  };
  mount();
  let anchorAt = 0;
  const anchor = vp => {
    if (performance.now() - anchorAt < 500) return;
    anchorAt = performance.now();
    const top = vp.getBoundingClientRect().top;
    let y = 54;
    for (const id of ["proximityVoiceBar"]) {
      const e = document.getElementById(id);
      if (!e || !e.offsetParent) continue;
      const r = e.getBoundingClientRect();
      if (r.height) y = Math.max(y, r.bottom - top + 6);
    }
    vp.style.setProperty("--sc-round-top", `${Math.round(y)}px`);
  };
  root.addEventListener("pointerdown", ev => { if (ev.target.closest("button")) ev.stopPropagation(); }, true);
  root.addEventListener("click", ev => {
    const b = ev.target.closest("[data-vote]");
    if (b) { ev.stopPropagation(); command(`round vote ${b.dataset.vote}`); }
    const sh = ev.target.closest("[data-share]");
    if (sh) { ev.stopPropagation(); if (sh.dataset.share === "clip") clip(); else picture(lastRes); }
  });
  addEventListener("keydown", ev => {
    if (ev.code !== "KeyO" || ev.repeat || ev.target?.closest?.("input,textarea")) return;
    command("round start");
  });
  const toast = document.createElement("div"); toast.id = "scToast"; toast.hidden = true;
  let toastUntil = 0;
  const say = (t, ms = 2500) => { toast.textContent = t; toast.hidden = false; toastUntil = performance.now() + ms; };
  // the player's experience: rounds played, points, wins - kept in this browser, nothing to buy
  let xp = 0, xpRound = -1, xpGain = 0;  // xpRound: the round already counted (this visit)
  try { xp = Math.max(0, Number(localStorage.getItem("spawnCityXP")) || 0); } catch { /* private mode */ }
  const levelOf = v => 1 + Math.floor(Math.sqrt(v / 60));
  const clip = () => {
    if (clipRunning()) return;
    shareClip(10, left => { if (left > 0) say(`🎬 CLIP ● ${Math.ceil(left)} s`, 600); }).then(r => say(`🎬 Clip ${r}`));
  };
  const picture = line => sharePicture(line).then(r => say(`📸 Bild ${r}`));
  addEventListener("keydown", ev => {
    if (ev.repeat || ev.target?.closest?.("input,textarea")) return;
    if (ev.code === "F8") { ev.preventDefault(); clip(); }
    else if (ev.code === "F9") { ev.preventDefault(); picture(""); }
  });
  let key = "", best = null, lastRes = "";
  try { best = JSON.parse(localStorage.getItem("spawnCityRaceBest") || "null"); } catch { best = null; }
  const mmss = s => { s = Math.max(0, Math.ceil(s)); return `${Math.floor(s / 60)}:${String(s % 60).padStart(2, "0")}`; };
  const esc = s => String(s ?? "").replace(/[&<>"]/g, c => ({"&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;"}[c]));
  registerViewBinder("rounds", h => {
    const vp = mount();
    if (toast.parentElement !== vp) vp.appendChild(toast);
    if (!toast.hidden && performance.now() > toastUntil) toast.hidden = true;
    anchor(vp);
    const R = h.round;
    const playing = !!document.body.classList.contains("solo-flight") || !!h.placed;
    btn.hidden = !R || R.phase !== 0 || !playing;
    const lv = `🏁 RUNDE<b>LV ${levelOf(xp)}</b>`;
    if (btn.innerHTML !== lv) btn.innerHTML = lv;
    if (!R) { if (root.innerHTML) root.innerHTML = ""; return; }
    // a personal best of the race (alone or not): kept here
    if (R.myTime > 0 && (!best || R.myTime < best)) { best = R.myTime; try { localStorage.setItem("spawnCityRaceBest", JSON.stringify(best)); } catch { /* private mode */ } }
    let html = "";
    const me = (R.players || []).find(p => p.me);
    if (R.phase === 1) {
      const counts = {};
      for (const p of R.players || []) if (p.vote) counts[p.vote] = (counts[p.vote] || 0) + 1;
      html = `<div class="card"><h3><span>WÄHLT EINE RUNDE</span><span>${mmss(R.left)}</span></h3><div class="modes">${MODES.map(([id, ic, name, sub]) =>
        `<button data-vote="${id}" class="${me?.vote === id ? "on" : ""}"><b>${ic} ${name}</b><small>${sub}</small><i>${counts[id] ? "● ".repeat(counts[id]).trim() : ""}</i></button>`).join("")}</div></div>`;
    } else if (R.phase === 2) {
      html = `<div class="big">${Math.max(1, Math.ceil(R.left))}</div><div class="bar"><div class="row"><em>${esc(R.modeName)}</em>${R.role ? `<u>${esc(R.role)}</u>` : ""}</div></div>`;
    } else if (R.phase === 3) {
      const board = [...(R.players || [])].sort((a, b) => b.score - a.score).slice(0, 6)
        .map(p => `<div class="${p.me ? "me" : ""}${p.out ? " out" : ""}" style="--c:${p.color}"><span>${p.me ? "DU" : "P" + p.index}</span><span>${p.score}</span>${p.role ? `<span style="opacity:.7">${esc(p.role)}</span>` : ""}</div>`).join("");
      const bestTxt = R.mode === 2 && best ? ` · Bestzeit ${best.toFixed(1)} s` : "";
      html = `<div class="bar"><div class="row"><em>${esc(R.modeName)}</em><span>${mmss(R.left)}</span>${R.role ? `<u>${esc(R.role)}</u>` : ""}</div><div class="goal">${esc(R.objective)}${bestTxt}</div></div><div class="board">${board}</div>`;
      if (R.target) html += `<div class="mark" style="left:${R.tx}px;top:${R.ty}px"><span>${Math.round(R.td)} m</span><i></i></div>`;
      if (R.blind) html = `<div class="blind"><span>AUGEN ZU</span><span>${Math.ceil(R.left)}</span><small style="font:700 13px system-ui;opacity:.7">Die anderen verstecken sich …</small></div>` + html;
    } else if (R.phase === 4) {
      // once per round: its experience (taking part, points, the win)
      if (xpRound !== R.id && me) {
        const top = Math.max(0, ...(R.players || []).map(p => p.score));
        xpGain = 25 + Math.min(300, 10 * me.score) + (me.score > 0 && me.score === top ? 50 : 0);
        const before = levelOf(xp);
        xp += xpGain; xpRound = R.id;
        try { localStorage.setItem("spawnCityXP", String(xp)); } catch { /* private mode */ }
        if (levelOf(xp) > before) say(`⭐ LEVEL ${levelOf(xp)}!`, 4000);
      }
      lastRes = `${R.modeName}: ${me ? me.score : 0} Punkte`;
      const ranking = [...(R.players || [])].sort((a, b) => b.score - a.score)
        .map(p => `<li style="color:${p.color}">${p.me ? "DU" : "P" + p.index} – ${p.score} Punkte${p.role ? ` (${esc(p.role)})` : ""}</li>`).join("");
      html = `<div class="card res"><h3><span>${esc(R.modeName)}</span><span>ENDE</span></h3><b>${esc(R.result)}</b>${R.myTime > 0 ? `<div>Deine Zeit: ${R.myTime.toFixed(1)} s${best ? ` · Bestzeit ${best.toFixed(1)} s` : ""}</div>` : ""}<ol>${ranking}</ol><div class="xp">+${xpGain} XP · LEVEL ${levelOf(xp)}</div><div class="share"><button data-share="pic">📸 BILD TEILEN</button><button data-share="clip">🎬 CLIP 10 s</button></div></div>`;
    }
    // the marker moves every frame: only it is updated in place when the rest is the same
    const k = html.replace(/style="left:[^"]*"/, "");
    if (k !== key) { root.innerHTML = html; key = k; }
    else if (R.target) { const m = root.querySelector(".mark"); if (m) { m.style.left = R.tx + "px"; m.style.top = R.ty + "px"; m.firstChild.textContent = `${Math.round(R.td)} m`; } }
  });
}
