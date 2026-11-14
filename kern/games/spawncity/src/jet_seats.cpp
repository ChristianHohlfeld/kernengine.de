// The seat table's questions to the Blueprint of the actor "JetSeats" (see jet_seats.h): no rule lives here.
#include "jet_seats.h"

#include "blueprint_call.h"

namespace sc::seats {

namespace {
using kern::logic::Value;
using rules::of_enum;
rules::Blueprint g_rules{"JetSeats"};

// The Blueprint's struct Row, field by field in its order - and a table of them.
Value to_value(const Row& r) { return Value::structure({Value::integer(r.owner), Value::integer(r.pilot), Value::integer(r.passenger)}); }
Value to_value(const std::vector<Row>& rows) {
  std::vector<Value> list;
  list.reserve(rows.size());
  for (const Row& r : rows) list.push_back(to_value(r));
  return Value::of(std::move(list));
}
Row to_row(const Value& v) {
  Row r;
  if (v.fields() < 3) return r;
  r.owner = static_cast<u8>(v.field(0)->i);
  r.pilot = static_cast<u8>(v.field(1)->i);
  r.passenger = static_cast<u8>(v.field(2)->i);
  return r;
}
std::vector<Row> to_rows(const Value& v) {
  std::vector<Row> rows;
  for (std::size_t k = 0; k < v.size(); ++k) rows.push_back(to_row(v.item(k)));
  return rows;
}
}  // namespace

void bind(kern::logic::Instance* blueprint) { g_rules.running = blueprint; }
kern::logic::Instance* blueprint() { return g_rules.running; }

std::string result_name(Result r) {
  std::vector<Value> out;
  return g_rules.call("ResultName", {of_enum(r)}, out) && !out.empty() ? out[0].as_text() : std::string();
}

Row Table::get(u8 owner) const {
  std::vector<Value> out;
  return g_rules.call("Get", {to_value(rows_), Value::integer(owner)}, out) && !out.empty() ? to_row(out[0]) : Row{owner, kNone, kNone};
}

bool Table::seat_of(u8 player, Row& row, Seat& seat) const {
  std::vector<Value> out;
  if (!g_rules.call("SeatOf", {to_value(rows_), Value::integer(player)}, out) || out.size() < 3 || !out[0].b) return false;
  row = to_row(out[1]);
  seat = static_cast<Seat>(out[2].i);
  return true;
}

Result Table::claim(u8 owner, u8 player, Seat seat) {
  std::vector<Value> out;
  if (!g_rules.call("Claim", {to_value(rows_), Value::integer(owner), Value::integer(player), of_enum(seat)}, out) || out.size() < 2) return Result::invalid;
  const Result r = static_cast<Result>(out[1].i);
  if (r == Result::ok) {
    rows_ = to_rows(out[0]);
    ++revision_;
  }
  return r;
}

Result Table::release(u8 owner, u8 player, Seat seat) {
  std::vector<Value> out;
  if (!g_rules.call("Release", {to_value(rows_), Value::integer(owner), Value::integer(player), of_enum(seat)}, out) || out.size() < 2) return Result::invalid;
  const Result r = static_cast<Result>(out[1].i);
  if (r == Result::ok) {
    rows_ = to_rows(out[0]);
    ++revision_;
  }
  return r;
}

bool Table::drop(u8 player) {
  std::vector<Value> out;
  if (!g_rules.call("Drop", {to_value(rows_), Value::integer(player)}, out) || out.size() < 2 || !out[1].b) return false;
  rows_ = to_rows(out[0]);
  ++revision_;
  return true;
}

bool Table::load(u32 revision, const std::vector<Row>& rows) {
  if (!g_rules.flag("Valid", {to_value(rows)})) return false;
  rows_ = rows;
  revision_ = revision;
  return true;
}

}  // namespace sc::seats
