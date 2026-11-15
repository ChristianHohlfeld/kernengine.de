// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include "kern/core/base.h"
#include "kern/core/jobs.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;


// engine.last_error: kern::last_error
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::last_error();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.clear_error: kern::clear_error
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::clear_error();
  return true;
}

// engine.now_ms: kern::now_ms
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::now_ms();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.jobs.submit: kern::jobs::submit
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >>>();
  ev::take<ev::List<ev::Int>>(o, in, 1, a1);
  decltype(auto) r = kern::jobs::submit(std::move(a0), std::move(a1));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.jobs.take: kern::jobs::take
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >&>>();
  ev::take<ev::List<ev::Int>>(o, in, 1, a1);
  decltype(auto) r = kern::jobs::take(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::List<ev::Int>>(o, out, 1, a1);
  return true;
}

// engine.jobs.cancel: kern::jobs::cancel
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  kern::jobs::cancel(std::move(a0));
  return true;
}

// engine.jobs.pump: kern::jobs::pump
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 0, a0);
  kern::jobs::pump(std::move(a0));
  return true;
}

// engine.jobs.pending: kern::jobs::pending
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::jobs::pending();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.jobs.set_external: kern::jobs::set_external
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 0, a0);
  kern::jobs::set_external(std::move(a0));
  return true;
}

// engine.jobs.external: kern::jobs::external
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::jobs::external();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.jobs.take_queued: kern::jobs::take_queued
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::jobs::take_queued();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.jobs.run: kern::jobs::run
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  std::vector<unsigned char> a1;
  ev::take<ev::List<ev::Int>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >&>>();
  ev::take<ev::List<ev::Int>>(o, in, 2, a2);
  kern::jobs::run(std::move(a0), kern::Span<unsigned char const>(a1.data(), a1.size()), a2);
  ev::give<ev::List<ev::Int>>(o, out, 0, a2);
  return true;
}

// engine.jobs.done: kern::jobs::done
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  std::vector<unsigned char> a1;
  ev::take<ev::List<ev::Int>>(o, in, 1, a1);
  kern::jobs::done(std::move(a0), kern::Span<unsigned char const>(a1.data(), a1.size()));
  return true;
}

const EngineCall kCalls[] = {
    {"engine.last_error", &c0},
    {"engine.clear_error", &c1},
    {"engine.now_ms", &c2},
    {"engine.jobs.submit", &c3},
    {"engine.jobs.take", &c4},
    {"engine.jobs.cancel", &c5},
    {"engine.jobs.pump", &c6},
    {"engine.jobs.pending", &c7},
    {"engine.jobs.set_external", &c8},
    {"engine.jobs.external", &c9},
    {"engine.jobs.take_queued", &c10},
    {"engine.jobs.run", &c11},
    {"engine.jobs.done", &c12},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

}  // namespace
}  // namespace kern::logic
