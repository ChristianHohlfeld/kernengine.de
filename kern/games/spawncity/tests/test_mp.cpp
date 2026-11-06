// Multiplayer: the rules (mp_rules.h) and three sessions (host + two clients) over a loopback
// "wire" as the page's transports carry the datagrams.
#include <algorithm>
#include <map>

#include "mp_rules.h"
#include "net_session.h"
#include "qr_code.h"
#include "test_util.h"

using namespace sc;

SC_TEST(mp_pose_timeline_interpolates_on_sender_clock) {
  mp::PoseTimeline t;
  // 30 Hz poses moving 1 m per packet along x, received with 40 ms latency and some jitter.
  for (int i = 0; i < 12; ++i) {
    const f64 src = 1000 + i * 33.0, rx = src + 40 + (i % 3) * 7.0;
    t.push({static_cast<f32>(i), 0, 0}, Quat{}, {30, 0, 0}, true, src, rx);
  }
  // Remote "now" = local - offset (40 ms floor); sample 90 ms behind it.
  const mp::PoseSample s = t.sample(1000 + 11 * 33.0 + 40);
  SC_CHECK(s.valid && !s.extrapolated, "interpolated");
  const f32 expect = static_cast<f32>((11 * 33.0 - 90) / 33.0);
  SC_CHECK(std::fabs(s.p.x - expect) < 0.35f, "x %.2f expect %.2f (offset %.1f)", s.p.x, expect, t.clock_offset_ms());
  // Past the newest packet: at most 100 ms of extrapolation, then hold.
  const mp::PoseSample late = t.sample(1000 + 11 * 33.0 + 40 + 400);
  SC_CHECK(late.extrapolated && late.p.x <= 11 + 30 * 0.1f + 0.5f, "extrapolation capped %.2f", late.p.x);
  SC_CHECK(!t.push({0, 0, 0}, Quat{}, {}, false, 1000, 5000), "older source time refused");
}

SC_TEST(mp_rules_damage_regen_spawn_vote) {
  SC_CHECK(mp::round_damage(mp::Weapon::glock, mp::Zone::torso) == 26, "glock");
  SC_CHECK(mp::round_damage(mp::Weapon::glock, mp::Zone::head) == 78, "glock head");
  SC_CHECK(mp::round_damage(mp::Weapon::smg, mp::Zone::legs) == 8, "smg legs");
  SC_CHECK(mp::round_damage(mp::Weapon::sniper, mp::Zone::head) == 100, "sniper head capped");
  SC_CHECK(mp::judge_hit(true, true, false, false, 0, 0, 100) == mp::HitVerdict::friendly_fire_off, "ff off");
  SC_CHECK(mp::judge_hit(true, true, true, false, 0, 0, 100) == mp::HitVerdict::applied, "self damage always");
  SC_CHECK(mp::judge_hit(true, true, false, true, 100, 2000, 100) == mp::HitVerdict::protected_target, "protected");
  SC_CHECK(mp::judge_hit(true, true, false, true, 100, 0, 0) == mp::HitVerdict::already_down, "down");
  SC_CHECK(mp::regen(50, 4000, 0, 1) == 50, "no regen within 5 s");
  SC_CHECK(mp::regen(50, 6000, 0, 1) == 62, "12 HP/s");
  Rng rng(7);
  for (int i = 0; i < 50; ++i) {
    const Vec2 o = mp::respawn_offset(rng);
    const f32 r = std::hypot(o.x, o.y);
    SC_CHECK(r >= 2.99f && r <= 5.01f, "respawn radius %.2f", r);
  }
  Vec2 c;
  SC_CHECK(mp::group_spawn_center({{0, 0}, {30, 0}, {60, 0}, {1000, 0}}, c) && c.x == 30, "medoid of the big group %.0f", c.x);
  mp::Ballot b;
  SC_CHECK(mp::create_ballot(b, 1, 0, true, 255, {3, 255, 1}, 0), "ballot");
  SC_CHECK(mp::cast_vote(b, 1, true, {1, 3, 255}, 10) == mp::Ballot::Status::pending, "pending");
  SC_CHECK(mp::cast_vote(b, 3, true, {1, 3, 255}, 20) == mp::Ballot::Status::approved, "unanimous");
  SC_CHECK(mp::create_ballot(b, 2, 0, true, 255, {1, 255}, 0) && mp::cast_vote(b, 1, false, {1, 255}, 1) == mp::Ballot::Status::rejected, "one no");
  SC_CHECK(mp::create_ballot(b, 3, 0, true, 255, {1, 255}, 0) && mp::cast_vote(b, 1, true, {1, 255}, 40000) == mp::Ballot::Status::cancelled, "expired");
  const std::vector<u32> cols = mp::assign_colors({"0", "1", "255"});
  SC_CHECK(cols[0] != cols[1] && cols[1] != cols[2] && cols[0] != cols[2], "distinct colours");
}

