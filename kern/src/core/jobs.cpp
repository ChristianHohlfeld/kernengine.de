// Kern jobs (see kern/core/jobs.h).
#include "kern/core/jobs.h"

#include <algorithm>
#include <cstring>
#include <deque>
#include <unordered_map>

#if !defined(__wasm__)
#include <condition_variable>
#include <mutex>
#include <thread>
#endif

namespace kern::jobs {

namespace {

struct Job {
  u32 id = 0, kind = 0;
  std::vector<u8> input, output;
  bool done = false, cancelled = false;
};

struct State {
  Fn fns[64] = {};
  u32 next_id = 1;
  std::deque<u32> queue;                 // waiting (ids)
  std::unordered_map<u32, Job> jobs;     // waiting, running or done
  bool external = false;
  std::vector<u8> handout;
#if !defined(__wasm__)
  std::mutex mutex;
  std::condition_variable wake;
  std::vector<std::thread> threads;
  bool stopping = false;
  ~State() {
    { std::lock_guard<std::mutex> lock(mutex); stopping = true; }
    wake.notify_all();
    for (std::thread& t : threads) t.join();
  }
#endif
};

State& S() {
  static State s;
  return s;
}

Fn fn_of(u32 kind) { return kind < 64 ? S().fns[kind] : nullptr; }

void execute(Job& j) {
  if (Fn f = fn_of(j.kind)) f(Span<const u8>(j.input.data(), j.input.size()), j.output);
  j.input.clear();
  j.input.shrink_to_fit();
  j.done = true;
}

#if !defined(__wasm__)
void worker_loop() {
  State& s = S();
  for (;;) {
    u32 id = 0;
    Job* job = nullptr;
    {
      std::unique_lock<std::mutex> lock(s.mutex);
      s.wake.wait(lock, [&] { return s.stopping || !s.queue.empty(); });
      if (s.stopping) return;
      id = s.queue.front();
      s.queue.pop_front();
      auto it = s.jobs.find(id);
      if (it == s.jobs.end()) continue;
      job = &it->second;
    }
    // The input is only touched by this thread until done is set under the lock.
    Job local;
    {
      std::lock_guard<std::mutex> lock(s.mutex);
      local.kind = job->kind;
      local.input = std::move(job->input);
    }
    execute(local);
    {
      std::lock_guard<std::mutex> lock(s.mutex);
      auto it = s.jobs.find(id);
      if (it == s.jobs.end()) continue;
      if (it->second.cancelled) { s.jobs.erase(it); continue; }
      it->second.output = std::move(local.output);
      it->second.done = true;
    }
  }
}
void ensure_threads() {
  State& s = S();
  if (!s.threads.empty()) return;
  const u32 n = std::clamp<u32>(std::thread::hardware_concurrency() > 2 ? std::thread::hardware_concurrency() - 1 : 1, 1, 4);
  for (u32 i = 0; i < n; ++i) s.threads.emplace_back(worker_loop);
}
#endif

void put_u32(std::vector<u8>& b, u32 v) { const std::size_t n = b.size(); b.resize(n + 4); std::memcpy(b.data() + n, &v, 4); }

}  // namespace

bool define(u32 kind, Fn fn) {
  if (kind >= 64) return false;
  S().fns[kind] = fn;
  return true;
}

u32 submit(u32 kind, std::vector<u8> input) {
  State& s = S();
#if !defined(__wasm__)
  ensure_threads();
  std::lock_guard<std::mutex> lock(s.mutex);
#endif
  const u32 id = s.next_id++;
  Job& j = s.jobs[id];
  j.id = id;
  j.kind = kind;
  j.input = std::move(input);
  s.queue.push_back(id);
#if !defined(__wasm__)
  s.wake.notify_one();
#endif
  return id;
}

bool take(u32 id, std::vector<u8>& out) {
  State& s = S();
#if !defined(__wasm__)
  std::lock_guard<std::mutex> lock(s.mutex);
#endif
  auto it = s.jobs.find(id);
  if (it == s.jobs.end() || !it->second.done) return false;
  out = std::move(it->second.output);
  s.jobs.erase(it);
  return true;
}

void cancel(u32 id) {
  State& s = S();
#if !defined(__wasm__)
  std::lock_guard<std::mutex> lock(s.mutex);
#endif
  auto it = s.jobs.find(id);
  if (it == s.jobs.end()) return;
  if (it->second.done) { s.jobs.erase(it); return; }
  it->second.cancelled = true;
  s.queue.erase(std::remove(s.queue.begin(), s.queue.end(), id), s.queue.end());
  // a queued job (never started) goes now; a running one when its result arrives
  bool running = true;
  (void)running;
}

void pump(f64 budget_ms) {
#if defined(__wasm__)
  State& s = S();
  if (s.external) return;
  const f64 end = now_ms() + budget_ms;
  while (!s.queue.empty()) {
    const u32 id = s.queue.front();
    s.queue.pop_front();
    auto it = s.jobs.find(id);
    if (it == s.jobs.end()) continue;
    if (it->second.cancelled) { s.jobs.erase(it); continue; }
    execute(it->second);
    if (now_ms() > end) break;
  }
#else
  (void)budget_ms;  // threads run them
#endif
}

u32 pending() {
  State& s = S();
#if !defined(__wasm__)
  std::lock_guard<std::mutex> lock(s.mutex);
#endif
  u32 n = 0;
  for (const auto& [id, j] : s.jobs) n += j.done ? 0 : 1;
  return n;
}

void set_external(bool on) { S().external = on; }
bool external() { return S().external; }

const std::vector<u8>& take_queued() {
  State& s = S();
  s.handout.clear();
  put_u32(s.handout, 0);
  u32 count = 0;
  while (!s.queue.empty()) {
    const u32 id = s.queue.front();
    s.queue.pop_front();
    auto it = s.jobs.find(id);
    if (it == s.jobs.end()) continue;
    if (it->second.cancelled) { s.jobs.erase(it); continue; }
    Job& j = it->second;
    put_u32(s.handout, j.id);
    put_u32(s.handout, j.kind);
    put_u32(s.handout, static_cast<u32>(j.input.size()));
    s.handout.insert(s.handout.end(), j.input.begin(), j.input.end());
    j.input.clear();
    j.input.shrink_to_fit();
    ++count;
  }
  std::memcpy(s.handout.data(), &count, 4);
  return s.handout;
}

void run(u32 kind, Span<const u8> input, std::vector<u8>& output) {
  output.clear();
  if (Fn f = fn_of(kind)) f(input, output);
}

void done(u32 id, Span<const u8> output) {
  State& s = S();
  auto it = s.jobs.find(id);
  if (it == s.jobs.end()) return;
  if (it->second.cancelled) { s.jobs.erase(it); return; }
  it->second.output.assign(output.begin(), output.end());
  it->second.done = true;
}

}  // namespace kern::jobs

