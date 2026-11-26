struct controls_cpp : virtual kern::Library, virtual blueprint_call, virtual controls {
  // ---- types ----
  KERN_STRUCT(controls_Cook) { I32 carry = 0; Float regen_at = 0.0; Bool cooking = false; Float at = 0.0; Float power = 0.0; I32 source = 0; };
  KERN_STRUCT(controls_Look) { Float yaw = 0.0; Float pitch = 0.0; Bool snapping = false; Float idle = 0.0; Bool owned = false; };

  // ---- functions ----
  struct Controls_DeviceInputResult { Bool desktop = false; Bool pad = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("DeviceInput") Controls_DeviceInputResult Controls_DeviceInput(controls_Devices_Event event = controls_Devices_Event::touch_down, Bool fine = false, Bool coarse = false, Bool desktop = false, Bool pad_2 = false);

  struct Controls_Smoothstep01Result { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("Smoothstep01") Controls_Smoothstep01Result Controls_Smoothstep01(Float t = 0.0);

  struct Controls_WalkPadAxisResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("WalkPadAxis") Controls_WalkPadAxisResult Controls_WalkPadAxis(Float v = 0.0);

  struct Controls_ShapeFirstPersonMoveResult { Float x = 0.0; Float y = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ShapeFirstPersonMove") Controls_ShapeFirstPersonMoveResult Controls_ShapeFirstPersonMove(Float x = 0.0, Float y = 0.0, Float fineness = 0.0, Bool invertH = false);

  struct Controls_FpsDynamicCurveResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("FpsDynamicCurve") Controls_FpsDynamicCurveResult Controls_FpsDynamicCurve(Float n = 0.0);

  struct Controls_LookCurveStrengthResult { Float strength = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookCurveStrength") Controls_LookCurveStrengthResult Controls_LookCurveStrength(Float fineness = 0.0);

  struct Controls_ShapeFpsStickResult { Float x = 0.0; Float y = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ShapeFpsStick") Controls_ShapeFpsStickResult Controls_ShapeFpsStick(Float x = 0.0, Float y = 0.0, Float inner = 0.0, Float curve = 0.0);

  struct Controls_LookAxisStepResult { Float velocity = 0.0; Float angle = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookAxisStep") Controls_LookAxisStepResult Controls_LookAxisStep(Float velocity = 0.0, Float target = 0.0, Float dt = 0.0);

  struct Controls_TouchLookDeltaResult { Float yaw = 0.0; Float pitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("TouchLookDelta") Controls_TouchLookDeltaResult Controls_TouchLookDelta(Float dx = 0.0, Float dy = 0.0, Float speed_2 = 0.0);

  struct Controls_FpsAimAssistResult { Bool active = false; Float slowdown = 0.0; Float yaw = 0.0; Float pitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("FpsAimAssist") Controls_FpsAimAssistResult Controls_FpsAimAssist(Float yawErr = 0.0, Float pitchErr = 0.0, Float dist_2 = 0.0, Float input = 0.0, Float inYaw = 0.0, Float inPitch = 0.0, Float s = 0.0);

  struct Controls_AimEllipseResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("AimEllipse") Controls_AimEllipseResult Controls_AimEllipse(Float yawErr = 0.0, Float pitchErr = 0.0);

  struct Controls_GamepadAxisResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GamepadAxis") Controls_GamepadAxisResult Controls_GamepadAxis(Float v = 0.0, Float dz = 0.0);

  struct Controls_HardCutResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("HardCut") Controls_HardCutResult Controls_HardCut(Float v = 0.0, Float dz = 0.0);

  struct Controls_HeightAxisResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("HeightAxis") Controls_HeightAxisResult Controls_HeightAxis(Float rt = 0.0, Float lt = 0.0);

  struct Controls_PhoneAxisResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("PhoneAxis") Controls_PhoneAxisResult Controls_PhoneAxis(Float v = 0.0, Float fineness = 0.0);

  struct Controls_GameStickMagnitudeResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GameStickMagnitude") Controls_GameStickMagnitudeResult Controls_GameStickMagnitude(Float a = 0.0);

  struct Controls_InverseGameStickMagnitudeResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("InverseGameStickMagnitude") Controls_InverseGameStickMagnitudeResult Controls_InverseGameStickMagnitude(Float v = 0.0);

  struct Controls_GameStickLeftResult { Float roll = 0.0; Float pitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GameStickLeft") Controls_GameStickLeftResult Controls_GameStickLeft(Float x = 0.0, Float y = 0.0, Float fineness = 0.0, Bool invertH = false, Float maxKmh = 0.0);

  struct Controls_GameStickRightResult { Float yaw = 0.0; Float bodyPitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GameStickRight") Controls_GameStickRightResult Controls_GameStickRight(Float x = 0.0, Float y = 0.0, Float fineness = 0.0, Bool invertH = false, Bool invertV = false);

  struct Controls_ClearancePadAxisResult { Float value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ClearancePadAxis") Controls_ClearancePadAxisResult Controls_ClearancePadAxis(Float centreY = 0.0, Float y = 0.0, Float height = 0.0);

  struct Controls_RateStickStepResult { Float rate = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("RateStickStep") Controls_RateStickStepResult Controls_RateStickStep(Float rate = 0.0, Float px = 0.0, Float dt = 0.0, Float fullPxS = 0.0, Float tau = 0.0);

  struct Controls_LookAddResult { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookAdd") Controls_LookAddResult Controls_LookAdd(controls_Look look_2 = controls_Look{0.0, 0.0, false, 0.0, false}, Float dx = 0.0, Float dy = 0.0, Float degPerPxX = 0.0, Float degPerPxY = 0.0);

  struct Controls_LookSetResult { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookSet") Controls_LookSetResult Controls_LookSet(Float yaw = 0.0, Float pitch = 0.0);

  struct Controls_LookReleaseResult { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookRelease") Controls_LookReleaseResult Controls_LookRelease(controls_Look look_2 = controls_Look{0.0, 0.0, false, 0.0, false});

  struct Controls_LookStepResult { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookStep") Controls_LookStepResult Controls_LookStep(controls_Look look_2 = controls_Look{0.0, 0.0, false, 0.0, false}, Float dt = 0.0, Bool keep = false);

  struct Controls_CarHeadStepResult { Float yaw = 0.0; Float pitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("CarHeadStep") Controls_CarHeadStepResult Controls_CarHeadStep(Float yaw = 0.0, Float pitch = 0.0, Float x = 0.0, Float y = 0.0, Float dt = 0.0);

  struct Controls_ViewButtonStepResult { Int out = 0; Bool was = false; Bool heldMap = false; Float downAt = 0.0; Float mapAt = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ViewButtonStep") Controls_ViewButtonStepResult Controls_ViewButtonStep(Bool was = false, Bool heldMap = false, Float downAt = 0.0, Float mapAt = 0.0, Bool down = false, Float now = 0.0);

  struct Controls_VoiceChordStepResult { Bool mic = false; Bool mute = false; Bool suppressWeapon = false; Bool r3 = false; Bool y = false; Bool latched = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("VoiceChordStep") Controls_VoiceChordStepResult Controls_VoiceChordStep(Bool r3 = false, Bool y = false, Bool latched = false, Bool r3Now = false, Bool yNow = false, Bool allowed = false);

  struct Controls_ReleaseLatchStepResult { Bool neutral = false; Bool armed = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ReleaseLatchStep") Controls_ReleaseLatchStepResult Controls_ReleaseLatchStep(Bool armed = false, List<Float> values = List<Float>{});

  struct Controls_GrenadePullResult { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; Bool pulled = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadePull") Controls_GrenadePullResult Controls_GrenadePull(controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}, Float now = 0.0, Int source = 0);

  struct Controls_GrenadeDragResult { Float power = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeDrag") Controls_GrenadeDragResult Controls_GrenadeDrag(Float upPx = 0.0);

  struct Controls_GrenadeReleaseResult { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; Float left = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeRelease") Controls_GrenadeReleaseResult Controls_GrenadeRelease(controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}, Float now = 0.0);

  struct Controls_GrenadeStepResult { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; Bool cookedOff = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeStep") Controls_GrenadeStepResult Controls_GrenadeStep(controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}, Float now = 0.0);

  struct Controls_GrenadeLeftResult { Float seconds = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeLeft") Controls_GrenadeLeftResult Controls_GrenadeLeft(controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}, Float now = 0.0);

  struct Controls_GrenadeSpeedResult { Float mps = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeSpeed") Controls_GrenadeSpeedResult Controls_GrenadeSpeed(Float power = 0.0);

  struct Controls_DevicePlatformResult { Bool desktop = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("DevicePlatform") Controls_DevicePlatformResult Controls_DevicePlatform(Bool fine = false, Int touchPoints = 0);

  struct Controls_HintLineResult { Text line = ""; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("HintLine") Controls_HintLineResult Controls_HintLine(Text mode = "", Text weapon = "", Bool dead = false);

  void Devices_input(controls_Devices& self, controls_Devices_Event e = controls_Devices_Event::touch_down) {
    Bool now_desktop = false;
    Bool now_pad = false;
    now_desktop = false;
    now_pad = false;
    func_call_on<"Controls_DeviceInput">(actor_find("Controls"), kern::out(n10.desktop, n10.pad), e, self.fine, self.coarse, self.desktop, self.pad);
    now_desktop = n10.desktop;
    now_pad = n10.pad;
    controls::Devices_set(self, now_desktop);
    self.pad = now_pad;
  }

  void Devices_touch_down(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::touch_down);
  }

  void Devices_mouse_down(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::mouse_down);
  }

  /// The mouse moved by itself (not locked, not over the UI).
  void Devices_mouse_move(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::mouse_move);
  }

  /// A real key (any device, phones too).
  void Devices_key(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::key);
  }

  void Devices_pad_button(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::pad_button);
  }

  struct look_ofResult { controls_Look Result = controls_Look{0.0, 0.0, false, 0.0, false}; };
  KERN_PURE look_ofResult look_of(const controls_WorldLook& l) {
    return {struct_make<controls_Look>(blueprint_call::decimal(l.yaw_deg).Result, blueprint_call::decimal(l.pitch_deg).Result, l.snapping, blueprint_call::decimal(l.idle).Result, l.owned)};
  }

  void take(const controls_Look& v, controls_WorldLook& l) {
    l.yaw_deg = v.yaw;
    l.pitch_deg = v.pitch;
    l.snapping = v.snapping;
    l.idle = v.idle;
    l.owned = v.owned;
  }

  struct cook_ofResult { controls_Cook Result = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; };
  KERN_PURE cook_ofResult cook_of(const controls_GrenadeCook& g) {
    return {struct_make<controls_Cook>(g.carry, blueprint_call::decimal(g.regen_at).Result, g.cooking, blueprint_call::decimal(g.at).Result, blueprint_call::decimal(g.power).Result, g.source)};
  }

  void take_2(const controls_Cook& c, controls_GrenadeCook& g) {
    g.carry = c.carry;
    g.regen_at = c.regen_at;
    g.cooking = c.cooking;
    g.at = c.at;
    g.power = c.power;
    g.source = c.source;
  }

  struct bindResult { Bool Result = false; };
  bindResult bind() {
    return {func_has_blueprint(actor_find("Controls"))};
  }

  struct smoothstep01Result { Float Result = 0.0; };
  smoothstep01Result smoothstep01(Float t = 0.0) {
    func_call_on<"Controls_Smoothstep01">(actor_find("Controls"), kern::out(n11.value), blueprint_call::decimal(t).Result);
    return {n11.value};
  }

  struct walk_pad_axisResult { Float Result = 0.0; };
  walk_pad_axisResult walk_pad_axis(Float v = 0.0) {
    func_call_on<"Controls_WalkPadAxis">(actor_find("Controls"), kern::out(n12.value), blueprint_call::decimal(v).Result);
    return {n12.value};
  }

  struct shape_first_person_moveResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  shape_first_person_moveResult shape_first_person_move(Float x = 0.0, Float y = 0.0, Float move_fineness = 0.0, Bool invert_h = false) {
    Float rx = 0.0;
    Float ry = 0.0;
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_ShapeFirstPersonMove">(actor_find("Controls"), kern::out(n13.x, n13.y), blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(move_fineness).Result, invert_h);
    rx = n13.x;
    ry = n13.y;
    return {vector_make(rx, ry, 0.0)};
  }

  struct fps_dynamic_curveResult { Float Result = 0.0; };
  fps_dynamic_curveResult fps_dynamic_curve(Float n = 0.0) {
    func_call_on<"Controls_FpsDynamicCurve">(actor_find("Controls"), kern::out(n14.value), blueprint_call::decimal(n).Result);
    return {n14.value};
  }

  struct look_curve_strengthResult { Float Result = 0.0; };
  look_curve_strengthResult look_curve_strength(Float look_fineness = 0.0) {
    func_call_on<"Controls_LookCurveStrength">(actor_find("Controls"), kern::out(n15.strength), blueprint_call::decimal(look_fineness).Result);
    return {n15.strength};
  }

  struct shape_fps_stickResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  shape_fps_stickResult shape_fps_stick(Float x = 0.0, Float y = 0.0, Float inner = 0.0, Float curve_strength = 0.0) {
    Float rx = 0.0;
    Float ry = 0.0;
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_ShapeFpsStick">(actor_find("Controls"), kern::out(n16.x, n16.y), blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(inner).Result, blueprint_call::decimal(curve_strength).Result);
    rx = n16.x;
    ry = n16.y;
    return {vector_make(rx, ry, 0.0)};
  }

  struct LookAxis_stepResult { Float Result = 0.0; };
  LookAxis_stepResult LookAxis_step(controls_LookAxis& self, Float target = 0.0, Float dt = 0.0) {
    Float rx = 0.0;
    Float ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_LookAxisStep">(actor_find("Controls"), kern::out(n17.velocity, n17.angle), blueprint_call::decimal(self.velocity).Result, blueprint_call::decimal(target).Result, blueprint_call::decimal(dt).Result);
    rx = n17.velocity;
    ry = n17.angle;
    r = vector_make(rx, ry, 0.0);
    self.velocity = vector_break(r).x;
    return {vector_break(r).y};
  }

  void touch_look_delta(Float& yaw, Float& pitch, Float dx = 0.0, Float dy = 0.0, Float speed_px_s = 0.0) {
    Float rx = 0.0;
    Float ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_TouchLookDelta">(actor_find("Controls"), kern::out(n18.yaw, n18.pitch), blueprint_call::decimal(dx).Result, blueprint_call::decimal(dy).Result, blueprint_call::decimal(speed_px_s).Result);
    rx = n18.yaw;
    ry = n18.pitch;
    r = vector_make(rx, ry, 0.0);
    yaw = vector_break(r).x;
    pitch = vector_break(r).y;
  }

  struct fps_aim_assistResult { controls_AimAssist Result = controls_AimAssist{false, 1.0, 0.0, 0.0}; };
  fps_aim_assistResult fps_aim_assist(Float yaw = 0.0, Float pitch = 0.0, Float dist = 0.0, Float input = 0.0, Float in_yaw = 0.0, Float in_pitch = 0.0, Float s = 0.0) {
    controls_AimAssist a = controls_AimAssist{false, 1.0, 0.0, 0.0};
    Float slowdown = 0.0;
    Float turn_yaw = 0.0;
    Float turn_pitch = 0.0;
    a = controls_AimAssist{false, 1.0, 0.0, 0.0};
    slowdown = 0.0;
    turn_yaw = 0.0;
    turn_pitch = 0.0;
    func_call_on<"Controls_FpsAimAssist">(actor_find("Controls"), kern::out(n19.active, n19.slowdown, n19.yaw, n19.pitch), blueprint_call::decimal(yaw).Result, blueprint_call::decimal(pitch).Result, blueprint_call::decimal(dist).Result, blueprint_call::decimal(input).Result, blueprint_call::decimal(in_yaw).Result, blueprint_call::decimal(in_pitch).Result, blueprint_call::decimal(s).Result);
    a.active = n19.active;
    slowdown = n19.slowdown;
    turn_yaw = n19.yaw;
    turn_pitch = n19.pitch;
    a.slowdown = slowdown;
    a.yaw = turn_yaw;
    a.pitch = turn_pitch;
    return {a};
  }

  struct aim_ellipseResult { Float Result = 0.0; };
  aim_ellipseResult aim_ellipse(Float yaw_err = 0.0, Float pitch_err = 0.0) {
    func_call_on<"Controls_AimEllipse">(actor_find("Controls"), kern::out(n20.value), blueprint_call::decimal(yaw_err).Result, blueprint_call::decimal(pitch_err).Result);
    return {n20.value};
  }

  struct gamepad_axisResult { Float Result = 0.0; };
  gamepad_axisResult gamepad_axis(Float v = 0.0, Float dz = 0.0) {
    func_call_on<"Controls_GamepadAxis">(actor_find("Controls"), kern::out(n21.value), blueprint_call::decimal(v).Result, blueprint_call::decimal(dz).Result);
    return {n21.value};
  }

  struct hard_cutResult { Float Result = 0.0; };
  hard_cutResult hard_cut(Float v = 0.0, Float dz = 0.0) {
    func_call_on<"Controls_HardCut">(actor_find("Controls"), kern::out(n22.value), blueprint_call::decimal(v).Result, blueprint_call::decimal(dz).Result);
    return {n22.value};
  }

  struct height_axisResult { Float Result = 0.0; };
  height_axisResult height_axis(Float rt = 0.0, Float lt = 0.0) {
    func_call_on<"Controls_HeightAxis">(actor_find("Controls"), kern::out(n23.value), blueprint_call::decimal(rt).Result, blueprint_call::decimal(lt).Result);
    return {n23.value};
  }

  struct phone_axisResult { Float Result = 0.0; };
  phone_axisResult phone_axis(Float v = 0.0, Float fineness = 0.0) {
    func_call_on<"Controls_PhoneAxis">(actor_find("Controls"), kern::out(n24.value), blueprint_call::decimal(v).Result, blueprint_call::decimal(fineness).Result);
    return {n24.value};
  }

  struct game_state_stick_magnitudeResult { Float Result = 0.0; };
  game_state_stick_magnitudeResult game_state_stick_magnitude(Float a = 0.0) {
    func_call_on<"Controls_GameStickMagnitude">(actor_find("Controls"), kern::out(n25.value), blueprint_call::decimal(a).Result);
    return {n25.value};
  }

  struct inverse_game_state_stick_magnitudeResult { Float Result = 0.0; };
  inverse_game_state_stick_magnitudeResult inverse_game_state_stick_magnitude(Float v = 0.0) {
    func_call_on<"Controls_InverseGameStickMagnitude">(actor_find("Controls"), kern::out(n26.value), blueprint_call::decimal(v).Result);
    return {n26.value};
  }

  void apply_game_stick_left(controls_DroneAxes& out, Float x = 0.0, Float y = 0.0, Float fineness = 0.0, Bool invert_h = false, Float max_kmh = 0.0) {
    Float rx = 0.0;
    Float ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_GameStickLeft">(actor_find("Controls"), kern::out(n27.roll, n27.pitch), blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(fineness).Result, invert_h, blueprint_call::decimal(max_kmh).Result);
    rx = n27.roll;
    ry = n27.pitch;
    r = vector_make(rx, ry, 0.0);
    out.roll = vector_break(r).x;
    out.pitch = vector_break(r).y;
  }

  void apply_game_stick_right(controls_DroneAxes& out, Float x = 0.0, Float y = 0.0, Float fineness = 0.0, Bool invert_h = false, Bool invert_v = false) {
    Float rx = 0.0;
    Float ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_GameStickRight">(actor_find("Controls"), kern::out(n28.yaw, n28.bodyPitch), blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(fineness).Result, invert_h, invert_v);
    rx = n28.yaw;
    ry = n28.bodyPitch;
    r = vector_make(rx, ry, 0.0);
    out.yaw = vector_break(r).x;
    out.body_pitch = vector_break(r).y;
  }

  struct clearance_pad_axisResult { Float Result = 0.0; };
  clearance_pad_axisResult clearance_pad_axis(Float centre_y = 0.0, Float y = 0.0, Float height = 0.0) {
    func_call_on<"Controls_ClearancePadAxis">(actor_find("Controls"), kern::out(n29.value), blueprint_call::decimal(centre_y).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(height).Result);
    return {n29.value};
  }

  void RateStick_step(controls_RateStick& self, Vector px = Vector{0.0, 0.0, 0.0}, Float dt = 0.0) {
    func_call_on<"Controls_RateStickStep">(actor_find("Controls"), kern::out(n30.rate), blueprint_call::decimal(vector_break(self.rate).x).Result, blueprint_call::decimal(vector_break(px).x).Result, blueprint_call::decimal(dt).Result, blueprint_call::decimal(self.full_px_s).Result, blueprint_call::decimal(self.tau).Result);
    self.rate = vector_make(n30.rate, vector_break(self.rate).y, vector_break(self.rate).z);
    func_call_on<"Controls_RateStickStep">(actor_find("Controls"), kern::out(n31.rate), blueprint_call::decimal(vector_break(self.rate).y).Result, blueprint_call::decimal(vector_break(px).y).Result, blueprint_call::decimal(dt).Result, blueprint_call::decimal(self.full_px_s).Result, blueprint_call::decimal(self.tau).Result);
    self.rate = vector_make(vector_break(self.rate).x, n31.rate, vector_break(self.rate).z);
  }

  void WorldLook_add(controls_WorldLook& self, Float dx = 0.0, Float dy = 0.0, Float kx = 0.0, Float ky = 0.0) {
    controls_Look l = controls_Look{0.0, 0.0, false, 0.0, false};
    l = controls_Look{0.0, 0.0, false, 0.0, false};
    func_call_on<"Controls_LookAdd">(actor_find("Controls"), kern::out(n32.look), look_of(self).Result, blueprint_call::decimal(dx).Result, blueprint_call::decimal(dy).Result, blueprint_call::decimal(kx).Result, blueprint_call::decimal(ky).Result);
    l = n32.look;
    take(l, self);
  }

  void WorldLook_set(controls_WorldLook& self, Float yaw = 0.0, Float pitch = 0.0) {
    controls_Look l = controls_Look{0.0, 0.0, false, 0.0, false};
    l = controls_Look{0.0, 0.0, false, 0.0, false};
    func_call_on<"Controls_LookSet">(actor_find("Controls"), kern::out(n33.look), blueprint_call::decimal(yaw).Result, blueprint_call::decimal(pitch).Result);
    l = n33.look;
    take(l, self);
  }

  void WorldLook_release(controls_WorldLook& self) {
    controls_Look l = controls_Look{0.0, 0.0, false, 0.0, false};
    l = controls_Look{0.0, 0.0, false, 0.0, false};
    func_call_on<"Controls_LookRelease">(actor_find("Controls"), kern::out(n34.look), look_of(self).Result);
    l = n34.look;
    take(l, self);
  }

  void WorldLook_step(controls_WorldLook& self, Float dt = 0.0, Bool keep = false) {
    controls_Look l = controls_Look{0.0, 0.0, false, 0.0, false};
    l = controls_Look{0.0, 0.0, false, 0.0, false};
    func_call_on<"Controls_LookStep">(actor_find("Controls"), kern::out(n35.look), look_of(self).Result, blueprint_call::decimal(dt).Result, keep);
    l = n35.look;
    take(l, self);
  }

  void CarHeadSpring_step(controls_CarHeadSpring& self, Float x = 0.0, Float y = 0.0, Float dt = 0.0) {
    Float rx = 0.0;
    Float ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_CarHeadStep">(actor_find("Controls"), kern::out(n36.yaw, n36.pitch), blueprint_call::decimal(self.yaw).Result, blueprint_call::decimal(self.pitch).Result, blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(dt).Result);
    rx = n36.yaw;
    ry = n36.pitch;
    r = vector_make(rx, ry, 0.0);
    self.yaw = vector_break(r).x;
    self.pitch = vector_break(r).y;
  }

  struct ViewButton_stepResult { I32 Result = 0; };
  ViewButton_stepResult ViewButton_step(controls_ViewButton& self, Bool down = false, Float now = 0.0) {
    I32 out = 0;
    Bool now_was = false;
    Bool now_held_map = false;
    Float now_down_at = 0.0;
    Float now_map_at = 0.0;
    out = 0;
    now_was = false;
    now_held_map = false;
    now_down_at = 0.0;
    now_map_at = 0.0;
    func_call_on<"Controls_ViewButtonStep">(actor_find("Controls"), kern::out(n37.out, n37.was, n37.heldMap, n37.downAt, n37.mapAt), self.was, self.held_map, blueprint_call::decimal(self.down_at).Result, blueprint_call::decimal(self.map_at).Result, down, blueprint_call::decimal(now).Result);
    out = n37.out;
    now_was = n37.was;
    now_held_map = n37.heldMap;
    now_down_at = n37.downAt;
    now_map_at = n37.mapAt;
    self.was = now_was;
    self.held_map = now_held_map;
    self.down_at = now_down_at;
    self.map_at = now_map_at;
    return {out};
  }

  struct VoiceChord_stepResult { controls_VoiceChord_Out Result = controls_VoiceChord_Out{false, false, false}; };
  VoiceChord_stepResult VoiceChord_step(controls_VoiceChord& self, Bool r3_now = false, Bool y_now = false, Bool allowed = false) {
    controls_VoiceChord_Out o = controls_VoiceChord_Out{false, false, false};
    o = controls_VoiceChord_Out{false, false, false};
    func_call_on<"Controls_VoiceChordStep">(actor_find("Controls"), kern::out(n38.mic, n38.mute, n38.suppressWeapon, n38.r3, n38.y, n38.latched), self.r3, self.y, self.latched, r3_now, y_now, allowed);
    o.mic = n38.mic;
    o.mute = n38.mute;
    o.suppress_weapon = n38.suppressWeapon;
    self.r3 = n38.r3;
    self.y = n38.y;
    self.latched = n38.latched;
    return {o};
  }

  struct ReleaseLatch_stepResult { Bool Result = false; };
  ReleaseLatch_stepResult ReleaseLatch_step(controls_ReleaseLatch& self, const Fixed<Float, 9>& values) {
    List<Float> list = List<Float>{};
    Float v = 0.0;
    Bool neutral = false;
    list = List<Float>{};
    for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(values))))); it39.next();) {
      v = values[n39.index];
      list_add(list, v);
    }
    neutral = false;
    func_call_on<"Controls_ReleaseLatchStep">(actor_find("Controls"), kern::out(n40.neutral, n40.armed), self.armed, list);
    neutral = n40.neutral;
    self.armed = n40.armed;
    return {neutral};
  }

  struct GrenadeCook_pullResult { Bool Result = false; };
  GrenadeCook_pullResult GrenadeCook_pull(controls_GrenadeCook& self, Float now = 0.0, I32 from = 0) {
    controls_Cook c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    Bool pulled = false;
    c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    pulled = false;
    func_call_on<"Controls_GrenadePull">(actor_find("Controls"), kern::out(n41.cook, n41.pulled), cook_of(self).Result, blueprint_call::decimal(now).Result, from);
    c = n41.cook;
    pulled = n41.pulled;
    take_2(c, self);
    return {pulled};
  }

  void GrenadeCook_drag(controls_GrenadeCook& self, Float up_px = 0.0) {
    func_call_on<"Controls_GrenadeDrag">(actor_find("Controls"), kern::out(n42.power), blueprint_call::decimal(up_px).Result);
    self.power = n42.power;
  }

  struct GrenadeCook_releaseResult { Float Result = 0.0; };
  GrenadeCook_releaseResult GrenadeCook_release(controls_GrenadeCook& self, Float now = 0.0) {
    controls_Cook c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    Float left = 0.0;
    c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    left = 0.0;
    func_call_on<"Controls_GrenadeRelease">(actor_find("Controls"), kern::out(n43.cook, n43.left), cook_of(self).Result, blueprint_call::decimal(now).Result);
    c = n43.cook;
    left = n43.left;
    take_2(c, self);
    return {left};
  }

  struct GrenadeCook_stepResult { Bool Result = false; };
  GrenadeCook_stepResult GrenadeCook_step(controls_GrenadeCook& self, Float now = 0.0) {
    controls_Cook c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    Bool cooked_off = false;
    c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    cooked_off = false;
    func_call_on<"Controls_GrenadeStep">(actor_find("Controls"), kern::out(n44.cook, n44.cookedOff), cook_of(self).Result, blueprint_call::decimal(now).Result);
    c = n44.cook;
    cooked_off = n44.cookedOff;
    take_2(c, self);
    return {cooked_off};
  }

  struct GrenadeCook_leftResult { Float Result = 0.0; };
  GrenadeCook_leftResult GrenadeCook_left(const controls_GrenadeCook& self, Float now = 0.0) {
    func_call_on<"Controls_GrenadeLeft">(actor_find("Controls"), kern::out(n45.seconds), cook_of(self).Result, blueprint_call::decimal(now).Result);
    return {n45.seconds};
  }

  struct GrenadeCook_speedResult { Float Result = 0.0; };
  GrenadeCook_speedResult GrenadeCook_speed(const controls_GrenadeCook& self) {
    func_call_on<"Controls_GrenadeSpeed">(actor_find("Controls"), kern::out(n46.mps), blueprint_call::decimal(self.power).Result);
    return {n46.mps};
  }

  struct grenade_fuseResult { Float Result = 0.0; };
  KERN_PURE grenade_fuseResult grenade_fuse() {
    return {func_get_on<"GrenadeFuse", Float>(actor_find("Controls"))};
  }

  void Devices_platform(controls_Devices& self, Bool fine_pointer = false, Bool coarse_pointer = false, I32 max_touch_points = 0) {
    self.fine = fine_pointer;
    self.coarse = coarse_pointer;
    self.touch_points = max_touch_points;
    func_call_on<"Controls_DevicePlatform">(actor_find("Controls"), kern::out(n47.desktop), self.fine, self.touch_points);
    controls::Devices_set(self, n47.desktop);
  }

  struct hint_lineResult { Text Result = ""; };
  hint_lineResult hint_line(Text mode = "", Text weapon = "", Bool dead = false) {
    Text line = "";
    line = "";
    func_call_on<"Controls_HintLine">(actor_find("Controls"), kern::out(n48.line), mode, weapon, dead);
    line = n48.line;
    return {line};
  }

  // ---- what nodes give and keep ----
  struct { Bool desktop = false; Bool pad = false; } n10;
  struct { Float value = 0.0; } n11;
  struct { Float value = 0.0; } n12;
  struct { Float x = 0.0; Float y = 0.0; } n13;
  struct { Float value = 0.0; } n14;
  struct { Float strength = 0.0; } n15;
  struct { Float x = 0.0; Float y = 0.0; } n16;
  struct { Float velocity = 0.0; Float angle = 0.0; } n17;
  struct { Float yaw = 0.0; Float pitch = 0.0; } n18;
  struct { Bool active = false; Float slowdown = 0.0; Float yaw = 0.0; Float pitch = 0.0; } n19;
  struct { Float value = 0.0; } n20;
  struct { Float value = 0.0; } n21;
  struct { Float value = 0.0; } n22;
  struct { Float value = 0.0; } n23;
  struct { Float value = 0.0; } n24;
  struct { Float value = 0.0; } n25;
  struct { Float value = 0.0; } n26;
  struct { Float roll = 0.0; Float pitch = 0.0; } n27;
  struct { Float yaw = 0.0; Float bodyPitch = 0.0; } n28;
  struct { Float value = 0.0; } n29;
  struct { Float rate = 0.0; } n30;
  struct { Float rate = 0.0; } n31;
  struct { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; } n32;
  struct { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; } n33;
  struct { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; } n34;
  struct { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; } n35;
  struct { Float yaw = 0.0; Float pitch = 0.0; } n36;
  struct { Int out = 0; Bool was = false; Bool heldMap = false; Float downAt = 0.0; Float mapAt = 0.0; } n37;
  struct { Bool mic = false; Bool mute = false; Bool suppressWeapon = false; Bool r3 = false; Bool y = false; Bool latched = false; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  struct { Bool neutral = false; Bool armed = false; } n40;
  struct { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; Bool pulled = false; } n41;
  struct { Float power = 0.0; } n42;
  struct { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; Float left = 0.0; } n43;
  struct { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; Bool cookedOff = false; } n44;
  struct { Float seconds = 0.0; } n45;
  struct { Float mps = 0.0; } n46;
  struct { Bool desktop = false; } n47;
  struct { Text line = ""; } n48;
};
