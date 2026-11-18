// How the native Blueprint test writes a value down: the interpreter's side (kern_tests
// --emit-native) and the compiled side (blueprint_native_test) use this one function, and the
// two texts must be the same, character by character.
#pragma once

#include <cmath>
#include <cstdio>
#include <set>
#include <string>
#include <vector>

#include <sys/wait.h>
#include <unistd.h>

#include "kern/core/jobs.h"
#include "kern/logic/engine_api.h"
#include "kern/logic/engine_host.h"
#include "kern/logic/logic.h"

inline std::string native_show(const kern::logic::Value& v) {
  using kern::logic::PinType;
  char b[160];
  auto number = [&](double x, int digits) -> std::string {
    if (std::isnan(x)) return "nan";  // (whatever its sign)
    std::snprintf(b, sizeof b, "%.*g", digits, x);
    return b;
  };
  switch (v.type) {
    case PinType::boolean: return v.b ? "true" : "false";
    case PinType::integer: std::snprintf(b, sizeof b, "%lld", static_cast<long long>(v.i)); return b;
    case PinType::number: return number(v.n, 17);
    case PinType::vector: return "(" + number(v.v.x, 9) + ", " + number(v.v.y, 9) + ", " + number(v.v.z, 9) + ")";
    case PinType::text: return "\"" + v.s + "\"";
    case PinType::actor: std::snprintf(b, sizeof b, "#%llu", static_cast<unsigned long long>(v.a)); return b;
    case PinType::enumeration: std::snprintf(b, sizeof b, "~%lld", static_cast<long long>(v.i)); return b;  // (its place among its enum's values)
    case PinType::structure: {  // its fields, each as above
      std::string r = "{";
      for (std::size_t k = 0, n = v.fields(); k < n; ++k) r += (k ? ", " : "") + native_show(*v.field(k));
      return r + "}";
    }
    default: break;
  }
  if (!kern::logic::is_list(v.type)) return "?";
  std::string r = "[";  // a list: its items, each as above
  for (std::size_t k = 0, n = v.size(); k < n; ++k) r += (k ? ", " : "") + native_show(v.item(k));
  return r + "]";
}

/// The host of the native Blueprint test: time stands still, errors are kept; its random numbers
/// are one fixed sequence (a Blueprint draws its seed from it). Its engine is the stand-in: every
/// function answers by its outputs' forms, the same in every tier.
struct NativeTestHost : kern::logic::Host {
  std::vector<std::string> errors;
  kern::logic::EngineStandIn stand_in;
  bool engine(std::string_view function, const std::vector<kern::logic::Value>& in, std::vector<kern::logic::Value>& out) override { return stand_in.engine(function, in, out); }
  kern::f64 time() const override { return 0; }
  void print(const std::string&, bool) override {}
  void error(const std::string& text) override { errors.push_back(text); }
  kern::u32 state = 2463534242u;
  kern::u32 random() override {
    state = state * 1664525u + 1013904223u;
    return state;
  }
};

/// The host of a case played on the engine itself: its engine is the engine - the real function of every
/// node (kern/logic/engine_host.h), on objects of its own. `direct`: it gives a Blueprint's class these
/// objects, and the class calls the engine's functions on them itself (kern/logic/engine_direct.h); else
/// every call comes through its door. It counts the calls that came through the door and keeps what it
/// was told of calls that failed, in their order.
struct NativeEngineHost : NativeTestHost {
  kern::logic::EngineObjects objects;
  bool direct = false;
  long door = 0;
  std::string told;
  bool engine(std::string_view function, const std::vector<kern::logic::Value>& in, std::vector<kern::logic::Value>& out) override {
    ++door;
    if (kern::logic::call_engine(objects, function, in, out)) return true;
    engine_failed(function);
    return false;
  }
  kern::logic::EngineObjects* engine_objects() override { return direct ? &objects : nullptr; }
  void engine_failed(std::string_view function) override { told += " !" + std::string(function) + ": " + objects.error; }
};