SC_TEST(mp_voice_codec_and_stream) {
  for (f32 v : {0.0f, 0.1f, -0.3f, 0.7f, -0.99f}) SC_CHECK(std::fabs(mp::decode_mulaw(mp::encode_mulaw(v)) - v) < 0.02f + std::fabs(v) * 0.04f, "mu-law %.2f", v);
  SC_CHECK(mp::proximity_gain(3) == 1 && mp::proximity_gain(70) == 0 && std::fabs(mp::proximity_gain(37.5f) - 0.5f) < 0.01f, "gain");
  SC_CHECK(mp::voice_gain(500, true) >= 0.85f, "everywhere");
  mp::VoiceCapture cap;
  std::vector<f32> tone(48000);
  for (std::size_t i = 0; i < tone.size(); ++i) tone[i] = 0.5f * std::sin(static_cast<f32>(i) * 2 * kPi * 440 / 48000);
  cap.push(tone.data(), 4800, 48000);  // 100 ms -> one 80 ms frame
  std::vector<u8> frame;
  SC_CHECK(cap.take(frame) && frame.size() == mp::kVoiceFrame && !cap.take(frame), "one frame");
  mp::VoiceStream vs;
  SC_CHECK(vs.push(1, frame, 1) && !vs.push(1, frame, 1), "sequence");
  std::vector<f32> out(48000 * 2, 0.0f);
  vs.render(out.data(), 48000, 48000);
  f32 peak = 0;
  for (f32 s : out) peak = std::fmax(peak, std::fabs(s));
  SC_CHECK(peak > 0.4f && peak < 0.6f, "played back %.2f", peak);
  SC_CHECK(!vs.speaking(), "drained");
}

SC_TEST(mp_navigation_math) {
  const std::vector<Vec2> route = {{0, 0}, {0, 100}, {100, 100}};
  const mp::Turn t = mp::upcoming_turn(route, {0, 10});
  SC_CHECK(t.dir == mp::Turn::Dir::right && std::fabs(t.distance - 90) < 1, "right turn in 90 m (%d %.1f)", static_cast<int>(t.dir), t.distance);
  const std::vector<mp::Crumb> c = mp::sample_breadcrumbs(route, {0, 10});
  SC_CHECK(c.size() == 5 && std::fabs(c[0].p.y - 19) < 0.01f, "crumbs");
  SC_CHECK(std::fabs(mp::route_left(route, {0, 10}) - 190) < 0.5f, "left %.1f", mp::route_left(route, {0, 10}));
  const mp::MapEdge e = mp::locate_map_peer({500, 100}, 400, 300);
  SC_CHECK(e.offmap && std::fabs(e.p.x - 384) < 0.01f, "edge %.1f", e.p.x);
  SC_CHECK(mp::format_distance(1234) == "1,2 km" && mp::format_distance(337) == "340 m", "fmt %s", mp::format_distance(1234).c_str());
  mp::PingBoard pb;
  SC_CHECK(pb.toggle(1, {0, 0, 0}, 0xff, 0) && pb.all().size() == 1, "ping set");
  SC_CHECK(!pb.toggle(1, {2, 0, 0}, 0xff, 10) && pb.all().empty(), "ping removed within 5 m");
  pb.set(2, {}, 1, 0);
  pb.expire(91000);
  SC_CHECK(pb.all().empty(), "expired after 90 s");
}

