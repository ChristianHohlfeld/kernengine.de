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
import {readGamepad, wheelPixels} from "../../../web/kern.mjs";
import {WorldBasemap, mercX, mercY, lonOf, latOf} from "./world_map.mjs";

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

// ---- Nostr relays (nostr_data_relay.mjs): a second public way to find each other - the same
// hello / offer / answer / ice as over the brokers, as signed ephemeral events (kind 25045, tag the
// room, expiring after 30 s; BIP-340 in the game: "nostr event", nostr.cpp). Signalling only: the
// data itself goes over the WebRTC channel (or the brokers), never as one event a packet.
const MP_NOSTR = ["wss://relay-rpi.edufeed.org", "wss://relay2.angor.io", "wss://top.testrelay.top", "wss://relay.primal.net"];
const MP_NOSTR_KIND = 25045;
class MpNostr {
  constructor(url, topics, onMessage, sign) {
    this.url = url; this.topics = topics; this.onMessage = onMessage; this.sign = sign; this.open = false; this.signalOnly = true;
    this.sub = "sc" + Math.random().toString(36).slice(2, 10);
    try { this.ws = new WebSocket(url); } catch { this.ws = null; return; }
    this.ws.onopen = () => {
      this.open = true;
      this.ws.send(JSON.stringify(["REQ", this.sub, {kinds: [MP_NOSTR_KIND], "#t": this.topics, since: Math.floor(Date.now() / 1000) - 3}]));
    };
    this.ws.onmessage = ev => {
      let m;
      try { m = JSON.parse(ev.data); } catch { return; }
      if (m[0] !== "EVENT" || m[1] !== this.sub || !m[2]?.content) return;
      const t = (m[2].tags || []).find(x => x[0] === "t")?.[1];
      if (t && this.topics.includes(t)) this.onMessage(t, String(m[2].content));
    };
    this.ws.onclose = this.ws.onerror = () => { this.open = false; };
  }
  signEvent(topic, text) {
    if (!this.open || text.length > 30000) return null;
    const now = Math.floor(Date.now() / 1000);
    return this.sign(now, [["t", topic], ["expiration", String(now + 30)]], text);
  }
  sendSigned(ev) { if (!this.open) return false; try { this.ws.send(`["EVENT",${ev}]`); } catch { return false; } return true; }
  publish(topic, text) { const ev = this.signEvent(topic, text); return ev ? this.sendSigned(ev) : false; }
  close() { try { this.ws?.send(JSON.stringify(["CLOSE", this.sub])); this.ws?.close(); } catch { /* closed */ } }
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
    // Nostr only on request (?nostr=1): a BIP-340 signature costs ~140 ms on a desktop and several
    // hundred on a phone - signed for every hello it froze the game every second
    if (this.sign && this.nostr) for (const url of MP_NOSTR) this.clients.push(new MpNostr(url, [this.topic], (t, text) => this.onBroker(text), this.sign));
    this.hello = setInterval(() => this.announce(), MP_HELLO_MS);
    this.timer = setInterval(() => this.flush(), 16);
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
    // the Nostr relays: no data, the hello only every 10 s, one signature for all relays
    const nostr = !packet.bulk && (packet.kind !== "hello" || Date.now() - (this.nostrHelloAt || 0) > 10000);
    if (nostr && packet.kind === "hello") this.nostrHelloAt = Date.now();
    let signed = null;
    for (const c of this.clients) {
      if (!c.signalOnly) { c.publish(this.topic, text); continue; }
      if (!nostr) continue;
      signed ??= c.signEvent(this.topic, text) || "";
      if (signed) c.sendSigned(signed);
    }
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
  async seal(p, value, bulk = false) {
    const iv = crypto.getRandomValues(new Uint8Array(12)), plain = new TextEncoder().encode(JSON.stringify(value));
    const box = new Uint8Array(await crypto.subtle.encrypt({name: "AES-GCM", iv}, p.key, plain));
    this.publish({kind: "sealed", target: p.id, iv: mpB64(iv), box: mpB64(box), bulk});
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
    if (value.t === "data") {
      if (Array.isArray(value.ds)) for (const d of value.ds) this.deliver(p.id, mpUnB64(d));
      else this.deliver(p.id, mpUnB64(value.d));
      return;
    }
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
    // without a direct channel the packets of this flush go out as one sealed relay message per
    // peer (one AES-GCM and one broker publish, not one per packet)
    let relay = null;
    for (let guard = 0; guard < 256; guard++) {
      const d = this.engine.netTake();
      if (!d) break;
      this.sent++;
      const p = this.peers.get(d.to);
      if (!p) continue;
      if (p.dc?.readyState === "open") { try { p.dc.send(d.bytes); continue; } catch { /* fall back */ } }
      relay ??= new Map();
      if (!relay.has(p)) relay.set(p, []);
      relay.get(p).push(mpB64(d.bytes));
    }
    if (relay) for (const [p, ds] of relay) this.seal(p, {t: "data", ds}, true);  // encrypted relay over the brokers (not the Nostr relays)
  }
  status() {
    const open = this.clients?.filter(c => c.open && !c.signalOnly).length || 0, nostr = this.clients?.filter(c => c.open && c.signalOnly).length || 0;
    return `${this.mode} · ${open}/${MP_BROKERS.length} Broker · ${nostr}/${MP_NOSTR.length} Nostr`;
  }
  close() { clearInterval(this.hello); clearInterval(this.timer); for (const p of this.peers.values()) p.pc?.close(); for (const c of this.clients || []) c.close(); this.clients = null; }
}

