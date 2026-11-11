// One town in multiplayer: the bytes (see town_sync.h).
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include "town_sync.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace sc::town {

namespace {
struct Out {
  std::vector<u8> b;
  template <class T> Out& put(T v) { const auto* p = reinterpret_cast<const u8*>(&v); b.insert(b.end(), p, p + sizeof(T)); return *this; }
};
struct In {
  const std::vector<u8>& b;
  std::size_t at = 0;
  bool ok = true;
  template <class T> T get() {
    T v{};
    if (at + sizeof(T) > b.size()) { ok = false; return v; }
    std::memcpy(&v, b.data() + at, sizeof(T));
    at += sizeof(T);
    return v;
  }
  bool done() const { return ok && at == b.size(); }
};
bool vec_finite(Vec3 v) { return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z); }
bool lamp_ok(const LampRec& r) { return r.state == 1 || r.state == 2; }
bool plank_ok(const PlankRec& r) { return r.doors >= 1 && r.doors <= kDoorsMax && r.door < r.doors && r.planks <= kPlanksMax; }
bool mark_ok(const MarkRec& m) { return vec_finite(m.at) && std::fabs(m.at.x) < 1e6f && std::fabs(m.at.y) < 1e6f && m.radius > 0 && m.radius < 200 && m.strength >= 0 && m.strength <= 4; }
constexpr std::size_t kLampBytes = 9, kPlankBytes = 11, kMarkBytes = 20;
}  // namespace

Kind kind_of(const std::vector<u8>& blob) {
  if (blob.empty() || blob[0] < 1 || blob[0] > static_cast<u8>(Kind::ask)) return Kind::none;
  return static_cast<Kind>(blob[0]);
}

std::vector<u8> pack(const CarHit& h) { return Out().put(static_cast<u8>(Kind::car_hit)).put(h.car).put(h.damage).put(static_cast<u8>(h.cause)).b; }
bool unpack(const std::vector<u8>& blob, CarHit& h) {
  In in{blob};
  if (in.get<u8>() != static_cast<u8>(Kind::car_hit)) return false;
  h.car = in.get<u8>();
  h.damage = in.get<f32>();
  const u8 cause = in.get<u8>();
  if (!in.done() || cause > static_cast<u8>(Cause::kLast) || !std::isfinite(h.damage) || !(h.damage > 0) || h.damage > 1e4f) return false;
  h.cause = static_cast<Cause>(cause);
  return true;
}

std::vector<u8> pack(const PersonHit& h) { return Out().put(static_cast<u8>(Kind::person_hit)).put(h.person).put(h.seed).put(h.damage).put(h.by).put(h.push).b; }
bool unpack(const std::vector<u8>& blob, PersonHit& h) {
  In in{blob};
  if (in.get<u8>() != static_cast<u8>(Kind::person_hit)) return false;
  h.person = in.get<u8>();
  h.seed = in.get<u32>();
  h.damage = in.get<f32>();
  h.by = in.get<u8>();
  h.push = in.get<Vec3>();
  return in.done() && h.by <= 2 && std::isfinite(h.damage) && h.damage > 0 && h.damage <= 1e4f && vec_finite(h.push) && kern::length(h.push) <= 40;
}

std::vector<std::vector<u8>> pack_lamps(const std::vector<LampRec>& lamps) {
  std::vector<std::vector<u8>> out;
  for (std::size_t from = 0; from < lamps.size(); from += 255) {
    const std::size_t n = std::min<std::size_t>(255, lamps.size() - from);
    Out o;
    o.put(static_cast<u8>(Kind::lamps)).put(static_cast<u8>(n));
    for (std::size_t i = from; i < from + n; ++i) o.put(lamps[i].key).put(lamps[i].state);
    out.push_back(std::move(o.b));
  }
  return out;
}
bool unpack_lamps(const std::vector<u8>& blob, std::vector<LampRec>& lamps) {
  In in{blob};
  lamps.clear();
  if (in.get<u8>() != static_cast<u8>(Kind::lamps)) return false;
  const u8 n = in.get<u8>();
  for (u8 i = 0; i < n && in.ok; ++i) {
    LampRec r;
    r.key = in.get<i64>();
    r.state = in.get<u8>();
    if (!in.ok || !lamp_ok(r)) return false;
    lamps.push_back(r);
  }
  return in.done();
}

