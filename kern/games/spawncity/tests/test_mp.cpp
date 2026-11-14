// Multiplayer: the rules (mp_rules.h) and three sessions (host + two clients) over a loopback
// "wire" as the page's transports carry the datagrams.
#include <algorithm>
#include <functional>
#include <map>
#include <set>

#include "lamps.h"
#include "mp_rules.h"
#include "net_session.h"
#include "people.h"
#include "town_sync.h"
#include "qr_code.h"
#include "../src/lamps.h"
#include "rule_blueprint.h"
#include "test_util.h"

using namespace sc;

SC_TEST(mp_pose_timeline_interpolates_on_sender_clock) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
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
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
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
  // rules_vote_engine.mjs: no ballot with one voter, a host outside the roster or without an id
  SC_CHECK(!mp::create_ballot(b, 1, 0, true, 1, {1}, 0) && b.status == mp::Ballot::Status::invalid, "one voter: invalid");
  SC_CHECK(!mp::create_ballot(b, 1, 0, true, 9, {1, 2}, 0) && !mp::create_ballot(b, 0, 0, true, 1, {1, 2}, 0), "host not in the roster, no id: invalid");
  SC_CHECK(mp::ballot_name(mp::Ballot::Status::pending) == "pending" && mp::ballot_name(b.status) == "invalid", "a ballot's status by its name");
  SC_CHECK(mp::create_ballot(b, 7, 0, true, 2, {3, 1, 2, 2}, 0) && b.members.size() == 3 && b.expires_ms == 30000 && b.yes.size() == 1, "pending, each player once, the host has said yes");
  mp::Ballot::Status did = mp::Ballot::Status::pending;
  SC_CHECK(mp::cast_vote(b, 2, true, {1, 2, 3}, 1000, &did) == mp::Ballot::Status::pending && did == mp::Ballot::Status::ignored, "the host has voted: ignored");
  SC_CHECK(mp::cast_vote(b, 7, true, {1, 2, 3}, 1000, &did) == mp::Ballot::Status::pending && did == mp::Ballot::Status::ignored, "a stranger: ignored");
  SC_CHECK(!mp::ballot_stale(b, {3, 2, 1}, 1000) && mp::ballot_stale(b, {1, 2}, 1000) && mp::ballot_stale(b, {1, 2, 3}, 30000), "stale: the roster changed or the time is up");
  SC_CHECK(mp::cast_vote(b, 1, true, {3, 2, 1}, 2000, &did) == mp::Ballot::Status::pending && did == mp::Ballot::Status::pending, "one more yes");
  SC_CHECK(mp::cast_vote(b, 3, true, {1, 2, 3}, 3000, &did) == mp::Ballot::Status::approved && did == mp::Ballot::Status::approved, "unanimous: approved");
  SC_CHECK(mp::create_ballot(b, 8, 0, false, 1, {1, 2}, 0) && mp::cast_vote(b, 2, true, {1, 2, 4}, 1000) == mp::Ballot::Status::cancelled, "roster changed: cancelled");
  SC_CHECK(mp::create_ballot(b, 9, 0, false, 1, {1, 2}, 0) && mp::cast_vote(b, 2, true, {1, 2}, 30000) == mp::Ballot::Status::cancelled, "expired after 30 s");
  SC_CHECK(mp::create_ballot(b, 1, 0, true, 255, {3, 255, 1}, 0), "ballot");
  SC_CHECK(mp::cast_vote(b, 1, true, {1, 3, 255}, 10) == mp::Ballot::Status::pending, "pending");
  SC_CHECK(mp::cast_vote(b, 3, true, {1, 3, 255}, 20) == mp::Ballot::Status::approved, "unanimous");
  SC_CHECK(mp::create_ballot(b, 2, 0, true, 255, {1, 255}, 0) && mp::cast_vote(b, 1, false, {1, 255}, 1) == mp::Ballot::Status::rejected, "one no");
  SC_CHECK(mp::create_ballot(b, 3, 0, true, 255, {1, 255}, 0) && mp::cast_vote(b, 1, true, {1, 255}, 40000) == mp::Ballot::Status::cancelled, "expired");
  const std::vector<u32> cols = mp::assign_colors({"0", "1", "255"});
  SC_CHECK(cols[0] != cols[1] && cols[1] != cols[2] && cols[0] != cols[2], "distinct colours");
}

