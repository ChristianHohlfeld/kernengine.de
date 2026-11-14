// SpawnCity on Kern: multiplayer in the game (see net_session.h, mp_rules.h) - this player's
// pose out, the others drawn from their pose timelines (bodies, parked drones and jets), hits
// judged by the host and applied by the victim, kill feed and scores, remote effects, spawn
// anchor, respawn near where one went down with spawn protection, proximity voice (mixed into
// the game's audio), ping markers, the big map with destination, route and guidance, and the
// HUD data the page draws (nameplates, feed, board, hit marker, damage direction, shield).
#include <algorithm>
#include <cmath>
#include <cstdio>

#include "game.h"
#include "game_world.h"
#include "person_hit.h"
#include "mp_art.h"
#include "mp_rules.h"
#include "qr_code.h"
#include "tiles.h"

namespace sc {

namespace {
constexpr f32 kSeatDrop = 0.42f;  // a seated passenger: hips 0.5 m over the road instead of 0.92 (game_seats.cpp)
// Body colours of the cars the players drive (by the car's colour index).
const Vec3 kCarColors[] = {{0.85f, 0.2f, 0.15f}, {0.15f, 0.45f, 0.85f}, {0.95f, 0.75f, 0.1f}, {0.2f, 0.7f, 0.3f}, {0.7f, 0.3f, 0.8f}, {0.95f, 0.5f, 0.15f},
                           {0.1f, 0.75f, 0.75f}, {0.9f, 0.9f, 0.9f}};
constexpr f64 kTracerSpeed = 210;     // vs_multiplayer.mjs spawnRemoteShot: 210 m/s
constexpr f64 kTracerLifeMs = 1600;
constexpr f64 kExplosionMs = 780, kImpactMs = 320;
constexpr f64 kFeedMs = 5200;         // mp_fun_hud.mjs: a kill line stays 5.2 s
constexpr f64 kHitMarkerMs = 110, kKillMarkerMs = 260, kHurtMs = 140;
constexpr f64 kLastHitCreditMs = 10000;  // a death within 10 s of a mate's hit is his kill
constexpr f64 kNavRerouteMs = 10000;  // world_map_full.mjs: a new route at most every 10 s
constexpr f32 kArriveM = 25;

std::string esc(const std::string& s) {
  std::string o;
  for (char c : s) {
    if (c == '"' || c == '\\') { o += '\\'; o += c; }
    else if (static_cast<unsigned char>(c) < 0x20) o += ' ';
    else o += c;
  }
  return o;
}
std::string hex(u32 c) {
  char b[16];
  std::snprintf(b, sizeof b, "#%06x", c & 0xffffff);
  return b;
}
mp::Weapon mp_weapon(weapons::Id id) {
  using weapons::Id;
  switch (id) {
    case Id::smg: return mp::Weapon::smg;
    case Id::glock: return mp::Weapon::glock;
    case Id::sniper: return mp::Weapon::sniper;
    case Id::fists: return mp::Weapon::fists;
    case Id::knife: return mp::Weapon::knife;
    case Id::grenade_launcher: return mp::Weapon::launcher;
    case Id::rocket: return mp::Weapon::rocket;
    case Id::hand_grenade: return mp::Weapon::grenade;
    case Id::drone_gun: return mp::Weapon::drone_gun;
    case Id::drone_rockets: case Id::drone_guided: return mp::Weapon::drone_rocket;
    case Id::jet_cannon: return mp::Weapon::jet_gun;
    case Id::jet_missile: return mp::Weapon::jet_missile;
    case Id::jet_bomb: return mp::Weapon::jet_bomb;
  }
  return mp::Weapon::other;
}
}  // namespace

MpLocal& Game::mp_local() {
  if (!mp_) mp_ = std::make_unique<MpLocal>();
  if (!mp_->art.ready() && engine_) mp_->art.init(engine_->renderer);
  return *mp_;
}

// ---------------------------------------------------------------------------------------------
// the WORLD switches: the host's apply to everybody

namespace {
// world.* switches that make the world (friendly fire is the rule voted on: game_rules_sync)
constexpr Opt kWorldRules[] = {Opt::world_people, Opt::world_traffic, Opt::world_dogs, Opt::world_cats, Opt::world_birds, Opt::world_zombies, Opt::world_police,
                               Opt::world_military, Opt::world_flak, Opt::world_jetpack, Opt::world_terrain, Opt::world_guided, Opt::world_always_day};
}  // namespace

bool Game::voice_mic_on() const { return mp_ && mp_->mic && !mp_->muted && net_.active(); }

bool Game::world_rule_locked(Opt o) const {
  if (!net_.active() || net_.hosting()) return false;
  for (Opt w : kWorldRules) if (w == o) return true;
  return false;
}

void Game::net_world_rules() {
  Settings& st = ui_.settings;
  MpLocal& m = mp_local();
  if (!net_.active()) {
    // out of the session: our own switches again
    if (m.world_saved) { for (u32 i = 0; i < std::size(kWorldRules); ++i) st.set(kWorldRules[i], m.world_own[i]); m.world_saved = false; }
    return;
  }
  if (net_.hosting()) {
    u16 mask = 0;
    for (u32 i = 0; i < std::size(kWorldRules); ++i) if (st.on(kWorldRules[i])) mask |= static_cast<u16>(1u << i);
    net_.set_world_rules(mask);
    return;
  }
  u16 mask = 0;
  if (!net_.world_rules(mask)) return;
  if (!m.world_saved) { for (u32 i = 0; i < std::size(kWorldRules); ++i) m.world_own[i] = st.get(kWorldRules[i]); m.world_saved = true; }
  for (u32 i = 0; i < std::size(kWorldRules); ++i) {
    const f32 v = (mask >> i) & 1u ? 1.0f : 0.0f;
    if (st.get(kWorldRules[i]) != v) st.set(kWorldRules[i], v);
  }
}

std::string Game::world_rules_json() const {
  std::string o = net_.active() && !net_.hosting() ? ",\"worldLocked\":true,\"worldRules\":{" : ",\"worldLocked\":false,\"worldRules\":{";
  bool first = true;
  for (Opt w : kWorldRules) {
    const char* key = opt_spec(w).key;  // "world.people"
    o += first ? "\"" : ",\"";
    o += key + 6;
    o += ui_.settings.on(w) ? "\":true" : "\":false";
    first = false;
  }
  return o + "}";
}

// ---------------------------------------------------------------------------------------------
// this player's state

net::PlayerState Game::my_state() const {
  net::PlayerState s;
  s.mode = static_cast<net::PlayerMode>(mode_);
  const kern::game::ActorId a = mode_ == Mode::car && car_index_ >= 0 ? cars_[car_index_].actor : mode_ == Mode::jet ? jet_actor_
                                : mode_ == Mode::drone ? drone_actor_
                                : mode_ == Mode::parachute ? chute_actor_
                                                           : soldier_;
  const BodyState b = state_of(a);
  s.position = b.position;
  s.velocity = b.velocity;
  s.rotation = b.rotation;
  s.avatar = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
  s.avatar_yaw = world_->camera_yaw() * kDeg;
  s.yaw = world_->camera_yaw() * kDeg;
  s.pitch = world_->camera_pitch() * kDeg;
  if (engine_ && engine_->xr) {
    // VR: where the gun points (the right hand's ray) is the aim the others see - yaw and pitch
    Vec3 from, dir;
    if (mode_ == Mode::foot && vr_hand_ray(from, dir)) { s.yaw = std::atan2(dir.y, dir.x); s.pitch = -std::asin(clampf(dir.z, -1, 1)); }
    else { const Vec3 f = engine_->xr_forward; s.pitch = -std::asin(clampf(f.z, -1, 1)); }
  }
  s.speed = std::hypot(b.velocity.x, b.velocity.y);
  const kern::game::Hud& h = world_->hud();
  s.health = h.health >= 0 ? clampf(h.health / std::fmax(1.0f, h.max_health) * 100, 0, 100) : 100;
  s.stars = static_cast<u8>(wanted::stars());
  s.weapon = static_cast<u8>(weapons_.foot_weapon);
  // the hands and the blows (the others draw his arms there: game_net.cpp draw)
  s.hands = 0;
  for (kern::i8& v : s.hand) v = 0;
  if (mode_ == Mode::foot && !player_dead_) {
    const f32 tnow = world_->sim_time();
    const Vec3 feet = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
    if (engine_ && engine_->xr && vr_.on) {
      kern::Mat4 hm;
      bool any = false;
      for (i32 side = 0; side < 2; ++side)
        if (vr_hand_matrix(side, hm)) {
          const Vec3 d = hm.translation() - feet;
          // (x, y within 1.27 m of the feet, z 0 .. 2.54 m over them)
          s.hand[side * 3 + 0] = static_cast<kern::i8>(std::lround(clampf(d.x * 100, -127, 127)));
          s.hand[side * 3 + 1] = static_cast<kern::i8>(std::lround(clampf(d.y * 100, -127, 127)));
          s.hand[side * 3 + 2] = static_cast<kern::i8>(std::lround(clampf(d.z * 50 - 127, -127, 127)));
          any = true;
        }
      if (any) s.hands |= 1;
    } else if (gaps_) {
      if (tnow - gaps_->fist_last[0] < 0.22f) s.hands |= 2;
      if (tnow - gaps_->fist_last[1] < 0.22f) s.hands |= 4;
      if (weapons_.foot_weapon == weapons::Id::knife && tnow - look_.vm_fire_at < 0.3f) s.hands |= 8;
    }
  }
  // the traffic car we drive, by the host's index (his town's: game_worldsync.cpp); a car of our
  // own town (far from him) is none of his
  s.car = 255;
  if (mode_ == Mode::car && car_index_ >= 0) {
    if (net_.hosting()) s.car = static_cast<u8>(std::min(car_index_, 254));
    else if (worldsync_.puppet_world && static_cast<std::size_t>(car_index_) < worldsync_.car_net.size() && worldsync_.car_net[static_cast<std::size_t>(car_index_)] >= 0) s.car = static_cast<u8>(worldsync_.car_net[static_cast<std::size_t>(car_index_)]);
  }
  s.day = look_.tod;  // the host's phase is the session's (game_look.cpp)
  s.thrust = jet_.thrust() / std::fmax(1.0f, jet_.spec().thrust_max);
  u16 f = 0;
  if (player_dead_) f |= net::kDead;
  if (drone_.telemetry().armed) f |= net::kArmed;
  if (mode_ == Mode::car && car_index_ >= 0) {
    if (cars_[car_index_].car->spec().radius > 0.4f) f |= net::kBus;
    if (cars_[car_index_].police) f |= net::kPoliceCar;
    s.color = static_cast<u8>(car_index_ % 8);
  }
  if (jet_.gear_down()) f |= net::kGearDown;
  if (vehicle_.drone_out && mode_ != Mode::drone) {
    f |= net::kDroneOut;
    const BodyState d = state_of(drone_actor_);
    s.drone_pos = d.position;
    s.drone_rot = d.rotation;
  }
  if (vehicle_.jet_placed && mode_ != Mode::jet) {
    f |= net::kJetOut;
    const BodyState j = state_of(jet_actor_);
    s.jet_pos = j.position;
    s.jet_rot = j.rotation;
  }
  if (mp_ && mp_->mic && !mp_->muted) f |= net::kTalking;
  if (ui_.jetpack.thrusting && mode_ == Mode::foot) f |= net::kJetpack;  // game_equipment.cpp
  if (Vec3 rp; gaps_riding() && !(gaps_ && gaps_->ride_car)) {  // (a car's passenger: himself, standing in it)
    // riding in a mate's jet (game_seats.cpp): where we are is his jet, and nobody walks there
    Quat rq;
    if (gaps_ride_pose(rp, rq)) { s.position = rp; s.avatar = rp; s.rotation = rq; s.velocity = {}; s.speed = 0; }
    f |= net::kPassenger;
  }
  if (gaps_ride_car()) f |= net::kSeated;
  s.flags = f;
  return s;
}

void Game::rebase_origin(f64 lon, f64 lat) {
  // A client plays in the host's city: the same streamed map from the same origin.
  lon_ = lon;
  lat_ = lat;
  destruction_.reset(city_);
  city_.clear();
  city_.start(engine_->renderer, world_->physics(), lon_, lat_, engine_->arg_number("dem", 1) > 0 && engine_->arg_number("offline", 0) <= 0);
  if (engine_->arg_number("offline", 0) > 0) city_.go_offline();
  for (TrafficCar& c : cars_) {
    if (c.placed) world_->set_alive(c.actor, false);
    c.placed = false;
    c.unit = -1;
    c.wreck = false;
  }
  police_.units.clear();
  for (PoliceDrone& d : police_.pdrones) d.active = false;
  people_.clear();
  weapons_.projectiles.clear();
  wanted::clear();
  if (mode_ != Mode::foot) command("mode foot");
  world_placed_ = false;
  world_->set_paused(true);
  toast_ = "Mit dem Spiel verbunden - die Stadt des Hosts wird geladen";
  toast_age_ = 0;
}

// ---------------------------------------------------------------------------------------------
// per frame

void Game::update_net(f32 dt) {
  MpLocal& m = mp_local();
  ++m.frames;
  const f64 now = time_ * 1000;
  m.pings.expire(now);
  m.feed.erase(std::remove_if(m.feed.begin(), m.feed.end(), [&](const MpLocal::Feed& f) { return now - f.at > kFeedMs + 450; }), m.feed.end());
  for (MpLocal::Tracer& t : m.tracers) {
    const f32 step = static_cast<f32>(kTracerSpeed) * dt;
    t.p += kern::normalize(t.v) * std::fmin(step, t.left);
    t.left -= step;
  }
  m.tracers.erase(std::remove_if(m.tracers.begin(), m.tracers.end(), [](const MpLocal::Tracer& t) { return t.left <= 0; }), m.tracers.end());
  m.flashes.erase(std::remove_if(m.flashes.begin(), m.flashes.end(), [&](const MpLocal::Flash& f) { return now - f.born > f.life; }), m.flashes.end());
  if (world_placed_) update_nav();
  net_world_rules();  // the host's WORLD switches are everybody's (server authority); out of a session our own
  if (!net_.active()) return;

  // Changing FRIENDLY FIRE in the settings during a session asks everybody (game_rules_sync.mjs);
  // the rule that counts is the session's, and the setting follows it.
  const bool ff_setting = ui_.settings.on(Opt::world_friendly_fire);
  if (ff_setting != m.ff_setting) {
    m.ff_setting = ff_setting;
    if (ff_setting != net_.friendly_fire() && !net_.propose_rule(ff_setting) && net_.hosting() && net_.remotes().empty()) net_.set_friendly_fire_initial(ff_setting);
  }
  if (m.ff_rule != net_.friendly_fire()) {
    m.ff_rule = net_.friendly_fire();
    ui_.settings.set(Opt::world_friendly_fire, m.ff_rule ? 1.0f : 0.0f);
    m.ff_setting = m.ff_rule;
  }
  net_.update(time_, dt, world_placed_ ? my_state() : net::PlayerState{}, world_placed_);  // no pose before the world stands
  // The host is gone: the oldest of the rest takes over (lan_vs.mjs getAuthorityId), the others
  // join him - same city, same origin, everybody stays where he is (the page reopens its transport).
  if (net_.host_lost() && !m.migrate) {
    m.migrate = net_.successor() == net_.self_slot() ? 1 : 2;
    toast_ = m.migrate == 1 ? "Host weg - du uebernimmst die Sitzung" : "Host weg - die Sitzung zieht um";
    toast_age_ = 0;
    net_.stop();
  }
  // The client takes the host's map origin once.
  f64 lon, lat;
  // The client plays in the host's city: his origin on joining, and again whenever he moves it.
  if (!net_.hosting() && net_.origin(lon, lat) && (std::fabs(lon - lon_) > 1e-7 || std::fabs(lat - lat_) > 1e-7)) {
    rebased_ = true;
    rebase_origin(lon, lat);
  }
  // The joining player spawns at the host's anchor, in his own slot of a ring around it.
  Vec3 anchor;
  if (net_.take_anchor(anchor) && !m.rejoined) { m.anchor = anchor; m.anchor_pending = true; }
  if (m.anchor_pending && world_placed_ && mode_ == Mode::foot) {
    m.anchor_pending = false;
    const Vec2 o = mp::spawn_ring_offset(std::max(1u, net_.index_of(net_.self_slot()) - 1));
    Spot s{};
    if (!clear_spot(m.anchor + Vec3{o.x, o.y, 0}, {0.6f, 0.6f}, 30, s)) s = {m.anchor + Vec3{o.x, o.y, 0}, 0};
    put_down(s.p, s.yaw / kDeg);  // on the ground there as it will be (game.cpp spawn guard)
  }

  // Hits other players landed on us (the authority approved them): the owner applies them.
  const kern::game::Hud& hud = world_->hud();
  const f32 max_hp = std::fmax(1.0f, hud.max_health);
  for (const net::Hit& h : net_.take_hits()) {
    if (!world_placed_ || player_dead_) continue;
    m.last_by = h.by;
    m.last_weapon = h.weapon;
    m.last_zone = h.zone;
    m.last_by_at = now;
    m.hurt_at = now;
    m.hurt_from = h.from;
    if (mode_ == Mode::foot || mode_ == Mode::parachute || mode_ == Mode::drone) world_->damage(soldier_, h.damage * max_hp / 100, 0, h.zone == 1, h.from);
    else damage_player_vehicle(h.damage);
  }
  for (const net::Hit& h : net_.take_confirms()) {
    m.hit_at = now;
    m.hit_head = h.zone == static_cast<u8>(mp::Zone::head);
    m.hit_kill = false;
  }
  for (const net::Kill& k : net_.take_kills()) {
    m.feed.push_back({k, now});
    if (m.feed.size() > 5) m.feed.erase(m.feed.begin());
    if (k.by == net_.self_slot() && k.victim != net_.self_slot()) {
      m.hit_at = now;
      m.hit_kill = true;
      wanted::report(wanted::Crime::player, true);  // vs_multiplayer.mjs: a pvp kill is a crime
    }
  }
  for (const net::Fx& f : net_.take_fx()) {
    switch (f.kind) {
      case net::Fx::shot: {
        // The tracer starts where it would be by now (vs_multiplayer.mjs spawnRemoteShot).
        if (m.tracers.size() < 40) m.tracers.push_back({f.p, f.dir, std::fmax(1.0f, f.a), net_.color_of(f.owner)});
        osfx("player_vehicle_runtime_v2.playPistol", f.p);  // a mate's round: playPistol(p, true)
        // the town is the host's: his people hear a mate's shot as they hear ours (handle_shots) -
        // they run on his machine, so for everybody
        if (net_.hosting()) people_.frighten(f.p, 22, world_->sim_time(), 1);
        break;
      }
      case net::Fx::explosion: {
        projsync_blast(f.owner, f.p);  // his round ends in his blast (game_projsync.cpp)
        m.flashes.push_back({f.p, std::fmax(1.0f, f.a * 0.6f), now, kExplosionMs});
        world_->particle_burst(f.p, {3.0f, 1.2f, 0.3f});
        world_->particle_burst(f.p + Vec3{0, 0, 0.8f}, {1.1f, 1.1f, 1.1f});
        world_->play_sound("builtin:explosion", f.p, 1, 1, true);
        osfx("player_vehicle_runtime_v2.playExplosion", f.p);
        world_->shake_camera(clampf(1.4f - kern::length(view_.cam.eye - f.p) / (std::fmax(2.0f, f.a) * 3), 0, 1.2f));
        ++weapons_.explosions;
        gaps_lamps_blast(f.p, f.a);  // street lamps break on every machine (game_lamps.cpp)
        if (f.dir.y > 0.5f) destruction_.blast(f.p, f.a, f.dir.x, f.dir.y > 1.5f, city_, rng_);  // and buildings
        // the scorch and the wall marks too (the crater, if any, comes with the host's state:
        // game_destruction_sync.cpp - dug here it would be ours, not the city's)
        if (life_ && f.a >= 1.5f) town_mark(f.p, f.a, std::fmin(1.4f, f.dir.x / 110), false);
        // a mate's blast in our world hurts our people, zombies, military and police too (their own are his)
        if (f.dir.x > 0) {
          people_.blast(f.p, f.a, f.dir.x, false, world_->sim_time());
          combat_blast(f.p, f.a, f.dir.x, false);
          blast_cars(f.p, f.a, f.dir.x, false, true);        // our cruisers (the town's cars took it where it was fired)
          blast_police_drones(f.p, f.a, f.dir.x, false);
        }
        break;
      }
      case net::Fx::impact: m.flashes.push_back({f.p, 0.35f, now, kImpactMs}); break;
      case net::Fx::melee:  // a mate's blow landed there (a fist or a blade): its sound, no round
        osfx(f.a > 0.5f ? "gameplay_final_runtime_v2.knifeHit" : "gameplay_final_runtime_v2.punchThud", f.p, 0, 0);
        break;
      case net::Fx::cow_death:
      case net::Fx::push: life_net_fx(f); break;  // M3 life (game_life.cpp)
      default: gaps_remote_fx(f); break;  // sc-m3-gaps effects (game_gaps.cpp)
    }
  }
  for (const net::PingEvent& p : net_.take_pings()) {
    if (p.on) { m.pings.set(p.owner, p.p, p.color, now); osfx("ping_marker.set", p.p); }  // ping("set"): __oppUiBlip(false)
    else m.pings.clear(p.owner);
  }
  // Incoming voice: the gain from the distance between the two players, here.
  const Vec3 me = focus_position();
  for (net::VoiceFrame& v : net_.take_voice()) {
    if (m.muted) continue;
    for (net::Remote& r : net_.remotes()) {
      if (r.state.slot != v.from) continue;
      const Vec3 at = r.state.mode == net::PlayerMode::foot || r.state.mode == net::PlayerMode::drone ? r.person_shown : r.shown;
      const f32 gain = mp::voice_gain(kern::length(at - me), v.everywhere || m.everywhere);
      if (r.voice.push(v.seq, v.data, gain)) r.speaking_until = now + 450;
    }
  }
  // Our microphone frames out (only while the mic is on and not muted).
  std::vector<u8> frame;
  while (m.capture.take(frame))
    if (m.mic && !m.muted) net_.send_voice(++m.seq, m.everywhere, frame);

  // Death and respawn (vs_combat_presentation.mjs): the death goes to the authority (with the
  // mate whose hit it was), the respawn is 3..5 m from where it happened, then spawn protection.
  if (world_placed_) {
    if (player_dead_ && !m.was_dead) {
      m.death_feet = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
      const bool credit = now - m.last_by_at < kLastHitCreditMs;
      net_.report_death(credit ? m.last_by : net::kNobody, credit ? m.last_weapon : 0, credit ? m.last_zone : 0);
    } else if (!player_dead_ && m.was_dead) {
      const Vec2 o = mp::respawn_offset(rng_);
      Spot s{};
      if (!clear_spot(m.death_feet + Vec3{o.x, o.y, 0}, {0.6f, 0.6f}, 24, s)) s = {m.death_feet + Vec3{o.x, o.y, 0}, 0};
      if (mode_ == Mode::foot) put_down(s.p, s.yaw / kDeg);
      net_.report_respawn();
      m.last_hp = -1;
    }
    m.was_dead = player_dead_;
    // Health comes back after 5 quiet seconds, 12 HP/s (vs_multiplayer.mjs authorityRegen).
    const f32 hp = hud.health >= 0 ? hud.health / max_hp * 100 : 100;
    if (m.last_hp >= 0 && hp < m.last_hp - 0.01f) m.last_hurt = now;
    m.last_hp = hp;
    if (now - m.regen_clock >= mp::kRegenTickMs) {
      const f32 step = static_cast<f32>(std::fmin(2.0, (now - m.regen_clock) / 1000));
      m.regen_clock = now;
      const f32 next = mp::regen(hp, now, m.last_hurt, step);
      if (!player_dead_ && next > hp) { world_->heal(soldier_, (next - hp) * max_hp / 100); m.last_hp = next; }
    }
  }

  // Present the others: their timelines sampled 90 ms behind on their own clocks.
  for (net::Remote& r : net_.remotes()) {
    const mp::PoseSample b = r.body.sample(now), p = r.person.sample(now);
    if (b.valid) { r.shown = b.p; r.shown_rotation = b.q; }
    if (p.valid) r.person_shown = p.p;
    // his feet on our ground: his height field may be refined differently - walking he neither
    // floats nor wades (a jump, a roof, the jetpack stay as he sent them)
    // his car on our ground: never sunk into it (his ground may lie lower than ours)
    if (b.valid && r.state.mode == net::PlayerMode::car) {
      const f32 g = city_.ground_height(r.shown.x, r.shown.y);
      if (std::isfinite(g) && r.shown.z < g + 0.45f && r.shown.z > g - 4) r.shown.z = g + 0.45f;
    }
    if (p.valid && (r.state.mode == net::PlayerMode::foot || r.state.mode == net::PlayerMode::drone)) {
      const f32 g = city_.ground_height(r.person_shown.x, r.person_shown.y);
      if (std::isfinite(g) && r.person_shown.z < g + 0.9f && r.person_shown.z > g - 4) r.person_shown.z = g;
    }
    r.phase += r.state.speed * dt * 3.4f;
    r.shown_hp += (r.state.health - r.shown_hp) * damp_alpha(10, dt);
  }
}

// ---------------------------------------------------------------------------------------------
// input (H mic, K mute, Z / middle mouse ping, M big map; pad R3 / R3+Y / R3+right / Menu)

void Game::mp_input(kern::Engine& e, f32 dt) {
  (void)dt;
  MpLocal& m = mp_local();
  (void)e;
  // H / K, Z / middle mouse, M and the pad's R3, R3+Y, D-pad right, VIEW hold are read with every
  // other control in game_controls.cpp (voice_control_bindings.mjs, ping_marker.mjs, pad_actions.mjs).
  // The vote is answered with the JA / NEIN buttons.
  if (m.map_open) {
    // The big map takes the pad (the page pans / zooms with it); the game gets nothing.
    pad.lx = pad.ly = pad.rx = pad.ry = pad.lt = pad.rt = 0;
    pad.buttons = pad.pressed = 0;
  }
}

void Game::voice_capture(kern::Span<const f32> samples, u32 rate) {
  MpLocal& m = mp_local();
  if (m.mic && !m.muted) m.capture.push(samples, rate);
}

void Game::mix_voice(std::vector<f32>& out, u32 frames) {
  if (!net_.active() || !mp_ || !audio_.mixer.ready()) return;
  for (net::Remote& r : net_.remotes()) r.voice.render(out, frames, audio_.mixer.sample_rate());
}

// ---------------------------------------------------------------------------------------------
// shots and blasts against the other players

// The mate closest along a segment (his body as it is drawn, a sphere around his vehicle): -1 for
// none; `t_out` the share of the segment where it touches him. Rounds, blows, grenades and rockets
// all ask this - a mate's body stops them on every machine as a passer-by's does.
i32 Game::remote_on_segment(Vec3 from, Vec3 to, f32& t_out, mp::Zone& zone_out) {
  t_out = 2;
  zone_out = mp::Zone::torso;
  if (!net_.active()) return -1;
  const Vec3 seg = to - from;
  const f32 l2 = std::fmax(1e-6f, kern::dot(seg, seg));
  i32 best = -1;
  f32 best_t = 2;
  const std::vector<net::Remote>& rs = net_.remotes();
  // player_vehicle_runtime_v2.mjs vs hit proxies: head, torso and legs turned with the player, here
  // the pose as it is drawn (person_hit.h: the running lean, x 1.16 vs_multiplayer.mjs
  // HITBOX_PADDING); the round's entry decides the zone (a steep shot through the head is a head shot)
  mp::Zone best_zone = mp::Zone::torso;
  auto zones = [&](Vec3 feet, f32 yaw, f32 speed, f32& t_hit, mp::Zone& z) {
    const BodyPart part = person_hit::hit(feet, yaw, speed, from, seg, t_hit, 1.16f);
    z = part == BodyPart::head ? mp::Zone::head : part == BodyPart::legs ? mp::Zone::legs : mp::Zone::torso;
    return part != BodyPart::none;
  };
  for (std::size_t i = 0; i < rs.size(); ++i) {
    const net::Remote& r = rs[i];
    if (r.state.flags & (net::kDead | net::kPassenger)) continue;  // a passenger rides in the vehicle hit as one
    if (gaps_riding() && r.state.slot == gaps_->ride_owner) continue;  // the car we ride in: our rounds leave it
    if (r.state.mode == net::PlayerMode::foot) {
      f32 t = 2;
      mp::Zone z = mp::Zone::torso;
      const Vec3 feet = (r.state.flags & net::kSeated) ? r.person_shown - Vec3{0, 0, kSeatDrop} : r.person_shown;
      if (zones(feet, r.state.avatar_yaw, r.state.speed, t, z) && t < best_t) { best_t = t; best = static_cast<i32>(i); best_zone = z; }
      continue;
    }
    // the drone's pilot stands where he is drawn (the remote in his hands) - he can be shot too
    if (r.state.mode == net::PlayerMode::drone) {
      f32 t = 2;
      mp::Zone z = mp::Zone::torso;
      if (zones(r.person_shown, r.state.avatar_yaw, 0, t, z) && t < best_t) { best_t = t; best = static_cast<i32>(i); best_zone = z; }
    }
    // the parachutist hangs under his canopy (draw_remote_people: harness .85 m below, feet .9 m below that)
    if (r.state.mode == net::PlayerMode::parachute) {
      f32 t = 2;
      mp::Zone z = mp::Zone::torso;
      if (zones(r.shown - Vec3{0, 0, 1.75f}, r.state.yaw, 0, t, z) && t < best_t) { best_t = t; best = static_cast<i32>(i); best_zone = z; }
    }
    // A capsule on foot (and the drone pilot), a sphere around a vehicle.
    const bool person = r.state.mode == net::PlayerMode::foot;
    Vec3 c = person ? r.person_shown + Vec3{0, 0, (r.state.flags & net::kSeated) ? 0.73f - kSeatDrop : 0.73f} : r.shown;
    f32 radius = 0.45f * 1.16f;  // vs_multiplayer.mjs HITBOX_PADDING
    switch (r.state.mode) {
      case net::PlayerMode::foot: c += Vec3{0, 0, 0.2f}; break;
      case net::PlayerMode::parachute: radius = 0.6f; break;
      case net::PlayerMode::drone: radius = 0.3f; break;
      case net::PlayerMode::car: radius = r.state.flags & net::kBus ? 3.2f : 2.0f; break;
      case net::PlayerMode::jet: radius = 5.5f; break;
    }
    const f32 t = clampf(kern::dot(c - from, seg) / l2, 0, 1);
    const Vec3 p = from + seg * t;
    f32 d = kern::length(p - c);
    if (person) {
      const Vec3 q{c.x, c.y, clampf(p.z, c.z - 0.93f, c.z + 0.9f)};
      d = kern::length(p - q);
    }
    if (d < radius && t < best_t) { best_t = t; best = static_cast<i32>(i); best_zone = mp::Zone::torso; }
  }
  t_out = best_t;
  zone_out = best_zone;
  return best;
}

bool Game::hit_remotes(Vec3 from, Vec3& to, f32 damage, bool melee) {
  (void)damage;
  if (!net_.active()) return false;
  const weapons::Id wid = mode_ == Mode::drone ? (weapons_.drone_weapon == 0 ? weapons::Id::drone_gun : weapons::Id::drone_rockets)
                          : mode_ == Mode::jet ? weapons::Id::jet_cannon
                                               : weapons_.foot_weapon;
  const Vec3 seg = to - from;
  const f32 l2 = std::fmax(1e-6f, kern::dot(seg, seg));
  std::vector<net::Remote>& rs = net_.remotes();
  f32 best_t = 2;
  mp::Zone best_zone = mp::Zone::torso;
  const i32 best = remote_on_segment(from, to, best_t, best_zone);
  // Everybody sees the round fly (a tracer in our colour up to where it ends) - a blow is no
  // round: only when it lands they hear it there
  if (!melee) {
    net::Fx shot;
    shot.kind = net::Fx::shot;
    shot.p = from;
    shot.dir = kern::normalize(seg);
    shot.a = std::sqrt(l2) * (best >= 0 ? best_t : 1);
    net_.send_fx(shot);
  } else if (best >= 0) {
    net::Fx blow;
    blow.kind = net::Fx::melee;
    blow.p = from + seg * best_t;
    blow.dir = kern::normalize(seg);
    blow.a = damage >= 99 ? 1.0f : 0.0f;  // the knife
    net_.send_fx(blow);
  }
  if (best < 0) return false;
  to = from + seg * best_t;
  const net::Remote& r = rs[static_cast<std::size_t>(best)];
  const bool body_zone = r.state.mode == net::PlayerMode::foot || r.state.mode == net::PlayerMode::drone || r.state.mode == net::PlayerMode::parachute;
  const mp::Zone zone = body_zone ? best_zone : mp::Zone::torso;
  mp_local().shot_zone = static_cast<u8>(zone);
  const mp::Weapon w = mp_weapon(wid);
  net_.request_hit(r.state.slot, mp::round_damage(w, zone), static_cast<u8>(w), static_cast<u8>(zone), from);
  return true;
}

void Game::blast_shown(Vec3 at, f32 radius, f32 damage) {
  if (!net_.active()) return;
  net::Fx fx;
  fx.kind = net::Fx::explosion;
  fx.p = at;
  fx.a = radius;
  fx.dir = {damage, 0, 0};
  net_.send_fx(fx);
}

void Game::blast_remotes(Vec3 at, f32 radius, f32 damage, u8 breaks) {
  if (!net_.active()) return;
  net::Fx fx;
  fx.kind = net::Fx::explosion;
  fx.p = at;
  fx.a = radius;
  // a weapon's blast breaks the same buildings on every machine (dir: damage, 1 weapon / 2 missile)
  fx.dir = {damage, static_cast<f32>(breaks), 0};
  net_.send_fx(fx);
  for (const net::Remote& r : net_.remotes()) {
    if (r.state.flags & net::kDead) continue;
    const Vec3 c = r.state.mode == net::PlayerMode::foot ? r.person_shown + Vec3{0, 0, 0.9f} : r.shown;
    const f32 d = kern::length(c - at);
    if (d < radius) {
      const f32 dmg = mp::blast_damage(damage * weapons::blast_factor(d, radius, false));
      if (dmg >= 1) net_.request_hit(r.state.slot, dmg, static_cast<u8>(mp::Weapon::explosion), 0, at);
    }
  }
}

// ---------------------------------------------------------------------------------------------
// drawing

void Game::draw_remote_people() {
  const f32 now = world_->sim_time();
  // jet_mode.mjs makeChute: under the canopy hangs the player's own character, both arms up on the
  // risers, legs a little apart - ours (seen in the third-person camera) and every mate's
  auto pilot = [&](Vec3 harness, f32 yaw, Vec3 shirt) {
    People::Person p;
    p.kind = People::Kind::citizen;
    p.shirt = shirt;
    p.pants = {0.12f, 0.13f, 0.16f};
    p.skin = {0.86f, 0.66f, 0.5f};
    p.pos = harness - Vec3{0, 0, 0.9f};
    p.yaw = yaw;
    p.v = 0;
    p.hands_up_until = now + 1;
    people_art_.person(p, now);
  };
  if (mode_ == Mode::parachute && world_placed_)
    pilot(matrix_of(chute_actor_).translation() - Vec3{0, 0, 0.85f}, world_->camera_yaw() * kDeg, net_.active() ? mp::rgb(net_.color_of(net_.self_slot())) : Vec3{0.3f, 0.36f, 0.28f});
  if (!net_.active()) return;
  for (const net::Remote& r : net_.remotes())
    if (r.state.mode == net::PlayerMode::parachute && !(r.state.flags & net::kDead) && !(r.state.flags & net::kPassenger))
      pilot(r.shown - Vec3{0, 0, 0.85f}, r.state.yaw, mp::rgb(net_.color_of(r.state.slot)));
  // the mates' beacons: a pillar of light over each one farther than 40 m (find each other)
  for (const net::Remote& r : net_.remotes()) {
    if (r.state.flags & (net::kDead | net::kPassenger)) continue;
    const Vec3 at = r.state.mode == net::PlayerMode::foot || r.state.mode == net::PlayerMode::drone ? r.person_shown : r.shown;
    const f32 d = kern::length(at - view_.cam.eye);
    if (d > 40) people_art_.beacon(at, mp::rgb(net_.color_of(r.state.slot)), clampf((d - 40) / 200, 0, 1));
  }
  for (const net::Remote& r : net_.remotes()) {
    People::Person p;
    p.kind = People::Kind::citizen;
    p.shirt = mp::rgb(net_.color_of(r.state.slot));
    p.pants = {0.12f, 0.13f, 0.16f};
    p.skin = {0.86f, 0.66f, 0.5f};
    p.phase = r.phase;
    p.yaw = r.state.avatar_yaw;
    if (r.state.flags & net::kPassenger) continue;  // in a jet (game_seats.cpp)
    if (r.state.flags & net::kDead) {
      p.state = People::State::down;
      p.fall_dir = {std::cos(r.state.avatar_yaw), std::sin(r.state.avatar_yaw), 0};
    }
    if (r.state.mode == net::PlayerMode::foot) {
      p.pos = r.person_shown;
      p.v = r.state.speed;
      // a passenger: seated in the car he rides in - the hips on the seat, the legs in the
      // footwell (out of sight under the doors), head and arms where he looks and aims
      if (r.state.flags & net::kSeated) { p.pos.z -= kSeatDrop; p.v = 0; }
      // his weapon in his hand, the arms along his aim (up / down) - a gun is always held up, the
      // fists and the knife swing; the body faces where he looks
      const weapons::Id w = static_cast<weapons::Id>(r.state.weapon);
      p.held = static_cast<i32>(r.state.weapon);
      p.aiming = w != weapons::Id::fists && w != weapons::Id::knife;
      p.aim_pitch = r.state.pitch;
      p.yaw = r.state.yaw;
      // his real hands (VR) - the arms reach them, the gun in the right one - or his blows
      p.hands = (r.state.hands & 1) != 0;
      if (p.hands) {
        auto hp = [&](i32 side) {
          return p.pos + Vec3{r.state.hand[side * 3] / 100.0f, r.state.hand[side * 3 + 1] / 100.0f, (r.state.hand[side * 3 + 2] + 127) / 50.0f};
        };
        p.hand_l = hp(0);
        p.hand_r = hp(1);
        p.aiming = false;
      }
      p.punch = static_cast<u8>((r.state.hands >> 1) & 7);
      if (p.punch) p.aiming = false;
      people_art_.person(p, now);
    } else if (r.state.mode == net::PlayerMode::drone) {
      p.pos = r.person_shown;
      p.v = 0;
      p.aiming = true;  // the remote control in both hands
      people_art_.person(p, now);
    }
  }
}

void Game::draw_remote_vehicles() {
  if (!mp_) return;
  MpLocal& m = *mp_;
  const f64 now_ms = time_ * 1000;
  const f32 now = static_cast<f32>(time_);
  if (m.art.ready()) m.art.begin();
  if (net_.active()) {
    if (!remote_car_) remote_car_ = std::make_unique<Car>(CarSpec::car());
    if (!remote_bus_) remote_bus_ = std::make_unique<Car>(CarSpec::bus());
    std::array<Vec3, 4> motors;
    std::array<f32, 4> angles;
    for (const net::Remote& r : net_.remotes()) {
      const Vec3 color = mp::rgb(net_.color_of(r.state.slot));
      auto drone_at = [&](const kern::Mat4& mtx, bool armed) {
        for (int i = 0; i < 4; ++i) {
          motors[i] = drone_.motor_position(i);
          angles[i] = armed ? now * 160.0f * (i % 2 ? 1.0f : -1.0f) : 0;
        }
        art_.drone(mtx, motors, angles, drone_.params().prop_d, armed, false);
        if (m.art.ready()) m.art.drone_accent(mtx, color);
      };
      const bool dead = r.state.flags & net::kDead;
      const kern::Mat4 mtx = kern::Mat4::trs(r.shown, r.shown_rotation, {1, 1, 1});
      if (!dead) switch (r.state.mode) {
          case net::PlayerMode::car: {
            const bool bus = r.state.flags & net::kBus;
            const Vec3 body = r.state.flags & net::kPoliceCar ? Vec3{0.93f, 0.94f, 0.96f} : bus ? Vec3{0.92f, 0.78f, 0.1f} : kCarColors[r.state.color % 8];
            art_.car(mtx, bus ? *remote_bus_ : *remote_car_, body, false, false);
            if (r.state.flags & net::kPoliceCar) people_art_.light_bar(mtx, false, world_->sim_time());
            break;
          }
          case net::PlayerMode::jet: art_.jet(mtx, r.state.flags & net::kGearDown, 0, clampf(r.state.thrust, 0.05f, 1), false); break;
          case net::PlayerMode::drone: drone_at(mtx, r.state.flags & net::kArmed); break;
          case net::PlayerMode::parachute: art_.parachute(r.shown - Vec3{0, 0, 0.85f}, r.state.yaw, true); break;
          case net::PlayerMode::foot: break;
        }
      // Parked craft: the drone where it stands, the jet where it was left (player_vehicle_runtime_v2.mjs ja).
      if (r.state.flags & net::kDroneOut) drone_at(kern::Mat4::trs(r.state.drone_pos, r.state.drone_rot, {1, 1, 1}), false);
      if (r.state.flags & net::kJetOut) art_.jet(kern::Mat4::trs(r.state.jet_pos, r.state.jet_rot, {1, 1, 1}), true, 0, 0, false);
      if ((r.state.flags & net::kProtected) && m.art.ready()) {
        const Vec3 c = r.state.mode == net::PlayerMode::foot ? r.person_shown + Vec3{0, 0, 0.9f} : r.shown;
        m.art.shield(c, r.state.mode == net::PlayerMode::foot ? 0.9f : 2.5f, now);
      }
    }
  }
  if (m.art.ready()) {
    for (const MpLocal::Tracer& t : m.tracers) m.art.tracer(t.p, t.v, 2.2f, mp::rgb(t.color) * 2.0f + Vec3{0.6f, 0.5f, 0.2f});
    for (const MpLocal::Flash& f : m.flashes) m.art.flash(f.p, f.r, static_cast<f32>((now_ms - f.born) / f.life));
    for (const mp::Ping& p : m.pings.all()) {
      const f32 d = kern::length(view_.cam.eye - p.p);
      m.art.ping(p.p, clampf(d / 22, 1, 40), mp::rgb(p.color), mp::PingBoard::fade(p, now_ms), now);
    }
    m.art.end();
  }
}

// ---------------------------------------------------------------------------------------------
// navigation (world_map_full.mjs): a destination, a route over the streamed streets (or a road
// route the page fetches from routing.openstreetmap.de when the streets do not reach it).

void Game::update_nav() {
  MpLocal& m = *mp_;
  if (!m.has_dest) return;
  const Vec3 p3 = focus_position();
  const Vec2 me{p3.x, p3.y};
  if (std::hypot(m.dest.x - me.x, m.dest.y - me.y) < kArriveM) {
    m.has_dest = false;
    m.route.clear();
    toast_ = "Ziel erreicht: " + m.dest_name;
    toast_age_ = 0;
    return;
  }
  const f64 now = time_ * 1000;
  const bool routable = mode_ == Mode::foot || mode_ == Mode::car;
  if (!routable || now - m.route_at < kNavRerouteMs) return;
  m.route_at = now;
  if (roads_.built_from != lane_signature(city_.lanes())) roads_.rebuild(city_.lanes());
  std::vector<Vec2> path;
  if (roads_.path(me, m.dest, path) && path.size() >= 2 && std::hypot(path.back().x - m.dest.x, path.back().y - m.dest.y) < 60) {
    path.push_back(m.dest);
    m.route = path;
    m.route_streets = true;
    return;
  }
  // The loaded streets do not reach it: ask the page for a road route (only in a real city).
  if (!city_.has_real_data()) { m.route = {me, m.dest}; m.route_streets = false; return; }
  f64 lon, lat;
  unproject(city_.projector(), me.x, me.y, lon, lat);
  char b[200];
  std::snprintf(b, sizeof b, "{\"id\":%u,\"from\":[%.6f,%.6f],\"to\":[%.6f,%.6f],\"profile\":\"%s\"}", ++m.fetch_id, lon, lat, m.dest_lon, m.dest_lat,
                mode_ == Mode::foot ? "foot" : "car");
  m.fetch_json = b;
  if (m.route.empty()) m.route = {me, m.dest};
}

// ---------------------------------------------------------------------------------------------
// HUD data (the page draws it: web/mp_host.mjs)

bool Game::project_screen(Vec3 p, f32& x, f32& y, f32& depth) const {
  const Vec3 f = kern::normalize(view_.cam.target - view_.cam.eye);
  Vec3 r{f.y * view_.cam.up.z - f.z * view_.cam.up.y, f.z * view_.cam.up.x - f.x * view_.cam.up.z, f.x * view_.cam.up.y - f.y * view_.cam.up.x};
  r = kern::normalize(r);
  const Vec3 u{r.y * f.z - r.z * f.y, r.z * f.x - r.x * f.z, r.x * f.y - r.y * f.x};
  const Vec3 d = p - view_.cam.eye;
  depth = kern::dot(d, f);
  const f32 t = std::tan(view_.cam.fov / 2), aspect = frame_w_ / std::fmax(1.0f, frame_h_);
  const f32 z = std::fabs(depth) < 1e-3f ? 1e-3f : std::fabs(depth);
  f32 nx = kern::dot(d, r) / (z * t * aspect), ny = kern::dot(d, u) / (z * t);
  if (depth < 0.05f) {
    // behind the camera: the side he is on, unprojected (a perspective division would mirror it -
    // a mate behind on the left showed on the right); straight behind is down
    // (the side from the horizontal: left / right by the camera's right, down by how far behind -
    // straight behind is the bottom edge, behind on the left the bottom left)
    nx = kern::dot(d, r);
    ny = depth;
    const f32 l = std::hypot(nx, ny);
    if (l < 1e-3f) { nx = 0; ny = -1; } else { nx = nx / l * 4; ny = ny / l * 4; }
  }
  x = nx;
  y = ny;
  return depth > 0 && std::fabs(nx) < 0.97f && std::fabs(ny) < 0.93f;
}

std::string Game::mp_hud_json() {
  MpLocal& m = mp_local();
  const f64 now = time_ * 1000;
  std::string s;
  char b[512];
  auto add = [&](const char* fmt, auto... v) { std::snprintf(b, sizeof b, fmt, v...); s += b; };
  const f32 w = frame_w_, h = frame_h_;
  // Screen point of a world point (vs_multiplayer.mjs renderMarker: off screen at the edge with
  // an arrow; phones keep it off the sticks).
  auto marker = [&](Vec3 p, f32& sx, f32& sy, bool& on, f32& angle) {
    f32 nx, ny, depth;
    on = project_screen(p, nx, ny, depth);
    const f32 margin = std::fmin(86.0f, w * 0.1f), hw = std::fmax(1.0f, w / 2 - margin), hh = std::fmax(1.0f, h / 2 - margin);
    f32 x = nx * w / 2, y = -ny * h / 2;
    if (!on) { const f32 k = std::fmin(hw / std::fmax(1e-3f, std::fabs(x)), hh / std::fmax(1e-3f, std::fabs(y))); x *= k; y *= k; }
    const Vec2 q = mp::safe_mobile_marker_point({w / 2 + x, h / 2 + y}, w, h, touch_mode_);
    sx = q.x;
    sy = q.y;
    angle = std::atan2(y, x) / kDeg - 90;
  };
  s += ",\"mp\":{";
  add("\"frames\":%u,\"mapOpen\":%s", m.frames, m.map_open ? "true" : "false");
  add(",\"voice\":{\"mic\":%s,\"muted\":%s,\"all\":%s,\"tx\":%u}", m.mic ? "true" : "false", m.muted ? "true" : "false", m.everywhere ? "true" : "false", m.seq);
  // Pings (labels: distance).
  s += ",\"pings\":[";
  bool first = true;
  for (const mp::Ping& p : m.pings.all()) {
    const f32 d = kern::length(view_.cam.eye - p.p), sc = clampf(d / 22, 1, 40);
    f32 nx, ny, depth;
    const bool on = project_screen(p.p + Vec3{0, 0, 3.4f * sc}, nx, ny, depth);
    add("%s{\"on\":%s,\"x\":%.0f,\"y\":%.0f,\"d\":%.0f,\"color\":\"%s\",\"fade\":%.2f,\"mine\":%s}", first ? "" : ",", on ? "true" : "false", (nx + 1) / 2 * w,
        (1 - ny) / 2 * h, d, hex(p.color).c_str(), mp::PingBoard::fade(p, now), p.owner == net_.self_slot() ? "true" : "false");
    first = false;
  }
  s += "]";
  // Navigation guidance: one line and a chevron where the route goes.
  if (m.has_dest) {
    const Vec3 p3 = focus_position();
    const Vec2 me{p3.x, p3.y};
    const f32 left = m.route.size() >= 2 ? mp::route_left(m.route, me) : std::hypot(m.dest.x - me.x, m.dest.y - me.y);
    const std::vector<mp::Crumb> crumbs = mp::sample_breadcrumbs(m.route, me, 3, 18, 14);
    const Vec2 ahead = crumbs.empty() ? m.dest : crumbs.back().p;
    const f32 bearing = std::atan2(ahead.x - me.x, ahead.y - me.y);
    // Relative to where the camera looks (clockwise from north both).
    const Vec3 look = view_.cam.target - view_.cam.eye;
    const f32 rel = wrap_pi(bearing - std::atan2(look.x, look.y));
    f32 cx, cy, ca;
    bool on;
    marker({ahead.x, ahead.y, p3.z + 1.2f}, cx, cy, on, ca);
    const mp::Turn t = mp::upcoming_turn(m.route, me);
    std::string turn;
    if (t.dir != mp::Turn::Dir::none)
      turn = std::string(t.dir == mp::Turn::Dir::left ? "links" : t.dir == mp::Turn::Dir::right ? "rechts" : "wenden") + " in " + mp::format_distance(t.distance);
    add(",\"nav\":{\"name\":\"%s\",\"dist\":\"%s\",\"arrow\":\"%s\",\"turn\":\"%s\",\"x\":%.0f,\"y\":%.0f,\"on\":%s,\"angle\":%.0f,\"streets\":%s}",
        esc(m.dest_name).c_str(), mp::format_distance(left).c_str(), mp::bearing_arrow(rel).c_str(), turn.c_str(), cx, cy, on ? "true" : "false", ca,
        m.route_streets ? "true" : "false");
  }
  if (!m.fetch_json.empty()) s += ",\"navFetch\":" + m.fetch_json;
  if (net_.active()) {
    const u8 self = net_.self_slot();
    add(",\"self\":%u,\"index\":%u,\"color\":\"%s\",\"host\":%s,\"connected\":%s,\"ff\":%s,\"vote\":\"%s\",\"votePending\":%s,\"status\":\"%s\",\"poses\":%u",
        self, net_.index_of(self), hex(net_.color_of(self)).c_str(), net_.hosting() ? "true" : "false", net_.connected() ? "true" : "false",
        net_.friendly_fire() ? "true" : "false", esc(net_.vote_text()).c_str(), net_.vote_pending() ? "true" : "false", esc(net_.status()).c_str(), net_.sent_poses());
    add(",\"hit\":%.0f,\"hitKill\":%s,\"hitHead\":%s,\"hitShow\":%s", now - m.hit_at, m.hit_kill ? "true" : "false", m.hit_head ? "true" : "false",
        now - m.hit_at < (m.hit_kill ? kKillMarkerMs : kHitMarkerMs) ? "true" : "false");
    {
      // Damage direction: where the hit came from relative to the view (mp_fun_hud.mjs onHurt).
      const Vec3 look = view_.cam.target - view_.cam.eye;
      const f32 ang = (std::atan2(look.y, look.x) - std::atan2(m.hurt_from.y - view_.cam.eye.y, m.hurt_from.x - view_.cam.eye.x)) / kDeg;
      add(",\"hurt\":%s,\"hurtAngle\":%.0f", now - m.hurt_at < kHurtMs ? "true" : "false", ang);
    }
    add(",\"shield\":%s", net_.protected_now(self, now) ? "true" : "false");
    // Players: nameplates (P1.., distance, health), the board, who is talking.
    s += ",\"players\":[";
    first = true;
    const Vec3 mine = mode_ == Mode::foot ? matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f} : focus_position();
    for (const net::Remote& r : net_.remotes()) {
      if (now - r.received_ms > mp::kStaleHoldMs) continue;
      // Over the head on foot, over the craft otherwise (the marker follows what the mate steers).
      const Vec3 anchor = r.state.mode == net::PlayerMode::foot ? r.person_shown + Vec3{0, 0, 2.05f} : r.shown + Vec3{0, 0, 0.45f};
      f32 x, y, ang;
      bool on;
      marker(anchor, x, y, on, ang);
      const f32 d = kern::length(anchor - view_.cam.eye);
      // where he is from us: higher or lower (his feet / craft against ours), and his bearing
      // against the view (0 ahead, + to the right, +-180 behind) - the HUD says it in words
      const Vec3 his = r.state.mode == net::PlayerMode::foot ? r.person_shown : r.shown;
      const f32 dz = his.z - mine.z;
      const Vec3 look = view_.cam.target - view_.cam.eye;
      const f32 rel = wrap_pi(std::atan2(his.x - view_.cam.eye.x, his.y - view_.cam.eye.y) - std::atan2(look.x, look.y)) / kDeg;
      // (stars: his wanted level - his own, as in the original; the plate shows it)
      add("%s{\"slot\":%u,\"index\":%u,\"color\":\"%s\",\"hp\":%.0f,\"dead\":%s,\"mode\":\"%s\",\"x\":%.0f,\"y\":%.0f,\"on\":%s,\"angle\":%.0f,\"d\":%.1f,"
          "\"dz\":%.1f,\"rel\":%.0f,"
          "\"talk\":%s,\"protected\":%s,\"wx\":%.1f,\"wy\":%.1f,\"stars\":%u}",
          first ? "" : ",", r.state.slot, net_.index_of(r.state.slot), hex(net_.color_of(r.state.slot)).c_str(), r.shown_hp, r.state.flags & net::kDead ? "true" : "false",
          mode_name(static_cast<Mode>(r.state.mode)), x, y, on ? "true" : "false", ang, d, dz, rel, now < r.speaking_until ? "true" : "false",
          r.state.flags & net::kProtected ? "true" : "false", r.shown.x, r.shown.y, static_cast<unsigned>(std::min<u8>(r.state.stars, 5)));
      first = false;
    }
    s += "],\"scores\":[";
    first = true;
    for (const mp::Score& sc : net_.scores().ranked()) {
      add("%s{\"index\":%u,\"color\":\"%s\",\"self\":%s,\"k\":%u,\"d\":%u}", first ? "" : ",", net_.index_of(sc.slot), hex(net_.color_of(sc.slot)).c_str(),
          sc.slot == self ? "true" : "false", sc.kills, sc.deaths);
      first = false;
    }
    s += "],\"feed\":[";
    first = true;
    for (auto it = m.feed.rbegin(); it != m.feed.rend(); ++it) {
      const net::Kill& k = it->k;
      const bool accident = k.by == net::kNobody || k.by == k.victim;
      add("%s{\"by\":%u,\"byColor\":\"%s\",\"bySelf\":%s,\"victim\":%u,\"victimColor\":\"%s\",\"victimSelf\":%s,\"weapon\":\"%s\",\"head\":%s,\"accident\":%s,\"age\":%.0f}",
          first ? "" : ",", accident ? 0 : net_.index_of(k.by), accident ? "#ffffff" : hex(net_.color_of(k.by)).c_str(), k.by == self ? "true" : "false",
          net_.index_of(k.victim), hex(net_.color_of(k.victim)).c_str(), k.victim == self ? "true" : "false",
          mp::weapon_label(static_cast<mp::Weapon>(k.weapon)).c_str(), k.zone == static_cast<u8>(mp::Zone::head) ? "true" : "false", accident ? "true" : "false", now - it->at);
      first = false;
    }
    s += "]";
    if (player_dead_) add(",\"respawnIn\":%.1f", std::fmax(0.0f, world_->hud().respawn_in));
  }
  return s + "}";
}

