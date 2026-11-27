struct touch_play_cpp : virtual kern::Library, virtual common, virtual touch_play {
  // ---- variables ----
  /// thumb_layout.mjs STICK
  KERN_CONST f32 kStick = 168.0f;
  /// edge_hud_layout.mjs SAFE / GAP
  KERN_CONST f32 kSafe = 10.0f;
  /// edge_hud_layout.mjs SAFE / GAP
  KERN_CONST f32 kGap = 8.0f;

  // ---- functions ----
  struct escResult { Text Result = ""; };
  KERN_PURE escResult esc(const Text& s) {
    Text o = "";
    bool skip = false;
    i8 c = 0;
    o = "";
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(text_length(s), 1)); it10.next();) {
      c = num_cast<i8, i64>(text_code_at(s, n10.index));
      skip = false;
      if (math_compare<"<", i32>(num_cast<i32, u8>(num_cast<u8, i8>(c)), 32)) {
        o = text_append(o, text_of_code(32), "");
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(math_compare<"==", i32>(num_cast<i32, i8>(c), 34), math_compare<"==", i32>(num_cast<i32, i8>(c), 92))) {
          o = text_append(o, text_of_code(92), "");
        }
        o = text_append(o, text_of_code(num_cast<i64, i8>(c)), "");
      }
    }
    return {o};
  }

  struct btnResult { TouchBtn Result = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344}; };
  KERN_PURE btnResult btn(const Text& label, Text id = "", f32 w = 0.0f, f32 h = 0.0f, u32 color = 0, bool the_round = false, bool on_down = false) {
    TouchBtn b = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    b = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    b.id = id;
    b.label = label;
    b.w = w;
    b.h = h;
    b.color = color;
    b.the_round = the_round;
    b.on_down = on_down;
    return {b};
  }

  void TouchPlay_place_row(TouchPlay& self, List<TouchBtn>& items, f32 x0 = 0.0f, f32 bottom = 0.0f, f32 dir = 0.0f, f32 max_w = 0.0f) {
    f32 x = 0.0f;
    f32 row_h = 0.0f;
    f32 y_bottom = 0.0f;
    f32 used = 0.0f;
    x = x0;
    row_h = 0.0f;
    y_bottom = bottom;
    used = 0.0f;
    for (auto it12 = flow_for(n12, 0, math_sub<i64>(list_length<TouchBtn>(items), 1)); it12.next();) {
      if (logic_and(math_compare<">", f32>(used, 0.0f), math_compare<">", f32>(kern::formula<f32, f32>([](auto used, auto a) { return (used + a); }, used, items[n12.index].w), max_w))) {
        y_bottom = kern::formula<f32, f32, f32>([](auto y_bottom, auto row_h, auto kGap) { return (y_bottom - (row_h + kGap)); }, y_bottom, row_h, kGap);
        x = x0;
        used = 0.0f;
        row_h = 0.0f;
      }
      items[n12.index].x = kern::formula<f32, f32, f32>([](auto dir, auto x, auto a) { return ((dir > 0.0f) ? x : (x - a)); }, dir, x, items[n12.index].w);
      items[n12.index].y = kern::formula<f32, f32>([](auto y_bottom, auto a) { return (y_bottom - a); }, y_bottom, items[n12.index].h);
      x += kern::formula<f32, f32, f32>([](auto dir, auto a, auto kGap) { return (dir * (a + kGap)); }, dir, items[n12.index].w, kGap);
      used += kern::formula<f32, f32>([](auto a, auto kGap) { return (a + kGap); }, items[n12.index].w, kGap);
      row_h = kern::formula<f32, f32>([](auto row_h, auto a) { return max(row_h, a); }, row_h, items[n12.index].h);
      touch_play::TouchPlay_add(self, items[n12.index]);
    }
  }

  void TouchPlay_apply_dom_rects(TouchPlay& self) {
    bool skip = false;
    for (auto it17 = flow_for(n17, 0, math_sub<i64>(list_length<TouchPlay_DomRect>(self.dom_), 1)); it17.next();) {
      skip = false;
      if (text_equal(self.dom_[n17.index].id, "clearance")) {
        if (math_compare<">", f32>(self.pad_w_, 0.0f)) {
          self.pad_x_ = self.dom_[n17.index].x;
          self.pad_y_ = self.dom_[n17.index].y;
          self.pad_w_ = self.dom_[n17.index].w;
          self.pad_h_ = self.dom_[n17.index].h;
        }
        for (auto it18 = flow_for(n18, 0, math_sub<i64>(list_length<TouchBtn>(self.buttons_), 1)); it18.next();) {
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
        for (auto it19 = flow_for(n19, 0, math_sub<i64>(list_length<TouchBtn>(self.buttons_), 1)); it19.next();) {
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
    f32 Lx = 0.0f;
    f32 Ly = 0.0f;
    f32 Rx = 0.0f;
    f32 Ry = 0.0f;
    u32 dark = 0;
    u32 amber = 0;
    u32 blue = 0;
    u32 red = 0;
    u32 green = 0;
    u32 yellow = 0;
    TouchBtn c = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    Text a = "";
    TouchBtn mode = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    Text a_2 = "";
    List<TouchBtn> left = List<TouchBtn>{};
    Text a_3 = "";
    TouchBtn g = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    Text a_4 = "";
    TouchBtn p = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    Text a_5 = "";
    List<TouchBtn> low = List<TouchBtn>{};
    Text a_6 = "";
    Text a_7 = "";
    Text a_8 = "";
    Text a_9 = "";
    Text a_10 = "";
    f32 x = 0.0f;
    i64 at = 0;
    i64 count = 0;
    bool stop = false;
    TouchBtn s = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    Text a_11 = "";
    TouchBtn cam = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    Text a_12 = "";
    TouchBtn hb = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    Text a_13 = "";
    TouchBtn cam_2 = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
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
    f32 total = 0.0f;
    f32 bx = 0.0f;
    TouchBtn ej = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    Text a_22 = "";
    f32 size = 0.0f;
    TouchBtn fire = TouchBtn{"", "", 0.0f, 0.0f, 0.0f, 0.0f, true, false, false, true, 856344};
    Text a_23 = "";
    i64 at_2 = 0;
    i64 end = 0;
    bool go = false;
    down = List<Text>{};
    for (auto it11 = flow_for(n11, 0, math_sub<i64>(list_length<TouchBtn>(self.buttons_), 1)); it11.next();) {
      if (self.buttons_[n11.index].pressed) {
        list_add(down, self.buttons_[n11.index].id);
      }
    }
    list_clear(self.buttons_);
    self.pad_h_ = 0.0f;
    self.pad_w_ = self.pad_h_;
    if (enum_equal<TouchMode>(self.lay_.mode, TouchMode::off)) {
      return;
    }
    m = self.lay_.mode;
    Lx = self.sl_;
    Ly = kern::formula<f32, f32, f32>([](auto h_, auto sb_, auto kStick) { return ((h_ - sb_) - kStick); }, self.h_, self.sb_, kStick);
    Rx = kern::formula<f32, f32, f32, f32>([](auto w_, auto sr_, auto the_result, auto kStick) { return (((w_ - sr_) - the_result) - kStick); }, self.w_, self.sr_, touch_play::TouchPlay_right_inset().Result, kStick);
    Ry = kern::formula<f32, f32, f32>([](auto h_, auto sb_, auto kStick) { return ((h_ - sb_) - kStick); }, self.h_, self.sb_, kStick);
    dark = 856344;
    amber = 2759948;
    blue = 928318;
    red = 9118774;
    green = 3120730;
    yellow = 16763196;
    if (self.lay_.aiming) {
      a = "\342\234\225 ABBRECHEN";
      c = btn(a, "strike_cancel", 120.0f, 40.0f, red, false, false).Result;
      c.x = kern::formula<f32, f32>([](auto w_, auto w) { return ((w_ - w) / 2.0f); }, self.w_, c.w);
      c.y = kern::formula<f32>([](auto st_) { return (max(8.0f, st_) + 60.0f); }, self.st_);
      touch_play::TouchPlay_add(self, c);
      return;
    }
    if (logic_or(enum_equal<TouchMode>(m, TouchMode::foot), enum_equal<TouchMode>(m, TouchMode::car))) {
      a_2 = logic_select<Text>(enum_equal<TouchMode>(m, TouchMode::foot), logic_select<Text>(self.lay_.drone_cooldown, "\342\207\204 DRONE \342\200\246", "\342\207\204 DRONE"), "\342\207\204 FPS");
      mode = btn(a_2, "mode", 96.0f, 40.0f, kern::formula<bool, u32, u32>([](auto a, auto a3811344, auto blue) { return (a ? a3811344 : blue); }, enum_equal<TouchMode>(m, TouchMode::foot), 3811344, blue), false, false).Result;
      mode.x = kern::formula<f32, f32>([](auto lx, auto kStick) { return ((lx + kStick) + 8.0f); }, Lx, kStick);
      mode.y = kern::formula<f32, f32, f32>([](auto h_, auto kSafe, auto h) { return ((h_ - kSafe) - h); }, self.h_, kSafe, mode.h);
      touch_play::TouchPlay_add(self, mode);
    }
    left = List<TouchBtn>{};
    if (enum_equal<TouchMode>(m, TouchMode::foot)) {
      a_3 = "STRIKE";
      list_add(left, btn(a_3, "strike", 58.0f, 58.0f, kern::formula<bool, u32, u32>([](auto strike_ready, auto dark, auto a3815994) { return (strike_ready ? dark : a3815994); }, self.lay_.strike_ready, dark, 3815994), true, true).Result);
    }
    if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.grenade)) {
      a_4 = logic_select<Text>(math_compare<">=", f32>(self.lay_.grenade_left, 0.0f), text_append(text_append(text_format<"%d">(-1, num_cast<i64, i32>(kern::formula<f32>([](auto grenade_left) { return (num_cast<i32, f32>((grenade_left * 10.0f)) / 10); }, self.lay_.grenade_left))), ".", ""), text_format<"%d">(-1, num_cast<i64, i32>(kern::formula<f32>([](auto grenade_left) { return (num_cast<i32, f32>((grenade_left * 10.0f)) % 10); }, self.lay_.grenade_left))), ""), text_append("\342\227\217 ", text_format<"%d">(-1, num_cast<i64, i32>(self.lay_.grenades)), ""));
      g = btn(a_4, "nade", 64.0f, 64.0f, kern::formula<f32, i32, i32>([](auto grenade_left, auto a3808268, auto a1844244) { return num_cast<u32, i32>(((grenade_left >= 0.0f) ? a3808268 : a1844244)); }, self.lay_.grenade_left, 3808268, 1844244), true, true).Result;
      g.on = math_compare<">=", f32>(self.lay_.grenade_left, 0.0f);
      list_add(left, g);
    }
    TouchPlay_place_row(self, left, kern::formula<f32, f32>([](auto kSafe, auto lx) { return max(kSafe, (lx + 10.0f)); }, kSafe, Lx), kern::formula<f32>([](auto ly) { return (ly - 8.0f); }, Ly), 1.0f, kern::formula<f32>([](auto kStick) { return max(kStick, 190.0f); }, kStick));
    if (logic_and(enum_equal<TouchMode>(m, TouchMode::drone), logic_not(self.lay_.pad_drone))) {
      a_5 = "\342\226\262 H\303\226HE \342\226\274";
      p = btn(a_5, "clearance", 48.0f, 132.0f, 1054752, false, true).Result;
      p.x = kern::formula<f32, f32>([](auto lx, auto kStick) { return ((lx + kStick) + 8.0f); }, Lx, kStick);
      p.y = kern::formula<f32, f32, f32>([](auto h_, auto kSafe, auto h) { return ((h_ - kSafe) - h); }, self.h_, kSafe, p.h);
      self.pad_x_ = p.x;
      self.pad_y_ = p.y;
      self.pad_w_ = p.w;
      self.pad_h_ = p.h;
      touch_play::TouchPlay_add(self, p);
    }
    low = List<TouchBtn>{};
    if (enum_equal<TouchMode>(m, TouchMode::car)) {
      a_6 = "\342\206\227";
      list_add(low, btn(a_6, "enter", 54.0f, 54.0f, 16739162, true, false).Result);
    } else if (enum_equal<TouchMode>(m, TouchMode::drone)) {
      if (logic_not(self.lay_.pad_drone)) {
        a_7 = logic_select<Text>(self.lay_.armed, "ARMED", "ARM");
        list_add(low, btn(a_7, "arm", 104.0f, 44.0f, kern::formula<bool, u32, u32>([](auto armed, auto green, auto a16102145) { return (armed ? green : a16102145); }, self.lay_.armed, green, 16102145), false, false).Result);
        a_8 = "STOP";
        list_add(low, btn(a_8, "stop", 94.0f, 44.0f, red, false, false).Result);
      }
    } else if (enum_equal<TouchMode>(m, TouchMode::foot)) {
      a_9 = text_append("\342\207\204 ", self.lay_.weapon, "");
      list_add(low, btn(a_9, "wpn", 60.0f, 60.0f, amber, true, true).Result);
      a_10 = "JUMP";
      list_add(low, btn(a_10, "jump", 64.0f, 64.0f, dark, true, true).Result);
    }
    x = kern::formula<f32>([](auto rx) { return (rx - 8.0f); }, Rx);
    for (auto it13 = flow_for(n13, 0, math_sub<i64>(list_length<TouchBtn>(low), 1)); it13.next();) {
      low[n13.index].x = kern::formula<f32, f32>([](auto x, auto a) { return (x - a); }, x, low[n13.index].w);
      low[n13.index].y = kern::formula<f32, f32, f32>([](auto h_, auto kSafe, auto a) { return ((h_ - kSafe) - a); }, self.h_, kSafe, low[n13.index].h);
      x = kern::formula<f32, f32>([](auto x, auto a) { return (x - (a + 8.0f)); }, x, low[n13.index].w);
      touch_play::TouchPlay_add(self, low[n13.index]);
    }
    if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.sniper)) {
      count = list_length<TouchBtn>(self.buttons_);
      at = 0;
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<"<", i64>(at, count)))) {
        if (text_equal(self.buttons_[at].id, "wpn")) {
          a_11 = "\342\227\216 SCOPE";
          s = btn(a_11, "scope", 60.0f, 60.0f, kern::formula<bool, i32, i32>([](auto scope_on, auto a11740702, auto a793108) { return num_cast<u32, i32>((scope_on ? a11740702 : a793108)); }, self.lay_.scope_on, 11740702, 793108), true, true).Result;
          s.on = self.lay_.scope_on;
          s.x = self.buttons_[at].x;
          s.y = kern::formula<f32>([](auto a) { return (a - 66.0f); }, self.buttons_[at].y);
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
      cam = btn(a_12, "cam", 104.0f, 36.0f, dark, false, false).Result;
      a_13 = "HANDBREMSE";
      hb = btn(a_13, "hb", 112.0f, 36.0f, dark, false, true).Result;
      hb.x = kern::formula<f32, f32, f32>([](auto w_, auto kSafe, auto w) { return ((w_ - kSafe) - w); }, self.w_, kSafe, hb.w);
      hb.y = kern::formula<f32, f32>([](auto ry, auto h) { return ((ry - 8.0f) - h); }, Ry, hb.h);
      cam.x = kern::formula<f32, f32, f32>([](auto w_, auto kSafe, auto w) { return ((w_ - kSafe) - w); }, self.w_, kSafe, cam.w);
      cam.y = kern::formula<f32, f32>([](auto y, auto h) { return ((y - 8.0f) - h); }, hb.y, cam.h);
      touch_play::TouchPlay_add(self, cam);
      touch_play::TouchPlay_add(self, hb);
    } else if (enum_equal<TouchMode>(m, TouchMode::drone)) {
      a_14 = text_append("\360\237\216\245 ", self.lay_.camera, "");
      cam_2 = btn(a_14, "cam", 104.0f, 36.0f, blue, false, false).Result;
      cam_2.x = kern::formula<f32, f32, f32>([](auto w_, auto kSafe, auto w) { return ((w_ - kSafe) - w); }, self.w_, kSafe, cam_2.w);
      cam_2.y = kern::formula<f32, f32>([](auto ry, auto h) { return ((ry - 8.0f) - h); }, Ry, cam_2.h);
      touch_play::TouchPlay_add(self, cam_2);
    } else if (enum_equal<TouchMode>(m, TouchMode::foot)) {
      up = List<TouchBtn>{};
      if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.near_car)) {
        a_15 = "\360\237\232\227";
        list_add(up, btn(a_15, "enter", 54.0f, 54.0f, yellow, true, false).Result);
      }
      if (logic_and(enum_equal<TouchMode>(m, TouchMode::foot), self.lay_.near_jet)) {
        a_16 = "\342\234\210";
        list_add(up, btn(a_16, "jet", 54.0f, 54.0f, 8377343, true, false).Result);
      }
      if (self.lay_.emp) {
        a_17 = "EMP";
        list_add(up, btn(a_17, "emp", 62.0f, 36.0f, 2649770, false, true).Result);
      }
      TouchPlay_place_row(self, up, kern::formula<f32, f32>([](auto w_, auto kSafe) { return (w_ - kSafe); }, self.w_, kSafe), kern::formula<f32>([](auto ry) { return (ry - 8.0f); }, Ry), -1.0f, kern::formula<f32>([](auto kStick) { return max(kStick, 190.0f); }, kStick));
    }
    if (enum_equal<TouchMode>(m, TouchMode::jet)) {
      a_18 = logic_select<Text>(self.lay_.hover, "\342\207\204 FLUG", "\342\207\204 SCHWEBEN");
      a_19 = "RAKETE";
      a_20 = "BOMBE";
      a_21 = "CAM";
      row = list_make<TouchBtn, "4">(btn(a_18, "vtol", 96.0f, 40.0f, 529420, false, false).Result, btn(a_19, "rkt", 72.0f, 40.0f, kern::formula<bool, i32, i32>([](auto rocket, auto a6635531, auto a529420) { return num_cast<u32, i32>((rocket ? a6635531 : a529420)); }, self.lay_.rocket, 6635531, 529420), false, false).Result, btn(a_20, "bomb", 70.0f, 40.0f, 529420, false, false).Result, btn(a_21, "cam", 60.0f, 40.0f, 529420, false, false).Result);
      row[1].on = self.lay_.rocket;
      total = -8.0f;
      for (auto it14 = flow_for(n14, 0, math_sub<i64>(list_length<TouchBtn>(row), 1)); it14.next();) {
        total += kern::formula<f32>([](auto a) { return (a + 8.0f); }, row[n14.index].w);
      }
      bx = kern::formula<f32, f32>([](auto w_, auto total) { return ((w_ - total) / 2.0f); }, self.w_, total);
      for (auto it15 = flow_for(n15, 0, math_sub<i64>(list_length<TouchBtn>(row), 1)); it15.next();) {
        row[n15.index].x = bx;
        row[n15.index].y = kern::formula<f32, f32, f32>([](auto h_, auto sb_, auto a) { return ((h_ - max(16.0f, sb_)) - a); }, self.h_, self.sb_, row[n15.index].h);
        bx += kern::formula<f32>([](auto a) { return (a + 8.0f); }, row[n15.index].w);
        touch_play::TouchPlay_add(self, row[n15.index]);
      }
      a_22 = logic_select<Text>(self.lay_.ground, "AUSSTEIGEN", "EJECT");
      ej = btn(a_22, "eject", 84.0f, 38.0f, 4266001, false, false).Result;
      ej.x = kern::formula<f32>([](auto sl_) { return max(8.0f, sl_); }, self.sl_);
      ej.y = kern::formula<f32>([](auto st_) { return (st_ + 55.0f); }, self.st_);
      touch_play::TouchPlay_add(self, ej);
      size = common::clampf(kern::formula<f32>([](auto w_) { return (w_ * 0.13f); }, self.w_), 96.0f, 116.0f).Result;
      a_23 = "KANONE";
      fire = btn(a_23, "fire", size, size, 8002066, true, true).Result;
      fire.x = kern::formula<f32, f32, f32>([](auto w_, auto sr_, auto size) { return ((w_ - ((max(12.0f, sr_) + min((w_ * 0.25f), 148.0f)) + 18.0f)) - size); }, self.w_, self.sr_, size);
      fire.y = kern::formula<f32, f32, f32>([](auto h_, auto sb_, auto size) { return ((h_ - max(22.0f, (sb_ + 18.0f))) - size); }, self.h_, self.sb_, size);
      touch_play::TouchPlay_add(self, fire);
    }
    for (auto it16 = flow_for(n16, 0, math_sub<i64>(list_length<TouchBtn>(self.buttons_), 1)); it16.next();) {
      at_2 = 0;
      end = list_length<Text>(down);
      go = math_compare<"<", i64>(at_2, end);
      while (loop(go)) {
        if (text_equal(down[at_2], self.buttons_[n16.index].id)) {
          go = false;
        } else {
          at_2 += 1;
          go = math_compare<"<", i64>(at_2, end);
        }
      }
      if (logic_not(math_compare<"==", i64>(at_2, list_length<Text>(down)))) {
        self.buttons_[n16.index].pressed = true;
      }
    }
    TouchPlay_apply_dom_rects(self);
  }

  void TouchPlay_set_frame(TouchPlay& self, f32 w = 0.0f, f32 h = 0.0f, f32 sl = 0.0f, f32 st = 0.0f, f32 sr = 0.0f, f32 sb = 0.0f) {
    self.w_ = w;
    self.h_ = h;
    self.sl_ = sl;
    self.st_ = st;
    self.sr_ = sr;
    self.sb_ = sb;
    TouchPlay_layout(self);
  }

  struct release_all_matchResult { bool Result = false; };
  KERN_PURE release_all_matchResult release_all_match(const TouchFire& f) {
    return {logic_not(f.lifted)};
  }

  void TouchPlay_release_all(TouchPlay& self) {
    i64 at = 0;
    i64 kept = 0;
    i64 end = 0;
    for (auto it20 = flow_for(n20, 0, math_sub<i64>(list_length<TouchPlay_Finger>(self.fingers_), 1)); it20.next();) {
      if (math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[n20.index].kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::press)))) {
        list_add(self.released_, self.fingers_[n20.index].button);
      }
    }
    list_clear(self.fingers_);
    for (auto it21 = flow_for(n21, 0, math_sub<i64>(list_length<TouchFire>(self.fires_), 1)); it21.next();) {
      self.fires_[n21.index].lifted = logic_or(self.fires_[n21.index].lifted, self.fires_[n21.index].fresh);
    }
    at = 0;
    kept = at;
    end = list_length<TouchFire>(self.fires_);
    while (loop(math_compare<"<", i64>(at, end))) {
      if (logic_not(release_all_match(self.fires_[at]).Result)) {
        if (math_compare<"<", i64>(kept, at)) {
          var_take(n22, self.fires_[at]);
          self.fires_[kept] = n22.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.fires_, kept, TouchFire{0, 0.0f, 0.0f, 0.0f, -1.0f, true, 0, -1.0f, false});
    self.drag_ = Vector{0.0, 0.0, 0.0};
    self.look_edge_ = self.drag_;
    self.look_ = self.look_edge_;
    self.move_ = self.look_;
    self.look_id_ = -1;
    self.move_id_ = self.look_id_;
    self.clearance_ = 0.0f;
    for (auto it23 = flow_for(n23, 0, math_sub<i64>(list_length<TouchBtn>(self.buttons_), 1)); it23.next();) {
      self.buttons_[n23.index].pressed = false;
    }
  }

  void TouchPlay_set_layout(TouchPlay& self, const TouchLayout& l) {
    bool mode_changed = false;
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

  struct TouchPlay_take_dragResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  TouchPlay_take_dragResult TouchPlay_take_drag(TouchPlay& self) {
    Vector d = Vector{0.0, 0.0, 0.0};
    d = self.drag_;
    self.drag_ = Vector{0.0, 0.0, 0.0};
    return {d};
  }

  struct TouchPlay_take_zoomResult { f32 Result = 0.0f; };
  TouchPlay_take_zoomResult TouchPlay_take_zoom(TouchPlay& self) {
    f32 z = 0.0f;
    z = self.zoom_;
    self.zoom_ = 1.0f;
    return {z};
  }

  struct TouchPlay_heldResult { bool Result = false; };
  KERN_PURE TouchPlay_heldResult TouchPlay_held(const TouchPlay& self, const Text& id) {
    for (auto it26 = flow_for(n26, 0, math_sub<i64>(list_length<TouchBtn>(self.buttons_), 1)); it26.next();) {
      if (text_equal(self.buttons_[n26.index].id, id)) {
        return {self.buttons_[n26.index].pressed};
      }
    }
    return {false};
  }

  struct TouchPlay_take_pressedResult { bool Result = false; };
  TouchPlay_take_pressedResult TouchPlay_take_pressed(TouchPlay& self, const Text& id) {
    i64 it = 0;
    i64 at = 0;
    i64 end = 0;
    bool go = false;
    at = 0;
    end = list_length<Text>(self.pressed_);
    go = math_compare<"<", i64>(at, end);
    while (loop(go)) {
      if (text_equal(self.pressed_[at], id)) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<", i64>(at, end);
      }
    }
    it = at;
    if (math_compare<"==", i64>(it, list_length<Text>(self.pressed_))) {
      return {false};
    }
    list_remove_at(self.pressed_, it);
    return {true};
  }

  struct TouchPlay_take_releasedResult { bool Result = false; };
  TouchPlay_take_releasedResult TouchPlay_take_released(TouchPlay& self, const Text& id) {
    i64 it = 0;
    i64 at = 0;
    i64 end = 0;
    bool go = false;
    at = 0;
    end = list_length<Text>(self.released_);
    go = math_compare<"<", i64>(at, end);
    while (loop(go)) {
      if (text_equal(self.released_[at], id)) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<", i64>(at, end);
      }
    }
    it = at;
    if (math_compare<"==", i64>(it, list_length<Text>(self.released_))) {
      return {false};
    }
    list_remove_at(self.released_, it);
    return {true};
  }

  struct TouchPlay_take_strike_tapResult { bool Result = false; };
  TouchPlay_take_strike_tapResult TouchPlay_take_strike_tap(TouchPlay& self, f32& x, f32& y) {
    if (logic_not(self.strike_tap_)) {
      return {false};
    }
    self.strike_tap_ = false;
    x = self.strike_x_;
    y = self.strike_y_;
    return {true};
  }

  struct view_json_stickResult { Text Result = ""; };
  view_json_stickResult view_json_stick(Text& buf, bool aiming = false, f32 Ly = 0.0f, f32 kk = 0.0f, f32 x = 0.0f, Vector k = Vector{0.0, 0.0, 0.0}, bool live = false, bool accent = false, Text label = "") {
    buf = text_format<"{\"x\":%.1f,\"y\":%.1f,\"size\":%.1f,\"kx\":%.3f,\"ky\":%.3f,\"live\":%s,\"accent\":%s,\"label\":\"%s\"}">(319, kern::formula<bool, f32, f32>([](auto aiming, auto a, auto x) { return num_cast<f64, f32>((aiming ? a : x)); }, aiming, -9999.0f, x), num_cast<f64, f32>(Ly), num_cast<f64, f32>(kStick), kern::formula<f32, f32>([](auto x, auto kk) { return num_cast<f64, f32>((x * kk)); }, vector_break(k).x, kk), kern::formula<f32, f32>([](auto y, auto kk) { return num_cast<f64, f32>((y * kk)); }, vector_break(k).y, kk), logic_select<Text>(live, "true", "false"), logic_select<Text>(accent, "true", "false"), label);
    return {buf};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  struct : kern::Node<12> { i64 index = 0; } n12;
  struct : kern::Node<13> { i64 index = 0; } n13;
  struct : kern::Node<14> { i64 index = 0; } n14;
  struct : kern::Node<15> { i64 index = 0; } n15;
  struct : kern::Node<16> { i64 index = 0; } n16;
  struct : kern::Node<17> { i64 index = 0; } n17;
  struct : kern::Node<18> { i64 index = 0; } n18;
  struct : kern::Node<19> { i64 index = 0; } n19;
  struct : kern::Node<20> { i64 index = 0; } n20;
  struct : kern::Node<21> { i64 index = 0; } n21;
  struct { TouchFire value = TouchFire{0, 0.0f, 0.0f, 0.0f, -1.0f, true, 0, -1.0f, false}; } n22;
  struct : kern::Node<23> { i64 index = 0; } n23;
  struct : kern::Node<26> { i64 index = 0; } n26;
};