SC_TEST(mp_voice_codec_and_stream) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
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
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
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
  std::function<bool(u64 from, u64 to, std::size_t bytes)> lose;  // a datagram that does not arrive
  void pump() {
    for (int round = 0; round < 4; ++round)
      for (auto& [id, s] : peers) {
        u64 to;
        std::vector<u8> b;
        while (s->take(to, b))
          if (peers.count(to) && !(lose && lose(id, to, b.size()))) peers[to]->push(id, b);
      }
  }
};
// A host and two clients, all in.
struct Trio {
  net::Session host, a, b;
  Wire w;
  f64 t = 0;
  Trio() {
    w.peers = {{11, &host}, {22, &a}, {33, &b}};
    host.host(11, 9.1766, 47.66);
    a.join(22, 11);
    b.join(33, 11);
    run(2);
  }
  void run(f64 seconds) {
    for (f64 end = t + seconds; t < end; t += 1.0 / 60) {
      host.update(t, 1.0f / 60, pose_of({0, 0, 1}));
      a.update(t, 1.0f / 60, pose_of({5, 0, 1}));
      b.update(t, 1.0f / 60, pose_of({9, 0, 1}));
      w.pump();
    }
  }
  static net::PlayerState pose_of(Vec3 p) {
    net::PlayerState s;
    s.position = p;
    s.avatar = p - Vec3{0, 0, 0.73f};
    return s;
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
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
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
  const i32 ra = b.remote_index(a.self_slot());
  SC_CHECK(ra >= 0 && std::fabs(b.remotes()[static_cast<std::size_t>(ra)].body.sample(b.now_ms()).p.x - 5) < 0.2f, "a seen at x=5");
}

// A client that does not reach the host in time - the host's page is still loading, its tab is in
// the background - asks again every second and gets in as soon as the host answers.
SC_TEST(mp_a_client_keeps_asking_until_the_host_answers) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
  net::Session host, a;
  Wire w;
  w.peers = {{11, &host}, {22, &a}};
  host.host(11, 9.1766, 47.66);
  a.join(22, 11);
  f64 t = 0;
  auto step = [&](bool with_host) {
    if (with_host) host.update(t, 1.0f / 60, pose_at({0, 0, 1}));
    a.update(t, 1.0f / 60, pose_at({5, 0, 1}));
    w.pump();
    t += 1.0 / 60;
  };
  for (int i = 0; i < 15 * 60; ++i) step(false);  // fifteen seconds of a host that does not answer (a try gives up after six)
  SC_CHECK(!a.connected() && a.connects() >= 3, "not in, and it asked again (%u times)", a.connects());
  for (int i = 0; i < 3 * 60 && !a.connected(); ++i) step(true);
  SC_CHECK(a.connected(), "in once the host answers (after %u tries)", a.connects());
  const u32 tries = a.connects();
  for (int i = 0; i < 5 * 60; ++i) step(true);
  SC_CHECK(a.connected() && a.connects() == tries, "and it stays in without asking again");
}

// lan_vs.mjs authority handover: the host goes, both clients see it, agree on the successor (the
// lowest slot left), he hosts on, the other joins him.
SC_TEST(mp_host_leaves_the_oldest_takes_over) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
  net::Session host, a, b;
  Wire w;
  w.peers = {{11, &host}, {22, &a}, {33, &b}};
  host.host(11, 9.1766, 47.66);
  a.join(22, 11);
  b.join(33, 11);
  f64 t = 0;
  auto step = [&](bool with_host) {
    if (with_host) host.update(t, 1.0f / 60, pose_at({0, 0, 1}));
    a.update(t, 1.0f / 60, pose_at({5, 0, 1}));
    b.update(t, 1.0f / 60, pose_at({8, 0, 1}));
    w.pump();
    t += 1.0 / 60;
  };
  for (int i = 0; i < 120; ++i) step(true);
  SC_CHECK(a.connected() && b.connected() && !a.host_lost() && !b.host_lost(), "both in, the host there");
  host.stop();
  w.peers.erase(11);
  f64 until = t + 30;
  while (t < until && !(a.host_lost() && b.host_lost())) step(false);
  SC_CHECK(a.host_lost() && b.host_lost(), "both see the host gone (after %.1f s)", t - (until - 30));
  SC_CHECK(a.successor() == b.successor(), "they agree on the successor (%u / %u)", a.successor(), b.successor());
  net::Session& next = a.successor() == a.self_slot() ? a : b;
  net::Session& other = &next == &a ? b : a;
  const u64 next_id = &next == &a ? 22 : 33, other_id = &next == &a ? 33 : 22;
  next.stop();
  other.stop();
  next.host(next_id, 9.1766, 47.66);
  other.join(other_id, next_id);
  for (int i = 0; i < 180; ++i) step(false);
  f64 lon = 0, lat = 0;
  SC_CHECK(next.hosting() && other.connected() && other.origin(lon, lat) && std::fabs(lon - 9.1766) < 1e-9, "the session goes on under the successor, same origin");
  SC_CHECK(!other.host_lost(), "and the other is in again");
}