// ---- the multiplayer page parts ------------------------------------------------------------------
const MP_CSS = `
#mpHud{position:fixed;inset:0;pointer-events:none;z-index:5;font-family:Inter,system-ui,sans-serif;color:#fff}
/* the mates (vs_multiplayer.mjs renderMarker, redone): in view a plate over his head - name, distance,
   above / below - with a caret on him; out of view a badge at the edge with a big arrow toward him,
   the distance, how much higher or lower, and the side in words (LINKS / RECHTS / HINTER DIR) */
.mp-plate{--c:#fff;--a:0deg;font-family:Inter,system-ui,sans-serif;color:#fff;position:absolute;left:0;top:0;pointer-events:none;will-change:transform;filter:drop-shadow(0 1px 2px #000d)}
.mp-plate .on{display:flex;flex-direction:column;align-items:center;gap:2px}
.mp-plate .t{display:flex;gap:6px;align-items:center;padding:3px 9px;border-radius:9px;background:#060b10cc;border:2px solid var(--c);white-space:nowrap}
.mp-plate strong{font:900 14px/1 system-ui;letter-spacing:.06em;color:var(--c)}.mp-plate small{font:800 12px/1 system-ui;color:#fff}
.mp-plate .el{font:900 12px/1 system-ui;color:#fff;padding:1px 5px;border-radius:6px;background:var(--c);color:#06090d}.mp-plate .el:empty{display:none}
.mp-plate .st{font:900 12px/1 system-ui;color:#ffd24a;letter-spacing:.04em}.mp-plate .st:empty{display:none}
.mp-plate .hp{width:48px;height:4px;border-radius:2px;background:#ffffff30;overflow:hidden}.mp-plate .hp u{display:block;height:100%;background:var(--c)}
.mp-plate .caret{width:0;height:0;border-left:9px solid transparent;border-right:9px solid transparent;border-top:12px solid var(--c)}
.mp-plate .edge{display:none;position:relative;width:54px;height:54px}
.mp-plate .edge .ring{position:absolute;inset:0;border-radius:50%;background:#060b10d9;border:3px solid var(--c);box-shadow:0 0 0 3px #0008,0 0 14px var(--c);display:flex;align-items:center;justify-content:center;font:900 15px/1 system-ui;color:var(--c);letter-spacing:.04em}
.mp-plate .edge .arrow{position:absolute;left:50%;top:50%;width:0;height:0;margin:-14px 0 0 -12px;border-left:12px solid transparent;border-right:12px solid transparent;border-bottom:20px solid var(--c);transform:rotate(var(--a)) translateY(-44px);transform-origin:12px 14px;filter:drop-shadow(0 0 4px var(--c))}
.mp-plate .edge .lab{position:absolute;left:50%;top:60px;transform:translateX(-50%);display:flex;flex-direction:column;align-items:center;gap:2px;white-space:nowrap}
.mp-plate .edge .lab b{font:900 12px/1 system-ui;color:#fff;padding:2px 6px;border-radius:6px;background:#060b10cc;border:1.5px solid var(--c)}
.mp-plate .edge .lab i{font:900 11px/1 system-ui;font-style:normal;letter-spacing:.08em;color:var(--c)}
.mp-plate.low .edge .lab{top:auto;bottom:60px;flex-direction:column-reverse}
.mp-plate.off .on{display:none}.mp-plate.off .edge{display:block}
.mp-plate.off.near .edge .ring{animation:mpNear 1s ease-in-out infinite}@keyframes mpNear{50%{box-shadow:0 0 0 3px #0008,0 0 24px var(--c)}}
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
#mpMap{position:fixed;inset:0;z-index:80;background:#0b1016;pointer-events:auto;touch-action:none}#mpMap canvas{width:100%;height:100%;display:block}
#mpMap .bar{position:absolute;left:12px;right:190px;top:max(10px,env(safe-area-inset-top));display:flex;gap:8px}
#mpMap input{flex:1;min-width:0;padding:9px 12px;border-radius:10px;border:1px solid #ffffff30;background:#101820e8;color:#fff;font:600 14px system-ui}
#mpMap button{border-radius:10px;border:1px solid #ffffff30;background:#16222ee8;color:#fff;font:800 13px system-ui;padding:0 14px}
#mpMap ul{position:absolute;left:12px;right:12px;top:calc(max(10px,env(safe-area-inset-top)) + 46px);margin:0;padding:0;list-style:none;max-height:40vh;overflow:auto}
#mpMap li{padding:8px 12px;background:#101820f0;border-bottom:1px solid #ffffff14;font:600 13px system-ui;cursor:pointer}#mpMap li small{display:block;color:#9fb0c2}
#mpMap .info{position:absolute;left:12px;bottom:max(10px,env(safe-area-inset-bottom));max-width:calc(100% - 190px);padding:6px 10px;border-radius:8px;background:#070b10c4;font:700 12px system-ui;color:#dde6ef}#mpMap .info a{color:#9fd0ff}
`;

