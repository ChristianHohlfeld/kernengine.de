// Backends (backend.h): where rollouts are collected and the update's parallel work runs.
#include "kern/lehrer/backend.h"
#include "kern/lehrer/checkpoint.h"

namespace kern::lehrer {

ThreadBackend::ThreadBackend(i32 workers, i32 envs_per_worker, u64 seed, const EnvFactory& make, EnvPrepare prepare)
    : prepare_(std::move(prepare)), pool_(thread_pool(workers)) {
  groups_.resize(static_cast<std::size_t>(max_of(1, workers)));
  for (std::size_t w = 0; w < groups_.size(); ++w) {
    Group& g = groups_[w];
    const i32 first = static_cast<i32>(w) * envs_per_worker;
    g.envs = make(first, envs_per_worker, seed);
    for (auto& e : g.envs) {
      e->reset();
      g.ptrs.push_back(e.get());
    }
    g.collector = Collector(seed, first);
  }
}

i32 ThreadBackend::environments() const {
  i32 n = 0;
  for (const Group& g : groups_) n += static_cast<i32>(g.ptrs.size());
  return n;
}

void ThreadBackend::collect(const Policy& policy, i32 steps, i32 iteration, std::vector<Rollout>& out) {
  out.resize(groups_.size());
  pool_(static_cast<i32>(groups_.size()), [&](i32 w) {
    Group& g = groups_[static_cast<std::size_t>(w)];
    if (prepare_)
      for (Env* e : g.ptrs) prepare_(*e, iteration);
    g.collector.collect(g.ptrs, steps, policy, out[static_cast<std::size_t>(w)]);  // its group, on this thread
  });
}

std::vector<u8> ThreadBackend::save() const {
  checkpoint::Writer w;w.scalar(u32{1});w.scalar(static_cast<u32>(groups_.size()));
  for(const Group& g:groups_){
    w.scalar(static_cast<u32>(g.envs.size()));w.vector(g.collector.save());
    for(const auto& e:g.envs){
      const auto* duck=dynamic_cast<const DuckEnv*>(e.get());
      if(!duck)return {};
      const auto state=duck->save_episode();if(state.empty())return {};
      w.vector(state);
    }
  }
  return std::move(w.bytes);
}
bool ThreadBackend::load(const u8* data,std::size_t size) {
  checkpoint::Reader r{data,size};
  u32 version=0,n=0;
  if(!r.scalar(version)||version!=1||!r.scalar(n)||n!=groups_.size())return false;
  std::vector<u8> bytes;
  for(Group& g:groups_){
    if(!r.scalar(n)||n!=g.envs.size()||!r.vector(bytes,size)||
       !g.collector.load(bytes.data(),bytes.size(),n))return false;
    for(auto& e:g.envs){
      auto* duck=dynamic_cast<DuckEnv*>(e.get());
      if(!duck||!r.vector(bytes,size)||!duck->load_episode(bytes.data(),bytes.size()))return false;
    }
  }
  return r.end();
}

IterationStats train_iteration(Ppo& ppo, Backend& backend) {
  std::vector<Rollout> rollouts;
  backend.collect(ppo.policy(), ppo.config().steps_per_env, ppo.iteration() + 1, rollouts);
  std::vector<const Rollout*> ptrs;
  for (const Rollout& r : rollouts) ptrs.push_back(&r);
  ppo.set_parallel(backend.update_parallel());
  return ppo.learn(ptrs);
}

}  // namespace kern::lehrer
