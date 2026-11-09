// Training recipes (recipe.h).
#include "kern/lehrer/recipe.h"

#include "kern/game/walker.h"
#include "kern/game/walker_policy.h"

namespace kern::lehrer {

f32 Recipe::push_scale(i32 iteration) const { return iteration < 300 ? 0.0f : iteration < 500 ? 0.27f : 1.0f; }

std::unique_ptr<Task> make_task(const std::string& name) {
  if (name == "jump") return make_jump_task();
  if (name == "stand") return make_stand_task();
  if (name.compare(0, 9, "scenario:") == 0) return make_scenario_task(name.substr(9));
  if (name.compare(0, 7, "motion:") == 0) return make_motion_task(name.substr(7));
  return nullptr;
}

std::unique_ptr<Task> make_task(const Recipe& recipe) {
  if (recipe.task == "jump" && recipe.imitate && recipe.imitate_reward > 0) return make_jump_task(&recipe.demo, recipe.imitate_reward);
  return make_task(recipe.task);
}

Recipe recipe_for(const std::string& task) {
  Recipe r;
  r.task = task;
  r.substeps = game::kWalkerSubsteps;
  r.ppo.critic_hidden = {256, 128};
  if (task.compare(0, 9, "scenario:") == 0) {
    // Pollen-style: begin with standing balance; two goal-vector command inputs are fresh.
    r.warm_start = "velstand";
    r.ppo.init_std = 0.25f;
    r.ppo.learning_rate = 1e-4f;
    r.ppo.steps_per_env = 48;
    r.ppo.entropy_coef = 0.005f;
  }
  if (task.compare(0, 7, "motion:") == 0) {
    r.warm_start = "velstand";
    r.ppo.init_std = 0.12f;
    r.ppo.learning_rate = 1e-4f;
    r.ppo.entropy_coef = 0.001f;
    r.ppo.steps_per_env = 48;
  }
  if (task == "jump") {
    // From velstand's balance (warm start); imitation of the built-in demonstration (vorbild.h),
    // then practice: small action noise, a careful learning rate, the demonstration's reward.
    r.warm_start = "velstand";
    r.ppo.init_std = 0.3f;
    r.ppo.entropy_coef = 0.005f;
    r.ppo.steps_per_env = 48;
    r.ppo.learning_rate = 1e-4f;
    r.imitate = true;
    r.demo = builtin_jump_demo();
    r.imitation_rounds = 8;
    r.imitation_episodes = 96;
    r.practice_std = 0.1f;
    r.imitate_reward = 10;
  }
  return r;
}

std::unique_ptr<DuckEnv> make_env(const Recipe& recipe, u64 seed, i32 index) {
  auto env = std::make_unique<DuckEnv>(make_task(recipe), seed * 1000003ull + static_cast<u64>(index), recipe.substeps);
  env->servo = recipe.servo;
  return env;
}

void prepare_env(DuckEnv& env, const Recipe& recipe, i32 iteration) {
  env.task().progress(iteration);
  env.push_scale = recipe.push_scale(iteration);
}

std::unique_ptr<Ppo> make_learner(const Recipe& recipe, u64 seed) {
  DuckEnv probe(make_task(recipe), 1, recipe.substeps);
  PpoConfig cfg = recipe.ppo;
  cfg.seed = seed;
  auto ppo = std::make_unique<Ppo>(cfg, probe.obs_size(), probe.critic_size(), probe.action_size());
  if (recipe.warm_start == "velstand") {
    auto w = game::walker_policy("velstand.onnx");
    if (!w || !ppo->warm_start(w->dims, w->weights, w->biases, w->mean, w->div, {game::kJumpPhaseSlot, game::kJumpPhaseSlot + 1})) return nullptr;
  }
  // Behaviour cloning from the user's keyframes: PPO alone rarely discovers
  // a deliberate head/whole-body gesture through random exploration.
  if (recipe.task.compare(0, 7, "motion:") == 0) {
    std::vector<f32> obs, target;
    for (i32 n = 0; n < 4; ++n) {
      std::unique_ptr<DuckEnv> env = make_env(recipe, seed, 10000 + n);
      env->task().randomize = n == 3;
      env->reset();
      const i32 steps = static_cast<i32>(env->task().episode_seconds() / kControlDt);
      for (i32 t = 0; t < steps; ++t) {
        f32 label[kActions] = {};
        if (!env->task().reference_action(env->steps(), label)) break;
        obs.insert(obs.end(), env->observation(), env->observation() + kObs);
        target.insert(target.end(), label, label + kActions);
        const Env::Step next = env->step(label);
        if (next.terminal || next.timeout) env->reset();
      }
    }
    if (!obs.empty()) ppo->imitate(obs, target, 8, 128, 7e-4f, seed + 42);
  }
  return ppo;
}

void begin_practice(Ppo& learner, const Recipe& recipe) {
  learner.set_stddev(recipe.practice_std);
  learner.set_actor_frozen(learner.iteration() < recipe.critic_warmup);
}

void before_learning(Ppo& learner, const Recipe& recipe) {
  learner.set_actor_frozen(recipe.imitate && learner.iteration() < recipe.critic_warmup);
}

}  // namespace kern::lehrer
