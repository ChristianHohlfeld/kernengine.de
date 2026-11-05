// Jet seats (see jet_seats.h): jet_seat_table.mjs.
#include "jet_seats.h"

#include <algorithm>

namespace sc::seats {

const char* result_name(Result r) {
  static const char* n[] = {"ok", "unchanged", "invalid", "owner-only-pilot", "already-seated", "seat-occupied", "no-pilot", "not-occupant"};
  return n[static_cast<u8>(r)];
}

Row* Table::find(u8 owner) {
  for (Row& r : rows_)
    if (r.owner == owner) return &r;
  return nullptr;
}

Row Table::get(u8 owner) const {
  for (const Row& r : rows_)
    if (r.owner == owner) return r;
  return Row{owner, kNone, kNone};
}

bool Table::seat_of(u8 player, Row& row, Seat& seat) const {
  for (const Row& r : rows_) {
    if (r.pilot == player) { row = r; seat = Seat::pilot; return true; }
    if (r.passenger == player) { row = r; seat = Seat::passenger; return true; }
  }
  return false;
}

Result Table::claim(u8 owner, u8 player, Seat seat) {
  if (owner == kNone || player == kNone) return Result::invalid;
  if (seat == Seat::pilot && player != owner) return Result::owner_only_pilot;
  Row before;
  Seat bs;
  if (seat_of(player, before, bs) && !(before.owner == owner && bs == seat)) return Result::already_seated;
  Row cur = get(owner);
  u8& slot = seat == Seat::pilot ? cur.pilot : cur.passenger;
  if (slot == player) return Result::unchanged;
  if (slot != kNone) return Result::seat_occupied;
  if (seat == Seat::passenger && cur.pilot == kNone) return Result::no_pilot;
  slot = player;
  if (Row* r = find(owner)) *r = cur;
  else rows_.push_back(cur);
  ++revision_;
  return Result::ok;
}

Result Table::release(u8 owner, u8 player, Seat seat) {
  if (owner == kNone || player == kNone) return Result::invalid;
  Row* r = find(owner);
  if (!r || (seat == Seat::pilot ? r->pilot : r->passenger) != player) return Result::not_occupant;
  if (seat == Seat::pilot) { r->pilot = kNone; r->passenger = kNone; }  // failsafe: no unmanaged flight
  else r->passenger = kNone;
  if (r->pilot == kNone && r->passenger == kNone) rows_.erase(rows_.begin() + (r - rows_.data()));
  ++revision_;
  return Result::ok;
}

bool Table::drop(u8 player) {
  bool changed = false;
  for (std::size_t i = 0; i < rows_.size();) {
    if (rows_[i].pilot == player) { rows_.erase(rows_.begin() + static_cast<std::ptrdiff_t>(i)); changed = true; continue; }
    if (rows_[i].passenger == player) { rows_[i].passenger = kNone; changed = true; }
    ++i;
  }
  if (changed) ++revision_;
  return changed;
}

bool Table::load(u32 revision, const std::vector<Row>& rows) {
  if (rows.size() > 64) return false;
  std::vector<u8> seen;
  for (const Row& r : rows) {
    if (r.owner == kNone || r.pilot != r.owner || r.passenger == r.pilot) return false;
    for (const Row& q : rows)
      if (&q != &r && q.owner == r.owner) return false;
    for (u8 p : {r.pilot, r.passenger}) {
      if (p == kNone) continue;
      if (std::find(seen.begin(), seen.end(), p) != seen.end()) return false;
      seen.push_back(p);
    }
  }
  rows_ = rows;
  revision_ = revision;
  return true;
}

}  // namespace sc::seats