std::vector<u8> pack(const PlankStep& s) { return Out().put(static_cast<u8>(Kind::plank_step)).put(s.key).put(s.door).put(s.doors).put(s.step).b; }
bool unpack(const std::vector<u8>& blob, PlankStep& s) {
  In in{blob};
  if (in.get<u8>() != static_cast<u8>(Kind::plank_step)) return false;
  s.key = in.get<i64>();
  s.door = in.get<u8>();
  s.doors = in.get<u8>();
  s.step = in.get<kern::i8>();
  return in.done() && s.doors >= 1 && s.doors <= kDoorsMax && s.door < s.doors && (s.step == 1 || s.step == -1);
}

std::vector<u8> pack(const PlankRec& r) { return Out().put(static_cast<u8>(Kind::planks)).put(r.key).put(r.door).put(r.doors).put(r.planks).b; }
bool unpack(const std::vector<u8>& blob, PlankRec& r) {
  In in{blob};
  if (in.get<u8>() != static_cast<u8>(Kind::planks)) return false;
  r.key = in.get<i64>();
  r.door = in.get<u8>();
  r.doors = in.get<u8>();
  r.planks = in.get<u8>();
  return in.done() && plank_ok(r);
}

std::vector<u8> pack_ask() { return {static_cast<u8>(Kind::ask)}; }

std::vector<std::vector<u8>> pack_state(u32 epoch, const std::vector<LampRec>& lamps, const std::vector<PlankRec>& planks, const std::vector<MarkRec>& marks) {
  // a part: kind, epoch, flags (1 first, 2 last), then three counted lists (u16 each); the records
  // are dealt out in their order - lamps, planks, marks - a part taking what fits
  std::vector<std::vector<u8>> out;
  std::size_t li = 0, pi = 0, mi = 0;
  do {
    std::size_t room = kStateBytes - 12;
    const std::size_t nl = std::min(lamps.size() - li, room / kLampBytes);
    room -= nl * kLampBytes;
    const std::size_t np = li + nl == lamps.size() ? std::min(planks.size() - pi, room / kPlankBytes) : 0;
    room -= np * kPlankBytes;
    const std::size_t nm = li + nl == lamps.size() && pi + np == planks.size() ? std::min(marks.size() - mi, room / kMarkBytes) : 0;
    const bool last = li + nl == lamps.size() && pi + np == planks.size() && mi + nm == marks.size();
    Out o;
    o.put(static_cast<u8>(Kind::state)).put(epoch).put(static_cast<u8>((out.empty() ? 1 : 0) | (last ? 2 : 0)));
    o.put(static_cast<u16>(nl));
    for (std::size_t i = li; i < li + nl; ++i) o.put(lamps[i].key).put(lamps[i].state);
    o.put(static_cast<u16>(np));
    for (std::size_t i = pi; i < pi + np; ++i) o.put(planks[i].key).put(planks[i].door).put(planks[i].doors).put(planks[i].planks);
    o.put(static_cast<u16>(nm));
    for (std::size_t i = mi; i < mi + nm; ++i) o.put(marks[i].at).put(marks[i].radius).put(marks[i].strength);
    out.push_back(std::move(o.b));
    li += nl;
    pi += np;
    mi += nm;
  } while (li < lamps.size() || pi < planks.size() || mi < marks.size());
  return out;
}

bool unpack_state(const std::vector<u8>& blob, State& s) {
  In in{blob};
  s = State{};
  if (in.get<u8>() != static_cast<u8>(Kind::state)) return false;
  s.epoch = in.get<u32>();
  const u8 flags = in.get<u8>();
  s.first = flags & 1;
  s.last = flags & 2;
  const u16 nl = in.get<u16>();
  for (u16 i = 0; i < nl && in.ok; ++i) {
    LampRec r;
    r.key = in.get<i64>();
    r.state = in.get<u8>();
    if (!in.ok || !lamp_ok(r)) return false;
    s.lamps.push_back(r);
  }
  const u16 np = in.get<u16>();
  for (u16 i = 0; i < np && in.ok; ++i) {
    PlankRec r;
    r.key = in.get<i64>();
    r.door = in.get<u8>();
    r.doors = in.get<u8>();
    r.planks = in.get<u8>();
    if (!in.ok || !plank_ok(r)) return false;
    s.planks.push_back(r);
  }
  const u16 nm = in.get<u16>();
  for (u16 i = 0; i < nm && in.ok; ++i) {
    MarkRec m;
    m.at = in.get<Vec3>();
    m.radius = in.get<f32>();
    m.strength = in.get<f32>();
    if (!in.ok || !mark_ok(m)) return false;
    s.marks.push_back(m);
  }
  return in.done();
}

}  // namespace sc::town
