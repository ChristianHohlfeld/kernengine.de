struct car : virtual kern::Library, virtual engine_physics_physics {
  // ---- types ----
  KERN_STRUCT(CarSpec_Wheel) { Float x = 0.0; Float y = 0.0; Bool front = false; };
  KERN_STRUCT(Car_Applied) { Vector p = Vector{0.0, 0.0, 0.0}; Vector f = Vector{0.0, 0.0, 0.0}; I32 on = 0; };
  KERN_STRUCT(WheelState) { Float omega = 0.0; Float angle = 0.0; Float travel = 0.0; Float load = 0.0; Float load_f = -1.0; Float fx = 0.0; Float fy = 0.0; Float kappa = 0.0; Float alpha = 0.0; Bool contact = false; Bool has_travel = false; Vector hub = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(CarSpec) { Text kind = "car"; Vector half = Vector{1.78, 0.82, 0.36}; Float mass = 1350.0; Float com_z = -0.06; Float com_x = 0.15; Float radius = 0.34; Float wheel_width = 0.22; Float wheel_mass = 22.0; Float attach_z = -0.26; Fixed<CarSpec_Wheel, 4> wheels = List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}; Float susp_lower = -0.15; Float susp_upper = 0.11; Float sag = 0.06; Float torque_front = 550.0; Float torque_rear = 850.0; Float power = 140000.0; Float steer_lag = 0.15; Float steer_damp_speed = 15.0; Float drag_area = 0.68; Float rolling_resistance = 0.013; Float wheel_inertia = 1.3; Float brake = 2400.0; Float handbrake = 3200.0; Float coast = 30.0; Float steer_lock = 0.58; Float steer_hand = 430.0; Float steer_assist = 600.0; Float steer_assist_speed = 6.0; Float friction = 1.1; Float rack_rate = 2.8; Float steer_servo = 3000.0; Float upper_mass = 95.0; List<Vector> upper_profile = List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}; Float upper_half_width = 0.71; Float upper_dz = 0.54; };
  KERN_STRUCT(Car_Rig) { physics_Body body = physics_Body{Int64{0x0}}; physics_Joint joint = physics_Joint{Int64{0x0}}; physics_Joint strut = physics_Joint{Int64{0x0}}; };
  KERN_STRUCT(Car) { Int64 hit_user = Int64{0x0}; CarSpec s_ = CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54}; Fixed<WheelState, 4> w_ = List<WheelState>{}; Float steer_ = 0.0; Float steer_eq_f_ = 0.0; Float vf_ = 0.0; Float throttle_ = 0.0; I32 gear_ = 1; Fixed<Car_Applied, 6> applied_ = List<Car_Applied>{}; I32 n_applied_ = 0; Fixed<Car_Rig, 4> rig_ = List<Car_Rig>{}; Int rig_world_ = 0; physics_Body rig_chassis_ = physics_Body{Int64{0x0}}; Vector last_pos_ = Vector{0.0, 0.0, 0.0}; Bool has_last_ = false; };

  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct CarSpec_carResult { CarSpec Result = CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54}; };
  KERN_PURE CarSpec_carResult CarSpec_car() {
    return {struct_make<CarSpec>("car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54)};
  }

  struct Car_newResult { Car Result = Car{Int64{0x0}, CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54}, List<WheelState>{}, 0.0, 0.0, 0.0, 0.0, 1, List<Car_Applied>{}, 0, List<Car_Rig>{}, 0, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, false}; };
  KERN_PURE Car_newResult Car_new(const CarSpec& s) {
    Car self = Car{Int64{0x0}, CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54}, List<WheelState>{}, 0.0, 0.0, 0.0, 0.0, 1, List<Car_Applied>{}, 0, List<Car_Rig>{}, 0, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, false};
    self.s_ = s;
    return {self};
  }

  struct Car_riggedResult { Bool Result = false; };
  KERN_PURE Car_riggedResult Car_rigged(const Car& self) {
    return {math_compare<"!=">(kern::to<Float>(self.rig_world_), 0.0)};
  }

  /// A car somebody else drives (shown only): its steering angle, and its wheels rolling at `v`.
  void Car_show(Car& self, Float steer = 0.0) {
    self.steer_ = steer;
  }

  void Car_roll(Car& self, Float v = 0.0, Float dt = 0.0) {
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WheelState>(self.w_))))); it10.next();) {
      self.w_[n10.index].omega = kern::formula([](Float v, Float radius) -> Float { return kern::math::div(v, radius); }, v, self.s_.radius);
      self.w_[n10.index].angle = kern::formula([](Float a, Float a_2, Float dt, Float kPi) -> Float { return kern::math::mod((a + (a_2 * dt)), (2.0 * kPi)); }, self.w_[n10.index].angle, self.w_[n10.index].omega, dt, kPi);
    }
  }

  struct Car_wheel_bodyResult { physics_Body Result = physics_Body{Int64{0x0}}; };
  /// One wheel body of the rig (valid while rigged): for whoever must know it (a ray's ignore).
  KERN_PURE Car_wheel_bodyResult Car_wheel_body(const Car& self, I32 i = 0) {
    return {self.rig_[i].body};
  }

  struct Car_ground_offsetResult { Float Result = 0.0; };
  /// Height of the chassis centre above flat ground at rest.
  KERN_PURE Car_ground_offsetResult Car_ground_offset(const CarSpec& s) {
    return {kern::formula([](Float radius, Float attach_z, Float sag) -> Float { return ((radius - attach_z) - sag); }, s.radius, s.attach_z, s.sag)};
  }

  struct Car_steer_angleResult { Float Result = 0.0; };
  KERN_PURE Car_steer_angleResult Car_steer_angle(const Car& self) {
    return {self.steer_};
  }

  struct Car_forward_speedResult { Float Result = 0.0; };
  KERN_PURE Car_forward_speedResult Car_forward_speed(const Car& self) {
    return {self.vf_};
  }

  struct Car_gearResult { I32 Result = 0; };
  KERN_PURE Car_gearResult Car_gear(const Car& self) {
    return {self.gear_};
  }

  struct Car_throttleResult { Float Result = 0.0; };
  /// Engine load 0..1 and the wheel speed for the engine sound.
  KERN_PURE Car_throttleResult Car_throttle(const Car& self) {
    return {self.throttle_};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