// The microphone: asked for on a tap (a browser asks only inside a user gesture, a headset not at
// all once in VR) - __scMicPrime() on the mic button and the VR button takes the stream then; the
// voice chat uses that one (or asks itself when it opens without a primed one).
let primed = null;
const MIC = {audio: {echoCancellation: true, noiseSuppression: true, autoGainControl: true}, video: false};
function micStream() {
  if (primed) { const p = primed; primed = null; return p; }
  return navigator.mediaDevices?.getUserMedia ? navigator.mediaDevices.getUserMedia(MIC) : Promise.reject(new Error("no microphone"));
}
globalThis.__scMicPrime = () => {
  if (primed || !navigator.mediaDevices?.getUserMedia) return;
  primed = navigator.mediaDevices.getUserMedia(MIC);
  primed.catch(() => {});
  // a primed stream nobody takes within a minute is closed again (no open microphone for nothing)
  const mine = primed;
  setTimeout(() => { if (primed === mine) { primed = null; mine.then(st => st.getTracks().forEach(t => t.stop())).catch(() => {}); } }, 60000);
};

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
      if (a === "mic") { try { const v = JSON.parse(this.command("voice state")); if (!v.mic || v.muted) globalThis.__scMicPrime?.(); } catch { /* older game */ } this.command("voice mic"); }
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
      // the Nostr relays sign with a key for this session only (BIP-340 in the game)
      const hex = n => Array.from(crypto.getRandomValues(new Uint8Array(n)), b => b.toString(16).padStart(2, "0")).join("");
      const secret = this.nostrKey ||= hex(32);
      this.net.sign = (created, tags, content) => { const ev = this.command(`nostr event ${created} ${secret} ${hex(32)} ${JSON.stringify(tags)} ${content}`); return ev.startsWith("{") ? ev : null; };
      this.net.nostr = this.params?.get?.("nostr") === "1";
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
    // solo is said plainly: the button names where a tap goes, the line under it what you are in
    if (!this.net) return this.failed ? {state: "error", label: "MULTI ⚠", status: this.failed} : {state: "offline", label: "SOLO ▸ MULTI", status: ""};
    const brokers = this.kind === "public" ? (this.net.clients?.filter(c => c.open).length || 0) : 1;
    if (!brokers && performance.now() - (this.openedAt || 0) > 6000) return {state: "error", label: "MULTI ⚠", status: "Keine Verbindung · tippen = neu versuchen"};
    if (others > 0) return {state: "connected", label: `MULTI · ${others + 1}P`, status: `${others + 1} Spieler · ${this.hosting ? "du hostest" : "verbunden"}`};
    return {state: "waiting", label: "WARTE …", status: this.hosting ? "Lobby offen · warte auf Mitspieler" : "verbunden · warte auf Mitspieler"};
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
        el.innerHTML = '<div class="on"><span class="t"><strong></strong><span class="st"></span><small></small><span class="el"></span><span class="mic">🎙</span></span><b class="hp"><u></u></b><i class="caret"></i></div>'
          + '<div class="edge"><i class="arrow"></i><div class="ring"></div><div class="lab"><b></b><i></i></div></div>';
        // the plates live with the game's own HUD (the original view's #viewport: above its canvas),
        // else in Kern's layer
        if (!this.plateRoot) {
          const vp = document.getElementById("viewport");
          this.plateRoot = document.createElement("div"); this.plateRoot.id = "mpPlates";
          this.plateRoot.style.cssText = "position:absolute;inset:0;pointer-events:none;z-index:7;overflow:hidden";
          (vp || this.root).appendChild(this.plateRoot);
        }
        this.plateRoot.appendChild(el); this.plates.set(p.slot, el);
      }
      el.style.setProperty("--c", p.color);
      el.classList.toggle("off", !p.on); el.classList.toggle("dead", p.dead); el.classList.toggle("talk", p.talk);
      el.classList.toggle("near", !p.on && p.d < 40);
      const vh = (document.getElementById("viewport") || document.documentElement).clientHeight || innerHeight;
      el.classList.toggle("low", !p.on && p.y > vh * 0.6);  // at the bottom edge the words go above the badge
      const sc = p.on ? Math.max(.85, Math.min(1.2, 1.3 - p.d / 160)) : 1;
      el.style.transform = `translate3d(${p.x}px,${p.y}px,0) translate(-50%,${p.on ? "-100%" : "-50%"}) scale(${sc.toFixed(3)})`;
      // the arrow points from the badge toward him (screen angle, 0 = up)
      el.style.setProperty("--a", `${(p.angle + 180).toFixed(0)}deg`);
      const name = p.dead ? `P${p.index} DOWN` : `P${p.index}`, d = p.d < 10 ? `${p.d.toFixed(1)} m` : p.d < 1000 ? `${Math.round(p.d)} m` : `${(p.d / 1000).toFixed(1)} km`;
      const dz = Number(p.dz) || 0, el3 = Math.abs(dz) >= 2.5 ? `${dz > 0 ? "▲" : "▼"} ${Math.round(Math.abs(dz))} m` : "";
      const rel = Number(p.rel) || 0, side = Math.abs(rel) > 135 ? "HINTER DIR" : rel < -20 ? "◀ LINKS" : rel > 20 ? "RECHTS ▶" : dz > 2.5 ? "ÜBER DIR" : dz < -2.5 ? "UNTER DIR" : "VORN";
      const put = (q, t) => { const n = el.querySelector(q); if (n.textContent !== t) n.textContent = t; };
      put("strong", name); put(".on .st", p.dead ? "" : "★".repeat(Math.max(0, Math.min(5, p.stars | 0)))); put("small", d);  // his wanted level (his own) put(".on .el", el3); el.querySelector(".hp u").style.width = `${p.hp}%`;
      put(".edge .ring", `P${p.index}`); put(".edge .lab b", el3 ? `${d} · ${el3}` : d); put(".edge .lab i", side);
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
      micStream().then(stream => {
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
      }).catch(() => { this.command("voice denied"); this.statusEl.textContent = "Mikrofon nicht freigegeben"; this.statusEl.hidden = false; setTimeout(() => { this.statusEl.hidden = true; }, 2500); });
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
  // The view is the world's: longitude / latitude at the centre and a zoom (MapLibre's numbers, 3 the
  // continents .. 19.5 the kerb - world_map_full.mjs). The ground is the real map of the whole
  // world (world_map.mjs); what the game knows - me, the mates, the jet, pings, the destination and
  // its route - is drawn on it.
  constructor(page) {
    this.page = page; this.view = null; this.follow = true; this.data = null; this.last = 0; this.pointers = new Map();
    this.base = new WorldBasemap(() => globalThis.SpawnCity?.tiles?.template || null);
    const el = document.createElement("div"); el.id = "mpMap";
    el.innerHTML = `<canvas></canvas><div class="bar"><input type="search" placeholder="Adresse oder Ort suchen" enterkeyhint="search"><button data-b="me">◎</button><button data-b="clear">ZIEL ✕</button><button data-b="close">✕</button></div><ul></ul>
      <div class="info">Tippen: Wegpunkt · Ziehen: verschieben · Rad / Pinch: Zoom · M / Esc: schließen · © <a href="https://openfreemap.org" target="_blank" rel="noopener">OpenFreeMap</a> © <a href="https://openmaptiles.org" target="_blank" rel="noopener">OpenMapTiles</a> © <a href="https://www.openstreetmap.org/copyright" target="_blank" rel="noopener">OpenStreetMap</a>-Mitwirkende · Suche: Photon · Routen: FOSSGIS</div>`;
    document.body.appendChild(el);
    document.exitPointerLock?.();
    this.el = el; this.cv = el.querySelector("canvas"); this.g = this.cv.getContext("2d"); this.list = el.querySelector("ul"); this.input = el.querySelector("input");
    el.querySelector(".bar").addEventListener("click", ev => {
      const b = ev.target.closest("button")?.dataset.b;
      if (b === "close") page.command("bigmap close");
      if (b === "clear") page.command("nav clear");
      if (b === "me") { this.follow = true; this.last = 0; }
    });
    this.input.addEventListener("keydown", ev => { ev.stopPropagation(); if (ev.key === "Escape") page.command("bigmap close"); });
    this.onSlash = ev => { if (ev.code === "Slash" && document.activeElement !== this.input) { ev.preventDefault(); this.input.focus(); } };
    addEventListener("keydown", this.onSlash, true);
    this.input.addEventListener("input", () => { clearTimeout(this.searchTimer); this.searchTimer = setTimeout(() => this.search(), 350); });
    this.cv.addEventListener("wheel", ev => { this.zoomBy(-wheelPixels(ev) * 0.0022, ev.clientX, ev.clientY); ev.preventDefault(); }, {passive: false});
    this.cv.addEventListener("pointerdown", ev => { this.pointers.set(ev.pointerId, {x: ev.clientX, y: ev.clientY, sx: ev.clientX, sy: ev.clientY, moved: false}); this.cv.setPointerCapture(ev.pointerId); });
    this.cv.addEventListener("pointermove", ev => {
      const p = this.pointers.get(ev.pointerId); if (!p) return;
      if (this.pointers.size === 2) {
        const [a, b] = [...this.pointers.values()], d0 = Math.hypot(a.x - b.x, a.y - b.y);
        p.x = ev.clientX; p.y = ev.clientY;
        const d1 = Math.hypot(a.x - b.x, a.y - b.y);
        if (d0 > 1 && d1 > 1) this.zoomBy(Math.log2(d1 / d0), (a.x + b.x) / 2, (a.y + b.y) / 2);
        for (const q of this.pointers.values()) q.moved = true;
        return;
      }
      this.panPx(p.x - ev.clientX, p.y - ev.clientY); p.x = ev.clientX; p.y = ev.clientY;
      if (Math.hypot(p.x - p.sx, p.y - p.sy) > 8) { this.follow = false; p.moved = true; }
    });
    const up = ev => {
      const p = this.pointers.get(ev.pointerId); this.pointers.delete(ev.pointerId);
      if (ev.type === "pointerup" && p && !p.moved && this.pointers.size === 0 && this.view) {
        const [lon, lat] = this.unproject(ev.clientX, ev.clientY);
        page.command(`nav geo ${lon.toFixed(7)} ${lat.toFixed(7)} Wegpunkt`);
        this.last = 0;
      }
    };
    this.cv.addEventListener("pointerup", up);
    this.cv.addEventListener("pointercancel", up);
  }
  // ---- the view
  scale() { return 512 * 2 ** this.view.zoom; }
  project(lon, lat) { const s = this.scale(); return [this.cv.clientWidth / 2 + (mercX(lon) - mercX(this.view.lon)) * s, this.cv.clientHeight / 2 + (mercY(lat) - mercY(this.view.lat)) * s]; }
  unproject(px, py) { const s = this.scale(); return [lonOf(mercX(this.view.lon) + (px - this.cv.clientWidth / 2) / s), latOf(mercY(this.view.lat) + (py - this.cv.clientHeight / 2) / s)]; }
  panPx(dx, dy) {
    if (!this.view) return;
    const s = this.scale();
    this.view.lon = lonOf(((mercX(this.view.lon) + dx / s) % 1 + 1) % 1);
    this.view.lat = Math.max(-84, Math.min(84, latOf(mercY(this.view.lat) + dy / s)));
  }
  zoomBy(dz, px, py) {
    if (!this.view) return;
    const w = this.cv.clientWidth, h = this.cv.clientHeight, at = px === undefined ? [w / 2, h / 2] : [px, py];
    const before = this.unproject(at[0], at[1]);
    this.view.zoom = Math.max(3, Math.min(19.5, this.view.zoom + dz));
    // the place under the finger / the cursor stays under it
    const after = this.project(before[0], before[1]);
    this.panPx(after[0] - at[0], after[1] - at[1]);
    if (px !== undefined && Math.abs(dz) > 1e-4 && Math.hypot(at[0] - w / 2, at[1] - h / 2) > 40) this.follow = false;
  }
  // the game's metres <-> the world (kern::GeoProjector: equirectangular round the city's origin)
  toLonLat(x, y) { const d = this.data, R = 6378137, k = Math.PI / 180; return [d.lon0 + x / (R * k * Math.max(0.01, Math.cos(d.lat0 * k))), d.lat0 + y / (R * k)]; }
  P(x, y) { const [lon, lat] = this.toLonLat(x, y); return this.project(lon, lat); }
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
        this.list.innerHTML = ""; this.input.value = ""; this.last = 0;
        // the place found, in view
        if (this.view) { this.follow = false; this.view.lon = it.lon; this.view.lat = it.lat; this.view.zoom = Math.max(this.view.zoom, 14); }
      };
    } catch { this.list.innerHTML = "<li>Suche nicht erreichbar</li>"; }
  }
  frame() {
    const now = performance.now();
    if (now - this.last > 250) {
      this.last = now;
      // (the game's own streets are only drawn where there is no map of the world: the stand-in town)
      let range = 60;
      if (this.view && this.data && !this.data.real) range = Math.min(3000, Math.max(this.cv.clientWidth, this.cv.clientHeight) * 40075016.686 * Math.cos(this.view.lat * Math.PI / 180) / this.scale() * 0.6 + 50);
      try { this.data = JSON.parse(this.page.command(`bigmap follow ${range.toFixed(0)}`)); } catch { /* keep */ }
    }
    const d = this.data;
    if (d?.me && d.lon0 !== undefined) {
      const [lon, lat] = this.toLonLat(d.me[0], d.me[1]);
      // opened: on me, as close as the craft is fast (world_map_full.mjs: jet 12.5, a high drone 14.5, else 16)
      if (!this.view) this.view = {lon, lat, zoom: d.mode === "jet" ? 12.5 : d.mode === "drone" && d.agl > 120 ? 14.5 : 16};
      else if (this.follow) { this.view.lon = lon; this.view.lat = lat; }
    }
    this.padInput();
    this.draw();
  }
  padInput() {
    // world_map_full.mjs:355-362: LS pans -s|s| 900 px/s (dead zone .15, rescaled), RT/LT zoom
    // (rt - lt) 2.2 /s, A waypoint at the centre, X clears it, Y search, D-pad left/right zoom -1/+1,
    // R3 follows me, B or VIEW closes (VIEW held when the map opened does not close it).
    const p = readGamepad()?.pad;  // (the pad the game reads)
    const now = performance.now(), dt = Math.min(0.05, Math.max(0, (now - (this.padAt || now)) / 1000));
    this.padAt = now;
    if (!p || !this.view) return;
    const ax = i => { const v = +p.axes[i] || 0, a = Math.abs(v); return a <= 0.15 ? 0 : Math.sign(v) * (a - 0.15) / 0.85; }, b = i => !!p.buttons[i]?.pressed;
    const sx = ax(0), sy = ax(1);
    if (sx || sy) { this.panPx(sx * Math.abs(sx) * 900 * dt, sy * Math.abs(sy) * 900 * dt); this.follow = false; }
    const tz = (p.buttons[7]?.value || 0) - (p.buttons[6]?.value || 0);
    if (tz) this.zoomBy(tz * 2.2 * dt);
    if (!this.padWas) this.padWas = p.buttons.map(x => x.pressed);  // buttons held as the map opened
    const pressed = i => b(i) && !this.padWas[i];
    if (pressed(0)) this.page.command(`nav geo ${this.view.lon.toFixed(7)} ${this.view.lat.toFixed(7)} Wegpunkt`);
    if (pressed(2)) this.page.command("nav clear");
    if (pressed(3)) this.input.focus();
    if (pressed(14)) this.zoomBy(-1);
    if (pressed(15)) this.zoomBy(1);
    if (pressed(11)) this.follow = true;
    if (pressed(1) || pressed(8)) this.page.command("bigmap close");
    this.padWas = p.buttons.map(x => x.pressed);
  }
  draw() {
    const cv = this.cv, g = this.g, dpr = Math.min(2, devicePixelRatio || 1), w = cv.clientWidth, h = cv.clientHeight;
    if (cv.width !== Math.round(w * dpr) || cv.height !== Math.round(h * dpr)) { cv.width = Math.round(w * dpr); cv.height = Math.round(h * dpr); }
    g.setTransform(dpr, 0, 0, dpr, 0, 0);
    const d = this.data;
    if (!d?.me || !this.view) { g.fillStyle = "#0b1016"; g.fillRect(0, 0, w, h); return; }
    const mpp = 40075016.686 * Math.cos(this.view.lat * Math.PI / 180) / this.scale();  // metres per pixel here
    g.lineCap = g.lineJoin = "round";
    if (d.real) this.base.draw(g, w, h, this.view);
    else {
      // the stand-in town (no map service): the game's own streets
      g.fillStyle = "#f8f4f0"; g.fillRect(0, 0, w, h);
      for (const pass of [["#cfcdca", 2], ["#ffffff", 0]]) {
        g.strokeStyle = pass[0]; g.lineWidth = Math.max(1.2, 7 / mpp) + pass[1];
        for (const l of d.lanes) { g.beginPath(); for (let i = 0; i < l.length; i += 2) { const [x, y] = this.P(l[i], l[i + 1]); (i ? g.lineTo : g.moveTo).call(g, x, y); } g.stroke(); }
      }
    }
    if (d.route?.length >= 4) {
      for (const pass of [["#0b2235", 7, .85], ["#4ab3ff", 4, 1]]) {
        g.strokeStyle = pass[0]; g.lineWidth = pass[1]; g.globalAlpha = pass[2]; g.beginPath();
        for (let i = 0; i < d.route.length; i += 2) { const [x, y] = this.P(d.route[i], d.route[i + 1]); (i ? g.lineTo : g.moveTo).call(g, x, y); }
        g.stroke();
      }
      g.globalAlpha = 1;
    }
    if (d.real) this.base.drawLabels(g, w, h, this.view);
    const dot = (x, y, c, r) => { g.fillStyle = "#0b1118"; g.beginPath(); g.arc(x, y, r + 2, 0, Math.PI * 2); g.fill(); g.fillStyle = c; g.beginPath(); g.arc(x, y, r, 0, Math.PI * 2); g.fill(); };
    const tag = (t, x, y) => { g.font = "800 12px Inter, system-ui, sans-serif"; g.lineWidth = 3.5; g.strokeStyle = "#0b1118"; g.strokeText(t, x, y); g.fillStyle = "#fff"; g.fillText(t, x, y); };
    if (d.dest) { const [x, y] = this.P(d.dest.x, d.dest.y); dot(x, y, "#ffd76a", 8); tag(d.dest.name, x + 12, y + 4); }
    if (d.jet) { const [x, y] = this.P(d.jet[0], d.jet[1]); dot(x, y, "#7fd3ff", 6); }
    for (const p of d.pings) { const [x, y] = this.P(p.x, p.y); g.fillStyle = p.color; g.save(); g.translate(x, y); g.rotate(Math.PI / 4); g.fillRect(-6, -6, 12, 12); g.restore(); }
    // Players; one off the map sits at its edge with an arrow toward him (map_peer_edge.mjs).
    const edge = (x, y) => {
      const ix = 16, it = 60, ib = 40;
      if (!(x < ix || x > w - ix || y < it || y > h - ib)) return [x, y, false];
      const cx = w / 2, cy = (it + h - ib) / 2, dx = x - cx, dy = y - cy;
      const t = Math.min(dx ? ((dx > 0 ? w - ix : ix) - cx) / dx : Infinity, dy ? ((dy > 0 ? h - ib : it) - cy) / dy : Infinity);
      return [cx + dx * t, cy + dy * t, true];
    };
    for (const p of d.players) {
      const [x, y, off] = edge(...this.P(p.x, p.y));
      dot(x, y, p.dead ? "#777" : p.color, off ? 6 : 7);
      tag(`P${p.index}${off ? " ›" : ""}`, x + 10, y + 4);
    }
    // Me: an arrow along the view (at the map's edge when I am off it).
    {
      const [x, y, off] = edge(...this.P(d.me[0], d.me[1]));
      g.save(); g.translate(x, y); g.rotate(d.me[2]); if (off) g.scale(0.8, 0.8);
      g.fillStyle = "#0b1118"; g.beginPath(); g.moveTo(0, -16); g.lineTo(12, 13); g.lineTo(0, 7); g.lineTo(-12, 13); g.closePath(); g.fill();
      g.fillStyle = "#ffd23e"; g.beginPath(); g.moveTo(0, -13); g.lineTo(9, 10); g.lineTo(0, 5); g.lineTo(-9, 10); g.closePath(); g.fill(); g.restore();
    }
    // scale bar
    const m = [5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000, 2000000].find(v => v / mpp > 70) || 2000000, bw = m / mpp;
    g.fillStyle = "#0b1118"; g.fillRect(w - 21 - bw, h - 31, bw + 2, 5); g.fillStyle = "#fff"; g.fillRect(w - 20 - bw, h - 30, bw, 3);
    tag(m >= 1000 ? `${m / 1000} km` : `${m} m`, w - 20 - bw, h - 36);
  }
  close() { removeEventListener("keydown", this.onSlash, true); this.base.dispose(); this.el.remove(); }
}
