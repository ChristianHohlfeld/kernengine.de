struct combat_audio_cpp : virtual kern::Library, virtual combat_audio, virtual shapes {
  // ---- types ----
  KERN_STRUCT(combat_audio_Random) { Int s = 0; };

  // ---- variables ----
  KERN_CONST Float TAU = 6.283185307179586;
  /// Reward / fail cues in E-flat minor at ~176 BPM, 16ths, a filtered pulse over a sine sub and a dotted-8th echo (renderCue).
  KERN_CONST Float kSixteenth = 0.08522727272727272;

  // ---- functions ----
  struct Random_newResult { combat_audio_Random Result = combat_audio_Random{0}; };
  KERN_PURE Random_newResult Random_new(Int seed = 0) {
    combat_audio_Random self = combat_audio_Random{0};
    self.s = kern::to<Int>(kern::formula([](Float seed) -> Float { return ((seed != 0.0) ? seed : 1.0); }, kern::to<Float>(seed)));
    return {self};
  }

  struct Random_callResult { Float Result = 0.0; };
  Random_callResult Random_call(combat_audio_Random& self) {
    self.s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 13.0))); }, kern::to<Float>(self.s)));
    self.s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::shr(s, 17.0)); }, kern::to<Float>(self.s)));
    self.s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 5.0))); }, kern::to<Float>(self.s)));
    return {kern::formula([](Float s) -> Float { return kern::math::div(s, 4294967296.0); }, kern::to<Float>(self.s))};
  }

  struct envelopeResult { Float Result = 0.0; };
  KERN_PURE envelopeResult envelope(Float t = 0.0, Float attack = 0.0, Float release = 0.0, Float duration = 0.0) {
    return {kern::formula([](Float t, Float attack, Float duration, Float release) -> Float { return (kern::math::min(1.0, kern::math::div(t, kern::math::max(0.001, attack))) * kern::math::min(1.0, kern::math::div((duration - t), kern::math::max(0.001, release)))); }, t, attack, duration, release)};
  }

  struct finishResult { List<Float> Result = List<Float>{}; };
  finishResult finish(List<Float>& d, Float target = 0.0) {
    Float peak = 0.0;
    Float v = 0.0;
    Float gain = 0.0;
    List<Float> out = List<Float>{};
    List<Float> made = List<Float>{};
    peak = 1e-06;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(d))))); it10.next();) {
      v = d[n10.index];
      peak = kern::formula([](Float peak, Float v) -> Float { return kern::math::max(peak, kern::math::abs(v)); }, peak, v);
    }
    gain = kern::formula([](Float target, Float peak) -> Float { return kern::math::div(target, peak); }, target, peak);
    made = List<Float>{};
    list_resize(made, list_length<Float>(d), 0.0);
    out = made;
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float>(d))))); it11.next();) {
      out[n11.index] = kern::formula([](Float a, Float gain) -> Float { return kern::math::div(kern::math::exp(2.0 * ((a * gain) * 1.35)) - 1.0, kern::math::exp(2.0 * ((a * gain) * 1.35)) + 1.0); }, d[n11.index], gain);
    }
    return {out};
  }

  struct samplesResult { Int Result = 0; };
  KERN_PURE samplesResult samples(Float duration = 0.0, Int rate = 0) {
    return {kern::to<Int>(kern::formula([](Float duration, Float rate) -> Float { return kern::math::trunc(kern::math::ceil((duration * rate))); }, duration, kern::to<Float>(rate)))};
  }

  struct nwave_atResult { Float Result = 0.0; };
  KERN_PURE nwave_atResult nwave_at(Float t = 0.0, Float rise = 0.0, Float fall = 0.0) {
    if (math_compare<"<">(t, rise)) {
      return {kern::formula([](Float t, Float rise) -> Float { return kern::math::div(t, rise); }, t, rise)};
    }
    if (math_compare<"<">(t, kern::formula([](Float rise, Float fall) -> Float { return (rise + fall); }, rise, fall))) {
      return {kern::formula([](Float t, Float rise, Float fall) -> Float { return (1.0 - kern::math::div((1.6 * (t - rise)), fall)); }, t, rise, fall)};
    }
    if (math_compare<"<">(t, kern::formula([](Float rise, Float fall) -> Float { return (rise + (fall * 1.9)); }, rise, fall))) {
      return {kern::formula([](Float t, Float rise, Float fall) -> Float { return ((-0.6) * (1.0 - kern::math::div(((t - rise) - fall), (fall * 0.9)))); }, t, rise, fall)};
    }
    return {0.0};
  }

  struct shotResult { List<Float> Result = List<Float>{}; };
  shotResult shot(Int rate = 0, Int v = 0) {
    Float duration = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made = List<Float>{};
    List<Float> dry = List<Float>{};
    List<Float> made_2 = List<Float>{};
    combat_audio_Random random = combat_audio_Random{0};
    Float lpA = 0.0;
    Float lpB = 0.0;
    Float phase = 0.0;
    Float rise = 0.0;
    Float fall = 0.0;
    Float t = 0.0;
    Float white = 0.0;
    Float nwave = 0.0;
    Float blast = 0.0;
    Float thump = 0.0;
    Float u = 0.0;
    Float bolt = 0.0;
    Float chosen = 0.0;
    Fixed<Float_2_row, 3> taps = Fixed<Float_2_row, 3>{};
    Int k = 0;
    Int n = 0;
    Int was = 0;
    Bool go = false;
    Float lp = 0.0;
    Int n_2 = 0;
    duration = kern::formula([](Float v) -> Float { return (0.2 + (v * 0.01)); }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = made;
    made_2 = List<Float>{};
    list_resize(made_2, list_length<Float>(data), 0.0);
    dry = made_2;
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(5905421.0 + kern::math::mul32(v, 4049.0)); }, kern::to<Float>(v)))).Result;
    lpA = 0.0;
    lpB = 0.0;
    phase = 0.0;
    rise = 0.00024;
    fall = kern::formula([](Float v) -> Float { return (0.0009 + (v * 5e-05)); }, kern::to<Float>(v));
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float>(data))))); it12.next();) {
      t = kern::formula([](Float index, Float rate) -> Float { return kern::math::div((index < 0.0 ? index + 18446744073709551616.0 : index), rate); }, kern::to<Float>(n12.index), kern::to<Float>(rate));
      n13 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return ((the_result * 2.0) - 1.0); }, n13.Result);
      nwave = nwave_at(t, rise, fall).Result;
      lpA += kern::formula([](Float white, Float lpA) -> Float { return (0.6 * (white - lpA)); }, white, lpA);
      lpB += kern::formula([](Float lpA, Float lpB) -> Float { return (0.25 * (lpA - lpB)); }, lpA, lpB);
      blast = kern::formula([](Float lpA, Float lpB, Float white, Float t) -> Float { return ((((lpA - lpB) * 0.9) + ((white - lpA) * 0.5)) * kern::math::exp(kern::math::div((-t), 0.008))); }, lpA, lpB, white, t);
      phase += kern::formula([](Float tAU, Float t, Float rate) -> Float { return kern::math::div((tAU * ((185.0 * kern::math::exp(((-t) * 30.0))) + 70.0)), rate); }, TAU, t, kern::to<Float>(rate));
      thump = kern::formula([](Float phase, Float t) -> Float { return ((kern::math::sin(phase) * kern::math::exp(kern::math::div((-t), 0.02))) * (1.0 - kern::math::exp(kern::math::div((-t), 0.0008)))); }, phase, t);
      u = kern::formula([](Float t, Float v) -> Float { return ((t - 0.017) - (v * 0.001)); }, t, kern::to<Float>(v));
      if (logic_and(math_compare<">">(u, 0.0), math_compare<"<">(u, 0.012))) {
        n15 = Random_call(random);
        chosen = kern::formula([](Float tAU, Float u, Float the_result) -> Float { return (((kern::math::sin(((tAU * 2700.0) * u)) + (0.5 * ((the_result * 2.0) - 1.0))) * kern::math::exp(kern::math::div((-u), 0.003))) * 0.25); }, TAU, u, n15.Result);
      } else {
        chosen = 0.0;
      }
      bolt = kern::formula([](Float chosen) -> Float { return chosen; }, chosen);
      dry[n12.index] = kern::formula([](Float nwave, Float blast, Float thump, Float bolt) -> Float { return ((((nwave * 1.2) + (blast * 0.9)) + (thump * 0.7)) + bolt); }, nwave, blast, thump, bolt);
    }
    taps = list_make<Float_2_row, "3">(struct_make<Float_2_row>(list_make<Float, "2">(0.011, 0.32)), struct_make<Float_2_row>(list_make<Float, "2">(0.019, 0.22)), struct_make<Float_2_row>(list_make<Float, "2">(0.031, 0.14)));
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_2_row>(taps))))); it16.next();) {
      k = kern::to<Int>(kern::formula([](Float a, Float v, Float rate) -> Float { return kern::math::trunc(((a + (v * 0.0006)) * rate)); }, taps[n16.index].items[0], kern::to<Float>(v), kern::to<Float>(rate)));
      n = list_length<Float>(data);
      go = true;
      while (loop(go)) {
        was = n;
        n = kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n)));
        if (math_compare<">">(kern::formula([](Float k, Float was) -> Float { return ((k < 0.0) != (was < 0.0) ? (was < 0.0) : (k < was)); }, kern::to<Float>(k), kern::to<Float>(was)), 0.5)) {
          data[n] = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (a + (a_2 * a_3)); }, data[n], dry[kern::to<Int>(kern::formula([](Float n, Float k) -> Float { return (n - k); }, kern::to<Float>(n), kern::to<Float>(k)))], taps[n16.index].items[1]);
        } else {
          go = false;
        }
      }
    }
    lp = 0.0;
    n_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float n_2, Float a) -> Float { return ((n_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (n_2 < a)); }, kern::to<Float>(n_2), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      lp += kern::formula([](Float a, Float lp) -> Float { return (0.5 * (a - lp)); }, data[n_2], lp);
      data[n_2] = kern::formula([](Float a, Float lp) -> Float { return kern::math::div(kern::math::exp(2.0 * ((a + (lp * 0.8)) * 2.1)) - 1.0, kern::math::exp(2.0 * ((a + (lp * 0.8)) * 2.1)) + 1.0); }, dry[n_2], lp);
      n_2 += 1;
    }
    n17 = finish(data, 0.95);
    return {n17.Result};
  }

  struct pistolResult { List<Float> Result = List<Float>{}; };
  pistolResult pistol(Int rate = 0, Int v = 0) {
    Float duration = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made = List<Float>{};
    List<Float> dry = List<Float>{};
    List<Float> made_2 = List<Float>{};
    combat_audio_Random random = combat_audio_Random{0};
    Float rise = 0.0;
    Float fall = 0.0;
    Float hp = 0.0;
    Float lpA = 0.0;
    Float lpB = 0.0;
    Float tail = 0.0;
    Float phase = 0.0;
    Float ring2 = 0.0;
    Fixed<Float_4_row, 3> mech = Fixed<Float_4_row, 3>{};
    Float t = 0.0;
    Float white = 0.0;
    Float nwave = 0.0;
    Float the_band = 0.0;
    Float blast = 0.0;
    Float f = 0.0;
    Float thump = 0.0;
    Float boom = 0.0;
    Float snap = 0.0;
    Float chosen = 0.0;
    Float m = 0.0;
    Float u = 0.0;
    Fixed<Float_2_row, 5> taps = Fixed<Float_2_row, 5>{};
    Fixed<Int, 5> d = Fixed<Int, 5>{};
    Int n = 0;
    Float t_2 = 0.0;
    Float val = 0.0;
    Float diffuse = 0.0;
    Float lp = 0.0;
    Int n_2 = 0;
    duration = kern::formula([](Float v) -> Float { return (0.58 + (v * 0.012)); }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = made;
    made_2 = List<Float>{};
    list_resize(made_2, list_length<Float>(data), 0.0);
    dry = made_2;
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(6406311.0 + kern::math::mul32(v, 7919.0)); }, kern::to<Float>(v)))).Result;
    rise = kern::formula([](Float v) -> Float { return (0.00028 + (v * 2e-05)); }, kern::to<Float>(v));
    fall = kern::formula([](Float v) -> Float { return (0.00105 + (v * 6e-05)); }, kern::to<Float>(v));
    hp = 0.0;
    lpA = 0.0;
    lpB = 0.0;
    tail = 0.0;
    phase = 0.0;
    ring2 = 0.0;
    mech = list_make<Float_4_row, "3">(struct_make<Float_4_row>(list_make<Float, "4">(kern::formula([](Float v) -> Float { return (0.0215 + (v * 0.0011)); }, kern::to<Float>(v)), 2450.0, 3810.0, 1.0)), struct_make<Float_4_row>(list_make<Float, "4">(kern::formula([](Float v) -> Float { return (0.0452 + (v * 0.0017)); }, kern::to<Float>(v)), 2080.0, 3260.0, 0.8)), struct_make<Float_4_row>(list_make<Float, "4">(kern::formula([](Float v) -> Float { return (0.0705 + (v * 0.0013)); }, kern::to<Float>(v)), 2900.0, 4400.0, 0.35)));
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float>(data))))); it18.next();) {
      t = kern::formula([](Float index, Float rate) -> Float { return kern::math::div((index < 0.0 ? index + 18446744073709551616.0 : index), rate); }, kern::to<Float>(n18.index), kern::to<Float>(rate));
      n19 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return ((the_result * 2.0) - 1.0); }, n19.Result);
      nwave = nwave_at(t, rise, fall).Result;
      lpA += kern::formula([](Float white, Float lpA) -> Float { return (0.55 * (white - lpA)); }, white, lpA);
      hp = kern::formula([](Float white, Float lpA) -> Float { return (white - lpA); }, white, lpA);
      lpB += kern::formula([](Float lpA, Float lpB) -> Float { return (0.22 * (lpA - lpB)); }, lpA, lpB);
      the_band = kern::formula([](Float lpA, Float lpB) -> Float { return (lpA - lpB); }, lpA, lpB);
      blast = kern::formula([](Float the_band, Float hp, Float t) -> Float { return (((the_band * 0.9) + (hp * 0.55)) * kern::math::exp(kern::math::div((-t), 0.0105))); }, the_band, hp, t);
      f = kern::formula([](Float t) -> Float { return ((185.0 * kern::math::exp(((-t) * 22.0))) + 50.0); }, t);
      phase += kern::formula([](Float tAU, Float f, Float rate) -> Float { return kern::math::div((tAU * f), rate); }, TAU, f, kern::to<Float>(rate));
      thump = kern::formula([](Float phase, Float t) -> Float { return ((kern::math::sin(phase) * kern::math::exp(kern::math::div((-t), 0.042))) * (1.0 - kern::math::exp(kern::math::div((-t), 0.0009)))); }, phase, t);
      boom = kern::formula([](Float tAU, Float t) -> Float { return ((kern::math::sin(((tAU * 44.0) * t)) * kern::math::exp(kern::math::div((-t), 0.11))) * (1.0 - kern::math::exp(kern::math::div((-t), 0.004)))); }, TAU, t);
      if (math_compare<"<">(t, 0.0018)) {
        n21 = Random_call(random);
        chosen = kern::formula([](Float the_result, Float t) -> Float { return (((the_result * 2.0) - 1.0) * (1.0 - kern::math::div(t, 0.0018))); }, n21.Result, t);
      } else {
        chosen = 0.0;
      }
      snap = kern::formula([](Float chosen) -> Float { return chosen; }, chosen);
      m = 0.0;
      for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_4_row>(mech))))); it22.next();) {
        u = kern::formula([](Float t, Float a) -> Float { return (t - a); }, t, mech[n22.index].items[0]);
        if (logic_and(math_compare<">=">(u, 0.0), math_compare<"<">(u, 0.02))) {
          n23 = Random_call(random);
          m += kern::formula([](Float a, Float tAU, Float a_2, Float u, Float a_3, Float the_result) -> Float { return ((a * ((kern::math::sin(((tAU * a_2) * u)) + (0.6 * kern::math::sin(((tAU * a_3) * u)))) + (0.4 * ((the_result * 2.0) - 1.0)))) * kern::math::exp(kern::math::div((-u), 0.0042))); }, mech[n22.index].items[3], TAU, mech[n22.index].items[1], u, mech[n22.index].items[2], n23.Result);
        }
      }
      dry[n18.index] = kern::formula([](Float nwave, Float snap, Float blast, Float thump, Float boom, Float m) -> Float { return ((((((nwave * 1.3) + (snap * 0.7)) + (blast * 1.0)) + (thump * 1.15)) + (boom * 0.38)) + (m * 0.36)); }, nwave, snap, blast, thump, boom, m);
    }
    taps = list_make<Float_2_row, "5">(struct_make<Float_2_row>(list_make<Float, "2">(0.0142, 0.42)), struct_make<Float_2_row>(list_make<Float, "2">(0.0235, 0.3)), struct_make<Float_2_row>(list_make<Float, "2">(0.0371, 0.24)), struct_make<Float_2_row>(list_make<Float, "2">(0.0516, 0.17)), struct_make<Float_2_row>(list_make<Float, "2">(0.0784, 0.11)));
    d = List<Int>{};
    for (auto it24 = flow_for(n24, 0, 4); it24.next();) {
      d[n24.index] = kern::to<Int>(kern::formula([](Float a, Float v, Float rate) -> Float { return kern::math::trunc(((a + (v * 0.0007)) * rate)); }, taps[n24.index].items[0], kern::to<Float>(v), kern::to<Float>(rate)));
    }
    n = 0;
    while (loop(math_compare<">">(kern::formula([](Float n, Float a) -> Float { return ((n < 0.0) != (a < 0.0) ? (a < 0.0) : (n < a)); }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t_2 = kern::formula([](Float n, Float rate) -> Float { return kern::math::div((n < 0.0 ? n + 18446744073709551616.0 : n), rate); }, kern::to<Float>(n), kern::to<Float>(rate));
      val = dry[n];
      for (auto it25 = flow_for(n25, 0, 4); it25.next();) {
        if (math_compare<">">(kern::formula([](Float n, Float a) -> Float { return (!((n < 0.0) != (a < 0.0) ? (a < 0.0) : (n < a))); }, kern::to<Float>(n), kern::to<Float>(d[n25.index])), 0.5)) {
          val += kern::formula([](Float a, Float a_2) -> Float { return ((a * a_2) * 0.8); }, taps[n25.index].items[1], dry[kern::to<Int>(kern::formula([](Float n, Float a) -> Float { return (n - a); }, kern::to<Float>(n), kern::to<Float>(d[n25.index])))]);
        }
      }
      n26 = Random_call(random);
      ring2 += kern::formula([](Float the_result, Float ring2) -> Float { return (0.08 * (((the_result * 2.0) - 1.0) - ring2)); }, n26.Result, ring2);
      tail += kern::formula([](Float ring2, Float tail) -> Float { return (0.15 * (ring2 - tail)); }, ring2, tail);
      diffuse = kern::formula([](Float t_2, Float tail) -> Float { return ((t_2 > 0.012) ? ((tail * 0.42) * kern::math::exp(kern::math::div((-(t_2 - 0.012)), 0.16))) : 0.0); }, t_2, tail);
      data[n] = kern::formula([](Float val, Float diffuse) -> Float { return (val + diffuse); }, val, diffuse);
      n += 1;
    }
    lp = 0.0;
    n_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float n_2, Float a) -> Float { return ((n_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (n_2 < a)); }, kern::to<Float>(n_2), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      lp += kern::formula([](Float a, Float lp) -> Float { return (0.45 * (a - lp)); }, data[n_2], lp);
      data[n_2] = kern::formula([](Float a, Float lp) -> Float { return (a + (lp - (a * 0.45))); }, dry[n_2], lp);
      n_2 += 1;
    }
    for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(data))))); it27.next();) {
      data[n27.index] = kern::formula([](Float a) -> Float { return kern::math::div(kern::math::exp(2.0 * (a * 2.6)) - 1.0, kern::math::exp(2.0 * (a * 2.6)) + 1.0); }, data[n27.index]);
    }
    n28 = finish(data, 0.99);
    return {n28.Result};
  }

  struct hitResult { List<Float> Result = List<Float>{}; };
  hitResult hit(Int rate = 0, Int v = 0) {
    Float duration = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made = List<Float>{};
    combat_audio_Random random = combat_audio_Random{0};
    Int delay = 0;
    Float low = 0.0;
    Float phase = 0.0;
    Int i = 0;
    Float t = 0.0;
    Float white = 0.0;
    Float surface = 0.0;
    Float frequency = 0.0;
    Float thud = 0.0;
    Float slap = 0.0;
    duration = kern::formula([](Float v) -> Float { return (0.105 + (v * 0.009)); }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = made;
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(9555439.0 + kern::math::mul32(v, 733.0)); }, kern::to<Float>(v)))).Result;
    delay = kern::to<Int>(kern::formula([](Float rate, Float v) -> Float { return kern::math::trunc((rate * (0.008 + (v * 0.001)))); }, kern::to<Float>(rate), kern::to<Float>(v)));
    low = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n29 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return ((the_result * 2.0) - 1.0); }, n29.Result);
      low += kern::formula([](Float white, Float low) -> Float { return (0.2 * (white - low)); }, white, low);
      surface = kern::formula([](Float white, Float low, Float t, Float v) -> Float { return (((white * 0.35) + (low * 0.65)) * kern::math::exp(((-t) * (37.0 - (v * 2.0))))); }, white, low, t, kern::to<Float>(v));
      frequency = kern::formula([](Float v, Float t) -> Float { return ((kern::math::wrap32(112.0 + kern::math::mul32(v, 9.0)) * kern::math::exp(((-t) * 13.0))) + 46.0); }, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float frequency, Float rate) -> Float { return kern::math::div((tAU * frequency), rate); }, TAU, frequency, kern::to<Float>(rate));
      thud = kern::formula([](Float phase, Float t) -> Float { return (kern::math::sin(phase) * kern::math::exp(((-t) * 30.0))); }, phase, t);
      slap = kern::formula([](Float i, Float delay, Float a) -> Float { return ((i >= delay) ? (a * 0.16) : 0.0); }, kern::to<Float>(i), kern::to<Float>(delay), data[kern::to<Int>(kern::formula([](Float i, Float delay) -> Float { return (i - delay); }, kern::to<Float>(i), kern::to<Float>(delay)))]);
      data[i] = kern::formula([](Float surface, Float thud, Float slap) -> Float { return (((surface * 0.78) + (thud * 0.58)) + slap); }, surface, thud, slap);
      i += 1;
    }
    n30 = finish(data, 0.82);
    return {n30.Result};
  }

  struct crackResult { List<Float> Result = List<Float>{}; };
  crackResult crack(Int rate = 0, Int v = 0) {
    Float duration = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made = List<Float>{};
    combat_audio_Random random = combat_audio_Random{0};
    Float low = 0.0;
    Float phase = 0.0;
    Int i = 0;
    Float t = 0.0;
    Float white = 0.0;
    Float snap = 0.0;
    Float frequency = 0.0;
    Float wood = 0.0;
    duration = kern::formula([](Float v) -> Float { return (0.082 + (v * 0.006)); }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = made;
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(12659399.0 + kern::math::mul32(v, 991.0)); }, kern::to<Float>(v)))).Result;
    low = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n31 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return ((the_result * 2.0) - 1.0); }, n31.Result);
      low += kern::formula([](Float white, Float low) -> Float { return (0.08 * (white - low)); }, white, low);
      snap = kern::formula([](Float white, Float low, Float t, Float v) -> Float { return ((white - low) * kern::math::exp(((-t) * (92.0 - (v * 4.0))))); }, white, low, t, kern::to<Float>(v));
      frequency = kern::formula([](Float v, Float t) -> Float { return ((kern::math::wrap32(760.0 + kern::math::mul32(v, 85.0)) * kern::math::exp(((-t) * 24.0))) + kern::math::wrap32(135.0 + kern::math::mul32(v, 13.0))); }, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float frequency, Float rate) -> Float { return kern::math::div((tAU * frequency), rate); }, TAU, frequency, kern::to<Float>(rate));
      wood = kern::formula([](Float phase, Float t, Float v) -> Float { return ((kern::math::sin(phase) + (0.28 * kern::math::sin(((phase * 2.07) + 0.4)))) * kern::math::exp(((-t) * (42.0 - (v * 2.0))))); }, phase, t, kern::to<Float>(v));
      data[i] = kern::formula([](Float snap, Float wood) -> Float { return ((snap * 0.82) + (wood * 0.52)); }, snap, wood);
      i += 1;
    }
    n32 = finish(data, 0.84);
    return {n32.Result};
  }

  struct damageResult { List<Float> Result = List<Float>{}; };
  damageResult damage(Int rate = 0, Int v = 0) {
    Float duration = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made = List<Float>{};
    combat_audio_Random random = combat_audio_Random{0};
    Float low = 0.0;
    Float phase = 0.0;
    Int i = 0;
    Float t = 0.0;
    Float white = 0.0;
    Float frequency = 0.0;
    Float pressure = 0.0;
    Float rush = 0.0;
    duration = kern::formula([](Float v) -> Float { return (0.21 + (v * 0.035)); }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = made;
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(8048817.0 + kern::math::mul32(v, 811.0)); }, kern::to<Float>(v)))).Result;
    low = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n33 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return ((the_result * 2.0) - 1.0); }, n33.Result);
      low += kern::formula([](Float white, Float low) -> Float { return (0.065 * (white - low)); }, white, low);
      frequency = kern::formula([](Float v, Float t) -> Float { return ((kern::math::wrap32(92.0 + kern::math::mul32(v, 12.0)) * kern::math::exp(((-t) * 8.0))) + 31.0); }, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float frequency, Float rate) -> Float { return kern::math::div((tAU * frequency), rate); }, TAU, frequency, kern::to<Float>(rate));
      pressure = kern::formula([](Float phase, Float t) -> Float { return (kern::math::sin((phase + (0.3 * kern::math::sin((phase * 0.5))))) * kern::math::exp(((-t) * 13.0))); }, phase, t);
      rush = kern::formula([](Float low, Float t) -> Float { return (low * kern::math::exp(((-t) * 8.0))); }, low, t);
      data[i] = kern::formula([](Float pressure, Float rush) -> Float { return ((pressure * 0.7) + (rush * 0.76)); }, pressure, rush);
      i += 1;
    }
    n34 = finish(data, 0.86);
    return {n34.Result};
  }

  struct gaussianResult { Float Result = 0.0; };
  KERN_PURE gaussianResult gaussian(Float value = 0.0, Float center = 0.0, Float width = 0.0) {
    Float x = 0.0;
    x = kern::formula([](Float value, Float center, Float width) -> Float { return kern::math::div((value - center), width); }, value, center, width);
    return {kern::formula([](Float x) -> Float { return kern::math::exp((((-0.5) * x) * x)); }, x)};
  }

  struct screamResult { List<Float> Result = List<Float>{}; };
  screamResult scream(Int rate = 0, Int v = 0) {
    Float duration = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made = List<Float>{};
    combat_audio_Random random = combat_audio_Random{0};
    Fixed<Float, 4> bases = Fixed<Float, 4>{};
    Fixed<Float_3_row, 4> formants = Fixed<Float_3_row, 4>{};
    Float base = 0.0;
    Int which = 0;
    Float phase = 0.0;
    Float breath = 0.0;
    Int i = 0;
    Float t = 0.0;
    Float p = 0.0;
    Float rise = 0.0;
    Float fall = 0.0;
    Float f0 = 0.0;
    Float voice = 0.0;
    Float hz = 0.0;
    Float shape = 0.0;
    Float white = 0.0;
    Float air = 0.0;
    Float amp = 0.0;
    duration = kern::formula([](Float v) -> Float { return (0.56 + (v * 0.055)); }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = made;
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(12648430.0 + kern::math::mul32(v, 1297.0)); }, kern::to<Float>(v)))).Result;
    bases = list_make<Float, "4">(238.0, 276.0, 218.0, 258.0);
    formants = list_make<Float_3_row, "4">(struct_make<Float_3_row>(list_make<Float, "3">(760.0, 1280.0, 2480.0)), struct_make<Float_3_row>(list_make<Float, "3">(870.0, 1430.0, 2670.0)), struct_make<Float_3_row>(list_make<Float, "3">(690.0, 1130.0, 2320.0)), struct_make<Float_3_row>(list_make<Float, "3">(820.0, 1510.0, 2580.0)));
    base = bases[kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 4.0); }, kern::to<Float>(v)))];
    which = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 4.0); }, kern::to<Float>(v)));
    phase = 0.0;
    breath = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      p = kern::formula([](Float t, Float duration) -> Float { return kern::math::div(t, duration); }, t, duration);
      rise = kern::formula([](Float p) -> Float { return kern::math::sin(kern::math::div((kern::math::min(1.0, kern::math::div(p, 0.34)) * 3.141592653589793), 2.0)); }, p);
      fall = kern::formula([](Float p) -> Float { return kern::math::max(0.0, kern::math::div((p - 0.42), 0.58)); }, p);
      f0 = kern::formula([](Float base, Float rise, Float fall, Float tAU, Float v, Float t) -> Float { return ((base * ((0.82 + (0.34 * rise)) - (0.35 * fall))) * (1.0 + (0.018 * kern::math::sin(((tAU * (5.1 + (v * 0.35))) * t))))); }, base, rise, fall, TAU, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float f0, Float rate) -> Float { return kern::math::div((tAU * f0), rate); }, TAU, f0, kern::to<Float>(rate));
      voice = 0.0;
      for (auto it35 = flow_for(n35, 1, 12); it35.next();) {
        hz = kern::formula([](Float f0, Float index) -> Float { return (f0 * index); }, f0, kern::to<Float>(n35.index));
        shape = kern::formula([](Float the_result, Float the_result_2, Float the_result_3) -> Float { return (((0.18 + (1.35 * the_result)) + (1.02 * the_result_2)) + (0.55 * the_result_3)); }, gaussian(hz, formants[which].items[0], 260.0).Result, gaussian(hz, formants[which].items[1], 390.0).Result, gaussian(hz, formants[which].items[2], 620.0).Result);
        voice += kern::formula([](Float phase, Float index, Float v, Float shape) -> Float { return kern::math::div((kern::math::sin(((phase * index) + ((index * 0.31) * v))) * shape), kern::math::pow(index, 0.82)); }, phase, kern::to<Float>(n35.index), kern::to<Float>(v), shape);
      }
      n36 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return ((the_result * 2.0) - 1.0); }, n36.Result);
      breath += kern::formula([](Float white, Float breath) -> Float { return (0.14 * (white - breath)); }, white, breath);
      air = kern::formula([](Float white, Float breath, Float rise) -> Float { return ((white - breath) * (0.18 + (0.22 * rise))); }, white, breath, rise);
      amp = kern::formula([](Float the_result, Float p) -> Float { return (the_result * (0.72 + (0.28 * kern::math::sin((3.141592653589793 * p))))); }, envelope(t, 0.024, 0.15, duration).Result, p);
      data[i] = kern::formula([](Float voice, Float air, Float amp) -> Float { return (((voice * 0.3) + air) * amp); }, voice, air, amp);
      i += 1;
    }
    n37 = finish(data, 0.9);
    return {n37.Result};
  }

  struct explosionResult { List<Float> Result = List<Float>{}; };
  explosionResult explosion(Int rate = 0, Int v = 0) {
    Float duration = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made = List<Float>{};
    combat_audio_Random random = combat_audio_Random{0};
    Float rumble = 0.0;
    Float phase = 0.0;
    Int i = 0;
    Float t = 0.0;
    Float white = 0.0;
    Float crk = 0.0;
    Float frequency = 0.0;
    Float boom = 0.0;
    Float tail = 0.0;
    duration = kern::formula([](Float v) -> Float { return (0.76 + (v * 0.08)); }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = made;
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(720983.0 + kern::math::mul32(v, 1901.0)); }, kern::to<Float>(v)))).Result;
    rumble = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n38 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return ((the_result * 2.0) - 1.0); }, n38.Result);
      rumble += kern::formula([](Float white, Float rumble) -> Float { return (0.035 * (white - rumble)); }, white, rumble);
      crk = kern::formula([](Float white, Float rumble, Float t) -> Float { return ((white - rumble) * kern::math::exp(((-t) * 24.0))); }, white, rumble, t);
      frequency = kern::formula([](Float v, Float t) -> Float { return ((kern::math::wrap32(88.0 + kern::math::mul32(v, 13.0)) * kern::math::exp(((-t) * 5.7))) + 22.0); }, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float frequency, Float rate) -> Float { return kern::math::div((tAU * frequency), rate); }, TAU, frequency, kern::to<Float>(rate));
      boom = kern::formula([](Float phase, Float t) -> Float { return (kern::math::sin((phase + (0.45 * kern::math::sin((phase * 0.37))))) * kern::math::exp(((-t) * 4.7))); }, phase, t);
      tail = kern::formula([](Float rumble, Float t) -> Float { return (rumble * kern::math::exp(((-t) * 3.1))); }, rumble, t);
      data[i] = kern::formula([](Float crk, Float boom, Float tail) -> Float { return (((crk * 0.5) + (boom * 0.82)) + (tail * 1.1)); }, crk, boom, tail);
      i += 1;
    }
    n39 = finish(data, 0.94);
    return {n39.Result};
  }

  struct stepResult { List<Float> Result = List<Float>{}; };
  stepResult step(Int rate = 0, Int v = 0) {
    Float duration = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made = List<Float>{};
    combat_audio_Random random = combat_audio_Random{0};
    Float low = 0.0;
    Float phase = 0.0;
    Int i = 0;
    Float t = 0.0;
    Float white = 0.0;
    duration = kern::formula([](Float v) -> Float { return (0.075 + (v * 0.008)); }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = made;
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(22505.0 + kern::math::mul32(v, 431.0)); }, kern::to<Float>(v)))).Result;
    low = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n40 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return ((the_result * 2.0) - 1.0); }, n40.Result);
      low += kern::formula([](Float white, Float low) -> Float { return (0.09 * (white - low)); }, white, low);
      phase += kern::formula([](Float tAU, Float v, Float t, Float rate) -> Float { return kern::math::div(((tAU * kern::math::wrap32(78.0 + kern::math::mul32(v, 9.0))) * kern::math::exp(((-t) * 12.0))), rate); }, TAU, kern::to<Float>(v), t, kern::to<Float>(rate));
      data[i] = kern::formula([](Float low, Float phase, Float t) -> Float { return (((low * 0.92) + (kern::math::sin(phase) * 0.36)) * kern::math::exp(((-t) * 38.0))); }, low, phase, t);
      i += 1;
    }
    n41 = finish(data, 0.72);
    return {n41.Result};
  }

  struct bounceResult { List<Float> Result = List<Float>{}; };
  bounceResult bounce(Int rate = 0, Int v = 0) {
    Float duration = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made = List<Float>{};
    combat_audio_Random random = combat_audio_Random{0};
    Float lp = 0.0;
    Float phase = 0.0;
    Float ring = 0.0;
    Float f0 = 0.0;
    Float fr = 0.0;
    Int i = 0;
    Float t = 0.0;
    Float white = 0.0;
    Float click = 0.0;
    Float knock = 0.0;
    Float shell = 0.0;
    Float grit = 0.0;
    duration = kern::formula([](Float v) -> Float { return (0.16 + (v * 0.01)); }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = made;
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(53444.0 + kern::math::mul32(v, 977.0)); }, kern::to<Float>(v)))).Result;
    lp = 0.0;
    phase = 0.0;
    ring = 0.0;
    f0 = kern::formula([](Float v) -> Float { return kern::math::wrap32(150.0 + kern::math::mul32(v, 18.0)); }, kern::to<Float>(v));
    fr = kern::formula([](Float v) -> Float { return kern::math::wrap32(1850.0 + kern::math::mul32(v, 240.0)); }, kern::to<Float>(v));
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n42 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return ((the_result * 2.0) - 1.0); }, n42.Result);
      lp += kern::formula([](Float white, Float lp) -> Float { return (0.35 * (white - lp)); }, white, lp);
      click = kern::formula([](Float white, Float lp, Float t) -> Float { return ((white - lp) * kern::math::exp(kern::math::div((-t), 0.0022))); }, white, lp, t);
      phase += kern::formula([](Float tAU, Float f0, Float t, Float rate) -> Float { return kern::math::div((tAU * (f0 * (1.0 + (0.6 * kern::math::exp(((-t) * 90.0)))))), rate); }, TAU, f0, t, kern::to<Float>(rate));
      knock = kern::formula([](Float phase, Float t) -> Float { return ((kern::math::sin(phase) * kern::math::exp(kern::math::div((-t), 0.026))) * (1.0 - kern::math::exp(kern::math::div((-t), 0.0006)))); }, phase, t);
      ring += kern::formula([](Float tAU, Float fr, Float rate) -> Float { return kern::math::div((tAU * fr), rate); }, TAU, fr, kern::to<Float>(rate));
      shell = kern::formula([](Float ring, Float t) -> Float { return ((kern::math::sin(ring) * kern::math::exp(kern::math::div((-t), 0.012))) * 0.35); }, ring, t);
      n43 = Random_call(random);
      grit = kern::formula([](Float lp, Float t, Float the_result) -> Float { return (((lp * kern::math::exp(kern::math::div((-t), 0.05))) * 0.18) * ((the_result < 0.04) ? 2.5 : 1.0)); }, lp, t, n43.Result);
      data[i] = kern::formula([](Float click, Float knock, Float shell, Float grit) -> Float { return ((((click * 0.9) + (knock * 1.1)) + shell) + grit); }, click, knock, shell, grit);
      i += 1;
    }
    n44 = finish(data, 0.82);
    return {n44.Result};
  }

  struct note_hzResult { Float Result = 0.0; };
  KERN_PURE note_hzResult note_hz(Int n = 0) {
    return {kern::formula([](Float n) -> Float { return (440.0 * kern::math::pow(2.0, kern::math::div((n - 69.0), 12.0))); }, kern::to<Float>(n))};
  }

  struct cueResult { List<Float> Result = List<Float>{}; };
  cueResult cue(Int rate = 0, List<Int> notes = List<Int>{}, List<Float> lengths = List<Float>{}, Float cutoff = 0.0, Float echo = 0.0, Float level = 0.0) {
    Float total = 0.0;
    Float l = 0.0;
    List<Float> dry = List<Float>{};
    List<Float> made = List<Float>{};
    Float t0 = 0.0;
    Bool skip = false;
    Float len = 0.0;
    Float f = 0.0;
    Float dur = 0.0;
    Int start = 0;
    Int count = 0;
    Float phase = 0.0;
    Float sub = 0.0;
    Float lp = 0.0;
    Int i = 0;
    Float t = 0.0;
    Float env = 0.0;
    Float pulse = 0.0;
    Float alpha = 0.0;
    List<Float> data = List<Float>{};
    List<Float> made_2 = List<Float>{};
    Int delay = 0;
    Int i_2 = 0;
    Float e1 = 0.0;
    Float e2 = 0.0;
    Int fade = 0;
    Int i_3 = 0;
    total = 0.0;
    for (auto it45 = flow_for(n45, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(lengths))))); it45.next();) {
      l = lengths[n45.index];
      total += l;
    }
    total = kern::formula([](Float total, Float kSixteenth) -> Float { return (total * kSixteenth); }, total, kSixteenth);
    made = List<Float>{};
    list_resize(made, samples(kern::formula([](Float total, Float kSixteenth) -> Float { return (total + (kSixteenth * 6.0)); }, total, kSixteenth), rate).Result, 0.0);
    dry = made;
    t0 = 0.0;
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int>(notes))))); it46.next();) {
      skip = false;
      len = kern::formula([](Float a, Float kSixteenth) -> Float { return (a * kSixteenth); }, lengths[n46.index], kSixteenth);
      if (math_compare<"<">(kern::to<Float>(notes[n46.index]), 0.0)) {
        t0 += len;
        skip = true;
      }
      if (logic_not(skip)) {
        f = note_hz(notes[n46.index]).Result;
        dur = kern::formula([](Float len) -> Float { return (len * 1.6); }, len);
        start = kern::to<Int>(kern::formula([](Float t0, Float rate) -> Float { return kern::math::trunc((t0 * rate)); }, t0, kern::to<Float>(rate)));
        count = kern::to<Int>(kern::formula([](Float dur, Float rate) -> Float { return kern::math::trunc((dur * rate)); }, dur, kern::to<Float>(rate)));
        phase = 0.0;
        sub = 0.0;
        lp = 0.0;
        i = 0;
        while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float count) -> Float { return ((i < 0.0) != (count < 0.0) ? (count < 0.0) : (i < count)); }, kern::to<Float>(i), kern::to<Float>(count)), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float start, Float i) -> Float { return (start + i); }, kern::to<Float>(start), kern::to<Float>(i))))), kern::to<Float>(list_length<Float>(dry))), 0.5)))) {
          t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div((i < 0.0 ? i + 18446744073709551616.0 : i), rate); }, kern::to<Float>(i), kern::to<Float>(rate));
          env = kern::formula([](Float t, Float len) -> Float { return (kern::math::min(1.0, kern::math::div(t, 0.006)) * kern::math::exp(kern::math::div((-t), (len * 0.75)))); }, t, len);
          phase += kern::formula([](Float f, Float rate) -> Float { return kern::math::div(f, rate); }, f, kern::to<Float>(rate));
          sub += kern::formula([](Float f, Float rate) -> Float { return kern::math::div((f * 0.5), rate); }, f, kern::to<Float>(rate));
          pulse = kern::formula([](Float phase) -> Float { return ((kern::math::mod(phase, 1.0) < 0.5) ? 1.0 : (-1.0)); }, phase);
          alpha = kern::formula([](Float tAU, Float cutoff, Float t, Float dur, Float rate) -> Float { return kern::math::min(1.0, kern::math::div(((tAU * cutoff) * (1.0 - kern::math::div((0.5 * t), dur))), rate)); }, TAU, cutoff, t, dur, kern::to<Float>(rate));
          lp += kern::formula([](Float alpha, Float pulse, Float lp) -> Float { return (alpha * (pulse - lp)); }, alpha, pulse, lp);
          dry[kern::to<Int>(kern::formula([](Float start, Float i) -> Float { return (start + i); }, kern::to<Float>(start), kern::to<Float>(i)))] = kern::formula([](Float a, Float lp, Float tAU, Float sub, Float env) -> Float { return (a + (((lp * 0.55) + (kern::math::sin((tAU * sub)) * 0.28)) * env)); }, dry[kern::to<Int>(kern::formula([](Float start, Float i) -> Float { return (start + i); }, kern::to<Float>(start), kern::to<Float>(i)))], lp, TAU, sub, env);
          i += 1;
        }
        t0 += len;
      }
    }
    made_2 = List<Float>{};
    list_resize(made_2, list_length<Float>(dry), 0.0);
    data = made_2;
    delay = kern::to<Int>(kern::formula([](Float kSixteenth, Float rate) -> Float { return kern::math::trunc(((kSixteenth * 3.0) * rate)); }, kSixteenth, kern::to<Float>(rate)));
    i_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float i_2, Float a) -> Float { return ((i_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_2 < a)); }, kern::to<Float>(i_2), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      e1 = kern::formula([](Float i_2, Float delay, Float a, Float echo) -> Float { return ((i_2 >= delay) ? (a * echo) : 0.0); }, kern::to<Float>(i_2), kern::to<Float>(delay), dry[kern::to<Int>(kern::formula([](Float i_2, Float delay) -> Float { return (i_2 - delay); }, kern::to<Float>(i_2), kern::to<Float>(delay)))], echo);
      e2 = kern::formula([](Float i_2, Float delay, Float a, Float echo) -> Float { return ((i_2 >= (2.0 * delay)) ? ((a * echo) * echo) : 0.0); }, kern::to<Float>(i_2), kern::to<Float>(delay), dry[kern::to<Int>(kern::formula([](Float i_2, Float delay) -> Float { return (i_2 - (2.0 * delay)); }, kern::to<Float>(i_2), kern::to<Float>(delay)))], echo);
      data[i_2] = kern::formula([](Float a, Float e1, Float e2) -> Float { return ((a + e1) + e2); }, dry[i_2], e1, e2);
      i_2 += 1;
    }
    fade = kern::to<Int>(kern::formula([](Float rate) -> Float { return kern::math::trunc((0.04 * rate)); }, kern::to<Float>(rate)));
    i_3 = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i_3, Float fade) -> Float { return ((i_3 < 0.0) != (fade < 0.0) ? (fade < 0.0) : (i_3 < fade)); }, kern::to<Float>(i_3), kern::to<Float>(fade)), 0.5), math_compare<">">(kern::formula([](Float i_3, Float a) -> Float { return ((i_3 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_3 < a)); }, kern::to<Float>(i_3), kern::to<Float>(list_length<Float>(data))), 0.5)))) {
      data[kern::to<Int>(kern::formula([](Float a, Float i_3) -> Float { return ((a - 1.0) - i_3); }, kern::to<Float>(list_length<Float>(data)), kern::to<Float>(i_3)))] = kern::formula([](Float a, Float i_3, Float fade) -> Float { return (a * kern::math::div((i_3 < 0.0 ? i_3 + 18446744073709551616.0 : i_3), (fade < 0.0 ? fade + 18446744073709551616.0 : fade))); }, data[kern::to<Int>(kern::formula([](Float a, Float i_3) -> Float { return ((a - 1.0) - i_3); }, kern::to<Float>(list_length<Float>(data)), kern::to<Float>(i_3)))], kern::to<Float>(i_3), kern::to<Float>(fade));
      i_3 += 1;
    }
    n47 = finish(data, level);
    return {n47.Result};
  }

  struct kind_nameResult { Text Result = ""; };
  KERN_PURE kind_nameResult kind_name(combat_audio_Kind k = combat_audio_Kind::shot) {
    switch (flow_switch_enum<combat_audio_Kind>(k)) {
      case combat_audio_Kind::shot: {
        return {"shot"};
      } break;
      case combat_audio_Kind::pistol: {
        return {"pistol"};
      } break;
      case combat_audio_Kind::hit: {
        return {"hit"};
      } break;
      case combat_audio_Kind::crack: {
        return {"crack"};
      } break;
      case combat_audio_Kind::damage: {
        return {"damage"};
      } break;
      case combat_audio_Kind::scream: {
        return {"scream"};
      } break;
      case combat_audio_Kind::explosion: {
        return {"explosion"};
      } break;
      case combat_audio_Kind::step: {
        return {"step"};
      } break;
      case combat_audio_Kind::bounce: {
        return {"bounce"};
      } break;
      case combat_audio_Kind::reward: {
        return {"reward"};
      } break;
      case combat_audio_Kind::fail: {
        return {"fail"};
      } break;
      case combat_audio_Kind::count: {
        return {""};
      } break;
    }
  }

  struct renderResult { List<Float> Result = List<Float>{}; };
  renderResult render(combat_audio_Kind k = combat_audio_Kind::shot, Int v = 0, Int rate = 0) {
    Int rate_ = 0;
    rate_ = rate;
    rate_ = kern::to<Int>(kern::formula([](Float rate_) -> Float { return ((rate_ < 8000.0) ? 8000.0 : rate_); }, kern::to<Float>(rate_)));
    switch (flow_switch_enum<combat_audio_Kind>(k)) {
      case combat_audio_Kind::shot: {
        n48 = shot(rate_, v);
        return {n48.Result};
      } break;
      case combat_audio_Kind::pistol: {
        n49 = pistol(rate_, v);
        return {n49.Result};
      } break;
      case combat_audio_Kind::hit: {
        n50 = hit(rate_, v);
        return {n50.Result};
      } break;
      case combat_audio_Kind::crack: {
        n51 = crack(rate_, v);
        return {n51.Result};
      } break;
      case combat_audio_Kind::damage: {
        n52 = damage(rate_, v);
        return {n52.Result};
      } break;
      case combat_audio_Kind::scream: {
        n53 = scream(rate_, v);
        return {n53.Result};
      } break;
      case combat_audio_Kind::explosion: {
        n54 = explosion(rate_, v);
        return {n54.Result};
      } break;
      case combat_audio_Kind::step: {
        n55 = step(rate_, v);
        return {n55.Result};
      } break;
      case combat_audio_Kind::bounce: {
        n56 = bounce(rate_, v);
        return {n56.Result};
      } break;
      case combat_audio_Kind::reward: {
        if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 3.0); }, kern::to<Float>(v))))), 0.0)) {
          n57 = cue(rate_, list_make<Int, "2">(70, 75), list_make<Float, "2">(1.0, 2.0), 2400.0, 0.3, 0.75);
          return {n57.Result};
        }
        if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 3.0); }, kern::to<Float>(v))))), 1.0)) {
          n58 = cue(rate_, list_make<Int, "4">(63, 66, 70, 75), list_make<Float, "4">(1.0, 1.0, 1.0, 2.0), 2800.0, 0.3, 0.75);
          return {n58.Result};
        }
        n59 = cue(rate_, list_make<Int, "6">(66, 70, 73, 78, -1, 82), list_make<Float, "6">(1.0, 1.0, 1.0, 1.0, 1.0, 3.0), 3200.0, 0.36, 0.75);
        return {n59.Result};
      } break;
      case combat_audio_Kind::fail: {
        if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 2.0); }, kern::to<Float>(v))))), 0.0)) {
          n60 = cue(rate_, list_make<Int, "3">(70, 69, 66), list_make<Float, "3">(1.0, 1.0, 3.0), 1400.0, 0.22, 0.7);
          return {n60.Result};
        }
        n61 = cue(rate_, list_make<Int, "3">(63, 58, 51), list_make<Float, "3">(1.0, 1.0, 4.0), 1100.0, 0.2, 0.72);
        return {n61.Result};
      } break;
      case combat_audio_Kind::count: {
        return {List<Float>{}};
      } break;
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  Random_callResult n13;
  Random_callResult n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  finishResult n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  Random_callResult n19;
  Random_callResult n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  Random_callResult n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  Random_callResult n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  finishResult n28;
  Random_callResult n29;
  finishResult n30;
  Random_callResult n31;
  finishResult n32;
  Random_callResult n33;
  finishResult n34;
  struct : kern::Node<35> { Int index = 0; } n35;
  Random_callResult n36;
  finishResult n37;
  Random_callResult n38;
  finishResult n39;
  Random_callResult n40;
  finishResult n41;
  Random_callResult n42;
  Random_callResult n43;
  finishResult n44;
  struct : kern::Node<45> { Int index = 0; } n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  finishResult n47;
  shotResult n48;
  pistolResult n49;
  hitResult n50;
  crackResult n51;
  damageResult n52;
  screamResult n53;
  explosionResult n54;
  stepResult n55;
  bounceResult n56;
  cueResult n57;
  cueResult n58;
  cueResult n59;
  cueResult n60;
  cueResult n61;
};
