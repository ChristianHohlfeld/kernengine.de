// SpawnCity on Kern - HUD parts of sc-m3-gaps (PARITY.md), drawn from the game's JSON (`gaps`):
//  * impact_explosion_overlay.mjs: a spark burst (core, ring, 9 sparks, 330 ms, pool of 12) at the
//    screen point where a drone MG round landed (the game projects the impact points),
//  * wanted_dispatch_guard.mjs: "POLIZEI-EINSATZ · n DROHNEN IM ANFLUG · x s" while drones are on their way.
// DOM only; every decision is C++'s.
const GAPS_STYLE = `
.sc-burst{position:absolute;z-index:14;width:0;height:0;pointer-events:none;display:none;contain:layout style paint}
.sc-burst.active{display:block}
.sc-burst .core{position:absolute;left:-7px;top:-7px;width:14px;height:14px;border-radius:50%;background:radial-gradient(circle,#fff 0 12%,#fff39a 18%,#ff9e31 42%,#ff3d20aa 62%,transparent 76%);box-shadow:0 0 12px #ffbe49,0 0 28px #ff5b2a}
.sc-burst .ring{position:absolute;left:-8px;top:-8px;width:16px;height:16px;border-radius:50%;border:2px solid #ffd46f;box-shadow:0 0 8px #ff7a32}
.sc-burst .spark{position:absolute;left:-2px;top:-2px;width:4px;height:4px;border-radius:50%;background:#fff0a0;box-shadow:0 0 6px #ff9a32;transform:rotate(var(--a)) translateX(0) scale(1)}
.sc-burst.active .core{animation:scBurstCore .33s ease-out forwards}
.sc-burst.active .ring{animation:scBurstRing .33s ease-out forwards}
.sc-burst.active .spark{animation:scBurstSpark .33s cubic-bezier(.15,.7,.2,1) forwards;animation-delay:var(--d)}
@keyframes scBurstCore{0%{transform:scale(.35);opacity:1}38%{transform:scale(1.8);opacity:1}100%{transform:scale(3.6);opacity:0}}
@keyframes scBurstRing{0%{transform:scale(.25);opacity:1}100%{transform:scale(5.2);opacity:0}}
@keyframes scBurstSpark{0%{transform:rotate(var(--a)) translateX(1px) scale(1.2);opacity:1}100%{transform:rotate(var(--a)) translateX(var(--r)) scale(.35);opacity:0}}
.sc-repair{position:absolute;right:calc(max(14px,env(safe-area-inset-right)) + 120px);bottom:calc(max(16px,env(safe-area-inset-bottom)) + 150px);pointer-events:auto;font:900 15px system-ui,sans-serif;letter-spacing:.06em;color:#111;background:#ffb84a;border:0;border-radius:10px;padding:10px 16px;box-shadow:0 3px 12px #0007}
.sc-dispatch{position:absolute;left:50%;top:calc(max(12px,env(safe-area-inset-top)) + 34px);transform:translateX(-50%);font:800 12px system-ui,sans-serif;letter-spacing:.06em;color:#fff;background:rgba(40,60,160,.72);border:1px solid #6f8cff;padding:4px 10px;border-radius:6px;white-space:nowrap;pointer-events:none}
`;

export class GapsHud {
  constructor(root, command = () => "") {
    this.root = root;
    this.command = command;
    const style = document.createElement("style");
    style.textContent = GAPS_STYLE;
    document.head.appendChild(style);
    this.pool = Array.from({length: 12}, (_, k) => {
      const el = document.createElement("i");
      el.className = "sc-burst";
      el.innerHTML = '<i class="core"></i><i class="ring"></i>';
      for (let i = 0; i < 9; i++) {
        const s = document.createElement("i");
        s.className = "spark";
        s.style.setProperty("--a", `${(i * 40 + k * 11) % 360}deg`);
        s.style.setProperty("--r", `${25 + (i % 4) * 9}px`);
        s.style.setProperty("--d", `${(i % 3) * 7}ms`);
        el.appendChild(s);
      }
      root.appendChild(el);
      return {el, timer: 0};
    });
    this.cursor = 0;
    this.seen = 0;
    this.bursts = 0;
    this.dispatch = Object.assign(document.createElement("div"), {className: "sc-dispatch", hidden: true});
    root.appendChild(this.dispatch);
    // zombie_nights.mjs #zombieRepair: REPAIR · 10 near a door with a plank missing (F does the same)
    this.repair = Object.assign(document.createElement("button"), {className: "sc-repair", type: "button", hidden: true, textContent: "REPARIEREN · 10"});
    this.repair.addEventListener("pointerdown", e => { e.preventDefault(); e.stopPropagation(); this.command("repair"); });
    root.appendChild(this.repair);
  }
  update(h) {
    const g = h?.gaps;
    if (!g) return;
    const W = this.root.clientWidth || innerWidth, H = this.root.clientHeight || innerHeight;
    for (const [x, y, serial] of g.bursts || []) {
      if (serial <= this.seen) continue;
      this.seen = serial;
      const item = this.pool[this.cursor++ % this.pool.length];
      clearTimeout(item.timer);
      item.el.classList.remove("active");
      item.el.style.left = `${(x + 1) / 2 * W}px`;
      item.el.style.top = `${(1 - y) / 2 * H}px`;
      void item.el.offsetWidth;
      item.el.classList.add("active");
      item.timer = setTimeout(() => item.el.classList.remove("active"), 360);
      this.bursts++;
    }
    this.repair.hidden = !g.repair;
    this.dispatch.hidden = !(g.inbound > 0);
    if (g.inbound > 0) this.dispatch.textContent = `POLIZEI-EINSATZ · ${g.inbound} ${g.inbound === 1 ? "DROHNE" : "DROHNEN"} IM ANFLUG · ${g.inboundS.toFixed(1)} s`;
  }
}