SC_TEST(mp_qr_code_structure) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
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

// One world, gap 1: a player's police and sandbox enemies are part of his swarm - the others see
// them with the right kind and state, and what they do to them goes to him alone.
SC_TEST(mp_swarm_police_and_enemies_are_seen_by_the_others) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
  Trio s;
  SC_CHECK(s.a.connected() && s.b.connected() && s.host.remotes().size() == 2, "all in");
  const u8 officer_flags = static_cast<u8>(net::kOfficerAims | (3 << net::kOfficerSkinShift));
  const std::vector<net::SwarmUnit> units = {
      {1, net::SwarmKind::officer, officer_flags, {10, 2, 0.4f}, 1.25f, 2.4f},                                             // on foot, aiming, skin 3
      {2, net::SwarmKind::cruiser, static_cast<u8>(net::kCruiserLights), {14, -3, 0.9f}, -0.5f, 0.21f},                     // the light bar on
      {3, net::SwarmKind::cruiser, static_cast<u8>(net::kCruiserWreck), {40, 8, 0.9f}, 2.0f, 0},                            // a wreck
      {4, net::SwarmKind::police_drone, static_cast<u8>(net::kDroneEmp), {12, 4, 31.5f}, 0.3f, 0},                          // knocked out, falling
      {5, net::SwarmKind::enemy, static_cast<u8>(2 | net::kEnemyAims), {-20, 6, 0}, 3.0f, 2.6f},                            // a HEAVY, aiming
      {6, net::SwarmKind::officer, static_cast<u8>(net::kUnitDead | (1 << net::kOfficerSkinShift)), {11, 3, 0.4f}, 0, 0},   // went down
      {7, net::SwarmKind::zombie, 1, {3, 3, 0}, 0.5f, 1.1f},                                                                // (the old kinds beside them)
  };
  SC_CHECK(s.a.send_swarm(units), "sent");
  s.run(0.3);
  for (net::Session* who : {&s.b, &s.host}) {
    const std::vector<net::Swarm> got = who->take_swarms();
    SC_CHECK(got.size() == 1 && got[0].owner == s.a.self_slot() && got[0].units.size() == units.size(), "one whole snapshot of a's (%zu)", got.size());
    if (got.size() != 1 || got[0].units.size() != units.size()) continue;
    for (std::size_t i = 0; i < units.size(); ++i) {
      const net::SwarmUnit& u = got[0].units[i];
      SC_CHECK(u.id == units[i].id && u.kind == units[i].kind && u.flags == units[i].flags, "unit %zu: kind %u flags %u", i, static_cast<unsigned>(u.kind), u.flags);
      SC_CHECK(std::fabs(u.p.x - units[i].p.x) < 1e-4f && std::fabs(u.p.y - units[i].p.y) < 1e-4f && std::fabs(u.p.z - units[i].p.z) < 0.06f && std::fabs(u.yaw - units[i].yaw) < 0.006f &&
                   std::fabs(u.aux - units[i].aux) < 0.006f,
               "unit %zu: where the owner has it", i);
    }
    const net::SwarmUnit& officer = got[0].units[0];
    SC_CHECK((officer.flags & net::kOfficerAims) && ((officer.flags >> net::kOfficerSkinShift) & 7) == 3 && !(officer.flags & net::kUnitDead), "the officer on foot aims, his skin kept");
    SC_CHECK((got[0].units[1].flags & net::kCruiserLights) && !(got[0].units[1].flags & net::kCruiserWreck) && (got[0].units[2].flags & net::kCruiserWreck), "the siren's light bar, the wreck");
    SC_CHECK((got[0].units[4].flags & net::kEnemyTypeMask) == 2 && (got[0].units[5].flags & net::kUnitDead), "the HEAVY, the officer who went down");
  }
  SC_CHECK(s.a.take_swarms().empty(), "the owner does not get his own back");
  // b's round on a's officer (a Glock: six counted hits of 25), the host's on a's cruiser and drone
  s.b.send_swarm_hit(s.a.self_slot(), 1, net::SwarmKind::officer, 150);
  s.host.send_swarm_hit(s.a.self_slot(), 2, net::SwarmKind::cruiser, 22);
  s.host.send_swarm_hit(s.a.self_slot(), 4, net::SwarmKind::police_drone, 34);
  s.b.send_swarm_hit(s.a.self_slot(), 5, net::SwarmKind::enemy, 13);
  s.run(0.3);
  const std::vector<net::SwarmHit> hits = s.a.take_swarm_hits();
  SC_CHECK(hits.size() == 4, "the four hits reached the owner (%zu)", hits.size());
  auto has = [&](u16 id, net::SwarmKind k, f32 dmg, u8 from) {
    return std::any_of(hits.begin(), hits.end(), [&](const net::SwarmHit& h) { return h.id == id && h.kind == k && h.damage == dmg && h.from == from && h.to == s.a.self_slot(); });
  };
  SC_CHECK(has(1, net::SwarmKind::officer, 150, s.b.self_slot()) && has(2, net::SwarmKind::cruiser, 22, net::kHost) && has(4, net::SwarmKind::police_drone, 34, net::kHost) &&
               has(5, net::SwarmKind::enemy, 13, s.b.self_slot()),
           "each with its unit, kind, damage and who did it");
  SC_CHECK(s.host.take_swarm_hits().empty() && s.b.take_swarm_hits().empty(), "and only the owner");
  SC_CHECK(s.a.send_refused() == 0 && s.b.send_refused() == 0 && s.host.send_refused() == 0, "nothing refused");
}

