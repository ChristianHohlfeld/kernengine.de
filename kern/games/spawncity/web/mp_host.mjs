// SpawnCity on Kern - the page side of multiplayer. The game (C++) decides everything; this module
// only does what the browser forces into JavaScript:
//   * transports for the game's datagrams (sc_net_push / sc_net_take):
//       local   BroadcastChannel between tabs of one browser (Kern's KernNet)
//       relay   the Kern relay over WebSocket (tools/net_relay.mjs, Kern's KernNet)
//       public  as spawncity.de connects players (lan_vs.mjs, direct_udp_peer.mjs,
//               mqtt_data_relay.mjs): WebRTC data channels (unordered, no retransmits = UDP-like,
//               STUN from Cloudflare / Google) signalled over SpawnCity's public MQTT brokers, every
//               signal end-to-end sealed (ECDH P-256 + AES-GCM); while no data channel is open
//               the sealed datagrams go over the brokers themselves (the encrypted relay fallback)
//   * the microphone for the proximity voice chat (explicit consent: opened only while the game
//     says the mic is on; samples go to C++, which encodes, sends, and mixes what it receives)
//   * DOM HUD: nameplates, kill feed, score board, hit marker, damage direction, spawn shield,
//     respawn panel, vote line, voice buttons, ping labels, route guidance, share / QR panel
//   * the big map (world_map_full.mjs): canvas drawing of what the game sends, pan / zoom, waypoint,
//     address search (Photon, (c) OpenStreetMap contributors) and road routes
//     (routing.openstreetmap.de, FOSSGIS) when the streamed streets do not reach the goal
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).

// mqtt_data_relay.mjs BROKER_CONFIGS, direct_udp_peer.mjs DEFAULT_ICE_SERVERS
const MP_BROKERS = [
  {url: "wss://public.cloud.shiftr.io", user: "public", pass: "public"},
  {url: "wss://broker.emqx.io:8084/mqtt"},
  {url: "wss://test.mosquitto.org:8081/mqtt"},
  {url: "wss://broker.hivemq.com:8884/mqtt"},
];
const MP_ICE = [{urls: ["stun:stun.cloudflare.com:3478", "stun:stun.cloudflare.com:53"]}, {urls: "stun:stun.l.google.com:19302"}];
const MP_TOPIC = "spawncity-kern/vs-data/v1/";
const MP_HELLO_MS = 1000;           // mqtt_data_relay.mjs HELLO_MS
const MP_HOST_WAIT_MS = 15000;      // CONNECT_ERROR_MS
const MP_AUTO_WAIT_MS = 4000;       // mp=auto: nobody hosts -> we host
const MP_LOBBY = "lobby";           // lan_vs.mjs: one public lobby, no split
const MP_PHOTON = "https://photon.komoot.io/api/";
const MP_ROUTER = "https://routing.openstreetmap.de";

