// SpawnCity on Kern: the original's sound on the page (web/audio.mjs). There every sound comes from
// spawncity.de's own Web Audio code - its audio modules unchanged, the sound functions of its
// gameplay modules cut out of their source (web/audio/original_sfx.mjs) and its music file - so it
// sounds exactly like spawncity.de. The game only says what happened and what is running:
//
//   * audio events (osfx): "<module>.<function>" with a place and up to three numbers, queued at the
//     moment the original calls that function (every call site names it);
//   * the audio state: listener, mode, drone motors, the car, jetpack, sirens, jets, missiles.
//
// "audio web 1" (the page, when it runs the original's modules) switches it on; from then on Kern's
// own mixer still runs but its output is dropped (only the voice chat of the other players is mixed,
// game_net.cpp), and the car radio is the page's (car_radio.mjs). Native builds keep Kern's sound.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include <cstdio>

#include "game.h"
#include "game_combat.h"
#include "game_gaps.h"
#include "game_life.h"

namespace sc {

void Game::osfx(const char* fn, Vec3 at, f32 a, f32 b, f32 c) {
  if (!audio_.web_audio || audio_.osfx.size() > 256 * 1024) return;  // nobody drains it: never grow without bound
  char buf[192];
  std::snprintf(buf, sizeof buf, "%s[\"%s\",%.2f,%.2f,%.2f,%.5g,%.5g,%.5g]", audio_.osfx.empty() ? "" : ",", fn, at.x, at.y, at.z, a, b, c);
  audio_.osfx += buf;
}

// world_action_feedback.mjs materialKind(hit): a person / player -> flesh; a car, bus, vehicle or
// lamp (or a metal object, metalness > .32: the police drones) -> metal; a tree -> wood; else
// concrete. Codes for audio.mjs: 1 concrete, 2 metal, 3 flesh, 4 wood.
f32 Game::hit_material(Vec3 at, u8 surface, bool flesh) const {
  if (flesh) return 3;
  if (surface == 2) return 2;  // a car or a police drone (hit_scan)
  if (gaps_)
    for (i32 i : gaps_->lamps.near(at.x, at.y, 2.5f)) {
      const lamps::Lamp& l = gaps_->lamps.all()[static_cast<std::size_t>(i)];
      if (l.state < 2 && (std::hypot(at.x - l.foot.x, at.y - l.foot.y) < lamps::pole_radius() + 0.25f || std::hypot(at.x - l.head.x, at.y - l.head.y) < 0.6f) && at.z < l.foot.z + lamps::kPoleH + 0.4f)
        return 2;
    }
  if (glife_)
    for (const Life::Tree& t : glife_->life.trees()) {
      // the tree as life_art.cpp draws it: a trunk 2.3 m and a crown ball at 2.75 m (+ 15 cm)
      const f32 s = t.scale, h = std::hypot(at.x - t.foot.x, at.y - t.foot.y);
      if (h < 0.3f * s && at.z > t.foot.z - 0.2f && at.z < t.foot.z + 2.4f * s) return 4;
      const f32 cx = h / (1.0f * s + 0.15f), cz = (at.z - t.foot.z - 2.75f * s) / (1.2f * s + 0.15f);
      if (cx * cx + cz * cz < 1) return 4;
    }
  return 1;
}

std::string Game::web_audio_state() {
  std::string s;
  char b[512];
  auto add = [&](const char* fmt, auto... v) { std::snprintf(b, sizeof b, fmt, v...); s += b; };
  auto v3 = [&](Vec3 p) { add("[%.2f,%.2f,%.2f]", p.x, p.y, p.z); };
  const Settings& st = ui_.settings;
  const Vec3 fwd = kern::normalize(view_.cam.target - view_.cam.eye, {0, 1, 0});
  add("\"paused\":%d,\"mode\":\"%s\",\"fwd\":[%.3f,%.3f,%.3f],\"L\":", ui_.paused ? 1 : 0, mode_name(mode_), fwd.x, fwd.y, fwd.z);
  v3(view_.cam.eye);
  // the settings in the original's names (audio_settings.mjs normalizeAudioSettings)
  add(",\"set\":{\"soundEnabled\":%d,\"musicEnabled\":%d,\"musicVolume\":%.0f,\"droneVolume\":%.0f,\"shotsVolume\":%.0f,\"fxVolume\":%.0f,\"footstepsVolume\":%.0f,"
      "\"vehicleVolume\":%.0f,\"ambientVolume\":%.0f}",
      st.on(Opt::audio_on) ? 1 : 0, st.on(Opt::music_on) ? 1 : 0, st.get(Opt::music_volume), st.get(Opt::audio_drone), st.get(Opt::audio_shots), st.get(Opt::audio_fx),
      st.get(Opt::audio_steps), st.get(Opt::audio_vehicle), st.get(Opt::audio_ambient));
  // __arondightWalkMode: position (the eye on foot), dead; __arondightWalkJump.airborne
  {
    kern::game::SoldierState ss;
    const bool have = world_->soldier_state(soldier_, ss);
    const Vec3 feet = matrix_of(soldier_).translation();
    const bool air = mode_ == Mode::foot && feet.z - city_.ground_height(feet.x, feet.y) > 1.15f;
    add(",\"walk\":{\"dead\":%d,\"air\":%d,\"p\":", have && ss.dead ? 1 : 0, air ? 1 : 0);
    v3(mode_ == Mode::foot ? view_.cam.eye : feet);
    s += "}";
  }
  // motor_sound.mjs update(model, cameraPosition) / syncFcState(fcState, arm)
  if (vehicle_.drone_out) {
    const DroneTelemetry& t = drone_.telemetry();
    add(",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":", t.state,
        vehicle_.sticks.arm ? 1 : 0, drone_.params().prop_d, t.omega[0], t.omega[1], t.omega[2], t.omega[3], t.motor_torque[0], t.motor_torque[1], t.motor_torque[2],
        t.motor_torque[3], t.prop_torque[0], t.prop_torque[1], t.prop_torque[2], t.prop_torque[3]);
    v3(matrix_of(drone_actor_).translation());
    s += "}";
  }
  // __arondightVehicleDrive (immersive_soundscape.mjs): active, speedMps, throttle
  if (mode_ == Mode::car && car_index_ >= 0) {
    const TrafficCar& c = cars_[car_index_];
    add(",\"car\":{\"id\":%d,\"v\":%.2f,\"th\":%.3f}", car_index_, std::fabs(c.car->forward_speed()), c.car->throttle());
  }
  // __arondightJetpack (jetpack_audio.mjs) and the other players' jetpacks (__arondightRemoteJetPos)
  add(",\"jp\":[%d,%.2f],\"rj\":[", ui_.jetpack.thrusting && !ui_.paused ? 1 : 0, ui_.jet_acc);
  {
    bool first = true;
    const auto& rs = net_.remotes();
    for (std::size_t i = 0; i < rs.size(); ++i) {
      if (!(rs[i].state.flags & net::kJetpack)) continue;
      add("%s[%zu,%.2f,%.2f,%.2f]", first ? "" : ",", i, rs[i].shown.x, rs[i].shown.y, rs[i].shown.z - 0.6f);
      first = false;
    }
  }
  // player_vehicle_runtime_v2.mjs: the other players on foot (their steps)
  s += "],\"rp\":[";
  {
    bool first = true;
    const auto& rs = net_.remotes();
    for (std::size_t i = 0; i < rs.size(); ++i) {
      if (rs[i].state.mode != net::PlayerMode::foot) continue;
      add("%s[%u,%.2f,%.2f,%.2f,%d]", first ? "" : ",", static_cast<unsigned>(rs[i].state.slot), rs[i].person_shown.x, rs[i].person_shown.y, rs[i].person_shown.z,
          (rs[i].state.flags & net::kJetpack) ? 1 : 0);
      first = false;
    }
  }
  // police_sirens.mjs: cruisers with lights (not wrecked, abandoned, leaving or driven by the player),
  // police drones (active, not retreating, not knocked out)
  s += "],\"pc\":[";
  {
    bool first = true;
    for (const PoliceUnit& u : police_.units) {
      if (u.car < 0 || u.state == PoliceUnit::State::abandoned || u.state == PoliceUnit::State::wreck || u.state == PoliceUnit::State::leave) continue;
      if (mode_ == Mode::car && car_index_ == u.car) continue;
      if (!first) s += ",";
      v3(matrix_of(cars_[u.car].actor).translation() + Vec3{0, 0, 0.9f});
      first = false;
    }
    s += "],\"pd\":[";
    first = true;
    for (const PoliceDrone& d : police_.pdrones) {
      if (!d.active || d.retreating || d.emp) continue;
      if (!first) s += ",";
      v3(d.pos);
      first = false;
    }
  }
  // fighter_jets.mjs: each jet flies north at SPEED from (x, y0, z) since t0 (seconds before now)
  s += "],\"fj\":[";
  {
    const f32 now = world_->sim_time();
    bool first = true;
    for (const FighterJet& j : combat_.jets) {
      add("%s[%.2f,%.2f,%.2f,%.3f]", first ? "" : ",", j.x, j.y0, j.z, now - j.t0);
      first = false;
    }
  }
  s += "]";
  // jet_mode.mjs updateVoice: af.mode, lever, nozzle, thrust / JET_SPEC.thrustDry, the cockpit camera
  if (mode_ == Mode::jet && vehicle_.jet_placed)
    add(",\"jet\":{\"flight\":%d,\"lever\":%.3f,\"nozzle\":%.3f,\"thrust\":%.0f,\"dry\":%.0f,\"cockpit\":%d}", jet_.mode() == Jet::Mode::flight ? 1 : 0, jet_.lever(),
        jet_.nozzle(), jet_.thrust(), jet_.spec().thrust_dry, view_.jet_cockpit ? 1 : 0);
  // guided_missile.mjs: missiles whose motor burns (a voice each, panned at the missile)
  s += ",\"gm\":[";
  {
    bool first = true;
    for (const guided::Flight& f : guided_.flights) {
      if (!f.m.motor) continue;
      add("%s[%.3f,%.2f,%.2f,%.2f]", first ? "" : ",", f.m.born, f.m.p.x, f.m.p.y, f.m.p.z);
      first = false;
    }
  }
  // rockets in flight (the launcher's, the drone's, the jet's): their motor hisses on the way
  // (the guided missile's voice, guided_missile.mjs motorVoice) - kind 1 foot, 2 drone, 3 jet
  s += "],\"rk\":[";
  {
    bool first = true;
    for (const Projectile& p : weapons_.projectiles) {
      if (!p.alive || p.guided) continue;
      const int kind = p.id == weapons::Id::rocket ? 1 : p.id == weapons::Id::drone_rockets ? 2 : p.id == weapons::Id::jet_missile ? 3 : 0;
      if (!kind) continue;
      add("%s[%.3f,%.2f,%.2f,%.2f,%d]", first ? "" : ",", p.born + 0.0001f * static_cast<f32>(kind), p.pos.x, p.pos.y, p.pos.z, kind);
      first = false;
    }
  }
  add("],\"gps\":%d", ui_.gps.active() ? 1 : 0);
  return s;
}

bool Game::web_audio_command(const std::string& cmd, std::string& out) {
  if (cmd == "audio frame") {
    out = "{\"e\":[" + audio_.osfx + "]," + web_audio_state() + "}";
    audio_.osfx.clear();
    return true;
  }
  if (cmd == "shoot") {
    // tests: one round of the foot weapon in hand through the middle of the view (the touch path)
    out = mode_ == Mode::foot && ctl_touch_shot(frame_w_ / 2, frame_h_ / 2) ? "reload" : "ok";
    return true;
  }
  if (cmd.rfind("audio web ", 0) == 0) {
    audio_.web_audio = cmd.substr(10) != "0";
    audio_.osfx.clear();
    out = audio_.web_audio ? "web" : "kern";
    return true;
  }
  return false;
}

}  // namespace sc