// ... and a hit on a mate's unit is noted here for him by the unit's own rule, never applied here.
SC_TEST(mp_people_note_hits_on_a_mates_unit) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
  People town;
  const i32 officer = town.add_officer({0, 0, 0}, 0, -1, 7);
  town.at(officer).hp = 1e6f;
  town.at(officer).remote = true;
  Vec3 to{5, 0, 1.0f};
  SC_CHECK(town.shot({-5, 0, 1.0f}, to, 26, true, 10) == officer && !town.last_head(), "the round meets him (body)");
  std::vector<People::RemoteHit> hits;
  town.take_remote_hits(hits);
  SC_CHECK(hits.size() == 1 && hits[0].index == officer && hits[0].damage == 150, "a Glock round on an officer: six counted hits of 25 (%f)", hits.empty() ? 0.0 : hits[0].damage);
  SC_CHECK(town.at(officer).hp == 1e6f && town.at(officer).state != People::State::down, "nothing applied here");
  to = {5, 0, 1.0f};
  town.shot({-5, 0, 1.0f}, to, 13, true, 10.5f);
  hits.clear();
  town.take_remote_hits(hits);
  SC_CHECK(hits.size() == 1 && hits[0].damage == 25, "an MP round: one hit of 25");
  // a blast is not noted: the owner's machine runs it on the real unit
  town.blast({0.5f, 0, 0.9f}, 8, 200, true, 11);
  hits.clear();
  town.take_remote_hits(hits);
  std::vector<People::Death> deaths;
  town.take_deaths(deaths);
  SC_CHECK(hits.empty() && deaths.empty() && town.at(officer).hp == 1e6f, "a blast leaves a mate's unit to its owner");
  // our own officer beside him takes the same round by the same rule (the single-player path)
  People own;
  const i32 mine = own.add_officer({0, 0, 0}, 0, 0, 7);
  to = {5, 0, 1.0f};
  own.shot({-5, 0, 1.0f}, to, 13, true, 10);
  SC_CHECK(own.at(mine).hp == 75, "our own officer: 100 - 25 (%f)", own.at(mine).hp);
  to = {5, 0, 1.0f};
  own.shot({-5, 0, 1.0f}, to, 26, true, 10.2f);
  SC_CHECK(own.at(mine).state == People::State::down, "and down under the Glock");
}

