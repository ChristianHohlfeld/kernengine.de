// SpawnCity on Kern - street life on the page (M3 life): what the browser has to do for it and
// nothing else. The game (game_life.cpp) decides who says what, where the speech bubbles are on
// screen (projected in C++) and when a banner shows; this module speaks the lines with the
// browser's speech synthesis (de-DE, the speaker's pitch and rate, as street_voices.mjs and
// jogger_chain.mjs do) and draws the bubbles and the banner as DOM.

export function installSpawnCityLife({root = document.body, muted = false} = {}) {
  const style = document.createElement("style");
  style.textContent = `#scLifeBubbles{position:absolute;inset:0;pointer-events:none;z-index:12;overflow:hidden}
#scLifeBubbles div{position:absolute;left:0;top:0;transform:translate(-50%,-100%);padding:4px 9px;border-radius:10px;background:#fffffff0;color:#111;font:800 12px/1.2 system-ui,sans-serif;white-space:nowrap;box-shadow:0 3px 10px #0006;transition:opacity .35s}
#scLifeBubbles div::after{content:"";position:absolute;left:50%;bottom:-6px;margin-left:-6px;border:6px solid transparent;border-bottom:0;border-top-color:#fffffff0}
#scLifeBubbles div.angry,#scLifeBubbles div.nearmiss{background:#ffe2dcf2;color:#7a0d00}#scLifeBubbles div.angry::after,#scLifeBubbles div.nearmiss::after{border-top-color:#ffe2dcf2}
#scLifeBanner{position:absolute;left:50%;top:30%;transform:translate(-50%,-50%);z-index:32;pointer-events:none;text-align:center;font:900 italic clamp(20px,4.4vw,40px)/1 system-ui,sans-serif;letter-spacing:.06em;color:#d8ff3a;text-shadow:0 3px 0 #2a3a06,0 0 18px #d8ff3a88;transition:opacity .4s;opacity:0}`;
  document.head.appendChild(style);
  const layer = document.createElement("div");
  layer.id = "scLifeBubbles";
  const banner = document.createElement("div");
  banner.id = "scLifeBanner";
  root.appendChild(layer);
  root.appendChild(banner);
  let voice = null;
  const pickVoice = () => {
    if (voice !== null || typeof speechSynthesis === "undefined") return voice;
    const vs = speechSynthesis.getVoices?.() || [];
    if (!vs.length) return null;
    voice = vs.find(v => /^de(-|_|$)/i.test(v.lang)) || false;
    return voice;
  };
  try { speechSynthesis?.addEventListener?.("voiceschanged", () => { voice = null; }); } catch { /* no speech */ }
  const speak = ([text, pitch, rate, volume]) => {
    if (muted || typeof SpeechSynthesisUtterance === "undefined" || volume < 0.05) return;
    try {
      const u = new SpeechSynthesisUtterance(text);
      u.lang = "de-DE";
      const v = pickVoice();
      if (v) u.voice = v;
      u.pitch = pitch; u.rate = rate; u.volume = Math.min(1, volume);
      speechSynthesis.speak(u);
    } catch { /* no speech */ }
  };
  let said = 0, key = "";
  return {
    update(h) {
      const L = h?.life;
      if (!L) return;
      for (const s of L.say || []) {
        said++;
        if (s[4] > 0) setTimeout(() => speak(s), s[4] * 1000); else speak(s);
      }
      const w = layer.clientWidth, hh = layer.clientHeight;
      const k = (L.bubbles || []).map(b => b[2]).join("|");
      if (k !== key) {
        key = k;
        layer.replaceChildren(...(L.bubbles || []).map(b => { const d = document.createElement("div"); d.className = b[3]; d.textContent = b[2]; return d; }));
      }
      (L.bubbles || []).forEach((b, i) => {
        const d = layer.children[i];
        if (!d) return;
        d.style.transform = `translate(${(b[0] * w).toFixed(1)}px,${(b[1] * hh).toFixed(1)}px) translate(-50%,-100%)`;
        d.style.opacity = b[4] ? "0" : "1";
      });
      banner.style.opacity = L.banner ? "1" : "0";
      if (L.banner) banner.textContent = L.banner;
      layer.dataset.said = String(said);
    },
  };
}
