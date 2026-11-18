struct game_gaps_cpp : virtual kern::Library, virtual engine_geo_mesh_data, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(Biquad) { Float b0 = 1.0; Float b1 = 0.0; Float b2 = 0.0; Float a1 = 0.0; Float a2 = 0.0; Float z1 = 0.0; Float z2 = 0.0; };
  KERN_ENUM(Osc, saw, square, triangle, sine);
  KERN_STRUCT(OscSpec) { Osc type = Osc::saw; Float cents = 0.0; Float gain = 0.0; };

  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct Biquad_runResult { Float Result = 0.0; };
  Biquad_runResult Biquad_run(Biquad& self, Float x = 0.0) {
    Float y = 0.0;
    y = kern::formula([](Float b0, Float x, Float z1) -> Float { return ((b0 * x) + z1); }, self.b0, x, self.z1);
    self.z1 = kern::formula([](Float b1, Float x, Float a1, Float y, Float z2) -> Float { return (((b1 * x) - (a1 * y)) + z2); }, self.b1, x, self.a1, y, self.z2);
    self.z2 = kern::formula([](Float b2, Float x, Float a2, Float y) -> Float { return ((b2 * x) - (a2 * y)); }, self.b2, x, self.a2, y);
    return {y};
  }

  struct Biquad_lowpassResult { Biquad Result = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE Biquad_lowpassResult Biquad_lowpass(Float f = 0.0, Float q = 0.0, Float rate = 0.0) {
    Float w = 0.0;
    Float c = 0.0;
    Float al = 0.0;
    Float a0 = 0.0;
    Biquad b = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    w = kern::formula([](Float kPi, Float f, Float rate) -> Float { return kern::math::div(((2.0 * kPi) * f), rate); }, kPi, f, rate);
    c = kern::formula([](Float w) -> Float { return kern::math::cos(w); }, w);
    al = kern::formula([](Float w, Float q) -> Float { return kern::math::div(kern::math::sin(w), (2.0 * q)); }, w, q);
    a0 = kern::formula([](Float al) -> Float { return (1.0 + al); }, al);
    b = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    b.b0 = kern::formula([](Float c, Float a0) -> Float { return kern::math::div(kern::math::div((1.0 - c), 2.0), a0); }, c, a0);
    b.b1 = kern::formula([](Float c, Float a0) -> Float { return kern::math::div((1.0 - c), a0); }, c, a0);
    b.b2 = b.b0;
    b.a1 = kern::formula([](Float c, Float a0) -> Float { return kern::math::div(((-2.0) * c), a0); }, c, a0);
    b.a2 = kern::formula([](Float al, Float a0) -> Float { return kern::math::div((1.0 - al), a0); }, al, a0);
    return {b};
  }

  struct Biquad_peakingResult { Biquad Result = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE Biquad_peakingResult Biquad_peaking(Float f = 0.0, Float q = 0.0, Float gain_db = 0.0, Float rate = 0.0) {
    Float A = 0.0;
    Float w = 0.0;
    Float c = 0.0;
    Float al = 0.0;
    Float a0 = 0.0;
    Biquad b = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    A = kern::formula([](Float gain_db) -> Float { return kern::math::pow(10.0, kern::math::div(gain_db, 40.0)); }, gain_db);
    w = kern::formula([](Float kPi, Float f, Float rate) -> Float { return kern::math::div(((2.0 * kPi) * f), rate); }, kPi, f, rate);
    c = kern::formula([](Float w) -> Float { return kern::math::cos(w); }, w);
    al = kern::formula([](Float w, Float q) -> Float { return kern::math::div(kern::math::sin(w), (2.0 * q)); }, w, q);
    a0 = kern::formula([](Float al, Float a) -> Float { return (1.0 + kern::math::div(al, a)); }, al, A);
    b = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    b.b0 = kern::formula([](Float al, Float a, Float a0) -> Float { return kern::math::div((1.0 + (al * a)), a0); }, al, A, a0);
    b.b1 = kern::formula([](Float c, Float a0) -> Float { return kern::math::div(((-2.0) * c), a0); }, c, a0);
    b.b2 = kern::formula([](Float al, Float a, Float a0) -> Float { return kern::math::div((1.0 - (al * a)), a0); }, al, A, a0);
    b.a1 = kern::formula([](Float c, Float a0) -> Float { return kern::math::div(((-2.0) * c), a0); }, c, a0);
    b.a2 = kern::formula([](Float al, Float a, Float a0) -> Float { return kern::math::div((1.0 - kern::math::div(al, a)), a0); }, al, A, a0);
    return {b};
  }

  struct siren_loopResult { List<Float> Result = List<Float>{}; };
  siren_loopResult siren_loop(Int rate = 0, Float f0 = 0.0, List<OscSpec> oscs = List<OscSpec>{}, Bool formant = false, Float cutoff = 0.0) {
    Float secs = 0.0;
    List<Float> out = List<Float>{};
    List<Float> made = List<Float>{};
    Biquad peak = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    Biquad lp = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    Biquad air = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    List<Float> phase = List<Float>{};
    List<Float> made_2 = List<Float>{};
    Int n = 0;
    Float s = 0.0;
    Int k = 0;
    Float f = 0.0;
    Float p = 0.0;
    Float v = 0.0;
    secs = kern::formula([](Float f0) -> Float { return kern::math::div(kern::math::round((f0 * 2.0)), f0); }, f0);
    made = List<Float>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float secs, Float rate) -> Float { return kern::math::trunc((secs * rate)); }, secs, kern::to<Float>(rate))), 0.0);
    out = made;
    peak = Biquad_peaking(1650.0, 0.9, 7.0, kern::to<Float>(rate)).Result;
    lp = Biquad_lowpass(4200.0, 0.4, kern::to<Float>(rate)).Result;
    air = Biquad_lowpass(cutoff, 0.5, kern::to<Float>(rate)).Result;
    made_2 = List<Float>{};
    list_resize(made_2, list_length<OscSpec>(oscs), 0.0);
    phase = made_2;
    n = 0;
    while (loop(math_compare<">">(kern::formula([](Float n, Float a) -> Float { return ((n < 0.0) != (a < 0.0) ? (a < 0.0) : (n < a)); }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(out))), 0.5))) {
      s = 0.0;
      k = 0;
      for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<OscSpec>(oscs))))); it10.next();) {
        f = kern::formula([](Float f0, Float a) -> Float { return (f0 * kern::math::pow(2.0, kern::math::div(a, 1200.0))); }, f0, oscs[n10.index].cents);
        phase[k] = kern::formula([](Float a, Float f, Float rate) -> Float { return (a + kern::math::div(f, rate)); }, phase[k], f, kern::to<Float>(rate));
        phase[k] = kern::formula([](Float a) -> Float { return (a - kern::math::floor(a)); }, phase[k]);
        p = phase[k];
        v = 0.0;
        switch (flow_switch_enum<Osc>(oscs[n10.index].type)) {
          case Osc::saw: {
            v = kern::formula([](Float p) -> Float { return ((2.0 * p) - 1.0); }, p);
          } break;
          case Osc::square: {
            v = kern::formula([](Float p) -> Float { return ((p < 0.5) ? 1.0 : (-1.0)); }, p);
          } break;
          case Osc::triangle: {
            v = kern::formula([](Float p) -> Float { return (1.0 - (4.0 * kern::math::abs((p - 0.5)))); }, p);
          } break;
          case Osc::sine: {
            v = kern::formula([](Float kPi, Float p) -> Float { return kern::math::sin(((2.0 * kPi) * p)); }, kPi, p);
          } break;
        }
        s += kern::formula([](Float v, Float a) -> Float { return (v * a); }, v, oscs[n10.index].gain);
        k += 1;
      }
      if (formant) {
        n11 = Biquad_run(peak, s);
        s = n11.Result;
      }
      n12 = Biquad_run(lp, s);
      s = n12.Result;
      if (math_compare<"<">(cutoff, 4000.0)) {
        n13 = Biquad_run(air, s);
        s = n13.Result;
      }
      out[n] = s;
      n += 1;
    }
    engine_audio_make_seamless(n14, out, rate, 0.05);
    out = n14.samples;
    engine_audio_normalize(n15, out, 0.8);
    out = n15.samples;
    return {out};
  }

  struct billboardResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE billboardResult billboard(Vector p = Vector{0.0, 0.0, 0.0}, Vector right = Vector{0.0, 0.0, 0.0}, Vector up = Vector{0.0, 0.0, 0.0}, Vector normal = Vector{0.0, 0.0, 0.0}, Float size = 0.0, Float rotation = 0.0) {
    Float c = 0.0;
    Float s = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector u = Vector{0.0, 0.0, 0.0};
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    c = kern::formula([](Float rotation) -> Float { return kern::math::cos(rotation); }, rotation);
    s = kern::formula([](Float rotation) -> Float { return kern::math::sin(rotation); }, rotation);
    r = vector_scale(vector_add(vector_scale(right, c), vector_scale(up, s)), size);
    u = vector_scale(vector_sub(vector_scale(up, c), vector_scale(right, s)), size);
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    m.m[0] = vector_break(r).x;
    m.m[1] = vector_break(r).y;
    m.m[2] = vector_break(r).z;
    m.m[4] = vector_break(u).x;
    m.m[5] = vector_break(u).y;
    m.m[6] = vector_break(u).z;
    m.m[8] = vector_break(normal).x;
    m.m[9] = vector_break(normal).y;
    m.m[10] = vector_break(normal).z;
    m.m[12] = vector_break(p).x;
    m.m[13] = vector_break(p).y;
    m.m[14] = vector_break(p).z;
    return {m};
  }

  struct smoke_discResult { MeshData Result = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; };
  /// The smoke sprite of world_critical_damage_fx.mjs makeTexture(): a filled white disc (radius 27/32) with a grey (180/255) rim 3 px wide - as a mesh, the rim in the vertex colours.
  smoke_discResult smoke_disc() {
    MeshData d = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Int seg = 0;
    Float inner = 0.0;
    Float outer = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector white = Vector{0.0, 0.0, 0.0};
    Vector rim = Vector{0.0, 0.0, 0.0};
    Float a = 0.0;
    Int j = 0;
    Int a_2 = 0;
    Int b = 0;
    Int place = 0;
    Int item = 0;
    Int item_2 = 0;
    Int item_3 = 0;
    Int place_2 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    Int item_7 = 0;
    Int item_8 = 0;
    Int item_9 = 0;
    d = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    seg = 24;
    inner = 0.796875;
    outer = 0.890625;
    n = Vector{0.0, 0.0, 1.0};
    white = Vector{1.0, 1.0, 1.0};
    rim = Vector{0.706, 0.706, 0.706};
    engine_geo_mesh_data::MeshData_push(d, Vector{0.0, 0.0, 0.0}, n, white, Vector{0.5, 0.5, 0.0});
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(seg)))); it16.next();) {
      a = kern::formula([](Float index, Float seg, Float kPi) -> Float { return ((kern::math::div(index, seg) * 2.0) * kPi); }, kern::to<Float>(n16.index), kern::to<Float>(seg), kPi);
      engine_geo_mesh_data::MeshData_push(d, vector_make(kern::formula([](Float a, Float inner) -> Float { return (kern::math::cos(a) * inner); }, a, inner), kern::formula([](Float a, Float inner) -> Float { return (kern::math::sin(a) * inner); }, a, inner), 0.0), n, white, Vector{0.0, 0.0, 0.0});
      engine_geo_mesh_data::MeshData_push(d, vector_make(kern::formula([](Float a, Float inner) -> Float { return (kern::math::cos(a) * inner); }, a, inner), kern::formula([](Float a, Float inner) -> Float { return (kern::math::sin(a) * inner); }, a, inner), 0.0), n, rim, Vector{0.0, 0.0, 0.0});
      engine_geo_mesh_data::MeshData_push(d, vector_make(kern::formula([](Float a, Float outer) -> Float { return (kern::math::cos(a) * outer); }, a, outer), kern::formula([](Float a, Float outer) -> Float { return (kern::math::sin(a) * outer); }, a, outer), 0.0), n, rim, Vector{0.0, 0.0, 0.0});
    }
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(seg)))); it17.next();) {
      j = kern::to<Int>(kern::formula([](Float index, Float seg) -> Float { return kern::math::mod(kern::math::wrap32(index + 1.0), seg); }, kern::to<Float>(n17.index), kern::to<Float>(seg)));
      a_2 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(1.0 + kern::math::mul32(index, 3.0)); }, kern::to<Float>(n17.index)));
      b = kern::to<Int>(kern::formula([](Float j) -> Float { return kern::math::wrap32(1.0 + kern::math::mul32(j, 3.0)); }, kern::to<Float>(j)));
      place = list_length<Int>(d.indices);
      item = 0;
      item_2 = a_2;
      item_3 = b;
      list_insert_list(d.indices, place, list_make<Int, "3">(item, item_2, item_3));
      place_2 = list_length<Int>(d.indices);
      item_4 = kern::to<Int>(kern::formula([](Float a_2) -> Float { return kern::math::wrap32(a_2 + 1.0); }, kern::to<Float>(a_2)));
      item_5 = kern::to<Int>(kern::formula([](Float a_2) -> Float { return kern::math::wrap32(a_2 + 2.0); }, kern::to<Float>(a_2)));
      item_6 = kern::to<Int>(kern::formula([](Float b) -> Float { return kern::math::wrap32(b + 2.0); }, kern::to<Float>(b)));
      item_7 = kern::to<Int>(kern::formula([](Float a_2) -> Float { return kern::math::wrap32(a_2 + 1.0); }, kern::to<Float>(a_2)));
      item_8 = kern::to<Int>(kern::formula([](Float b) -> Float { return kern::math::wrap32(b + 2.0); }, kern::to<Float>(b)));
      item_9 = kern::to<Int>(kern::formula([](Float b) -> Float { return kern::math::wrap32(b + 1.0); }, kern::to<Float>(b)));
      list_insert_list(d.indices, place_2, list_make<Int, "6">(item_4, item_5, item_6, item_7, item_8, item_9));
    }
    return {d};
  }

  struct jet_roar_pcmResult { List<Float> Result = List<Float>{}; };
  /// fighter_jets.mjs ensureAudio(): 4 s of rumble (noise low-passed twice), roar, hiss and a turbine whine at 1180 Hz (vibrato 3 Hz) + 2360 Hz, loop cross-faded over 50 ms, normalised.
  jet_roar_pcmResult jet_roar_pcm(Int rate = 0, Float cutoff = 0.0) {
    Int n = 0;
    List<Float> d = List<Float>{};
    List<Float> made = List<Float>{};
    Int s = 0;
    Float lp = 0.0;
    Float lp2 = 0.0;
    Float br = 0.0;
    Float t = 0.0;
    Float w = 0.0;
    Float whine = 0.0;
    Int f = 0;
    Float a = 0.0;
    Biquad lpf = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    n = kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 4.0); }, kern::to<Float>(rate)));
    made = List<Float>{};
    list_resize(made, n, 0.0);
    d = made;
    s = 625341585;
    lp = 0.0;
    lp2 = 0.0;
    br = 0.0;
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it18.next();) {
      t = kern::formula([](Float index, Float rate) -> Float { return kern::math::div((index < 0.0 ? index + 18446744073709551616.0 : index), rate); }, kern::to<Float>(n18.index), kern::to<Float>(rate));
      s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 13.0))); }, kern::to<Float>(s)));
      s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::shr(s, 17.0)); }, kern::to<Float>(s)));
      s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 5.0))); }, kern::to<Float>(s)));
      w = kern::formula([](Float s) -> Float { return (kern::math::div(s, 2147483648.0) - 1.0); }, kern::to<Float>(s));
      lp += kern::formula([](Float w, Float lp) -> Float { return ((w - lp) * 0.08); }, w, lp);
      lp2 += kern::formula([](Float lp, Float lp2) -> Float { return ((lp - lp2) * 0.02); }, lp, lp2);
      br += kern::formula([](Float w, Float br) -> Float { return ((w - br) * 0.6); }, w, br);
      whine = kern::formula([](Float kPi, Float t) -> Float { return ((kern::math::sin(((((2.0 * kPi) * 1180.0) * t) + (kern::math::sin((((2.0 * kPi) * 3.0) * t)) * 2.0))) * 0.05) + (kern::math::sin((((2.0 * kPi) * 2360.0) * t)) * 0.025)); }, kPi, t);
      d[n18.index] = kern::formula([](Float lp2, Float lp, Float w, Float br, Float whine) -> Float { return ((((lp2 * 2.4) + (lp * 0.9)) + ((w - br) * 0.18)) + whine); }, lp2, lp, w, br, whine);
    }
    f = kern::to<Int>(kern::formula([](Float rate) -> Float { return kern::math::trunc((rate * 0.05)); }, kern::to<Float>(rate)));
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(f)))); it19.next();) {
      a = kern::formula([](Float index, Float f) -> Float { return kern::math::div((index < 0.0 ? index + 18446744073709551616.0 : index), (f < 0.0 ? f + 18446744073709551616.0 : f)); }, kern::to<Float>(n19.index), kern::to<Float>(f));
      d[kern::to<Int>(kern::formula([](Float n, Float f, Float index) -> Float { return ((n - f) + index); }, kern::to<Float>(n), kern::to<Float>(f), kern::to<Float>(n19.index)))] = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a * (1.0 - a_2)) + (a_3 * a_2)); }, d[kern::to<Int>(kern::formula([](Float n, Float f, Float index) -> Float { return ((n - f) + index); }, kern::to<Float>(n), kern::to<Float>(f), kern::to<Float>(n19.index)))], a, d[n19.index]);
    }
    lpf = Biquad_lowpass(cutoff, 0.7, kern::to<Float>(rate)).Result;
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(d))))); it20.next();) {
      n21 = Biquad_run(lpf, d[n20.index]);
      d[n20.index] = n21.Result;
    }
    engine_audio_normalize(n22, d, 0.9);
    d = n22.samples;
    return {d};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  Biquad_runResult n11;
  Biquad_runResult n12;
  Biquad_runResult n13;
  struct { List<Float> samples = List<Float>{}; } n14;
  struct { List<Float> samples = List<Float>{}; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  Biquad_runResult n21;
  struct { List<Float> samples = List<Float>{}; } n22;
};