/// The functions `runs` of a Blueprint (no inputs), called on the engine itself: after each, a row - the run
/// with what the host was told of calls that failed, then every variable of the Blueprint (`name=value`).
/// `as_class`: the Blueprint runs as its compiled class, which calls the engine's functions itself - else in
/// the interpreter, through the host's door. `problem`: it did not run that way.
inline std::vector<std::vector<std::string>> native_rows_on_engine(const kern::logic::Graph& graph, const std::string& name, const std::vector<std::string>& runs, bool as_class, std::string& problem) {
  NativeEngineHost host;
  host.direct = as_class;
  kern::logic::Runtime rt(host);
  rt.jit = false;
  rt.native = as_class;
  kern::logic::Instance* inst = rt.add(graph, 0, name);
  rt.begin();
  if ((std::string(inst->tier()) == "native C++") != as_class) problem = name + " runs in the " + inst->tier();
  std::vector<std::vector<std::string>> rows;
  for (const std::string& run : runs) {
    std::vector<kern::logic::Value> results;
    host.told.clear();
    std::vector<std::string>& row = rows.emplace_back();
    row.push_back(run + (inst->call(run, {}, results) ? " ->" : " (no call) ->") + host.told);
    for (const kern::logic::Variable& v : graph.variables) row.push_back(v.name + "=" + native_show(inst->get(v.name)));
  }
  if (problem.empty() && as_class && host.door != 0) problem = "the class of " + name + " called the engine through the host's door " + std::to_string(host.door) + " times";
  if (problem.empty() && !as_class && host.door == 0) problem = "the interpreter did not call the engine through the host's door";
  return rows;
}

/// The same as one text: a line per run. The interpreter's and the class's are the same, character by character.
inline std::string native_play_on_engine(const kern::logic::Graph& graph, const std::string& name, const std::vector<std::string>& runs, bool as_class, std::string& problem) {
  std::string text = "== " + name + " on the engine itself\n";
  for (const std::vector<std::string>& row : native_rows_on_engine(graph, name, runs, as_class, problem)) {
    text += row[0] + " |";
    for (std::size_t k = 1; k < row.size(); ++k) text += " " + row[k];
    text += "\n";
  }
  return text;
}

/// The rows of native_rows_on_engine, played in a process of its own that starts as this one stands (fork): so
/// every play begins from the same state of the engine - what it keeps between calls (the number of the next
/// job, its jobs, its last error ...) is what it was before the first play, whatever the plays before did.
inline std::vector<std::vector<std::string>> native_rows_on_engine_apart(const kern::logic::Graph& graph, const std::string& name, const std::vector<std::string>& runs, bool as_class, std::string& problem) {
  std::fflush(nullptr);
  int pipe_ends[2];
  if (pipe(pipe_ends) != 0) { problem = "no pipe for a play of " + name; return {}; }
  const pid_t child = fork();
  if (child < 0) { problem = "no process for a play of " + name; return {}; }
  if (child == 0) {
    close(pipe_ends[0]);
    std::string trouble;
    const std::vector<std::vector<std::string>> rows = native_rows_on_engine(graph, name, runs, as_class, trouble);
    std::string text;
    auto put = [&](const std::string& t) { text += std::to_string(t.size()) + ":" + t; };
    put(trouble);
    text += std::to_string(rows.size()) + ":";
    for (const std::vector<std::string>& row : rows) {
      text += std::to_string(row.size()) + ":";
      for (const std::string& cell : row) put(cell);
    }
    for (std::size_t at = 0; at < text.size();) {
      const ssize_t n = write(pipe_ends[1], text.data() + at, text.size() - at);
      if (n <= 0) _exit(3);
      at += static_cast<std::size_t>(n);
    }
    close(pipe_ends[1]);
    std::fflush(nullptr);
    _exit(0);
  }
  close(pipe_ends[1]);
  std::string text;
  char buffer[65536];
  for (ssize_t n; (n = read(pipe_ends[0], buffer, sizeof buffer)) != 0;) {
    if (n < 0) break;
    text.append(buffer, static_cast<std::size_t>(n));
  }
  close(pipe_ends[0]);
  int status = 0;
  waitpid(child, &status, 0);
  if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
    problem = "the play of " + name + " ended " + (WIFEXITED(status) ? "with " + std::to_string(WEXITSTATUS(status)) : "by signal " + std::to_string(WTERMSIG(status)));
    return {};
  }
  std::size_t at = 0;
  bool torn = false;
  auto number = [&]() -> std::size_t {
    const std::size_t colon = text.find(':', at);
    if (colon == std::string::npos) { torn = true; return 0; }
    const std::size_t n = std::stoul(text.substr(at, colon - at));
    at = colon + 1;
    return n;
  };
  auto get = [&]() -> std::string {
    const std::size_t n = number();
    if (torn || at + n > text.size()) { torn = true; return {}; }
    at += n;
    return text.substr(at - n, n);
  };
  const std::string trouble = get();
  if (problem.empty()) problem = trouble;
  std::vector<std::vector<std::string>> rows(number());
  for (std::vector<std::string>& row : rows) {
    row.resize(torn ? 0 : number());
    for (std::string& cell : row) cell = get();
  }
  if (torn || at != text.size()) { problem = "the play of " + name + " did not say all it gave"; return {}; }
  return rows;
}

