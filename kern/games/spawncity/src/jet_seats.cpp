// The seat table's questions to the Blueprint of the actor "JetSeats" (see jet_seats.h): no rule lives here.
#include "jet_seats.h"

#include "blueprint_call.h"

namespace sc::seats {

namespace {
rules::Blueprint g_rules{"JetSeats"};
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_rules.running = blueprint; }
kern::logic::Instance* blueprint() { return g_rules.running; }

std::string result_name(Result r) {
  std::string name;
  g_rules.call("ResultName", std::tie(name), r);
  return name;
}

Row Table::get(u8 owner) const {
  Row row;
  g_rules.call("Get", std::tie(row), rows_, owner);
  return row;
}

bool Table::seat_of(u8 player, Row& row, Seat& seat) const {
  bool found = false;
  Row r;
  Seat s = Seat::pilot;
  g_rules.call("SeatOf", std::tie(found, r, s), rows_, player);
  if (!found) return false;
  row = r;
  seat = s;
  return true;
}

Result Table::claim(u8 owner, u8 player, Seat seat) {
  std::vector<Row> rows;
  Result r = Result::ok;
  g_rules.call("Claim", std::tie(rows, r), rows_, owner, player, seat);
  if (r == Result::ok) {
    rows_ = rows;
    ++revision_;
  }
  return r;
}

Result Table::release(u8 owner, u8 player, Seat seat) {
  std::vector<Row> rows;
  Result r = Result::ok;
  g_rules.call("Release", std::tie(rows, r), rows_, owner, player, seat);
  if (r == Result::ok) {
    rows_ = rows;
    ++revision_;
  }
  return r;
}

bool Table::drop(u8 player) {
  std::vector<Row> rows;
  bool changed = false;
  g_rules.call("Drop", std::tie(rows, changed), rows_, player);
  if (!changed) return false;
  rows_ = rows;
  ++revision_;
  return true;
}

bool Table::load(u32 revision, const std::vector<Row>& rows) {
  if (!g_rules.flag("Valid", rows)) return false;
  rows_ = rows;
  revision_ = revision;
  return true;
}

}  // namespace sc::seats
