struct touch_play : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(TouchBtn) { Text id = ""; Text label = ""; f32 x = 0.0f; f32 y = 0.0f; f32 w = 0.0f; f32 h = 0.0f; bool the_round = true; bool pressed = false; bool on = false; bool on_down = true; u32 color = 856344; };
  KERN_STRUCT(TouchFire) { i32 id = 0; f32 x = 0.0f; f32 y = 0.0f; f32 down_t = 0.0f; f32 last_shot = -1.0f; bool fresh = true; i32 hand = 0; f32 depth = -1.0f; bool lifted = false; };
  KERN_ENUM(TouchMode, off, foot, drone, car, jet, chute);
  KERN_STRUCT(TouchPlay_DomRect) { Text id = ""; f32 x = 0.0f; f32 y = 0.0f; f32 w = 0.0f; f32 h = 0.0f; };
  KERN_ENUM(TouchPlay_Finger_Kind, move, look, fire, press, clearance, pinch, strike, none);
  KERN_STRUCT(TouchLayout) { TouchMode mode = TouchMode::off; bool alive = true; Text weapon = ""; bool sniper = false; bool scope_on = false; bool grenade = false; i32 grenades = 0; f32 grenade_left = -1.0f; bool near_car = false; bool near_jet = false; bool emp = false; bool strike_ready = true; bool aiming = false; bool armed = false; Text camera = "FPV"; bool cockpit = false; bool hover = false; bool rocket = false; bool ground = false; bool drone_cooldown = false; bool pad_drone = false; };
  KERN_STRUCT(TouchPlay_Finger) { i32 id = 0; TouchPlay_Finger_Kind kind = TouchPlay_Finger_Kind::none; Text button = ""; f32 x0 = 0.0f; f32 y0 = 0.0f; f32 x = 0.0f; f32 y = 0.0f; f32 last_x = 0.0f; f32 last_y = 0.0f; f64 t0 = 0.0; f64 last_t = 0.0; Vector base = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(TouchPlay) { f32 w_ = 960.0f; f32 h_ = 540.0f; f32 sl_ = 0.0f; f32 st_ = 0.0f; f32 sr_ = 0.0f; f32 sb_ = 0.0f; TouchLayout lay_ = TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0f, false, false, false, true, false, false, "FPV", false, false, false, false, false, false}; List<TouchBtn> buttons_ = List<TouchBtn>{}; List<TouchPlay_Finger> fingers_ = List<TouchPlay_Finger>{}; List<Text> pressed_ = List<Text>{}; List<Text> released_ = List<Text>{}; List<TouchFire> fires_ = List<TouchFire>{}; i32 move_id_ = -1; i32 look_id_ = -1; Vector move_ = Vector{0.0, 0.0, 0.0}; Vector look_ = Vector{0.0, 0.0, 0.0}; Vector look_edge_ = Vector{0.0, 0.0, 0.0}; Vector drag_ = Vector{0.0, 0.0, 0.0}; f32 clearance_ = 0.0f; f32 nade_up_ = 0.0f; f32 zoom_ = 1.0f; f32 pinch_d_ = 0.0f; bool strike_tap_ = false; List<TouchPlay_DomRect> dom_ = List<TouchPlay_DomRect>{}; f32 strike_x_ = 0.0f; f32 strike_y_ = 0.0f; f32 pad_x_ = 0.0f; f32 pad_y_ = 0.0f; f32 pad_w_ = 0.0f; f32 pad_h_ = 0.0f; };

  // ---- variables ----
  /// Kern: the right stick off the edge (room for the thumb to turn, as CoD Mobile), in every mode
  KERN_CONST f32 kRightInset = 76.0f;
  /// thumb_layout.mjs STICK
  KERN_CONST f32 kStick = 168.0f;
  /// control_semantics.mjs normalizedPointer
  KERN_CONST f32 kRadiusK = 0.42f;
  /// foot_touch_controller.mjs edge turn
  KERN_CONST f32 kEdgeStart = 0.92f;
  /// foot_touch_controller.mjs edge turn
  KERN_CONST f32 kEdgeRampPx = 42.0f;
  /// jet_mode.mjs: EJECT after 650 ms
  KERN_CONST f32 kEjectHold = 0.65f;

  // ---- functions ----
  struct TouchPlay_right_insetResult { f32 Result = 0.0f; };
  KERN_PURE TouchPlay_right_insetResult TouchPlay_right_inset() {
    return {kRightInset};
  }

  struct TouchPlay_moveResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// ---- what the fingers give (read by the game each frame) ----
  KERN_PURE TouchPlay_moveResult TouchPlay_move(const TouchPlay& self) {
    return {self.move_};
  }

  struct TouchPlay_lookResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// right stick (vehicles, drone), same axes
  KERN_PURE TouchPlay_lookResult TouchPlay_look(const TouchPlay& self) {
    return {self.look_};
  }

  struct TouchPlay_move_liveResult { bool Result = false; };
  KERN_PURE TouchPlay_move_liveResult TouchPlay_move_live(const TouchPlay& self) {
    return {math_compare<">=", i32>(self.move_id_, 0)};
  }

  struct TouchPlay_look_liveResult { bool Result = false; };
  KERN_PURE TouchPlay_look_liveResult TouchPlay_look_live(const TouchPlay& self) {
    return {math_compare<">=", i32>(self.look_id_, 0)};
  }

  struct TouchPlay_look_edgeResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// On foot: the look stick's edge turn (x/m * edge, y/m * edge), 0 when inside the ring.
  KERN_PURE TouchPlay_look_edgeResult TouchPlay_look_edge(const TouchPlay& self) {
    return {self.look_edge_};
  }

  struct drop_lifted_matchResult { bool Result = false; };
  /// After the game's frame read the fingers: the taps let go in between are gone now.
  KERN_PURE drop_lifted_matchResult drop_lifted_match(const TouchFire& f) {
    return {f.lifted};
  }

  /// After the game's frame read the fingers: the taps let go in between are gone now.
  void TouchPlay_drop_lifted(TouchPlay& self) {
    i64 at = 0;
    i64 kept = 0;
    i64 end = 0;
    at = 0;
    kept = at;
    end = list_length<TouchFire>(self.fires_);
    while (loop(math_compare<"<", i64>(at, end))) {
      if (logic_not(drop_lifted_match(self.fires_[at]).Result)) {
        if (math_compare<"<", i64>(kept, at)) {
          var_take(n10, self.fires_[at]);
          self.fires_[kept] = n10.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.fires_, kept, TouchFire{0, 0.0f, 0.0f, 0.0f, -1.0f, true, 0, -1.0f, false});
  }

  struct TouchPlay_clearanceResult { f32 Result = 0.0f; };
  /// Drone altitude pad axis (-1..1, up positive), 0 when released.
  KERN_PURE TouchPlay_clearanceResult TouchPlay_clearance(const TouchPlay& self) {
    return {self.clearance_};
  }

  struct TouchPlay_grenade_dragResult { f32 Result = 0.0f; };
  /// Grenade button held: drag up in px since it went down.
  KERN_PURE TouchPlay_grenade_dragResult TouchPlay_grenade_drag(const TouchPlay& self) {
    return {self.nade_up_};
  }

  /// A click on the page's copy of a button (mouse, keyboard focus): the same press as a finger.
  void TouchPlay_inject(TouchPlay& self, const Text& id) {
    list_add(self.pressed_, id);
  }

  struct TouchPlay_anyResult { bool Result = false; };
  KERN_PURE TouchPlay_anyResult TouchPlay_any(const TouchPlay& self) {
    return {logic_not(math_compare<"==", i64>(list_length<TouchPlay_Finger>(self.fingers_), 0))};
  }

  struct TouchPlay_modeResult { TouchMode Result = TouchMode::off; };
  KERN_PURE TouchPlay_modeResult TouchPlay_mode(const TouchPlay& self) {
    return {self.lay_.mode};
  }

  struct TouchPlay_no_sticksResult { bool Result = false; };
  KERN_PURE TouchPlay_no_sticksResult TouchPlay_no_sticks(const TouchPlay& self) {
    return {logic_or(self.lay_.aiming, logic_and(enum_equal<TouchMode>(self.lay_.mode, TouchMode::drone), self.lay_.pad_drone))};
  }

  void TouchPlay_add(TouchPlay& self, const TouchBtn& b) {
    list_add(self.buttons_, b);
  }

  // ---- what nodes give and keep ----
  struct { TouchFire value = TouchFire{0, 0.0f, 0.0f, 0.0f, -1.0f, true, 0, -1.0f, false}; } n10;
};