std::string Game::bigmap_json(f32 cx, f32 cy, f32 range) {
  // The big map (world_map_full.mjs, "a world it draws itself"): streets, players, pings,
  // destination and route around (cx, cy) in local metres.
  MpLocal& m = mp_local();
  std::string o;
  char b[256];
  const Vec3 me = focus_position();
  const Vec3 look = view_.cam.target - view_.cam.eye;
  f64 lon = 0, lat = 0;
  unproject(city_.projector(), me.x, me.y, lon, lat);
  static const char* kModes[] = {"foot", "drone", "car", "jet", "parachute"};
  const u32 mi = static_cast<u32>(mode_);
  std::snprintf(b, sizeof b, "{\"me\":[%.1f,%.1f,%.3f],\"lon\":%.6f,\"lat\":%.6f,\"lon0\":%.8f,\"lat0\":%.8f,\"mode\":\"%s\",\"agl\":%.0f,\"real\":%s,\"lanes\":[", me.x, me.y,
                std::atan2(look.x, look.y), lon, lat, city_.projector().lon0, city_.projector().lat0, mi < 5 ? kModes[mi] : "foot", me.z - city_.ground_height(me.x, me.y),
                city_.has_real_data() ? "true" : "false");
  o += b;
  bool first = true;
  for (const Lane& L : city_.lanes()) {
    if (!L.alive) continue;
    bool near = false;
    for (const Vec2& p : L.points)
      if (std::fabs(p.x - cx) < range && std::fabs(p.y - cy) < range) { near = true; break; }
    if (!near) continue;
    o += first ? "[" : ",[";
    first = false;
    for (std::size_t i = 0; i < L.points.size(); ++i) {
      std::snprintf(b, sizeof b, "%s%.0f,%.0f", i ? "," : "", L.points[i].x, L.points[i].y);
      o += b;
    }
    o += "]";
  }
  o += "],\"players\":[";
  first = true;
  if (net_.active())
    for (const net::Remote& r : net_.remotes()) {
      const Vec3 p = r.state.mode == net::PlayerMode::foot || r.state.mode == net::PlayerMode::drone ? r.person_shown : r.shown;
      Vec3 np = p;
      f32 heading = r.state.avatar_yaw;
      r.nav.sample(time_ * 1000, np, heading);
      std::snprintf(b, sizeof b, "%s{\"index\":%u,\"color\":\"%s\",\"x\":%.1f,\"y\":%.1f,\"mode\":\"%s\",\"dead\":%s}", first ? "" : ",", net_.index_of(r.state.slot),
                    hex(net_.color_of(r.state.slot)).c_str(), np.x, np.y, mode_name(static_cast<Mode>(r.state.mode)), r.state.flags & net::kDead ? "true" : "false");
      o += b;
      first = false;
    }
  o += "],\"pings\":[";
  first = true;
  for (const mp::Ping& p : m.pings.all()) {
    std::snprintf(b, sizeof b, "%s{\"x\":%.1f,\"y\":%.1f,\"color\":\"%s\"}", first ? "" : ",", p.p.x, p.p.y, hex(p.color).c_str());
    o += b;
    first = false;
  }
  o += "]";
  if (m.has_dest) {
    std::snprintf(b, sizeof b, ",\"dest\":{\"x\":%.1f,\"y\":%.1f,\"name\":\"%s\"},\"route\":[", m.dest.x, m.dest.y, esc(m.dest_name).c_str());
    o += b;
    for (std::size_t i = 0; i < m.route.size(); ++i) {
      std::snprintf(b, sizeof b, "%s%.0f,%.0f", i ? "," : "", m.route[i].x, m.route[i].y);
      o += b;
    }
    o += "]";
  }
  if (vehicle_.jet_placed && mode_ != Mode::jet) {
    const Vec3 j = matrix_of(jet_actor_).translation();
    std::snprintf(b, sizeof b, ",\"jet\":[%.0f,%.0f]", j.x, j.y);
    o += b;
  }
  return o + "}";
}

