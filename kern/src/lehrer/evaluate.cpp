// evaluate_jump (env.h): what a jump policy does in the Studio's physics.
#include <cmath>

#include "kern/lehrer/env.h"
#include "kern/lehrer/recipe.h"
#include "kern/core/json.h"

namespace kern::lehrer {

void JumpMeter::tick(const game::GameWorld::WalkerProbe& p) {
  if (!started) { z0 = ground_z = p.position.z; started = true; }
  ++ticks;
  min_up = min_of(min_up, -p.gravity.z);
  const bool air = !p.foot_contact[0] && !p.foot_contact[1] && !p.body_contact;
  if (air) {
    if (!in_air) { liftoff_z = ground_z; flight = 0; }
    in_air = true;
    flight += kControlDt;
    longest = max_of(longest, flight);
    rise = max_of(rise, p.position.z - liftoff_z);
    air_apex = max_of(air_apex, p.position.z - z0);
  } else {
    in_air = false;
    ground_z = p.position.z;
  }
}

bool JumpMeter::finish(bool fell, const game::GameWorld::WalkerProbe& p, JumpReport& rep) const {
  const bool standing = !fell && p.foot_contact[0] && p.foot_contact[1] && !p.body_contact && p.gravity.z < -0.9f;
  const f32 tilt = kern::dm::acos(clamp(min_up, -1.0f, 1.0f)) * 57.2957795f;
  ++rep.episodes;
  rep.falls += fell ? 1 : 0;
  rep.height_mm += max_of(0.0f, air_apex) * 1000;
  rep.rise_mm += rise * 1000;
  rep.flight_s += longest;
  rep.tilt_deg += tilt;
  rep.best_height_mm = max_of(rep.best_height_mm, max_of(0.0f, air_apex) * 1000);
  rep.best_flight_s = max_of(rep.best_flight_s, longest);
  const bool real = standing && longest >= 0.06f - 1e-4f && rise >= 0.005f && tilt <= 30.0f;
  rep.jumps += real ? 1 : 0;
  return real;
}

void finish_report(JumpReport& rep) {
  if (!rep.episodes) return;
  const f32 n = static_cast<f32>(rep.episodes);
  rep.height_mm /= n;
  rep.rise_mm /= n;
  rep.flight_s /= n;
  rep.tilt_deg /= n;
}

JumpReport evaluate_jump(ActFn act, void* user, i32 episodes, u64 seed, i32 substeps, bool randomized) {
  JumpReport rep;
  for (i32 e = 0; e < episodes; ++e) {
    DuckEnv env(make_jump_task(), seed + static_cast<u64>(e) * 7919u, substeps > 0 ? substeps : game::kWalkerSubsteps);
    env.task().randomize = randomized;
    if (randomized) env.task().progress(100000);  // the curricula at their end: the full ranges
    env.reset();
    JumpMeter m;
    m.tick(env.probe());
    bool fell = false;
    for (;;) {
      f32 a[kActions];
      act(user, env.observation(), a);
      const Env::Step s = env.step(a);
      m.tick(env.probe());
      if (s.terminal) { fell = true; break; }
      if (s.timeout) break;
    }
    m.finish(fell, env.probe(), rep);
  }
  finish_report(rep);
  return rep;
}

std::string evaluate_training(const Recipe& recipe, Ppo& learner, i32 episodes) {
  if (recipe.task.compare(0, 7, "motion:") == 0) {
    const i32 n = clamp(episodes, 1, 16);
    f64 error2 = 0, samples = 0;
    i32 falls = 0;
    for (i32 run = 0; run < n; ++run) {
      std::unique_ptr<DuckEnv> env = make_env(recipe, 2000, run);
      env->task().randomize = false;
      env->reset();
      const i32 max_ticks = static_cast<i32>(env->task().episode_seconds() / kControlDt);
      for (i32 t = 0; t < max_ticks; ++t) {
        f32 a[kActions] = {}, expected[kActions] = {};
        learner.act(env->observation(), a);
        const Env::Step state = env->step(a);
        env->task().reference_action(env->steps(), expected);
        for (i32 j = 0; j < kActions; ++j) {
          const f64 d = env->probe().joint[j] - expected[j];
          error2 += d * d;
        }
        ++samples;
        if (state.terminal) { ++falls; break; }
        if (state.timeout) break;
      }
    }
    json::Value v = json::Value::object();
    v.set("episodes", n);
    v.set("falls", falls);
    v.set("meanErrorDeg", samples > 0 ? std::sqrt(error2 / (samples * kActions)) * 57.2957795 : 0.0);
    return json::stringify(v);
  }
  if (recipe.task.compare(0, 9, "scenario:") == 0) {
    const i32 n = clamp(episodes, 1, 32);
    i32 success = 0, falls = 0;
    f64 total = 0;
    for (i32 k = 0; k < n; ++k) {
      std::unique_ptr<DuckEnv> env = make_env(recipe, 2000, k);
      if (!env) return {};
      env->task().randomize = false;
      env->reset();
      for (i32 step = 0; step < 3000; ++step) {
        f32 action[kActions] = {};
        learner.act(env->observation(), action);
        const Env::Step result = env->step(action);
        if (result.terminal || result.timeout) break;
      }
      total += env->episode_return();
      if (env->task().succeeded()) ++success;
      else ++falls;
    }
    json::Value v = json::Value::object();
    v.set("episodes", n);
    v.set("successes", success);
    v.set("failures", falls);
    v.set("meanReturn", total / n);
    return json::stringify(v);
  }
  if (recipe.task != "jump") return {};
  const JumpReport r = evaluate_jump([](void* u, const f32* o, f32* a) { static_cast<Ppo*>(u)->act(o, a); }, &learner, max_of(1, episodes), 1000,
                                     recipe.substeps);
  json::Value v = json::Value::object();
  v.set("jumps", r.jumps);
  v.set("episodes", r.episodes);
  v.set("heightMm", static_cast<f64>(r.height_mm));
  v.set("riseMm", static_cast<f64>(r.rise_mm));
  v.set("tiltDeg", static_cast<f64>(r.tilt_deg));
  v.set("flightS", static_cast<f64>(r.flight_s));
  v.set("falls", r.falls);
  return json::stringify(v);
}


}  // namespace kern::lehrer
