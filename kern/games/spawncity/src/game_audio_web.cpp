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

namespace sc {

void Game::osfx(const char* fn, Vec3 at, f32 a, f32 b, f32 c) {
  if (!web_audio_ || osfx_.size() > 256 * 1024) return;  // nobody drains it: never grow without bound
  char buf[192];
  std::snprintf(buf, sizeof buf, "%s[\"%s\",%.2f,%.2f,%.2f,%.5g,%.5g,%.5g]", osfx_.empty() ? "" : ",", fn, at.x, at.y, at.z, a, b, c);
  osfx_ += buf;
}

std::string Game::web_audio_state() {
  std::string s;
  char b[512];
  auto add = [&](const char* fmt, auto... v) { std::snprintf(b, sizeof b, fmt, v...); s += b; };
  auto v3 = [&](Vec3 p) { add("[%.2f,%.2f,%.2f]", p.x, p.y, p.z); };
  const Settings& st = ui_.settings;
  const Vec3 fwd = kern::normalize(cam_.target - cam_.eye, {0, 1, 0});
  add("\"paused\":%d,\"mode\":\"%s\",\"fwd\":[%.3f,%.3f,%.3f],\"L\":", ui_.paused ? 1 : 0, mode_name(mode_), fwd.x, fwd.y, fwd.z);
  v3(cam_.eye);
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
    v3(mode_ == Mode::foot ? cam_.eye : feet);
    s += "}";
  }
  // motor_sound.mjs update(model, cameraPosition) / syncFcState(fcState, arm)
  if (drone_out_) {
    const DroneTelemetry& t = drone_.telemetry();
    add(",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":", t.state,
        sticks_.arm ? 1 : 0, drone_.params().prop_d, t.omega[0], t.omega[1], t.omega[2], t.omega[3], t.motor_torque[0], t.motor_torque[1], t.motor_torque[2],
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
    for (const PoliceUnit& u : units_) {
      if (u.car < 0 || u.state == PoliceUnit::State::abandoned || u.state == PoliceUnit::State::wreck || u.state == PoliceUnit::State::leave) continue;
      if (mode_ == Mode::car && car_index_ == u.car) continue;
      if (!first) s += ",";
      v3(matrix_of(cars_[u.car].actor).translation() + Vec3{0, 0, 0.9f});
      first = false;
    }
    s += "],\"pd\":[";
    first = true;
    for (const PoliceDrone& d : pdrones_) {
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
  if (mode_ == Mode::jet && jet_placed_)
    add(",\"jet\":{\"flight\":%d,\"lever\":%.3f,\"nozzle\":%.3f,\"thrust\":%.0f,\"dry\":%.0f,\"cockpit\":%d}", jet_.mode() == Jet::Mode::flight ? 1 : 0, jet_.lever(),
        jet_.nozzle(), jet_.thrust(), jet_.spec().thrust_dry, jet_cockpit_ ? 1 : 0);
  // guided_missile.mjs: missiles whose motor burns (a voice each, panned at the missile)
  s += ",\"gm\":[";
  {
    bool first = true;
    for (const guided::Flight& f : guided_) {
      if (!f.m.motor) continue;
      add("%s[%.3f,%.2f,%.2f,%.2f]", first ? "" : ",", f.m.born, f.m.p.x, f.m.p.y, f.m.p.z);
      first = false;
    }
  }
  add("],\"gps\":%d", ui_.gps.active() ? 1 : 0);
  return s;
}

bool Game::web_audio_command(const std::string& cmd, std::string& out) {
  if (cmd == "audio frame") {
    out = "{\"e\":[" + osfx_ + "]," + web_audio_state() + "}";
    osfx_.clear();
    return true;
  }
  if (cmd.rfind("audio web ", 0) == 0) {
    web_audio_ = cmd.substr(10) != "0";
    osfx_.clear();
    out = web_audio_ ? "web" : "kern";
    return true;
  }
  return false;
}

}  // namespace sc