// ---- the host page's interface (kern/web/jobs.mjs) ----
#include "kern/api/c_api.h"

using kern::u8;
using kern::u32;
using kern::i32;

namespace {
std::vector<u8> g_job_result;
const std::vector<u8>* g_handout = nullptr;
}  // namespace

/// The page runs the jobs in Web Workers from now on (1) or not (0).
KERN_EXPORT(kern_jobs_external) void kern_jobs_external(i32 on) { kern::jobs::set_external(on != 0); }
/// Queued jobs as one buffer (see jobs.h take_queued); its length: kern_jobs_take_length.
KERN_EXPORT(kern_jobs_take) const u8* kern_jobs_take() { g_handout = &kern::jobs::take_queued(); return g_handout->data(); }
KERN_EXPORT(kern_jobs_take_length) u32 kern_jobs_take_length() { return g_handout ? static_cast<u32>(g_handout->size()) : 0; }
/// In the worker: runs one job; the output stays valid until the next call (kern_job_result_length).
KERN_EXPORT(kern_job_run) const u8* kern_job_run(u32 kind, const u8* input, u32 length) {
  kern::jobs::run(kind, kern::Span<const u8>(input, length), g_job_result);
  return g_job_result.data();
}
KERN_EXPORT(kern_job_result_length) u32 kern_job_result_length() { return static_cast<u32>(g_job_result.size()); }
/// The result of a job the page handed out.
KERN_EXPORT(kern_job_done) void kern_job_done(u32 id, const u8* output, u32 length) { kern::jobs::done(id, kern::Span<const u8>(output, length)); }