// ---------------------------------------------------------------------------------------------
// commands (the page and the tests)

std::string Game::net_command(const std::string& cmd) {
  MpLocal& m = mp_local();
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  const f64 now = time_ * 1000;
  if (starts("net ")) {
    // The page opened its transport: "net host <self>" / "net join <self> <server>".
    const std::string rest = cmd.substr(4);
    unsigned long long self = 0, server = 0;
    if (rest.rfind("host ", 0) == 0 && std::sscanf(rest.c_str() + 5, "%llu", &self) == 1) {
      m.rejoined = m.migrate != 0;
      m.migrate = 0;
      net_.host(self, lon_, lat_);
      // The first player sets the rule (world_options.mjs FRIENDLY FIRE, off by default).
      net_.set_friendly_fire_initial(ui_.settings.on(Opt::world_friendly_fire) || (engine_ && engine_->arg_number("ff", 0) > 0));
      m.ff_setting = ui_.settings.on(Opt::world_friendly_fire);
      return "hosting";
    }
    if (rest.rfind("join ", 0) == 0 && std::sscanf(rest.c_str() + 5, "%llu %llu", &self, &server) == 2) {
      m.rejoined = m.migrate != 0;
      m.migrate = 0;
      net_.join(self, server);
      rebased_ = false;  // the same origin after a handover: no new city (rebase only when it differs)
      return "joining";
    }
    if (rest == "migrate") return std::to_string(m.migrate);
    if (rest == "rules") { u16 w = 0; const bool have = net_.world_rules(w); return std::string(have ? "have " : "none ") + std::to_string(w) + (net_.hosting() ? " host" : " client"); }
    if (rest == "stop") { net_.stop(); return "stopped"; }
    if (rest == "swarm") {
      // Tests: the mates' units as they are shown here (game_swarm.cpp): [owner, kind, id, flags, x, y] each
      std::string o = "[";
      for (const auto& [owner, sw] : swarm_.mates)
        for (const RemoteUnit& u : sw.units) {
          char b[96];
          std::snprintf(b, sizeof b, "%s[%u,%u,%u,%u,%.1f,%.1f]", o.size() > 1 ? "," : "", static_cast<unsigned>(owner), static_cast<unsigned>(u.kind), static_cast<unsigned>(u.id),
                        static_cast<unsigned>(u.flags), u.p.x, u.p.y);
          o += b;
        }
      return o + "]";
    }
    if (rest == "remotes") {
      std::string o = "[";
      for (const net::Remote& r : net_.remotes()) {
        char b[220];
        std::snprintf(b, sizeof b, "%s{\"slot\":%u,\"mode\":%u,\"x\":%.2f,\"y\":%.2f,\"z\":%.2f,\"px\":%.2f,\"py\":%.2f,\"hp\":%.0f,\"flags\":%u}", o.size() > 1 ? "," : "",
                      r.state.slot, static_cast<unsigned>(r.state.mode), r.shown.x, r.shown.y, r.shown.z, r.person_shown.x, r.person_shown.y, r.state.health, r.state.flags);
        o += b;
      }
      return o + "]";
    }
    if (rest.rfind("hit ", 0) == 0) {
      // Tests: a shot from 3 m beside the first remote player straight at his chest.
      if (net_.remotes().empty()) return "nobody";
      const net::Remote& r = net_.remotes()[0];
      // "net hit <dmg> [head|steep]": at the chest, at the head, or steeply down through the head
      const bool head = rest.find("head") != std::string::npos, steep = rest.find("steep") != std::string::npos;
      const Vec3 c = r.state.mode == net::PlayerMode::foot ? r.person_shown + Vec3{0, 0, head || steep ? 1.66f : 1.1f} : r.shown;
      Vec3 to = steep ? c + Vec3{-2, 0, -4} : c + Vec3{-3, 0, 0};
      const Vec3 from = steep ? c + Vec3{2, 0, 4} : c + Vec3{3, 0, 0};
      const bool hit = hit_remotes(from, to, static_cast<f32>(std::atof(rest.c_str() + 4)));
      return hit ? std::string("hit ") + mp::zone_name(static_cast<mp::Zone>(mp_local().shot_zone)) : "miss";
    }
    if (rest.rfind("blast", 0) == 0) {
      if (net_.remotes().empty()) return "nobody";
      const net::Remote& r = net_.remotes()[0];
      blast_remotes((r.state.mode == net::PlayerMode::foot ? r.person_shown : r.shown) + Vec3{1, 0, 1}, 7, 110);
      return "ok";
    }
    return "usage: net host <self> | net join <self> <server> | net stop | net remotes | net swarm | net hit N | net blast";
  }
  if (starts("mp ")) {
    const std::string rest = cmd.substr(3);
    if (rest == "ff on" || rest == "ff off") return net_.propose_rule(rest == "ff on") ? "proposed" : "no";
    if (rest == "vote yes" || rest == "vote no") return net_.vote(rest == "vote yes") ? "voted" : "no";
    if (rest == "state") return "{" + mp_hud_json().substr(6);
    return "usage: mp ff on|off | mp vote yes|no | mp state";
  }
  if (starts("voice")) {
    const std::string rest = cmd.size() > 6 ? cmd.substr(6) : "";
    if (rest == "mic") {
      // A tap toggles the mic (always-on / off, proximity_voice.mjs toggleMic); un-mutes first.
      if (!m.mic && m.muted) m.muted = false;
      m.mic = !m.mic;
      if (!m.mic) m.capture.clear();
      toast_ = m.mic ? "MIC AN - Mitspieler in 70 m hoeren dich" : "MIC AUS";
      toast_age_ = 0;
      return m.mic ? "on" : "off";
    }
    if (rest == "mute") {
      m.muted = !m.muted;
      if (m.muted) { m.mic = false; m.capture.clear(); for (net::Remote& r : net_.remotes()) r.voice.reset(); }
      toast_ = m.muted ? "SPRACHCHAT STUMM" : "SPRACHCHAT AN";
      toast_age_ = 0;
      return m.muted ? "muted" : "unmuted";
    }
    if (rest == "all") { m.everywhere = !m.everywhere; return m.everywhere ? "everywhere" : "near"; }
    if (rest == "off") { m.mic = false; m.capture.clear(); return "off"; }
    if (rest == "denied") {  // the browser / the headset did not give the microphone
      m.mic = false;
      m.capture.clear();
      toast_ = "MIKROFON NICHT FREIGEGEBEN - in den Browser-Einstellungen erlauben";
      toast_age_ = 0;
      return "off";
    }
    if (starts("voice tone ")) {
      // Tests (no microphone in a headless browser): 100 ms of a 440 Hz tone as if spoken.
      std::vector<f32> tone(4800);
      for (std::size_t i = 0; i < tone.size(); ++i) tone[i] = 0.4f * std::sin(static_cast<f32>(i) * 2 * kPi * 440 / 48000);
      for (int k = 0; k < std::max(1, std::atoi(cmd.c_str() + 11)); ++k) voice_capture(tone, 48000);
      return std::to_string(m.seq);
    }
    if (rest == "state") {
      u32 speaking = 0;
      for (const net::Remote& r : net_.remotes()) speaking += r.voice.speaking() || now < r.speaking_until ? 1 : 0;
      char b[96];
      std::snprintf(b, sizeof b, "{\"mic\":%d,\"muted\":%d,\"tx\":%u,\"speaking\":%u}", m.mic, m.muted, m.seq, speaking);
      return b;
    }
    return "usage: voice mic | mute | all | off | tone N | state";
  }
  if (starts("ping")) {
    // At the crosshair (or at a screen point "ping X Y" in CSS px: the touch long press).
    if (!world_placed_) return "no world";
    Vec3 dir = kern::normalize(view_.cam.target - view_.cam.eye);
    f32 sx = 0, sy = 0;
    if (std::sscanf(cmd.c_str(), "ping %f %f", &sx, &sy) == 2) {
      const Vec3 f = dir;
      Vec3 r = kern::normalize(Vec3{f.y * view_.cam.up.z - f.z * view_.cam.up.y, f.z * view_.cam.up.x - f.x * view_.cam.up.z, f.x * view_.cam.up.y - f.y * view_.cam.up.x});
      const Vec3 u{r.y * f.z - r.z * f.y, r.z * f.x - r.x * f.z, r.x * f.y - r.y * f.x};
      const f32 t = std::tan(view_.cam.fov / 2), aspect = frame_w_ / std::fmax(1.0f, frame_h_);
      const f32 nx = sx / std::fmax(1.0f, frame_w_) * 2 - 1, ny = 1 - sy / std::fmax(1.0f, frame_h_) * 2;
      dir = kern::normalize(f + r * (nx * t * aspect) + u * (ny * t));
    }
    const RayResult hit = cast(world_->physics(), view_.cam.eye, dir, 1500, body(soldier_));
    Vec3 at;
    if (hit.hit) at = hit.point;
    else if (dir.z < -0.01f) at = view_.cam.eye + dir * (-view_.cam.eye.z / dir.z);  // to the ground plane
    else return "nothing";
    const u8 owner = net_.active() ? net_.self_slot() : net::kHost;
    const u32 color = net_.active() ? net_.color_of(owner) : 0xe8f4ff;
    const bool on = m.pings.toggle(owner, at, color, now);
    if (on) osfx("ping_marker.set", at);  // ping("set"): __oppUiBlip(false)
    net_.send_ping(on, at, color);
    return on ? "set" : "cleared";
  }
  if (starts("qr ")) {
    // The share panel's join code (qr_pairing.mjs renderQr): rows of 0 / 1.
    qr::Code c;
    if (!qr::encode(cmd.substr(3), c)) return "{}";
    std::string o = "{\"size\":" + std::to_string(c.size) + ",\"version\":" + std::to_string(c.version) + ",\"rows\":[";
    for (i32 y = 0; y < c.size; ++y) {
      o += y ? ",\"" : "\"";
      for (i32 x = 0; x < c.size; ++x) o += c.dark(x, y) ? '1' : '0';
      o += "\"";
    }
    return o + "]}";
  }
  if (starts("bigmap")) {
    const std::string rest = cmd.size() > 7 ? cmd.substr(7) : "";
    if (rest == "toggle") { m.map_open = !m.map_open; return m.map_open ? "open" : "closed"; }
    if (rest == "open") { m.map_open = true; return "open"; }
    if (rest == "close") { m.map_open = false; return "closed"; }
    f32 cx = 0, cy = 0, range = 600;
    if (rest.rfind("follow", 0) == 0 || std::sscanf(rest.c_str(), "%f %f %f", &cx, &cy, &range) < 2) {
      const Vec3 p = focus_position();
      cx = p.x;
      cy = p.y;
      if (rest.rfind("follow ", 0) == 0) range = static_cast<f32>(std::atof(rest.c_str() + 7));
    }
    return bigmap_json(cx, cy, clampf(range, 50, 3000));
  }
  if (starts("nav ")) {
    const std::string rest = cmd.substr(4);
    f64 a = 0, c = 0;
    char name[96] = "Wegpunkt";
    if (rest == "clear") { m.has_dest = false; m.route.clear(); m.fetch_json.clear(); return "cleared"; }
    if (rest.rfind("set ", 0) == 0 && std::sscanf(rest.c_str() + 4, "%lf %lf %95[^\n]", &a, &c, name) >= 2) {
      // A waypoint on the map (local metres). Tapping the pin again removes it.
      if (m.has_dest && std::hypot(m.dest.x - a, m.dest.y - c) < 12) { m.has_dest = false; m.route.clear(); return "cleared"; }
      m.dest = {static_cast<f32>(a), static_cast<f32>(c)};
      unproject(city_.projector(), a, c, m.dest_lon, m.dest_lat);
      m.dest_name = name;
    } else if (rest.rfind("geo ", 0) == 0 && std::sscanf(rest.c_str() + 4, "%lf %lf %95[^\n]", &a, &c, name) >= 2) {
      // A searched address (the page asks Photon): lon lat name.
      f64 x, y;
      city_.projector().project(a, c, x, y);
      m.dest = {static_cast<f32>(x), static_cast<f32>(y)};
      m.dest_lon = a;
      m.dest_lat = c;
      m.dest_name = name;
    } else if (rest.rfind("route ", 0) == 0) {
      // The page's road route: "route ID lon,lat lon,lat ..." (only for the newest request).
      std::size_t at = 6;
      const u32 id = static_cast<u32>(std::strtoul(rest.c_str() + at, nullptr, 10));
      if (id != m.fetch_id) return "stale";
      m.fetch_json.clear();
      std::vector<Vec2> pts;
      at = rest.find(' ', at);
      while (at != std::string::npos && pts.size() < 4000) {
        f64 lo, la;
        if (std::sscanf(rest.c_str() + at + 1, "%lf,%lf", &lo, &la) != 2) break;
        f64 x, y;
        city_.projector().project(lo, la, x, y);
        pts.push_back({static_cast<f32>(x), static_cast<f32>(y)});
        at = rest.find(' ', at + 1);
      }
      if (pts.size() >= 2) { m.route = pts; m.route_streets = true; }
      return std::to_string(pts.size());
    } else if (rest == "failed") {
      m.fetch_json.clear();
      return "ok";
    } else {
      return "usage: nav set X Y [name] | nav geo LON LAT [name] | nav route ID lon,lat ... | nav clear";
    }
    m.has_dest = true;
    m.route.clear();
    m.route_at = -1e12;
    m.fetch_json.clear();
    if (world_placed_) update_nav();
    return "set";
  }
  return "unknown command";
}

}  // namespace sc
