// Native unit tests of SpawnCity's simulation models (no GL, deterministic):
// flight controller + drone plant, the map decoder, the city and the terrain, the controls and the settings.
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
#include "kern/test/blueprint.h"
#include "kern/test/test.h"

// (The registry of the tests is the engine's - kern/test/test.h -, and so is the allocator the comparison counts with:
// kern/tools/test_memory.cpp, linked by run_tests.sh.)

/// Every function of a rule actor's Blueprint with many values of its inputs' types - enums,
/// structs and lists among them.
static blueprint_native::Case rule_case(const char* actor) {
  using kern::logic::Value;
  blueprint_native::Case c;
  c.name = actor;
  std::string error;
  if (!kern::test::read_blueprint(actor, c.graph, error)) std::printf("the level has no %s Blueprint: %s\n", actor, error.c_str());
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

int main(int argc, char** argv) {
  // spawncity_tests --emit-native <dir>: the rules' Blueprints (Controls, Settings) as their C++ text, calls to it and
  // the interpreter's answers (run_tests.sh compiles them into wanted_native_test).
  if (argc > 2 && std::strcmp(argv[1], "--emit-native") == 0) {
    std::string error;
    if (!blueprint_native::emit(argv[2], {rule_case("Controls"), rule_case("Settings")}, &error)) {
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
    for (const kern::test::Case& c : kern::test::cases()) known = known || n == c.name;
    if (!known) { std::printf("FAIL %s (no test has this name)\n", n.c_str()); ++kern::test::failures; }
  }
  std::vector<const kern::test::Case*> todo;
  for (const kern::test::Case& c : kern::test::cases()) {
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
    std::vector<const kern::test::Case*> mine;
    for (std::size_t i = 0; i < todo.size(); ++i)
      if (static_cast<int>(i % static_cast<std::size_t>(n)) == k - 1) mine.push_back(todo[i]);
    todo = std::move(mine);
  }
  int ran = 0;
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
    const kern::test::Case* c = nullptr;
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
      kern::test::checks = 0;
      kern::test::failures = 0;
      r.c->fn();
      int counts[2] = {kern::test::checks, kern::test::failures};
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
    const kern::test::Case& c = *r.c;
    kern::test::checks += r.counts[0];
    kern::test::failures += r.counts[1];
    const int status = r.status;
    if (r.child < 0 || r.got != sizeof r.counts || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      ++kern::test::failures;
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
  std::printf("%d tests, %d checks, %d failures (CPU of the tests %.1f s, %d at once)\n", ran, kern::test::checks, kern::test::failures, cpu_all, jobs);
  return kern::test::failures ? 1 : 0;
}