const mpB64 = bytes => { let s = ""; for (let i = 0; i < bytes.length; i += 0x8000) s += String.fromCharCode.apply(null, bytes.subarray(i, i + 0x8000)); return btoa(s); };
const mpUnB64 = text => { const raw = atob(text), out = new Uint8Array(raw.length); for (let i = 0; i < raw.length; i++) out[i] = raw.charCodeAt(i); return out; };
const mpPeerId = () => { const v = new Uint32Array(2); crypto.getRandomValues(v); return (v[0] & 0x1fffff) * 4294967296 + v[1] + 1; };
const mpEsc = s => String(s ?? "").replace(/[&<>"']/g, c => ({"&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;"}[c]));

// ---- a minimal MQTT 3.1.1 client over WebSocket (QoS 0) ---------------------------------------
class MpMqtt {
  constructor({url, user, pass}, topics, onMessage) {
    this.url = url; this.topics = topics; this.onMessage = onMessage; this.open = false; this.buffer = new Uint8Array(0); this.packetId = 1;
    try { this.ws = new WebSocket(url, "mqtt"); } catch { this.ws = null; return; }
    this.ws.binaryType = "arraybuffer";
    this.ws.onopen = () => {
      const enc = new TextEncoder(), str = s => { const b = enc.encode(s); return [b.length >> 8, b.length & 255, ...b]; };
      const id = "sck-" + Math.random().toString(36).slice(2, 14);
      let flags = 0x02; const payload = [...str(id)];
      if (user) { flags |= 0x80; payload.push(...str(user)); }
      if (pass) { flags |= 0x40; payload.push(...str(pass)); }
      this.send(0x10, [...str("MQTT"), 4, flags, 0, 15, ...payload]);
    };
    this.ws.onmessage = ev => this.receive(new Uint8Array(ev.data));
    this.ws.onclose = this.ws.onerror = () => { this.open = false; };
    this.ping = setInterval(() => { if (this.open) this.send(0xc0, []); }, 10000);
  }
  send(head, body) {
    if (this.ws?.readyState !== 1) return;
    const len = []; let n = body.length;
    do { let b = n & 127; n >>= 7; if (n) b |= 128; len.push(b); } while (n);
    const out = new Uint8Array(1 + len.length + body.length);
    out[0] = head; out.set(len, 1); out.set(body, 1 + len.length);
    this.ws.send(out);
  }
  subscribe() {
    const enc = new TextEncoder();
    for (const t of this.topics) { const b = enc.encode(t), id = this.packetId++; this.send(0x82, [id >> 8, id & 255, b.length >> 8, b.length & 255, ...b, 0]); }
  }
  publish(topic, text) {
    if (!this.open) return false;
    const t = new TextEncoder().encode(topic), p = new TextEncoder().encode(text), body = new Uint8Array(2 + t.length + p.length);
    body[0] = t.length >> 8; body[1] = t.length & 255; body.set(t, 2); body.set(p, 2 + t.length);
    this.send(0x30, body);
    return true;
  }
  receive(chunk) {
    const b = new Uint8Array(this.buffer.length + chunk.length); b.set(this.buffer); b.set(chunk, this.buffer.length);
    let at = 0;
    while (at + 2 <= b.length) {
      let mul = 1, len = 0, i = at + 1, byte;
      do { if (i >= b.length) { this.buffer = b.slice(at); return; } byte = b[i++]; len += (byte & 127) * mul; mul *= 128; } while (byte & 128);
      if (i + len > b.length) break;
      const type = b[at] >> 4, body = b.subarray(i, i + len);
      if (type === 2) { this.open = true; this.subscribe(); }
      else if (type === 3) {
        const tl = (body[0] << 8) | body[1], qos = (b[at] >> 1) & 3, start = 2 + tl + (qos ? 2 : 0);
        try { this.onMessage(new TextDecoder().decode(body.subarray(2, 2 + tl)), new TextDecoder().decode(body.subarray(start))); } catch { /* a foreign packet */ }
      }
      at = i + len;
    }
    this.buffer = b.slice(at);
  }
  close() { clearInterval(this.ping); try { this.send(0xe0, []); this.ws?.close(); } catch { /* closed */ } }
}

// ---- public transport: WebRTC data channels, signalled (and relayed) over the brokers -------------
export class MpPublicNet {
  constructor(engine) { this.engine = engine; this.peers = new Map(); this.seen = new Set(); this.onstatus = null; this.sent = 0; this.received = 0; this.mode = "Relay"; }
  get active() { return !!this.clients; }
  async open({room = MP_LOBBY, hosting = false, auto = false} = {}) {
    this.self = mpPeerId(); this.hosting = hosting; this.room = room; this.host = hosting ? this.self : 0;
    this.topic = MP_TOPIC + room.replace(/[^a-zA-Z0-9_.-]/g, "_").slice(0, 64);
    this.pair = await crypto.subtle.generateKey({name: "ECDH", namedCurve: "P-256"}, true, ["deriveKey"]);
    this.jwk = await crypto.subtle.exportKey("jwk", this.pair.publicKey);
    this.clients = MP_BROKERS.map(cfg => new MpMqtt(cfg, [this.topic], (t, text) => this.onBroker(text)));
    this.hello = setInterval(() => this.announce(), MP_HELLO_MS);
    this.timer = setInterval(() => this.flush(), 8);
    this.announce();
    if (!hosting) {
      this.host = await new Promise((resolve, reject) => {
        this.hostWaiter = resolve;
        setTimeout(() => { if (this.host) return; this.hostWaiter = null; reject(Error(`niemand hostet den Raum "${room}" (öffentliche Broker)`)); }, auto ? MP_AUTO_WAIT_MS : MP_HOST_WAIT_MS);
      }).catch(error => { if (!auto) throw error; return 0; });
      if (!this.host) { this.hosting = true; this.host = this.self; }  // mp=auto: nobody there, we host
    }
    return {self: this.self, host: this.hosting ? 0 : this.host};
  }
  publish(packet) {
    packet.v = 1; packet.id = this.self; packet.msg = packet.msg || Math.random().toString(36).slice(2, 12);
    const text = JSON.stringify(packet);
    for (const c of this.clients) c.publish(this.topic, text);
  }
  announce() { this.publish({kind: "hello", host: this.hosting, key: this.jwk}); }
  async peerFor(id, jwk) {
    let p = this.peers.get(id);
    if (p) return p;
    if (!jwk) return null;
    const pub = await crypto.subtle.importKey("jwk", jwk, {name: "ECDH", namedCurve: "P-256"}, false, []);
    const key = await crypto.subtle.deriveKey({name: "ECDH", public: pub}, this.pair.privateKey, {name: "AES-GCM", length: 256}, false, ["encrypt", "decrypt"]);
    if (this.peers.has(id)) return this.peers.get(id);
    p = {id, key, pc: null, dc: null, queue: []};
    this.peers.set(id, p);
    return p;
  }
  async seal(p, value) {
    const iv = crypto.getRandomValues(new Uint8Array(12)), plain = new TextEncoder().encode(JSON.stringify(value));
    const box = new Uint8Array(await crypto.subtle.encrypt({name: "AES-GCM", iv}, p.key, plain));
    this.publish({kind: "sealed", target: p.id, iv: mpB64(iv), box: mpB64(box)});
  }
  async onBroker(text) {
    const m = JSON.parse(text);
    if (!m || m.v !== 1 || m.id === this.self || typeof m.msg !== "string" || this.seen.has(m.msg)) return;
    this.seen.add(m.msg); if (this.seen.size > 4096) this.seen.delete(this.seen.values().next().value);
    if (m.target && m.target !== this.self) return;
    if (m.kind === "hello") {
      // Only the host and its clients pair up: a client talks to the host, the host to everyone.
      if (!this.hosting && !m.host) return;
      const p = await this.peerFor(m.id, m.key);
      if (!p) return;
      if (!this.hosting && m.host && !this.host) { this.host = m.id; this.hostWaiter?.(m.id); }
      if (!this.hosting && m.id === this.host && !p.pc) this.connect(p, true);
      return;
    }
    if (m.kind !== "sealed") return;
    const p = this.peers.get(m.id);
    if (!p) return;
    let value;
    try { value = JSON.parse(new TextDecoder().decode(await crypto.subtle.decrypt({name: "AES-GCM", iv: mpUnB64(m.iv)}, p.key, mpUnB64(m.box)))); } catch { return; }
    if (value.t === "data") { this.deliver(p.id, mpUnB64(value.d)); return; }
    if (value.t === "offer" && this.hosting) { this.connect(p, false); await p.pc.setRemoteDescription(value.sdp); const a = await p.pc.createAnswer(); await p.pc.setLocalDescription(a); this.seal(p, {t: "answer", sdp: p.pc.localDescription}); return; }
    if (value.t === "answer" && p.pc) { await p.pc.setRemoteDescription(value.sdp).catch(() => {}); return; }
    if (value.t === "ice" && p.pc) { await p.pc.addIceCandidate(value.c).catch(() => {}); }
  }
  connect(p, offer) {
    if (typeof RTCPeerConnection !== "function") return;
    p.pc?.close();
    const pc = new RTCPeerConnection({iceServers: MP_ICE, bundlePolicy: "max-bundle", iceCandidatePoolSize: 2});
    p.pc = pc;
    // One negotiated channel, unordered without retransmits: Kern's connection does its own
    // reliability (direct_udp_peer.mjs "pose": ordered false, maxRetransmits 0).
    const dc = pc.createDataChannel("kern", {negotiated: true, id: 0, ordered: false, maxRetransmits: 0});
    dc.binaryType = "arraybuffer";
    dc.onopen = () => { p.dc = dc; this.mode = "WebRTC direkt"; this.onstatus?.(""); };
    dc.onclose = () => { if (p.dc === dc) p.dc = null; };
    dc.onmessage = ev => this.deliver(p.id, new Uint8Array(ev.data));
    pc.onicecandidate = ev => { if (ev.candidate) this.seal(p, {t: "ice", c: ev.candidate.toJSON()}); };
    if (offer) pc.createOffer().then(o => pc.setLocalDescription(o)).then(() => this.seal(p, {t: "offer", sdp: pc.localDescription})).catch(() => {});
  }
  deliver(from, bytes) { this.received++; this.engine.netPush(from, bytes); }
  flush() {
    if (!this.clients) return;
    for (let guard = 0; guard < 256; guard++) {
      const d = this.engine.netTake();
      if (!d) break;
      this.sent++;
      const p = this.peers.get(d.to);
      if (!p) continue;
      if (p.dc?.readyState === "open") { try { p.dc.send(d.bytes); continue; } catch { /* fall back */ } }
      this.seal(p, {t: "data", d: mpB64(d.bytes)});  // encrypted relay over the brokers
    }
  }
  status() {
    const open = this.clients?.filter(c => c.open).length || 0;
    return `${this.mode} · ${open}/${MP_BROKERS.length} Broker`;
  }
  close() { clearInterval(this.hello); clearInterval(this.timer); for (const p of this.peers.values()) p.pc?.close(); for (const c of this.clients || []) c.close(); this.clients = null; }
}

// ---- the multiplayer page parts ------------------------------------------------------------------
const MP_CSS = `
#mpHud{position:fixed;inset:0;pointer-events:none;z-index:5;font-family:Inter,system-ui,sans-serif;color:#fff}
.mp-plate{--c:#fff;position:absolute;left:0;top:0;display:flex;flex-direction:column;align-items:center;gap:3px;filter:drop-shadow(0 1px 2px #000c);will-change:transform}
.mp-plate .t{display:flex;gap:6px;align-items:baseline;padding:3px 8px;border-radius:8px;background:#060b10b8;border:1.5px solid var(--c);white-space:nowrap}
.mp-plate strong{font:900 13px/1 system-ui;letter-spacing:.06em;color:var(--c)}.mp-plate small{font:800 11.5px/1 system-ui;opacity:.92}
.mp-plate .hp{width:44px;height:3px;border-radius:2px;background:#ffffff30;overflow:hidden}.mp-plate .hp u{display:block;height:100%;background:var(--c)}
.mp-plate .ar{order:3;width:0;height:0;border-left:9px solid transparent;border-right:9px solid transparent;border-top:12px solid var(--c)}
.mp-plate.off .hp{display:none}.mp-plate.off .ar{position:absolute;left:50%;top:50%;margin:-6px 0 0 -9px;transform:rotate(var(--a)) translateY(30px)}
.mp-plate.dead{filter:grayscale(1);opacity:.75}.mp-plate .mic{display:none}.mp-plate.talk .mic{display:inline}
#mpFeed{position:absolute;right:max(10px,env(safe-area-inset-right));top:calc(max(12px,env(safe-area-inset-top)) + 160px);display:flex;flex-direction:column;align-items:flex-end;gap:4px}
#mpFeed div{padding:4px 9px;border-radius:7px;background:#070b10c4;font:800 12px/1.2 Inter,system-ui;white-space:nowrap}#mpFeed div.me{box-shadow:0 0 0 1.5px #ffd76a}
#mpFeed i{font-style:normal;opacity:.75;margin:0 6px}#mpFeed em{font-style:normal;color:#ff5a46;margin-left:5px}
#mpScore{position:absolute;left:max(10px,env(safe-area-inset-left));top:calc(max(12px,env(safe-area-inset-top)) + 40px);display:flex;flex-direction:column;gap:2px;padding:5px 7px;border-radius:8px;background:#070b10a8;font:800 11px/1.25 Inter,system-ui}
#mpScore span{display:flex;gap:8px;justify-content:space-between}#mpScore span.me{color:#ffd76a}#mpScore u{display:inline-block;width:8px;height:8px;border-radius:50%;margin-right:5px}
#mpHit{position:absolute;left:50%;top:50%;width:26px;height:26px;margin:-13px 0 0 -13px;opacity:0}
#mpHit::before,#mpHit::after{content:"";position:absolute;left:12px;top:0;width:2.5px;height:26px;background:#fff;box-shadow:0 0 3px #000;transform:rotate(45deg)}#mpHit::after{transform:rotate(-45deg)}
#mpHit.kill::before,#mpHit.kill::after{background:#ff3b2f}#mpHit.head::before,#mpHit.head::after{background:#ffd76a}#mpHit.show{opacity:1}
#mpHurt{position:absolute;left:50%;top:50%;width:min(56vmin,340px);height:min(56vmin,340px);transform:translate(-50%,-50%);border-radius:50%;opacity:0;transition:opacity .6s;background:conic-gradient(from calc(var(--a) - 22deg),#ff2b1f 0deg,#ff2b1fcc 44deg,transparent 44deg);-webkit-mask:radial-gradient(circle,transparent 62%,#000 63%,#000 70%,transparent 71%);mask:radial-gradient(circle,transparent 62%,#000 63%,#000 70%,transparent 71%)}
#mpHurt.show{opacity:.9;transition:none}
#mpShield{position:absolute;inset:0;opacity:0;transition:opacity .5s;box-shadow:inset 0 0 60px 10px #7fd4ffaa}#mpShield.show{opacity:1}
#mpShield b{position:absolute;left:50%;top:22%;transform:translateX(-50%);font:900 13px/1 Inter,system-ui;letter-spacing:.2em;color:#bfeaff;text-shadow:0 1px 3px #000}
#mpRespawn{position:absolute;left:50%;top:50%;transform:translate(-50%,-50%);min-width:min(78vw,330px);padding:20px 24px;border-radius:16px;border:1px solid #ff5a42bb;background:#130504f2;text-align:center}
#mpRespawn strong{display:block;font:950 29px/.95 system-ui;letter-spacing:.09em;color:#ff6650}#mpRespawn span{display:block;margin-top:9px;font:850 11px/1.25 system-ui;letter-spacing:.1em}
#mpVote{position:absolute;left:50%;top:calc(max(12px,env(safe-area-inset-top)) + 44px);transform:translateX(-50%);padding:5px 12px;border-radius:9px;background:#0b1118d0;border:1px solid #ffd76a88;font:800 12px/1.2 Inter,system-ui;pointer-events:auto}
#mpVote button{margin-left:8px;pointer-events:auto;border-radius:7px;border:1px solid #fff5;background:#1b2b3a;color:#fff;font:800 11px system-ui;padding:3px 9px}
#mpBar{position:absolute;left:clamp(180px,27vw,320px);top:max(8px,env(safe-area-inset-top));display:flex;gap:6px;pointer-events:auto}
#mpBar button{min-width:38px;height:34px;padding:0 9px;border-radius:9px;border:1.5px solid #94d4ea90;background:#132b3ddb;color:#eaf8ff;font:800 12px/1 system-ui;letter-spacing:.04em}
#mpBar button[aria-pressed="true"]{background:#1b674f;border-color:#91ffd7}#mpBar button.muted[aria-pressed="true"]{background:#60332c;border-color:#ffa998}
#mpPings{position:absolute;inset:0}#mpPings span{position:absolute;transform:translate(-50%,-140%);padding:1px 6px;border-radius:8px;background:#0b1118c0;font:800 10px/1.4 system-ui;white-space:nowrap;border:1px solid var(--c,#fff)}
#mpNav{position:absolute;left:50%;bottom:calc(max(34px,env(safe-area-inset-bottom)) + 22px);transform:translateX(-50%);padding:4px 12px;border-radius:9px;background:#070b10b0;font:800 12px/1.2 Inter,system-ui;white-space:nowrap}
#mpChev{position:absolute;left:0;top:0;width:0;height:0;border-left:11px solid transparent;border-right:11px solid transparent;border-bottom:16px solid #ffd76a;filter:drop-shadow(0 1px 2px #000)}
#mpShare{position:fixed;inset:0;display:grid;place-items:center;background:#0008;z-index:30;pointer-events:auto}
#mpShare>div{padding:18px 20px;border-radius:14px;background:#0d141c;border:1px solid #ffffff30;text-align:center;max-width:min(92vw,420px)}
#mpShare canvas{background:#fff;border-radius:8px;image-rendering:pixelated;width:220px;height:220px}#mpShare p{font:600 12px/1.4 system-ui;color:#c8d4e0;word-break:break-all}
#mpShare button{margin:4px;border-radius:9px;border:1px solid #fff4;background:#1b2b3a;color:#fff;font:800 12px system-ui;padding:8px 14px}
#mpMap{position:fixed;inset:0;z-index:20;background:#0b1016;pointer-events:auto;touch-action:none}#mpMap canvas{width:100%;height:100%;display:block}
#mpMap .bar{position:absolute;left:12px;right:12px;top:max(10px,env(safe-area-inset-top));display:flex;gap:8px}
#mpMap input{flex:1;min-width:0;padding:9px 12px;border-radius:10px;border:1px solid #ffffff30;background:#101820e8;color:#fff;font:600 14px system-ui}
#mpMap button{border-radius:10px;border:1px solid #ffffff30;background:#16222ee8;color:#fff;font:800 13px system-ui;padding:0 14px}
#mpMap ul{position:absolute;left:12px;right:12px;top:calc(max(10px,env(safe-area-inset-top)) + 46px);margin:0;padding:0;list-style:none;max-height:40vh;overflow:auto}
#mpMap li{padding:8px 12px;background:#101820f0;border-bottom:1px solid #ffffff14;font:600 13px system-ui;cursor:pointer}#mpMap li small{display:block;color:#9fb0c2}
#mpMap .info{position:absolute;left:12px;bottom:max(10px,env(safe-area-inset-bottom));padding:6px 10px;border-radius:8px;background:#070b10c4;font:700 12px system-ui;color:#dde6ef}
`;

export class MpPage {
  constructor({e, kern, command, params, canvas, statusEl}) {
    this.e = e; this.kern = kern; this.command = command; this.params = params; this.canvas = canvas; this.statusEl = statusEl;
    this.net = null; this.kind = ""; this.plates = new Map(); this.voice = null; this.map = null; this.lastFetch = 0; this.holds = new Map();
    const style = document.createElement("style"); style.textContent = MP_CSS; document.head.appendChild(style);
    this.root = document.createElement("div"); this.root.id = "mpHud";
    this.root.innerHTML = `<div id="mpPings"></div><i id="mpChev" hidden></i><div id="mpNav" hidden></div><div id="mpFeed"></div><div id="mpScore" hidden></div>
      <div id="mpHit"></div><div id="mpHurt"></div><div id="mpShield"><b>GESCHÜTZT</b></div><div id="mpRespawn" hidden><strong>ZERSTÖRT</strong><span></span></div>
      <div id="mpVote" hidden></div><div id="mpBar"><button data-a="multi">MULTI</button><button data-a="share" hidden>TEILEN</button><button data-a="mic" aria-pressed="false" hidden>🎤 MIC</button>
      <button data-a="mute" class="muted" aria-pressed="false" hidden>🔈</button><button data-a="all" aria-pressed="false" hidden>ÜBERALL</button><button data-a="map">KARTE</button></div>`;
    document.body.appendChild(this.root);
    const $ = s => this.root.querySelector(s);
    this.el = {pings: $("#mpPings"), chev: $("#mpChev"), nav: $("#mpNav"), feed: $("#mpFeed"), score: $("#mpScore"), hit: $("#mpHit"), hurt: $("#mpHurt"), shield: $("#mpShield"),
      respawn: $("#mpRespawn"), vote: $("#mpVote"), bar: $("#mpBar")};
    this.el.bar.addEventListener("click", ev => {
      const a = ev.target.closest("button")?.dataset.a;
      if (a === "mic") this.command("voice mic");
      else if (a === "mute") this.command("voice mute");
      else if (a === "all") this.command("voice all");
      else if (a === "map") this.command("bigmap toggle");
      else if (a === "share") this.showShare();
      else if (a === "multi") this.goMulti();
      ev.stopPropagation();
    });
    this.el.bar.addEventListener("pointerdown", ev => ev.stopPropagation());
    this.el.vote.addEventListener("click", ev => { const v = ev.target.closest("button")?.dataset.v; if (v) this.command(`mp vote ${v}`); });
  }

  // ---- connection --------------------------------------------------------------------------------
  async connect(KernNet) {
    this.KernNet = KernNet;
    const mp = this.params.get("mp");
    if (mp !== "host" && mp !== "join" && mp !== "auto") return;
    const e = this.e, kern = this.kern;
    const adapter = {
      netPush(from, bytes) {
        const ptr = e.kern_alloc(Math.max(1, bytes.length));
        new Uint8Array(kern.memory.buffer, ptr, bytes.length).set(bytes);
        e.sc_net_push(from % 4294967296, Math.floor(from / 4294967296), ptr, bytes.length);
        e.kern_free(ptr);
      },
      netTake() {
        const ptr = e.sc_net_take();
        if (!ptr) return null;
        const head = new Uint32Array(kern.memory.buffer, ptr, 3);
        return {to: head[1] + head[2] * 4294967296, bytes: new Uint8Array(kern.memory.buffer, ptr + 12, head[0]).slice()};
      },
    };
    const relay = this.params.get("relay") || "", room = this.params.get("room") || (this.params.get("net") === "public" ? MP_LOBBY : "spawncity");
    this.kind = relay ? "relay" : this.params.get("net") === "public" ? "public" : "local";
    this.room = room;
    this.adapter = adapter; this.KernNet = KernNet; this.relay = relay;
    this.setStatus = text => { this.statusEl.textContent = text; this.statusEl.hidden = !text; };
    try {
      await this.openTransport(mp);
      setInterval(() => this.net?.flush(), 16);
    } catch (error) {
      this.setStatus("Mehrspieler: " + (error?.message || error));
    }
  }
  async openTransport(mp) {
    let r;
    const room = this.room;
    if (this.kind === "public") {
      this.net = new MpPublicNet(this.adapter);
      this.net.onstatus = this.setStatus;
      r = await this.net.open({room, hosting: mp === "host", auto: mp === "auto"});
    } else {
      this.net = new this.KernNet(this.adapter);
      this.net.onstatus = this.setStatus;
      const open = hosting => this.net.open({kind: this.kind === "relay" ? "relay" : "local", room, hosting, url: this.relay});
      if (mp === "auto") r = await open(false).catch(() => open(true));  // nobody hosts: we do
      else r = await open(mp === "host");
    }
    this.hosting = !r.host;
    this.command(this.hosting ? `net host ${r.self}` : `net join ${r.self} ${r.host}`);
    window.SpawnCityNet = this.net;
  }
  /** lan_vs.mjs authority handover: the host left; the game says who takes over (1: we, 2: another). */
  async migrate(we) {
    if (this.migrating) return;
    this.migrating = true;
    this.setStatus?.(we ? "Host weg - du hostest jetzt" : "Host weg - verbinde neu ...");
    try { this.net?.close?.(); } catch { /* closed */ }
    try {
      if (we) await this.openTransport("host");
      else {
        // the successor needs a moment to open his room: try a few times, then host ourselves
        let ok = false;
        for (let i = 0; i < 4 && !ok; i++) {
          try { await new Promise(r => setTimeout(r, 600 + i * 900)); await this.openTransport("join"); ok = true; } catch { try { this.net?.close?.(); } catch { /* closed */ } }
        }
        if (!ok) await this.openTransport("host");
      }
      this.setStatus("");
    } catch (error) {
      this.setStatus("Mehrspieler: " + (error?.message || error));
    } finally { this.migrating = false; }
  }
  joinUrl() {
    const u = new URL(location.href);
    for (const k of ["mp", "room", "net", "relay", "offline", "start"]) u.searchParams.delete(k);
    u.searchParams.set("mp", "join");
    u.searchParams.set("room", this.room || "spawncity");
    if (this.kind === "public") u.searchParams.set("net", "public");
    if (this.kind === "relay") u.searchParams.set("relay", this.params.get("relay"));
    if (this.params.get("offline")) u.searchParams.set("offline", this.params.get("offline"));
    return u.href;
  }
  /** mobile_gameplay_ui.mjs multiplayerSnapshot: the MULTI button's state and its status line. */
  uiState(h) {
    const others = Array.isArray(h?.mp?.players) ? h.mp.players.filter(p => !p.me).length : 0;
    if (this.connecting) return {state: "finding", label: "SUCHE …", status: "Suche Mitspieler in der Lobby …"};
    if (!this.net) return {state: this.failed ? "error" : "offline", label: "MULTI", status: this.failed || ""};
    const brokers = this.kind === "public" ? (this.net.clients?.filter(c => c.open).length || 0) : 1;
    if (!brokers && performance.now() - (this.openedAt || 0) > 6000) return {state: "error", label: "MULTI ⚠", status: "Keine Verbindung · tippen = neu versuchen"};
    if (others > 0) return {state: "connected", label: `MULTI · ${others + 1}`, status: `${others + 1} Spieler · ${this.hosting ? "du hostest" : "verbunden"}`};
    return {state: "waiting", label: "MULTI · 1", status: this.hosting ? "Lobby offen · warte auf Mitspieler" : "verbunden · warte auf Mitspieler"};
  }
  /** MULTI again: a lobby that does not connect is tried anew; in a lobby it is left (lan_vs toggleVs). */
  toggleMulti(h) {
    const st = this.uiState(h).state;
    if (this.connecting) return;
    if (!this.net) { this.goMulti(); return; }
    try { this.net.close?.(); } catch { /* closed */ }
    this.net = null;
    window.SpawnCityNet = null;
    this.command("net stop");
    const u = new URL(location.href);
    for (const k of ["mp", "net", "room"]) u.searchParams.delete(k);
    try { history.replaceState(history.state, "", u.href); } catch { /* the address stays */ }
    this.params = u.searchParams;
    if (st === "error") this.goMulti();
  }
  goMulti() {
    // MULTI (lan_vs.mjs): into the one public lobby - join it, or host it when nobody is there -
    // right here, the game goes on (no reload: a reload threw the player back to the title)
    if (this.net || this.connecting) return;
    this.failed = "";
    const u = new URL(location.href);
    u.searchParams.set("mp", "auto"); u.searchParams.set("net", "public"); u.searchParams.set("room", MP_LOBBY);
    try { history.replaceState(history.state, "", u.href); } catch { /* the address stays */ }
    this.params = u.searchParams;
    this.connecting = true;
    this.connect(this.KernNet).then(() => { this.openedAt = performance.now(); if (!this.net) this.failed = "Mehrspieler nicht verfügbar · tippen = neu versuchen"; })
      .catch(err => { this.failed = "Fehler: " + (err?.message || err); }).finally(() => { this.connecting = false; });
  }
  showShare() {
    // TEILEN (share_button.mjs) with a QR code to join (the game encodes it: sc QR encoder).
    const url = this.joinUrl();
    let qr = null;
    try { qr = JSON.parse(this.command("qr " + url)); } catch { /* no code */ }
    const box = document.createElement("div"); box.id = "mpShare";
    box.innerHTML = `<div><b>Mitspielen: Code scannen</b><br><canvas width="1" height="1"></canvas><p>${mpEsc(url)}</p>
      <p>${this.kind === "local" ? "Nur Tabs dieses Browsers (BroadcastChannel). Für andere Geräte: MULTI (öffentlich)." : this.kind === "public" ? "Öffentlich über WebRTC / MQTT-Broker (verschlüsselt)." : "Über das Kern-Relay."}</p>
      <button data-s="share">TEILEN</button><button data-s="close">SCHLIESSEN</button></div>`;
    document.body.appendChild(box);
    const c = box.querySelector("canvas");
    if (qr?.size) {
      const n = qr.size + 8; c.width = c.height = n;
      const g = c.getContext("2d"); g.fillStyle = "#fff"; g.fillRect(0, 0, n, n); g.fillStyle = "#000";
      qr.rows.forEach((row, y) => { for (let x = 0; x < row.length; x++) if (row[x] === "1") g.fillRect(x + 4, y + 4, 1, 1); });
    }
    box.addEventListener("click", async ev => {
      const s = ev.target.closest("button")?.dataset.s;
      if (s === "close" || ev.target === box) box.remove();
      if (s === "share") {
        const text = "SpawnCity – spiel mit mir in der echten Stadt:";
        try { if (navigator.share) { await navigator.share({title: "SpawnCity", text, url}); return; } } catch (err) { if (err?.name === "AbortError") return; }
        try { await navigator.clipboard.writeText(url); ev.target.textContent = "LINK KOPIERT"; } catch { /* no clipboard */ }
      }
    });
    window.__scShareQr = qr;
  }

  // ---- touch long press = ping (ping_marker.mjs HOLD_MS 600, slop 12 px) ---------------------------
  touch(type, id, x, y, used) {
    if (type === 0) {
      if (used) return;
      const hold = {x, y, timer: setTimeout(() => { this.holds.delete(id); if (this.command(`ping ${x.toFixed(0)} ${y.toFixed(0)}`) === "set") navigator.vibrate?.(25); }, 600)};
      this.holds.set(id, hold);
    } else {
      const h = this.holds.get(id);
      if (!h) return;
      if (type !== 1 || Math.hypot(x - h.x, y - h.y) > 12) { clearTimeout(h.timer); this.holds.delete(id); }
    }
  }

  // ---- per frame -----------------------------------------------------------------------------------
  update(h) {
    if (this.net && !this.migrating && (this.migrateCheck = (this.migrateCheck || 0) + 1) % 15 === 0) {
      const mg = this.command("net migrate");
      if (mg === "1" || mg === "2") this.migrate(mg === "1");
    }
    const m = h?.mp;
    if (!m) return;
    const net = m.self !== undefined;
    const $ = this.el;
    // voice bar
    const bar = $.bar.children;
    bar[0].hidden = net; bar[1].hidden = !net; bar[2].hidden = !net; bar[3].hidden = !net; bar[4].hidden = !net;
    if (net) {
      bar[2].setAttribute("aria-pressed", String(m.voice.mic && !m.voice.muted)); bar[2].textContent = m.voice.mic ? "🎙 MIC AN" : "🎤 MIC";
      bar[3].setAttribute("aria-pressed", String(m.voice.muted)); bar[3].textContent = m.voice.muted ? "🔇" : "🔈";
      bar[4].setAttribute("aria-pressed", String(m.voice.all));
    }
    this.updateVoice(net && m.voice.mic && !m.voice.muted);
    // pings
    this.sync($.pings, m.pings, (el, p) => {
      el.hidden = !p.on; el.style.left = p.x + "px"; el.style.top = p.y + "px"; el.style.setProperty("--c", p.color); el.style.opacity = p.fade;
      const t = `📍 ${p.d} m`; if (el.textContent !== t) el.textContent = t;
    });
    // route guidance
    $.nav.hidden = !m.nav; $.chev.hidden = !m.nav;
    if (m.nav) {
      const t = `${m.nav.arrow} ${m.nav.dist} · ${m.nav.name}${m.nav.turn ? " · " + m.nav.turn : ""}`;
      if ($.nav.textContent !== t) $.nav.textContent = t;
      $.chev.style.transform = `translate(${m.nav.x - 11}px,${m.nav.y - 8}px) rotate(${m.nav.on ? 0 : m.nav.angle + 180}deg)`;
    }
    if (m.navFetch && m.navFetch.id !== this.lastFetch) this.fetchRoute(m.navFetch);
    // the big map
    if (m.mapOpen && !this.map) this.map = new MpBigMap(this);
    if (!m.mapOpen && this.map) { this.map.close(); this.map = null; }
    this.map?.frame();
    if (!net) { for (const el of [$.feed, $.score, $.vote, $.respawn]) el.hidden = true; this.plates.forEach(p => p.remove()); this.plates.clear(); return; }
    // nameplates (vs_multiplayer.mjs renderMarker)
    const seen = new Set();
    for (const p of m.players) {
      seen.add(p.slot);
      let el = this.plates.get(p.slot);
      if (!el) {
        el = document.createElement("div"); el.className = "mp-plate";
        el.innerHTML = '<i class="ar"></i><span class="t"><strong></strong><small></small><span class="mic">🎙</span></span><b class="hp"><u></u></b>';
        this.root.appendChild(el); this.plates.set(p.slot, el);
      }
      el.style.setProperty("--c", p.color);
      el.classList.toggle("off", !p.on); el.classList.toggle("dead", p.dead); el.classList.toggle("talk", p.talk);
      const sc = p.on ? Math.max(.82, Math.min(1.15, 1.25 - p.d / 160)) : 1;
      el.style.transform = `translate3d(${p.x}px,${p.y}px,0) translate(-50%,${p.on ? "-100%" : "-50%"}) scale(${sc.toFixed(3)})`;
      el.style.setProperty("--a", `${p.angle}deg`);
      const name = p.dead ? `P${p.index} DOWN` : `P${p.index}`, d = p.d < 10 ? `${p.d.toFixed(1)} m` : p.d < 1000 ? `${Math.round(p.d)} m` : `${(p.d / 1000).toFixed(1)} km`;
      el.querySelector("strong").textContent = name; el.querySelector("small").textContent = d; el.querySelector(".hp u").style.width = `${p.hp}%`;
    }
    for (const [slot, el] of this.plates) if (!seen.has(slot)) { el.remove(); this.plates.delete(slot); }
    // kill feed and board (mp_fun_hud.mjs)
    const who = (i, c, self) => `<b style="color:${c}">${self ? "DU" : `P${i}`}</b>`;
    const feed = m.feed.filter(f => f.age < 5200).map(f => `<div class="${f.bySelf || f.victimSelf ? "me" : ""}">${f.accident ? `${who(f.victim, f.victimColor, f.victimSelf)}<i>✕</i>${f.weapon || "UNFALL"}`
      : `${who(f.by, f.byColor, f.bySelf)}<i>${f.weapon || "✕"}</i>${who(f.victim, f.victimColor, f.victimSelf)}${f.head ? "<em>KOPF</em>" : ""}`}</div>`).join("");
    if (feed !== this.feedKey) { $.feed.innerHTML = feed; this.feedKey = feed; }
    const board = m.scores.map(s => `<span class="${s.self ? "me" : ""}"><span><u style="background:${s.color}"></u>${s.self ? "DU" : `P${s.index}`}</span><span>${s.k} / ${s.d}</span></span>`).join("");
    if (board !== this.boardKey) { $.score.innerHTML = board; this.boardKey = board; }
    $.score.hidden = m.scores.length < 1;
    $.hit.className = m.hitShow ? `show${m.hitKill ? " kill" : ""}${m.hitHead ? " head" : ""}` : "";
    $.hurt.style.setProperty("--a", `${m.hurtAngle}deg`); $.hurt.className = m.hurt ? "show" : "";
    $.shield.className = m.shield ? "show" : "";
    $.respawn.hidden = m.respawnIn === undefined;
    if (m.respawnIn !== undefined) $.respawn.querySelector("span").textContent = m.respawnIn > 0 ? `RESPAWN IN ${Math.ceil(m.respawnIn)}` : "RESPAWN ...";
    const vote = m.votePending ? `${mpEsc(m.vote)}<button data-v="yes">JA</button><button data-v="no">NEIN</button>` : mpEsc(m.vote);
    $.vote.hidden = !m.votePending && !/Abstimmung|angenommen|Abgelehnt|gesendet/i.test(m.vote);
    if (vote !== this.voteKey) { $.vote.innerHTML = vote; this.voteKey = vote; }
  }
  sync(root, list, fill) {
    while (root.children.length < list.length) root.appendChild(document.createElement("span"));
    while (root.children.length > list.length) root.lastChild.remove();
    list.forEach((p, i) => fill(root.children[i], p));
  }

  // ---- microphone (proximity_voice.mjs: opened only while the mic is on, closed when off) ---------
  updateVoice(on) {
    if (on && !this.voice) {
      this.voice = {stopped: false};
      const v = this.voice;
      navigator.mediaDevices?.getUserMedia?.({audio: {echoCancellation: true, noiseSuppression: true, autoGainControl: true}, video: false}).then(stream => {
        if (v.stopped) { stream.getTracks().forEach(t => t.stop()); return; }
        const ctx = new (window.AudioContext || window.webkitAudioContext)();
        const src = ctx.createMediaStreamSource(stream), proc = ctx.createScriptProcessor(2048, 1, 1), silent = ctx.createGain();
        silent.gain.value = 0;
        src.connect(proc); proc.connect(silent); silent.connect(ctx.destination);
        proc.onaudioprocess = ev => {
          const input = ev.inputBuffer.getChannelData(0), e = this.e;
          const ptr = e.kern_alloc(input.length * 4);
          new Float32Array(this.kern.memory.buffer, ptr, input.length).set(input);
          e.sc_voice_push(ptr, input.length, ev.inputBuffer.sampleRate);
          e.kern_free(ptr);
        };
        Object.assign(v, {stream, ctx, src, proc});
      }).catch(() => { this.command("voice off"); this.statusEl.textContent = "Mikrofon nicht freigegeben"; this.statusEl.hidden = false; setTimeout(() => { this.statusEl.hidden = true; }, 2500); });
    } else if (!on && this.voice) {
      const v = this.voice; this.voice = null; v.stopped = true;
      v.stream?.getTracks().forEach(t => t.stop()); v.proc?.disconnect(); v.src?.disconnect(); v.ctx?.close();
    }
  }

  // ---- road route (world_map_full.mjs requestRoute) ------------------------------------------------
  fetchRoute(r) {
    this.lastFetch = r.id;
    this.routeAbort?.abort();
    const ctl = new AbortController(); this.routeAbort = ctl;
    const url = `${MP_ROUTER}/routed-${r.profile}/route/v1/driving/${r.from[0].toFixed(6)},${r.from[1].toFixed(6)};${r.to[0].toFixed(6)},${r.to[1].toFixed(6)}?overview=full&geometries=geojson`;
    fetch(url, {signal: ctl.signal}).then(res => { if (!res.ok) throw Error("route " + res.status); return res.json(); }).then(json => {
      let c = json?.routes?.[0]?.geometry?.coordinates;
      if (!Array.isArray(c) || c.length < 2) throw Error("no route");
      const step = Math.max(1, Math.ceil(c.length / 800));
      c = c.filter((_, i) => i % step === 0 || i === c.length - 1);
      this.command(`nav route ${r.id} ` + c.map(p => `${(+p[0]).toFixed(6)},${(+p[1]).toFixed(6)}`).join(" "));
    }).catch(() => { if (!ctl.signal.aborted) this.command("nav failed"); });
  }
}

// ---- the big map (world_map_full.mjs: GTA pause-map style; the game sends what it knows) -----------
class MpBigMap {
  constructor(page) {
    this.page = page; this.zoom = 1.2; this.follow = true; this.cx = 0; this.cy = 0; this.data = null; this.last = 0; this.pointers = new Map();
    const el = document.createElement("div"); el.id = "mpMap";
    el.innerHTML = `<canvas></canvas><div class="bar"><input type="search" placeholder="Adresse oder Ort suchen" enterkeyhint="search"><button data-b="me">◎</button><button data-b="clear">ZIEL ✕</button><button data-b="close">✕</button></div><ul></ul>
      <div class="info">Tippen: Wegpunkt · Ziehen: verschieben · Rad / Pinch: Zoom · M / Esc: schließen · © OpenStreetMap-Mitwirkende · Suche: Photon · Routen: FOSSGIS</div>`;
    document.body.appendChild(el);
    document.exitPointerLock?.();
    this.el = el; this.cv = el.querySelector("canvas"); this.g = this.cv.getContext("2d"); this.list = el.querySelector("ul"); this.input = el.querySelector("input");
    el.querySelector(".bar").addEventListener("click", ev => {
      const b = ev.target.closest("button")?.dataset.b;
      if (b === "close") page.command("bigmap close");
      if (b === "clear") page.command("nav clear");
      if (b === "me") this.follow = true;
    });
    this.input.addEventListener("keydown", ev => { ev.stopPropagation(); if (ev.key === "Escape") page.command("bigmap close"); });
    this.onSlash = ev => { if (ev.code === "Slash" && document.activeElement !== this.input) { ev.preventDefault(); this.input.focus(); } };
    addEventListener("keydown", this.onSlash, true);
    this.input.addEventListener("input", () => { clearTimeout(this.searchTimer); this.searchTimer = setTimeout(() => this.search(), 350); });
    this.cv.addEventListener("wheel", ev => { this.zoom = Math.max(0.05, Math.min(12, this.zoom * Math.exp(-ev.deltaY * 0.0015))); ev.preventDefault(); }, {passive: false});
    this.cv.addEventListener("pointerdown", ev => { this.pointers.set(ev.pointerId, {x: ev.clientX, y: ev.clientY, sx: ev.clientX, sy: ev.clientY}); this.cv.setPointerCapture(ev.pointerId); });
    this.cv.addEventListener("pointermove", ev => {
      const p = this.pointers.get(ev.pointerId); if (!p) return;
      if (this.pointers.size === 2) {
        const [a, b] = [...this.pointers.values()], d0 = Math.hypot(a.x - b.x, a.y - b.y);
        p.x = ev.clientX; p.y = ev.clientY;
        const d1 = Math.hypot(a.x - b.x, a.y - b.y); if (d0 > 1) this.zoom = Math.max(0.05, Math.min(12, this.zoom * d1 / d0));
        return;
      }
      this.cx -= (ev.clientX - p.x) / this.zoom; this.cy += (ev.clientY - p.y) / this.zoom; p.x = ev.clientX; p.y = ev.clientY;
      if (Math.hypot(p.x - p.sx, p.y - p.sy) > 8) this.follow = false;
    });
    this.cv.addEventListener("pointerup", ev => {
      const p = this.pointers.get(ev.pointerId); this.pointers.delete(ev.pointerId);
      if (p && Math.hypot(ev.clientX - p.sx, ev.clientY - p.sy) < 8 && this.pointers.size === 0) {
        const [x, y] = this.toWorld(ev.clientX, ev.clientY);
        page.command(`nav set ${x.toFixed(1)} ${y.toFixed(1)} Wegpunkt`);
        this.last = 0;
      }
    });
  }
  toWorld(px, py) { const w = this.cv.clientWidth, h = this.cv.clientHeight; return [this.cx + (px - w / 2) / this.zoom, this.cy - (py - h / 2) / this.zoom]; }
  async search() {
    const q = this.input.value.trim(), d = this.data;
    if (q.length < 3) { this.list.innerHTML = ""; return; }
    const seq = (this.seq = (this.seq || 0) + 1);
    try {
      const u = `${MP_PHOTON}?q=${encodeURIComponent(q)}&limit=6&lang=de${d?.real ? `&lat=${d.lat}&lon=${d.lon}` : ""}`;
      const json = await (await fetch(u)).json();
      if (seq !== this.seq) return;
      const items = (json?.features || []).map(f => ({lon: f.geometry.coordinates[0], lat: f.geometry.coordinates[1], name: f.properties.name || f.properties.street || q,
        sub: [f.properties.street && f.properties.housenumber ? `${f.properties.street} ${f.properties.housenumber}` : "", f.properties.city || f.properties.town || "", f.properties.country || ""].filter(Boolean).join(", ")}));
      this.list.innerHTML = items.map((it, i) => `<li data-i="${i}">${mpEsc(it.name)}<small>${mpEsc(it.sub)}</small></li>`).join("");
      this.list.onclick = ev => {
        const it = items[+ev.target.closest("li")?.dataset.i];
        if (!it) return;
        this.page.command(`nav geo ${it.lon} ${it.lat} ${it.name.replace(/\s+/g, " ").slice(0, 80)}`);
        this.list.innerHTML = ""; this.input.value = ""; this.follow = true; this.last = 0;
      };
    } catch { this.list.innerHTML = "<li>Suche nicht erreichbar</li>"; }
  }
  frame() {
    const now = performance.now();
    this.padInput();
    if (now - this.last > 250) {
      this.last = now;
      const range = Math.max(this.cv.clientWidth, this.cv.clientHeight) / this.zoom * 0.6 + 50;
      try { this.data = JSON.parse(this.page.command(this.follow ? `bigmap follow ${Math.min(3000, range).toFixed(0)}` : `bigmap ${this.cx.toFixed(0)} ${this.cy.toFixed(0)} ${Math.min(3000, range).toFixed(0)}`)); } catch { /* keep */ }
      if (this.follow && this.data?.me) { this.cx = this.data.me[0]; this.cy = this.data.me[1]; }
    }
    this.draw();
  }
  padInput() {
    // world_map_full.mjs:355-362: LS pans -s|s| 900 px/s (dead zone .15, rescaled), RT/LT zoom
    // (rt - lt) 2.2 /s, A waypoint at the centre, X clears it, Y search, D-pad left/right zoom -1/+1,
    // R3 follows me, B or VIEW closes (VIEW held when the map opened does not close it).
    const p = navigator.getGamepads ? [...navigator.getGamepads()].find(g => g?.connected) : null;
    const now = performance.now(), dt = Math.min(0.05, Math.max(0, (now - (this.padAt || now)) / 1000));
    this.padAt = now;
    if (!p) return;
    const ax = i => { const v = +p.axes[i] || 0, a = Math.abs(v); return a <= 0.15 ? 0 : Math.sign(v) * (a - 0.15) / 0.85; }, b = i => !!p.buttons[i]?.pressed;
    const sx = ax(0), sy = ax(1);
    if (sx || sy) { this.cx += sx * Math.abs(sx) * 900 * dt / this.zoom; this.cy -= sy * Math.abs(sy) * 900 * dt / this.zoom; this.follow = false; }
    const tz = (p.buttons[7]?.value || 0) - (p.buttons[6]?.value || 0);
    if (tz) this.zoom = Math.max(0.05, Math.min(12, this.zoom * Math.exp(tz * 2.2 * dt)));
    if (!this.padWas) this.padWas = p.buttons.map(x => x.pressed);  // buttons held as the map opened
    const pressed = i => b(i) && !this.padWas[i];
    if (pressed(0)) this.page.command(`nav set ${this.cx.toFixed(1)} ${this.cy.toFixed(1)} Wegpunkt`);
    if (pressed(2)) this.page.command("nav clear");
    if (pressed(3)) this.input.focus();
    if (pressed(14)) this.zoom = Math.max(0.05, this.zoom / 2);
    if (pressed(15)) this.zoom = Math.min(12, this.zoom * 2);
    if (pressed(11)) this.follow = true;
    if (pressed(1) || pressed(8)) this.page.command("bigmap close");
    this.padWas = p.buttons.map(x => x.pressed);
  }
  draw() {
    const cv = this.cv, g = this.g, dpr = Math.min(2, devicePixelRatio || 1), w = cv.clientWidth, h = cv.clientHeight;
    if (cv.width !== Math.round(w * dpr)) { cv.width = Math.round(w * dpr); cv.height = Math.round(h * dpr); }
    g.setTransform(dpr, 0, 0, dpr, 0, 0);
    g.fillStyle = "#0b1016"; g.fillRect(0, 0, w, h);
    const d = this.data; if (!d?.me) return;
    const X = x => w / 2 + (x - this.cx) * this.zoom, Y = y => h / 2 - (y - this.cy) * this.zoom;
    g.strokeStyle = "#cfd7e0"; g.lineCap = g.lineJoin = "round"; g.lineWidth = Math.max(1.2, 7 * this.zoom); g.globalAlpha = .75;
    for (const l of d.lanes) { g.beginPath(); for (let i = 0; i < l.length; i += 2) (i ? g.lineTo : g.moveTo).call(g, X(l[i]), Y(l[i + 1])); g.stroke(); }
    g.globalAlpha = 1;
    if (d.route?.length >= 4) {
      g.strokeStyle = "#ffd76a"; g.lineWidth = Math.max(3, 4 * this.zoom); g.setLineDash([10, 6]); g.beginPath();
      for (let i = 0; i < d.route.length; i += 2) (i ? g.lineTo : g.moveTo).call(g, X(d.route[i]), Y(d.route[i + 1]));
      g.stroke(); g.setLineDash([]);
    }
    const dot = (x, y, c, r) => { g.fillStyle = c; g.beginPath(); g.arc(x, y, r, 0, Math.PI * 2); g.fill(); };
    if (d.dest) { dot(X(d.dest.x), Y(d.dest.y), "#ffd76a", 8); g.fillStyle = "#fff"; g.font = "700 12px system-ui"; g.fillText(d.dest.name, X(d.dest.x) + 11, Y(d.dest.y) + 4); }
    if (d.jet) dot(X(d.jet[0]), Y(d.jet[1]), "#7fd3ff", 6);
    for (const p of d.pings) { g.fillStyle = p.color; g.save(); g.translate(X(p.x), Y(p.y)); g.rotate(Math.PI / 4); g.fillRect(-6, -6, 12, 12); g.restore(); }
    // Players; one off the map sits at its edge with an arrow toward him (map_peer_edge.mjs).
    for (const p of d.players) {
      let x = X(p.x), y = Y(p.y), off = false;
      const ix = 16, it = 60, ib = 40;
      if (x < ix || x > w - ix || y < it || y > h - ib) {
        const cx = w / 2, cy = (it + h - ib) / 2, dx = x - cx, dy = y - cy;
        const t = Math.min(dx ? ((dx > 0 ? w - ix : ix) - cx) / dx : Infinity, dy ? ((dy > 0 ? h - ib : it) - cy) / dy : Infinity);
        x = cx + dx * t; y = cy + dy * t; off = true;
      }
      dot(x, y, p.dead ? "#777" : p.color, off ? 6 : 7);
      g.fillStyle = "#fff"; g.font = "800 11px system-ui"; g.fillText(`P${p.index}${off ? " ›" : ""}`, x + 9, y + 4);
    }
    // Me: an arrow along the view.
    g.save(); g.translate(X(d.me[0]), Y(d.me[1])); g.rotate(d.me[2]);
    g.fillStyle = "#ffd23e"; g.beginPath(); g.moveTo(0, -13); g.lineTo(9, 10); g.lineTo(0, 5); g.lineTo(-9, 10); g.closePath(); g.fill(); g.restore();
    // scale bar
    const m = [10, 20, 50, 100, 200, 500, 1000, 2000, 5000].find(v => v * this.zoom > 70) || 5000;
    g.fillStyle = "#fff"; g.fillRect(w - 20 - m * this.zoom, h - 30, m * this.zoom, 3); g.font = "700 11px system-ui"; g.fillText(m >= 1000 ? `${m / 1000} km` : `${m} m`, w - 20 - m * this.zoom, h - 36);
  }
  close() { removeEventListener("keydown", this.onSlash, true); this.el.remove(); }
}
