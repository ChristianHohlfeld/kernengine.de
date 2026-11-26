struct touch_play_cpp : virtual kern::Library, virtual touch_play {
  // ---- variables ----
  /// thumb_layout.mjs STICK
  KERN_CONST Float kStick = 168.0;
  /// edge_hud_layout.mjs SAFE / GAP
  KERN_CONST Float kSafe = 10.0;
  /// edge_hud_layout.mjs SAFE / GAP
  KERN_CONST Float kGap = 8.0;

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
  KERN_PURE btnResult btn(const Text& label, Text id = "", Float w = 0.0, Float h = 0.0, U32 color = 0, Bool the_round = false, Bool on_down = false) {
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
    U32 dark = 0;
    U32 amber = 0;
    U32 blue = 0;
    U32 red = 0;
    U32 green = 0;
    U32 yellow = 0;
    TouchBtn c = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a = "";
    TouchBtn mode = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_2 = "";
    List<TouchBtn> left = List<TouchBtn>{};
    Text a_3 = "";
    TouchBtn g = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_4 = "";
    TouchBtn p = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_5 = "";
    List<TouchBtn> low = List<TouchBtn>{};
    Text a_6 = "";
    Text a_7 = "";
    Text a_8 = "";
    Text a_9 = "";
    Text a_10 = "";
    Float x = 0.0;
    Int at = 0;
    Int count = 0;
    Bool stop = false;
    TouchBtn s = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_11 = "";
    TouchBtn cam = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_12 = "";
    TouchBtn hb = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_13 = "";
    TouchBtn cam_2 = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_14 = "";
    List<TouchBtn> up = List<TouchBtn>{};
    Text a_15 = "";
    Text a_16 = "";
    Text a_17 = "";
    List<TouchBtn> row = List<TouchBtn>{};
    Text a_18 = "";
    Text a_19 = "";
    Text a_20 = "";
    Text a_21 = "";
    Float total = 0.0;
    Float bx = 0.0;
    TouchBtn ej = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_22 = "";
    Float size = 0.0;
    TouchBtn fire = TouchBtn{"", "", 0.0, 0.0, 0.0, 0.0, true, false, false, true, 856344};
    Text a_23 = "";
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
    if (enum_equal<TouchMode>(m, TouchMode::foot)) {
      a_3 = "STRIKE";
      list_add(left, btn(a_3, "strike", 58.0, 58.0, kern::to<Int>(kern::formula([](Float strike_ready, Float dark) -> Float { return ((strike_ready != 0.0) ? dark : 3815994.0); }, kern::to<Float>(self.lay_.strike_ready), kern::to<Float>(dark))), true, true).Result);
    }
    if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.grenade)) {
      a_4 = logic_select<Text>(math_compare<">=">(self.lay_.grenade_left, 0.0), text_append(text_append(text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float grenade_left) -> Float { return kern::math::trunc(kern::math::div(kern::math::trunc((grenade_left * 10.0)), 10.0)); }, self.lay_.grenade_left))), ".", ""), text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float grenade_left) -> Float { return kern::math::mod(kern::math::trunc((grenade_left * 10.0)), 10.0); }, self.lay_.grenade_left))), ""), text_append("\342\227\217 ", text_format<"%d">(-1, self.lay_.grenades), ""));
      g = btn(a_4, "nade", 64.0, 64.0, kern::to<Int>(kern::formula([](Float grenade_left) -> Float { return kern::math::wrap32(((grenade_left >= 0.0) ? 3808268.0 : 1844244.0)); }, self.lay_.grenade_left)), true, true).Result;
      g.on = math_compare<">=">(self.lay_.grenade_left, 0.0);
      list_add(left, g);
    }
    TouchPlay_place_row(self, left, kern::formula([](Float kSafe, Float lx) -> Float { return kern::math::max(kSafe, (lx + 10.0)); }, kSafe, Lx), kern::formula([](Float ly) -> Float { return (ly - 8.0); }, Ly), 1.0, kern::formula([](Float kStick) -> Float { return kern::math::max(kStick, 190.0); }, kStick));
    if (logic_and(enum_equal<TouchMode>(m, TouchMode::drone), logic_not(self.lay_.pad_drone))) {
      a_5 = "\342\226\262 H\303\226HE \342\226\274";
      p = btn(a_5, "clearance", 48.0, 132.0, 1054752, false, true).Result;
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
      a_6 = "\342\206\227";
      list_add(low, btn(a_6, "enter", 54.0, 54.0, 16739162, true, false).Result);
    } else if (enum_equal<TouchMode>(m, TouchMode::drone)) {
      if (logic_not(self.lay_.pad_drone)) {
        a_7 = logic_select<Text>(self.lay_.armed, "ARMED", "ARM");
        list_add(low, btn(a_7, "arm", 104.0, 44.0, kern::to<Int>(kern::formula([](Float armed, Float green) -> Float { return ((armed != 0.0) ? green : 16102145.0); }, kern::to<Float>(self.lay_.armed), kern::to<Float>(green))), false, false).Result);
        a_8 = "STOP";
        list_add(low, btn(a_8, "stop", 94.0, 44.0, red, false, false).Result);
      }
    } else if (enum_equal<TouchMode>(m, TouchMode::foot)) {
      a_9 = text_append("\342\207\204 ", self.lay_.weapon, "");
      list_add(low, btn(a_9, "wpn", 60.0, 60.0, amber, true, true).Result);
      a_10 = "JUMP";
      list_add(low, btn(a_10, "jump", 64.0, 64.0, dark, true, true).Result);
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
          a_11 = "\342\227\216 SCOPE";
          s = btn(a_11, "scope", 60.0, 60.0, kern::to<Int>(kern::formula([](Float scope_on) -> Float { return kern::math::wrap32(((scope_on != 0.0) ? 11740702.0 : 793108.0)); }, kern::to<Float>(self.lay_.scope_on))), true, true).Result;
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
      a_12 = logic_select<Text>(self.lay_.cockpit, "\360\237\216\245 AUSSEN", "\360\237\216\245 INNEN");
      cam = btn(a_12, "cam", 104.0, 36.0, dark, false, false).Result;
      a_13 = "HANDBREMSE";
      hb = btn(a_13, "hb", 112.0, 36.0, dark, false, true).Result;
      hb.x = kern::formula([](Float w_, Float kSafe, Float w) -> Float { return ((w_ - kSafe) - w); }, self.w_, kSafe, hb.w);
      hb.y = kern::formula([](Float ry, Float h) -> Float { return ((ry - 8.0) - h); }, Ry, hb.h);
      cam.x = kern::formula([](Float w_, Float kSafe, Float w) -> Float { return ((w_ - kSafe) - w); }, self.w_, kSafe, cam.w);
      cam.y = kern::formula([](Float y, Float h) -> Float { return ((y - 8.0) - h); }, hb.y, cam.h);
      touch_play::TouchPlay_add(self, cam);
      touch_play::TouchPlay_add(self, hb);
    } else if (enum_equal<TouchMode>(m, TouchMode::drone)) {
      a_14 = text_append("\360\237\216\245 ", self.lay_.camera, "");
      cam_2 = btn(a_14, "cam", 104.0, 36.0, blue, false, false).Result;
      cam_2.x = kern::formula([](Float w_, Float kSafe, Float w) -> Float { return ((w_ - kSafe) - w); }, self.w_, kSafe, cam_2.w);
      cam_2.y = kern::formula([](Float ry, Float h) -> Float { return ((ry - 8.0) - h); }, Ry, cam_2.h);
      touch_play::TouchPlay_add(self, cam_2);
    } else if (enum_equal<TouchMode>(m, TouchMode::foot)) {
      up = List<TouchBtn>{};
      if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.near_car)) {
        a_15 = "\360\237\232\227";
        list_add(up, btn(a_15, "enter", 54.0, 54.0, yellow, true, false).Result);
      }
      if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.near_jet)) {
        a_16 = "\342\234\210";
        list_add(up, btn(a_16, "jet", 54.0, 54.0, 8377343, true, false).Result);
      }
      if (self.lay_.emp) {
        a_17 = "EMP";
        list_add(up, btn(a_17, "emp", 62.0, 36.0, 2649770, false, true).Result);
      }
      TouchPlay_place_row(self, up, kern::formula([](Float w_, Float kSafe) -> Float { return (w_ - kSafe); }, self.w_, kSafe), kern::formula([](Float ry) -> Float { return (ry - 8.0); }, Ry), -1.0, kern::formula([](Float kStick) -> Float { return kern::math::max(kStick, 190.0); }, kStick));
    }
    if (enum_equal<TouchMode>(m, TouchMode::jet)) {
      a_18 = logic_select<Text>(self.lay_.hover, "\342\207\204 FLUG", "\342\207\204 SCHWEBEN");
      a_19 = "RAKETE";
      a_20 = "BOMBE";
      a_21 = "CAM";
      row = list_make<TouchBtn, "4">(btn(a_18, "vtol", 96.0, 40.0, 529420, false, false).Result, btn(a_19, "rkt", 72.0, 40.0, kern::to<Int>(kern::formula([](Float rocket) -> Float { return kern::math::wrap32(((rocket != 0.0) ? 6635531.0 : 529420.0)); }, kern::to<Float>(self.lay_.rocket))), false, false).Result, btn(a_20, "bomb", 70.0, 40.0, 529420, false, false).Result, btn(a_21, "cam", 60.0, 40.0, 529420, false, false).Result);
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
      a_22 = logic_select<Text>(self.lay_.ground, "AUSSTEIGEN", "EJECT");
      ej = btn(a_22, "eject", 84.0, 38.0, 4266001, false, false).Result;
      ej.x = kern::formula([](Float sl_) -> Float { return kern::math::max(8.0, sl_); }, self.sl_);
      ej.y = kern::formula([](Float st_) -> Float { return (st_ + 55.0); }, self.st_);
      touch_play::TouchPlay_add(self, ej);
      size = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float w_) -> Float { return (w_ * 0.13); }, self.w_), 96.0, 116.0));
      a_23 = "KANONE";
      fire = btn(a_23, "fire", size, size, 8002066, true, true).Result;
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

  struct TouchPlay_button_atResult { I32 Result = 0; };
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
  KERN_PURE pointer_matchResult pointer_match(const TouchPlay_Finger& f, I32 id = 0) {
    return {math_compare<"==">(kern::to<Float>(f.id), kern::to<Float>(id))};
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
    for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it26.next();) {
      if (text_equal(self.buttons_[n26.index].id, id)) {
        return {self.buttons_[n26.index].pressed};
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
    n27 = view_json_stick(buf, aiming, Ly, kk, self.sl_, self.move_, math_compare<">=">(kern::to<Float>(self.move_id_), 0.0), logic_and(TouchPlay_sprint(self).Result, enum_equal<TouchMode>(self.lay_.mode, TouchMode::foot)), ml);
    s = text_append("{\"move\":", n27.Result, "");
    n28 = view_json_stick(buf, aiming, Ly, kk, kern::formula([](Float w_, Float sr_, Float the_result, Float kStick) -> Float { return (((w_ - sr_) - the_result) - kStick); }, self.w_, self.sr_, touch_play::TouchPlay_right_inset().Result, kStick), self.look_, math_compare<">=">(kern::to<Float>(self.look_id_), 0.0), false, rl);
    s = text_append(s, text_append(",\"look\":", n28.Result, ""), "");
    s = text_append(s, ",\"buttons\":[", "");
    first = true;
    for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TouchBtn>(self.buttons_))))); it29.next();) {
      s = text_append(s, logic_select<Text>(first, "", ","), "");
      first = false;
      s = text_append(s, text_append(text_append(text_append(text_append("{\"id\":\"", esc(self.buttons_[n29.index].id).Result, ""), "\",\"label\":\"", ""), esc(self.buttons_[n29.index].label).Result, ""), "\"", ""), "");
      buf = text_format<",\"x\":%.1f,\"y\":%.1f,\"w\":%.1f,\"h\":%.1f,\"round\":%s,\"pressed\":%s,\"on\":%s,\"color\":\"#%06x\"}">(319, self.buttons_[n29.index].x, self.buttons_[n29.index].y, self.buttons_[n29.index].w, self.buttons_[n29.index].h, logic_select<Text>(self.buttons_[n29.index].the_round, "true", "false"), logic_select<Text>(self.buttons_[n29.index].pressed, "true", "false"), logic_select<Text>(self.buttons_[n29.index].on, "true", "false"), kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 16777215.0); }, kern::to<Float>(self.buttons_[n29.index].color))));
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
  struct : kern::Node<26> { Int index = 0; } n26;
  view_json_stickResult n27;
  view_json_stickResult n28;
  struct : kern::Node<29> { Int index = 0; } n29;
};
