struct gps_live : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(GpsLive_Activity, stand, walk, run, bike, car);
  KERN_ENUM(GpsLive_View, eye, follow, map);
  KERN_STRUCT(GpsLive) { GpsLive_View view = GpsLive_View::eye; U32 rejected = 0; U32 accepted = 0; Bool active_ = false; Bool has_fix_ = false; Bool has_shown_ = false; Bool has_compass_ = false; Float started_ = 0.0; Float last_fix_ = -1000000000.0; Float x_ = 0.0; Float y_ = 0.0; Float vx_ = 0.0; Float vy_ = 0.0; Float speed_ = 0.0; Float speed_avg_ = 0.0; Float heading_ = 0.0; Float distance_ = 0.0; Float accuracy_ = 0.0; Float compass_ = 0.0; Vector shown_ = Vector{0.0, 0.0, 0.0}; };

  // ---- functions ----
  void GpsLive_stop(GpsLive& self) {
    self.active_ = false;
  }

  struct GpsLive_activeResult { Bool Result = false; };
  KERN_PURE GpsLive_activeResult GpsLive_active(const GpsLive& self) {
    return {self.active_};
  }

  /// The phone's compass (radians, clockwise from north), used while standing.
  void GpsLive_compass(GpsLive& self, Float heading = 0.0) {
    self.compass_ = heading;
    self.has_compass_ = true;
  }

  struct GpsLive_has_fixResult { Bool Result = false; };
  KERN_PURE GpsLive_has_fixResult GpsLive_has_fix(const GpsLive& self) {
    return {self.has_fix_};
  }

  struct GpsLive_shownResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE GpsLive_shownResult GpsLive_shown(const GpsLive& self) {
    return {self.shown_};
  }

  struct GpsLive_positionResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// the filtered track (trail points)
  KERN_PURE GpsLive_positionResult GpsLive_position(const GpsLive& self) {
    return {vector_make(self.x_, self.y_, 0.0)};
  }

  struct GpsLive_headingResult { Float Result = 0.0; };
  /// radians, clockwise from +y (north)
  KERN_PURE GpsLive_headingResult GpsLive_heading(const GpsLive& self) {
    return {self.heading_};
  }

  struct GpsLive_speedResult { Float Result = 0.0; };
  KERN_PURE GpsLive_speedResult GpsLive_speed(const GpsLive& self) {
    return {self.speed_avg_};
  }

  struct GpsLive_distanceResult { Float Result = 0.0; };
  KERN_PURE GpsLive_distanceResult GpsLive_distance(const GpsLive& self) {
    return {self.distance_};
  }

  struct GpsLive_accuracyResult { Float Result = 0.0; };
  KERN_PURE GpsLive_accuracyResult GpsLive_accuracy(const GpsLive& self) {
    return {self.accuracy_};
  }

  struct GpsLive_elapsedResult { Float Result = 0.0; };
  KERN_PURE GpsLive_elapsedResult GpsLive_elapsed(const GpsLive& self, Float now = 0.0) {
    return {kern::formula([](Float active_, Float now, Float started_) -> Float { return ((active_ != 0.0) ? (now - started_) : 0.0); }, kern::to<Float>(self.active_), now, self.started_)};
  }
};
