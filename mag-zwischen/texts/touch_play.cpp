struct touch_play : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(TouchBtn) { Text id = ""; Text label = ""; Float x = 0.0; Float y = 0.0; Float w = 0.0; Float h = 0.0; Bool the_round = true; Bool pressed = false; Bool on = false; Bool on_down = true; U32 color = 856344; };
  KERN_STRUCT(TouchFire) { I32 id = 0; Float x = 0.0; Float y = 0.0; Float down_t = 0.0; Float last_shot = -1.0; Bool fresh = true; I32 hand = 0; Float depth = -1.0; Bool lifted = false; };
  KERN_ENUM(TouchMode, off, foot, drone, car, jet, chute);
  KERN_STRUCT(TouchPlay_DomRect) { Text id = ""; Float x = 0.0; Float y = 0.0; Float w = 0.0; Float h = 0.0; };
  KERN_ENUM(TouchPlay_Finger_Kind, move, look, fire, press, clearance, pinch, strike, none);
  KERN_STRUCT(TouchLayout) { TouchMode mode = TouchMode::off; Bool alive = true; Text weapon = ""; Bool sniper = false; Bool scope_on = false; Bool grenade = false; I32 grenades = 0; Float grenade_left = -1.0; Bool near_car = false; Bool near_jet = false; Bool emp = false; Bool strike_ready = true; Bool aiming = false; Bool armed = false; Text camera = "FPV"; Bool cockpit = false; Bool hover = false; Bool rocket = false; Bool ground = false; Bool drone_cooldown = false; Bool pad_drone = false; };
  KERN_STRUCT(TouchPlay_Finger) { I32 id = 0; TouchPlay_Finger_Kind kind = TouchPlay_Finger_Kind::none; Text button = ""; Float x0 = 0.0; Float y0 = 0.0; Float x = 0.0; Float y = 0.0; Float last_x = 0.0; Float last_y = 0.0; Float t0 = 0.0; Float last_t = 0.0; Vector base = Vector{0.0, 0.0, 0.0}; };
  KERN_STRUCT(TouchPlay) { Float w_ = 960.0; Float h_ = 540.0; Float sl_ = 0.0; Float st_ = 0.0; Float sr_ = 0.0; Float sb_ = 0.0; TouchLayout lay_ = TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0, false, false, false, true, false, false, "FPV", false, false, false, false, false, false}; List<TouchBtn> buttons_ = List<TouchBtn>{}; List<TouchPlay_Finger> fingers_ = List<TouchPlay_Finger>{}; List<Text> pressed_ = List<Text>{}; List<Text> released_ = List<Text>{}; List<TouchFire> fires_ = List<TouchFire>{}; I32 move_id_ = -1; I32 look_id_ = -1; Vector move_ = Vector{0.0, 0.0, 0.0}; Vector look_ = Vector{0.0, 0.0, 0.0}; Vector look_edge_ = Vector{0.0, 0.0, 0.0}; Vector drag_ = Vector{0.0, 0.0, 0.0}; Float clearance_ = 0.0; Float nade_up_ = 0.0; Float zoom_ = 1.0; Float pinch_d_ = 0.0; Bool strike_tap_ = false; List<TouchPlay_DomRect> dom_ = List<TouchPlay_DomRect>{}; Float strike_x_ = 0.0; Float strike_y_ = 0.0; Float pad_x_ = 0.0; Float pad_y_ = 0.0; Float pad_w_ = 0.0; Float pad_h_ = 0.0; };

  // ---- variables ----
  /// Kern: the right stick off the edge (room for the thumb to turn, as CoD Mobile), in every mode
  KERN_CONST Float kRightInset = 76.0;
  /// thumb_layout.mjs STICK
  KERN_CONST Float kStick = 168.0;
  /// control_semantics.mjs normalizedPointer
  KERN_CONST Float kRadiusK = 0.42;
  /// foot_touch_controller.mjs edge turn
  KERN_CONST Float kEdgeStart = 0.92;
  /// foot_touch_controller.mjs edge turn
  KERN_CONST Float kEdgeRampPx = 42.0;
  /// jet_mode.mjs: EJECT after 650 ms
  KERN_CONST Float kEjectHold = 0.65;

  // ---- functions ----
  struct TouchPlay_right_insetResult { Float Result = 0.0; };
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

  struct TouchPlay_move_liveResult { Bool Result = false; };
  KERN_PURE TouchPlay_move_liveResult TouchPlay_move_live(const TouchPlay& self) {
    return {math_compare<">=">(kern::to<Float>(self.move_id_), 0.0)};
  }

  struct TouchPlay_look_liveResult { Bool Result = false; };
  KERN_PURE TouchPlay_look_liveResult TouchPlay_look_live(const TouchPlay& self) {
    return {math_compare<">=">(kern::to<Float>(self.look_id_), 0.0)};
  }

  struct TouchPlay_look_edgeResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// On foot: the look stick's edge turn (x/m * edge, y/m * edge), 0 when inside the ring.
  KERN_PURE TouchPlay_look_edgeResult TouchPlay_look_edge(const TouchPlay& self) {
    return {self.look_edge_};
  }

  struct drop_lifted_matchResult { Bool Result = false; };
  /// After the game's frame read the fingers: the taps let go in between are gone now.
  KERN_PURE drop_lifted_matchResult drop_lifted_match(const TouchFire& f) {
    return {f.lifted};
  }

  /// After the game's frame read the fingers: the taps let go in between are gone now.
  void TouchPlay_drop_lifted(TouchPlay& self) {
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    at = 0;
    kept = at;
    end = list_length<TouchFire>(self.fires_);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(drop_lifted_match(self.fires_[at]).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n10, self.fires_[at]);
          self.fires_[kept] = n10.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.fires_, kept, TouchFire{0, 0.0, 0.0, 0.0, -1.0, true, 0, -1.0, false});
  }

  struct TouchPlay_clearanceResult { Float Result = 0.0; };
  /// Drone altitude pad axis (-1..1, up positive), 0 when released.
  KERN_PURE TouchPlay_clearanceResult TouchPlay_clearance(const TouchPlay& self) {
    return {self.clearance_};
  }

  struct TouchPlay_grenade_dragResult { Float Result = 0.0; };
  /// Grenade button held: drag up in px since it went down.
  KERN_PURE TouchPlay_grenade_dragResult TouchPlay_grenade_drag(const TouchPlay& self) {
    return {self.nade_up_};
  }

  /// A click on the page's copy of a button (mouse, keyboard focus): the same press as a finger.
  void TouchPlay_inject(TouchPlay& self, const Text& id) {
    list_add(self.pressed_, id);
  }

  struct TouchPlay_anyResult { Bool Result = false; };
  KERN_PURE TouchPlay_anyResult TouchPlay_any(const TouchPlay& self) {
    return {logic_not(math_compare<"==">(kern::to<Float>(list_length<TouchPlay_Finger>(self.fingers_)), 0.0))};
  }

  struct TouchPlay_modeResult { TouchMode Result = TouchMode::off; };
  KERN_PURE TouchPlay_modeResult TouchPlay_mode(const TouchPlay& self) {
    return {self.lay_.mode};
  }

  struct TouchPlay_no_sticksResult { Bool Result = false; };
  KERN_PURE TouchPlay_no_sticksResult TouchPlay_no_sticks(const TouchPlay& self) {
    return {logic_or(self.lay_.aiming, logic_and(enum_equal<TouchMode>(self.lay_.mode, TouchMode::drone), self.lay_.pad_drone))};
  }

  void TouchPlay_add(TouchPlay& self, const TouchBtn& b) {
    list_add(self.buttons_, b);
  }

  // ---- what nodes give and keep ----
  struct { TouchFire value = TouchFire{0, 0.0, 0.0, 0.0, -1.0, true, 0, -1.0, false}; } n10;
};
