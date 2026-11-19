struct game_vehicle_damage_cpp : virtual kern::Library, virtual game {
  // ---- functions ----
  struct is_busResult { Bool Result = false; };
  KERN_PURE is_busResult is_bus(const TrafficCar& c) {
    return {math_compare<">">(c.car[0].s_.radius, 0.4)};
  }
};