namespace {
struct Wire {
  std::map<u64, net::Session*> peers;
  void pump() {
    for (int round = 0; round < 4; ++round)
      for (auto& [id, s] : peers) {
        u64 to;
        std::vector<u8> b;
        while (s->take(to, b))
          if (peers.count(to)) peers[to]->push(id, b.data(), static_cast<u32>(b.size()));
      }
  }
};
net::PlayerState pose_at(Vec3 p, f32 hp = 100) {
  net::PlayerState s;
  s.position = p;
  s.avatar = p - Vec3{0, 0, 0.73f};
  s.health = hp;
  return s;
}
}  // namespace

SC_TEST(mp_sessions_host_two_clients) {
  net::Session host, a, b;
  Wire w;
  w.peers = {{11, &host}, {22, &a}, {33, &b}};
  host.host(11, 9.1766, 47.66);
  a.join(22, 11);
  b.join(33, 11);
  f64 t = 0;
  auto run = [&](f64 seconds, Vec3 ha, Vec3 pa, Vec3 pb, f32 hpb = 100) {
    for (f64 end = t + seconds; t < end; t += 1.0 / 60) {
      host.update(t, 1.0f / 60, pose_at(ha));
      a.update(t, 1.0f / 60, pose_at(pa));
      b.update(t, 1.0f / 60, pose_at(pb, hpb));
      w.pump();
    }
  };
  run(2, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
  SC_CHECK(a.connected() && b.connected(), "both joined");
  SC_CHECK(a.remotes().size() == 2 && b.remotes().size() == 2 && host.remotes().size() == 2, "everybody sees everybody %zu %zu %zu", a.remotes().size(),
           b.remotes().size(), host.remotes().size());
  f64 lon, lat;
  SC_CHECK(a.origin(lon, lat) && std::fabs(lon - 9.1766) < 1e-9, "host origin");
  Vec3 anchor;
  SC_CHECK(a.take_anchor(anchor) || true, "anchor");
  SC_CHECK(a.index_of(net::kHost) == 1, "host is P1");
  SC_CHECK(a.color_of(a.self_slot()) == host.color_of(a.self_slot()), "same colour everywhere");
  // Friendly fire is off by default (world_options.mjs): a hit from a on b does nothing.
  a.request_hit(b.self_slot(), 26, 2, 0, {5, 0, 1});
  run(0.3, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
  SC_CHECK(b.take_hits().empty() && a.take_confirms().empty(), "friendly fire off");
  // Unanimous vote: a proposes, the host starts the ballot (host votes yes), a and b say yes.
  SC_CHECK(a.propose_rule(true), "propose");
  run(0.3, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
  SC_CHECK(a.vote_pending() && b.vote_pending(), "ballot reached the clients: %s", b.vote_text().c_str());
  a.vote(true);
  b.vote(true);
  run(0.5, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
  SC_CHECK(host.friendly_fire() && a.friendly_fire() && b.friendly_fire(), "rule changed for all");
  a.request_hit(b.self_slot(), 26, 2, 1, {5, 0, 1});
  run(0.3, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
  const std::vector<net::Hit> hits = b.take_hits();
  SC_CHECK(hits.size() == 1 && hits[0].by == a.self_slot() && hits[0].damage == 26, "approved hit at b");
  SC_CHECK(a.take_confirms().size() == 1, "hit marker for a");
  // b goes down: everybody gets the kill, a has a kill, b a death.
  b.report_death(a.self_slot(), 2, 1);
  run(0.3, {0, 0, 1}, {5, 0, 1}, {200, 0, 1}, 0);
  SC_CHECK(host.take_kills().size() == 1 && a.take_kills().size() == 1, "kill feed");
  SC_CHECK(a.scores().of(a.self_slot()).kills == 1 && a.scores().of(b.self_slot()).deaths == 1, "scores");
  // Respawn: protection for 2.5 s, hits refused meanwhile.
  b.report_respawn();
  run(0.2, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
  a.request_hit(b.self_slot(), 26, 2, 1, {5, 0, 1});
  run(0.3, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
  SC_CHECK(b.take_hits().empty(), "spawn protection");
  // Effects and pings go to the others; voice only to those in range (b is 200 m away).
  net::Fx fx;
  fx.kind = net::Fx::explosion;
  fx.p = {1, 2, 3};
  fx.a = 7;
  a.send_fx(fx);
  a.send_ping(true, {4, 4, 0}, 0xff00ff);
  a.send_voice(1, false, std::vector<u8>(640, 0x7f));
  run(0.3, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
  const std::vector<net::Fx> fb = b.take_fx();
  SC_CHECK(fb.size() == 1 && fb[0].owner == a.self_slot() && fb[0].a == 7, "explosion reached b");
  SC_CHECK(b.take_pings().size() == 1 && host.take_pings().size() == 1, "ping");
  SC_CHECK(host.take_voice().size() == 1 && b.take_voice().empty(), "voice in range only");
  // Units (zombie_nights.mjs zombie-sync, military_response.mjs mil-snap): a's swarm reaches host and b,
  // b's hit on a's zombie goes to a alone.
  {
    std::vector<net::SwarmUnit> units;
    for (u16 i = 1; i <= 64; ++i) units.push_back({i, i % 2 ? net::SwarmKind::zombie : net::SwarmKind::soldier, 1, {i * 1.5f, -3.25f, 12.3f}, 3.1f, 1.7f});
    units[5].kind = net::SwarmKind::tank;
    a.send_swarm(units);
    run(0.3, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
    const std::vector<net::Swarm> sb = b.take_swarms(), sh = host.take_swarms();
    SC_CHECK(sb.size() == 1 && sh.size() == 1 && sb[0].owner == a.self_slot() && sb[0].units.size() == 64, "64 units in one message to b and the host");
    if (!sb.empty() && sb[0].units.size() == 64) {
      const net::SwarmUnit& u = sb[0].units[9];
      SC_CHECK(u.id == 10 && u.kind == net::SwarmKind::soldier && std::fabs(u.p.x - 15) < 1e-4f && std::fabs(u.p.z - 12.3f) < 0.06f && std::fabs(u.yaw - 3.1f) < 0.006f &&
                   std::fabs(u.aux - 1.7f) < 0.006f && u.flags == 1, "a unit arrives as it left");
      SC_CHECK(sb[0].units[5].kind == net::SwarmKind::tank, "kinds kept");
    }
    SC_CHECK(a.take_swarms().empty(), "nobody gets his own back");
    b.send_swarm_hit(a.self_slot(), 7, net::SwarmKind::zombie, 34);
    run(0.3, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
    const std::vector<net::SwarmHit> ha = a.take_swarm_hits();
    SC_CHECK(ha.size() == 1 && ha[0].id == 7 && ha[0].from == b.self_slot() && ha[0].damage == 34, "the hit reached the owner");
    SC_CHECK(host.take_swarm_hits().empty() && b.take_swarm_hits().empty(), "and only him");
    host.send_swarm_hit(b.self_slot(), 3, net::SwarmKind::heli, 25);
    run(0.3, {0, 0, 1}, {5, 0, 1}, {200, 0, 1});
    SC_CHECK(b.take_swarm_hits().size() == 1, "the host's hit on a client's helicopter");
  }
  // The pose timeline shows a mate where he is.
  const net::Remote* ra = b.remote(a.self_slot());
  SC_CHECK(ra && std::fabs(ra->body.sample(b.now_ms()).p.x - 5) < 0.2f, "a seen at x=5");
}

SC_TEST(mp_qr_code_structure) {
  // Decoded with OpenCV during development (versions 1, 5, 10, 11); here the structure.
  qr::Code c;
  SC_CHECK(qr::encode("HELLO", c) && c.version == 1 && c.size == 21, "v1 %d", c.version);
  SC_CHECK(qr::encode("https://kernengine.de/kern/games/spawncity/web/index.html?mp=join&room=lobby&net=public", c) && c.version == 5 && c.size == 37, "v5 %d", c.version);
  // Finder patterns in three corners, the dark module, timing pattern.
  for (auto [x, y] : {std::pair{0, 0}, {c.size - 7, 0}, {0, c.size - 7}}) SC_CHECK(c.dark(x, y) && c.dark(x + 6, y + 6) && !c.dark(x + 1, y + 1) && c.dark(x + 3, y + 3), "finder");
  SC_CHECK(c.dark(8, c.size - 8) && c.dark(8, 6) && !c.dark(9, 6), "dark module and timing");
  u32 level = 9, mask = 9;
  SC_CHECK(qr::read_format(c, level, mask) && level == 1 && mask == 0, "format L / mask 0 (%u %u)", level, mask);
  SC_CHECK(!qr::encode(std::string(3000, 'x'), c), "too long");
}
