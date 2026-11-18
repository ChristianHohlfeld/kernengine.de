struct jet_seats : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(seats_Row) { Int owner = 253; Int pilot = 253; Int passenger = 253; };
  KERN_ENUM(seats_Seat, pilot, passenger);
  KERN_ENUM(the_seats_Result_, ok, unchanged, invalid, owner_only_pilot, already_seated, seat_occupied, no_pilot, not_occupant);
  KERN_STRUCT(seats_Table) { List<seats_Row> rows_ = List<seats_Row>{}; Int revision_ = 0; };

  // ---- functions ----
  struct Table_revisionResult { Int Result = 0; };
  KERN_PURE Table_revisionResult Table_revision(const seats_Table& self) {
    return {self.revision_};
  }

  void Table_clear(seats_Table& self) {
    list_clear(self.rows_);
    self.revision_ = kern::to<Int>(kern::formula([](Float revision_) -> Float { return kern::math::wrap32(revision_ + 1.0); }, kern::to<Float>(self.revision_)));
  }
};