/// The variables of a Blueprint that hold what a clock gave (EngineFunction::clock - now_ms, wall_ms): set by a
/// node one of whose inputs comes, wire by wire, from a node of such a function.
inline std::set<std::string> native_clocked_variables(const kern::logic::Graph& graph) {
  std::set<kern::u32> clocked;
  for (const kern::logic::Node& n : graph.nodes)
    if (const kern::logic::EngineFunction* f = kern::logic::engine_function(n.type); f && f->clock) clocked.insert(n.id);
  for (bool grew = true; grew;) {
    grew = false;
    for (const kern::logic::Link& l : graph.links)
      if (l.in != "exec" && clocked.count(l.from) && clocked.insert(l.to).second) grew = true;
  }
  std::set<std::string> variables;
  for (const kern::logic::Node& n : graph.nodes)
    if (clocked.count(n.id) && n.type != "var.get" && !n.param("variable").empty() && n.param("scope").empty()) variables.insert(n.param("variable"));
  return variables;
}

/// The functions `runs` played on the engine itself three times, each in a process of its own that starts from the
/// same state of the engine (native_rows_on_engine_apart): in the interpreter through the host's door, as the
/// compiled class that calls the engine's functions itself, and through the door again. The engine's jobs run on
/// this thread, in pump (kern::jobs::set_threads): when one runs is fixed by the calls alone. So every play gives
/// the same, character by character - what the host was told after a run, each variable after it; each difference
/// is a line of what this returns, a difference between the two plays through the door as well. Only what a clock
/// gave is not the same twice: the variables a node of a function that reads a clock sets (EngineFunction::clock)
/// are not compared and are named in `clocks`.
inline std::string native_compare_on_engine(const kern::logic::Graph& graph, const std::string& name, const std::vector<std::string>& runs, std::string& clocks, std::string& problem) {
  kern::jobs::set_threads(false);
  const auto door = native_rows_on_engine_apart(graph, name, runs, false, problem);
  const auto itself = native_rows_on_engine_apart(graph, name, runs, true, problem);
  const auto again = native_rows_on_engine_apart(graph, name, runs, false, problem);
  if (!problem.empty()) return {};
  const std::set<std::string> clocked = native_clocked_variables(graph);
  for (const std::string& v : clocked) clocks += " " + v;
  std::string differs;
  if (door.size() != runs.size() || door.size() != itself.size() || door.size() != again.size()) return "the three plays have not the same runs\n";
  for (std::size_t r = 0; r < door.size(); ++r) {
    if (door[r].size() != itself[r].size() || door[r].size() != again[r].size() || door[r].size() != graph.variables.size() + 1) return "the three plays have not the same variables\n";
    for (std::size_t k = 0; k < door[r].size(); ++k) {
      if (k > 0 && clocked.count(graph.variables[k - 1].name)) continue;
      if (door[r][k] != again[r][k]) differs += "after " + runs[r] + ", through the door: " + door[r][k] + "\n     through the door again: " + again[r][k] + "\n";
      if (itself[r][k] != door[r][k]) differs += "after " + runs[r] + ": " + itself[r][k] + "\n     through the door: " + door[r][k] + "\n";
    }
  }
  return differs;
}
