// Kern Logic: the engine's nodes on the engine itself - the objects a Blueprint holds handles of, and
// the calls of the components (kern/logic/engine_host.h).
#include "kern/logic/engine_host.h"

#include <algorithm>

namespace kern::logic {

i64 EngineObjects::put(Kinds kinds, void (*end)(void*)) {
  if (!end) {   // (the host's own: the same object of the same class has the same handle)
    const auto it = by_object_.find(kinds.front().second);
    if (it != by_object_.end())
      for (const auto& [kind, handle] : it->second)
        if (kind == kinds.front().first) return handle;
  }
  const i64 handle = next_++;
  void* const at = kinds.front().second;
  place(handle, std::move(kinds), end);
  if (!end && calling_on_ && calling_on_ != handle) {   // (the host's own, handed out by a call on an object this owns: lent)
    const auto on = entries_.find(calling_on_);
    if (on != entries_.end() && std::any_of(on->second.begin(), on->second.end(), [](const Entry& e) { return e.end != nullptr; })) lent_[calling_on_].push_back({handle, at});
  }
  return handle;
}

void EngineObjects::place(i64 handle, Kinds kinds, void (*end)(void*), void* owner) {
  ++generation_;
  Entry e;
  e.at = kinds.front().second;
  e.end = end;
  e.owner = owner;
  e.kinds = std::move(kinds);
  for (const auto& [kind, where] : e.kinds) by_object_[where].push_back({kind, handle});
  order_.push_back({handle, e.at});
  entries_[handle].push_back(std::move(e));
  if (handle >= next_) next_ = handle + 1;
}

void* EngineObjects::find(i64 handle, Kind k) const {
  const auto it = entries_.find(handle);
  if (it == entries_.end()) return nullptr;
  for (const Entry& e : it->second)
    for (const auto& [kind, where] : e.kinds)
      if (kind == k) {
        if (!calling_on_) calling_on_ = handle;
        return where;
      }
  return nullptr;
}

namespace {
// The entry of `at` at a handle, taken out (false: none there).
template <class Entries, class Entry> bool take(Entries& entries, i64 handle, const void* at, Entry& e) {
  const auto it = entries.find(handle);
  if (it == entries.end()) return false;
  auto& v = it->second;
  for (auto x = v.begin(); x != v.end(); ++x)
    if (x->at == at) {
      e = std::move(*x);
      v.erase(x);
      if (v.empty()) entries.erase(it);
      return true;
    }
  return false;
}
}  // namespace

bool EngineObjects::end(i64 handle, Kind k) {
  const auto it = entries_.find(handle);
  if (it == entries_.end()) return false;
  const auto x = std::find_if(it->second.begin(), it->second.end(), [&](const Entry& e) { return e.kinds.front().first == k; });
  if (x == it->second.end() || !x->end) return false;
  ++generation_;
  Entry e;
  take(entries_, handle, x->at, e);
  order_.erase(std::find(order_.begin(), order_.end(), std::pair<i64, void*>{handle, e.at}));
  for (const auto& [kind, where] : e.kinds) {
    auto o = by_object_.find(where);
    if (o == by_object_.end()) continue;
    auto& v = o->second;
    v.erase(std::remove_if(v.begin(), v.end(), [&](const auto& y) { return y.second == handle; }), v.end());
    if (v.empty()) by_object_.erase(o);
  }
  forget_lent(handle);   // (what it lent is no longer there)
  e.end(e.owner ? e.owner : e.at);
  if (!entries_.count(handle)) kept_.erase(handle);  // (the Callbacks it was given go with it)
  return true;
}

void EngineObjects::forget_lent(i64 lender) {
  const auto it = lent_.find(lender);
  if (it == lent_.end()) return;
  const std::vector<std::pair<i64, void*>> lent = std::move(it->second);
  lent_.erase(it);
  for (const auto& [handle, at] : lent) {
    const auto held = entries_.find(handle);
    if (held == entries_.end()) continue;
    if (std::none_of(held->second.begin(), held->second.end(), [&, at = at](const Entry& e) { return e.at == at && !e.end; })) continue;  // (gone already)
    Entry e;
    take(entries_, handle, at, e);
    const auto o = std::find(order_.begin(), order_.end(), std::pair<i64, void*>{handle, e.at});
    if (o != order_.end()) order_.erase(o);
    for (const auto& [kind, where] : e.kinds) {
      auto b = by_object_.find(where);
      if (b == by_object_.end()) continue;
      auto& v = b->second;
      v.erase(std::remove_if(v.begin(), v.end(), [&, handle = handle](const auto& y) { return y.second == handle; }), v.end());
      if (v.empty()) by_object_.erase(b);
    }
    forget_lent(handle);   // (what was reached through it)
    if (!entries_.count(handle)) kept_.erase(handle);
  }
}

void EngineObjects::clear() {
  ++generation_;
  // (what it owns ends in the reverse order it came - what came later may hold what came before)
  while (!order_.empty()) {
    const auto [handle, at] = order_.back();
    order_.pop_back();
    Entry e;
    if (!take(entries_, handle, at, e)) continue;
    if (e.end) e.end(e.owner ? e.owner : e.at);
  }
  entries_.clear();
  by_object_.clear();
  lent_.clear();
  kept_.clear();
  calling_.clear();
  calling_on_ = 0;
  next_ = 1;
}

std::size_t EngineObjects::size() const {
  std::size_t n = 0;
  for (const auto& [handle, v] : entries_) n += v.size();
  return n;
}

namespace {
const EngineCalls*& first_calls() {
  static const EngineCalls* first = nullptr;
  return first;
}
}  // namespace

EngineCalls::EngineCalls(const EngineCall* c, std::size_t n) : calls(c), count(n), next(first_calls()) { first_calls() = this; }

namespace {
const EngineMakers*& first_makers() {
  static const EngineMakers* first = nullptr;
  return first;
}
}  // namespace

EngineMakers::EngineMakers(const EngineMaker* m, std::size_t n) : makers(m), count(n), next(first_makers()) { first_makers() = this; }

void* engine_make_default(EngineObjects& objects, i64 handle, EngineObjects::Kind want) {
  for (const EngineMakers* p = first_makers(); p; p = p->next)
    for (std::size_t k = 0; k < p->count; ++k)
      if (p->makers[k].kind == want) return p->makers[k].make(objects, handle);
  return nullptr;
}

EngineCallFn engine_call(std::string_view function) {
  // (the parts of the components are all in before the first call: they are put in as the program starts)
  static const std::unordered_map<std::string_view, EngineCallFn> by_type = [] {
    std::unordered_map<std::string_view, EngineCallFn> m;
    for (const EngineCalls* p = first_calls(); p; p = p->next)
      for (std::size_t k = 0; k < p->count; ++k) m.emplace(p->calls[k].type, p->calls[k].call);
    return m;
  }();
  const auto it = by_type.find(function);
  return it == by_type.end() ? nullptr : it->second;
}

namespace {
const EngineImplements*& first_adapters() {
  static const EngineImplements* first = nullptr;
  return first;
}

struct InstanceImplementer final : Implementer {
  i32 id = 0;
  std::vector<std::string> functions;  // (what the graph has: its functions do not change while it runs)
  bool has(std::string_view function) const override { return std::find(functions.begin(), functions.end(), function) != functions.end(); }
  bool call(std::string_view function, std::span<const Value> args, std::vector<Value>& results) override {
    Instance* const inst = Runtime::instance_by_id(id);
    if (!inst) return false;
    // (each as the type of its input: what the engine hands over is a value of the signature)
    const Function* const fn = inst->graph().function(function);
    std::vector<Value> in;
    in.reserve(args.size());
    for (std::size_t k = 0; k < args.size(); ++k) in.push_back(fn && k < fn->inputs.size() && args[k].type != fn->inputs[k].type ? args[k].converted(fn->inputs[k].type) : args[k]);
    return inst->call(function, in, results);
  }
};
}  // namespace

EngineImplements::EngineImplements(const EngineImplement* a, std::size_t n) : adapters(a), count(n), next(first_adapters()) { first_adapters() = this; }

i64 engine_implement(EngineObjects& objects, std::string_view interface, std::shared_ptr<Implementer> by) {
  for (const EngineImplements* p = first_adapters(); p; p = p->next)
    for (std::size_t k = 0; k < p->count; ++k)
      if (interface == p->adapters[k].interface) return p->adapters[k].make(objects, std::move(by));
  return 0;
}

std::shared_ptr<Implementer> implementer_of(const Instance& blueprint) {
  auto i = std::make_shared<InstanceImplementer>();
  i->id = blueprint.id();
  for (const Function& f : blueprint.graph().functions) i->functions.push_back(f.name);
  return i;
}

i64 engine_implement(EngineObjects& objects, const Instance& blueprint) {
  if (blueprint.graph().implements.empty()) return 0;
  return engine_implement(objects, blueprint.graph().implements, implementer_of(blueprint));
}

bool call_engine(EngineObjects& objects, std::string_view function, const std::vector<Value>& in, std::vector<Value>& out) {
  const EngineCallFn f = engine_call(function);
  if (!f) {
    objects.error = "this build has no function " + std::string(function);
    return false;
  }
  objects.begin_call();
  const bool done = f(objects, in, out);
  if (!in.empty() && in[0].type == PinType::integer) objects.kept_by(in[0].as_int(), function);  // (what a method of an object was handed stays with it)
  return done;
}

}  // namespace kern::logic
