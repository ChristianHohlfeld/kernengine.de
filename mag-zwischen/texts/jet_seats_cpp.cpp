struct jet_seats_cpp : virtual kern::Library, virtual jet_seats {
  // ---- functions ----
  struct JetSeats_ResultNameResult { Text name = ""; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("ResultName") JetSeats_ResultNameResult JetSeats_ResultName(the_seats_Result_ result_2 = the_seats_Result_::ok);

  struct JetSeats_GetResult { seats_Row row = seats_Row{253, 253, 253}; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Get") JetSeats_GetResult JetSeats_Get(List<seats_Row> rows = List<seats_Row>{}, Int owner = 0);

  struct JetSeats_SeatOfResult { Bool found = false; seats_Row row = seats_Row{253, 253, 253}; seats_Seat seat = seats_Seat::pilot; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("SeatOf") JetSeats_SeatOfResult JetSeats_SeatOf(List<seats_Row> rows = List<seats_Row>{}, Int player = 0);

  struct JetSeats_ClaimResult { List<seats_Row> rows = List<seats_Row>{}; the_seats_Result_ result = the_seats_Result_::ok; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Claim") JetSeats_ClaimResult JetSeats_Claim(List<seats_Row> rows = List<seats_Row>{}, Int owner = 0, Int player = 0, seats_Seat seat = seats_Seat::pilot);

  struct JetSeats_ReleaseResult { List<seats_Row> rows = List<seats_Row>{}; the_seats_Result_ result = the_seats_Result_::ok; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Release") JetSeats_ReleaseResult JetSeats_Release(List<seats_Row> rows = List<seats_Row>{}, Int owner = 0, Int player = 0, seats_Seat seat = seats_Seat::pilot);

  struct JetSeats_DropResult { List<seats_Row> rows = List<seats_Row>{}; Bool changed = false; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Drop") JetSeats_DropResult JetSeats_Drop(List<seats_Row> rows = List<seats_Row>{}, Int player = 0);

  struct JetSeats_ValidResult { Bool ok = false; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Valid") JetSeats_ValidResult JetSeats_Valid(List<seats_Row> rows = List<seats_Row>{});

  struct bindResult { Bool Result = false; };
  KERN_PURE bindResult bind() {
    return {func_has_blueprint(actor_find("JetSeats"))};
  }

  struct result_nameResult { Text Result = ""; };
  result_nameResult result_name(the_seats_Result_ r = the_seats_Result_::ok) {
    Text name = "";
    name = "";
    func_call_on<"JetSeats_ResultName">(actor_find("JetSeats"), kern::out(n10.name), r);
    name = n10.name;
    return {name};
  }

  struct Table_getResult { seats_Row Result = seats_Row{253, 253, 253}; };
  Table_getResult Table_get(const seats_Table& self, U8 owner = 0) {
    seats_Row row = seats_Row{253, 253, 253};
    row = seats_Row{253, 253, 253};
    func_call_on<"JetSeats_Get">(actor_find("JetSeats"), kern::out(n11.row), self.rows_, owner);
    row = n11.row;
    return {row};
  }

  struct Table_seat_ofResult { Bool Result = false; };
  Table_seat_ofResult Table_seat_of(const seats_Table& self, seats_Row& row, seats_Seat& seat, U8 player = 0) {
    Bool found = false;
    seats_Row r = seats_Row{253, 253, 253};
    seats_Seat s = seats_Seat::pilot;
    found = false;
    r = seats_Row{253, 253, 253};
    s = seats_Seat::pilot;
    func_call_on<"JetSeats_SeatOf">(actor_find("JetSeats"), kern::out(n12.found, n12.row, n12.seat), self.rows_, player);
    found = n12.found;
    r = n12.row;
    s = n12.seat;
    if (logic_not(found)) {
      return {false};
    }
    row = r;
    seat = s;
    return {true};
  }

  struct Table_claimResult { the_seats_Result_ Result = the_seats_Result_::ok; };
  Table_claimResult Table_claim(seats_Table& self, U8 owner = 0, U8 player = 0, seats_Seat seat = seats_Seat::pilot) {
    List<seats_Row> rows = List<seats_Row>{};
    the_seats_Result_ r = the_seats_Result_::ok;
    rows = List<seats_Row>{};
    r = the_seats_Result_::ok;
    func_call_on<"JetSeats_Claim">(actor_find("JetSeats"), kern::out(n13.rows, n13.result), self.rows_, owner, player, seat);
    rows = n13.rows;
    r = n13.result;
    if (enum_equal<the_seats_Result_>(r, the_seats_Result_::ok)) {
      self.rows_ = rows;
      self.revision_ = kern::to<Int>(kern::formula([](Float revision_) -> Float { return (revision_ + 1.0); }, kern::to<Float>(self.revision_)));
    }
    return {r};
  }

  struct Table_releaseResult { the_seats_Result_ Result = the_seats_Result_::ok; };
  Table_releaseResult Table_release(seats_Table& self, U8 owner = 0, U8 player = 0, seats_Seat seat = seats_Seat::pilot) {
    List<seats_Row> rows = List<seats_Row>{};
    the_seats_Result_ r = the_seats_Result_::ok;
    rows = List<seats_Row>{};
    r = the_seats_Result_::ok;
    func_call_on<"JetSeats_Release">(actor_find("JetSeats"), kern::out(n14.rows, n14.result), self.rows_, owner, player, seat);
    rows = n14.rows;
    r = n14.result;
    if (enum_equal<the_seats_Result_>(r, the_seats_Result_::ok)) {
      self.rows_ = rows;
      self.revision_ = kern::to<Int>(kern::formula([](Float revision_) -> Float { return (revision_ + 1.0); }, kern::to<Float>(self.revision_)));
    }
    return {r};
  }

  struct Table_dropResult { Bool Result = false; };
  Table_dropResult Table_drop(seats_Table& self, U8 player = 0) {
    List<seats_Row> rows = List<seats_Row>{};
    Bool changed = false;
    rows = List<seats_Row>{};
    changed = false;
    func_call_on<"JetSeats_Drop">(actor_find("JetSeats"), kern::out(n15.rows, n15.changed), self.rows_, player);
    rows = n15.rows;
    changed = n15.changed;
    if (logic_not(changed)) {
      return {false};
    }
    self.rows_ = rows;
    self.revision_ = kern::to<Int>(kern::formula([](Float revision_) -> Float { return (revision_ + 1.0); }, kern::to<Float>(self.revision_)));
    return {true};
  }

  struct Table_loadResult { Bool Result = false; };
  Table_loadResult Table_load(seats_Table& self, const List<seats_Row>& rows, U32 revision = 0) {
    func_call_on<"JetSeats_Valid">(actor_find("JetSeats"), kern::out(n16.ok), rows);
    if (logic_not(n16.ok)) {
      return {false};
    }
    self.rows_ = rows;
    self.revision_ = revision;
    return {true};
  }

  // ---- what nodes give and keep ----
  struct { Text name = ""; } n10;
  struct { seats_Row row = seats_Row{253, 253, 253}; } n11;
  struct { Bool found = false; seats_Row row = seats_Row{253, 253, 253}; seats_Seat seat = seats_Seat::pilot; } n12;
  struct { List<seats_Row> rows = List<seats_Row>{}; the_seats_Result_ result = the_seats_Result_::ok; } n13;
  struct { List<seats_Row> rows = List<seats_Row>{}; the_seats_Result_ result = the_seats_Result_::ok; } n14;
  struct { List<seats_Row> rows = List<seats_Row>{}; Bool changed = false; } n15;
  struct { Bool ok = false; } n16;
};
