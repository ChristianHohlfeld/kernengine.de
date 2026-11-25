struct equipment : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Flight) { U32 id = 0; Float start = 0.0; Float duration = 0.0; Float distance = 0.0; Float max_speed = 0.0; Float max_agl = 0.0; Float max_alt = 0.0; Float volts_start = 0.0; Float volts_end = 0.0; Float volts_min = 0.0; Text reason = ""; List<Vector> path = List<Vector>{}; };
  KERN_STRUCT(FlightLog_Event) { Float t = 0.0; Text stage = ""; };
  KERN_STRUCT(Jetpack) { Float fuel = 1.0; Bool thrusting = false; Bool used = false; U32 ignitions = 0; Float burst_left_ = 0.0; Float ground_since_ = -1.0; Float last_thrust_ = -99.0; };
  KERN_STRUCT(MusicSynth_Station) { Text name = ""; Text genre = ""; Float bpm = 0.0; U32 seed = 0; };
  KERN_STRUCT(MusicSynth_Voice) { Float phase = 0.0; Float freq = 0.0; Float env = 0.0; Float decay = 0.9995; Float amp = 0.0; Float phase2 = 0.0; };
  KERN_STRUCT(FlightLog) { List<Flight> flights_ = List<Flight>{}; List<FlightLog_Event> events_ = List<FlightLog_Event>{}; Flight cur_ = Flight{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{}}; Bool active_ = false; Bool have_last_ = false; Vector last_ = Vector{0.0, 0.0, 0.0}; Float last_t_ = 0.0; Float last_path_ = -1000000000.0; U32 next_id_ = 1; U32 serial_ = 0; };
  KERN_STRUCT(MusicSynth) { Float peak = 0.0; U32 rate_ = 0; I32 programme_ = -2; I32 want_ = -2; Float gain_ = 0.0; Float target_ = 0.0; Float fade_ = 1.0; Int64 sample_ = Int64{0x0}; U32 step_ = 0; U32 samples_per_step_ = 1; U32 until_step_ = 0; U32 rng_ = 1; MusicSynth_Voice kick_ = MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}; MusicSynth_Voice snare_ = MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}; MusicSynth_Voice hat_ = MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}; MusicSynth_Voice bass_ = MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}; MusicSynth_Voice lead_ = MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}; Fixed<MusicSynth_Voice, 3> pad_ = List<MusicSynth_Voice>{}; Float lp_ = 0.0; Float bass_lp_ = 0.0; Fixed<Float, 24000> echo_ = List<Float>{}; U32 echo_at_ = 0; };

  // ---- functions ----
  struct FlightLog_flyingResult { Bool Result = false; };
  KERN_PURE FlightLog_flyingResult FlightLog_flying(const FlightLog& self) {
    return {self.active_};
  }

  struct FlightLog_serialResult { U32 Result = 0; };
  /// counts finished flights and events
  KERN_PURE FlightLog_serialResult FlightLog_serial(const FlightLog& self) {
    return {self.serial_};
  }

  struct MusicSynth_programmeResult { I32 Result = 0; };
  KERN_PURE MusicSynth_programmeResult MusicSynth_programme(const MusicSynth& self) {
    return {self.programme_};
  }

  struct MusicSynth_levelResult { Float Result = 0.0; };
  KERN_PURE MusicSynth_levelResult MusicSynth_level(const MusicSynth& self) {
    return {self.gain_};
  }
};
