struct game_swarm_cpp : virtual kern::Library, virtual game {
  // ---- functions ----
  struct is_personResult { Bool Result = false; };
  KERN_PURE is_personResult is_person(net_SwarmKind k = net_SwarmKind::zombie) {
    return {logic_or(logic_or(logic_or(enum_equal<net_SwarmKind>(k, net_SwarmKind::zombie), enum_equal<net_SwarmKind>(k, net_SwarmKind::soldier)), enum_equal<net_SwarmKind>(k, net_SwarmKind::officer)), enum_equal<net_SwarmKind>(k, net_SwarmKind::enemy))};
  }

  struct fallsResult { Bool Result = false; };
  /// the kinds whose death is shown: the unit goes on for a moment with kUnitDead
  KERN_PURE fallsResult falls(net_SwarmKind k = net_SwarmKind::zombie) {
    return {logic_or(enum_equal<net_SwarmKind>(k, net_SwarmKind::officer), enum_equal<net_SwarmKind>(k, net_SwarmKind::enemy))};
  }

  struct drone_idResult { U16 Result = 0; };
  KERN_PURE drone_idResult drone_id(const PoliceDrone& d) {
    return {kern::to<Int>(kern::formula([](Float serial) -> Float { return kern::math::band(kern::math::wrap32(kern::math::mod(serial, 65535.0) + 1.0), 65535.0); }, kern::to<Float>(d.serial)))};
  }
};
