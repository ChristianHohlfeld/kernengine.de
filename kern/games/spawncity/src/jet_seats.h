// Who sits in which jet (jet_seat_table.mjs): the table the multiplayer authority keeps
// (jet_seat_coordinator.mjs). Every player has one jet (owner = the player's slot); only the owner
// flies it (pilot), one other player may ride along (passenger) while a pilot is in. A player sits in
// at most one seat; the pilot leaving takes the passenger out too (no unmanaged flight); a player who
// leaves the session loses his seats.
//
// The rules themselves are a Blueprint: the level's actor "JetSeats" (level/spawncity.klevel, open
// it in Kern Studio) - its functions Claim, Release, Drop, Valid, Get and SeatOf take a table and
// give the table as it is afterwards. A Table here only holds the rows and asks the Blueprint:
// nothing is decided here.
#pragma once

#include <string>
#include <vector>

#include "common.h"
#include "kern/logic/logic.h"

namespace sc::seats {

/// The running Blueprint of the actor "JetSeats" (the game binds it once its world has started;
/// tests bind the one they run). Unbound, no seat is given and nothing changes.
void bind(kern::logic::Instance* blueprint);
kern::logic::Instance* blueprint();

constexpr u8 kNone = 253;  // no player (the host is slot 255, net::kNobody 254)
enum class Seat : u8 { pilot = 0, passenger = 1 };
enum class Result : u8 { ok, unchanged, invalid, owner_only_pilot, already_seated, seat_occupied, no_pilot, not_occupant };
/// The Blueprint's enums Seat and Result have the same values in the same order; kNone is its variable None.
std::string result_name(Result r);

struct Row { u8 owner = kNone, pilot = kNone, passenger = kNone; };

class Table {
 public:
  Result claim(u8 owner, u8 player, Seat seat);
  Result release(u8 owner, u8 player, Seat seat);
  /// A player left: his jet's row goes, his passenger seat elsewhere is freed. True: changed.
  bool drop(u8 player);
  /// The row of `owner`'s jet (all kNone when nobody sits in it).
  Row get(u8 owner) const;
  /// Where `player` sits: the row (owner) and the seat; false when he sits nowhere.
  bool seat_of(u8 player, Row& row, Seat& seat) const;
  const std::vector<Row>& rows() const { return rows_; }
  u32 revision() const { return revision_; }
  /// Replace with the authority's snapshot (validated like JetSeatTable.load).
  bool load(u32 revision, const std::vector<Row>& rows);
  void clear() { rows_.clear(); ++revision_; }

 private:
  std::vector<Row> rows_;
  u32 revision_ = 0;
};

}  // namespace sc::seats
