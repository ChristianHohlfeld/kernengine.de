struct town_sync_cpp : virtual kern::Library, virtual shapes, virtual town_sync, virtual wire {
  // ---- variables ----
  KERN_CONST U8 kDoorsMax = 64;
  KERN_CONST U8 kPlanksMax = 6;
  /// a part's size at most (reliable, but small parts interleave with the rest)
  KERN_CONST Int kStateBytes = 1000;
  KERN_CONST Int kLampBytes = 9;
  KERN_CONST Int kPlankBytes = 11;
  KERN_CONST Int kMarkBytes = 20;
  /// the blasts kept (scorches 40, soot 48, breaches 64 - world_marks.h)
  KERN_CONST Int kMarks = 64;

  // ---- functions ----
  struct vec_finiteResult { Bool Result = false; };
  KERN_PURE vec_finiteResult vec_finite(Vector v = Vector{0.0, 0.0, 0.0}) {
    return {logic_and(logic_and(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(v).x), 0.0), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(v).y), 0.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(v).z), 0.0))};
  }

  struct lamp_okResult { Bool Result = false; };
  KERN_PURE lamp_okResult lamp_ok(const town_LampRec& r) {
    return {logic_or(math_compare<"==">(kern::to<Float>(r.state), 1.0), math_compare<"==">(kern::to<Float>(r.state), 2.0))};
  }

  struct plank_okResult { Bool Result = false; };
  KERN_PURE plank_okResult plank_ok(const town_PlankRec& r) {
    return {logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(r.doors), 1.0), math_compare<"<=">(kern::to<Float>(r.doors), kern::to<Float>(kDoorsMax))), math_compare<"<">(kern::to<Float>(r.door), kern::to<Float>(r.doors))), math_compare<"<=">(kern::to<Float>(r.planks), kern::to<Float>(kPlanksMax)))};
  }

  struct mark_okResult { Bool Result = false; };
  KERN_PURE mark_okResult mark_ok(const town_MarkRec& m) {
    return {logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(vec_finite(m.at).Result, math_compare<"<">(kern::formula([](Float x) -> Float { return kern::math::abs(x); }, vector_break(m.at).x), 1000000.0)), math_compare<"<">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, vector_break(m.at).y), 1000000.0)), math_compare<">">(m.radius, 0.0)), math_compare<"<">(m.radius, 200.0)), math_compare<">=">(m.strength, 0.0)), math_compare<"<=">(m.strength, 4.0))};
  }

  struct kind_ofResult { town_Kind Result = town_Kind::none; };
  KERN_PURE kind_ofResult kind_of(const List<U8>& blob) {
    if (logic_or(logic_or(math_compare<"==">(kern::to<Float>(list_length<U8>(blob)), 0.0), math_compare<"<">(kern::to<Float>(blob[0]), 1.0)), math_compare<">">(kern::to<Float>(blob[0]), kern::to<Float>(enum_index<town_Kind>(town_Kind::ask))))) {
      return {town_Kind::none};
    }
    return {enum_from_index<town_Kind>(blob[0]).value};
  }

  struct packResult { List<U8> Result = List<U8>{}; };
  packResult pack(const town_CarHit& h) {
    List<U8> o = List<U8>{};
    o = List<U8>{};
    wire::put_u8(o, enum_index<town_Kind>(town_Kind::car_hit));
    wire::put_u8(o, h.car);
    wire::put_f32(o, h.damage);
    wire::put_u8(o, enum_index<town_Cause>(h.cause));
    return {o};
  }

  struct unpackResult { Bool Result = false; };
  unpackResult unpack(const List<U8>& blob, town_CarHit& h) {
    wire_Reader in = wire_Reader{0, true};
    U8 cause = 0;
    in = wire_Reader{0, true};
    n10 = wire::Reader_get_u8(in, blob);
    if (math_compare<"!=">(kern::to<Float>(n10.Result), kern::to<Float>(enum_index<town_Kind>(town_Kind::car_hit)))) {
      return {false};
    }
    n11 = wire::Reader_get_u8(in, blob);
    h.car = n11.Result;
    n12 = wire::Reader_get_f32(in, blob);
    h.damage = n12.Result;
    n13 = wire::Reader_get_u8(in, blob);
    cause = n13.Result;
    if (logic_or(logic_or(logic_or(logic_or(logic_not(wire::Reader_done(in, blob).Result), math_compare<">">(kern::to<Float>(cause), kern::to<Float>(enum_index<town_Cause>(town_Cause::blast)))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, h.damage), 0.0))), logic_not(math_compare<">">(h.damage, 0.0))), math_compare<">">(h.damage, 10000.0))) {
      return {false};
    }
    h.cause = enum_from_index<town_Cause>(cause).value;
    return {true};
  }

  struct pack_2Result { List<U8> Result = List<U8>{}; };
  pack_2Result pack_2(const town_PersonHit& h) {
    List<U8> o = List<U8>{};
    o = List<U8>{};
    wire::put_u8(o, enum_index<town_Kind>(town_Kind::person_hit));
    wire::put_u8(o, h.person);
    wire::put_u32(o, h.seed);
    wire::put_f32(o, h.damage);
    wire::put_u8(o, h.by);
    wire::put_vec3(o, h.push);
    return {o};
  }

  struct unpack_2Result { Bool Result = false; };
  unpack_2Result unpack_2(const List<U8>& blob, town_PersonHit& h) {
    wire_Reader in = wire_Reader{0, true};
    in = wire_Reader{0, true};
    n14 = wire::Reader_get_u8(in, blob);
    if (math_compare<"!=">(kern::to<Float>(n14.Result), kern::to<Float>(enum_index<town_Kind>(town_Kind::person_hit)))) {
      return {false};
    }
    n15 = wire::Reader_get_u8(in, blob);
    h.person = n15.Result;
    n16 = wire::Reader_get_u32(in, blob);
    h.seed = n16.Result;
    n17 = wire::Reader_get_f32(in, blob);
    h.damage = n17.Result;
    n18 = wire::Reader_get_u8(in, blob);
    h.by = n18.Result;
    n19 = wire::Reader_get_vec3(in, blob);
    h.push = n19.Result;
    return {logic_and(logic_and(logic_and(logic_and(logic_and(logic_and(wire::Reader_done(in, blob).Result, math_compare<"<=">(kern::to<Float>(h.by), 2.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, h.damage), 0.0)), math_compare<">">(h.damage, 0.0)), math_compare<"<=">(h.damage, 10000.0)), vec_finite(h.push).Result), math_compare<"<=">(vector_length(h.push), 40.0))};
  }

  struct pack_lampsResult { List<U8_row> Result = List<U8_row>{}; };
  pack_lampsResult pack_lamps(const List<town_LampRec>& lamps) {
    List<U8_row> out = List<U8_row>{};
    Int from = 0;
    Int n = 0;
    List<U8> o = List<U8>{};
    out = List<U8_row>{};
    from = 0;
    while (loop(math_compare<">">(kern::formula([](Float from, Float a) -> Float { return ((from < 0.0) != (a < 0.0) ? (a < 0.0) : (from < a)); }, kern::to<Float>(from), kern::to<Float>(list_length<town_LampRec>(lamps))), 0.5))) {
      n = kern::to<Int>(kern::formula([](Float a, Float from) -> Float { return kern::math::min(255.0, (a - from)); }, kern::to<Float>(list_length<town_LampRec>(lamps)), kern::to<Float>(from)));
      o = List<U8>{};
      wire::put_u8(o, enum_index<town_Kind>(town_Kind::lamps));
      wire::put_u8(o, kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::band(n, 255.0); }, kern::to<Float>(n))));
      for (auto it20 = flow_for(n20, from, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float from, Float n) -> Float { return (from + n); }, kern::to<Float>(from), kern::to<Float>(n)))))))); it20.next();) {
        wire::put_i64(o, lamps[n20.index].key);
        wire::put_u8(o, lamps[n20.index].state);
      }
      list_add(out, struct_make<U8_row>(o));
      from += 255;
    }
    return {out};
  }

  struct unpack_lampsResult { Bool Result = false; };
  unpack_lampsResult unpack_lamps(const List<U8>& blob, List<town_LampRec>& lamps) {
    wire_Reader in = wire_Reader{0, true};
    U8 n = 0;
    U8 i = 0;
    town_LampRec r = town_LampRec{Int64{0x0}, 0};
    in = wire_Reader{0, true};
    list_clear(lamps);
    n21 = wire::Reader_get_u8(in, blob);
    if (math_compare<"!=">(kern::to<Float>(n21.Result), kern::to<Float>(enum_index<town_Kind>(town_Kind::lamps)))) {
      return {false};
    }
    n22 = wire::Reader_get_u8(in, blob);
    n = n22.Result;
    i = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n)), in.ok))) {
      r = town_LampRec{Int64{0x0}, 0};
      n23 = wire::Reader_get_i64(in, blob);
      r.key = n23.Result;
      n24 = wire::Reader_get_u8(in, blob);
      r.state = n24.Result;
      if (logic_or(logic_not(in.ok), logic_not(lamp_ok(r).Result))) {
        return {false};
      }
      list_add(lamps, r);
      i += 1;
    }
    return {wire::Reader_done(in, blob).Result};
  }

  struct pack_3Result { List<U8> Result = List<U8>{}; };
  pack_3Result pack_3(const town_PlankStep& s) {
    List<U8> o = List<U8>{};
    o = List<U8>{};
    wire::put_u8(o, enum_index<town_Kind>(town_Kind::plank_step));
    wire::put_i64(o, s.key);
    wire::put_u8(o, s.door);
    wire::put_u8(o, s.doors);
    wire::put_i8(o, s.step);
    return {o};
  }

  struct unpack_3Result { Bool Result = false; };
  unpack_3Result unpack_3(const List<U8>& blob, town_PlankStep& s) {
    wire_Reader in = wire_Reader{0, true};
    in = wire_Reader{0, true};
    n25 = wire::Reader_get_u8(in, blob);
    if (math_compare<"!=">(kern::to<Float>(n25.Result), kern::to<Float>(enum_index<town_Kind>(town_Kind::plank_step)))) {
      return {false};
    }
    n26 = wire::Reader_get_i64(in, blob);
    s.key = n26.Result;
    n27 = wire::Reader_get_u8(in, blob);
    s.door = n27.Result;
    n28 = wire::Reader_get_u8(in, blob);
    s.doors = n28.Result;
    n29 = wire::Reader_get_i8(in, blob);
    s.step = n29.Result;
    return {logic_and(logic_and(logic_and(logic_and(wire::Reader_done(in, blob).Result, math_compare<">=">(kern::to<Float>(s.doors), 1.0)), math_compare<"<=">(kern::to<Float>(s.doors), kern::to<Float>(kDoorsMax))), math_compare<"<">(kern::to<Float>(s.door), kern::to<Float>(s.doors))), logic_or(math_compare<"==">(kern::to<Float>(s.step), 1.0), math_compare<"==">(kern::to<Float>(s.step), -1.0)))};
  }

  struct pack_4Result { List<U8> Result = List<U8>{}; };
  pack_4Result pack_4(const town_PlankRec& r) {
    List<U8> o = List<U8>{};
    o = List<U8>{};
    wire::put_u8(o, enum_index<town_Kind>(town_Kind::planks));
    wire::put_i64(o, r.key);
    wire::put_u8(o, r.door);
    wire::put_u8(o, r.doors);
    wire::put_u8(o, r.planks);
    return {o};
  }

  struct unpack_4Result { Bool Result = false; };
  unpack_4Result unpack_4(const List<U8>& blob, town_PlankRec& r) {
    wire_Reader in = wire_Reader{0, true};
    in = wire_Reader{0, true};
    n30 = wire::Reader_get_u8(in, blob);
    if (math_compare<"!=">(kern::to<Float>(n30.Result), kern::to<Float>(enum_index<town_Kind>(town_Kind::planks)))) {
      return {false};
    }
    n31 = wire::Reader_get_i64(in, blob);
    r.key = n31.Result;
    n32 = wire::Reader_get_u8(in, blob);
    r.door = n32.Result;
    n33 = wire::Reader_get_u8(in, blob);
    r.doors = n33.Result;
    n34 = wire::Reader_get_u8(in, blob);
    r.planks = n34.Result;
    return {logic_and(wire::Reader_done(in, blob).Result, plank_ok(r).Result)};
  }

  struct pack_askResult { List<U8> Result = List<U8>{}; };
  KERN_PURE pack_askResult pack_ask() {
    return {list_make<U8, "1">(enum_index<town_Kind>(town_Kind::ask))};
  }

  struct pack_stateResult { List<U8_row> Result = List<U8_row>{}; };
  pack_stateResult pack_state(const List<town_LampRec>& lamps, const List<town_PlankRec>& planks, const List<town_MarkRec>& marks, U32 epoch = 0) {
    List<U8_row> out = List<U8_row>{};
    Int li = 0;
    Int the_pi = 0;
    Int mi = 0;
    Bool again = false;
    Int room = 0;
    Int nl = 0;
    Int np = 0;
    Int nm = 0;
    Bool last = false;
    List<U8> o = List<U8>{};
    out = List<U8_row>{};
    li = 0;
    the_pi = 0;
    mi = 0;
    again = true;
    while (loop(again)) {
      room = kern::to<Int>(kern::formula([](Float kStateBytes) -> Float { return (kStateBytes - 12.0); }, kern::to<Float>(kStateBytes)));
      nl = kern::to<Int>(kern::formula([](Float a, Float li, Float room, Float kLampBytes) -> Float { return kern::math::min((a - li), kern::math::trunc(kern::math::div(room, kLampBytes))); }, kern::to<Float>(list_length<town_LampRec>(lamps)), kern::to<Float>(li), kern::to<Float>(room), kern::to<Float>(kLampBytes)));
      room = kern::to<Int>(kern::formula([](Float room, Float nl, Float kLampBytes) -> Float { return (room - (nl * kLampBytes)); }, kern::to<Float>(room), kern::to<Float>(nl), kern::to<Float>(kLampBytes)));
      np = kern::to<Int>(kern::formula([](Float li, Float nl, Float a, Float a_2, Float the_pi, Float room, Float kPlankBytes) -> Float { return (((li + nl) == a) ? kern::math::min((a_2 - the_pi), kern::math::trunc(kern::math::div(room, kPlankBytes))) : 0.0); }, kern::to<Float>(li), kern::to<Float>(nl), kern::to<Float>(list_length<town_LampRec>(lamps)), kern::to<Float>(list_length<town_PlankRec>(planks)), kern::to<Float>(the_pi), kern::to<Float>(room), kern::to<Float>(kPlankBytes)));
      room = kern::to<Int>(kern::formula([](Float room, Float np, Float kPlankBytes) -> Float { return (room - (np * kPlankBytes)); }, kern::to<Float>(room), kern::to<Float>(np), kern::to<Float>(kPlankBytes)));
      nm = kern::to<Int>(kern::formula([](Float li, Float nl, Float a, Float the_pi, Float np, Float a_2, Float a_3, Float mi, Float room, Float kMarkBytes) -> Float { return ((((li + nl) == a) && ((the_pi + np) == a_2)) ? kern::math::min((a_3 - mi), kern::math::trunc(kern::math::div(room, kMarkBytes))) : 0.0); }, kern::to<Float>(li), kern::to<Float>(nl), kern::to<Float>(list_length<town_LampRec>(lamps)), kern::to<Float>(the_pi), kern::to<Float>(np), kern::to<Float>(list_length<town_PlankRec>(planks)), kern::to<Float>(list_length<town_MarkRec>(marks)), kern::to<Float>(mi), kern::to<Float>(room), kern::to<Float>(kMarkBytes)));
      last = logic_and(logic_and(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float li, Float nl) -> Float { return (li + nl); }, kern::to<Float>(li), kern::to<Float>(nl))))), kern::to<Float>(list_length<town_LampRec>(lamps))), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float the_pi, Float np) -> Float { return (the_pi + np); }, kern::to<Float>(the_pi), kern::to<Float>(np))))), kern::to<Float>(list_length<town_PlankRec>(planks)))), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float mi, Float nm) -> Float { return (mi + nm); }, kern::to<Float>(mi), kern::to<Float>(nm))))), kern::to<Float>(list_length<town_MarkRec>(marks))));
      o = List<U8>{};
      wire::put_u8(o, enum_index<town_Kind>(town_Kind::state));
      wire::put_u32(o, epoch);
      wire::put_u8(o, kern::to<Int>(kern::formula([](Float a, Float last) -> Float { return kern::math::band(kern::math::bor(((a != 0.0) ? 1.0 : 0.0), ((last != 0.0) ? 2.0 : 0.0)), 255.0); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<U8_row>(out)), 0.0)), kern::to<Float>(last))));
      wire::put_u16(o, kern::to<Int>(kern::formula([](Float nl) -> Float { return kern::math::band(nl, 65535.0); }, kern::to<Float>(nl))));
      for (auto it35 = flow_for(n35, li, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float li, Float nl) -> Float { return (li + nl); }, kern::to<Float>(li), kern::to<Float>(nl)))))))); it35.next();) {
        wire::put_i64(o, lamps[n35.index].key);
        wire::put_u8(o, lamps[n35.index].state);
      }
      wire::put_u16(o, kern::to<Int>(kern::formula([](Float np) -> Float { return kern::math::band(np, 65535.0); }, kern::to<Float>(np))));
      for (auto it36 = flow_for(n36, the_pi, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float the_pi, Float np) -> Float { return (the_pi + np); }, kern::to<Float>(the_pi), kern::to<Float>(np)))))))); it36.next();) {
        wire::put_i64(o, planks[n36.index].key);
        wire::put_u8(o, planks[n36.index].door);
        wire::put_u8(o, planks[n36.index].doors);
        wire::put_u8(o, planks[n36.index].planks);
      }
      wire::put_u16(o, kern::to<Int>(kern::formula([](Float nm) -> Float { return kern::math::band(nm, 65535.0); }, kern::to<Float>(nm))));
      for (auto it37 = flow_for(n37, mi, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float mi, Float nm) -> Float { return (mi + nm); }, kern::to<Float>(mi), kern::to<Float>(nm)))))))); it37.next();) {
        wire::put_vec3(o, marks[n37.index].at);
        wire::put_f32(o, marks[n37.index].radius);
        wire::put_f32(o, marks[n37.index].strength);
      }
      list_add(out, struct_make<U8_row>(o));
      li += nl;
      the_pi += np;
      mi += nm;
      again = logic_or(logic_or(math_compare<">">(kern::formula([](Float li, Float a) -> Float { return ((li < 0.0) != (a < 0.0) ? (a < 0.0) : (li < a)); }, kern::to<Float>(li), kern::to<Float>(list_length<town_LampRec>(lamps))), 0.5), math_compare<">">(kern::formula([](Float the_pi, Float a) -> Float { return ((the_pi < 0.0) != (a < 0.0) ? (a < 0.0) : (the_pi < a)); }, kern::to<Float>(the_pi), kern::to<Float>(list_length<town_PlankRec>(planks))), 0.5)), math_compare<">">(kern::formula([](Float mi, Float a) -> Float { return ((mi < 0.0) != (a < 0.0) ? (a < 0.0) : (mi < a)); }, kern::to<Float>(mi), kern::to<Float>(list_length<town_MarkRec>(marks))), 0.5));
    }
    return {out};
  }

  struct unpack_stateResult { Bool Result = false; };
  unpack_stateResult unpack_state(const List<U8>& blob, town_State& s) {
    wire_Reader in = wire_Reader{0, true};
    U8 flags = 0;
    U16 nl = 0;
    U16 i = 0;
    town_LampRec r = town_LampRec{Int64{0x0}, 0};
    U16 np = 0;
    U16 i_2 = 0;
    town_PlankRec r_2 = town_PlankRec{Int64{0x0}, 0, 0, 0};
    U16 nm = 0;
    U16 i_3 = 0;
    town_MarkRec m = town_MarkRec{Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
    in = wire_Reader{0, true};
    s = struct_make<town_State>(0, false, false, List<town_LampRec>{}, List<town_PlankRec>{}, List<town_MarkRec>{});
    n38 = wire::Reader_get_u8(in, blob);
    if (math_compare<"!=">(kern::to<Float>(n38.Result), kern::to<Float>(enum_index<town_Kind>(town_Kind::state)))) {
      return {false};
    }
    n39 = wire::Reader_get_u32(in, blob);
    s.epoch = n39.Result;
    n40 = wire::Reader_get_u8(in, blob);
    flags = n40.Result;
    s.first = math_compare<">">(kern::formula([](Float flags) -> Float { return (kern::math::band(flags, 1.0) != 0.0); }, kern::to<Float>(flags)), 0.5);
    s.last = math_compare<">">(kern::formula([](Float flags) -> Float { return (kern::math::band(flags, 2.0) != 0.0); }, kern::to<Float>(flags)), 0.5);
    n41 = wire::Reader_get_u16(in, blob);
    nl = n41.Result;
    i = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(nl)), in.ok))) {
      r = town_LampRec{Int64{0x0}, 0};
      n42 = wire::Reader_get_i64(in, blob);
      r.key = n42.Result;
      n43 = wire::Reader_get_u8(in, blob);
      r.state = n43.Result;
      if (logic_or(logic_not(in.ok), logic_not(lamp_ok(r).Result))) {
        return {false};
      }
      list_add(s.lamps, r);
      i += 1;
    }
    n44 = wire::Reader_get_u16(in, blob);
    np = n44.Result;
    i_2 = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i_2), kern::to<Float>(np)), in.ok))) {
      r_2 = town_PlankRec{Int64{0x0}, 0, 0, 0};
      n45 = wire::Reader_get_i64(in, blob);
      r_2.key = n45.Result;
      n46 = wire::Reader_get_u8(in, blob);
      r_2.door = n46.Result;
      n47 = wire::Reader_get_u8(in, blob);
      r_2.doors = n47.Result;
      n48 = wire::Reader_get_u8(in, blob);
      r_2.planks = n48.Result;
      if (logic_or(logic_not(in.ok), logic_not(plank_ok(r_2).Result))) {
        return {false};
      }
      list_add(s.planks, r_2);
      i_2 += 1;
    }
    n49 = wire::Reader_get_u16(in, blob);
    nm = n49.Result;
    i_3 = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i_3), kern::to<Float>(nm)), in.ok))) {
      m = town_MarkRec{Vector{0.0, 0.0, 0.0}, 0.0, 0.0};
      n50 = wire::Reader_get_vec3(in, blob);
      m.at = n50.Result;
      n51 = wire::Reader_get_f32(in, blob);
      m.radius = n51.Result;
      n52 = wire::Reader_get_f32(in, blob);
      m.strength = n52.Result;
      if (logic_or(logic_not(in.ok), logic_not(mark_ok(m).Result))) {
        return {false};
      }
      list_add(s.marks, m);
      i_3 += 1;
    }
    return {wire::Reader_done(in, blob).Result};
  }

  // ---- what nodes give and keep ----
  wire::Reader_get_u8Result n10;
  wire::Reader_get_u8Result n11;
  wire::Reader_get_f32Result n12;
  wire::Reader_get_u8Result n13;
  wire::Reader_get_u8Result n14;
  wire::Reader_get_u8Result n15;
  wire::Reader_get_u32Result n16;
  wire::Reader_get_f32Result n17;
  wire::Reader_get_u8Result n18;
  wire::Reader_get_vec3Result n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  wire::Reader_get_u8Result n21;
  wire::Reader_get_u8Result n22;
  wire::Reader_get_i64Result n23;
  wire::Reader_get_u8Result n24;
  wire::Reader_get_u8Result n25;
  wire::Reader_get_i64Result n26;
  wire::Reader_get_u8Result n27;
  wire::Reader_get_u8Result n28;
  wire::Reader_get_i8Result n29;
  wire::Reader_get_u8Result n30;
  wire::Reader_get_i64Result n31;
  wire::Reader_get_u8Result n32;
  wire::Reader_get_u8Result n33;
  wire::Reader_get_u8Result n34;
  struct : kern::Node<35> { Int index = 0; } n35;
  struct : kern::Node<36> { Int index = 0; } n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  wire::Reader_get_u8Result n38;
  wire::Reader_get_u32Result n39;
  wire::Reader_get_u8Result n40;
  wire::Reader_get_u16Result n41;
  wire::Reader_get_i64Result n42;
  wire::Reader_get_u8Result n43;
  wire::Reader_get_u16Result n44;
  wire::Reader_get_i64Result n45;
  wire::Reader_get_u8Result n46;
  wire::Reader_get_u8Result n47;
  wire::Reader_get_u8Result n48;
  wire::Reader_get_u16Result n49;
  wire::Reader_get_vec3Result n50;
  wire::Reader_get_f32Result n51;
  wire::Reader_get_f32Result n52;
};
