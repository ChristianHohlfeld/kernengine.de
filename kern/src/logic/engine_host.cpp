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
  place(handle, std::move(kinds), end);
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
      if (kind == k) return where;
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
  e.end(e.owner ? e.owner : e.at);
  if (!entries_.count(handle)) kept_.erase(handle);  // (the Callbacks it was given go with it)
  return true;
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
  kept_.clear();
  calling_.clear();
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
