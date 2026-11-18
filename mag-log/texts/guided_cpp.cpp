struct guided_cpp : virtual kern::Library, virtual blueprint_call, virtual guided {
  // ---- types ----
  KERN_STRUCT(guided_MissileRun) { Vector p = Vector{0.0, 0.0, 0.0}; Vector v = Vector{0.0, 0.0, 0.0}; Vector goal = Vector{0.0, 0.0, 0.0}; Float born = 0.0; Float cruise_z = 0.0; Bool motor = false; Bool pedestrian = false; Float dodge_seconds = 0.0; Bool dodge_lost = false; Vector dodge_last = Vector{0.0, 0.0, 0.0}; Bool tracking = true; };

  // ---- functions ----
  struct flight_ofResult { guided_MissileRun Result = guided_MissileRun{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, 0.0, false, Vector{0.0, 0.0, 0.0}, true}; };
  KERN_PURE flight_ofResult flight_of(const guided_Missile& m) {
    return {struct_make<guided_MissileRun>(m.p, m.v, m.goal, blueprint_call::decimal(m.born).Result, blueprint_call::decimal(m.cruise_z).Result, m.motor, m.pedestrian, blueprint_call::decimal(m.dodge.seconds).Result, m.dodge.lost, vector_make(vector_break(m.dodge_last).x, vector_break(m.dodge_last).y, 0.0), m.tracking)};
  }

  void take(const guided_MissileRun& f, guided_Missile& m) {
    m.p = f.p;
    m.v = f.v;
    m.goal = f.goal;
    m.born = f.born;
    m.cruise_z = f.cruise_z;
    m.motor = f.motor;
    m.pedestrian = f.pedestrian;
    m.dodge.seconds = f.dodge_seconds;
    m.dodge.lost = f.dodge_lost;
    m.dodge_last = vector_make(vector_break(f.dodge_last).x, vector_break(f.dodge_last).y, 0.0);
    m.tracking = f.tracking;
  }
};
