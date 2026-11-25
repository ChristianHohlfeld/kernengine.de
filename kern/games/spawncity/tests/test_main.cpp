// Native unit tests of SpawnCity's simulation models (no GL, deterministic):
// flight controller + drone plant, the jet's aerodynamics and fly-by-wire, the car's
// suspension and tyres, the map decoder and the game rules.
//
//   kern/games/spawncity/tests/run_tests.sh
//   spawncity_tests [part]          the tests whose name has `part` in it (all of them without it)
//   spawncity_tests --name <n>...   exactly the tests named so, each once (a whole name; one that is no test's: FAIL)
//   SC_TEST_SHARD=<k>/<n>           the k-th of every n of the tests (n jobs: each test once)
//   SC_TEST_JOBS=<n>                up to n tests at once (else KERN_JOBS, else the cores; the output as one by one, in the order of the tests)
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include "blueprint_native.h"
#include "kern/app/app.h"
#include "cost.h"
#include "rule_blueprint.h"
#include "rules.h"
#include "test_util.h"

namespace sct {
std::vector<Case>& cases() { static std::vector<Case> c; return c; }
int failures = 0, checks = 0;
}  // namespace sct

// The allocator says every piece of memory it is asked for (cost.h): with the sanitizers, their
// allocator through its hook; without them, operator new itself.
#if defined(SC_ASAN)
// (the sanitizers' own interface - sanitizer/allocator_interface.h, where a compiler brings that header)
extern "C" int __sanitizer_install_malloc_and_free_hooks(void (*asked)(const volatile void*, std::size_t), void (*given_back)(const volatile void*));
// (and what it gives is zeroed: what C++ leaves open in an object that nothing sets is then the zero the
// Blueprint starts from, one of the values C++ allows - converted.h, zero_stack)
static void asked_for_memory(const volatile void* p, std::size_t size) {
  sc::cost::asked();
  if (p != nullptr) std::memset(const_cast<void*>(p), 0, size);
}
static void gave_memory_back(const volatile void*) {}
static void count_memory() { __sanitizer_install_malloc_and_free_hooks(asked_for_memory, gave_memory_back); }
// What the C runtime asks for itself when a function's static registers how it ends (__cxa_atexit keeps its list
// in blocks and asks for a new one whenever the last is full - which call meets that depends on how many statics
// the whole process has registered before, not on the function) is not memory the function asks for: operator new
// does not see it either. So the registration runs with the count off.
#include <dlfcn.h>
extern "C" int __cxa_atexit(void (*end)(void*), void* object, void* dso) {
  using Register = int (*)(void (*)(void*), void*, void*);
  static const Register next = reinterpret_cast<Register>(dlsym(RTLD_NEXT, "__cxa_atexit"));
  const bool counted = sc::cost::counting;
  sc::cost::counting = false;
  const int r = next(end, object, dso);
  sc::cost::counting = counted;
  return r;
}
#else
#include <new>
void* operator new(std::size_t size) {
  sc::cost::asked();
  if (void* p = std::calloc(1, size ? size : 1)) return p;   // (zeroed: see the sanitizers' hook above)
  std::abort();
}
void* operator new[](std::size_t size) { return operator new(size); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
static void count_memory() {}
#endif

// The calls of the native test of the rules' Blueprint: every function with many values, then a
// chase - crimes, out of sight, the escape.
static blueprint_native::Case wanted_case() {
  using kern::logic::PinType;
  using kern::logic::Value;
  blueprint_native::Case c;
  c.name = "Wanted";
  std::string error;
  if (!sc::rules::actor_graph("Wanted", c.graph, error)) std::printf("the level has no Wanted Blueprint: %s\n", error.c_str());
  const double numbers[] = {0, 3.9, 4, 8.5, 9, 15, 22, 30, 35, 60, -2, 0.016, 6, 38, 1e6};
  for (const kern::logic::Function& f : c.graph.functions) {
    if (!f.pure) continue;
    for (int r = 0; r < 30; ++r) {
      blueprint_native::Call call{f.name, {}};
      for (std::size_t k = 0; k < f.inputs.size(); ++k) {
        const int at = r * static_cast<int>(2 * k + 1) + static_cast<int>(k) * 4;
        if (f.inputs[k].type == PinType::integer) call.args.push_back(Value::integer(at % 9 - 1));
        else if (f.inputs[k].type == PinType::boolean) call.args.push_back(Value::boolean(at % 2 == 0));
        else call.args.push_back(Value::number(numbers[static_cast<std::size_t>(at) % (sizeof numbers / sizeof numbers[0])]));
      }
      c.calls.push_back(std::move(call));
      if (f.inputs.empty()) break;
    }
  }
  auto report = [&](int crime, bool witnessed) { c.calls.push_back({"Report", {Value::integer(crime), Value::boolean(witnessed)}}); };
  auto update = [&](double dt, bool seen) { c.calls.push_back({"Update", {Value::number(dt), Value::boolean(seen)}}); };
  auto stars = [&] { c.calls.push_back({"Stars", {}}); };
  for (int crime = -1; crime <= 10; ++crime) {
    report(crime, crime % 3 != 0);
    stars();
  }
  update(0.5, true);
  for (int i = 0; i < 12; ++i) {
    update(1.0, false);
    stars();
  }
  for (int i = 0; i < 11; ++i) report(6, true);
  stars();
  for (int i = 0; i < 5; ++i) update(2.0, i == 2);
  stars();
  c.calls.push_back({"Clear", {}});
  stars();
  update(1.0 / 60.0, false);
  return c;
}

// The same for the Blueprint of the actor "CombatDetail": every function with many values of its
// inputs' types - enums, structs and lists among them.
/// Every function of a rule actor's Blueprint with many values of its inputs' types - enums,
/// structs and lists among them.
static blueprint_native::Case rule_case(const char* actor) {
  using kern::logic::Value;
  blueprint_native::Case c;
  c.name = actor;
  std::string error;
  if (!sc::rules::actor_graph(actor, c.graph, error)) std::printf("the level has no %s Blueprint: %s\n", actor, error.c_str());
  for (const kern::logic::Function& f : c.graph.functions) {
    std::vector<std::vector<Value>> samples;
    for (const kern::logic::FunctionPin& p : f.inputs) samples.push_back(blueprint_native::samples(kern::logic::TypeRef{p.type, p.of}, c.graph));
    for (std::size_t r = 0; r < 40; ++r) {
      blueprint_native::Call call{f.name, {}};
      for (std::size_t k = 0; k < samples.size(); ++k)
        if (!samples[k].empty()) call.args.push_back(samples[k][(r * (2 * k + 1) + k * 3) % samples[k].size()]);
      c.calls.push_back(std::move(call));
      if (f.inputs.empty()) break;
    }
  }
  return c;
}

static blueprint_native::Case multiplayer_case() {
  using kern::logic::Value;
  blueprint_native::Case c = rule_case("Multiplayer");
  // a ballot from its start, and one that is none
  auto ints = [](std::initializer_list<kern::i64> v) { return Value::of(std::vector<kern::i64>(v)); };
  c.calls.push_back({"CreateBallot", {Value::integer(7), Value::integer(0), Value::boolean(true), Value::integer(2), ints({3, 1, 2, 2}), Value::number(0)}});
  c.calls.push_back({"CreateBallot", {Value::integer(7), Value::integer(0), Value::boolean(true), Value::integer(9), ints({1, 2}), Value::number(0)}});
  return c;
}

static blueprint_native::Case combat_detail_case() {
  using kern::logic::Value;
  blueprint_native::Case c = rule_case("CombatDetail");
  // the clock of a drone that turns critical and blows
  for (double hp : {100.0, 28.0, 20.0, 20.0})
    c.calls.push_back({"ClockUpdate", {Value::enumeration(3), Value::number(hp), Value::number(100), Value::number(1.5), Value::number(hp < 25 ? 3.05 : -1), Value::number(hp < 25 ? 28 : 100)}});
  return c;
}

// The Blueprint of the actor "Guided": its functions with many values, and a missile from the
// rail to its target - every step the class gives is the interpreter's.
static blueprint_native::Case guided_case() {
  using kern::logic::Value;
  blueprint_native::Case c = rule_case("Guided");
  auto missile = [](kern::Vec3 p, kern::Vec3 v, kern::Vec3 goal, double born, double cruise, bool motor) {
    return Value::structure({Value::vector(p), Value::vector(v), Value::vector(goal), Value::number(born), Value::number(cruise), Value::boolean(motor), Value::boolean(true), Value::number(0.2),
                             Value::boolean(false), Value::vector({goal.x, goal.y, 0}), Value::boolean(true)});
  };
  c.calls.push_back({"Launch", {Value::vector({0, 0, 20}), Value::vector({300, 0, 1}), Value::number(0), Value::boolean(false)}});
  for (double now : {0.1, 0.5, 3.0, 30.0})
    for (float x : {0.0f, 120.0f, 260.0f, 299.0f})
      c.calls.push_back({"Step", {missile({x, 1, 25}, {30, 0, -2}, {300, 0, 1}, 0, 29, now > 0.4), Value::number(now), Value::number(1.0 / 60), Value::number(x > 250 ? 26 : 0)}});
  for (float at : {0.0f, 0.4f})
    c.calls.push_back({"Track", {missile({0, 0, 20}, {30, 0, 0}, {100, 0, 1}, 0, 29, true), Value::vector({100 + at, at, 1}), Value::number(0.05)}});
  return c;
}

// The Blueprint of the actor "JetSeats": its functions with many values, and a table through a
// pilot, a passenger, a second jet and their leaving.
static blueprint_native::Case jet_seats_case() {
  using kern::logic::Value;
  blueprint_native::Case c = rule_case("JetSeats");
  auto row = [](int owner, int pilot, int passenger) { return Value::structure({Value::integer(owner), Value::integer(pilot), Value::integer(passenger)}); };
  auto rows = [](std::vector<Value> v) { return Value::of(std::move(v)); };
  const Value none = rows({}), one = rows({row(255, 255, 253)}), two = rows({row(255, 255, 0), row(1, 1, 253)});
  for (const Value& t : {none, one, two})
    for (int owner : {255, 1, 0, 253})
      for (int player : {255, 0, 1, 2, 253})
        for (int seat : {0, 1}) {
          c.calls.push_back({"Claim", {t, Value::integer(owner), Value::integer(player), Value::enumeration(seat)}});
          c.calls.push_back({"Release", {t, Value::integer(owner), Value::integer(player), Value::enumeration(seat)}});
        }
  for (const Value& t : {none, one, two})
    for (int player : {255, 0, 1, 7}) {
      c.calls.push_back({"Drop", {t, Value::integer(player)}});
      c.calls.push_back({"SeatOf", {t, Value::integer(player)}});
      c.calls.push_back({"Get", {t, Value::integer(player)}});
    }
  for (const Value& t : {none, one, two, rows({row(1, 2, 253)}), rows({row(1, 1, 1)}), rows({row(1, 1, 3), row(2, 2, 3)}), rows({row(1, 1, 253), row(1, 1, 253)})}) c.calls.push_back({"Valid", {t}});
  return c;
}

int main(int argc, char** argv) {
  // spawncity_tests --emit-native <dir>: the Wanted Blueprint as its C++ text, calls to it and
  // the interpreter's answers (run_tests.sh compiles them into wanted_native_test).
  if (argc > 2 && std::strcmp(argv[1], "--emit-native") == 0) {
    std::string error;
    if (!blueprint_native::emit(argv[2], {wanted_case(), combat_detail_case(), guided_case(), rule_case("SpawnGuard"), jet_seats_case(), rule_case("Lamps"), rule_case("Sirens"), rule_case("Controls"), rule_case("Settings"), rule_case("Jetpack"), rule_case("Traffic"), rule_case("Military"), rule_case("Zombies"), rule_case("Sandbox"), rule_case("Fun"), rule_case("Race"), multiplayer_case()}, &error)) {
      std::printf("FAIL --emit-native: %s\n", error.c_str());
      return 1;
    }
    return 0;
  }
  // (--name: whole names. A part of a name is not one test: "converted_game_details" is also part of
  // converted_game_details_cpp, and a run for the one ran both.)
  const bool by_name = argc > 1 && std::strcmp(argv[1], "--name") == 0;
  const char* only = argc > 1 && !by_name ? argv[1] : nullptr;
  std::vector<std::string> names;
  if (by_name) for (int k = 2; k < argc; ++k) names.emplace_back(argv[k]);
  for (const std::string& n : names) {
    bool known = false;
    for (const sct::Case& c : sct::cases()) known = known || n == c.name;
    if (!known) { std::printf("FAIL %s (no test has this name)\n", n.c_str()); ++sct::failures; }
  }
  std::vector<const sct::Case*> todo;
  for (const sct::Case& c : sct::cases()) {
    if (only && !std::strstr(c.name, only)) continue;
    if (by_name) {
      bool named = false;
      for (const std::string& n : names) named = named || n == c.name;
      if (!named) continue;
    }
    todo.push_back(&c);
  }
  // SC_TEST_SHARD=k/n: of these, the k-th of every n (k = 1 ... n), in their order - n jobs together run every test once
  // (the CI splits the suite so; in turn, so that the long ones of a file - the parts of Game stand side by side - spread).
  if (const char* shard = std::getenv("SC_TEST_SHARD")) {
    int k = 0, n = 0;
    if (std::sscanf(shard, "%d/%d", &k, &n) != 2 || n < 1 || k < 1 || k > n) {
      std::printf("FAIL SC_TEST_SHARD=%s (k/n with 1 <= k <= n)\n", shard);
      return 1;
    }
    std::vector<const sct::Case*> mine;
    for (std::size_t i = 0; i < todo.size(); ++i)
      if (static_cast<int>(i % static_cast<std::size_t>(n)) == k - 1) mine.push_back(todo[i]);
    todo = std::move(mine);
  }
  int ran = 0;
  count_memory();
  // Every test in a process of its own: one that crashes (a sanitizer's report, a signal) or leaks is that
  // test's FAIL, and the tests after it still run - a run shows every test's result, not the first crash's.
  // (Lines go out as they are written: what a crashing test printed before is in the log.)
  std::setvbuf(stdout, nullptr, _IOLBF, 0);
  // Up to SC_TEST_JOBS tests at once - else KERN_JOBS, else as many as the machine has cores. Each test's lines go to a
  // file of its own and come out as one piece, in the order of the tests, when it has ended - the output is the same as
  // one by one. One test (or SC_TEST_JOBS=1): every line as it is written (a test stopped from outside shows how far it came).
  const auto jobs_of = [](const char* name) { const char* t = std::getenv(name); return t ? std::atoi(t) : 0; };
  int jobs = jobs_of("SC_TEST_JOBS") > 0 ? jobs_of("SC_TEST_JOBS") : jobs_of("KERN_JOBS") > 0 ? jobs_of("KERN_JOBS") : static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
  if (jobs < 1 || todo.size() <= 1) jobs = 1;
  struct Run {
    const sct::Case* c = nullptr;
    pid_t child = -1;
    int counts_fd = -1;
    std::FILE* out = nullptr;   // (the test's lines, when tests run at once)
    std::chrono::steady_clock::time_point began;
    double seconds = 0, cpu = 0;   // (the wall clock and the CPU time of its process: shown, nothing depends on them)
    int status = 0, counts[2] = {0, 0};
    std::size_t got = 0;
    bool ended = false;
  };
  std::vector<Run> runs(todo.size());
  // A test in a process of its own; its counts come back through a pipe.
  const auto start = [&](Run& r) {
    std::fflush(stdout);
    std::fflush(stderr);
    r.began = std::chrono::steady_clock::now();
    int pipe_fd[2];
    if (pipe(pipe_fd) != 0) { r.ended = true; return; }
    if (jobs > 1) r.out = std::tmpfile();
    r.child = fork();
    if (r.child == 0) {
      close(pipe_fd[0]);
      if (r.out) {
        dup2(fileno(r.out), 1);
        dup2(fileno(r.out), 2);
      }
      // (the test's own counts: the child begins with what the tests before it added up - without this each test
      // gave the sum so far once more, the sum doubled from test to test, and one failure failed every test after it)
      sct::checks = 0;
      sct::failures = 0;
      r.c->fn();
      int counts[2] = {sct::checks, sct::failures};
      std::fflush(stdout);
      if (write(pipe_fd[1], counts, sizeof counts) != static_cast<ssize_t>(sizeof counts)) _exit(97);
      close(pipe_fd[1]);
      std::exit(0);   // (std::exit: a leak checker's report at the end of the test counts for it)
    }
    close(pipe_fd[1]);
    r.counts_fd = pipe_fd[0];
    if (r.child < 0) { close(r.counts_fd); r.counts_fd = -1; r.ended = true; }
  };
  // Its counts (written before it ends), and how it ended.
  double cpu_all = 0;
  const auto end = [&](Run& r, int status, const rusage& used) {
    r.cpu = static_cast<double>(used.ru_utime.tv_sec + used.ru_stime.tv_sec) + static_cast<double>(used.ru_utime.tv_usec + used.ru_stime.tv_usec) / 1e6;
    cpu_all += r.cpu;
    while (r.counts_fd >= 0 && r.got < sizeof r.counts) {
      const ssize_t n = read(r.counts_fd, reinterpret_cast<char*>(r.counts) + r.got, sizeof r.counts - r.got);
      if (n <= 0) break;
      r.got += static_cast<std::size_t>(n);
    }
    if (r.counts_fd >= 0) close(r.counts_fd);
    r.counts_fd = -1;
    r.status = status;
    r.seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - r.began).count();
    r.ended = true;
  };
  // What it printed (when tests run at once), the time it took and its result.
  const auto said = [&](Run& r) {
    if (r.out) {
      std::fflush(stdout);
      std::rewind(r.out);
      char buf[1 << 16];
      std::size_t n;
      while ((n = std::fread(buf, 1, sizeof buf, r.out)) > 0) std::fwrite(buf, 1, n, stdout);
      std::fclose(r.out);
      r.out = nullptr;
      std::fflush(stdout);
    }
    ++ran;
    const sct::Case& c = *r.c;
    sct::checks += r.counts[0];
    sct::failures += r.counts[1];
    const int status = r.status;
    if (r.child < 0 || r.got != sizeof r.counts || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      ++sct::failures;
      if (r.child < 0) std::printf("FAIL %s (no process)\n", c.name);
      else if (WIFSIGNALED(status)) std::printf("FAIL %s (crashed: signal %d; %.1f s, CPU %.1f s)\n", c.name, WTERMSIG(status), r.seconds, r.cpu);
      else std::printf("FAIL %s (ended with %d; %.1f s, CPU %.1f s)\n", c.name, WIFEXITED(status) ? WEXITSTATUS(status) : -1, r.seconds, r.cpu);
      return;
    }
    std::printf("%s %s (%.1f s, CPU %.1f s)\n", r.counts[1] == 0 ? "ok  " : "FAIL", c.name, r.seconds, r.cpu);
  };
  for (std::size_t k = 0; k < todo.size(); ++k) runs[k].c = todo[k];
  std::size_t next = 0, shown = 0, running = 0;
  while (shown < runs.size()) {
    while (next < runs.size() && running < static_cast<std::size_t>(jobs)) {
      start(runs[next]);
      if (!runs[next].ended) ++running;
      ++next;
    }
    if (running > 0) {
      int status = 0;
      rusage used{};
      const pid_t done = wait4(jobs > 1 ? -1 : runs[next - 1].child, &status, 0, &used);
      if (done < 0 && errno != EINTR) {   // (no child left to wait for: what still runs is lost - said as an end with 99)
        for (std::size_t k = shown; k < next; ++k)
          if (!runs[k].ended) end(runs[k], 99 << 8, rusage{});
        running = 0;
      }
      for (std::size_t k = shown; k < next; ++k)
        if (done > 0 && !runs[k].ended && runs[k].child == done) {
          end(runs[k], status, used);
          --running;
        }
    }
    while (shown < next && runs[shown].ended) said(runs[shown++]);
  }
  std::printf("%d tests, %d checks, %d failures (CPU of the tests %.1f s, %d at once)\n", ran, sct::checks, sct::failures, cpu_all, jobs);
  return sct::failures ? 1 : 0;
}