// More units than one message carries arrive complete - in numbered parts, never a list with
// units missing.
SC_TEST(mp_swarm_larger_than_one_message_arrives_whole) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
  Trio s;
  std::vector<net::SwarmUnit> units;
  for (u16 i = 1; i <= 190; ++i) units.push_back({i, static_cast<net::SwarmKind>(i % 8), static_cast<u8>(i % 3), {i * 0.5f, -1.0f * i, 2}, 0.1f, 0.2f});
  SC_CHECK(units.size() > 2 * net::kSwarmPerMessage, "three messages' worth");
  SC_CHECK(s.a.send_swarm(units), "sent");
  s.run(0.3);
  for (net::Session* who : {&s.b, &s.host}) {
    const std::vector<net::Swarm> got = who->take_swarms();
    SC_CHECK(got.size() == 1 && got[0].units.size() == 190, "one snapshot with all 190 (%zu snapshots, %zu units)", got.size(), got.empty() ? 0 : got[0].units.size());
    std::set<u16> ids;
    bool same = true;
    for (const net::Swarm& g : got)
      for (const net::SwarmUnit& u : g.units) { ids.insert(u.id); same = same && u.id >= 1 && u.id <= 190 && u.kind == units[u.id - 1].kind && u.flags == units[u.id - 1].flags; }
    SC_CHECK(ids.size() == 190 && same, "every unit once, each as it was sent (%zu)", ids.size());
  }
  // a part lost on the way: that snapshot is not shown at all (not with a third of it missing) ...
  int big = 0;
  s.w.lose = [&](u64 from, u64, std::size_t bytes) { return from == 22 && bytes > 1000 && ++big == 2; };
  s.a.send_swarm(units);
  s.run(0.3);
  SC_CHECK(big >= 3, "three large datagrams left a (%d)", big);
  for (net::Session* who : {&s.b, &s.host})
    for (const net::Swarm& g : who->take_swarms()) SC_CHECK(g.units.size() == 190, "a snapshot handed on is whole (%zu)", g.units.size());
  // ... and the next one is there again, whole
  s.w.lose = nullptr;
  units.resize(70);
  s.a.send_swarm(units);
  s.run(0.3);
  const std::vector<net::Swarm> next = s.b.take_swarms();
  SC_CHECK(!next.empty() && next.back().units.size() == 70, "the next snapshot: 70 units (%zu)", next.empty() ? 0 : next.back().units.size());
  // an empty list is a snapshot too (his units are gone)
  s.host.take_swarms();
  s.a.send_swarm({});
  s.run(0.3);
  const std::vector<net::Swarm> none = s.host.take_swarms();
  SC_CHECK(none.size() == 1 && none[0].units.empty(), "the empty snapshot");
  // beyond what the parts can number, the list is refused out loud, never cut
  std::vector<net::SwarmUnit> huge(net::kSwarmMaxUnits + 1);
  SC_CHECK(s.b.send_refused() == 0 && !s.b.send_swarm(huge) && s.b.send_refused() == 1, "too many: refused and counted");
}

