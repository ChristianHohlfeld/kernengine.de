// One town in multiplayer: the bytes (see town_sync.h).
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#include "town_sync.h"

#include <algorithm>
#include <cmath>

#include "wire.h"

namespace sc::town {

namespace {
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

std::vector<u8> pack(const CarHit& h) {
  std::vector<u8> o;
  wire::put_u8(o, static_cast<u8>(Kind::car_hit));
  wire::put_u8(o, h.car);
  wire::put_f32(o, h.damage);
  wire::put_u8(o, static_cast<u8>(h.cause));
  return o;
}
bool unpack(const std::vector<u8>& blob, CarHit& h) {
  wire::Reader in;
  if (in.get_u8(blob) != static_cast<u8>(Kind::car_hit)) return false;
  h.car = in.get_u8(blob);
  h.damage = in.get_f32(blob);
  const u8 cause = in.get_u8(blob);
  if (!in.done(blob) || cause > static_cast<u8>(Cause::kLast) || !std::isfinite(h.damage) || !(h.damage > 0) || h.damage > 1e4f) return false;
  h.cause = static_cast<Cause>(cause);
  return true;
}

std::vector<u8> pack(const PersonHit& h) {
  std::vector<u8> o;
  wire::put_u8(o, static_cast<u8>(Kind::person_hit));
  wire::put_u8(o, h.person);
  wire::put_u32(o, h.seed);
  wire::put_f32(o, h.damage);
  wire::put_u8(o, h.by);
  wire::put_vec3(o, h.push);
  return o;
}
bool unpack(const std::vector<u8>& blob, PersonHit& h) {
  wire::Reader in;
  if (in.get_u8(blob) != static_cast<u8>(Kind::person_hit)) return false;
  h.person = in.get_u8(blob);
  h.seed = in.get_u32(blob);
  h.damage = in.get_f32(blob);
  h.by = in.get_u8(blob);
  h.push = in.get_vec3(blob);
  return in.done(blob) && h.by <= 2 && std::isfinite(h.damage) && h.damage > 0 && h.damage <= 1e4f && vec_finite(h.push) && kern::length(h.push) <= 40;
}

std::vector<std::vector<u8>> pack_lamps(const std::vector<LampRec>& lamps) {
  std::vector<std::vector<u8>> out;
  for (std::size_t from = 0; from < lamps.size(); from += 255) {
    const std::size_t n = std::min<std::size_t>(255, lamps.size() - from);
    std::vector<u8> o;
    wire::put_u8(o, static_cast<u8>(Kind::lamps));
    wire::put_u8(o, static_cast<u8>(n));
    for (std::size_t i = from; i < from + n; ++i) { wire::put_i64(o, lamps[i].key); wire::put_u8(o, lamps[i].state); }
    out.push_back(std::move(o));
  }
  return out;
}
bool unpack_lamps(const std::vector<u8>& blob, std::vector<LampRec>& lamps) {
  wire::Reader in;
  lamps.clear();
  if (in.get_u8(blob) != static_cast<u8>(Kind::lamps)) return false;
  const u8 n = in.get_u8(blob);
  for (u8 i = 0; i < n && in.ok; ++i) {
    LampRec r;
    r.key = in.get_i64(blob);
    r.state = in.get_u8(blob);
    if (!in.ok || !lamp_ok(r)) return false;
    lamps.push_back(r);
  }
  return in.done(blob);
}

std::vector<u8> pack(const PlankStep& s) {
  std::vector<u8> o;
  wire::put_u8(o, static_cast<u8>(Kind::plank_step));
  wire::put_i64(o, s.key);
  wire::put_u8(o, s.door);
  wire::put_u8(o, s.doors);
  wire::put_i8(o, s.step);
  return o;
}
bool unpack(const std::vector<u8>& blob, PlankStep& s) {
  wire::Reader in;
  if (in.get_u8(blob) != static_cast<u8>(Kind::plank_step)) return false;
  s.key = in.get_i64(blob);
  s.door = in.get_u8(blob);
  s.doors = in.get_u8(blob);
  s.step = in.get_i8(blob);
  return in.done(blob) && s.doors >= 1 && s.doors <= kDoorsMax && s.door < s.doors && (s.step == 1 || s.step == -1);
}

std::vector<u8> pack(const PlankRec& r) {
  std::vector<u8> o;
  wire::put_u8(o, static_cast<u8>(Kind::planks));
  wire::put_i64(o, r.key);
  wire::put_u8(o, r.door);
  wire::put_u8(o, r.doors);
  wire::put_u8(o, r.planks);
  return o;
}
bool unpack(const std::vector<u8>& blob, PlankRec& r) {
  wire::Reader in;
  if (in.get_u8(blob) != static_cast<u8>(Kind::planks)) return false;
  r.key = in.get_i64(blob);
  r.door = in.get_u8(blob);
  r.doors = in.get_u8(blob);
  r.planks = in.get_u8(blob);
  return in.done(blob) && plank_ok(r);
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
    std::vector<u8> o;
    wire::put_u8(o, static_cast<u8>(Kind::state));
    wire::put_u32(o, epoch);
    wire::put_u8(o, static_cast<u8>((out.empty() ? 1 : 0) | (last ? 2 : 0)));
    wire::put_u16(o, static_cast<u16>(nl));
    for (std::size_t i = li; i < li + nl; ++i) { wire::put_i64(o, lamps[i].key); wire::put_u8(o, lamps[i].state); }
    wire::put_u16(o, static_cast<u16>(np));
    for (std::size_t i = pi; i < pi + np; ++i) { wire::put_i64(o, planks[i].key); wire::put_u8(o, planks[i].door); wire::put_u8(o, planks[i].doors); wire::put_u8(o, planks[i].planks); }
    wire::put_u16(o, static_cast<u16>(nm));
    for (std::size_t i = mi; i < mi + nm; ++i) { wire::put_vec3(o, marks[i].at); wire::put_f32(o, marks[i].radius); wire::put_f32(o, marks[i].strength); }
    out.push_back(std::move(o));
    li += nl;
    pi += np;
    mi += nm;
  } while (li < lamps.size() || pi < planks.size() || mi < marks.size());
  return out;
}

bool unpack_state(const std::vector<u8>& blob, State& s) {
  wire::Reader in;
  s = State{};
  if (in.get_u8(blob) != static_cast<u8>(Kind::state)) return false;
  s.epoch = in.get_u32(blob);
  const u8 flags = in.get_u8(blob);
  s.first = flags & 1;
  s.last = flags & 2;
  const u16 nl = in.get_u16(blob);
  for (u16 i = 0; i < nl && in.ok; ++i) {
    LampRec r;
    r.key = in.get_i64(blob);
    r.state = in.get_u8(blob);
    if (!in.ok || !lamp_ok(r)) return false;
    s.lamps.push_back(r);
  }
  const u16 np = in.get_u16(blob);
  for (u16 i = 0; i < np && in.ok; ++i) {
    PlankRec r;
    r.key = in.get_i64(blob);
    r.door = in.get_u8(blob);
    r.doors = in.get_u8(blob);
    r.planks = in.get_u8(blob);
    if (!in.ok || !plank_ok(r)) return false;
    s.planks.push_back(r);
  }
  const u16 nm = in.get_u16(blob);
  for (u16 i = 0; i < nm && in.ok; ++i) {
    MarkRec m;
    m.at = in.get_vec3(blob);
    m.radius = in.get_f32(blob);
    m.strength = in.get_f32(blob);
    if (!in.ok || !mark_ok(m)) return false;
    s.marks.push_back(m);
  }
  return in.done(blob);
}

}  // namespace sc::town
