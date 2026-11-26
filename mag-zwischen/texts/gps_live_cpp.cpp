struct gps_live_cpp : virtual kern::Library, virtual gps_live {
  // ---- variables ----
  /// gps_live_mode.mjs: MAX_ACCURACY_M 65, MAX_SPEED_MPS 75, GAP_RESET_MS 12000, REANCHOR_KM 40
  KERN_CONST Float kMaxAccuracy = 65.0;
  /// gps_live_mode.mjs: MAX_ACCURACY_M 65, MAX_SPEED_MPS 75, GAP_RESET_MS 12000, REANCHOR_KM 40
  KERN_CONST Float kGapReset = 12.0;
  /// gps_live_mode.mjs: MAX_ACCURACY_M 65, MAX_SPEED_MPS 75, GAP_RESET_MS 12000, REANCHOR_KM 40
  KERN_CONST Float kMaxSpeed = 75.0;

  // ---- functions ----
  void GpsLive_start(GpsLive& self, Float now = 0.0) {
    GpsLive_View v = GpsLive_View::eye;
    v = self.view;
    self = GpsLive{GpsLive_View::eye, 0, 0, false, false, false, false, 0.0, -1000000000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}};
    self.view = v;
    self.active_ = true;
    self.started_ = now;
  }

  struct GpsLive_fixResult { Bool Result = false; };
  GpsLive_fixResult GpsLive_fix(GpsLive& self, Float now = 0.0, Float x = 0.0, Float y = 0.0, Float accuracy = 0.0, Float speed = 0.0, Float course = 0.0) {
    Float dt = 0.0;
    Float h = 0.0;
    Float px = 0.0;
    Float py = 0.0;
    Float ex = 0.0;
    Float ey = 0.0;
    Float jump = 0.0;
    Float g = 0.0;
    Float gv = 0.0;
    Float ox = 0.0;
    Float oy = 0.0;
    Float sp = 0.0;
    Float k = 0.0;
    if (logic_or(logic_or(logic_not(self.active_), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, x), 0.0))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, y), 0.0)))) {
      return {false};
    }
    self.accuracy_ = kern::formula([](Float accuracy) -> Float { return kern::math::max(0.0, accuracy); }, accuracy);
    if (math_compare<">">(self.accuracy_, kMaxAccuracy)) {
      self.rejected = kern::to<Int>(kern::formula([](Float rejected) -> Float { return (rejected + 1.0); }, kern::to<Float>(self.rejected)));
      return {false};
    }
    dt = kern::formula([](Float now, Float last_fix_) -> Float { return (now - last_fix_); }, now, self.last_fix_);
    if (logic_or(logic_not(self.has_fix_), math_compare<">">(dt, kGapReset))) {
      self.x_ = x;
      self.y_ = y;
      self.vy_ = 0.0;
      self.vx_ = self.vy_;
      self.has_fix_ = true;
      self.has_shown_ = false;
      self.last_fix_ = now;
      self.accepted = kern::to<Int>(kern::formula([](Float accepted) -> Float { return (accepted + 1.0); }, kern::to<Float>(self.accepted)));
      return {true};
    }
    h = kern::formula([](Float dt) -> Float { return kern::math::max(0.2, dt); }, dt);
    px = kern::formula([](Float x_, Float vx_, Float dt) -> Float { return (x_ + (vx_ * dt)); }, self.x_, self.vx_, dt);
    py = kern::formula([](Float y_, Float vy_, Float dt) -> Float { return (y_ + (vy_ * dt)); }, self.y_, self.vy_, dt);
    ex = kern::formula([](Float x, Float px) -> Float { return (x - px); }, x, px);
    ey = kern::formula([](Float y, Float py) -> Float { return (y - py); }, y, py);
    jump = kern::formula([](Float x, Float x_, Float y, Float y_, Float h) -> Float { return kern::math::div(kern::math::sqrt(((x - x_) * (x - x_)) + ((y - y_) * (y - y_))), h); }, x, self.x_, y, self.y_, h);
    if (logic_and(math_compare<">">(jump, kMaxSpeed), math_compare<">">(self.accuracy_, 8.0))) {
      self.rejected = kern::to<Int>(kern::formula([](Float rejected) -> Float { return (rejected + 1.0); }, kern::to<Float>(self.rejected)));
      return {false};
    }
    g = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float accuracy_) -> Float { return (1.0 - kern::math::div(accuracy_, (accuracy_ + 12.0))); }, self.accuracy_), 0.25, 0.85));
    gv = kern::formula([](Float a, Float h) -> Float { return kern::math::div(a, h); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float g) -> Float { return (g * 0.6); }, g), 0.15, 0.6), h);
    ox = self.x_;
    oy = self.y_;
    self.x_ = kern::formula([](Float px, Float ex, Float g) -> Float { return (px + (ex * g)); }, px, ex, g);
    self.y_ = kern::formula([](Float py, Float ey, Float g) -> Float { return (py + (ey * g)); }, py, ey, g);
    self.vx_ = kern::formula([](Float vx_, Float ex, Float gv) -> Float { return (vx_ + (ex * gv)); }, self.vx_, ex, gv);
    self.vy_ = kern::formula([](Float vy_, Float ey, Float gv) -> Float { return (vy_ + (ey * gv)); }, self.vy_, ey, gv);
    if (math_compare<">=">(speed, 0.0)) {
      sp = kern::formula([](Float vx_, Float vy_) -> Float { return kern::math::sqrt((vx_ * vx_) + (vy_ * vy_)); }, self.vx_, self.vy_);
      if (math_compare<">">(sp, 0.05)) {
        k = kern::formula([](Float speed, Float sp) -> Float { return kern::math::div(speed, sp); }, speed, sp);
        self.vx_ = kern::formula([](Float vx_, Float k) -> Float { return ((vx_ * 0.5) + ((vx_ * k) * 0.5)); }, self.vx_, k);
        self.vy_ = kern::formula([](Float vy_, Float k) -> Float { return ((vy_ * 0.5) + ((vy_ * k) * 0.5)); }, self.vy_, k);
      } else if (math_compare<"<">(speed, 0.3)) {
        self.vx_ = kern::formula([](Float vx_) -> Float { return (vx_ * 0.5); }, self.vx_);
        self.vy_ = kern::formula([](Float vy_) -> Float { return (vy_ * 0.5); }, self.vy_);
      }
    }
    self.speed_ = kern::formula([](Float vx_, Float vy_) -> Float { return kern::math::sqrt((vx_ * vx_) + (vy_ * vy_)); }, self.vx_, self.vy_);
    if (math_compare<">">(self.speed_, 1.2)) {
      self.heading_ = kern::formula([](Float course, Float speed, Float vx_, Float vy_) -> Float { return (((course >= 0.0) && (speed > 1.0)) ? course : kern::math::atan2(vx_, vy_)); }, course, speed, self.vx_, self.vy_);
    }
    self.distance_ = kern::formula([](Float distance_, Float x_, Float ox, Float y_, Float oy, Float kMaxSpeed, Float h) -> Float { return (distance_ + kern::math::min(kern::math::sqrt(((x_ - ox) * (x_ - ox)) + ((y_ - oy) * (y_ - oy))), (kMaxSpeed * h))); }, self.distance_, self.x_, ox, self.y_, oy, kMaxSpeed, h);
    self.last_fix_ = now;
    self.accepted = kern::to<Int>(kern::formula([](Float accepted) -> Float { return (accepted + 1.0); }, kern::to<Float>(self.accepted)));
    return {true};
  }

  void GpsLive_frame(GpsLive& self, Float now = 0.0, Float dt = 0.0) {
    Float ahead = 0.0;
    Vector target = Vector{0.0, 0.0, 0.0};
    Float k = 0.0;
    if (logic_or(logic_not(self.active_), logic_not(self.has_fix_))) {
      return;
    }
    ahead = kern::formula([](Float now, Float last_fix_) -> Float { return kern::math::min(2.0, (now - last_fix_)); }, now, self.last_fix_);
    target = vector_make(kern::formula([](Float x_, Float vx_, Float ahead) -> Float { return (x_ + (vx_ * ahead)); }, self.x_, self.vx_, ahead), kern::formula([](Float y_, Float vy_, Float ahead) -> Float { return (y_ + (vy_ * ahead)); }, self.y_, self.vy_, ahead), 0.0);
    if (logic_or(logic_not(self.has_shown_), math_compare<">">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(target).x, vector_break(self.shown_).x, vector_break(target).y, vector_break(self.shown_).y), 60.0))) {
      self.shown_ = target;
      self.has_shown_ = true;
    } else {
      k = kern::formula([](Float dt) -> Float { return (1.0 - kern::math::exp(((-dt) * 4.0))); }, dt);
      self.shown_ = vector_add(self.shown_, vector_scale(vector_sub(target, self.shown_), k));
    }
    self.speed_avg_ = kern::formula([](Float speed_avg_, Float speed_, Float dt) -> Float { return (speed_avg_ + ((speed_ - speed_avg_) * kern::math::min(1.0, kern::math::div(dt, 4.0)))); }, self.speed_avg_, self.speed_, dt);
    if (logic_and(math_compare<"<">(self.speed_avg_, 1.0), self.has_compass_)) {
      self.heading_ = self.compass_;
    }
  }

  struct GpsLive_activityResult { GpsLive_Activity Result = GpsLive_Activity::stand; };
  KERN_PURE GpsLive_activityResult GpsLive_activity(const GpsLive& self) {
    Float s = 0.0;
    s = self.speed_avg_;
    return {logic_select<GpsLive_Activity>(math_compare<"<">(s, 0.5), GpsLive_Activity::stand, logic_select<GpsLive_Activity>(math_compare<"<">(s, 2.3), GpsLive_Activity::walk, logic_select<GpsLive_Activity>(math_compare<"<">(s, 5.2), GpsLive_Activity::run, logic_select<GpsLive_Activity>(math_compare<"<">(s, 11.0), GpsLive_Activity::bike, GpsLive_Activity::car))))};
  }

  struct GpsLive_activity_labelResult { Text Result = ""; };
  KERN_PURE GpsLive_activity_labelResult GpsLive_activity_label(const GpsLive& self) {
    switch (flow_switch_enum<GpsLive_Activity>(GpsLive_activity(self).Result)) {
      case GpsLive_Activity::stand: {
        return {"STEHT"};
      } break;
      case GpsLive_Activity::walk: {
        return {"GEHEN"};
      } break;
      case GpsLive_Activity::run: {
        return {"LAUFEN"};
      } break;
      case GpsLive_Activity::bike: {
        return {"RAD"};
      } break;
      case GpsLive_Activity::car: {
        return {"FAHRT"};
      } break;
    }
  }
};