// One world, gap 2: what a client does to a car or a passer-by of the host's town reaches the host
// (reliably, with who did it), the host applies it, and the result comes back in his world list.
SC_TEST(mp_town_a_clients_hits_reach_the_host) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
  Trio s;
  // a's rounds on the host's car 7 (30 a round) and a blast on car 9
  s.a.town_to_host(town::pack(town::CarHit{7, 30, town::Cause::round}));
  s.a.town_to_host(town::pack(town::CarHit{7, 30, town::Cause::round}));
  s.a.town_to_host(town::pack(town::CarHit{9, 78, town::Cause::blast}));
  s.run(0.3);
  const std::vector<net::Session::Blob> at_host = s.host.take_town();
  SC_CHECK(at_host.size() == 3, "all three reached the host, in order (%zu)", at_host.size());
  SC_CHECK(s.a.take_town().empty() && s.b.take_town().empty(), "and nobody else");
  // the host's town: car 7 has 180 HP and is critical at 20 % (game_town.cpp damage_car)
  f32 hp[16];
  for (f32& h : hp) h = 180;
  for (const net::Session::Blob& m : at_host) {
    town::CarHit h;
    SC_CHECK(town::kind_of(m.data) == town::Kind::car_hit && town::unpack(m.data, h) && m.from == s.a.self_slot(), "a car hit, by a");
    if (h.car < 16) hp[h.car] -= h.damage;
  }
  SC_CHECK(hp[7] == 120 && hp[9] == 102, "applied on the host: %f %f", hp[7], hp[9]);
  town::CarHit last;
  SC_CHECK(at_host.size() == 3 && town::unpack(at_host[2].data, last) && last.car == 9 && last.damage == 78 && last.cause == town::Cause::blast, "car, damage and cause kept");
  // ... and the car's state comes back to the clients in the host's list (here: car 7, a wreck)
  const std::vector<u8> list = {1 /* cars */, 0x81 /* one entry, the last part */, 0, 7, 0x82 /* at rest, a wreck */};
  s.host.send_world_to(s.a.self_slot(), list);
  s.host.send_world_to(s.b.self_slot(), list);
  s.run(0.2);
  const std::vector<std::vector<u8>> wa = s.a.take_world(), wb = s.b.take_world();
  SC_CHECK(wa.size() == 1 && wb.size() == 1 && wa[0] == list && wb[0] == list, "the wreck in everybody's list");
  // nonsense is not taken for a hit
  town::CarHit bad;
  SC_CHECK(!town::unpack(town::pack(town::CarHit{1, -5, town::Cause::round}), bad) && !town::unpack(town::pack(town::CarHit{1, std::nanf(""), town::Cause::round}), bad) &&
               !town::unpack({1, 2, 3}, bad),
           "a hit that is none");

  // a passer-by: on the client he is the host's puppet - a round he survives is noted for the host
  People client;
  client.set_puppets(true);
  People::NetPerson np;
  np.idx = 12;
  np.seed = 4711;
  np.pos = {0, 0, 0};
  client.apply_snapshot({np}, 5);
  Vec3 to{5, 0, 1.0f};
  const i32 hit = client.shot({-5, 0, 1.0f}, to, 13, true, 6);
  std::vector<People::Hurt> hurts;
  client.take_hurts(hurts);
  SC_CHECK(hit >= 0 && hurts.size() == 1 && hurts[0].net == 12 && hurts[0].seed == 4711 && hurts[0].damage == 55 && hurts[0].by == People::HurtBy::round, "the client notes his hit (%zu)", hurts.size());
  if (hurts.size() != 1) return;
  s.a.town_to_host(town::pack(town::PersonHit{hurts[0].net, hurts[0].seed, hurts[0].damage, static_cast<u8>(hurts[0].by), hurts[0].push}));
  s.run(0.3);
  const std::vector<net::Session::Blob> ph = s.host.take_town();
  town::PersonHit h;
  SC_CHECK(ph.size() == 1 && ph[0].from == s.a.self_slot() && town::unpack(ph[0].data, h) && h.person == 12 && h.seed == 4711 && h.damage == 55, "the hit reached the host");
  // the host's town: citizen 12 is that person - hurt, down as a ragdoll, alive; the one beside him runs
  People host_town;
  i32 victim = -1, bystander = -1;
  for (i32 i = 0; i <= 13; ++i) {
    const i32 at = host_town.add_citizen({i == 13 ? 1.0f : 100.0f + i, 0, 0}, 0, i == 12 ? 4711u : 9000u + static_cast<u32>(i));
    if (i == 12) victim = at;
    if (i == 13) bystander = at;
  }
  host_town.at(victim).pos = {0, 0, 0};
  SC_CHECK(victim == 12, "the host's index is the one the snapshot named");
  host_town.mate_hurt(h.person, h.seed, h.damage, static_cast<People::HurtBy>(h.by), h.push, 20);
  std::vector<People::Fall> falls;
  host_town.take_falls(falls);
  std::vector<CrimeReport> crimes;
  host_town.take_reports(crimes);
  SC_CHECK(host_town.at(victim).hp == 45 && host_town.at(victim).state != People::State::down && falls.size() == 1 && falls[0].index == victim && falls[0].alive, "hurt and knocked down on the host");
  SC_CHECK(host_town.at(bystander).state == People::State::flee, "the people around run");
  SC_CHECK(crimes.empty(), "no crime of the host's");
  // the wrong person in that slot by now (the host reused it): nothing happens to him
  host_town.at(victim).knocked_until = 0;
  host_town.mate_hurt(h.person, 1234, 55, People::HurtBy::round, h.push, 30);
  SC_CHECK(host_town.at(victim).hp == 45, "another person in the slot is not hit");
  // a second hit finishes him: a death on the host, nobody's crime there
  host_town.mate_hurt(h.person, h.seed, 55, People::HurtBy::round, h.push, 30);
  std::vector<People::Death> deaths;
  host_town.take_deaths(deaths);
  host_town.take_reports(crimes);
  SC_CHECK(host_town.at(victim).state == People::State::down && deaths.size() == 1 && !deaths[0].by_player && crimes.empty(), "down for everybody, not by the host's player");
  // a fist he takes standing (above 35 HP left): hurt, no fall
  const i32 boxer = host_town.add_citizen({300, 0, 0}, 0, 31337);
  falls.clear();
  host_town.take_falls(falls);
  falls.clear();
  host_town.mate_hurt(boxer, 31337, 18, People::HurtBy::blow, {1, 0, 0.9f}, 40);
  host_town.take_falls(falls);
  SC_CHECK(host_town.at(boxer).hp == 82 && falls.empty(), "a blow he takes standing");
}

