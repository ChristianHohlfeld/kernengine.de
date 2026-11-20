struct touch_play_cpp : virtual kern::Library, virtual controls_cpp, virtual touch_play {
  // ---- variables ----
  /// thumb_layout.mjs STICK
  KERN_CONST Float kStick = 168.0;
  /// edge_hud_layout.mjs SAFE / GAP
  KERN_CONST Float kSafe = 10.0;
  /// edge_hud_layout.mjs SAFE / GAP
  KERN_CONST Float kGap = 8.0;
  /// control_semantics.mjs normalizedPointer
  KERN_CONST Float kRadiusK = 0.42;
  /// foot_touch_controller.mjs edge turn
  KERN_CONST Float kEdgeStart = 0.92;
  /// foot_touch_controller.mjs edge turn
  KERN_CONST Float kEdgeRampPx = 42.0;

  // ---- functions ----
  struct escResult { Text Result = ""; };
  KERN_PURE escResult esc(const Text& s) {
    Text o = "";
    Bool skip = false;
    Int c = 0;
    o = "";
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it10.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n10.index))));
      skip = false;
      if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
        o = text_append(o, text_of_code(32), "");
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(math_compare<"==">(kern::to<Float>(c), 34.0), math_compare<"==">(kern::to<Float>(c), 92.0))) {
          o = text_append(o, text_of_code(92), "");
        }
        o = text_append(o, text_of_code(c), "");
      }
    }
    return {o};
  }

  struct btnResult { TouchBtn Result = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344}; };
  KERN_PURE btnResult btn(const Text& label, Text id = "", Float w = 0.0, Float h = 0.0, Int color = 0, Bool the_round = false, Bool on_down = false) {
    TouchBtn b = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    b = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    b.id = id;
    b.label = label;
    b.w = w;
    b.h = h;
    b.color = color;
    b.the_round = the_round;
    b.on_down = on_down;
    return {b};
  }

  void TouchPlay_place_row(TouchPlay& self, List<TouchBtn>& items, Float x0 = 0.0, Float bottom = 0.0, Float dir = 0.0, Float max_w = 0.0) {
    Float x = 0.0;
    Float row_h = 0.0;
    Float y_bottom = 0.0;
    Float used = 0.0;
    x = x0;
    row_h = 0.0;
    y_bottom = bottom;
    used = 0.0;
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(items))))); it12.next();) {
      if (logic_and(math_compare<">">(used, 0.0), math_compare<">">(kern::formula([](Float used, Float a) -> Float { return (used + a); }, used, items[n12.index].w), max_w))) {
        y_bottom = kern::formula([](Float y_bottom, Float row_h, Float kGap) -> Float { return (y_bottom - (row_h + kGap)); }, y_bottom, row_h, kGap);
        x = x0;
        used = 0.0;
        row_h = 0.0;
      }
      items[n12.index].x = kern::formula([](Float dir, Float x, Float a) -> Float { return ((dir > 0.0) ? x : (x - a)); }, dir, x, items[n12.index].w);
      items[n12.index].y = kern::formula([](Float y_bottom, Float a) -> Float { return (y_bottom - a); }, y_bottom, items[n12.index].h);
      x += kern::formula([](Float dir, Float a, Float kGap) -> Float { return (dir * (a + kGap)); }, dir, items[n12.index].w, kGap);
      used += kern::formula([](Float a, Float kGap) -> Float { return (a + kGap); }, items[n12.index].w, kGap);
      row_h = kern::formula([](Float row_h, Float a) -> Float { return kern::math::max(row_h, a); }, row_h, items[n12.index].h);
      touch_play::TouchPlay_add(self, items[n12.index]);
    }
  }

  void TouchPlay_apply_dom_rects(TouchPlay& self) {
    Bool skip = false;
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchPlay_DomRect>(self.dom_))))); it17.next();) {
      skip = false;
      if (text_equal(self.dom_[n17.index].id, "clearance")) {
        if (math_compare<">">(self.pad_w_, 0.0)) {
          self.pad_x_ = self.dom_[n17.index].x;
          self.pad_y_ = self.dom_[n17.index].y;
          self.pad_w_ = self.dom_[n17.index].w;
          self.pad_h_ = self.dom_[n17.index].h;
        }
        for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it18.next();) {
          if (text_equal(self.buttons_[n18.index].id, "clearance")) {
            self.buttons_[n18.index].x = self.dom_[n17.index].x;
            self.buttons_[n18.index].y = self.dom_[n17.index].y;
            self.buttons_[n18.index].w = self.dom_[n17.index].w;
            self.buttons_[n18.index].h = self.dom_[n17.index].h;
          }
        }
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it19.next();) {
          if (text_equal(self.buttons_[n19.index].id, self.dom_[n17.index].id)) {
            self.buttons_[n19.index].x = self.dom_[n17.index].x;
            self.buttons_[n19.index].y = self.dom_[n17.index].y;
            self.buttons_[n19.index].w = self.dom_[n17.index].w;
            self.buttons_[n19.index].h = self.dom_[n17.index].h;
          }
        }
      }
    }
  }

  void TouchPlay_layout(TouchPlay& self) {
    List<Text> down = List<Text>{};
    TouchMode m = TouchMode::off;
    Float Lx = 0.0;
    Float Ly = 0.0;
    Float Rx = 0.0;
    Float Ry = 0.0;
    Int dark = 0;
    Int amber = 0;
    Int blue = 0;
    Int red = 0;
    Int green = 0;
    Int yellow = 0;
    TouchBtn c = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a = "";
    TouchBtn mode = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_2 = "";
    List<TouchBtn> left = List<TouchBtn>{};
    Text a_3 = "";
    Text a_4 = "";
    TouchBtn g = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_5 = "";
    TouchBtn p = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_6 = "";
    List<TouchBtn> low = List<TouchBtn>{};
    Text a_7 = "";
    Text a_8 = "";
    Text a_9 = "";
    Text a_10 = "";
    Text a_11 = "";
    Text a_12 = "";
    Float x = 0.0;
    Int at = 0;
    Int count = 0;
    Bool stop = false;
    TouchBtn s = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_13 = "";
    TouchBtn cam = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_14 = "";
    TouchBtn hb = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_15 = "";
    List<TouchBtn> up = List<TouchBtn>{};
    Text a_16 = "";
    Text a_17 = "";
    Text a_18 = "";
    List<TouchBtn> row = List<TouchBtn>{};
    Text a_19 = "";
    Text a_20 = "";
    Text a_21 = "";
    Text a_22 = "";
    Float total = 0.0;
    Float bx = 0.0;
    TouchBtn ej = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_23 = "";
    Float size = 0.0;
    TouchBtn fire = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_24 = "";
    Int at_2 = 0;
    Int end = 0;
    Bool go = false;
    down = List<Text>{};
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it11.next();) {
      if (self.buttons_[n11.index].pressed) {
        list_add(down, self.buttons_[n11.index].id);
      }
    }
    list_clear(self.buttons_);
    self.pad_h_ = 0.0;
    self.pad_w_ = self.pad_h_;
    if (enum_equal<TouchMode>(self.lay_.mode, TouchMode::off)) {
      return;
    }
    m = self.lay_.mode;
    Lx = self.sl_;
    Ly = kern::formula([](Float h_, Float sb_, Float kStick) -> Float { return ((h_ - sb_) - kStick); }, self.h_, self.sb_, kStick);
    Rx = kern::formula([](Float w_, Float sr_, Float the_result, Float kStick) -> Float { return (((w_ - sr_) - the_result) - kStick); }, self.w_, self.sr_, touch_play::TouchPlay_right_inset().Result, kStick);
    Ry = kern::formula([](Float h_, Float sb_, Float kStick) -> Float { return ((h_ - sb_) - kStick); }, self.h_, self.sb_, kStick);
    dark = 856344;
    amber = 2759948;
    blue = 928318;
    red = 9118774;
    green = 3120730;
    yellow = 16763196;
    if (self.lay_.aiming) {
      a = "\342\234\225 ABBRECHEN";
      c = btn(a, "strike_cancel", 120.0, 40.0, red, false, false).Result;
      c.x = kern::formula([](Float w_, Float w) -> Float { return kern::math::div((w_ - w), 2.0); }, self.w_, c.w);
      c.y = kern::formula([](Float st_) -> Float { return (kern::math::max(8.0, st_) + 60.0); }, self.st_);
      touch_play::TouchPlay_add(self, c);
      return;
    }
    if (logic_or(enum_equal<TouchMode>(m, TouchMode::foot), enum_equal<TouchMode>(m, TouchMode::car))) {
      a_2 = logic_select<Text>(enum_equal<TouchMode>(m, TouchMode::foot), logic_select<Text>(self.lay_.drone_cooldown, "\342\207\204 DRONE \342\200\246", "\342\207\204 DRONE"), "\342\207\204 FPS");
      mode = btn(a_2, "mode", 96.0, 40.0, kern::to<Int>(kern::formula([](Float a, Float blue) -> Float { return ((a != 0.0) ? 3811344.0 : blue); }, kern::to<Float>(enum_equal<TouchMode>(m, TouchMode::foot)), kern::to<Float>(blue))), false, false).Result;
      mode.x = kern::formula([](Float lx, Float kStick) -> Float { return ((lx + kStick) + 8.0); }, Lx, kStick);
      mode.y = kern::formula([](Float h_, Float kSafe, Float h) -> Float { return ((h_ - kSafe) - h); }, self.h_, kSafe, mode.h);
      touch_play::TouchPlay_add(self, mode);
    }
    left = List<TouchBtn>{};
    if (enum_equal<TouchMode>(m, TouchMode::drone)) {
      a_3 = "\342\207\204 FPS";
      list_add(left, btn(a_3, "mode", 96.0, 40.0, blue, false, false).Result);
    }
    if (logic_or(enum_equal<TouchMode>(m, TouchMode::foot), enum_equal<TouchMode>(m, TouchMode::drone))) {
      a_4 = "STRIKE";
      list_add(left, btn(a_4, "strike", 58.0, 58.0, kern::to<Int>(kern::formula([](Float strike_ready, Float dark) -> Float { return ((strike_ready != 0.0) ? dark : 3815994.0); }, kern::to<Float>(self.lay_.strike_ready), kern::to<Float>(dark))), true, true).Result);
    }
    if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.grenade)) {
      a_5 = logic_select<Text>(math_compare<">=">(self.lay_.grenade_left, 0.0), text_append(text_append(text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float grenade_left) -> Float { return kern::math::trunc(kern::math::div(kern::math::trunc((grenade_left * 10.0)), 10.0)); }, self.lay_.grenade_left))), ".", ""), text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float grenade_left) -> Float { return kern::math::mod(kern::math::trunc((grenade_left * 10.0)), 10.0); }, self.lay_.grenade_left))), ""), text_append("\342\227\217 ", text_format<"%d">(-1, self.lay_.grenades), ""));
      g = btn(a_5, "nade", 64.0, 64.0, kern::to<Int>(kern::formula([](Float grenade_left) -> Float { return kern::math::wrap32(((grenade_left >= 0.0) ? 3808268.0 : 1844244.0)); }, self.lay_.grenade_left)), true, true).Result;
      g.on = math_compare<">=">(self.lay_.grenade_left, 0.0);
      list_add(left, g);
    }
    TouchPlay_place_row(self, left, kern::formula([](Float kSafe, Float lx) -> Float { return kern::math::max(kSafe, (lx + 10.0)); }, kSafe, Lx), kern::formula([](Float ly) -> Float { return (ly - 8.0); }, Ly), 1.0, kern::formula([](Float kStick) -> Float { return kern::math::max(kStick, 190.0); }, kStick));
    if (logic_and(enum_equal<TouchMode>(m, TouchMode::drone), logic_not(self.lay_.pad_drone))) {
      a_6 = "\342\226\262 H\303\226HE \342\226\274";
      p = btn(a_6, "clearance", 48.0, 132.0, 1054752, false, true).Result;
      p.x = kern::formula([](Float lx, Float kStick) -> Float { return ((lx + kStick) + 8.0); }, Lx, kStick);
      p.y = kern::formula([](Float h_, Float kSafe, Float h) -> Float { return ((h_ - kSafe) - h); }, self.h_, kSafe, p.h);
      self.pad_x_ = p.x;
      self.pad_y_ = p.y;
      self.pad_w_ = p.w;
      self.pad_h_ = p.h;
      touch_play::TouchPlay_add(self, p);
    }
    low = List<TouchBtn>{};
    if (enum_equal<TouchMode>(m, TouchMode::car)) {
      a_7 = "\342\206\227";
      list_add(low, btn(a_7, "enter", 54.0, 54.0, 16739162, true, false).Result);
    } else if (enum_equal<TouchMode>(m, TouchMode::drone)) {
      a_8 = text_append("\342\207\204 ", self.lay_.weapon, "");
      list_add(low, btn(a_8, "wpn", 60.0, 60.0, amber, true, true).Result);
      if (logic_not(self.lay_.pad_drone)) {
        a_9 = logic_select<Text>(self.lay_.armed, "ARMED", "ARM");
        list_add(low, btn(a_9, "arm", 104.0, 44.0, kern::to<Int>(kern::formula([](Float armed, Float green) -> Float { return ((armed != 0.0) ? green : 16102145.0); }, kern::to<Float>(self.lay_.armed), kern::to<Float>(green))), false, false).Result);
        a_10 = "STOP";
        list_add(low, btn(a_10, "stop", 94.0, 44.0, red, false, false).Result);
      }
    } else if (enum_equal<TouchMode>(m, TouchMode::foot)) {
      a_11 = text_append("\342\207\204 ", self.lay_.weapon, "");
      list_add(low, btn(a_11, "wpn", 60.0, 60.0, amber, true, true).Result);
      a_12 = "JUMP";
      list_add(low, btn(a_12, "jump", 64.0, 64.0, dark, true, true).Result);
    }
    x = kern::formula([](Float rx) -> Float { return (rx - 8.0); }, Rx);
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(low))))); it13.next();) {
      low[n13.index].x = kern::formula([](Float x, Float a) -> Float { return (x - a); }, x, low[n13.index].w);
      low[n13.index].y = kern::formula([](Float h_, Float kSafe, Float a) -> Float { return ((h_ - kSafe) - a); }, self.h_, kSafe, low[n13.index].h);
      x = kern::formula([](Float x, Float a) -> Float { return (x - (a + 8.0)); }, x, low[n13.index].w);
      touch_play::TouchPlay_add(self, low[n13.index]);
    }
    if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.sniper)) {
      count = list_length<TouchBtn>(self.buttons_);
      at = 0;
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(at), kern::to<Float>(count))))) {
        if (text_equal(self.buttons_[at].id, "wpn")) {
          a_13 = "\342\227\216 SCOPE";
          s = btn(a_13, "scope", 60.0, 60.0, kern::to<Int>(kern::formula([](Float scope_on) -> Float { return kern::math::wrap32(((scope_on != 0.0) ? 11740702.0 : 793108.0)); }, kern::to<Float>(self.lay_.scope_on))), true, true).Result;
          s.on = self.lay_.scope_on;
          s.x = self.buttons_[at].x;
          s.y = kern::formula([](Float a) -> Float { return (a - 66.0); }, self.buttons_[at].y);
          touch_play::TouchPlay_add(self, s);
          stop = true;
        }
        if (logic_not(stop)) {
          at += 1;
        }
      }
    }
    if (enum_equal<TouchMode>(m, TouchMode::car)) {
      a_14 = logic_select<Text>(self.lay_.cockpit, "\360\237\216\245 AUSSEN", "\360\237\216\245 INNEN");
      cam = btn(a_14, "cam", 104.0, 36.0, dark, false, false).Result;
      a_15 = "HANDBREMSE";
      hb = btn(a_15, "hb", 112.0, 36.0, dark, false, true).Result;
      hb.x = kern::formula([](Float w_, Float kSafe, Float w) -> Float { return ((w_ - kSafe) - w); }, self.w_, kSafe, hb.w);
      hb.y = kern::formula([](Float ry, Float h) -> Float { return ((ry - 8.0) - h); }, Ry, hb.h);
      cam.x = kern::formula([](Float w_, Float kSafe, Float w) -> Float { return ((w_ - kSafe) - w); }, self.w_, kSafe, cam.w);
      cam.y = kern::formula([](Float y, Float h) -> Float { return ((y - 8.0) - h); }, hb.y, cam.h);
      touch_play::TouchPlay_add(self, cam);
      touch_play::TouchPlay_add(self, hb);
    } else if (logic_or(enum_equal<TouchMode>(m, TouchMode::foot), enum_equal<TouchMode>(m, TouchMode::drone))) {
      up = List<TouchBtn>{};
      if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.near_car)) {
        a_16 = "\360\237\232\227";
        list_add(up, btn(a_16, "enter", 54.0, 54.0, yellow, true, false).Result);
      }
      if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.near_jet)) {
        a_17 = "\342\234\210";
        list_add(up, btn(a_17, "jet", 54.0, 54.0, 8377343, true, false).Result);
      }
      if (self.lay_.emp) {
        a_18 = "EMP";
        list_add(up, btn(a_18, "emp", 62.0, 36.0, 2649770, false, true).Result);
      }
      TouchPlay_place_row(self, up, kern::formula([](Float w_, Float kSafe) -> Float { return (w_ - kSafe); }, self.w_, kSafe), kern::formula([](Float ry) -> Float { return (ry - 8.0); }, Ry), -1.0, kern::formula([](Float kStick) -> Float { return kern::math::max(kStick, 190.0); }, kStick));
    }
    if (enum_equal<TouchMode>(m, TouchMode::jet)) {
      a_19 = logic_select<Text>(self.lay_.hover, "\342\207\204 FLUG", "\342\207\204 SCHWEBEN");
      a_20 = "RAKETE";
      a_21 = "BOMBE";
      a_22 = "CAM";
      row = list_make<TouchBtn, "4">(btn(a_19, "vtol", 96.0, 40.0, 529420, false, false).Result, btn(a_20, "rkt", 72.0, 40.0, kern::to<Int>(kern::formula([](Float rocket) -> Float { return kern::math::wrap32(((rocket != 0.0) ? 6635531.0 : 529420.0)); }, kern::to<Float>(self.lay_.rocket))), false, false).Result, btn(a_21, "bomb", 70.0, 40.0, 529420, false, false).Result, btn(a_22, "cam", 60.0, 40.0, 529420, false, false).Result);
      row[1].on = self.lay_.rocket;
      total = -8.0;
      for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(row))))); it14.next();) {
        total += kern::formula([](Float a) -> Float { return (a + 8.0); }, row[n14.index].w);
      }
      bx = kern::formula([](Float w_, Float total) -> Float { return kern::math::div((w_ - total), 2.0); }, self.w_, total);
      for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(row))))); it15.next();) {
        row[n15.index].x = bx;
        row[n15.index].y = kern::formula([](Float h_, Float sb_, Float a) -> Float { return ((h_ - kern::math::max(16.0, sb_)) - a); }, self.h_, self.sb_, row[n15.index].h);
        bx += kern::formula([](Float a) -> Float { return (a + 8.0); }, row[n15.index].w);
        touch_play::TouchPlay_add(self, row[n15.index]);
      }
      a_23 = logic_select<Text>(self.lay_.ground, "AUSSTEIGEN", "EJECT");
      ej = btn(a_23, "eject", 84.0, 38.0, 4266001, false, false).Result;
      ej.x = kern::formula([](Float sl_) -> Float { return kern::math::max(8.0, sl_); }, self.sl_);
      ej.y = kern::formula([](Float st_) -> Float { return (st_ + 55.0); }, self.st_);
      touch_play::TouchPlay_add(self, ej);
      size = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float w_) -> Float { return (w_ * 0.13); }, self.w_), 96.0, 116.0));
      a_24 = "KANONE";
      fire = btn(a_24, "fire", size, size, 8002066, true, true).Result;
      fire.x = kern::formula([](Float w_, Float sr_, Float size) -> Float { return ((w_ - ((kern::math::max(12.0, sr_) + kern::math::min((w_ * 0.25), 148.0)) + 18.0)) - size); }, self.w_, self.sr_, size);
      fire.y = kern::formula([](Float h_, Float sb_, Float size) -> Float { return ((h_ - kern::math::max(22.0, (sb_ + 18.0))) - size); }, self.h_, self.sb_, size);
      touch_play::TouchPlay_add(self, fire);
    }
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it16.next();) {
      at_2 = 0;
      end = list_length<Text>(down);
      go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end));
      while (loop(go)) {
        if (text_equal(down[at_2], self.buttons_[n16.index].id)) {
          go = false;
        } else {
          at_2 += 1;
          go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end));
        }
      }
      if (logic_not(math_compare<"==">(kern::to<Float>(at_2), kern::to<Float>(list_length<Text>(down))))) {
        self.buttons_[n16.index].pressed = true;
      }
    }
    TouchPlay_apply_dom_rects(self);
  }

  void TouchPlay_set_frame(TouchPlay& self, Float w = 0.0, Float h = 0.0, Float sl = 0.0, Float st = 0.0, Float sr = 0.0, Float sb = 0.0) {
    self.w_ = w;
    self.h_ = h;
    self.sl_ = sl;
    self.st_ = st;
    self.sr_ = sr;
    self.sb_ = sb;
    TouchPlay_layout(self);
  }

  struct release_all_matchResult { Bool Result = false; };
  KERN_PURE release_all_matchResult release_all_match(const TouchFire& f) {
    return {logic_not(f.lifted)};
  }

  void TouchPlay_release_all(TouchPlay& self) {
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchPlay_Finger>(self.fingers_))))); it20.next();) {
      if (math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[n20.index].kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::press)))) {
        list_add(self.released_, self.fingers_[n20.index].button);
      }
    }
    list_clear(self.fingers_);
    for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchFire>(self.fires_))))); it21.next();) {
      self.fires_[n21.index].lifted = logic_or(self.fires_[n21.index].lifted, self.fires_[n21.index].fresh);
    }
    at = 0;
    kept = at;
    end = list_length<TouchFire>(self.fires_);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(release_all_match(self.fires_[at]).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n22, self.fires_[at]);
          self.fires_[kept] = n22.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.fires_, kept, TouchFire{0, 0.0, 0.0, 0.0, -1.0, true, 0, -1.0, false});
    self.drag_ = Vector{0.0, 0.0, 0.0};
    self.look_edge_ = self.drag_;
    self.look_ = self.look_edge_;
    self.move_ = self.look_;
    self.look_id_ = -1;
    self.move_id_ = self.look_id_;
    self.clearance_ = 0.0;
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it23.next();) {
      self.buttons_[n23.index].pressed = false;
    }
  }

  void TouchPlay_set_layout(TouchPlay& self, const TouchLayout& l) {
    Bool mode_changed = false;
    mode_changed = logic_not(enum_equal<TouchMode>(l.mode, self.lay_.mode));
    self.lay_ = l;
    if (mode_changed) {
      TouchPlay_release_all(self);
    }
    TouchPlay_layout(self);
  }

  void TouchPlay_set_dom_rects(TouchPlay& self, List<TouchPlay_DomRect> rects = List<TouchPlay_DomRect>{}) {
    self.dom_ = rects;
    TouchPlay_layout(self);
  }

  struct TouchPlay_in_circleResult { Bool Result = false; };
  KERN_PURE TouchPlay_in_circleResult TouchPlay_in_circle(Float cx = 0.0, Float cy = 0.0, Float r = 0.0, Float x = 0.0, Float y = 0.0) {
    return {math_compare<"<=">(kern::formula([](Float x, Float cx, Float y, Float cy) -> Float { return kern::math::sqrt(((x - cx) * (x - cx)) + ((y - cy) * (y - cy))); }, x, cx, y, cy), r)};
  }

  struct TouchPlay_button_atResult { Int Result = 0; };
  KERN_PURE TouchPlay_button_atResult TouchPlay_button_at(const TouchPlay& self, Float x = 0.0, Float y = 0.0) {
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it24.next();) {
      if (logic_and(logic_and(logic_and(logic_and(text_equal(self.buttons_[n24.index].id, "emp"), math_compare<">=">(x, self.buttons_[n24.index].x)), math_compare<"<=">(x, kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, self.buttons_[n24.index].x, self.buttons_[n24.index].w))), math_compare<">=">(y, self.buttons_[n24.index].y)), math_compare<"<=">(y, kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, self.buttons_[n24.index].y, self.buttons_[n24.index].h)))) {
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n24.index)))};
      }
    }
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it25.next();) {
      if (logic_select<Bool>(self.buttons_[n25.index].the_round, TouchPlay_in_circle(kern::formula([](Float a, Float a_2) -> Float { return (a + kern::math::div(a_2, 2.0)); }, self.buttons_[n25.index].x, self.buttons_[n25.index].w), kern::formula([](Float a, Float a_2) -> Float { return (a + kern::math::div(a_2, 2.0)); }, self.buttons_[n25.index].y, self.buttons_[n25.index].h), kern::formula([](Float a) -> Float { return kern::math::div(a, 2.0); }, self.buttons_[n25.index].w), x, y).Result, logic_and(logic_and(logic_and(math_compare<">=">(x, self.buttons_[n25.index].x), math_compare<"<=">(x, kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, self.buttons_[n25.index].x, self.buttons_[n25.index].w))), math_compare<">=">(y, self.buttons_[n25.index].y)), math_compare<"<=">(y, kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, self.buttons_[n25.index].y, self.buttons_[n25.index].h))))) {
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n25.index)))};
      }
    }
    return {-1};
  }

  struct pointer_matchResult { Bool Result = false; };
  KERN_PURE pointer_matchResult pointer_match(const TouchPlay_Finger& f, Int id = 0) {
    return {math_compare<"==">(kern::to<Float>(f.id), kern::to<Float>(id))};
  }

  struct pointer_match_2Result { Bool Result = false; };
  /// The newest finger takes the stick over; deflection counts from where it lands.
  KERN_PURE pointer_match_2Result pointer_match_2(const TouchPlay_Finger& g) {
    return {math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(g.kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::move)))};
  }

  struct pointer_match_3Result { Bool Result = false; };
  KERN_PURE pointer_match_3Result pointer_match_3(const TouchPlay_Finger& g) {
    return {math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(g.kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::look)))};
  }

  struct pointer_match_4Result { Bool Result = false; };
  KERN_PURE pointer_match_4Result pointer_match_4(const TouchPlay_Finger& g) {
    return {math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(g.kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::fire)))};
  }

  struct pointer_match_5Result { Bool Result = false; };
  KERN_PURE pointer_match_5Result pointer_match_5(const TouchFire& fi, Int id = 0) {
    return {logic_and(math_compare<"==">(kern::to<Float>(fi.id), kern::to<Float>(id)), logic_not(fi.lifted))};
  }

  struct TouchPlay_pointerResult { Bool Result = false; };
  TouchPlay_pointerResult TouchPlay_pointer(TouchPlay& self, Int type = 0, Int id = 0, Float x = 0.0, Float y = 0.0, Float t_ms = 0.0) {
    Int it = 0;
    Int at = 0;
    Int end = 0;
    Bool go = false;
    Float t = 0.0;
    TouchPlay_Finger f = TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}};
    Float r = 0.0;
    Float Lcx = 0.0;
    Float Lcy = 0.0;
    Float Rcx = 0.0;
    Float Rcy = 0.0;
    Int at_2 = 0;
    Int at_3 = 0;
    Int kept = 0;
    Int end_2 = 0;
    Int at_4 = 0;
    Int kept_2 = 0;
    Int end_3 = 0;
    TouchFire fire = TouchFire{0, 0.0, 0.0, 0.0, -1.0, true, 0, -1.0, false};
    Int count = 0;
    Int at_5 = 0;
    Int end_4 = 0;
    Int count_2 = 0;
    Int at_6 = 0;
    Int end_5 = 0;
    Bool both = false;
    Int other = 0;
    Float dx = 0.0;
    Float dy = 0.0;
    Float dt_ms = 0.0;
    Float radius = 0.0;
    Int selected = 0;
    Float vx = 0.0;
    Float vy = 0.0;
    Float m = 0.0;
    Float yaw = 0.0;
    Float pitch = 0.0;
    Float cap = 0.0;
    Float ox = 0.0;
    Float oy = 0.0;
    Float dist = 0.0;
    Float pull = 0.0;
    Float edge = 0.0;
    Float m_2 = 0.0;
    Float vx_2 = 0.0;
    Float vy_2 = 0.0;
    Float m_3 = 0.0;
    Int other_2 = 0;
    Float d = 0.0;
    Int selected_2 = 0;
    Int at_7 = 0;
    Int kept_3 = 0;
    Int end_6 = 0;
    Bool still = false;
    Bool inside = false;
    if (enum_equal<TouchMode>(self.lay_.mode, TouchMode::off)) {
      return {false};
    }
    at = 0;
    end = list_length<TouchPlay_Finger>(self.fingers_);
    go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
    while (loop(go)) {
      if (pointer_match(self.fingers_[at], id).Result) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      }
    }
    it = at;
    t = kern::formula([](Float t_ms) -> Float { return kern::math::div(t_ms, 1000.0); }, t_ms);
    if (math_compare<"==">(kern::to<Float>(type), 0.0)) {
      if (logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(list_length<TouchPlay_Finger>(self.fingers_))))) {
        list_remove_at(self.fingers_, it);
      }
      f = TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}};
      f.id = id;
      f.last_x = x;
      f.x = f.last_x;
      f.x0 = f.x;
      f.last_y = y;
      f.y = f.last_y;
      f.y0 = f.y;
      f.last_t = t_ms;
      f.t0 = f.last_t;
      r = kern::formula([](Float kStick) -> Float { return kern::math::div(kStick, 2.0); }, kStick);
      Lcx = kern::formula([](Float sl_, Float r) -> Float { return (sl_ + r); }, self.sl_, r);
      Lcy = kern::formula([](Float h_, Float sb_, Float r) -> Float { return ((h_ - sb_) - r); }, self.h_, self.sb_, r);
      Rcx = kern::formula([](Float w_, Float sr_, Float the_result, Float r) -> Float { return (((w_ - sr_) - the_result) - r); }, self.w_, self.sr_, touch_play::TouchPlay_right_inset().Result, r);
      Rcy = kern::formula([](Float h_, Float sb_, Float r) -> Float { return ((h_ - sb_) - r); }, self.h_, self.sb_, r);
      at_2 = TouchPlay_button_at(self, x, y).Result;
      if (math_compare<">=">(kern::to<Float>(at_2), 0.0)) {
        f.kind = TouchPlay_Finger_Kind::press;
        f.button = self.buttons_[at_2].id;
        self.buttons_[at_2].pressed = true;
        if (self.buttons_[at_2].on_down) {
          list_add(self.pressed_, self.buttons_[at_2].id);
        }
        if (text_equal(self.buttons_[at_2].id, "clearance")) {
          f.kind = TouchPlay_Finger_Kind::clearance;
          n26 = controls_cpp::clearance_pad_axis(kern::formula([](Float pad_y_, Float pad_h_) -> Float { return (pad_y_ + kern::math::div(pad_h_, 2.0)); }, self.pad_y_, self.pad_h_), y, self.pad_h_);
          self.clearance_ = n26.Result;
        }
        if (text_equal(self.buttons_[at_2].id, "nade")) {
          self.nade_up_ = 0.0;
        }
      } else if (self.lay_.aiming) {
        f.kind = TouchPlay_Finger_Kind::strike;
      } else if (logic_and(logic_and(self.lay_.alive, logic_not(touch_play::TouchPlay_no_sticks(self).Result)), TouchPlay_in_circle(Lcx, Lcy, r, x, y).Result)) {
        at_3 = 0;
        kept = at_3;
        end_2 = list_length<TouchPlay_Finger>(self.fingers_);
        while (loop(math_compare<"<">(kern::to<Float>(at_3), kern::to<Float>(end_2)))) {
          if (logic_not(pointer_match_2(self.fingers_[at_3]).Result)) {
            if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at_3))) {
              var_take(n27, self.fingers_[at_3]);
              self.fingers_[kept] = n27.value;
            }
            kept += 1;
          }
          at_3 += 1;
        }
        list_resize(self.fingers_, kept, TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}});
        f.kind = TouchPlay_Finger_Kind::move;
        f.base = Vector{0.0, 0.0, 0.0};
        self.move_id_ = id;
        self.move_ = Vector{0.0, 0.0, 0.0};
      } else if (logic_and(logic_and(self.lay_.alive, logic_not(touch_play::TouchPlay_no_sticks(self).Result)), TouchPlay_in_circle(Rcx, Rcy, r, x, y).Result)) {
        at_4 = 0;
        kept_2 = at_4;
        end_3 = list_length<TouchPlay_Finger>(self.fingers_);
        while (loop(math_compare<"<">(kern::to<Float>(at_4), kern::to<Float>(end_3)))) {
          if (logic_not(pointer_match_3(self.fingers_[at_4]).Result)) {
            if (math_compare<"<">(kern::to<Float>(kept_2), kern::to<Float>(at_4))) {
              var_take(n28, self.fingers_[at_4]);
              self.fingers_[kept_2] = n28.value;
            }
            kept_2 += 1;
          }
          at_4 += 1;
        }
        list_resize(self.fingers_, kept_2, TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}});
        f.kind = TouchPlay_Finger_Kind::look;
        self.look_id_ = id;
        self.look_edge_ = Vector{0.0, 0.0, 0.0};
        self.look_ = self.look_edge_;
      } else if (logic_and(self.lay_.alive, logic_or(logic_or(enum_equal<TouchMode>(self.lay_.mode, TouchMode::foot), enum_equal<TouchMode>(self.lay_.mode, TouchMode::drone)), enum_equal<TouchMode>(self.lay_.mode, TouchMode::jet)))) {
        f.kind = TouchPlay_Finger_Kind::fire;
        fire = TouchFire{0, 0.0, 0.0, 0.0, -1.0, true, 0, -1.0, false};
        fire.id = id;
        fire.x = x;
        fire.y = y;
        fire.down_t = t;
        fire.hand = kern::to<Int>(kern::formula([](Float x, Float w_) -> Float { return ((x < kern::math::div(w_, 2.0)) ? 1.0 : 0.0); }, x, self.w_));
        list_add(self.fires_, fire);
        both = enum_equal<TouchMode>(self.lay_.mode, TouchMode::jet);
        if (both) {
          count_2 = 0;
          at_6 = 0;
          end_5 = list_length<TouchPlay_Finger>(self.fingers_);
          while (loop(math_compare<"<">(kern::to<Float>(at_6), kern::to<Float>(end_5)))) {
            if (pointer_match_4(self.fingers_[at_6]).Result) {
              count_2 += 1;
            }
            at_6 += 1;
          }
          both = math_compare<">">(kern::to<Float>(count_2), 0.0);
        }
        if (both) {
          list_remove_at(self.fires_, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TouchFire>(self.fires_)))));
          f.kind = TouchPlay_Finger_Kind::none;
        }
      } else if (enum_equal<TouchMode>(self.lay_.mode, TouchMode::car)) {
        f.kind = TouchPlay_Finger_Kind::pinch;
        other = list_length<TouchPlay_Finger>(self.fingers_);
        for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TouchPlay_Finger>(self.fingers_))))); it29.next();) {
          if (math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[n29.index].kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::pinch)))) {
            other = n29.index;
          }
        }
        if (math_compare<">">(kern::formula([](Float other, Float a) -> Float { return ((other < 0.0) != (a < 0.0) ? (a < 0.0) : (other < a)); }, kern::to<Float>(other), kern::to<Float>(list_length<TouchPlay_Finger>(self.fingers_))), 0.5)) {
          self.pinch_d_ = kern::formula([](Float a, Float x, Float a_2, Float y) -> Float { return kern::math::sqrt(((a - x) * (a - x)) + ((a_2 - y) * (a_2 - y))); }, self.fingers_[other].x, x, self.fingers_[other].y, y);
        }
      } else {
        f.kind = TouchPlay_Finger_Kind::none;
      }
      list_add(self.fingers_, f);
      return {logic_not(logic_or(logic_or(math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(f.kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::pinch))), math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(f.kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::none)))), logic_and(math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(f.kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::fire))), enum_equal<TouchMode>(self.lay_.mode, TouchMode::drone))))};
    }
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(list_length<TouchPlay_Finger>(self.fingers_)))) {
      return {false};
    }
    if (math_compare<"==">(kern::to<Float>(type), 1.0)) {
      dx = kern::formula([](Float x, Float a) -> Float { return (x - a); }, x, self.fingers_[it].last_x);
      dy = kern::formula([](Float y, Float a) -> Float { return (y - a); }, y, self.fingers_[it].last_y);
      dt_ms = kern::formula([](Float t_ms, Float a) -> Float { return kern::math::max(1.0, (t_ms - a)); }, t_ms, self.fingers_[it].last_t);
      self.fingers_[it].x = x;
      self.fingers_[it].y = y;
      radius = kern::formula([](Float kStick, Float kRadiusK) -> Float { return kern::math::max(1.0, (kStick * kRadiusK)); }, kStick, kRadiusK);
      selected = enum_index<TouchPlay_Finger_Kind>(self.fingers_[it].kind);
      if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
        vx = kern::formula([](Float x, Float x_2, Float a, Float radius) -> Float { return (x + kern::math::div((x_2 - a), radius)); }, vector_break(self.fingers_[it].base).x, x, self.fingers_[it].x0, radius);
        vy = kern::formula([](Float y, Float y_2, Float a, Float radius) -> Float { return (y + kern::math::div((y_2 - a), radius)); }, vector_break(self.fingers_[it].base).y, y, self.fingers_[it].y0, radius);
        m = kern::formula([](Float vx, Float vy) -> Float { return kern::math::sqrt((vx * vx) + (vy * vy)); }, vx, vy);
        if (math_compare<">">(m, 1.0)) {
          vx = kern::formula([](Float vx, Float m) -> Float { return kern::math::div(vx, m); }, vx, m);
          vy = kern::formula([](Float vy, Float m) -> Float { return kern::math::div(vy, m); }, vy, m);
        }
        self.move_ = vector_make(vx, vy, 0.0);
      } else if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
        if (enum_equal<TouchMode>(self.lay_.mode, TouchMode::foot)) {
          if (logic_or(math_compare<">">(kern::formula([](Float dx) -> Float { return (dx != 0.0); }, dx), 0.5), math_compare<">">(kern::formula([](Float dy) -> Float { return (dy != 0.0); }, dy), 0.5))) {
            yaw = 0.0;
            pitch = 0.0;
            controls_cpp::touch_look_delta(yaw, pitch, dx, dy, kern::formula([](Float dx, Float dy, Float dt_ms) -> Float { return (kern::math::div(kern::math::sqrt((dx * dx) + (dy * dy)), dt_ms) * 1000.0); }, dx, dy, dt_ms));
            self.drag_ = vector_make(kern::formula([](Float x, Float yaw) -> Float { return (x + yaw); }, vector_break(self.drag_).x, yaw), vector_break(self.drag_).y, vector_break(self.drag_).z);
            self.drag_ = vector_make(vector_break(self.drag_).x, kern::formula([](Float y, Float pitch) -> Float { return (y + pitch); }, vector_break(self.drag_).y, pitch), vector_break(self.drag_).z);
          }
          cap = kern::formula([](Float radius, Float kEdgeStart, Float kEdgeRampPx) -> Float { return ((radius * kEdgeStart) + kEdgeRampPx); }, radius, kEdgeStart, kEdgeRampPx);
          ox = kern::formula([](Float x, Float a) -> Float { return (x - a); }, x, self.fingers_[it].x0);
          oy = kern::formula([](Float y, Float a) -> Float { return (y - a); }, y, self.fingers_[it].y0);
          dist = kern::formula([](Float ox, Float oy) -> Float { return kern::math::sqrt((ox * ox) + (oy * oy)); }, ox, oy);
          if (math_compare<">">(dist, cap)) {
            pull = kern::formula([](Float dist, Float cap) -> Float { return kern::math::div((dist - cap), dist); }, dist, cap);
            self.fingers_[it].x0 = kern::formula([](Float a, Float ox, Float pull) -> Float { return (a + (ox * pull)); }, self.fingers_[it].x0, ox, pull);
            self.fingers_[it].y0 = kern::formula([](Float a, Float oy, Float pull) -> Float { return (a + (oy * pull)); }, self.fingers_[it].y0, oy, pull);
            ox = kern::formula([](Float ox, Float pull) -> Float { return (ox * (1.0 - pull)); }, ox, pull);
            oy = kern::formula([](Float oy, Float pull) -> Float { return (oy * (1.0 - pull)); }, oy, pull);
            dist = cap;
          }
          edge = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float dist, Float kEdgeStart, Float radius, Float kEdgeRampPx) -> Float { return kern::math::div((dist - (kEdgeStart * radius)), kEdgeRampPx); }, dist, kEdgeStart, radius, kEdgeRampPx), 0.0, 1.0));
          m_2 = kern::formula([](Float dist, Float radius) -> Float { return kern::math::div(dist, radius); }, dist, radius);
          self.look_ = vector_make(kern::formula([](Float m_2, Float dist, Float ox) -> Float { return (kern::math::min(1.0, m_2) * ((dist > 0.0) ? kern::math::div(ox, dist) : 0.0)); }, m_2, dist, ox), kern::formula([](Float m_2, Float dist, Float oy) -> Float { return (kern::math::min(1.0, m_2) * ((dist > 0.0) ? kern::math::div(oy, dist) : 0.0)); }, m_2, dist, oy), 0.0);
          self.look_edge_ = logic_select<Vector>(logic_and(math_compare<">">(edge, 0.0), math_compare<">">(m_2, 0.0)), vector_make(kern::formula([](Float ox, Float radius, Float m_2, Float edge) -> Float { return (kern::math::div(kern::math::div(ox, radius), m_2) * edge); }, ox, radius, m_2, edge), kern::formula([](Float oy, Float radius, Float m_2, Float edge) -> Float { return (kern::math::div(kern::math::div(oy, radius), m_2) * edge); }, oy, radius, m_2, edge), 0.0), Vector{0.0, 0.0, 0.0});
        } else {
          vx_2 = kern::formula([](Float x, Float a, Float radius) -> Float { return kern::math::div((x - a), radius); }, x, self.fingers_[it].x0, radius);
          vy_2 = kern::formula([](Float y, Float a, Float radius) -> Float { return kern::math::div((y - a), radius); }, y, self.fingers_[it].y0, radius);
          m_3 = kern::formula([](Float vx_2, Float vy_2) -> Float { return kern::math::sqrt((vx_2 * vx_2) + (vy_2 * vy_2)); }, vx_2, vy_2);
          if (math_compare<">">(m_3, 1.0)) {
            vx_2 = kern::formula([](Float vx_2, Float m_3) -> Float { return kern::math::div(vx_2, m_3); }, vx_2, m_3);
            vy_2 = kern::formula([](Float vy_2, Float m_3) -> Float { return kern::math::div(vy_2, m_3); }, vy_2, m_3);
          }
          self.look_ = vector_make(vx_2, vy_2, 0.0);
        }
      } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
        for (auto it30 = flow_for(n30, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchFire>(self.fires_))))); it30.next();) {
          if (math_compare<"==">(kern::to<Float>(self.fires_[n30.index].id), kern::to<Float>(id))) {
            self.fires_[n30.index].x = x;
            self.fires_[n30.index].y = y;
          }
        }
      } else if (math_compare<"==">(kern::to<Float>(selected), 4.0)) {
        n31 = controls_cpp::clearance_pad_axis(kern::formula([](Float pad_y_, Float pad_h_) -> Float { return (pad_y_ + kern::math::div(pad_h_, 2.0)); }, self.pad_y_, self.pad_h_), y, self.pad_h_);
        self.clearance_ = n31.Result;
      } else if (math_compare<"==">(kern::to<Float>(selected), 3.0)) {
        if (text_equal(self.fingers_[it].button, "nade")) {
          self.nade_up_ = kern::formula([](Float a, Float y) -> Float { return (a - y); }, self.fingers_[it].y0, y);
        }
      } else if (math_compare<"==">(kern::to<Float>(selected), 5.0)) {
        other_2 = list_length<TouchPlay_Finger>(self.fingers_);
        for (auto it32 = flow_for(n32, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TouchPlay_Finger>(self.fingers_))))); it32.next();) {
          if (logic_and(math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[n32.index].kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::pinch))), math_compare<"!=">(kern::to<Float>(self.fingers_[n32.index].id), kern::to<Float>(id)))) {
            other_2 = n32.index;
          }
        }
        if (math_compare<">">(kern::formula([](Float other_2, Float a) -> Float { return ((other_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (other_2 < a)); }, kern::to<Float>(other_2), kern::to<Float>(list_length<TouchPlay_Finger>(self.fingers_))), 0.5)) {
          d = kern::formula([](Float a, Float x, Float a_2, Float y) -> Float { return kern::math::sqrt(((a - x) * (a - x)) + ((a_2 - y) * (a_2 - y))); }, self.fingers_[other_2].x, x, self.fingers_[other_2].y, y);
          if (logic_and(math_compare<">">(self.pinch_d_, 10.0), math_compare<">">(d, 10.0))) {
            self.zoom_ = kern::formula([](Float zoom_, Float pinch_d_, Float d) -> Float { return (zoom_ * kern::math::div(pinch_d_, d)); }, self.zoom_, self.pinch_d_, d);
          }
          self.pinch_d_ = d;
        }
      } else {
      }
      self.fingers_[it].last_x = x;
      self.fingers_[it].last_y = y;
      self.fingers_[it].last_t = t_ms;
      return {true};
    }
    selected_2 = enum_index<TouchPlay_Finger_Kind>(self.fingers_[it].kind);
    if (math_compare<"==">(kern::to<Float>(selected_2), 0.0)) {
      self.move_ = Vector{0.0, 0.0, 0.0};
      self.move_id_ = -1;
    } else if (math_compare<"==">(kern::to<Float>(selected_2), 1.0)) {
      self.look_edge_ = Vector{0.0, 0.0, 0.0};
      self.look_ = self.look_edge_;
      self.look_id_ = -1;
    } else if (math_compare<"==">(kern::to<Float>(selected_2), 2.0)) {
      for (auto it33 = flow_for(n33, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchFire>(self.fires_))))); it33.next();) {
        if (logic_and(math_compare<"==">(kern::to<Float>(self.fires_[n33.index].id), kern::to<Float>(id)), self.fires_[n33.index].fresh)) {
          self.fires_[n33.index].lifted = true;
        }
      }
      at_7 = 0;
      kept_3 = at_7;
      end_6 = list_length<TouchFire>(self.fires_);
      while (loop(math_compare<"<">(kern::to<Float>(at_7), kern::to<Float>(end_6)))) {
        if (logic_not(pointer_match_5(self.fires_[at_7], id).Result)) {
          if (math_compare<"<">(kern::to<Float>(kept_3), kern::to<Float>(at_7))) {
            var_take(n34, self.fires_[at_7]);
            self.fires_[kept_3] = n34.value;
          }
          kept_3 += 1;
        }
        at_7 += 1;
      }
      list_resize(self.fires_, kept_3, TouchFire{0, 0.0, 0.0, 0.0, -1.0, true, 0, -1.0, false});
    } else if (math_compare<"==">(kern::to<Float>(selected_2), 4.0)) {
      self.clearance_ = 0.0;
    } else if (math_compare<"==">(kern::to<Float>(selected_2), 6.0)) {
      if (math_compare<"==">(kern::to<Float>(type), 2.0)) {
        self.strike_tap_ = true;
        self.strike_x_ = x;
        self.strike_y_ = y;
      }
    } else if (math_compare<"==">(kern::to<Float>(selected_2), 3.0)) {
      still = false;
      for (auto it35 = flow_for(n35, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchPlay_Finger>(self.fingers_))))); it35.next();) {
        if (logic_and(logic_and(math_compare<"!=">(kern::to<Float>(self.fingers_[n35.index].id), kern::to<Float>(id)), math_compare<"==">(kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[n35.index].kind)), kern::to<Float>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::press)))), text_equal(self.fingers_[n35.index].button, self.fingers_[it].button))) {
          still = true;
        }
      }
      for (auto it36 = flow_for(n36, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it36.next();) {
        if (text_equal(self.buttons_[n36.index].id, self.fingers_[it].button)) {
          if (logic_not(still)) {
            self.buttons_[n36.index].pressed = false;
          }
          inside = logic_and(logic_and(logic_and(math_compare<">=">(x, kern::formula([](Float a) -> Float { return (a - 4.0); }, self.buttons_[n36.index].x)), math_compare<"<=">(x, kern::formula([](Float a, Float a_2) -> Float { return ((a + a_2) + 4.0); }, self.buttons_[n36.index].x, self.buttons_[n36.index].w))), math_compare<">=">(y, kern::formula([](Float a) -> Float { return (a - 4.0); }, self.buttons_[n36.index].y))), math_compare<"<=">(y, kern::formula([](Float a, Float a_2) -> Float { return ((a + a_2) + 4.0); }, self.buttons_[n36.index].y, self.buttons_[n36.index].h)));
          if (logic_and(logic_and(logic_not(self.buttons_[n36.index].on_down), math_compare<"==">(kern::to<Float>(type), 2.0)), inside)) {
            list_add(self.pressed_, self.buttons_[n36.index].id);
          }
        }
      }
      list_add(self.released_, self.fingers_[it].button);
    } else {
    }
    list_remove_at(self.fingers_, it);
    return {true};
  }

  struct TouchPlay_take_dragResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  TouchPlay_take_dragResult TouchPlay_take_drag(TouchPlay& self) {
    Vector d = Vector{0.0, 0.0, 0.0};
    d = self.drag_;
    self.drag_ = Vector{0.0, 0.0, 0.0};
    return {d};
  }

  struct TouchPlay_sprintResult { Bool Result = false; };
  KERN_PURE TouchPlay_sprintResult TouchPlay_sprint(const TouchPlay& self) {
    Float m = 0.0;
    m = kern::formula([](Float x, Float y) -> Float { return kern::math::min(1.0, kern::math::sqrt((x * x) + (y * y))); }, vector_break(self.move_).x, vector_break(self.move_).y);
    return {logic_and(math_compare<">=">(m, 0.9), math_compare<"<">(vector_break(self.move_).y, -0.35))};
  }

  struct TouchPlay_take_zoomResult { Float Result = 0.0; };
  TouchPlay_take_zoomResult TouchPlay_take_zoom(TouchPlay& self) {
    Float z = 0.0;
    z = self.zoom_;
    self.zoom_ = 1.0;
    return {z};
  }

  struct TouchPlay_heldResult { Bool Result = false; };
  KERN_PURE TouchPlay_heldResult TouchPlay_held(const TouchPlay& self, const Text& id) {
    for (auto it37 = flow_for(n37, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it37.next();) {
      if (text_equal(self.buttons_[n37.index].id, id)) {
        return {self.buttons_[n37.index].pressed};
      }
    }
    return {false};
  }

  struct TouchPlay_take_pressedResult { Bool Result = false; };
  TouchPlay_take_pressedResult TouchPlay_take_pressed(TouchPlay& self, const Text& id) {
    Int it = 0;
    Int at = 0;
    Int end = 0;
    Bool go = false;
    at = 0;
    end = list_length<Text>(self.pressed_);
    go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
    while (loop(go)) {
      if (text_equal(self.pressed_[at], id)) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      }
    }
    it = at;
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(list_length<Text>(self.pressed_)))) {
      return {false};
    }
    list_remove_at(self.pressed_, it);
    return {true};
  }

  struct TouchPlay_take_releasedResult { Bool Result = false; };
  TouchPlay_take_releasedResult TouchPlay_take_released(TouchPlay& self, const Text& id) {
    Int it = 0;
    Int at = 0;
    Int end = 0;
    Bool go = false;
    at = 0;
    end = list_length<Text>(self.released_);
    go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
    while (loop(go)) {
      if (text_equal(self.released_[at], id)) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      }
    }
    it = at;
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(list_length<Text>(self.released_)))) {
      return {false};
    }
    list_remove_at(self.released_, it);
    return {true};
  }

  struct TouchPlay_take_strike_tapResult { Bool Result = false; };
  TouchPlay_take_strike_tapResult TouchPlay_take_strike_tap(TouchPlay& self, Float& x, Float& y) {
    if (logic_not(self.strike_tap_)) {
      return {false};
    }
    self.strike_tap_ = false;
    x = self.strike_x_;
    y = self.strike_y_;
    return {true};
  }

  struct view_json_stickResult { Text Result = ""; };
  view_json_stickResult view_json_stick(Text& buf, Bool aiming = false, Float Ly = 0.0, Float kk = 0.0, Float x = 0.0, Vector k = Vector{0.0, 0.0, 0.0}, Bool live = false, Bool accent = false, Text label = "") {
    buf = text_format<"{\"x\":%.1f,\"y\":%.1f,\"size\":%.1f,\"kx\":%.3f,\"ky\":%.3f,\"live\":%s,\"accent\":%s,\"label\":\"%s\"}">(319, kern::formula([](Float aiming, Float x) -> Float { return ((aiming != 0.0) ? (-9999.0) : x); }, kern::to<Float>(aiming), x), Ly, kStick, kern::formula([](Float x, Float kk) -> Float { return (x * kk); }, vector_break(k).x, kk), kern::formula([](Float y, Float kk) -> Float { return (y * kk); }, vector_break(k).y, kk), logic_select<Text>(live, "true", "false"), logic_select<Text>(accent, "true", "false"), label);
    return {buf};
  }

  struct TouchPlay_view_jsonResult { Text Result = ""; };
  TouchPlay_view_jsonResult TouchPlay_view_json(const TouchPlay& self) {
    Text buf = "";
    Float Ly = 0.0;
    Text ml = "";
    Text rl = "";
    Float kk = 0.0;
    Bool aiming = false;
    Text s = "";
    Bool first = false;
    if (enum_equal<TouchMode>(self.lay_.mode, TouchMode::off)) {
      return {"null"};
    }
    buf = "";
    Ly = kern::formula([](Float h_, Float sb_, Float kStick) -> Float { return ((h_ - sb_) - kStick); }, self.h_, self.sb_, kStick);
    ml = logic_select<Text>(enum_equal<TouchMode>(self.lay_.mode, TouchMode::car), "LENKEN / GAS", logic_select<Text>(enum_equal<TouchMode>(self.lay_.mode, TouchMode::jet), "STEIGEN / SCHUB", ""));
    rl = logic_select<Text>(enum_equal<TouchMode>(self.lay_.mode, TouchMode::car), "UMSCHAUEN", logic_select<Text>(enum_equal<TouchMode>(self.lay_.mode, TouchMode::jet), "KN\303\234PPEL", ""));
    kk = kern::formula([](Float a) -> Float { return ((a != 0.0) ? kern::math::div(42.0, 31.0) : 1.0); }, kern::to<Float>(enum_equal<TouchMode>(self.lay_.mode, TouchMode::drone)));
    aiming = touch_play::TouchPlay_no_sticks(self).Result;
    n38 = view_json_stick(buf, aiming, Ly, kk, self.sl_, self.move_, math_compare<">=">(kern::to<Float>(self.move_id_), 0.0), logic_and(TouchPlay_sprint(self).Result, enum_equal<TouchMode>(self.lay_.mode, TouchMode::foot)), ml);
    s = text_append("{\"move\":", n38.Result, "");
    n39 = view_json_stick(buf, aiming, Ly, kk, kern::formula([](Float w_, Float sr_, Float the_result, Float kStick) -> Float { return (((w_ - sr_) - the_result) - kStick); }, self.w_, self.sr_, touch_play::TouchPlay_right_inset().Result, kStick), self.look_, math_compare<">=">(kern::to<Float>(self.look_id_), 0.0), false, rl);
    s = text_append(s, text_append(",\"look\":", n39.Result, ""), "");
    s = text_append(s, ",\"buttons\":[", "");
    first = true;
    for (auto it40 = flow_for(n40, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it40.next();) {
      s = text_append(s, logic_select<Text>(first, "", ","), "");
      first = false;
      s = text_append(s, text_append(text_append(text_append(text_append("{\"id\":\"", esc(self.buttons_[n40.index].id).Result, ""), "\",\"label\":\"", ""), esc(self.buttons_[n40.index].label).Result, ""), "\"", ""), "");
      buf = text_format<",\"x\":%.1f,\"y\":%.1f,\"w\":%.1f,\"h\":%.1f,\"round\":%s,\"pressed\":%s,\"on\":%s,\"color\":\"#%06x\"}">(319, self.buttons_[n40.index].x, self.buttons_[n40.index].y, self.buttons_[n40.index].w, self.buttons_[n40.index].h, logic_select<Text>(self.buttons_[n40.index].the_round, "true", "false"), logic_select<Text>(self.buttons_[n40.index].pressed, "true", "false"), logic_select<Text>(self.buttons_[n40.index].on, "true", "false"), kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 16777215.0); }, kern::to<Float>(self.buttons_[n40.index].color))));
      s = text_append(s, buf, "");
    }
    s = text_append(s, "]", "");
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<TouchFire>(self.fires_)), 0.0))) {
      buf = text_format<",\"fire\":true,\"fireX\":%.1f,\"fireY\":%.1f">(319, self.fires_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchFire>(self.fires_))))].x, self.fires_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchFire>(self.fires_))))].y);
      s = text_append(s, buf, "");
    }
    buf = text_format<",\"mode\":%d,\"clearance\":%.3f">(319, enum_index<TouchMode>(self.lay_.mode), self.clearance_);
    s = text_append(s, buf, "");
    return {text_append(s, "}", "")};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct { TouchFire value = TouchFire{0, 0.0, 0.0, 0.0, -1.0, true, 0, -1.0, false}; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  controls_cpp::clearance_pad_axisResult n26;
  struct { TouchPlay_Finger value = TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}; } n27;
  struct { TouchPlay_Finger value = TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  controls_cpp::clearance_pad_axisResult n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct { TouchFire value = TouchFire{0, 0.0, 0.0, 0.0, -1.0, true, 0, -1.0, false}; } n34;
  struct : kern::Node<35> { Int index = 0; } n35;
  struct : kern::Node<36> { Int index = 0; } n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  view_json_stickResult n38;
  view_json_stickResult n39;
  struct : kern::Node<40> { Int index = 0; } n40;
};