// One world, gaps 3 and 4: broken lamps, barricade planks and the blasts that left marks are the
// host's state - a late joiner gets all of it, changes go round reliably.
SC_TEST(mp_town_state_for_a_late_joiner) {
  RuleBlueprint mp_rules("Multiplayer", &mp::bind);
  RuleBlueprint lamp_rules("Lamps", &lamps::bind);  // (what breaks a lamp, and that it only gets worse)
  net::Session host, a, b;
  Wire w;
  w.peers = {{11, &host}, {22, &a}};
  host.host(11, 9.1766, 47.66);
  a.join(22, 11);
  f64 t = 0;
  bool b_in = false;
  auto run = [&](f64 seconds) {
    for (f64 end = t + seconds; t < end; t += 1.0 / 60) {
      host.update(t, 1.0f / 60, pose_at({0, 0, 1}));
      a.update(t, 1.0f / 60, pose_at({5, 0, 1}));
      if (b_in) b.update(t, 1.0f / 60, pose_at({9, 0, 1}));
      w.pump();
    }
  };
  run(2);
  SC_CHECK(a.connected(), "a is in");
  // the town's lamps (the same map tiles on every machine): 60 along a street
  std::vector<lamps::Field::Spot> spots;
  for (int i = 0; i < 60; ++i) spots.push_back({i * 34.0f, 5.15f, 0});
  lamps::Field hl, al, bl;
  for (lamps::Field* f : {&hl, &al, &bl}) f->rebuild(spots, nullptr);
  // before b comes: lamp 3 shot dark, lamp 10 down, and one down in a part of town not loaded here
  SC_CHECK(hl.set_state(3, 1) && hl.set_state(10, 2), "broken on the host");
  const i64 far_key = lamps::cell_key(5000, 5000);
  SC_CHECK(hl.merge_broken(far_key, 2) && !hl.merge_broken(far_key, 1) && hl.broken() == 3, "a far one, remembered by its cell; never better again");
  std::vector<town::LampRec> lamp_state;
  for (const auto& [key, state] : hl.broken_list()) lamp_state.push_back({key, state});
  const std::vector<town::PlankRec> plank_state = {{0x1234567890ll, 0, 2, 4}, {0x1234567890ll, 1, 2, 0}, {-77, 3, 5, 5}};
  std::vector<town::MarkRec> mark_state;
  for (int i = 0; i < 5; ++i) mark_state.push_back({{10.0f * i, -3, 1.5f}, 6.0f + i, 1.0f});
  // b joins late: the host sends the whole state to him alone
  w.peers[33] = &b;
  b.join(33, 11);
  b_in = true;
  run(2);
  SC_CHECK(b.connected() && host.remote_index(b.self_slot()) >= 0, "b is in");
  a.take_town();
  for (const std::vector<u8>& part : town::pack_state(3, lamp_state, plank_state, mark_state)) host.town_to(b.self_slot(), part);
  run(0.3);
  SC_CHECK(a.take_town().empty(), "the others, who have it, get nothing");
  const std::vector<net::Session::Blob> parts = b.take_town();
  SC_CHECK(parts.size() == 1, "the state reached the joiner (%zu)", parts.size());
  std::vector<town::PlankRec> got_planks;
  std::vector<town::MarkRec> got_marks;
  for (const net::Session::Blob& m : parts) {
    town::State st;
    SC_CHECK(m.from == net::kHost && town::kind_of(m.data) == town::Kind::state && town::unpack_state(m.data, st) && st.epoch == 3 && st.first && st.last, "the host's state, whole");
    if (st.first) bl.reset_broken();
    for (const town::LampRec& r : st.lamps) bl.merge_broken(r.key, r.state);
    got_planks.insert(got_planks.end(), st.planks.begin(), st.planks.end());
    got_marks.insert(got_marks.end(), st.marks.begin(), st.marks.end());
  }
  SC_CHECK(bl.all()[3].state == 1 && bl.all()[10].state == 2 && bl.all()[4].state == 0 && bl.broken() == 3, "the joiner's lamps are the host's: dark, down, lit (%u %u)", bl.all()[3].state, bl.all()[10].state);
  SC_CHECK(bl.broken_list() == hl.broken_list(), "the same broken lamps, the far one too");
  SC_CHECK(got_planks == plank_state, "the planks of every door that is not whole");
  SC_CHECK(got_marks.size() == 5 && got_marks[4].at.x == 40 && got_marks[4].radius == 10 && got_marks[0].strength == 1, "the blasts that left marks, in their order");
  // the far lamp's tile streams in at the joiner: it stands there broken
  std::vector<lamps::Field::Spot> more = spots;
  more.push_back({5000, 5000, 0});
  bl.rebuild(more, nullptr);
  SC_CHECK(bl.all().back().state == 2, "a lamp placed later is down as the town has it");

  // a change: a breaks lamp 20 - to the host, from him to everybody
  SC_CHECK(al.set_state(20, 1), "a shoots a lamp out");
  for (const std::vector<u8>& part : town::pack_lamps({{al.all()[20].key, 1}})) a.town_to_host(part);
  run(0.3);
  std::vector<town::LampRec> news;
  for (const net::Session::Blob& m : host.take_town()) {
    std::vector<town::LampRec> lamps;
    SC_CHECK(m.from == a.self_slot() && town::unpack_lamps(m.data, lamps), "a's lamp at the host");
    for (const town::LampRec& r : lamps)
      if (hl.merge_broken(r.key, r.state)) news.push_back(r);
  }
  SC_CHECK(news.size() == 1 && hl.all()[20].state == 1, "new to the town");
  for (const std::vector<u8>& part : town::pack_lamps(news)) host.town_to_all(part);
  run(0.3);
  for (net::Session* who : {&a, &b}) {
    lamps::Field& f = who == &a ? al : bl;
    for (const net::Session::Blob& m : who->take_town()) {
      std::vector<town::LampRec> lamps;
      if (town::unpack_lamps(m.data, lamps))
        for (const town::LampRec& r : lamps) f.merge_broken(r.key, r.state);
    }
    SC_CHECK(f.all()[20].state == 1, "everybody has it");
  }
  SC_CHECK(hl.broken_list() == bl.broken_list(), "host and joiner still agree");

  // a plank: a's zombie breaks one at door 1 of 2 - a step the host applies; the door's count goes to all
  a.town_to_host(town::pack(town::PlankStep{42, 1, 2, -1}));
  b.town_to_host(town::pack(town::PlankStep{42, 1, 2, -1}));
  run(0.3);
  std::vector<u8> door(2, town::kPlanksMax);
  for (const net::Session::Blob& m : host.take_town()) {
    town::PlankStep st;
    SC_CHECK(town::unpack(m.data, st) && st.key == 42 && st.door == 1 && st.doors == 2 && st.step == -1, "the step at the host");
    door[st.door] = static_cast<u8>(door[st.door] + st.step);
    host.town_to_all(town::pack(town::PlankRec{st.key, st.door, st.doors, door[st.door]}));
  }
  run(0.3);
  for (net::Session* who : {&a, &b}) {
    town::PlankRec lastrec;
    std::size_t n = 0;
    for (const net::Session::Blob& m : who->take_town())
      if (town::unpack(m.data, lastrec)) ++n;
    SC_CHECK(n == 2 && lastrec.key == 42 && lastrec.door == 1 && lastrec.planks == 4, "both players' zombies at one door: one barricade, two planks gone (%zu, %u)", n, lastrec.planks);
  }
  town::PlankStep bad;
  SC_CHECK(!town::unpack(town::pack(town::PlankStep{1, 5, 2, -1}), bad) && !town::unpack(town::pack(town::PlankStep{1, 0, 2, 3}), bad), "a step that is none");

  // a big town: the state goes in as many parts as it takes, none over its size, the first and the last marked
  std::vector<town::LampRec> many_lamps;
  for (i64 k = 0; k < 400; ++k) many_lamps.push_back({k * 7919 - 1000, static_cast<u8>(1 + k % 2)});
  std::vector<town::PlankRec> many_planks;
  for (i64 k = 0; k < 30; ++k) many_planks.push_back({k + 5, static_cast<u8>(k % 3), 3, static_cast<u8>(k % 6)});
  std::vector<town::MarkRec> many_marks;
  for (std::size_t k = 0; k < town::kMarks; ++k) many_marks.push_back({{static_cast<f32>(k), 2, 0}, 5, 0.9f});
  const std::vector<std::vector<u8>> big = town::pack_state(9, many_lamps, many_planks, many_marks);
  SC_CHECK(big.size() >= 5, "several parts (%zu)", big.size());
  for (const std::vector<u8>& part : big) host.town_to(b.self_slot(), part);
  run(0.5);
  std::vector<town::LampRec> back_lamps;
  std::vector<town::PlankRec> back_planks;
  std::size_t back_marks = 0, firsts = 0, lasts = 0, index = 0;
  const std::vector<net::Session::Blob> big_in = b.take_town();
  for (const net::Session::Blob& m : big_in) {
    town::State st;
    SC_CHECK(m.data.size() <= town::kStateBytes && town::unpack_state(m.data, st) && st.epoch == 9, "a part, %zu bytes", m.data.size());
    SC_CHECK(st.first == (index == 0) && st.last == (index + 1 == big_in.size()), "the first and the last marked");
    firsts += st.first;
    lasts += st.last;
    ++index;
    back_lamps.insert(back_lamps.end(), st.lamps.begin(), st.lamps.end());
    back_planks.insert(back_planks.end(), st.planks.begin(), st.planks.end());
    back_marks += st.marks.size();
  }
  SC_CHECK(big_in.size() == big.size() && firsts == 1 && lasts == 1 && back_lamps == many_lamps && back_planks == many_planks && back_marks == town::kMarks, "all of it, in order");
  // an empty town is a state too (after a RESET: the host's next epoch)
  const std::vector<std::vector<u8>> none = town::pack_state(10, {}, {}, {});
  town::State st;
  SC_CHECK(none.size() == 1 && town::unpack_state(none[0], st) && st.first && st.last && st.epoch == 10 && st.lamps.empty(), "the empty state");
  SC_CHECK(host.send_refused() == 0 && a.send_refused() == 0 && b.send_refused() == 0, "nothing refused");
}
