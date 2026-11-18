// Generated from spawncity.klevel by tools/blueprint_cpp --classes - do not edit: the classes of Blueprints of the
// level (kern/docs/blueprint_cpp.md), without glue, named in the namespace blueprints.
#pragma once
#include "kern/logic/blueprint.h"

// ---------------------------------------------------------------------------
// The Blueprint of actor "blueprint_call"
namespace kern_blueprint_0eb7c15494c83360 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct blueprint_call : virtual kern::Library {
  // ---- functions ----
  struct decimalResult { Float Result = 0.0; };
  /// A number of the game's 32-bit state as a Blueprint gets it: the decimal number it stands for. A Blueprint computes with 64-bit numbers (as the original's JavaScript does) and its rules name their thresholds in decimals - 0.1, 2.6, 0.42. The game keeps its state in 32 bit, where "0.1" is 0.100000001490116...; passed on as that, it would lie just beyond a threshold of 0.1 it stands exactly on. So a 32-bit number crosses over as the shortest decimal that is this very 32-bit number (0.1f -> 0.1): every comparison of it with a decimal threshold then answers exactly as the comparison in 32 bit did, on both sides of the threshold and on it. The way is one to one and keeps the order: back in 32 bit it is the same number again, and of two numbers the larger stays the larger. (Whole numbers, and numbers beyond the range a game counts in, are what they are.)
  KERN_PURE decimalResult decimal(Float x = 0.0) {
    Float d = 0.0;
    Float a = 0.0;
    Float scale = 0.0;
    Float m = 0.0;
    Float m_2 = 0.0;
    Int digits = 0;
    Float c = 0.0;
    d = x;
    a = kern::formula([](Float d) -> Float { return kern::math::abs(d); }, d);
    if (logic_or(logic_not(logic_and(math_compare<">=">(a, 1e-09), math_compare<"<">(a, 10000000.0))), math_compare<">">(kern::formula([](Float d) -> Float { return d == kern::math::trunc(d); }, d), 0.5))) {
      return {d};
    }
    scale = 1.0;
    m = a;
    while (loop(math_compare<"<">(m, 1.0))) {
      scale = kern::formula([](Float scale) -> Float { return scale * 10.0; }, scale);
      m = kern::formula([](Float m) -> Float { return m * 10.0; }, m);
    }
    m_2 = a;
    while (loop(math_compare<">=">(m_2, 10.0))) {
      scale = kern::formula([](Float scale) -> Float { return kern::math::div(scale, 10.0); }, scale);
      m_2 = kern::formula([](Float m_2) -> Float { return kern::math::div(m_2, 10.0); }, m_2);
    }
    digits = 1;
    while (loop(math_compare<"<=">(kern::to<Float>(digits), 9.0))) {
      c = kern::formula([](Float scale, Float d) -> Float { return scale >= 1.0 ? kern::math::div(kern::math::round(d * scale), scale) : kern::math::round(d * scale) * kern::math::round(kern::math::div(1.0, scale)); }, scale, d);
      if (math_compare<">">(kern::formula([](Float c, Float x) -> Float { return c == x; }, c, x), 0.5)) {
        return {c};
      }
      digits += 1;
      scale = kern::formula([](Float scale) -> Float { return scale * 10.0; }, scale);
    }
    return {d};
  }
};

KERN_LAYOUT(blueprint_call) {
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 560, 0);
  node(4, 440, 150);
  node(5, 440, 240);
  node(6, 840, 0);
  node(7, 720, 150);
  node(8, 720, 240);
  node(9, 720, 330);
  node(10, 720, 420);
  node(11, 720, 510);
  node(12, 720, 600);
  node(13, 720, 690);
  node(14, 720, 780);
  node(15, 720, 870);
  node(16, 720, 960);
  node(17, 1120, 0);
  node(18, 1000, 150);
  node(19, 1400, 0);
  node(20, 1680, 0);
  node(21, 1560, 150);
  node(22, 1960, 0);
  node(23, 1840, 150);
  node(24, 1840, 240);
  node(25, 2240, 0);
  node(26, 2120, 150);
  node(27, 2120, 240);
  node(28, 2520, 0);
  node(29, 2400, 150);
  node(30, 2400, 240);
  node(31, 2800, 0);
  node(32, 2680, 150);
  node(33, 3080, 0);
  node(34, 2960, 150);
  node(35, 2960, 240);
  node(36, 3360, 0);
  node(37, 3240, 150);
  node(38, 3240, 240);
  node(39, 3640, 0);
  node(40, 3520, 150);
  node(41, 3520, 240);
  node(42, 3920, 0);
  node(43, 4200, 0);
  node(44, 4080, 150);
  node(45, 4080, 240);
  node(46, 4480, 0);
  node(47, 4360, 150);
  node(48, 4360, 240);
  node(49, 4360, 330);
  node(50, 4760, 0);
  node(51, 4640, 150);
  node(52, 4640, 240);
  node(53, 4640, 330);
  node(54, 5040, 0);
  node(55, 4920, 150);
  node(56, 5320, 0);
  node(57, 5600, 0);
  node(58, 5480, 150);
  node(59, 5480, 240);
  node(60, 5880, 0);
  node(61, 5760, 150);
  next_id(62);
}


}  // namespace kern_blueprint_0eb7c15494c83360

// ---------------------------------------------------------------------------
// The Blueprint of actor "shapes"
namespace kern_blueprint_6025b4a819f918d1 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct shapes : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Float_2_row) {
    Fixed<Float, 2> items = Fixed<Float, 2>{};
  };
  KERN_STRUCT(Float_3_row) {
    Fixed<Float, 3> items = Fixed<Float, 3>{};
  };
  KERN_STRUCT(Float_4_row) {
    Fixed<Float, 4> items = Fixed<Float, 4>{};
  };
  KERN_STRUCT(Float_5_row) {
    Fixed<Float, 5> items = Fixed<Float, 5>{};
  };
  KERN_STRUCT(Float_Vector_pair) {
    Float first = 0.0;
    Vector second = Vector{0.0, 0.0, 0.0};
  };
  KERN_STRUCT(Int64_Int_pair) {
    Int64 first = Int64{0x0};
    Int second = 0;
  };
  KERN_STRUCT(Int_4_row) {
    Fixed<Int, 4> items = Fixed<Int, 4>{};
  };
  KERN_STRUCT(Int_Bool_pair) {
    Int first = 0;
    Bool second = false;
  };
  KERN_STRUCT(Int_Float_pair) {
    Int first = 0;
    Float second = 0.0;
  };
  KERN_STRUCT(Int_Int_pair) {
    Int first = 0;
    Int second = 0;
  };
  KERN_STRUCT(Int_row) {
    List<Int> items = List<Int>{};
  };
  KERN_STRUCT(Vector_row) {
    List<Vector> items = List<Vector>{};
  };
  KERN_STRUCT(Vector_row_row) {
    List<Vector_row> items = List<Vector_row>{};
  };
};

KERN_LAYOUT(shapes) {
  next_id(1);
}


}  // namespace kern_blueprint_6025b4a819f918d1

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_geo_roads"
namespace kern_blueprint_c57f7a6bcfc775fb {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_geo_roads : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(GeoProjector) {
    Float lon0 = 0.0;
    Float lat0 = 0.0;
    Float cos_lat = 1.0;
  };

  // ---- variables ----
  KERN_CONST Float kRad = 0.017453292519943295;
  KERN_CONST Float kEarthRadius = 6378137.0;

  // ---- functions ----
  void GeoProjector_project(const GeoProjector& self, Float& x, Float& y, Float lon = 0.0, Float lat = 0.0) {
    x = kern::formula([](Float lon, Float lon0, Float kRad, Float kEarthRadius, Float cos_lat) -> Float { return (lon - lon0) * kRad * kEarthRadius * cos_lat; }, lon, self.lon0, kRad, kEarthRadius, self.cos_lat);
    y = kern::formula([](Float lat, Float lat0, Float kRad, Float kEarthRadius) -> Float { return (lat - lat0) * kRad * kEarthRadius; }, lat, self.lat0, kRad, kEarthRadius);
  }
};

KERN_LAYOUT(engine_geo_roads) {
  node(5, 0, 380);
  node(6, 280, 380);
  node(7, 160, 530);
  node(8, 160, 620);
  node(9, 160, 710);
  node(10, 160, 800);
  node(11, 160, 890);
  node(12, 560, 380);
  node(13, 440, 530);
  node(14, 440, 620);
  node(15, 440, 710);
  node(16, 440, 800);
  next_id(17);
}


}  // namespace kern_blueprint_c57f7a6bcfc775fb

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_math_math"
namespace kern_blueprint_388516bead732334 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_math_math : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Mat4) {
    Fixed<Float, 16> m = Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  };
  KERN_STRUCT(Quat) {
    Float x = 0.0;
    Float y = 0.0;
    Float z = 0.0;
    Float w = 1.0;
  };
  KERN_STRUCT(Vec4) {
    Float x = 0.0;
    Float y = 0.0;
    Float z = 0.0;
    Float w = 0.0;
  };
  KERN_STRUCT(Camera) {
    Vector eye = Vector{0.0, -18.0, 8.0};
    Vector target = Vector{0.0, 0.0, 2.0};
    Vector up = Vector{0.0, 0.0, 1.0};
    Float fov = 1.0471975511965976;
    Float near = 0.05;
    Float far = 1800.0;
    Bool orthographic = false;
    Float ortho_height = 20.0;
    Bool has_projection = false;
    Mat4 projection = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
  };

  // ---- functions ----
  struct Vec4_newResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE Vec4_newResult Vec4_new(Float x_ = 0.0, Float y_ = 0.0, Float z_ = 0.0, Float w_ = 0.0) {
    Vec4 self = Vec4{0.0, 0.0, 0.0, 0.0};
    self.x = x_;
    self.y = y_;
    self.z = z_;
    self.w = w_;
    return {self};
  }

  struct Quat_axis_angleResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE Quat_axis_angleResult Quat_axis_angle(Vector axis = Vector{0.0, 0.0, 0.0}, Float radians = 0.0) {
    Vector a = Vector{0.0, 0.0, 0.0};
    Float s = 0.0;
    a = logic_select<Vector>(math_compare<">">(vector_length(axis), 9.999999717180685e-10), vector_normalize(axis), Vector{0.0, 0.0, 1.0});
    s = kern::formula([](Float radians) -> Float { return kern::math::sin(radians * 0.5); }, radians);
    return {struct_make<Quat>(kern::formula([](Float x, Float s) -> Float { return x * s; }, vector_break(a).x, s), kern::formula([](Float y, Float s) -> Float { return y * s; }, vector_break(a).y, s), kern::formula([](Float z, Float s) -> Float { return z * s; }, vector_break(a).z, s), kern::formula([](Float radians) -> Float { return kern::math::cos(radians * 0.5); }, radians))};
  }

  struct Quat_mulResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE Quat_mulResult Quat_mul(const Quat& self, const Quat& b) {
    return {struct_make<Quat>(kern::formula([](Float w, Float x, Float x_2, Float w_2, Float y, Float z, Float z_2, Float y_2) -> Float { return w * x + x_2 * w_2 + y * z - z_2 * y_2; }, self.w, b.x, self.x, b.w, self.y, b.z, self.z, b.y), kern::formula([](Float w, Float y, Float x, Float z, Float y_2, Float w_2, Float z_2, Float x_2) -> Float { return w * y - x * z + y_2 * w_2 + z_2 * x_2; }, self.w, b.y, self.x, b.z, self.y, b.w, self.z, b.x), kern::formula([](Float w, Float z, Float x, Float y, Float y_2, Float x_2, Float z_2, Float w_2) -> Float { return w * z + x * y - y_2 * x_2 + z_2 * w_2; }, self.w, b.z, self.x, b.y, self.y, b.x, self.z, b.w), kern::formula([](Float w, Float w_2, Float x, Float x_2, Float y, Float y_2, Float z, Float z_2) -> Float { return w * w_2 - x * x_2 - y * y_2 - z * z_2; }, self.w, b.w, self.x, b.x, self.y, b.y, self.z, b.z))};
  }

  struct Quat_rotateResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Quat_rotateResult Quat_rotate(const Quat& self, Vector v = Vector{0.0, 0.0, 0.0}) {
    Vector q = Vector{0.0, 0.0, 0.0};
    Vector t = Vector{0.0, 0.0, 0.0};
    q = vector_make(self.x, self.y, self.z);
    t = vector_scale(vector_cross(q, v), 2.0);
    return {vector_add(vector_add(v, vector_scale(t, self.w)), vector_cross(q, t))};
  }

  struct Mat4_translationResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Mat4_translationResult Mat4_translation(const Mat4& self) {
    return {vector_make(self.m[12], self.m[13], self.m[14])};
  }

  struct Mat4_mulResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE Mat4_mulResult Mat4_mul(const Mat4& self, const Mat4& b) {
    Mat4 r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float b0 = 0.0;
    Float b1 = 0.0;
    Float b2 = 0.0;
    Float b3 = 0.0;
    r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    for (auto it10 = flow_for(n10, 0, 3); it10.next();) {
      b0 = b.m[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index; }, kern::to<Float>(n10.index)))];
      b1 = b.m[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index + 1.0; }, kern::to<Float>(n10.index)))];
      b2 = b.m[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index + 2.0; }, kern::to<Float>(n10.index)))];
      b3 = b.m[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index + 3.0; }, kern::to<Float>(n10.index)))];
      for (auto it11 = flow_for(n11, 0, 3); it11.next();) {
        r.m[kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return 4.0 * index + index_2; }, kern::to<Float>(n10.index), kern::to<Float>(n11.index)))] = kern::formula([](Float a, Float b0, Float a_2, Float b1, Float a_3, Float b2, Float a_4, Float b3) -> Float { return a * b0 + a_2 * b1 + a_3 * b2 + a_4 * b3; }, self.m[n11.index], b0, self.m[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 + index; }, kern::to<Float>(n11.index)))], b1, self.m[kern::to<Int>(kern::formula([](Float index) -> Float { return 8.0 + index; }, kern::to<Float>(n11.index)))], b2, self.m[kern::to<Int>(kern::formula([](Float index) -> Float { return 12.0 + index; }, kern::to<Float>(n11.index)))], b3);
      }
    }
    return {std::move(r)};
  }

  struct Mat4_transform_pointResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Mat4_transform_pointResult Mat4_transform_point(const Mat4& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    return {vector_make(kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z, Float a_4) -> Float { return a * x + a_2 * y + a_3 * z + a_4; }, self.m[0], vector_break(p).x, self.m[4], vector_break(p).y, self.m[8], vector_break(p).z, self.m[12]), kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z, Float a_4) -> Float { return a * x + a_2 * y + a_3 * z + a_4; }, self.m[1], vector_break(p).x, self.m[5], vector_break(p).y, self.m[9], vector_break(p).z, self.m[13]), kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z, Float a_4) -> Float { return a * x + a_2 * y + a_3 * z + a_4; }, self.m[2], vector_break(p).x, self.m[6], vector_break(p).y, self.m[10], vector_break(p).z, self.m[14]))};
  }

  struct Mat4_transform_vectorResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Mat4_transform_vectorResult Mat4_transform_vector(const Mat4& self, Vector v = Vector{0.0, 0.0, 0.0}) {
    return {vector_make(kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z) -> Float { return a * x + a_2 * y + a_3 * z; }, self.m[0], vector_break(v).x, self.m[4], vector_break(v).y, self.m[8], vector_break(v).z), kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z) -> Float { return a * x + a_2 * y + a_3 * z; }, self.m[1], vector_break(v).x, self.m[5], vector_break(v).y, self.m[9], vector_break(v).z), kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z) -> Float { return a * x + a_2 * y + a_3 * z; }, self.m[2], vector_break(v).x, self.m[6], vector_break(v).y, self.m[10], vector_break(v).z))};
  }

  struct Mat4_transformResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE Mat4_transformResult Mat4_transform(const Mat4& self, Vec4 p = Vec4{0.0, 0.0, 0.0, 0.0}) {
    return {Vec4_new(kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z, Float a_4, Float w) -> Float { return a * x + a_2 * y + a_3 * z + a_4 * w; }, self.m[0], p.x, self.m[4], p.y, self.m[8], p.z, self.m[12], p.w), kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z, Float a_4, Float w) -> Float { return a * x + a_2 * y + a_3 * z + a_4 * w; }, self.m[1], p.x, self.m[5], p.y, self.m[9], p.z, self.m[13], p.w), kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z, Float a_4, Float w) -> Float { return a * x + a_2 * y + a_3 * z + a_4 * w; }, self.m[2], p.x, self.m[6], p.y, self.m[10], p.z, self.m[14], p.w), kern::formula([](Float a, Float x, Float a_2, Float y, Float a_3, Float z, Float a_4, Float w) -> Float { return a * x + a_2 * y + a_3 * z + a_4 * w; }, self.m[3], p.x, self.m[7], p.y, self.m[11], p.z, self.m[15], p.w)).Result};
  }

  struct Mat4_translateResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE Mat4_translateResult Mat4_translate(Vector t = Vector{0.0, 0.0, 0.0}) {
    Mat4 r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r.m[12] = vector_break(t).x;
    r.m[13] = vector_break(t).y;
    r.m[14] = vector_break(t).z;
    return {std::move(r)};
  }

  struct Mat4_scaleResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE Mat4_scaleResult Mat4_scale(Vector s = Vector{0.0, 0.0, 0.0}) {
    Mat4 r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r.m[0] = vector_break(s).x;
    r.m[5] = vector_break(s).y;
    r.m[10] = vector_break(s).z;
    return {std::move(r)};
  }

  struct Mat4_trsResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  /// Translation * rotation * scale.
  KERN_PURE Mat4_trsResult Mat4_trs(Vector t = Vector{0.0, 0.0, 0.0}, Quat q = Quat{0.0, 0.0, 0.0, 1.0}, Vector s = Vector{0.0, 0.0, 0.0}) {
    Float x2 = 0.0;
    Float y2 = 0.0;
    Float z2 = 0.0;
    Float xx = 0.0;
    Float xy = 0.0;
    Float xz = 0.0;
    Float yy = 0.0;
    Float yz = 0.0;
    Float zz = 0.0;
    Float wx = 0.0;
    Float wy = 0.0;
    Float wz = 0.0;
    Mat4 r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    x2 = kern::formula([](Float x) -> Float { return x + x; }, q.x);
    y2 = kern::formula([](Float y) -> Float { return y + y; }, q.y);
    z2 = kern::formula([](Float z) -> Float { return z + z; }, q.z);
    xx = kern::formula([](Float x, Float x2) -> Float { return x * x2; }, q.x, x2);
    xy = kern::formula([](Float x, Float y2) -> Float { return x * y2; }, q.x, y2);
    xz = kern::formula([](Float x, Float z2) -> Float { return x * z2; }, q.x, z2);
    yy = kern::formula([](Float y, Float y2) -> Float { return y * y2; }, q.y, y2);
    yz = kern::formula([](Float y, Float z2) -> Float { return y * z2; }, q.y, z2);
    zz = kern::formula([](Float z, Float z2) -> Float { return z * z2; }, q.z, z2);
    wx = kern::formula([](Float w, Float x2) -> Float { return w * x2; }, q.w, x2);
    wy = kern::formula([](Float w, Float y2) -> Float { return w * y2; }, q.w, y2);
    wz = kern::formula([](Float w, Float z2) -> Float { return w * z2; }, q.w, z2);
    r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r.m[0] = kern::formula([](Float yy, Float zz, Float x) -> Float { return (1.0 - yy - zz) * x; }, yy, zz, vector_break(s).x);
    r.m[1] = kern::formula([](Float xy, Float wz, Float x) -> Float { return (xy + wz) * x; }, xy, wz, vector_break(s).x);
    r.m[2] = kern::formula([](Float xz, Float wy, Float x) -> Float { return (xz - wy) * x; }, xz, wy, vector_break(s).x);
    r.m[3] = 0.0;
    r.m[4] = kern::formula([](Float xy, Float wz, Float y) -> Float { return (xy - wz) * y; }, xy, wz, vector_break(s).y);
    r.m[5] = kern::formula([](Float xx, Float zz, Float y) -> Float { return (1.0 - xx - zz) * y; }, xx, zz, vector_break(s).y);
    r.m[6] = kern::formula([](Float yz, Float wx, Float y) -> Float { return (yz + wx) * y; }, yz, wx, vector_break(s).y);
    r.m[7] = 0.0;
    r.m[8] = kern::formula([](Float xz, Float wy, Float z) -> Float { return (xz + wy) * z; }, xz, wy, vector_break(s).z);
    r.m[9] = kern::formula([](Float yz, Float wx, Float z) -> Float { return (yz - wx) * z; }, yz, wx, vector_break(s).z);
    r.m[10] = kern::formula([](Float xx, Float yy, Float z) -> Float { return (1.0 - xx - yy) * z; }, xx, yy, vector_break(s).z);
    r.m[11] = 0.0;
    r.m[12] = vector_break(t).x;
    r.m[13] = vector_break(t).y;
    r.m[14] = vector_break(t).z;
    r.m[15] = 1.0;
    return {std::move(r)};
  }

  struct Mat4_perspectiveResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE Mat4_perspectiveResult Mat4_perspective(Float fov = 0.0, Float aspect = 0.0, Float near = 0.0, Float far = 0.0) {
    Float f = 0.0;
    Float nf = 0.0;
    Mat4 r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    f = kern::formula([](Float fov) -> Float { return kern::math::div(1.0, kern::math::tan(fov * 0.5)); }, fov);
    nf = kern::formula([](Float near, Float far) -> Float { return kern::math::div(1.0, near - far); }, near, far);
    r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r.m = List<Float>{};
    r.m[0] = kern::formula([](Float f, Float aspect) -> Float { return kern::math::div(f, aspect); }, f, aspect);
    r.m[5] = f;
    r.m[10] = kern::formula([](Float far, Float near, Float nf) -> Float { return (far + near) * nf; }, far, near, nf);
    r.m[11] = -1.0;
    r.m[14] = kern::formula([](Float far, Float near, Float nf) -> Float { return 2.0 * far * near * nf; }, far, near, nf);
    return {std::move(r)};
  }

  struct Mat4_orthoResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE Mat4_orthoResult Mat4_ortho(Float l = 0.0, Float r_ = 0.0, Float b = 0.0, Float t = 0.0, Float n = 0.0, Float f = 0.0) {
    Mat4 r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r.m = List<Float>{};
    r.m[0] = kern::formula([](Float r_, Float l) -> Float { return kern::math::div(2.0, r_ - l); }, r_, l);
    r.m[5] = kern::formula([](Float t, Float b) -> Float { return kern::math::div(2.0, t - b); }, t, b);
    r.m[10] = kern::formula([](Float f, Float n) -> Float { return kern::math::div(-2.0, f - n); }, f, n);
    r.m[12] = kern::formula([](Float r_, Float l) -> Float { return kern::math::div(-(r_ + l), r_ - l); }, r_, l);
    r.m[13] = kern::formula([](Float t, Float b) -> Float { return kern::math::div(-(t + b), t - b); }, t, b);
    r.m[14] = kern::formula([](Float f, Float n) -> Float { return kern::math::div(-(f + n), f - n); }, f, n);
    r.m[15] = 1.0;
    return {std::move(r)};
  }

  struct Mat4_look_atResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  /// View matrix. Returns identity (and false in `ok`) for a degenerate basis.
  Mat4_look_atResult Mat4_look_at(Bool& ok, Vector eye = Vector{0.0, 0.0, 0.0}, Vector target = Vector{0.0, 0.0, 0.0}, Vector up = Vector{0.0, 0.0, 0.0}) {
    Vector z = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Mat4 r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector x = Vector{0.0, 0.0, 0.0};
    Vector y = Vector{0.0, 0.0, 0.0};
    z = vector_sub(eye, target);
    len = vector_length(z);
    r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_not(math_compare<">">(len, 1e-10))) {
      ok = false;
      return {std::move(r)};
    }
    z = vector_make(kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, vector_break(z).x, len), kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, vector_break(z).y, len), kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, vector_break(z).z, len));
    x = vector_cross(up, z);
    len = vector_length(x);
    if (logic_not(math_compare<">">(len, 1e-10))) {
      ok = false;
      return {std::move(r)};
    }
    x = vector_make(kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, vector_break(x).x, len), kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, vector_break(x).y, len), kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, vector_break(x).z, len));
    y = vector_cross(z, x);
    r.m[0] = vector_break(x).x;
    r.m[1] = vector_break(y).x;
    r.m[2] = vector_break(z).x;
    r.m[3] = 0.0;
    r.m[4] = vector_break(x).y;
    r.m[5] = vector_break(y).y;
    r.m[6] = vector_break(z).y;
    r.m[7] = 0.0;
    r.m[8] = vector_break(x).z;
    r.m[9] = vector_break(y).z;
    r.m[10] = vector_break(z).z;
    r.m[11] = 0.0;
    r.m[12] = kern::formula([](Float a) -> Float { return -a; }, vector_dot(x, eye));
    r.m[13] = kern::formula([](Float a) -> Float { return -a; }, vector_dot(y, eye));
    r.m[14] = kern::formula([](Float a) -> Float { return -a; }, vector_dot(z, eye));
    r.m[15] = 1.0;
    ok = true;
    return {std::move(r)};
  }

  struct Mat4_look_at_2Result { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  /// The same, where nobody asks whether it is one.
  Mat4_look_at_2Result Mat4_look_at_2(Vector eye = Vector{0.0, 0.0, 0.0}, Vector target = Vector{0.0, 0.0, 0.0}, Vector up = Vector{0.0, 0.0, 0.0}) {
    Bool ok = false;
    ok = false;
    n12 = Mat4_look_at(ok, eye, target, up);
    return {std::move(n12.Result)};
  }

  struct projection_matrixResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  /// Projection matrix of the camera for a viewport aspect ratio (width / height).
  KERN_PURE projection_matrixResult projection_matrix(const Camera& c, Float aspect = 0.0) {
    Float h = 0.0;
    Float w = 0.0;
    if (c.has_projection) {
      return {c.projection};
    }
    if (logic_not(c.orthographic)) {
      return {Mat4_perspective(c.fov, aspect, c.near, c.far).Result};
    }
    h = kern::formula([](Float ortho_height) -> Float { return ortho_height * 0.5; }, c.ortho_height);
    w = kern::formula([](Float h, Float aspect) -> Float { return h * aspect; }, h, aspect);
    return {Mat4_ortho(kern::formula([](Float w) -> Float { return -w; }, w), w, kern::formula([](Float h) -> Float { return -h; }, h), h, c.near, c.far).Result};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  Mat4_look_atResult n12;
};

KERN_LAYOUT(engine_math_math) {
  node(350, 0, 17568);
  node(13, 280, 0);
  node(14, 560, 0);
  node(15, 840, 0);
  node(16, 1120, 0);
  node(17, 1400, 0);
  node(18, 1280, 150);
  node(19, 0, 290);
  node(20, 280, 290);
  node(21, 160, 440);
  node(22, 160, 530);
  node(23, 160, 620);
  node(24, 160, 710);
  node(25, 560, 290);
  node(26, 440, 440);
  node(27, 840, 290);
  node(28, 720, 440);
  node(29, 720, 530);
  node(30, 720, 620);
  node(31, 720, 710);
  node(32, 720, 800);
  node(33, 720, 890);
  node(34, 720, 980);
  node(35, 720, 1070);
  node(36, 720, 1160);
  node(37, 720, 1250);
  node(38, 720, 1340);
  node(39, 720, 1430);
  node(40, 720, 1520);
  node(41, 720, 1610);
  node(42, 0, 1750);
  node(43, 280, 1750);
  node(44, 160, 1900);
  node(45, 160, 1990);
  node(46, 160, 2080);
  node(47, 160, 2170);
  node(48, 160, 2260);
  node(49, 160, 2350);
  node(50, 160, 2440);
  node(51, 160, 2530);
  node(52, 160, 2620);
  node(53, 160, 2710);
  node(54, 160, 2800);
  node(55, 160, 2890);
  node(56, 160, 2980);
  node(57, 160, 3070);
  node(58, 160, 3160);
  node(59, 160, 3250);
  node(60, 160, 3340);
  node(61, 160, 3430);
  node(62, 160, 3520);
  node(63, 160, 3610);
  node(64, 160, 3700);
  node(65, 160, 3790);
  node(66, 160, 3880);
  node(67, 160, 3970);
  node(68, 160, 4060);
  node(69, 160, 4150);
  node(70, 160, 4240);
  node(71, 160, 4330);
  node(72, 160, 4420);
  node(73, 160, 4510);
  node(74, 160, 4600);
  node(75, 160, 4690);
  node(76, 160, 4780);
  node(77, 160, 4870);
  node(78, 160, 4960);
  node(79, 160, 5050);
  node(80, 160, 5140);
  node(81, 0, 5280);
  node(82, 280, 5280);
  node(83, 160, 5430);
  node(84, 160, 5520);
  node(85, 160, 5610);
  node(86, 160, 5700);
  node(87, 560, 5280);
  node(88, 440, 5430);
  node(89, 440, 5520);
  node(90, 440, 5610);
  node(91, 840, 5280);
  node(92, 720, 5430);
  node(93, 720, 5520);
  node(94, 720, 5610);
  node(95, 720, 5700);
  node(96, 720, 5790);
  node(97, 720, 5880);
  node(98, 720, 5970);
  node(99, 720, 6060);
  node(100, 0, 6200);
  node(101, 280, 6200);
  node(102, 160, 6350);
  node(103, 160, 6440);
  node(104, 160, 6530);
  node(105, 160, 6620);
  node(106, 0, 6760);
  node(107, 280, 6760);
  node(10, 560, 6760);
  node(108, 840, 6760);
  node(109, 720, 6910);
  node(110, 720, 7000);
  node(111, 1120, 6760);
  node(112, 1000, 6910);
  node(113, 1000, 7000);
  node(114, 1400, 6760);
  node(115, 1280, 6910);
  node(116, 1280, 7000);
  node(117, 1680, 6760);
  node(118, 1560, 6910);
  node(119, 1560, 7000);
  node(11, 1960, 6760);
  node(120, 2240, 6760);
  node(121, 2120, 6910);
  node(122, 2120, 7000);
  node(123, 2120, 7090);
  node(124, 2120, 7180);
  node(125, 2120, 7270);
  node(126, 2120, 7360);
  node(127, 2120, 7450);
  node(128, 2120, 7540);
  node(129, 2120, 7630);
  node(130, 2120, 7720);
  node(131, 2120, 7810);
  node(132, 2120, 7900);
  node(133, 2120, 7990);
  node(134, 2520, 6760);
  node(135, 2400, 6910);
  node(136, 0, 8130);
  node(137, 280, 8130);
  node(138, 160, 8280);
  node(139, 160, 8370);
  node(140, 160, 8460);
  node(141, 160, 8550);
  node(142, 160, 8640);
  node(143, 160, 8730);
  node(144, 160, 8820);
  node(145, 160, 8910);
  node(146, 160, 9000);
  node(147, 160, 9090);
  node(148, 160, 9180);
  node(149, 160, 9270);
  node(150, 160, 9360);
  node(151, 160, 9450);
  node(152, 160, 9540);
  node(153, 160, 9630);
  node(154, 160, 9720);
  node(155, 160, 9810);
  node(156, 160, 9900);
  node(157, 160, 9990);
  node(158, 160, 10080);
  node(159, 160, 10170);
  node(160, 160, 10260);
  node(161, 160, 10350);
  node(162, 160, 10440);
  node(163, 0, 10580);
  node(164, 280, 10580);
  node(165, 160, 10730);
  node(166, 160, 10820);
  node(167, 160, 10910);
  node(168, 160, 11000);
  node(169, 160, 11090);
  node(170, 160, 11180);
  node(171, 160, 11270);
  node(172, 160, 11360);
  node(173, 160, 11450);
  node(174, 160, 11540);
  node(175, 160, 11630);
  node(176, 160, 11720);
  node(177, 160, 11810);
  node(178, 160, 11900);
  node(179, 160, 11990);
  node(180, 160, 12080);
  node(181, 160, 12170);
  node(182, 160, 12260);
  node(183, 160, 12350);
  node(184, 160, 12440);
  node(185, 160, 12530);
  node(186, 160, 12620);
  node(187, 0, 12760);
  node(188, 280, 12760);
  node(189, 160, 12910);
  node(190, 160, 13000);
  node(191, 160, 13090);
  node(192, 160, 13180);
  node(193, 160, 13270);
  node(194, 160, 13360);
  node(195, 160, 13450);
  node(196, 160, 13540);
  node(197, 160, 13630);
  node(198, 160, 13720);
  node(199, 160, 13810);
  node(200, 160, 13900);
  node(201, 160, 13990);
  node(202, 160, 14080);
  node(203, 160, 14170);
  node(204, 160, 14260);
  node(205, 160, 14350);
  node(206, 160, 14440);
  node(207, 160, 14530);
  node(208, 160, 14620);
  node(209, 160, 14710);
  node(210, 160, 14800);
  node(211, 160, 14890);
  node(212, 160, 14980);
  node(213, 160, 15070);
  node(214, 160, 15160);
  node(215, 160, 15250);
  node(216, 160, 15340);
  node(217, 160, 15430);
  node(218, 160, 15520);
  node(219, 160, 15610);
  node(220, 160, 15700);
  node(221, 160, 15790);
  node(222, 160, 15880);
  node(223, 160, 15970);
  node(224, 160, 16060);
  node(225, 160, 16150);
  node(226, 0, 16290);
  node(227, 280, 16290);
  node(228, 560, 16290);
  node(229, 440, 16440);
  node(230, 840, 16290);
  node(231, 720, 16440);
  node(232, 1120, 16290);
  node(233, 1000, 16440);
  node(234, 1400, 16290);
  node(235, 1280, 16440);
  node(236, 0, 16580);
  node(237, 280, 16580);
  node(238, 560, 16580);
  node(239, 440, 16730);
  node(240, 840, 16580);
  node(241, 720, 16730);
  node(242, 1120, 16580);
  node(243, 1000, 16730);
  node(244, 1400, 16580);
  node(245, 1280, 16730);
  node(246, 0, 16870);
  node(247, 280, 16870);
  node(248, 160, 17020);
  node(249, 160, 17110);
  node(250, 560, 16870);
  node(251, 440, 17020);
  node(252, 440, 17110);
  node(253, 840, 16870);
  node(254, 720, 17020);
  node(255, 720, 17110);
  node(256, 1120, 16870);
  node(257, 1000, 17020);
  node(258, 1000, 17110);
  node(259, 1000, 17200);
  node(260, 1400, 16870);
  node(261, 1280, 17020);
  node(262, 1280, 17110);
  node(263, 1280, 17200);
  node(264, 1680, 16870);
  node(265, 1560, 17020);
  node(266, 1560, 17110);
  node(267, 1560, 17200);
  node(268, 1960, 16870);
  node(269, 1840, 17020);
  node(270, 1840, 17110);
  node(271, 1840, 17200);
  node(272, 2240, 16870);
  node(273, 2120, 17020);
  node(274, 2120, 17110);
  node(275, 2120, 17200);
  node(276, 2520, 16870);
  node(277, 2400, 17020);
  node(278, 2400, 17110);
  node(279, 2400, 17200);
  node(280, 2800, 16870);
  node(281, 2680, 17020);
  node(282, 2680, 17110);
  node(283, 2680, 17200);
  node(284, 3080, 16870);
  node(285, 2960, 17020);
  node(286, 2960, 17110);
  node(287, 2960, 17200);
  node(288, 3360, 16870);
  node(289, 3240, 17020);
  node(290, 3240, 17110);
  node(291, 3240, 17200);
  node(292, 3640, 16870);
  node(293, 3920, 16870);
  node(294, 3800, 17020);
  node(295, 3800, 17110);
  node(296, 3800, 17200);
  node(297, 3800, 17290);
  node(298, 4200, 16870);
  node(299, 4080, 17020);
  node(300, 4080, 17110);
  node(301, 4080, 17200);
  node(302, 4080, 17290);
  node(303, 4480, 16870);
  node(304, 4360, 17020);
  node(305, 4360, 17110);
  node(306, 4360, 17200);
  node(307, 4360, 17290);
  node(308, 4760, 16870);
  node(309, 5040, 16870);
  node(310, 4920, 17020);
  node(311, 4920, 17110);
  node(312, 4920, 17200);
  node(313, 4920, 17290);
  node(314, 5320, 16870);
  node(315, 5200, 17020);
  node(316, 5200, 17110);
  node(317, 5200, 17200);
  node(318, 5200, 17290);
  node(319, 5600, 16870);
  node(320, 5480, 17020);
  node(321, 5480, 17110);
  node(322, 5480, 17200);
  node(323, 5480, 17290);
  node(324, 5880, 16870);
  node(325, 6160, 16870);
  node(326, 6040, 17020);
  node(327, 6040, 17110);
  node(328, 6040, 17200);
  node(329, 6040, 17290);
  node(330, 6440, 16870);
  node(331, 6320, 17020);
  node(332, 6320, 17110);
  node(333, 6320, 17200);
  node(334, 6320, 17290);
  node(335, 6720, 16870);
  node(336, 6600, 17020);
  node(337, 6600, 17110);
  node(338, 6600, 17200);
  node(339, 6600, 17290);
  node(340, 7000, 16870);
  node(341, 7280, 16870);
  node(342, 7160, 17020);
  node(343, 7560, 16870);
  node(344, 7440, 17020);
  node(345, 7840, 16870);
  node(346, 7720, 17020);
  node(347, 8120, 16870);
  node(348, 8400, 16870);
  node(349, 8280, 17020);
  node(351, 0, 17848);
  node(352, 288, 17904);
  node(353, 240, 18056);
  node(354, 576, 17960);
  node(355, 528, 18112);
  node(356, 864, 18016);
  node(357, 1152, 18072);
  node(358, 1440, 18128);
  node(359, 1392, 18280);
  node(360, 1392, 18384);
  node(361, 1728, 18184);
  node(362, 1680, 18336);
  node(363, 2016, 18240);
  node(364, 1968, 18392);
  node(365, 1968, 18496);
  node(366, 2304, 18296);
  node(367, 2592, 18352);
  node(368, 2544, 18504);
  node(369, 2544, 18608);
  node(370, 2880, 18408);
  node(371, 2832, 18560);
  node(372, 0, 18888);
  node(373, 288, 18944);
  node(374, 576, 19000);
  node(375, 864, 19056);
  node(376, 816, 19208);
  node(377, 1152, 19112);
  node(378, 1104, 19264);
  node(379, 1440, 19168);
  node(380, 1392, 19320);
  node(381, 1728, 19224);
  node(382, 1680, 19376);
  node(383, 2016, 19280);
  node(384, 1968, 19432);
  node(385, 2304, 19336);
  node(386, 2256, 19488);
  node(387, 2592, 19392);
  node(388, 2880, 19448);
  node(389, 2832, 19600);
  node(390, 0, 19880);
  node(391, 288, 19936);
  node(392, 240, 20088);
  node(393, 576, 19992);
  node(394, 528, 20144);
  node(395, 528, 20248);
  node(396, 864, 20048);
  node(397, 1152, 20104);
  node(398, 1104, 20256);
  node(399, 1104, 20360);
  node(400, 1104, 20464);
  node(401, 1440, 20160);
  node(402, 1728, 20216);
  node(403, 1680, 20368);
  node(404, 2016, 20272);
  node(405, 1968, 20424);
  node(406, 1968, 20528);
  node(407, 1968, 20632);
  node(408, 1968, 20736);
  node(409, 1968, 20840);
  node(410, 1968, 20944);
  node(411, 1968, 21048);
  node(412, 1968, 21152);
  node(413, 1968, 21256);
  node(414, 1968, 21360);
  node(415, 1968, 21464);
  node(416, 1968, 21568);
  node(417, 1968, 21672);
  node(418, 2304, 20328);
  node(419, 2256, 20480);
  node(420, 2256, 20584);
  node(421, 2592, 20384);
  node(422, 2544, 20536);
  node(423, 2544, 20640);
  node(424, 2880, 20440);
  node(425, 2832, 20592);
  node(426, 2832, 20696);
  node(427, 2832, 20800);
  node(428, 3168, 20496);
  node(429, 3456, 20552);
  node(430, 3408, 20704);
  node(431, 3744, 20608);
  node(432, 3696, 20760);
  node(433, 3696, 20864);
  node(434, 3696, 20968);
  node(435, 3696, 21072);
  node(436, 3696, 21176);
  node(437, 3696, 21280);
  node(438, 3696, 21384);
  node(439, 3696, 21488);
  node(440, 3696, 21592);
  node(441, 3696, 21696);
  node(442, 3696, 21800);
  node(443, 3696, 21904);
  node(444, 3696, 22008);
  node(445, 4032, 20664);
  node(446, 3984, 20816);
  node(447, 3984, 20920);
  node(448, 3984, 21024);
  node(449, 4320, 20720);
  node(450, 4272, 20872);
  node(451, 4272, 20976);
  node(452, 4608, 20776);
  node(453, 4560, 20928);
  node(454, 4560, 21032);
  node(455, 4896, 20832);
  node(456, 4848, 20984);
  node(457, 4848, 21088);
  node(458, 5184, 20888);
  node(459, 5472, 20944);
  node(460, 5424, 21096);
  node(461, 5424, 21200);
  node(462, 5760, 21000);
  node(463, 5712, 21152);
  node(464, 5712, 21256);
  node(465, 6048, 21056);
  node(466, 6000, 21208);
  node(467, 6000, 21312);
  node(468, 6336, 21112);
  node(469, 6624, 21168);
  node(470, 6576, 21320);
  node(471, 6576, 21424);
  node(472, 6912, 21224);
  node(473, 6864, 21376);
  node(474, 6864, 21480);
  node(475, 7200, 21280);
  node(476, 7152, 21432);
  node(477, 7152, 21536);
  node(478, 7488, 21336);
  node(479, 7776, 21392);
  node(480, 7728, 21544);
  node(481, 7728, 21648);
  node(482, 7728, 21752);
  node(483, 8064, 21448);
  node(484, 8016, 21600);
  node(485, 8016, 21704);
  node(486, 8016, 21808);
  node(487, 8352, 21504);
  node(488, 8304, 21656);
  node(489, 8304, 21760);
  node(490, 8304, 21864);
  node(491, 8640, 21560);
  node(492, 8928, 21616);
  node(493, 9216, 21672);
  node(494, 9168, 21824);
  node(495, 0, 22288);
  node(496, 288, 22344);
  node(12, 576, 22400);
  node(497, 864, 22456);
  node(498, 0, 22736);
  node(499, 288, 22792);
  node(500, 240, 22944);
  node(501, 576, 22848);
  node(502, 528, 23000);
  node(503, 864, 22904);
  node(504, 816, 23056);
  node(505, 816, 23160);
  node(506, 1152, 22960);
  node(507, 1104, 23112);
  node(508, 1104, 23216);
  node(509, 1104, 23320);
  node(510, 1104, 23424);
  node(511, 1440, 23016);
  node(512, 1392, 23168);
  node(513, 1392, 23272);
  node(514, 1728, 23072);
  node(515, 1680, 23224);
  node(516, 1680, 23328);
  node(517, 2016, 23128);
  node(518, 1968, 23280);
  node(519, 1968, 23384);
  node(520, 1968, 23488);
  node(521, 1968, 23592);
  node(522, 1968, 23696);
  node(523, 1968, 23800);
  node(524, 1968, 23904);
  node(525, 1968, 24008);
  node(526, 1968, 24112);
  next_id(527);
}


}  // namespace kern_blueprint_388516bead732334

// ---------------------------------------------------------------------------
// The Blueprint of actor "jet_seats"
namespace kern_blueprint_9e16ff6b0cc7a88d {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct jet_seats : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(seats_Seat, pilot, passenger);
  KERN_ENUM(the_seats_Result_, ok, unchanged, invalid, owner_only_pilot, already_seated, seat_occupied, no_pilot, not_occupant);
  KERN_STRUCT(seats_Row) {
    Int owner = 253;
    Int pilot = 253;
    Int passenger = 253;
  };
  KERN_STRUCT(seats_Table) {
    List<seats_Row> rows_ = List<seats_Row>{};
    Int revision_ = 0;
  };

  // ---- functions ----
  struct Table_revisionResult { Int Result = 0; };
  KERN_PURE Table_revisionResult Table_revision(const seats_Table& self) {
    return {self.revision_};
  }

  void Table_clear(seats_Table& self) {
    list_clear(self.rows_);
    self.revision_ = kern::to<Int>(kern::formula([](Float revision_) -> Float { return kern::math::wrap32(revision_ + 1.0); }, kern::to<Float>(self.revision_)));
  }
};

KERN_LAYOUT(jet_seats) {
  node(4, 0, 290);
  node(5, 280, 290);
  node(6, 160, 440);
  node(7, 0, 580);
  node(8, 280, 580);
  node(9, 560, 580);
  node(10, 440, 730);
  node(11, 440, 820);
  next_id(12);
}


}  // namespace kern_blueprint_9e16ff6b0cc7a88d

// ---------------------------------------------------------------------------
// The Blueprint of actor "person_hit"
namespace kern_blueprint_80253b21ddcc8149 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct person_hit : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(BodyPart, none, legs, torso, head);

  // ---- variables ----
  KERN_CONST Float kHipZ = 0.92;
  KERN_CONST Float kHeadUp = 0.73;
  KERN_CONST Float kHeadR = 0.155;

  // ---- functions ----
  struct leanResult { Float Result = 0.0; };
  /// The body's forward lean (people_art.cpp: the running lean).
  KERN_PURE leanResult lean(Float speed = 0.0) {
    return {kern::formula([](Float a) -> Float { return a * 0.035; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : hi < v ? hi : v; }, kern::formula([](Float speed) -> Float { return speed - 2.5; }, speed), 0.0, 5.0))};
  }

  struct box_entryResult { Bool Result = false; };
  /// Entry of the segment from + seg * t (t in 0..1) into a box (centre c, half extents h) whose axes are ax, ay, az; false when it misses.
  box_entryResult box_entry(Float& t_out, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Vector ax = Vector{0.0, 0.0, 0.0}, Vector ay = Vector{0.0, 0.0, 0.0}, Vector az = Vector{0.0, 0.0, 0.0}, Vector h = Vector{0.0, 0.0, 0.0}) {
    Vector o = Vector{0.0, 0.0, 0.0};
    Fixed<Float, 3> po = Fixed<Float, 3>{};
    Fixed<Float, 3> pd = Fixed<Float, 3>{};
    Fixed<Float, 3> hh = Fixed<Float, 3>{};
    Float t0 = 0.0;
    Float t1 = 0.0;
    Bool skip = false;
    Float ta = 0.0;
    Float tb = 0.0;
    Float swapped = 0.0;
    o = vector_sub(from, c);
    po = list_make<Float, "3">(vector_dot(o, ax), vector_dot(o, ay), vector_dot(o, az));
    pd = list_make<Float, "3">(vector_dot(seg, ax), vector_dot(seg, ay), vector_dot(seg, az));
    hh = list_make<Float, "3">(vector_break(h).x, vector_break(h).y, vector_break(h).z);
    t0 = 0.0;
    t1 = 1.0;
    for (auto it10 = flow_for(n10, 0, 2); it10.next();) {
      skip = false;
      if (math_compare<"<">(kern::formula([](Float a) -> Float { return kern::math::abs(a); }, pd[n10.index]), 1e-07)) {
        if (math_compare<">">(kern::formula([](Float a) -> Float { return kern::math::abs(a); }, po[n10.index]), hh[n10.index])) {
          return {false};
        }
        skip = true;
      }
      if (logic_not(skip)) {
        ta = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::div(-a - a_2, a_3); }, hh[n10.index], po[n10.index], pd[n10.index]);
        tb = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::div(a - a_2, a_3); }, hh[n10.index], po[n10.index], pd[n10.index]);
        if (math_compare<">">(ta, tb)) {
          swapped = ta;
          ta = tb;
          tb = swapped;
        }
        t0 = kern::formula([](Float t0, Float ta) -> Float { return kern::math::max(t0, ta); }, t0, ta);
        t1 = kern::formula([](Float t1, Float tb) -> Float { return kern::math::min(t1, tb); }, t1, tb);
        if (math_compare<">">(t0, t1)) {
          return {false};
        }
      }
    }
    t_out = t0;
    return {true};
  }

  struct sphere_entryResult { Bool Result = false; };
  sphere_entryResult sphere_entry(Float& t_out, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Float r = 0.0) {
    Vector o = Vector{0.0, 0.0, 0.0};
    Float a = 0.0;
    Float b = 0.0;
    Float cc = 0.0;
    Float disc = 0.0;
    Float t = 0.0;
    o = vector_sub(from, c);
    a = vector_dot(seg, seg);
    b = vector_dot(o, seg);
    cc = kern::formula([](Float a, Float r) -> Float { return a - r * r; }, vector_dot(o, o), r);
    if (math_compare<"<">(a, 1e-09)) {
      return {false};
    }
    if (math_compare<"<=">(cc, 0.0)) {
      t_out = 0.0;
      return {true};
    }
    disc = kern::formula([](Float b, Float a, Float cc) -> Float { return b * b - a * cc; }, b, a, cc);
    if (logic_or(math_compare<"<">(disc, 0.0), math_compare<">">(b, 0.0))) {
      return {false};
    }
    t = kern::formula([](Float b, Float disc, Float a) -> Float { return kern::math::div(-b - kern::math::sqrt(disc), a); }, b, disc, a);
    if (math_compare<">">(t, 1.0)) {
      return {false};
    }
    t_out = kern::formula([](Float t) -> Float { return kern::math::max(0.0, t); }, t);
    return {true};
  }

  struct hitResult { BodyPart Result = BodyPart::none; };
  /// The part of a standing person (feet at `feet`, heading `yaw` rad - forward is (cos, sin) - walking at `speed` m/s, `scale` 1 for an adult) the segment from + seg enters first.
  hitResult hit(Float& t_out, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float speed = 0.0, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Float pad = 0.0, Float scale = 0.0) {
    Float cy = 0.0;
    Float sy = 0.0;
    Float l = 0.0;
    Float cl = 0.0;
    Float sl = 0.0;
    Vector fx = Vector{0.0, 0.0, 0.0};
    Vector fy = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Vector bz = Vector{0.0, 0.0, 0.0};
    Vector bx = Vector{0.0, 0.0, 0.0};
    Vector hip = Vector{0.0, 0.0, 0.0};
    BodyPart best = BodyPart::none;
    Float bt = 0.0;
    Float t = 0.0;
    cy = kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw);
    sy = kern::formula([](Float yaw) -> Float { return kern::math::sin(yaw); }, yaw);
    l = lean(speed).Result;
    cl = kern::formula([](Float l) -> Float { return kern::math::cos(l); }, l);
    sl = kern::formula([](Float l) -> Float { return kern::math::sin(l); }, l);
    fx = vector_make(cy, sy, 0.0);
    fy = vector_make(kern::formula([](Float sy) -> Float { return -sy; }, sy), cy, 0.0);
    up = Vector{0.0, 0.0, 1.0};
    bz = vector_add(vector_scale(up, cl), vector_scale(fx, sl));
    bx = vector_sub(vector_scale(fx, cl), vector_scale(up, sl));
    hip = vector_add(feet, vector_scale(up, kern::formula([](Float kHipZ, Float scale) -> Float { return kHipZ * scale; }, kHipZ, scale)));
    best = BodyPart::none;
    bt = 2.0;
    t = 0.0;
    n11 = sphere_entry(t, from, seg, vector_add(hip, vector_scale(bz, kern::formula([](Float kHeadUp, Float scale) -> Float { return kHeadUp * scale; }, kHeadUp, scale))), kern::formula([](Float kHeadR, Float pad, Float scale) -> Float { return kHeadR * pad * scale; }, kHeadR, pad, scale));
    if (logic_and(n11.Result, math_compare<"<">(t, bt))) {
      bt = t;
      best = BodyPart::head;
    }
    n12 = box_entry(t, from, seg, vector_add(hip, vector_scale(bz, kern::formula([](Float scale) -> Float { return 0.29 * scale; }, scale))), bx, fy, bz, vector_scale(Vector{0.15, 0.31, 0.31}, kern::formula([](Float pad, Float scale) -> Float { return pad * scale; }, pad, scale)));
    if (logic_and(n12.Result, math_compare<"<">(t, kern::formula([](Float bt) -> Float { return bt - 1e-05; }, bt)))) {
      bt = t;
      best = BodyPart::torso;
    }
    n13 = box_entry(t, from, seg, vector_add(feet, vector_scale(up, kern::formula([](Float scale) -> Float { return 0.47 * scale; }, scale))), fx, fy, up, vector_scale(Vector{0.15, 0.2, 0.47}, kern::formula([](Float pad, Float scale) -> Float { return pad * scale; }, pad, scale)));
    if (logic_and(n13.Result, math_compare<"<">(t, kern::formula([](Float bt) -> Float { return bt - 1e-05; }, bt)))) {
      bt = t;
      best = BodyPart::legs;
    }
    t_out = bt;
    return {best};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  sphere_entryResult n11;
  box_entryResult n12;
  box_entryResult n13;
};

KERN_LAYOUT(person_hit) {
  node(14, 0, 0);
  node(15, 280, 0);
  node(16, 160, 150);
  node(17, 160, 240);
  node(18, 160, 330);
  node(19, 0, 470);
  node(20, 280, 470);
  node(21, 160, 620);
  node(22, 560, 470);
  node(23, 440, 620);
  node(24, 440, 710);
  node(25, 440, 800);
  node(26, 440, 890);
  node(27, 440, 980);
  node(28, 440, 1070);
  node(29, 440, 1160);
  node(30, 840, 470);
  node(31, 720, 620);
  node(32, 720, 710);
  node(33, 720, 800);
  node(34, 720, 890);
  node(35, 1120, 470);
  node(36, 1000, 620);
  node(37, 1000, 710);
  node(38, 1000, 800);
  node(39, 1000, 890);
  node(40, 1400, 470);
  node(41, 1680, 470);
  node(10, 1960, 470);
  node(42, 2240, 470);
  node(43, 2520, 470);
  node(44, 2400, 620);
  node(45, 2400, 710);
  node(46, 2400, 800);
  node(47, 2800, 470);
  node(48, 2680, 620);
  node(49, 2680, 710);
  node(50, 2680, 800);
  node(51, 2680, 890);
  node(52, 3080, 470);
  node(53, 3360, 470);
  node(54, 3640, 470);
  node(55, 3520, 620);
  node(56, 3520, 710);
  node(57, 3920, 470);
  node(58, 3800, 620);
  node(59, 3800, 710);
  node(60, 3800, 800);
  node(61, 3800, 890);
  node(62, 4200, 470);
  node(63, 4080, 620);
  node(64, 4080, 710);
  node(65, 4080, 800);
  node(66, 4080, 890);
  node(67, 4480, 470);
  node(68, 4360, 620);
  node(69, 4360, 710);
  node(70, 4360, 800);
  node(71, 4760, 470);
  node(72, 4640, 620);
  node(73, 5040, 470);
  node(74, 4920, 620);
  node(75, 5320, 470);
  node(76, 5200, 620);
  node(77, 5600, 470);
  node(78, 5480, 620);
  node(79, 5480, 710);
  node(80, 5480, 800);
  node(81, 5880, 470);
  node(82, 5760, 620);
  node(83, 5760, 710);
  node(84, 5760, 800);
  node(85, 6160, 470);
  node(86, 6040, 620);
  node(87, 6040, 710);
  node(88, 6040, 800);
  node(89, 6440, 470);
  node(90, 6720, 470);
  node(91, 6600, 620);
  node(92, 7000, 470);
  node(93, 0, 1300);
  node(94, 280, 1300);
  node(95, 160, 1450);
  node(96, 560, 1300);
  node(97, 440, 1450);
  node(98, 840, 1300);
  node(99, 720, 1450);
  node(100, 720, 1540);
  node(101, 1120, 1300);
  node(102, 1000, 1450);
  node(103, 1000, 1540);
  node(104, 1000, 1630);
  node(105, 1000, 1720);
  node(106, 1400, 1300);
  node(107, 1280, 1450);
  node(108, 1280, 1540);
  node(109, 1680, 1300);
  node(110, 1960, 1300);
  node(111, 1840, 1450);
  node(112, 1840, 1540);
  node(113, 2240, 1300);
  node(114, 2520, 1300);
  node(115, 2800, 1300);
  node(116, 2680, 1450);
  node(117, 2680, 1540);
  node(118, 2680, 1630);
  node(119, 2680, 1720);
  node(120, 3080, 1300);
  node(121, 2960, 1450);
  node(122, 2960, 1540);
  node(123, 2960, 1630);
  node(124, 2960, 1720);
  node(125, 2960, 1810);
  node(126, 3360, 1300);
  node(127, 3640, 1300);
  node(128, 3520, 1450);
  node(129, 3520, 1540);
  node(130, 3520, 1630);
  node(131, 3520, 1720);
  node(132, 3920, 1300);
  node(133, 3800, 1450);
  node(134, 3800, 1540);
  node(135, 4200, 1300);
  node(136, 4480, 1300);
  node(137, 4360, 1450);
  node(138, 4360, 1540);
  node(139, 4760, 1300);
  node(140, 0, 1950);
  node(141, 280, 1950);
  node(142, 160, 2100);
  node(143, 560, 1950);
  node(144, 440, 2100);
  node(145, 840, 1950);
  node(146, 720, 2100);
  node(147, 1120, 1950);
  node(148, 1000, 2100);
  node(149, 1000, 2190);
  node(150, 1400, 1950);
  node(151, 1280, 2100);
  node(152, 1280, 2190);
  node(153, 1680, 1950);
  node(154, 1560, 2100);
  node(155, 1560, 2190);
  node(156, 1560, 2280);
  node(157, 1960, 1950);
  node(158, 1840, 2100);
  node(159, 1840, 2190);
  node(160, 1840, 2280);
  node(161, 1840, 2370);
  node(162, 2240, 1950);
  node(163, 2520, 1950);
  node(164, 2400, 2100);
  node(165, 2400, 2190);
  node(166, 2400, 2280);
  node(167, 2400, 2370);
  node(168, 2400, 2460);
  node(169, 2400, 2550);
  node(170, 2400, 2640);
  node(171, 2800, 1950);
  node(172, 2680, 2100);
  node(173, 2680, 2190);
  node(174, 2680, 2280);
  node(175, 2680, 2370);
  node(176, 2680, 2460);
  node(177, 2680, 2550);
  node(178, 2680, 2640);
  node(179, 3080, 1950);
  node(180, 2960, 2100);
  node(181, 2960, 2190);
  node(182, 2960, 2280);
  node(183, 2960, 2370);
  node(184, 2960, 2460);
  node(185, 3360, 1950);
  node(186, 3640, 1950);
  node(187, 3920, 1950);
  node(11, 4200, 1950);
  node(188, 4080, 2100);
  node(189, 4080, 2190);
  node(190, 4080, 2280);
  node(191, 4080, 2370);
  node(192, 4080, 2460);
  node(193, 4080, 2550);
  node(194, 4080, 2640);
  node(195, 4080, 2730);
  node(196, 4480, 1950);
  node(197, 4360, 2100);
  node(198, 4360, 2190);
  node(199, 4360, 2280);
  node(200, 4360, 2370);
  node(201, 4760, 1950);
  node(202, 4640, 2100);
  node(203, 5040, 1950);
  node(12, 5320, 1950);
  node(204, 5200, 2100);
  node(205, 5200, 2190);
  node(206, 5200, 2280);
  node(207, 5200, 2370);
  node(208, 5200, 2460);
  node(209, 5200, 2550);
  node(210, 5200, 2640);
  node(211, 5200, 2730);
  node(212, 5200, 2820);
  node(213, 5200, 2910);
  node(214, 5600, 1950);
  node(215, 5480, 2100);
  node(216, 5480, 2190);
  node(217, 5480, 2280);
  node(218, 5480, 2370);
  node(219, 5480, 2460);
  node(220, 5880, 1950);
  node(221, 5760, 2100);
  node(222, 6160, 1950);
  node(13, 6440, 1950);
  node(223, 6320, 2100);
  node(224, 6320, 2190);
  node(225, 6320, 2280);
  node(226, 6320, 2370);
  node(227, 6320, 2460);
  node(228, 6320, 2550);
  node(229, 6320, 2640);
  node(230, 6320, 2730);
  node(231, 6320, 2820);
  node(232, 6720, 1950);
  node(233, 6600, 2100);
  node(234, 6600, 2190);
  node(235, 6600, 2280);
  node(236, 6600, 2370);
  node(237, 6600, 2460);
  node(238, 7000, 1950);
  node(239, 6880, 2100);
  node(240, 7280, 1950);
  node(241, 7560, 1950);
  node(242, 7440, 2100);
  node(243, 7840, 1950);
  node(244, 7720, 2100);
  next_id(245);
}


}  // namespace kern_blueprint_80253b21ddcc8149

// ---------------------------------------------------------------------------
// The Blueprint of actor "qr_code"
namespace kern_blueprint_f1248d4ad2e4cc95 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct qr_code : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(qr_Code) {
    Int version = 0;
    Int size = 0;
    List<Int> modules = List<Int>{};
  };

  // ---- functions ----
  struct Code_darkResult { Bool Result = false; };
  KERN_PURE Code_darkResult Code_dark(const qr_Code& self, Int x = 0, Int y = 0) {
    return {logic_and(logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(x), 0.0), math_compare<">=">(kern::to<Float>(y), 0.0)), math_compare<"<">(kern::to<Float>(x), kern::to<Float>(self.size))), math_compare<"<">(kern::to<Float>(y), kern::to<Float>(self.size))), math_compare<"!=">(kern::to<Float>(self.modules[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return y * size + x; }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))]), 0.0))};
  }
};

KERN_LAYOUT(qr_code) {
  node(22, 0, 0);
  node(23, 280, 0);
  node(24, 160, 150);
  node(25, 160, 240);
  node(26, 160, 330);
  node(27, 160, 420);
  node(28, 160, 510);
  node(29, 160, 600);
  node(30, 160, 690);
  node(31, 160, 780);
  node(32, 160, 870);
  node(33, 160, 960);
  node(34, 160, 1050);
  node(35, 160, 1140);
  node(36, 160, 1230);
  node(37, 160, 1320);
  next_id(374);
}


}  // namespace kern_blueprint_f1248d4ad2e4cc95

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_physics_physics"
namespace kern_blueprint_ba8d70e522c7d944 {

using engine_math_math = ::kern_blueprint_388516bead732334::engine_math_math;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_physics_physics : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_ENUM(physics_BodyType, fixed, kinematic, dynamic);
  KERN_ENUM(physics_JointType, revolute, distance, prismatic, wheel, weld, motor, spherical, parallel, filter);
  KERN_ENUM(physics_ShapeKind, box, sphere, capsule, hull, mesh);
  KERN_ENUM(physics_Surface, none, cloth, tpu, floor_);
  KERN_STRUCT(physics_Body) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(physics_Joint) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(physics_QueryFilter) {
    Int64 mask_bits = Int64{0xffffffffffffffff};
    Int64 category_bits = Int64{0xffffffffffffffff};
  };
  KERN_STRUCT(physics_Shape) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(physics_BodySettings) {
    physics_BodyType type = physics_BodyType::dynamic;
    Vector position = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Vector angular_velocity = Vector{0.0, 0.0, 0.0};
    Float linear_damping = 0.0;
    Float angular_damping = 0.0;
    Float gravity_scale = 1.0;
    Bool enable_sleep = true;
    Bool awake = true;
    Bool enabled = true;
    Bool bullet = false;
    Int64 user = Int64{0x0};
  };
  KERN_STRUCT(physics_Pose) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
  };
  KERN_STRUCT(physics_RayHit) {
    Bool hit = false;
    physics_Shape shape = physics_Shape{Int64{0x0}};
    physics_Body body = physics_Body{Int64{0x0}};
    Vector point = Vector{0.0, 0.0, 0.0};
    Vector normal = Vector{0.0, 0.0, 0.0};
    Float fraction = 0.0;
  };
  KERN_STRUCT(physics_ShapeMaterial) {
    Float density = 1.0;
    Float friction = 0.6;
    Float restitution = 0.0;
    Float rolling_resistance = 0.0;
    Bool sensor = false;
    Int64 category_bits = Int64{0x1};
    Int64 mask_bits = Int64{0xffffffffffffffff};
    Int group_index = 0;
    Bool contact_events = true;
    Bool sensor_events = true;
    Bool hit_events = false;
    Int64 user = Int64{0x0};
    physics_Surface surface = physics_Surface::none;
  };
  KERN_STRUCT(physics_JointSettings) {
    physics_JointType type = physics_JointType::revolute;
    physics_Body body_a = physics_Body{Int64{0x0}};
    physics_Body body_b = physics_Body{Int64{0x0}};
    physics_Pose frame_a = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose frame_b = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool collide_connected = false;
    Bool enable_spring = false;
    Float hertz = 0.0;
    Float damping_ratio = 0.0;
    Bool enable_limit = false;
    Float lower = 0.0;
    Float upper = 0.0;
    Bool enable_motor = false;
    Float motor_speed = 0.0;
    Float max_motor_torque = 0.0;
    Float max_motor_force = 0.0;
    Vector motor_velocity = Vector{0.0, 0.0, 0.0};
    Float length = 1.0;
    Float linear_hertz = 0.0;
    Float angular_hertz = 0.0;
    Float linear_damping_ratio = 1.0;
    Float angular_damping_ratio = 1.0;
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Vector angular_velocity = Vector{0.0, 0.0, 0.0};
    Float max_velocity_force = 0.0;
    Float max_velocity_torque = 0.0;
    Float max_spring_force = 0.0;
    Float max_spring_torque = 0.0;
    Bool enable_steering = false;
    Bool enable_steering_limit = false;
    Float steering_hertz = 0.0;
    Float steering_damping_ratio = 0.0;
    Float target_steering = 0.0;
    Float max_steering_torque = 0.0;
    Float steering_lower = 0.0;
    Float steering_upper = 0.0;
    Float max_torque = 0.0;
    Float constraint_hertz = 0.0;
    Float constraint_damping_ratio = 0.0;
    Int64 user = Int64{0x0};
  };
  KERN_STRUCT(physics_ShapeSettings) {
    physics_ShapeKind kind = physics_ShapeKind::box;
    Vector center = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
    Vector size = Vector{1.0, 1.0, 1.0};
    Float radius = 0.5;
    Float length = 1.0;
    List<Vector> points = List<Vector>{};
    List<Int> indices = List<Int>{};
    physics_ShapeMaterial material = physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none};
  };

  // ---- functions ----
  struct Body_validResult { Bool Result = false; };
  KERN_PURE Body_validResult Body_valid(const physics_Body& self) {
    return {logic_not(int64_equal(self.id, Int64{0x0}))};
  }

  struct Body_equalResult { Bool Result = false; };
  KERN_PURE Body_equalResult Body_equal(const physics_Body& self, const physics_Body& o) {
    return {int64_equal(self.id, o.id)};
  }

  struct Shape_validResult { Bool Result = false; };
  KERN_PURE Shape_validResult Shape_valid(const physics_Shape& self) {
    return {logic_not(int64_equal(self.id, Int64{0x0}))};
  }

  struct Joint_validResult { Bool Result = false; };
  KERN_PURE Joint_validResult Joint_valid(const physics_Joint& self) {
    return {logic_not(int64_equal(self.id, Int64{0x0}))};
  }

  struct QueryFilter_newResult { physics_QueryFilter Result = physics_QueryFilter{Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}}; };
  KERN_PURE QueryFilter_newResult QueryFilter_new(Int64 mask = Int64{0x0}, Int64 category = Int64{0x0}) {
    physics_QueryFilter self = physics_QueryFilter{Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}};
    self.mask_bits = mask;
    self.category_bits = category;
    return {self};
  }
};

KERN_LAYOUT(engine_physics_physics) {
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 160, 150);
  node(29, 232, 152);
  node(5, 160, 330);
  node(7, 0, 560);
  node(8, 280, 560);
  node(30, 232, 712);
  node(10, 160, 800);
  node(11, 160, 890);
  node(12, 0, 1030);
  node(13, 280, 1030);
  node(14, 160, 1180);
  node(31, 232, 1184);
  node(16, 160, 1360);
  node(18, 0, 1590);
  node(19, 280, 1590);
  node(20, 160, 1740);
  node(32, 232, 1744);
  node(22, 160, 1920);
  node(24, 0, 2150);
  node(25, 280, 2150);
  node(26, 560, 2150);
  node(27, 840, 2150);
  node(28, 720, 2300);
  next_id(33);
}


}  // namespace kern_blueprint_ba8d70e522c7d944

// ---------------------------------------------------------------------------
// The Blueprint of actor "common"
namespace kern_blueprint_416f27166223668d {

using engine_math_math = ::kern_blueprint_388516bead732334::engine_math_math;
using engine_physics_physics = ::kern_blueprint_ba8d70e522c7d944::engine_physics_physics;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct common : virtual kern::Library, virtual engine_math_math, virtual engine_physics_physics {
  // ---- types ----
  KERN_STRUCT(Rng) {
    Int s = 2654435769;
    Float spare_ = 0.0;
    Bool has_spare_ = false;
  };
  KERN_STRUCT(the_RayResult_) {
    Bool hit = false;
    Float distance = 0.0;
    Vector point = Vector{0.0, 0.0, 0.0};
    Vector normal = Vector{0.0, 0.0, 1.0};
  };
  KERN_STRUCT(BodyState) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector velocity = Vector{0.0, 0.0, 0.0};
    Vector angular = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
  };

  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct clampfResult { Float Result = 0.0; };
  KERN_PURE clampfResult clampf(Float v = 0.0, Float lo = 0.0, Float hi = 0.0) {
    return {kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : v > hi ? hi : v; }, v, lo, hi)};
  }

  struct sign0Result { Float Result = 0.0; };
  KERN_PURE sign0Result sign0(Float v = 0.0) {
    return {kern::formula([](Float v) -> Float { return v > 0.0 ? 1.0 : v < 0.0 ? -1.0 : 0.0; }, v)};
  }

  struct rate_limitResult { Float Result = 0.0; };
  KERN_PURE rate_limitResult rate_limit(Float cur = 0.0, Float target = 0.0, Float max_step = 0.0) {
    return {kern::formula([](Float cur, Float a) -> Float { return cur + a; }, cur, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : hi < v ? hi : v; }, kern::formula([](Float target, Float cur) -> Float { return target - cur; }, target, cur), kern::formula([](Float max_step) -> Float { return -max_step; }, max_step), max_step))};
  }

  struct damp_alphaResult { Float Result = 0.0; };
  /// Exponential approach: the share of the remaining distance covered in dt at `rate` per second.
  KERN_PURE damp_alphaResult damp_alpha(Float rate = 0.0, Float dt = 0.0) {
    return {kern::formula([](Float rate, Float dt) -> Float { return rate > 0.0 && dt > 0.0 ? 1.0 - kern::math::exp(-rate * dt) : 0.0; }, rate, dt)};
  }

  struct wrap_piResult { Float Result = 0.0; };
  KERN_PURE wrap_piResult wrap_pi(Float a = 0.0) {
    Float a_ = 0.0;
    a_ = a;
    while (loop(math_compare<">">(a_, kPi))) {
      a_ = kern::formula([](Float a_, Float kPi) -> Float { return a_ - 2.0 * kPi; }, a_, kPi);
    }
    while (loop(math_compare<"<">(a_, kern::formula([](Float kPi) -> Float { return -kPi; }, kPi)))) {
      a_ += kern::formula([](Float kPi) -> Float { return 2.0 * kPi; }, kPi);
    }
    return {a_};
  }

  struct conjResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE conjResult conj(Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
    return {struct_make<Quat>(kern::formula([](Float x) -> Float { return -x; }, q.x), kern::formula([](Float y) -> Float { return -y; }, q.y), kern::formula([](Float z) -> Float { return -z; }, q.z), q.w)};
  }

  struct to_localResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE to_localResult to_local(Quat q = Quat{0.0, 0.0, 0.0, 1.0}, Vector v = Vector{0.0, 0.0, 0.0}) {
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    a = conj(q).Result;
    return {engine_math_math::Quat_rotate(a, v).Result};
  }

  struct qnormalizeResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE qnormalizeResult qnormalize(Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
    Float l = 0.0;
    l = kern::formula([](Float x, Float y, Float z, Float w) -> Float { return kern::math::sqrt(x * x + y * y + z * z + w * w); }, q.x, q.y, q.z, q.w);
    return {logic_select<Quat>(math_compare<">">(l, 1e-09), struct_make<Quat>(kern::formula([](Float x, Float l) -> Float { return kern::math::div(x, l); }, q.x, l), kern::formula([](Float y, Float l) -> Float { return kern::math::div(y, l); }, q.y, l), kern::formula([](Float z, Float l) -> Float { return kern::math::div(z, l); }, q.z, l), kern::formula([](Float w, Float l) -> Float { return kern::math::div(w, l); }, q.w, l)), struct_make<Quat>(0.0, 0.0, 0.0, 1.0))};
  }

  struct yaw_quatResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE yaw_quatResult yaw_quat(Float yaw = 0.0) {
    return {engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, yaw).Result};
  }

  struct heading_ofResult { Float Result = 0.0; };
  /// Heading (radians, CCW from +X) of a body axis projected on the ground plane.
  KERN_PURE heading_ofResult heading_of(Vector axis = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(axis).y, vector_break(axis).x)};
  }

  struct slerpResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  KERN_PURE slerpResult slerp(Quat a = Quat{0.0, 0.0, 0.0, 1.0}, Quat b = Quat{0.0, 0.0, 0.0, 1.0}, Float t = 0.0) {
    Quat b_ = Quat{0.0, 0.0, 0.0, 1.0};
    Float d = 0.0;
    Float th = 0.0;
    Float s = 0.0;
    Float wa = 0.0;
    Float wb = 0.0;
    b_ = b;
    d = kern::formula([](Float x, Float x_2, Float y, Float y_2, Float z, Float z_2, Float w, Float w_2) -> Float { return x * x_2 + y * y_2 + z * z_2 + w * w_2; }, a.x, b_.x, a.y, b_.y, a.z, b_.z, a.w, b_.w);
    if (math_compare<"<">(d, 0.0)) {
      b_ = struct_make<Quat>(kern::formula([](Float x) -> Float { return -x; }, b_.x), kern::formula([](Float y) -> Float { return -y; }, b_.y), kern::formula([](Float z) -> Float { return -z; }, b_.z), kern::formula([](Float w) -> Float { return -w; }, b_.w));
      d = kern::formula([](Float d) -> Float { return -d; }, d);
    }
    if (math_compare<">">(d, 0.9995)) {
      return {qnormalize(struct_make<Quat>(kern::formula([](Float x, Float x_2, Float t) -> Float { return x + (x_2 - x) * t; }, a.x, b_.x, t), kern::formula([](Float y, Float y_2, Float t) -> Float { return y + (y_2 - y) * t; }, a.y, b_.y, t), kern::formula([](Float z, Float z_2, Float t) -> Float { return z + (z_2 - z) * t; }, a.z, b_.z, t), kern::formula([](Float w, Float w_2, Float t) -> Float { return w + (w_2 - w) * t; }, a.w, b_.w, t))).Result};
    }
    th = kern::formula([](Float d) -> Float { return kern::math::acos(d); }, d);
    s = kern::formula([](Float th) -> Float { return kern::math::sin(th); }, th);
    wa = kern::formula([](Float t, Float th, Float s) -> Float { return kern::math::div(kern::math::sin((1.0 - t) * th), s); }, t, th, s);
    wb = kern::formula([](Float t, Float th, Float s) -> Float { return kern::math::div(kern::math::sin(t * th), s); }, t, th, s);
    return {struct_make<Quat>(kern::formula([](Float x, Float wa, Float x_2, Float wb) -> Float { return x * wa + x_2 * wb; }, a.x, wa, b_.x, wb), kern::formula([](Float y, Float wa, Float y_2, Float wb) -> Float { return y * wa + y_2 * wb; }, a.y, wa, b_.y, wb), kern::formula([](Float z, Float wa, Float z_2, Float wb) -> Float { return z * wa + z_2 * wb; }, a.z, wa, b_.z, wb), kern::formula([](Float w, Float wa, Float w_2, Float wb) -> Float { return w * wa + w_2 * wb; }, a.w, wa, b_.w, wb))};
  }

  struct read_bodyResult { BodyState Result = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE read_bodyResult read_body(Int w = 0, physics_Body b = physics_Body{Int64{0x0}}) {
    physics_Pose p = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Vector angular_velocity = Vector{0.0, 0.0, 0.0};
    pose = engine_world_pose(w, b);
    p = pose;
    linear_velocity = engine_world_linear_velocity(w, b);
    angular_velocity = engine_world_angular_velocity(w, b);
    return {struct_make<BodyState>(p.position, linear_velocity, angular_velocity, p.rotation)};
  }

  struct Rng_seededResult { Rng Result = Rng{2654435769, 0.0, false}; };
  /// The generator for a seed that may be 0 (0: the default seed).
  KERN_PURE Rng_seededResult Rng_seeded(Int seed = 0) {
    return {struct_make<Rng>(kern::to<Int>(kern::formula([](Float seed) -> Float { return seed != 0.0 ? seed : 2654435769.0; }, kern::to<Float>(seed))), 0.0, false)};
  }

  struct Rng_nextResult { Int Result = 0; };
  Rng_nextResult Rng_next(Rng& self) {
    self.s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 13.0))); }, kern::to<Float>(self.s)));
    self.s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::shr(s, 17.0)); }, kern::to<Float>(self.s)));
    self.s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 5.0))); }, kern::to<Float>(self.s)));
    return {self.s};
  }

  struct Rng_unitResult { Float Result = 0.0; };
  Rng_unitResult Rng_unit(Rng& self) {
    n10 = Rng_next(self);
    return {kern::formula([](Float the_result) -> Float { return kern::math::shr(the_result, 8.0) * kern::math::div(1.0, 16777216.0); }, kern::to<Float>(n10.Result))};
  }

  struct Rng_rangeResult { Float Result = 0.0; };
  Rng_rangeResult Rng_range(Rng& self, Float a = 0.0, Float b = 0.0) {
    n11 = Rng_unit(self);
    return {kern::formula([](Float a, Float b, Float the_result) -> Float { return a + (b - a) * the_result; }, a, b, n11.Result)};
  }

  struct Rng_gaussianResult { Float Result = 0.0; };
  /// Standard normal (Box-Muller), as SpawnCity's sensor noise.
  Rng_gaussianResult Rng_gaussian(Rng& self) {
    Float u = 0.0;
    Float v = 0.0;
    Float r = 0.0;
    if (self.has_spare_) {
      self.has_spare_ = false;
      return {self.spare_};
    }
    n12 = Rng_unit(self);
    u = kern::formula([](Float the_result) -> Float { return kern::math::max(1e-12, the_result); }, n12.Result);
    n13 = Rng_unit(self);
    v = n13.Result;
    r = kern::formula([](Float u) -> Float { return kern::math::sqrt(-2.0 * kern::math::log(u)); }, u);
    self.spare_ = kern::formula([](Float r, Float kPi, Float v) -> Float { return r * kern::math::sin(2.0 * kPi * v); }, r, kPi, v);
    self.has_spare_ = true;
    return {kern::formula([](Float r, Float kPi, Float v) -> Float { return r * kern::math::cos(2.0 * kPi * v); }, r, kPi, v)};
  }

  struct castResult { the_RayResult_ Result = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}}; };
  KERN_PURE castResult cast(Int w = 0, Vector origin = Vector{0.0, 0.0, 0.0}, Vector dir = Vector{0.0, 0.0, 0.0}, Float max_len = 0.0, physics_Body self = physics_Body{Int64{0x0}}, Int64 mask = Int64{0x0}) {
    physics_RayHit h = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    physics_RayHit cast_ray = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    the_RayResult_ r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    cast_ray = engine_world_cast_ray(w, origin, vector_scale(dir, max_len), engine_physics_physics::QueryFilter_new(mask, Int64{0xffffffffffffffff}).Result, self);
    h = cast_ray;
    r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    if (logic_not(h.hit)) {
      return {r};
    }
    r.hit = true;
    r.distance = kern::formula([](Float fraction, Float max_len) -> Float { return fraction * max_len; }, h.fraction, max_len);
    r.point = h.point;
    r.normal = h.normal;
    return {r};
  }

  // ---- what nodes give and keep ----
  Rng_nextResult n10;
  Rng_unitResult n11;
  Rng_unitResult n12;
  Rng_unitResult n13;
};

KERN_LAYOUT(common) {
  node(14, 0, 0);
  node(15, 280, 0);
  node(16, 160, 150);
  node(17, 0, 290);
  node(18, 280, 290);
  node(19, 160, 440);
  node(20, 0, 580);
  node(21, 280, 580);
  node(22, 160, 730);
  node(23, 160, 820);
  node(24, 160, 910);
  node(25, 160, 1000);
  node(26, 0, 1140);
  node(27, 280, 1140);
  node(28, 160, 1290);
  node(29, 0, 1430);
  node(30, 280, 1430);
  node(31, 560, 1430);
  node(32, 440, 1580);
  node(33, 440, 1670);
  node(34, 440, 1760);
  node(35, 840, 1430);
  node(36, 720, 1580);
  node(37, 720, 1670);
  node(38, 720, 1760);
  node(39, 1120, 1430);
  node(40, 1000, 1580);
  node(41, 1000, 1670);
  node(42, 1000, 1760);
  node(43, 1000, 1850);
  node(44, 1400, 1430);
  node(45, 1280, 1580);
  node(46, 1280, 1670);
  node(47, 1680, 1430);
  node(48, 1560, 1580);
  node(49, 0, 1990);
  node(50, 280, 1990);
  node(51, 160, 2140);
  node(52, 160, 2230);
  node(53, 160, 2320);
  node(54, 160, 2410);
  node(55, 160, 2500);
  node(56, 160, 2590);
  node(57, 160, 2680);
  node(58, 160, 2770);
  node(59, 0, 2910);
  node(60, 280, 2910);
  node(61, 160, 3060);
  node(62, 560, 2910);
  node(63, 440, 3060);
  node(64, 0, 3200);
  node(65, 280, 3200);
  node(66, 160, 3350);
  node(67, 160, 3440);
  node(68, 160, 3530);
  node(69, 160, 3620);
  node(70, 160, 3710);
  node(71, 560, 3200);
  node(72, 440, 3350);
  node(73, 440, 3440);
  node(74, 440, 3530);
  node(75, 440, 3620);
  node(76, 440, 3710);
  node(77, 440, 3800);
  node(78, 440, 3890);
  node(79, 440, 3980);
  node(80, 440, 4070);
  node(81, 440, 4160);
  node(82, 440, 4250);
  node(83, 440, 4340);
  node(84, 440, 4430);
  node(85, 440, 4520);
  node(86, 440, 4610);
  node(87, 440, 4700);
  node(88, 440, 4790);
  node(89, 0, 4930);
  node(90, 280, 4930);
  node(91, 160, 5080);
  node(92, 0, 5220);
  node(93, 280, 5220);
  node(94, 160, 5370);
  node(95, 160, 5460);
  node(96, 160, 5550);
  node(97, 0, 5690);
  node(98, 280, 5690);
  node(99, 560, 5690);
  node(100, 440, 5840);
  node(101, 440, 5930);
  node(102, 440, 6020);
  node(103, 440, 6110);
  node(104, 440, 6200);
  node(105, 440, 6290);
  node(106, 440, 6380);
  node(107, 440, 6470);
  node(108, 440, 6560);
  node(109, 840, 5690);
  node(110, 720, 5840);
  node(111, 720, 5930);
  node(112, 1120, 5690);
  node(113, 1000, 5840);
  node(114, 1000, 5930);
  node(115, 1000, 6020);
  node(116, 1000, 6110);
  node(117, 1000, 6200);
  node(118, 1000, 6290);
  node(119, 1000, 6380);
  node(120, 1000, 6470);
  node(121, 1000, 6560);
  node(122, 1400, 5690);
  node(123, 1280, 5840);
  node(124, 1280, 5930);
  node(125, 1680, 5690);
  node(126, 1560, 5840);
  node(127, 1560, 5930);
  node(128, 1960, 5690);
  node(129, 1840, 5840);
  node(130, 1840, 5930);
  node(131, 1840, 6020);
  node(132, 1840, 6110);
  node(133, 1840, 6200);
  node(134, 1840, 6290);
  node(135, 1840, 6380);
  node(136, 1840, 6470);
  node(137, 1840, 6560);
  node(138, 1840, 6650);
  node(139, 1840, 6740);
  node(140, 1840, 6830);
  node(141, 1840, 6920);
  node(142, 1840, 7010);
  node(143, 2240, 5690);
  node(144, 2120, 5840);
  node(145, 2120, 5930);
  node(146, 2520, 5690);
  node(147, 2400, 5840);
  node(148, 2400, 5930);
  node(149, 2800, 5690);
  node(150, 2680, 5840);
  node(151, 2680, 5930);
  node(152, 2680, 6020);
  node(153, 3080, 5690);
  node(154, 2960, 5840);
  node(155, 2960, 5930);
  node(156, 2960, 6020);
  node(157, 3360, 5690);
  node(158, 3240, 5840);
  node(159, 3240, 5930);
  node(160, 3240, 6020);
  node(161, 3240, 6110);
  node(162, 3240, 6200);
  node(163, 3240, 6290);
  node(164, 3240, 6380);
  node(165, 3240, 6470);
  node(166, 3240, 6560);
  node(167, 3240, 6650);
  node(168, 3240, 6740);
  node(169, 3240, 6830);
  node(170, 3240, 6920);
  node(171, 3240, 7010);
  node(172, 3240, 7100);
  node(173, 3240, 7190);
  node(174, 3240, 7280);
  node(175, 3240, 7370);
  node(176, 3240, 7460);
  node(177, 3240, 7550);
  node(178, 3240, 7640);
  node(225, 0, 9728);
  node(226, 288, 9784);
  node(227, 240, 9936);
  node(228, 576, 9840);
  node(229, 528, 9992);
  node(230, 864, 9896);
  node(231, 816, 10048);
  node(232, 1152, 9952);
  node(233, 1104, 10104);
  node(234, 1440, 10008);
  node(235, 1392, 10160);
  node(236, 1392, 10264);
  node(237, 1392, 10368);
  node(238, 1392, 10472);
  node(239, 1392, 10576);
  node(179, 0, 7780);
  node(264, 288, 7840);
  node(265, 240, 7992);
  node(181, 160, 7930);
  node(184, 0, 8070);
  node(185, 280, 8070);
  node(186, 160, 8220);
  node(187, 160, 8310);
  node(188, 560, 8070);
  node(189, 440, 8220);
  node(190, 440, 8310);
  node(191, 840, 8070);
  node(192, 720, 8220);
  node(193, 720, 8310);
  node(194, 1120, 8070);
  node(195, 1000, 8220);
  node(196, 0, 8450);
  node(10, 280, 8450);
  node(197, 560, 8450);
  node(198, 440, 8600);
  node(199, 0, 8740);
  node(11, 280, 8740);
  node(200, 560, 8740);
  node(201, 440, 8890);
  node(202, 0, 9030);
  node(203, 280, 9030);
  node(204, 160, 9180);
  node(205, 560, 9030);
  node(206, 840, 9030);
  node(207, 720, 9180);
  node(12, 1120, 9030);
  node(208, 1400, 9030);
  node(209, 1280, 9180);
  node(13, 1680, 9030);
  node(210, 1960, 9030);
  node(211, 2240, 9030);
  node(212, 2120, 9180);
  node(213, 2120, 9270);
  node(214, 2520, 9030);
  node(215, 2400, 9180);
  node(216, 2400, 9270);
  node(217, 2400, 9360);
  node(218, 2400, 9450);
  node(219, 2800, 9030);
  node(220, 3080, 9030);
  node(221, 2960, 9180);
  node(222, 2960, 9270);
  node(223, 2960, 9360);
  node(224, 2960, 9450);
  node(240, 0, 10856);
  node(241, 288, 10912);
  node(242, 240, 11064);
  node(243, 240, 11168);
  node(244, 240, 11272);
  node(246, 576, 10968);
  node(247, 528, 11120);
  node(248, 864, 11024);
  node(249, 1152, 11080);
  node(250, 1104, 11232);
  node(251, 1104, 11336);
  node(252, 1440, 11136);
  node(253, 1392, 11288);
  node(254, 1728, 11192);
  node(255, 2016, 11248);
  node(256, 1968, 11400);
  node(257, 1968, 11504);
  node(258, 2304, 11304);
  node(259, 2256, 11456);
  node(260, 2592, 11360);
  node(261, 2544, 11512);
  node(262, 2880, 11416);
  node(263, 2832, 11568);
  next_id(266);
}


}  // namespace kern_blueprint_416f27166223668d

// ---------------------------------------------------------------------------
// The Blueprint of actor "lamps"
namespace kern_blueprint_315081d4e46950aa {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct lamps : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(lamps_Field_BlastHit) {
    Int lamp = 0;
    Int state = 0;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Float speed = 0.0;
  };
  KERN_STRUCT(lamps_Field_CarHit) {
    Int lamp = -1;
    Bool topple = false;
    Vector impulse = Vector{0.0, 0.0, 0.0};
    Bool clang = false;
  };
  KERN_STRUCT(lamps_Lamp) {
    Vector foot = Vector{0.0, 0.0, 0.0};
    Float yaw = 0.0;
    Vector head = Vector{0.0, 0.0, 0.0};
    Int state = 0;
    Int64 key = Int64{0x0};
    Int lane = 0;
  };
  KERN_STRUCT(lamps_Field) {
    List<lamps_Lamp> lamps_ = List<lamps_Lamp>{};
    Set<Int64> seen_ = Set<Int64>{};
    Map<Int64, Int> broken_ = Map<Int64, Int>{};
    Map<Int64, List<Int>> grid_ = Map<Int64, List<Int>>{};
  };

  // ---- functions ----
  void Field_clear(lamps_Field& self) {
    list_clear(self.lamps_);
    set_clear(self.seen_);
    map_clear(self.grid_);
  }

  void Field_reset_broken(lamps_Field& self) {
    map_clear(self.broken_);
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_))))); it10.next();) {
      self.lamps_[n10.index].state = 0;
    }
  }

  struct Field_brokenResult { Int Result = 0; };
  KERN_PURE Field_brokenResult Field_broken(const lamps_Field& self) {
    return {map_length<Int64, Int>(self.broken_)};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};

KERN_LAYOUT(lamps) {
  node(12, 0, 0);
  node(13, 280, 0);
  node(143, 568, 56);
  node(144, 856, 112);
  node(19, 0, 490);
  node(145, 288, 544);
  node(10, 560, 490);
  node(21, 440, 640);
  node(22, 440, 730);
  node(23, 440, 820);
  node(24, 840, 490);
  node(25, 0, 960);
  node(26, 280, 960);
  node(146, 232, 1112);
  node(28, 160, 1200);
  next_id(147);
}


}  // namespace kern_blueprint_315081d4e46950aa

// ---------------------------------------------------------------------------
// The Blueprint of actor "mvt"
namespace kern_blueprint_5eb1d61c7210d38b {

using shapes = ::kern_blueprint_6025b4a819f918d1::shapes;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct mvt : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_ENUM(mvt_GeomType, unknown, point, line, polygon);
  KERN_ENUM(mvt_Value_Kind, none, string, number, boolean);
  KERN_STRUCT(mvt_Piece) {
    Int at = 0;
    Int end = 0;
  };
  KERN_STRUCT(mvt_Feature) {
    Int64 id = Int64{0x0};
    mvt_GeomType type = mvt_GeomType::unknown;
    List<Int_Int_pair> tags = List<Int_Int_pair>{};
    List<Vector_row> parts = List<Vector_row>{};
  };
  KERN_STRUCT(mvt_Value) {
    mvt_Value_Kind kind = mvt_Value_Kind::none;
    Text s = "";
    Float n = 0.0;
    Bool b = false;
  };
  KERN_STRUCT(Text_mvt_Value_pair) {
    Text first = "";
    mvt_Value second = mvt_Value{mvt_Value_Kind::none, "", 0.0, false};
  };
  KERN_STRUCT(mvt_Layer) {
    Text name = "";
    Int extent = 4096;
    List<Text> keys = List<Text>{};
    List<mvt_Value> values = List<mvt_Value>{};
    List<mvt_Feature> features = List<mvt_Feature>{};
  };
  KERN_STRUCT(mvt_Writer_PendingLayer) {
    Text name = "";
    Int extent = 4096;
    List<Text> keys = List<Text>{};
    List<mvt_Value> values = List<mvt_Value>{};
    List<Int_row> features = List<Int_row>{};
  };
  KERN_STRUCT(mvt_Tile) {
    List<mvt_Layer> layers = List<mvt_Layer>{};
  };
  KERN_STRUCT(mvt_Writer) {
    mvt_Writer_PendingLayer layer_ = mvt_Writer_PendingLayer{"", 4096, List<Text>{}, List<mvt_Value>{}, List<Int_row>{}};
    List<Int> out_ = List<Int>{};
  };
  KERN_STRUCT(mvt_Decoder) {
    List<mvt_Piece> layers_ = List<mvt_Piece>{};
    List<mvt_Piece> features_ = List<mvt_Piece>{};
    Int layer_ = 0;
    Int feature_ = 0;
    Bool layer_open_ = false;
    Bool done_ = true;
    Bool failed_ = false;
    mvt_Tile tile_ = mvt_Tile{List<mvt_Layer>{}};
  };

  // ---- functions ----
  struct Decoder_doneResult { Bool Result = false; };
  KERN_PURE Decoder_doneResult Decoder_done(const mvt_Decoder& self) {
    return {self.done_};
  }

  struct Decoder_failedResult { Bool Result = false; };
  KERN_PURE Decoder_failedResult Decoder_failed(const mvt_Decoder& self) {
    return {self.failed_};
  }
};

KERN_LAYOUT(mvt) {
  node(109, 0, 432);
  node(110, 288, 488);
  node(111, 240, 640);
  node(11, 0, 0);
  node(12, 280, 0);
  node(13, 160, 150);
  next_id(112);
}


}  // namespace kern_blueprint_5eb1d61c7210d38b

// ---------------------------------------------------------------------------
// The Blueprint of actor "road_graph"
namespace kern_blueprint_3a4ae1d57fed8576 {

using shapes = ::kern_blueprint_6025b4a819f918d1::shapes;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct road_graph : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_STRUCT(RoadGraph_Node) {
    Vector p = Vector{0.0, 0.0, 0.0};
    List<Int_Float_pair> edges = List<Int_Float_pair>{};
  };
  KERN_STRUCT(RoadGraph) {
    Int64 built_from = Int64{0xffffffffffffffff};
    List<RoadGraph_Node> nodes_ = List<RoadGraph_Node>{};
    Map<Int64, Int> cells_ = Map<Int64, Int>{};
  };

  // ---- functions ----
  struct RoadGraph_emptyResult { Bool Result = false; };
  KERN_PURE RoadGraph_emptyResult RoadGraph_empty(const RoadGraph& self) {
    return {math_compare<"==">(kern::to<Float>(list_length<RoadGraph_Node>(self.nodes_)), 0.0)};
  }

  struct RoadGraph_sizeResult { Int Result = 0; };
  KERN_PURE RoadGraph_sizeResult RoadGraph_size(const RoadGraph& self) {
    return {list_length<RoadGraph_Node>(self.nodes_)};
  }
};

KERN_LAYOUT(road_graph) {
  node(19, 0, 0);
  node(20, 280, 0);
  node(21, 160, 150);
  node(22, 160, 240);
  node(23, 160, 330);
  node(24, 0, 470);
  node(25, 280, 470);
  node(26, 160, 620);
  node(27, 160, 710);
  next_id(348);
}


}  // namespace kern_blueprint_3a4ae1d57fed8576

// ---------------------------------------------------------------------------
// The Blueprint of actor "perf"
namespace kern_blueprint_1b97e0faae1f6c60 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct perf : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(perf_Slot, logic, physics, stream, draw, count);
  KERN_STRUCT(perf_Governor) {
    Int tier_ = 4;
    Int max_ = 4;
    Bool auto_ = true;
    Float avg_dt_ = 0.016666666666666666;
    Float clock_ = 0.0;
    Float since_change_ = 0.0;
    Float low_ = 0.0;
    Float high_ = 0.0;
    Fixed<Float, 5> block_until_ = Fixed<Float, 5>{};
    Fixed<Float, 5> block_len_ = Fixed<Float, 5>{};
    Int changes_ = 0;
  };
  KERN_STRUCT(perf_Meter) {
    Float start_ = 0.0;
    Float last_ = 0.0;
    Fixed<Float, 4> cur_ = Fixed<Float, 4>{};
    Fixed<Float, 4> avg_ = Fixed<Float, 4>{};
    Float update_ = 0.0;
    Float render_ = 0.0;
    Float gpu_ = -1.0;
  };

  // ---- variables ----
  KERN_CONST Int kTiers = 5;

  // ---- functions ----
  void Meter_begin_frame(perf_Meter& self, Float now_ms = 0.0) {
    self.last_ = now_ms;
    self.start_ = self.last_;
  }

  struct Meter_msResult { Float Result = 0.0; };
  KERN_PURE Meter_msResult Meter_ms(const perf_Meter& self, perf_Slot s = perf_Slot::logic) {
    return {self.avg_[enum_index<perf_Slot>(s)]};
  }

  struct Meter_update_msResult { Float Result = 0.0; };
  KERN_PURE Meter_update_msResult Meter_update_ms(const perf_Meter& self) {
    return {self.update_};
  }

  struct Meter_render_msResult { Float Result = 0.0; };
  KERN_PURE Meter_render_msResult Meter_render_ms(const perf_Meter& self) {
    return {self.render_};
  }

  struct Meter_gpu_msResult { Float Result = 0.0; };
  KERN_PURE Meter_gpu_msResult Meter_gpu_ms(const perf_Meter& self) {
    return {self.gpu_};
  }

  struct Governor_tierResult { Int Result = 0; };
  KERN_PURE Governor_tierResult Governor_tier(const perf_Governor& self) {
    return {self.tier_};
  }

  struct Governor_max_tierResult { Int Result = 0; };
  KERN_PURE Governor_max_tierResult Governor_max_tier(const perf_Governor& self) {
    return {self.max_};
  }

  struct Governor_automaticResult { Bool Result = false; };
  KERN_PURE Governor_automaticResult Governor_automatic(const perf_Governor& self) {
    return {self.auto_};
  }

  void Governor_set_auto(perf_Governor& self) {
    self.auto_ = true;
    self.since_change_ = 0.0;
    self.high_ = 0.0;
    self.low_ = self.high_;
  }

  struct Governor_fpsResult { Float Result = 0.0; };
  KERN_PURE Governor_fpsResult Governor_fps(const perf_Governor& self) {
    return {kern::formula([](Float avg_dt_) -> Float { return avg_dt_ > 0.0 ? kern::math::div(1.0, avg_dt_) : 0.0; }, self.avg_dt_)};
  }

  struct Governor_changesResult { Int Result = 0; };
  KERN_PURE Governor_changesResult Governor_changes(const perf_Governor& self) {
    return {self.changes_};
  }

  struct Governor_blockedResult { Float Result = 0.0; };
  /// Seconds until `tier` may be tried again (0 = free).
  KERN_PURE Governor_blockedResult Governor_blocked(const perf_Governor& self, Int tier = 0) {
    return {kern::formula([](Float tier, Float kTiers, Float a, Float clock_) -> Float { return tier >= 0.0 && tier < kTiers ? kern::math::max(0.0, a - clock_) : 0.0; }, kern::to<Float>(tier), kern::to<Float>(kTiers), self.block_until_[tier], self.clock_)};
  }
};

KERN_LAYOUT(perf) {
  node(12, 0, 0);
  node(13, 280, 0);
  node(14, 560, 0);
  node(15, 440, 150);
  node(16, 0, 290);
  node(17, 280, 290);
  node(18, 160, 440);
  node(19, 160, 530);
  node(20, 0, 670);
  node(21, 280, 670);
  node(22, 160, 820);
  node(23, 0, 960);
  node(24, 280, 960);
  node(25, 160, 1110);
  node(26, 0, 1250);
  node(27, 280, 1250);
  node(28, 160, 1400);
  node(29, 0, 1540);
  node(30, 280, 1540);
  node(31, 160, 1690);
  node(32, 0, 1830);
  node(33, 280, 1830);
  node(34, 160, 1980);
  node(35, 0, 2120);
  node(36, 280, 2120);
  node(37, 160, 2270);
  node(38, 0, 2410);
  node(39, 280, 2410);
  node(40, 560, 2410);
  node(41, 840, 2410);
  node(42, 1120, 2410);
  node(43, 1000, 2560);
  node(44, 0, 2700);
  node(45, 280, 2700);
  node(46, 160, 2850);
  node(47, 160, 2940);
  node(48, 0, 3080);
  node(49, 280, 3080);
  node(50, 160, 3230);
  node(51, 0, 3370);
  node(52, 280, 3370);
  node(53, 160, 3520);
  node(54, 160, 3610);
  node(55, 160, 3700);
  node(56, 160, 3790);
  next_id(228);
}


}  // namespace kern_blueprint_1b97e0faae1f6c60

// ---------------------------------------------------------------------------
// The Blueprint of actor "tiles"
namespace kern_blueprint_508646ea29b97f65 {

using shapes = ::kern_blueprint_6025b4a819f918d1::shapes;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct tiles : virtual kern::Library, virtual shapes {
  // ---- types ----
  KERN_ENUM(AreaKind, water, park, wood, grass, sand, pitch);
  KERN_ENUM(RoadClass, motorway, trunk, primary, secondary, tertiary, minor, service, track, path, pedestrian, river, canal, stream);
  KERN_STRUCT(MapFeatures_Spot) {
    Vector p = Vector{0.0, 0.0, 0.0};
    Float a = 0.0;
  };
  KERN_STRUCT(TileKey) {
    Int z = 14;
    Int x = 0;
    Int y = 0;
  };
  KERN_STRUCT(MapFeatures_Area) {
    List<Vector_row> rings = List<Vector_row>{};
    AreaKind kind = AreaKind::park;
  };
  KERN_STRUCT(MapFeatures_Building) {
    List<Vector_row> rings = List<Vector_row>{};
    Float base = 0.0;
    Float top = 8.0;
    Int seed = 0;
    Float full_top = 0.0;
    Bool leveled = false;
  };
  KERN_STRUCT(MapFeatures_Road) {
    List<Vector> points = List<Vector>{};
    RoadClass cls = RoadClass::minor;
    Bool bridge = false;
  };
  KERN_STRUCT(MapFeatures) {
    List<MapFeatures_Building> buildings = List<MapFeatures_Building>{};
    List<MapFeatures_Road> roads = List<MapFeatures_Road>{};
    List<MapFeatures_Area> areas = List<MapFeatures_Area>{};
    List<MapFeatures_Spot> trees = List<MapFeatures_Spot>{};
    List<MapFeatures_Spot> lamps = List<MapFeatures_Spot>{};
    Bool procedural = false;
  };

  // ---- functions ----
  struct TileKey_equalResult { Bool Result = false; };
  KERN_PURE TileKey_equalResult TileKey_equal(const TileKey& self, const TileKey& o) {
    return {logic_and(logic_and(math_compare<"==">(kern::to<Float>(self.z), kern::to<Float>(o.z)), math_compare<"==">(kern::to<Float>(self.x), kern::to<Float>(o.x))), math_compare<"==">(kern::to<Float>(self.y), kern::to<Float>(o.y)))};
  }

  struct TileKey_hashResult { Int64 Result = Int64{0x0}; };
  KERN_PURE TileKey_hashResult TileKey_hash(const TileKey& self) {
    return {int64_xor(int64_xor(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(x); }, kern::to<Float>(self.x)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(y); }, kern::to<Float>(self.y))))), int64_shift_left(int64_of_int(self.z), 58))};
  }
};

KERN_LAYOUT(tiles) {
  node(20, 0, 0);
  node(21, 280, 0);
  node(22, 160, 150);
  node(23, 160, 240);
  node(24, 160, 330);
  node(25, 160, 420);
  node(26, 160, 510);
  node(27, 160, 600);
  node(28, 160, 690);
  node(29, 160, 780);
  node(30, 160, 870);
  node(31, 160, 960);
  node(32, 160, 1050);
  node(33, 0, 1190);
  node(34, 280, 1190);
  node(549, 232, 1344);
  node(550, 232, 1448);
  node(551, 232, 1552);
  node(552, 232, 1656);
  node(40, 160, 1790);
  node(41, 160, 1880);
  node(553, 232, 1760);
  node(44, 160, 2150);
  node(45, 160, 2240);
  node(554, 232, 1864);
  node(555, 232, 1968);
  node(48, 160, 2510);
  next_id(556);
}


}  // namespace kern_blueprint_508646ea29b97f65

// ---------------------------------------------------------------------------
// The Blueprint of actor "terrain"
namespace kern_blueprint_9a4bfed9ca9dc51a {

using engine_geo_roads = ::kern_blueprint_c57f7a6bcfc775fb::engine_geo_roads;
using engine_physics_physics = ::kern_blueprint_ba8d70e522c7d944::engine_physics_physics;
using tiles = ::kern_blueprint_508646ea29b97f65::tiles;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct terrain : virtual kern::Library, virtual engine_geo_roads, virtual engine_physics_physics, virtual tiles {
  // ---- types ----
  KERN_STRUCT(TerrainStats) {
    Int windows = 0;
    Int dem_tiles = 0;
    Int dem_failed = 0;
    Int render_tiles = 0;
    Int collision_tiles = 0;
    Int water_cells = 0;
    Int craters = 0;
    Int rebuilds = 0;
    Float min_height = 0.0;
    Float max_height = 0.0;
    Bool dem = false;
    Bool settled = false;
  };
  KERN_STRUCT(Terrain_Bridge) {
    Vector c = Vector{0.0, 0.0, 0.0};
    Float hl = 0.0;
    Float hw = 0.0;
    Float yaw = 0.0;
  };
  KERN_STRUCT(Terrain_CorridorSeg) {
    Float ax = 0.0;
    Float ay = 0.0;
    Float dx = 0.0;
    Float dy = 0.0;
    Float l2 = 0.0;
    Float ha = 0.0;
    Float hb = 0.0;
    Float half = 0.0;
  };
  KERN_STRUCT(Terrain_Crater) {
    Vector c = Vector{0.0, 0.0, 0.0};
    Float r = 0.0;
    Float depth = 0.0;
    Float rim = 0.0;
  };
  KERN_STRUCT(Terrain_CraterRec) {
    Float x = 0.0;
    Float y = 0.0;
    Float r = 0.0;
    Float depth = 0.0;
  };
  KERN_STRUCT(Terrain_Grid) {
    Float x0 = 0.0;
    Float y0 = 0.0;
    Float step = 10.0;
    Int n = 0;
    List<Float> h = List<Float>{};
    Float cx = 0.0;
    Float cy = 0.0;
  };
  KERN_STRUCT(Terrain_RenderTile) {
    Int mesh = 0;
    Bool skirt = true;
  };
  KERN_STRUCT(Terrain_WaterBlock) {
    Fixed<Int64, 4> bits = Fixed<Int64, 4>{};
    List<Vector> flow = List<Vector>{};
    List<Float> speed = List<Float>{};
  };
  KERN_STRUCT(Terrain_CollisionTile) {
    physics_Body body = physics_Body{Int64{0x0}};
  };
  KERN_STRUCT(Terrain_CorridorSet) {
    Int64 owner = Int64{0x0};
    Vec4 bounds = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 box = Vec4{0.0, 0.0, 0.0, 0.0};
    Map<Int64, List<Terrain_CorridorSeg>> grid = Map<Int64, List<Terrain_CorridorSeg>>{};
  };
  KERN_STRUCT(Terrain_DemTile) {
    TileKey key = TileKey{14, 0, 0};
    List<Float> h = List<Float>{};
    Int w = 0;
    Int hgt = 0;
    Bool failed = false;
    Bool requested = false;
  };
  KERN_STRUCT(Terrain_Image) {
    TileKey key = TileKey{14, 0, 0};
    List<Float> albedo = List<Float>{};
    Bool requested = false;
    Bool failed = false;
  };
  KERN_STRUCT(Terrain_Job) {
    Terrain_Grid g = Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0};
    Int id = 0;
    Bool side = false;
    Int x0 = 0;
    Int y0 = 0;
    Int x1 = 0;
    Int y1 = 0;
  };
  KERN_STRUCT(Terrain_Tint) {
    Int64 owner = Int64{0x0};
    Vec4 box = Vec4{0.0, 0.0, 0.0, 0.0};
    Vector lin = Vector{0.0, 0.0, 0.0};
    List<Vector_row> rings = List<Vector_row>{};
  };
  KERN_STRUCT(Terrain_CorridorSet_row) {
    List<Terrain_CorridorSet> items = List<Terrain_CorridorSet>{};
  };
  KERN_STRUCT(Terrain) {
    List<Vec4> needs = List<Vec4>{};
    Bool corridors_expected = false;
    Vector lead = Vector{0.0, 0.0, 0.0};
    List<Vector> extra = List<Vector>{};
    List<Terrain_CorridorSet_row> corridors_ = List<Terrain_CorridorSet_row>{};
    Map<Int64, Terrain_Image> images_ = Map<Int64, Terrain_Image>{};
    List<TileKey> image_requests_ = List<TileKey>{};
    Vector image_at_ = Vector{1e+09, 1e+09, 0.0};
    Int r_ = 0;
    Int world_ = 0;
    GeoProjector proj_ = GeoProjector{0.0, 0.0, 1.0};
    Bool dem_ = false;
    Bool settled_ = true;
    Bool grid_ready_ = false;
    Float ref_height_ = 0.0;
    Bool ref_ready_ = false;
    Terrain_Grid grid_ = Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0};
    List<Terrain_Grid> side_ = List<Terrain_Grid>{};
    List<Terrain_Job> job_ = List<Terrain_Job>{};
    Map<Int64, Terrain_DemTile> dem_tiles_ = Map<Int64, Terrain_DemTile>{};
    List<TileKey> requests_ = List<TileKey>{};
    Int dem_failures_ = 0;
    Float retry_at_ = 0.0;
    Map<Int64, Terrain_WaterBlock> water_ = Map<Int64, Terrain_WaterBlock>{};
    List<Terrain_Bridge> bridges_ = List<Terrain_Bridge>{};
    List<Terrain_Crater> craters_ = List<Terrain_Crater>{};
    List<Terrain_Tint> tints_ = List<Terrain_Tint>{};
    Map<Int64, Terrain_RenderTile> render_ = Map<Int64, Terrain_RenderTile>{};
    Map<Int64, Terrain_CollisionTile> collision_ = Map<Int64, Terrain_CollisionTile>{};
    Map<Int64, Int> water_meshes_ = Map<Int64, Int>{};
    Map<Int64, Bool> water_detail_ = Map<Int64, Bool>{};
    Set<Int64> dirty_render_ = Set<Int64>{};
    Set<Int64> dirty_collision_ = Set<Int64>{};
    Set<Int64> dirty_water_ = Set<Int64>{};
    List<Int64> want_render_ = List<Int64>{};
    List<Int64> want_collision_ = List<Int64>{};
    List<Int64> want_water_ = List<Int64>{};
    Vector planned_focus_ = Vector{1e+09, 1e+09, 0.0};
    Vector planned_anchor_ = Vector{1e+09, 1e+09, 0.0};
    List<Vector> planned_extra_ = List<Vector>{};
    Bool replan_ = true;
    List<Vec4> changes_ = List<Vec4>{};
    TerrainStats stats_ = TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false};
  };

  // ---- functions ----
  struct Terrain_settledResult { Bool Result = false; };
  /// The ground's shape is decided (elevation loaded, or flat because there is none).
  KERN_PURE Terrain_settledResult Terrain_settled(const Terrain& self) {
    return {self.settled_};
  }

  struct Terrain_has_demResult { Bool Result = false; };
  KERN_PURE Terrain_has_demResult Terrain_has_dem(const Terrain& self) {
    return {self.grid_ready_};
  }

  struct Terrain_deformedResult { Bool Result = false; };
  /// Whether a circle (x, y, r) touches a crater (the roads drape finely there: city_stream.cpp).
  KERN_PURE Terrain_deformedResult Terrain_deformed(const Terrain& self, Float x = 0.0, Float y = 0.0, Float r = 0.0) {
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Crater>(self.craters_))))); it10.next();) {
      if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt((x - x_2) * (x - x_2) + (y - y_2) * (y - y_2)); }, x, vector_break(self.craters_[n10.index].c).x, y, vector_break(self.craters_[n10.index].c).y), kern::formula([](Float a, Float r) -> Float { return a + r + 2.5; }, self.craters_[n10.index].r, r))) {
        return {true};
      }
    }
    return {false};
  }

  struct Terrain_crater_countResult { Int Result = 0; };
  KERN_PURE Terrain_crater_countResult Terrain_crater_count(const Terrain& self) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Terrain_Crater>(self.craters_))))};
  }

  struct CraterRec_equalResult { Bool Result = false; };
  /// One crater as it goes over the network (destruction sync: game_destruction_sync.cpp).
  KERN_PURE CraterRec_equalResult CraterRec_equal(const Terrain_CraterRec& self, const Terrain_CraterRec& o) {
    return {logic_and(logic_and(logic_and(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return x == x_2; }, self.x, o.x), 0.5), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return y == y_2; }, self.y, o.y), 0.5)), math_compare<">">(kern::formula([](Float r, Float r_2) -> Float { return r == r_2; }, self.r, o.r), 0.5)), math_compare<">">(kern::formula([](Float depth, Float depth_2) -> Float { return depth == depth_2; }, self.depth, o.depth), 0.5))};
  }

  /// Regions changed since the last call (city roads/areas re-lay themselves on them).
  void Terrain_take_changes(Terrain& self, List<Vec4>& out) {
    Int place = 0;
    place = list_length<Vec4>(out);
    list_insert_list(out, place, self.changes_);
    list_clear(self.changes_);
  }

  struct terrarium_heightResult { Float Result = 0.0; };
  /// Terrarium RGB -> metres.
  KERN_PURE terrarium_heightResult terrarium_height(Int r = 0, Int g = 0, Int b = 0) {
    return {kern::formula([](Float r, Float g, Float b) -> Float { return r * 256.0 + g + kern::math::div(b, 256.0) - 32768.0; }, kern::to<Float>(r), kern::to<Float>(g), kern::to<Float>(b))};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};

KERN_LAYOUT(terrain) {
  node(239, 0, 2048);
  node(240, 288, 2104);
  node(241, 240, 2256);
  node(242, 0, 2536);
  node(243, 288, 2592);
  node(244, 240, 2744);
  node(245, 0, 3024);
  node(10, 288, 3080);
  node(246, 240, 3232);
  node(247, 240, 3336);
  node(248, 240, 3440);
  node(249, 576, 3136);
  node(250, 528, 3288);
  node(251, 528, 3392);
  node(252, 528, 3496);
  node(253, 528, 3600);
  node(254, 528, 3704);
  node(255, 528, 3808);
  node(256, 528, 3912);
  node(257, 528, 4016);
  node(258, 864, 3192);
  node(259, 1152, 3248);
  node(260, 0, 4296);
  node(261, 288, 4352);
  node(262, 240, 4504);
  node(263, 240, 4608);
  node(264, 240, 4712);
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 160, 150);
  node(4, 160, 240);
  node(5, 160, 330);
  node(6, 160, 420);
  node(7, 160, 510);
  node(8, 160, 600);
  node(9, 160, 690);
  node(265, 232, 152);
  node(11, 160, 870);
  node(12, 160, 960);
  node(13, 160, 1050);
  node(14, 160, 1140);
  node(15, 160, 1230);
  node(16, 160, 1320);
  node(17, 160, 1410);
  node(18, 160, 1500);
  node(19, 160, 1590);
  node(20, 160, 1680);
  node(21, 160, 1770);
  node(266, 0, 4992);
  node(267, 288, 5048);
  node(268, 240, 5200);
  node(269, 240, 5304);
  node(270, 576, 5104);
  node(271, 528, 5256);
  node(272, 528, 5360);
  node(273, 864, 5160);
  node(274, 0, 5640);
  node(275, 288, 5696);
  node(276, 240, 5848);
  next_id(277);
}


}  // namespace kern_blueprint_9a4bfed9ca9dc51a

// ---------------------------------------------------------------------------
// The Blueprint of actor "city_stream"
namespace kern_blueprint_0265b76c2a3d1e83 {

using terrain = ::kern_blueprint_9a4bfed9ca9dc51a::terrain;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct city_stream : virtual kern::Library, virtual terrain {
  // ---- types ----
  KERN_ENUM(CityStream_Tile_State, requested, decoding, terrain_wait, building, roads, areas, collision, ready, failed);
  KERN_STRUCT(CityStream_AreaRef) {
    Int64 key = Int64{0x0};
    Int64 tile = Int64{0x0};
    Int index = 0;
  };
  KERN_STRUCT(CityStream_BuildingRef) {
    Int64 tile = Int64{0x0};
    Int index = 0;
  };
  KERN_STRUCT(CityStream_Damage) {
    Float height = 0.0;
    Bool leveled = false;
  };
  KERN_STRUCT(CityStream_DamageRec) {
    Int seed = 0;
    Float height = 0.0;
    Bool leveled = false;
  };
  KERN_STRUCT(CityStream_Wall) {
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector dir = Vector{0.0, 0.0, 0.0};
    Vector normal = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Float base = 0.0;
    Float top = 0.0;
    Int seed = 0;
    Int edge = 0;
  };
  KERN_STRUCT(Lane) {
    List<Vector> points = List<Vector>{};
    Float width = 6.0;
    Int64 tile = Int64{0x0};
    Bool alive = true;
  };
  KERN_STRUCT(StreamStats) {
    Int tiles_ready = 0;
    Int tiles_loading = 0;
    Int tiles_failed = 0;
    Int buildings = 0;
    Int colliders = 0;
    Int triangles = 0;
    Int meshes = 0;
    Float last_slice_ms = 0.0;
    Float t_request = -1.0;
    Float t_data = -1.0;
    Float t_decoded = -1.0;
    Float t_terrain = -1.0;
    Float t_built = -1.0;
    Float t_collision = -1.0;
    Float work_ms = 0.0;
    Float worst_ms = 0.0;
    Text worst_step = "";
    Float worst_step_ms = 0.0;
    Int rebuilds = 0;
    Int relays = 0;
    Int uploads = 0;
    Int upload_vertices = 0;
  };
  KERN_STRUCT(CityStream_NearCollision) {
    physics_Body body = physics_Body{Int64{0x0}};
    physics_Body old_body = physics_Body{Int64{0x0}};
    Vector at = Vector{1e+09, 1e+09, 0.0};
    Vector ahead = Vector{1e+09, 1e+09, 0.0};
    Float at_ms = -1000000000.0;
    List<Int64_Int_pair> list = List<Int64_Int_pair>{};
    List<physics_Shape> shapes = List<physics_Shape>{};
    Int next = 0;
    Bool building = false;
    Bool dirty = true;
    Bool built = false;
    List<Vector> mates_at = List<Vector>{};
    Int colliders = 0;
  };
  KERN_STRUCT(CityStream_Tile) {
    CityStream_Tile_State state = CityStream_Tile_State::requested;
    TileKey key = TileKey{14, 0, 0};
    Int tries = 0;
    Float retry_ms = 0.0;
    Bool procedural = false;
    MapFeatures features = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
    Int footprints = 0;
    Int roads = 0;
    Int buildings = 0;
    Int road_builder = 0;
    List<Int> meshes = List<Int>{};
    physics_Body body = physics_Body{Int64{0x0}};
    Int collide_next = 0;
    Int upload_next = 0;
    Int colliders = 0;
    Int triangles = 0;
    Vector center = Vector{0.0, 0.0, 0.0};
    Vec4 bounds = Vec4{0.0, 0.0, 0.0, 0.0};
    List<Int> lay_meshes = List<Int>{};
    List<Int> old_lay = List<Int>{};
    Bool relay = false;
    Bool relay_again = false;
    Vector roads_at = Vector{1e+09, 1e+09, 0.0};
    List<Vector> cent = List<Vector>{};
    Map<Int64, List<Int>> cell_items = Map<Int64, List<Int>>{};
    Set<Int64> want_cells = Set<Int64>{};
    Map<Int64, List<Int>> cell_old = Map<Int64, List<Int>>{};
    Bool cell_uploading = false;
    Int tile_job = 0;
    Int build_job = 0;
    Map<Int64, List<Int>> cell_ids = Map<Int64, List<Int>>{};
    Set<Int64> dirty_cells = Set<Int64>{};
    List<Int64> job_cells = List<Int64>{};
    Int cell_job = 0;
    List<Int> built_bytes = List<Int>{};
    Int built_at = 0;
    Bool prepared = false;
    Int skip = -1;
    physics_Body old_body = physics_Body{Int64{0x0}};
    Bool recollide = false;
  };
  KERN_STRUCT(CityStream) {
    Vector anchor = Vector{0.0, 0.0, 0.0};
    Vector view_lead = Vector{0.0, 0.0, 0.0};
    Int building_budget = 2600;
    Int ring = 1;
    List<Vector> extra_foci = List<Vector>{};
    Int mate_ring = 1;
    Bool training_offline = false;
    Int damage_serial_ = 0;
    Int furniture_serial_ = 0;
    List<Vec4> ground_changes_ = List<Vec4>{};
    CityStream_NearCollision near_ = CityStream_NearCollision{physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, -1000000000.0, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0};
    StreamStats ms_ = StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0};
    Float start_ms_ = 0.0;
    Float worst_ = 0.0;
    Float worst_at_ = 0.0;
    Text worst_step_ = "";
    Float worst_step_ms_ = 0.0;
    Int rebuilds_ = 0;
    Int relays_ = 0;
    Int upload_count_ = 0;
    Int64 upload_vertices_ = Int64{0x0};
    Vector focus_ = Vector{0.0, 0.0, 0.0};
    Vector vis_at_ = Vector{1e+09, 1e+09, 0.0};
    Vector vis_ahead_ = Vector{1e+09, 1e+09, 0.0};
    Float vis_ms_ = -1000000000.0;
    Map<Int, CityStream_Damage> damage_ = Map<Int, CityStream_Damage>{};
    List<Int> damage_log_ = List<Int>{};
    Bool damage_quiet_ = false;
    Int r_ = 0;
    Int world_ = 0;
    GeoProjector proj_ = GeoProjector{0.0, 0.0, 1.0};
    TileKey origin_tile_ = TileKey{14, 0, 0};
    Map<Int64, List<CityStream_Tile>> tiles_ = Map<Int64, List<CityStream_Tile>>{};
    List<TileKey> requests_ = List<TileKey>{};
    List<Lane> lanes_ = List<Lane>{};
    Bool offline_ = false;
    Int real_tiles_ = 0;
    StreamStats stats_ = StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0};
    Terrain terrain_ = Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1e+09, 1e+09, 0.0}, 0, 0, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}};
    List<Vec4> terrain_changes_ = List<Vec4>{};
    Int64 excl_tile_ = Int64{0x0};
    Int excl_index_ = -1;
  };

  // ---- functions ----
  struct CityStream_offlineResult { Bool Result = false; };
  KERN_PURE CityStream_offlineResult CityStream_offline(const CityStream& self) {
    return {self.offline_};
  }

  struct CityStream_has_real_dataResult { Bool Result = false; };
  KERN_PURE CityStream_has_real_dataResult CityStream_has_real_data(const CityStream& self) {
    return {math_compare<">">(kern::to<Float>(self.real_tiles_), 0.0)};
  }

  struct CityStream_furniture_serialResult { Int Result = 0; };
  KERN_PURE CityStream_furniture_serialResult CityStream_furniture_serial(const CityStream& self) {
    return {self.furniture_serial_};
  }

  /// Where the ground changed since the last call (a crater, a new elevation window): what stands on it is put on it again.
  void CityStream_take_ground_changes(CityStream& self, List<Vec4>& out) {
    Int place = 0;
    place = list_length<Vec4>(out);
    list_insert_list(out, place, self.ground_changes_);
    list_clear(self.ground_changes_);
  }

  struct CityStream_damaged_buildingsResult { Int Result = 0; };
  KERN_PURE CityStream_damaged_buildingsResult CityStream_damaged_buildings(const CityStream& self) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))))};
  }

  struct CityStream_damage_serialResult { Int Result = 0; };
  /// Counts every height a building lost (marks on the walls drop what hangs in the air: world_marks.cpp).
  KERN_PURE CityStream_damage_serialResult CityStream_damage_serial(const CityStream& self) {
    return {self.damage_serial_};
  }

  struct CityStream_take_damage_logResult { List<Int> Result = List<Int>{}; };
  /// The seeds whose damage changed since the last call (what this machine has to tell).
  CityStream_take_damage_logResult CityStream_take_damage_log(CityStream& self) {
    var_take(n10, self.damage_log_);
    return {std::move(n10.value)};
  }

  // ---- what nodes give and keep ----
  struct { List<Int> value = List<Int>{}; } n10;
};

KERN_LAYOUT(city_stream) {
  node(121, 0, 280);
  node(122, 288, 336);
  node(123, 240, 488);
  node(124, 0, 768);
  node(125, 288, 824);
  node(126, 240, 976);
  node(127, 240, 1080);
  node(128, 0, 1360);
  node(129, 288, 1416);
  node(130, 240, 1568);
  node(131, 0, 1848);
  node(132, 288, 1904);
  node(133, 240, 2056);
  node(134, 240, 2160);
  node(135, 576, 1960);
  node(136, 528, 2112);
  node(137, 528, 2216);
  node(138, 864, 2016);
  node(139, 0, 2496);
  node(140, 288, 2552);
  node(141, 240, 2704);
  node(142, 240, 2808);
  node(143, 240, 2912);
  node(144, 0, 3192);
  node(145, 288, 3248);
  node(146, 240, 3400);
  node(147, 0, 3680);
  node(10, 288, 3736);
  node(148, 576, 3792);
  next_id(149);
}


}  // namespace kern_blueprint_0265b76c2a3d1e83

// ---------------------------------------------------------------------------
// The Blueprint of actor "wanted"
namespace kern_blueprint_abcbfbad223ba2d6 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct wanted : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(wanted_Crime, person, person_hit, player, car, bus, police_drone, police_officer, police_car, carjacking, bird);
};

KERN_LAYOUT(wanted) {
  next_id(1);
}


}  // namespace kern_blueprint_abcbfbad223ba2d6

// ---------------------------------------------------------------------------
// The Blueprint of actor "nostr_cpp"
namespace kern_blueprint_6cb2cf0da8e6380e {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct nostr_cpp : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(nostr_Hasher) {
    Fixed<Int, 8> h = Fixed<Int, 8>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225};
    Fixed<Int, 64> chunk = Fixed<Int, 64>{};
    Int filled = 0;
    Int64 length = Int64{0x0};
  };
  KERN_STRUCT(nostr_U256) {
    Fixed<Int, 8> w = Fixed<Int, 8>{};
  };
  KERN_STRUCT(nostr_Pt) {
    nostr_U256 x = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 y = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 z = nostr_U256{Fixed<Int, 8>{}};
  };

  // ---- variables ----
  KERN_CONST Fixed<Int, 64> kK = Fixed<Int, 64>{1116352408, 1899447441, 3049323471, 3921009573, 961987163, 1508970993, 2453635748, 2870763221, 3624381080, 310598401, 607225278, 1426881987, 1925078388, 2162078206, 2614888103, 3248222580, 3835390401, 4022224774, 264347078, 604807628, 770255983, 1249150122, 1555081692, 1996064986, 2554220882, 2821834349, 2952996808, 3210313671, 3336571891, 3584528711, 113926993, 338241895, 666307205, 773529912, 1294757372, 1396182291, 1695183700, 1986661051, 2177026350, 2456956037, 2730485921, 2820302411, 3259730800, 3345764771, 3516065817, 3600352804, 4094571909, 275423344, 430227734, 506948616, 659060556, 883997877, 958139571, 1322822218, 1537002063, 1747873779, 1955562222, 2024104815, 2227730452, 2361852424, 2428436474, 2756734187, 3204031479, 3329325298};
  /// (the numbers of the curve, least significant word first: from_hex256 of the hex the standard gives - P = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F, N = FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141)
  KERN_CONST nostr_U256 kP = nostr_U256{Fixed<Int, 8>{4294966319, 4294967294, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295, 4294967295}};
  KERN_CONST nostr_U256 kN = nostr_U256{Fixed<Int, 8>{3493216577, 3218235020, 2940772411, 3132021990, 4294967294, 4294967295, 4294967295, 4294967295}};
  /// (the generator: x = 79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798, y = 483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8, z = 1)
  KERN_CONST nostr_Pt kG = nostr_Pt{nostr_U256{Fixed<Int, 8>{385357720, 1509065051, 768485593, 43777243, 3464956679, 1436574357, 4191992748, 2042521214}}, nostr_U256{Fixed<Int, 8>{4212184248, 2621952143, 2793755673, 4246189128, 235997352, 1571093500, 648266853, 1211816567}}, nostr_U256{Fixed<Int, 8>{}}};

  // ---- functions ----
  struct rotrResult { Int Result = 0; };
  KERN_PURE rotrResult rotr(Int x = 0, Int n = 0) {
    return {kern::to<Int>(kern::formula([](Float x, Float n) -> Float { return kern::math::bor(kern::math::shr(x, n), kern::math::wrap32(kern::math::shl(x, kern::math::wrap32(32.0 - n)))); }, kern::to<Float>(x), kern::to<Float>(n)))};
  }

  void block(Fixed<Int, 8>& h, const Fixed<Int, 64>& p) {
    Fixed<Int, 64> w = Fixed<Int, 64>{};
    Int s0 = 0;
    Int s1 = 0;
    Int a = 0;
    Int b = 0;
    Int c = 0;
    Int d = 0;
    Int e = 0;
    Int f = 0;
    Int g = 0;
    Int k = 0;
    Int t1 = 0;
    Int t2 = 0;
    w = List<Int>{};
    for (auto it10 = flow_for(n10, 0, 15); it10.next();) {
      w[n10.index] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::bor(kern::math::bor(kern::math::bor(kern::math::wrap32(kern::math::shl(a, 24.0)), kern::math::wrap32(kern::math::shl(a_2, 16.0))), kern::math::wrap32(kern::math::shl(a_3, 8.0))), a_4); }, kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index; }, kern::to<Float>(n10.index)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index + 1.0; }, kern::to<Float>(n10.index)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index + 2.0; }, kern::to<Float>(n10.index)))]), kern::to<Float>(p[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index + 3.0; }, kern::to<Float>(n10.index)))])));
    }
    for (auto it11 = flow_for(n11, 16, 63); it11.next();) {
      s0 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::bxor(kern::math::bxor(a, a_2), kern::math::shr(a_3, 3.0)); }, kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return index - 15.0; }, kern::to<Float>(n11.index)))], 7).Result), kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return index - 15.0; }, kern::to<Float>(n11.index)))], 18).Result), kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return index - 15.0; }, kern::to<Float>(n11.index)))])));
      s1 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::bxor(kern::math::bxor(a, a_2), kern::math::shr(a_3, 10.0)); }, kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return index - 2.0; }, kern::to<Float>(n11.index)))], 17).Result), kern::to<Float>(rotr(w[kern::to<Int>(kern::formula([](Float index) -> Float { return index - 2.0; }, kern::to<Float>(n11.index)))], 19).Result), kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return index - 2.0; }, kern::to<Float>(n11.index)))])));
      w[n11.index] = kern::to<Int>(kern::formula([](Float a, Float s0, Float a_2, Float s1) -> Float { return kern::math::wrap32(kern::math::wrap32(kern::math::wrap32(a + s0) + a_2) + s1); }, kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return index - 16.0; }, kern::to<Float>(n11.index)))]), kern::to<Float>(s0), kern::to<Float>(w[kern::to<Int>(kern::formula([](Float index) -> Float { return index - 7.0; }, kern::to<Float>(n11.index)))]), kern::to<Float>(s1)));
    }
    a = h[0];
    b = h[1];
    c = h[2];
    d = h[3];
    e = h[4];
    f = h[5];
    g = h[6];
    k = h[7];
    for (auto it12 = flow_for(n12, 0, 63); it12.next();) {
      t1 = kern::to<Int>(kern::formula([](Float k, Float a, Float a_2, Float a_3, Float e, Float f, Float g, Float a_4, Float a_5) -> Float { return kern::math::wrap32(kern::math::wrap32(kern::math::wrap32(kern::math::wrap32(k + kern::math::bxor(kern::math::bxor(a, a_2), a_3)) + kern::math::bxor(kern::math::band(e, f), kern::math::band(kern::math::wrap32(kern::math::bxor(e, -1.0)), g))) + a_4) + a_5); }, kern::to<Float>(k), kern::to<Float>(rotr(e, 6).Result), kern::to<Float>(rotr(e, 11).Result), kern::to<Float>(rotr(e, 25).Result), kern::to<Float>(e), kern::to<Float>(f), kern::to<Float>(g), kern::to<Float>(list_get<Int>(kK, n12.index)), kern::to<Float>(w[n12.index])));
      t2 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4, Float b, Float c) -> Float { return kern::math::wrap32(kern::math::bxor(kern::math::bxor(a, a_2), a_3) + kern::math::bxor(kern::math::bxor(kern::math::band(a_4, b), kern::math::band(a_4, c)), kern::math::band(b, c))); }, kern::to<Float>(rotr(a, 2).Result), kern::to<Float>(rotr(a, 13).Result), kern::to<Float>(rotr(a, 22).Result), kern::to<Float>(a), kern::to<Float>(b), kern::to<Float>(c)));
      k = g;
      g = f;
      f = e;
      e = kern::to<Int>(kern::formula([](Float d, Float t1) -> Float { return kern::math::wrap32(d + t1); }, kern::to<Float>(d), kern::to<Float>(t1)));
      d = c;
      c = b;
      b = a;
      a = kern::to<Int>(kern::formula([](Float t1, Float t2) -> Float { return kern::math::wrap32(t1 + t2); }, kern::to<Float>(t1), kern::to<Float>(t2)));
    }
    h[0] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::wrap32(a + a_2); }, kern::to<Float>(h[0]), kern::to<Float>(a)));
    h[1] = kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return kern::math::wrap32(a + b); }, kern::to<Float>(h[1]), kern::to<Float>(b)));
    h[2] = kern::to<Int>(kern::formula([](Float a, Float c) -> Float { return kern::math::wrap32(a + c); }, kern::to<Float>(h[2]), kern::to<Float>(c)));
    h[3] = kern::to<Int>(kern::formula([](Float a, Float d) -> Float { return kern::math::wrap32(a + d); }, kern::to<Float>(h[3]), kern::to<Float>(d)));
    h[4] = kern::to<Int>(kern::formula([](Float a, Float e) -> Float { return kern::math::wrap32(a + e); }, kern::to<Float>(h[4]), kern::to<Float>(e)));
    h[5] = kern::to<Int>(kern::formula([](Float a, Float f) -> Float { return kern::math::wrap32(a + f); }, kern::to<Float>(h[5]), kern::to<Float>(f)));
    h[6] = kern::to<Int>(kern::formula([](Float a, Float g) -> Float { return kern::math::wrap32(a + g); }, kern::to<Float>(h[6]), kern::to<Float>(g)));
    h[7] = kern::to<Int>(kern::formula([](Float a, Float k) -> Float { return kern::math::wrap32(a + k); }, kern::to<Float>(h[7]), kern::to<Float>(k)));
  }

  void push(nostr_Hasher& s, Int byte = 0) {
    s.chunk[s.filled] = byte;
    s.filled = kern::to<Int>(kern::formula([](Float filled) -> Float { return kern::math::wrap32(filled + 1.0); }, kern::to<Float>(s.filled)));
    s.length = int64_add(s.length, Int64{0x1});
    if (math_compare<"==">(kern::to<Float>(s.filled), 64.0)) {
      block(s.h, s.chunk);
      s.filled = 0;
    }
  }

  struct finishResult { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  /// The padding (0x80, zeros, the length in bits, big endian) and the hash.
  finishResult finish(nostr_Hasher& s) {
    Int64 bits = Int64{0x0};
    Int b = 0;
    Fixed<Int, 32> out = Fixed<Int, 32>{};
    bits = int64_mul(s.length, Int64{0x8});
    push(s, 128);
    while (loop(math_compare<"!=">(kern::to<Float>(s.filled), 56.0))) {
      push(s, 0);
    }
    b = 7;
    while (loop(math_compare<">=">(kern::to<Float>(b), 0.0))) {
      push(s, kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::band(x, 255.0); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(bits, kern::to<Int>(kern::formula([](Float b) -> Float { return 8.0 * b; }, kern::to<Float>(b)))))))));
      b = kern::to<Int>(kern::formula([](Float b) -> Float { return b - 1.0; }, kern::to<Float>(b)));
    }
    out = List<Int>{};
    for (auto it13 = flow_for(n13, 0, 7); it13.next();) {
      for (auto it14 = flow_for(n14, 0, 3); it14.next();) {
        out[kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return 4.0 * index + index_2; }, kern::to<Float>(n13.index), kern::to<Float>(n14.index)))] = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::band(kern::math::shr(a, 24.0 - 8.0 * index), 255.0); }, kern::to<Float>(s.h[n13.index]), kern::to<Float>(n14.index)));
      }
    }
    return {std::move(out)};
  }

  struct sha256Result { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  sha256Result sha256(const List<Int>& data) {
    nostr_Hasher s = nostr_Hasher{Fixed<Int, 8>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, Fixed<Int, 64>{}, 0, Int64{0x0}};
    Int byte = 0;
    s = nostr_Hasher{Fixed<Int, 8>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, Fixed<Int, 64>{}, 0, Int64{0x0}};
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(data))))); it15.next();) {
      byte = data[n15.index];
      push(s, byte);
    }
    n16 = finish(s);
    return {std::move(n16.Result)};
  }

  struct sha256_2Result { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  sha256_2Result sha256_2(Text text = "") {
    nostr_Hasher s = nostr_Hasher{Fixed<Int, 8>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, Fixed<Int, 64>{}, 0, Int64{0x0}};
    Int c = 0;
    s = nostr_Hasher{Fixed<Int, 8>{1779033703, 3144134277, 1013904242, 2773480762, 1359893119, 2600822924, 528734635, 1541459225}, Fixed<Int, 64>{}, 0, Int64{0x0}};
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(text))))); it17.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(text, n17.index))));
      push(s, kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))));
    }
    n18 = finish(s);
    return {std::move(n18.Result)};
  }

  struct from_beResult { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  KERN_PURE from_beResult from_be(const Fixed<Int, 32>& b) {
    nostr_U256 r = nostr_U256{Fixed<Int, 8>{}};
    r = nostr_U256{Fixed<Int, 8>{}};
    for (auto it19 = flow_for(n19, 0, 7); it19.next();) {
      r.w[kern::to<Int>(kern::formula([](Float index) -> Float { return 7.0 - index; }, kern::to<Float>(n19.index)))] = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::bor(kern::math::bor(kern::math::bor(kern::math::wrap32(kern::math::shl(a, 24.0)), kern::math::wrap32(kern::math::shl(a_2, 16.0))), kern::math::wrap32(kern::math::shl(a_3, 8.0))), a_4); }, kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index; }, kern::to<Float>(n19.index)))]), kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index + 1.0; }, kern::to<Float>(n19.index)))]), kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index + 2.0; }, kern::to<Float>(n19.index)))]), kern::to<Float>(b[kern::to<Int>(kern::formula([](Float index) -> Float { return 4.0 * index + 3.0; }, kern::to<Float>(n19.index)))])));
    }
    return {std::move(r)};
  }

  struct to_beResult { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  KERN_PURE to_beResult to_be(const nostr_U256& a) {
    Fixed<Int, 32> b = Fixed<Int, 32>{};
    b = List<Int>{};
    for (auto it20 = flow_for(n20, 0, 7); it20.next();) {
      for (auto it21 = flow_for(n21, 0, 3); it21.next();) {
        b[kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return 4.0 * index + index_2; }, kern::to<Float>(n20.index), kern::to<Float>(n21.index)))] = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::band(kern::math::shr(a, 24.0 - 8.0 * index), 255.0); }, kern::to<Float>(a.w[kern::to<Int>(kern::formula([](Float index) -> Float { return 7.0 - index; }, kern::to<Float>(n20.index)))]), kern::to<Float>(n21.index)));
      }
    }
    return {std::move(b)};
  }

  struct halfResult { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  /// One half of a signature: its first (0) or its second (1) 32 bytes.
  KERN_PURE halfResult half(const Fixed<Int, 64>& sig, Int which = 0) {
    Fixed<Int, 32> b = Fixed<Int, 32>{};
    b = List<Int>{};
    for (auto it22 = flow_for(n22, 0, 31); it22.next();) {
      b[n22.index] = sig[kern::to<Int>(kern::formula([](Float which, Float index) -> Float { return 32.0 * which + index; }, kern::to<Float>(which), kern::to<Float>(n22.index)))];
    }
    return {std::move(b)};
  }

  struct from_hex_vResult { Int Result = 0; };
  KERN_PURE from_hex_vResult from_hex_v(Int c = 0) {
    return {kern::to<Int>(kern::formula([](Float c) -> Float { return c >= 48.0 && c <= 57.0 ? c - 48.0 : c >= 97.0 && c <= 102.0 ? c - 97.0 + 10.0 : c >= 65.0 && c <= 70.0 ? c - 65.0 + 10.0 : -1.0; }, kern::to<Float>(c)))};
  }

  struct from_hexResult { Bool Result = false; };
  from_hexResult from_hex(Fixed<Int, 32>& out, Text s = "") {
    Int n = 0;
    Int a = 0;
    Int b = 0;
    n = list_length<Int>(out);
    if (math_compare<"!=">(kern::to<Float>(text_length(s)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return 2.0 * n; }, kern::to<Float>(n))))))) {
      return {false};
    }
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it23.next();) {
      a = from_hex_v(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, kern::to<Int>(kern::formula([](Float index) -> Float { return 2.0 * index; }, kern::to<Float>(n23.index)))))))).Result;
      b = from_hex_v(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, kern::to<Int>(kern::formula([](Float index) -> Float { return 2.0 * index + 1.0; }, kern::to<Float>(n23.index)))))))).Result;
      if (logic_or(math_compare<"<">(kern::to<Float>(a), 0.0), math_compare<"<">(kern::to<Float>(b), 0.0))) {
        return {false};
      }
      out[n23.index] = kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return kern::math::band(a * 16.0 + b, 255.0); }, kern::to<Float>(a), kern::to<Float>(b)));
    }
    return {true};
  }

  struct from_hex256Result { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  from_hex256Result from_hex256(Text s = "") {
    Fixed<Int, 32> b = Fixed<Int, 32>{};
    b = List<Int>{};
    from_hex(b, std::move(s));
    return {from_be(b).Result};
  }

  struct cmpResult { Int Result = 0; };
  KERN_PURE cmpResult cmp(const nostr_U256& a, const nostr_U256& b) {
    Int i = 0;
    i = 7;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      if (math_compare<"!=">(kern::to<Float>(a.w[i]), kern::to<Float>(b.w[i]))) {
        return {kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return a < a_2 ? -1.0 : 1.0; }, kern::to<Float>(a.w[i]), kern::to<Float>(b.w[i])))};
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
    }
    return {0};
  }

  struct is_zeroResult { Bool Result = false; };
  KERN_PURE is_zeroResult is_zero(const nostr_U256& a) {
    Int x = 0;
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(a.w))))); it25.next();) {
      x = a.w[n25.index];
      if (math_compare<"!=">(kern::to<Float>(x), 0.0)) {
        return {false};
      }
    }
    return {true};
  }

  struct addResult { Int Result = 0; };
  addResult add(nostr_U256& r, const nostr_U256& a, const nostr_U256& b) {
    Int64 c = Int64{0x0};
    c = Int64{0x0};
    for (auto it26 = flow_for(n26, 0, 7); it26.next();) {
      c = int64_add(c, int64_add(int64_of_int(a.w[n26.index]), int64_of_int(b.w[n26.index])));
      r.w[n26.index] = int64_low(c);
      c = int64_shift_right_unsigned(c, 32);
    }
    return {int64_low(c)};
  }

  struct subResult { Int Result = 0; };
  subResult sub(nostr_U256& r, const nostr_U256& a, const nostr_U256& b) {
    Int64 c = Int64{0x0};
    c = Int64{0x0};
    for (auto it27 = flow_for(n27, 0, 7); it27.next();) {
      c = int64_add(c, int64_sub(int64_of_int(a.w[n27.index]), int64_of_int(b.w[n27.index])));
      r.w[n27.index] = int64_low(c);
      c = int64_shift_right(c, 32);
    }
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a != 0.0 ? 1.0 : 0.0); }, kern::to<Float>(int64_less(c, Int64{0x0}))))};
  }

  struct addmResult { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  addmResult addm(const nostr_U256& a, const nostr_U256& b, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{Fixed<Int, 8>{}};
    Int carry = 0;
    r = nostr_U256{Fixed<Int, 8>{}};
    n28 = add(r, a, b);
    carry = n28.Result;
    if (logic_or(math_compare<"!=">(kern::to<Float>(carry), 0.0), math_compare<">=">(kern::to<Float>(cmp(r, m).Result), 0.0))) {
      sub(r, r, m);
    }
    return {std::move(r)};
  }

  struct submResult { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  submResult subm(const nostr_U256& a, const nostr_U256& b, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{Fixed<Int, 8>{}};
    r = nostr_U256{Fixed<Int, 8>{}};
    n30 = sub(r, a, b);
    if (math_compare<"!=">(kern::to<Float>(n30.Result), 0.0)) {
      add(r, r, m);
    }
    return {std::move(r)};
  }

  struct mulmResult { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  mulmResult mulm(const nostr_U256& a, const nostr_U256& b, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{Fixed<Int, 8>{}};
    Int i = 0;
    r = nostr_U256{Fixed<Int, 8>{}};
    i = 255;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      n32 = addm(r, r, m);
      r = std::move(n32.Result);
      if (math_compare<">">(kern::formula([](Float a, Float i) -> Float { return kern::math::band(kern::math::shr(a, kern::math::mod(i, 32.0)), 1.0) != 0.0; }, kern::to<Float>(b.w[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::trunc(kern::math::div(i, 32.0)); }, kern::to<Float>(i)))]), kern::to<Float>(i)), 0.5)) {
        n33 = addm(r, a, m);
        r = std::move(n33.Result);
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
    }
    return {std::move(r)};
  }

  struct powmResult { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  powmResult powm(const nostr_U256& a, const nostr_U256& e, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{Fixed<Int, 8>{}};
    Int i = 0;
    r = nostr_U256{Fixed<Int, 8>{}};
    r.w[0] = 1;
    i = 255;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      n34 = mulm(r, r, m);
      r = std::move(n34.Result);
      if (math_compare<">">(kern::formula([](Float a, Float i) -> Float { return kern::math::band(kern::math::shr(a, kern::math::mod(i, 32.0)), 1.0) != 0.0; }, kern::to<Float>(e.w[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::trunc(kern::math::div(i, 32.0)); }, kern::to<Float>(i)))]), kern::to<Float>(i)), 0.5)) {
        n35 = mulm(r, a, m);
        r = std::move(n35.Result);
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
    }
    return {std::move(r)};
  }

  struct reduceResult { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  reduceResult reduce(const nostr_U256& a, const nostr_U256& m) {
    nostr_U256 r = nostr_U256{Fixed<Int, 8>{}};
    r = a;
    if (math_compare<">=">(kern::to<Float>(cmp(r, m).Result), 0.0)) {
      sub(r, r, m);
    }
    return {std::move(r)};
  }

  struct fmResult { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  fmResult fm(const nostr_U256& a, const nostr_U256& b) {
    nostr_U256 a_2 = nostr_U256{Fixed<Int, 8>{}};
    a_2 = kP;
    n37 = mulm(a, b, a_2);
    return {std::move(n37.Result)};
  }

  struct faResult { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  faResult fa(const nostr_U256& a, const nostr_U256& b) {
    nostr_U256 a_2 = nostr_U256{Fixed<Int, 8>{}};
    a_2 = kP;
    n38 = addm(a, b, a_2);
    return {std::move(n38.Result)};
  }

  struct fsResult { nostr_U256 Result = nostr_U256{Fixed<Int, 8>{}}; };
  fsResult fs(const nostr_U256& a, const nostr_U256& b) {
    nostr_U256 a_2 = nostr_U256{Fixed<Int, 8>{}};
    a_2 = kP;
    n39 = subm(a, b, a_2);
    return {std::move(n39.Result)};
  }

  struct dblResult { nostr_Pt Result = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}}; };
  dblResult dbl(const nostr_Pt& a) {
    nostr_U256 y2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 s = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_3 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_4 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 x2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 m = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_5 = nostr_U256{Fixed<Int, 8>{}};
    nostr_Pt r = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_U256 a_6 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_7 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 y4 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 y4_8 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_8 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_9 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_10 = nostr_U256{Fixed<Int, 8>{}};
    if (logic_or(is_zero(a.z).Result, is_zero(a.y).Result)) {
      return {struct_make<nostr_Pt>(struct_make<nostr_U256>(List<Int>{}), struct_make<nostr_U256>(List<Int>{}), struct_make<nostr_U256>(List<Int>{}))};
    }
    n40 = fm(a.y, a.y);
    y2 = std::move(n40.Result);
    n41 = fa(a.x, a.x);
    a_3 = std::move(n41.Result);
    n42 = fa(a.x, a.x);
    a_4 = std::move(n42.Result);
    n43 = fa(a_3, a_4);
    a_2 = std::move(n43.Result);
    n44 = fm(a_2, y2);
    s = std::move(n44.Result);
    n45 = fm(a.x, a.x);
    x2 = std::move(n45.Result);
    n46 = fa(x2, x2);
    a_5 = std::move(n46.Result);
    n47 = fa(a_5, x2);
    m = std::move(n47.Result);
    r = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    n48 = fm(m, m);
    a_6 = std::move(n48.Result);
    n49 = fa(s, s);
    a_7 = std::move(n49.Result);
    n50 = fs(a_6, a_7);
    r.x = std::move(n50.Result);
    n51 = fm(y2, y2);
    y4 = std::move(n51.Result);
    n52 = fa(y4, y4);
    y4_8 = std::move(n52.Result);
    n53 = fa(y4_8, y4_8);
    y4_8 = std::move(n53.Result);
    n54 = fa(y4_8, y4_8);
    y4_8 = std::move(n54.Result);
    n55 = fs(s, r.x);
    a_9 = std::move(n55.Result);
    n56 = fm(m, a_9);
    a_8 = std::move(n56.Result);
    n57 = fs(a_8, y4_8);
    r.y = std::move(n57.Result);
    n58 = fa(a.y, a.y);
    a_10 = std::move(n58.Result);
    n59 = fm(a_10, a.z);
    r.z = std::move(n59.Result);
    return {std::move(r)};
  }

  struct add_ptResult { nostr_Pt Result = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}}; };
  add_ptResult add_pt(const nostr_Pt& a, const nostr_Pt& b) {
    nostr_U256 z1z1 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 z2z2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 u1 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 u2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 s1 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 s2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_3 = nostr_U256{Fixed<Int, 8>{}};
    nostr_Pt chosen = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_U256 h = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 r = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 h2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 h3 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 u1h2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_Pt o = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_U256 a_4 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_5 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_6 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_7 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_8 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_9 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_10 = nostr_U256{Fixed<Int, 8>{}};
    if (is_zero(a.z).Result) {
      return {b};
    }
    if (is_zero(b.z).Result) {
      return {a};
    }
    n60 = fm(a.z, a.z);
    z1z1 = std::move(n60.Result);
    n61 = fm(b.z, b.z);
    z2z2 = std::move(n61.Result);
    n62 = fm(a.x, z2z2);
    u1 = std::move(n62.Result);
    n63 = fm(b.x, z1z1);
    u2 = std::move(n63.Result);
    n64 = fm(b.z, z2z2);
    a_2 = std::move(n64.Result);
    n65 = fm(a.y, a_2);
    s1 = std::move(n65.Result);
    n66 = fm(a.z, z1z1);
    a_3 = std::move(n66.Result);
    n67 = fm(b.y, a_3);
    s2 = std::move(n67.Result);
    if (math_compare<"==">(kern::to<Float>(cmp(u1, u2).Result), 0.0)) {
      if (math_compare<"==">(kern::to<Float>(cmp(s1, s2).Result), 0.0)) {
        n69 = dbl(a);
        chosen = std::move(n69.Result);
      } else {
        chosen = struct_make<nostr_Pt>(struct_make<nostr_U256>(List<Int>{}), struct_make<nostr_U256>(List<Int>{}), struct_make<nostr_U256>(List<Int>{}));
      }
      return {std::move(chosen)};
    }
    n70 = fs(u2, u1);
    h = std::move(n70.Result);
    n71 = fs(s2, s1);
    r = std::move(n71.Result);
    n72 = fm(h, h);
    h2 = std::move(n72.Result);
    n73 = fm(h2, h);
    h3 = std::move(n73.Result);
    n74 = fm(u1, h2);
    u1h2 = std::move(n74.Result);
    o = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    n75 = fm(r, r);
    a_5 = std::move(n75.Result);
    n76 = fs(a_5, h3);
    a_4 = std::move(n76.Result);
    n77 = fa(u1h2, u1h2);
    a_6 = std::move(n77.Result);
    n78 = fs(a_4, a_6);
    o.x = std::move(n78.Result);
    n79 = fs(u1h2, o.x);
    a_8 = std::move(n79.Result);
    n80 = fm(r, a_8);
    a_7 = std::move(n80.Result);
    n81 = fm(s1, h3);
    a_9 = std::move(n81.Result);
    n82 = fs(a_7, a_9);
    o.y = std::move(n82.Result);
    n83 = fm(a.z, b.z);
    a_10 = std::move(n83.Result);
    n84 = fm(h, a_10);
    o.z = std::move(n84.Result);
    return {std::move(o)};
  }

  struct mul_ptResult { nostr_Pt Result = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}}; };
  mul_ptResult mul_pt(const nostr_Pt& p, const nostr_U256& k) {
    nostr_Pt r = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    Int i = 0;
    r = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    i = 255;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      n85 = dbl(r);
      r = std::move(n85.Result);
      if (math_compare<">">(kern::formula([](Float a, Float i) -> Float { return kern::math::band(kern::math::shr(a, kern::math::mod(i, 32.0)), 1.0) != 0.0; }, kern::to<Float>(k.w[kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::trunc(kern::math::div(i, 32.0)); }, kern::to<Float>(i)))]), kern::to<Float>(i)), 0.5)) {
        n86 = add_pt(r, p);
        r = std::move(n86.Result);
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
    }
    return {std::move(r)};
  }

  struct affineResult { Bool Result = false; };
  affineResult affine(const nostr_Pt& p, nostr_U256& x, nostr_U256& y) {
    nostr_U256 e = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 zi = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_3 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 zi2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_4 = nostr_U256{Fixed<Int, 8>{}};
    if (is_zero(p.z).Result) {
      return {false};
    }
    e = nostr_U256{Fixed<Int, 8>{}};
    a = kP;
    a_2 = struct_make<nostr_U256>(list_make<Int, "8">(0, 0, 0, 0, 0, 0, 0, 0));
    sub(e, a, a_2);
    a_3 = kP;
    n88 = powm(p.z, e, a_3);
    zi = std::move(n88.Result);
    n89 = fm(zi, zi);
    zi2 = std::move(n89.Result);
    n90 = fm(p.x, zi2);
    x = std::move(n90.Result);
    n91 = fm(zi2, zi);
    a_4 = std::move(n91.Result);
    n92 = fm(p.y, a_4);
    y = std::move(n92.Result);
    return {true};
  }

  struct lift_xResult { Bool Result = false; };
  lift_xResult lift_x(const nostr_U256& x, nostr_Pt& out) {
    nostr_U256 a = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 seven = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 c = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_2 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_3 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 e = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_4 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_5 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 y = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_6 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_7 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_8 = nostr_U256{Fixed<Int, 8>{}};
    a = kP;
    if (math_compare<">=">(kern::to<Float>(cmp(x, a).Result), 0.0)) {
      return {false};
    }
    seven = nostr_U256{Fixed<Int, 8>{}};
    seven.w[0] = 7;
    n93 = fm(x, x);
    a_3 = std::move(n93.Result);
    n94 = fm(a_3, x);
    a_2 = std::move(n94.Result);
    n95 = fa(a_2, seven);
    c = std::move(n95.Result);
    e = nostr_U256{Fixed<Int, 8>{}};
    a_4 = kP;
    a_5 = struct_make<nostr_U256>(list_make<Int, "8">(0, 0, 0, 0, 0, 0, 0, 0));
    add(e, a_4, a_5);
    for (auto it97 = flow_for(n97, 0, 7); it97.next();) {
      e.w[n97.index] = kern::to<Int>(kern::formula([](Float a, Float index, Float a_2) -> Float { return kern::math::bor(kern::math::shr(a, 2.0), index < 7.0 ? kern::math::wrap32(kern::math::shl(a_2, 30.0)) : 0.0); }, kern::to<Float>(e.w[n97.index]), kern::to<Float>(n97.index), kern::to<Float>(e.w[kern::to<Int>(kern::formula([](Float index) -> Float { return index + 1.0; }, kern::to<Float>(n97.index)))])));
    }
    a_6 = kP;
    n98 = powm(c, e, a_6);
    y = std::move(n98.Result);
    n99 = fm(y, y);
    a_7 = std::move(n99.Result);
    if (math_compare<"!=">(kern::to<Float>(cmp(a_7, c).Result), 0.0)) {
      return {false};
    }
    if (math_compare<">">(kern::formula([](Float a) -> Float { return kern::math::band(a, 1.0) != 0.0; }, kern::to<Float>(y.w[0])), 0.5)) {
      a_8 = struct_make<nostr_U256>(List<Int>{});
      n100 = fs(a_8, y);
      y = std::move(n100.Result);
    }
    out.x = x;
    out.y = std::move(y);
    out.z = struct_make<nostr_U256>(list_make<Int, "8">(0, 0, 0, 0, 0, 0, 0, 0));
    return {true};
  }

  struct taggedResult { Fixed<Int, 32> Result = Fixed<Int, 32>{}; };
  taggedResult tagged(const List<Int>& msg, Text tag = "") {
    Fixed<Int, 32> t = Fixed<Int, 32>{};
    List<Int> buf = List<Int>{};
    Int place = 0;
    Int place_2 = 0;
    n101 = sha256_2(std::move(tag));
    t = std::move(n101.Result);
    buf = t;
    place = list_length<Int>(buf);
    list_insert_list(buf, place, std::move(t));
    place_2 = list_length<Int>(buf);
    list_insert_list(buf, place_2, msg);
    n102 = sha256(buf);
    return {std::move(n102.Result)};
  }

  void append(List<Int>& v, const Fixed<Int, 32>& b) {
    Int place = 0;
    place = list_length<Int>(v);
    list_insert_list(v, place, b);
  }

  struct public_keyResult { Bool Result = false; };
  public_keyResult public_key(const Fixed<Int, 32>& secret, Fixed<Int, 32>& out) {
    nostr_U256 d = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_2 = nostr_U256{Fixed<Int, 8>{}};
    Bool either = false;
    nostr_U256 x = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 y = nostr_U256{Fixed<Int, 8>{}};
    nostr_Pt a_3 = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_Pt a_4 = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    d = from_be(secret).Result;
    either = is_zero(d).Result;
    if (logic_not(either)) {
      a_2 = kN;
      either = math_compare<">=">(kern::to<Float>(cmp(d, a_2).Result), 0.0);
    }
    if (either) {
      return {false};
    }
    x = nostr_U256{Fixed<Int, 8>{}};
    y = nostr_U256{Fixed<Int, 8>{}};
    a_4 = kG;
    n103 = mul_pt(a_4, d);
    a_3 = std::move(n103.Result);
    n104 = affine(a_3, x, y);
    if (logic_not(n104.Result)) {
      return {false};
    }
    out = to_be(x).Result;
    return {true};
  }

  struct verifyResult { Bool Result = false; };
  verifyResult verify(const Fixed<Int, 32>& pubkey, const Fixed<Int, 32>& message, const Fixed<Int, 64>& sig) {
    nostr_Pt p = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_U256 a = nostr_U256{Fixed<Int, 8>{}};
    Fixed<Int, 32> rb = Fixed<Int, 32>{};
    nostr_U256 r = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 s = nostr_U256{Fixed<Int, 8>{}};
    Fixed<Int, 32> a_2 = Fixed<Int, 32>{};
    nostr_U256 a_3 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_4 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_5 = nostr_U256{Fixed<Int, 8>{}};
    Bool either = false;
    List<Int> c = List<Int>{};
    nostr_U256 e = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_6 = nostr_U256{Fixed<Int, 8>{}};
    Fixed<Int, 32> a_7 = Fixed<Int, 32>{};
    nostr_U256 a_8 = nostr_U256{Fixed<Int, 8>{}};
    nostr_Pt ep = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_U256 a_9 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 x = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 y = nostr_U256{Fixed<Int, 8>{}};
    nostr_Pt a_10 = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_Pt a_11 = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_Pt a_12 = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    p = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    a = from_be(pubkey).Result;
    n120 = lift_x(a, p);
    if (logic_not(n120.Result)) {
      return {false};
    }
    rb = half(sig, 0).Result;
    r = from_be(rb).Result;
    a_2 = half(sig, 1).Result;
    s = from_be(a_2).Result;
    a_3 = kP;
    either = math_compare<">=">(kern::to<Float>(cmp(r, a_3).Result), 0.0);
    if (logic_not(either)) {
      a_5 = kN;
      either = math_compare<">=">(kern::to<Float>(cmp(s, a_5).Result), 0.0);
    }
    if (either) {
      return {false};
    }
    c = List<Int>{};
    append(c, rb);
    append(c, pubkey);
    append(c, message);
    n121 = tagged(c, "BIP0340/challenge");
    a_7 = std::move(n121.Result);
    a_6 = from_be(a_7).Result;
    a_8 = kN;
    n122 = reduce(a_6, a_8);
    e = std::move(n122.Result);
    n123 = mul_pt(p, e);
    ep = std::move(n123.Result);
    a_9 = struct_make<nostr_U256>(List<Int>{});
    n124 = fs(a_9, ep.y);
    ep.y = std::move(n124.Result);
    x = nostr_U256{Fixed<Int, 8>{}};
    y = nostr_U256{Fixed<Int, 8>{}};
    a_12 = kG;
    n125 = mul_pt(a_12, s);
    a_11 = std::move(n125.Result);
    n126 = add_pt(a_11, ep);
    a_10 = std::move(n126.Result);
    n127 = affine(a_10, x, y);
    if (logic_not(n127.Result)) {
      return {false};
    }
    return {logic_and(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 1.0); }, kern::to<Float>(y.w[0]))))), 0.0), math_compare<"==">(kern::to<Float>(cmp(x, r).Result), 0.0))};
  }

  struct the_signResult { Bool Result = false; };
  the_signResult the_sign(const Fixed<Int, 32>& secret, const Fixed<Int, 32>& message, const Fixed<Int, 32>& aux, Fixed<Int, 64>& sig) {
    nostr_U256 d = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_2 = nostr_U256{Fixed<Int, 8>{}};
    Bool either = false;
    nostr_U256 px = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 py = nostr_U256{Fixed<Int, 8>{}};
    nostr_Pt a_3 = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_Pt a_4 = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_U256 a_5 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_6 = nostr_U256{Fixed<Int, 8>{}};
    Fixed<Int, 32> pxb = Fixed<Int, 32>{};
    Fixed<Int, 32> db = Fixed<Int, 32>{};
    Fixed<Int, 32> ah = Fixed<Int, 32>{};
    List<Int> a_7 = List<Int>{};
    List<Int> m = List<Int>{};
    Fixed<Int, 32> rand = Fixed<Int, 32>{};
    nostr_U256 k = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_8 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_9 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 rx = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 ry = nostr_U256{Fixed<Int, 8>{}};
    nostr_Pt a_10 = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_Pt a_11 = nostr_Pt{nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}, nostr_U256{Fixed<Int, 8>{}}};
    nostr_U256 a_12 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_13 = nostr_U256{Fixed<Int, 8>{}};
    Fixed<Int, 32> rxb = Fixed<Int, 32>{};
    List<Int> c = List<Int>{};
    nostr_U256 e = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_14 = nostr_U256{Fixed<Int, 8>{}};
    Fixed<Int, 32> a_15 = Fixed<Int, 32>{};
    nostr_U256 a_16 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 s = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_17 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_18 = nostr_U256{Fixed<Int, 8>{}};
    nostr_U256 a_19 = nostr_U256{Fixed<Int, 8>{}};
    Fixed<Int, 32> sb = Fixed<Int, 32>{};
    d = from_be(secret).Result;
    either = is_zero(d).Result;
    if (logic_not(either)) {
      a_2 = kN;
      either = math_compare<">=">(kern::to<Float>(cmp(d, a_2).Result), 0.0);
    }
    if (either) {
      return {false};
    }
    px = nostr_U256{Fixed<Int, 8>{}};
    py = nostr_U256{Fixed<Int, 8>{}};
    a_4 = kG;
    n105 = mul_pt(a_4, d);
    a_3 = std::move(n105.Result);
    n106 = affine(a_3, px, py);
    if (logic_not(n106.Result)) {
      return {false};
    }
    if (math_compare<">">(kern::formula([](Float a) -> Float { return kern::math::band(a, 1.0) != 0.0; }, kern::to<Float>(py.w[0])), 0.5)) {
      a_5 = struct_make<nostr_U256>(List<Int>{});
      a_6 = kN;
      n107 = subm(a_5, d, a_6);
      d = std::move(n107.Result);
    }
    pxb = to_be(px).Result;
    db = to_be(d).Result;
    a_7 = aux;
    n108 = tagged(a_7, "BIP0340/aux");
    ah = std::move(n108.Result);
    m = List<Int>{};
    for (auto it109 = flow_for(n109, 0, 31); it109.next();) {
      list_add(m, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(kern::math::bxor(a, a_2), 255.0); }, kern::to<Float>(db[n109.index]), kern::to<Float>(ah[n109.index]))));
    }
    append(m, pxb);
    append(m, message);
    n110 = tagged(m, "BIP0340/nonce");
    rand = std::move(n110.Result);
    a_8 = from_be(rand).Result;
    a_9 = kN;
    n111 = reduce(a_8, a_9);
    k = std::move(n111.Result);
    if (is_zero(k).Result) {
      return {false};
    }
    rx = nostr_U256{Fixed<Int, 8>{}};
    ry = nostr_U256{Fixed<Int, 8>{}};
    a_11 = kG;
    n112 = mul_pt(a_11, k);
    a_10 = std::move(n112.Result);
    n113 = affine(a_10, rx, ry);
    if (logic_not(n113.Result)) {
      return {false};
    }
    if (math_compare<">">(kern::formula([](Float a) -> Float { return kern::math::band(a, 1.0) != 0.0; }, kern::to<Float>(ry.w[0])), 0.5)) {
      a_12 = struct_make<nostr_U256>(List<Int>{});
      a_13 = kN;
      n114 = subm(a_12, k, a_13);
      k = std::move(n114.Result);
    }
    rxb = to_be(rx).Result;
    c = List<Int>{};
    append(c, rxb);
    append(c, pxb);
    append(c, message);
    n115 = tagged(c, "BIP0340/challenge");
    a_15 = std::move(n115.Result);
    a_14 = from_be(a_15).Result;
    a_16 = kN;
    n116 = reduce(a_14, a_16);
    e = std::move(n116.Result);
    a_18 = kN;
    n117 = mulm(e, d, a_18);
    a_17 = std::move(n117.Result);
    a_19 = kN;
    n118 = addm(k, a_17, a_19);
    s = std::move(n118.Result);
    sb = to_be(s).Result;
    for (auto it119 = flow_for(n119, 0, 31); it119.next();) {
      sig[n119.index] = rxb[n119.index];
      sig[kern::to<Int>(kern::formula([](Float index) -> Float { return 32.0 + index; }, kern::to<Float>(n119.index)))] = sb[n119.index];
    }
    n128 = verify(pxb, message, sig);
    return {n128.Result};
  }

  struct escapeResult { Text Result = ""; };
  KERN_PURE escapeResult escape(Text s = "") {
    Text o = "";
    Int c = 0;
    Int selected = 0;
    Text b = "";
    o = "";
    for (auto it131 = flow_for(n131, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it131.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n131.index))));
      selected = c;
      if (math_compare<"==">(kern::to<Float>(selected), 10.0)) {
        o = text_append(o, "\\n", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 34.0)) {
        o = text_append(o, "\\\"", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 92.0)) {
        o = text_append(o, "\\\\", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 13.0)) {
        o = text_append(o, "\\r", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 9.0)) {
        o = text_append(o, "\\t", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 8.0)) {
        o = text_append(o, "\\b", "");
      } else if (math_compare<"==">(kern::to<Float>(selected), 12.0)) {
        o = text_append(o, "\\f", "");
      } else if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
        b = "";
        b = text_format<"\\u%04x">(7, c);
        o = text_append(o, b, "");
      } else {
        o = text_append(o, text_of_code(c), "");
      }
    }
    return {std::move(o)};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  finishResult n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  finishResult n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  addResult n28;
  subResult n30;
  addmResult n32;
  addmResult n33;
  mulmResult n34;
  mulmResult n35;
  mulmResult n37;
  addmResult n38;
  submResult n39;
  fmResult n40;
  faResult n41;
  faResult n42;
  faResult n43;
  fmResult n44;
  fmResult n45;
  faResult n46;
  faResult n47;
  fmResult n48;
  faResult n49;
  fsResult n50;
  fmResult n51;
  faResult n52;
  faResult n53;
  faResult n54;
  fsResult n55;
  fmResult n56;
  fsResult n57;
  faResult n58;
  fmResult n59;
  fmResult n60;
  fmResult n61;
  fmResult n62;
  fmResult n63;
  fmResult n64;
  fmResult n65;
  fmResult n66;
  fmResult n67;
  dblResult n69;
  fsResult n70;
  fsResult n71;
  fmResult n72;
  fmResult n73;
  fmResult n74;
  fmResult n75;
  fsResult n76;
  faResult n77;
  fsResult n78;
  fsResult n79;
  fmResult n80;
  fmResult n81;
  fsResult n82;
  fmResult n83;
  fmResult n84;
  dblResult n85;
  add_ptResult n86;
  powmResult n88;
  fmResult n89;
  fmResult n90;
  fmResult n91;
  fmResult n92;
  fmResult n93;
  fmResult n94;
  faResult n95;
  struct : kern::Node<97> { Int index = 0; } n97;
  powmResult n98;
  fmResult n99;
  fsResult n100;
  sha256_2Result n101;
  sha256Result n102;
  mul_ptResult n103;
  affineResult n104;
  mul_ptResult n105;
  affineResult n106;
  submResult n107;
  taggedResult n108;
  struct : kern::Node<109> { Int index = 0; } n109;
  taggedResult n110;
  reduceResult n111;
  mul_ptResult n112;
  affineResult n113;
  submResult n114;
  taggedResult n115;
  reduceResult n116;
  mulmResult n117;
  addmResult n118;
  struct : kern::Node<119> { Int index = 0; } n119;
  lift_xResult n120;
  taggedResult n121;
  reduceResult n122;
  mul_ptResult n123;
  fsResult n124;
  mul_ptResult n125;
  add_ptResult n126;
  affineResult n127;
  verifyResult n128;
  struct : kern::Node<131> { Int index = 0; } n131;
};

KERN_LAYOUT(nostr_cpp) {
  node(165, 0, 6008);
  node(166, 288, 6064);
  node(167, 240, 6216);
  node(168, 0, 6496);
  node(169, 288, 6552);
  node(10, 280, 940);
  node(170, 568, 1000);
  node(171, 520, 1152);
  node(172, 520, 1256);
  node(173, 520, 1360);
  node(174, 520, 1464);
  node(175, 520, 1568);
  node(176, 520, 1672);
  node(177, 520, 1776);
  node(178, 520, 1880);
  node(179, 520, 1984);
  node(11, 560, 1410);
  node(180, 848, 1464);
  node(181, 800, 1616);
  node(182, 800, 1720);
  node(183, 800, 1824);
  node(184, 800, 1928);
  node(185, 800, 2032);
  node(186, 800, 2136);
  node(187, 800, 2240);
  node(188, 800, 2344);
  node(189, 800, 2448);
  node(190, 1136, 1520);
  node(191, 1088, 1672);
  node(192, 1088, 1776);
  node(193, 1088, 1880);
  node(194, 1088, 1984);
  node(195, 1088, 2088);
  node(196, 1088, 2192);
  node(197, 1088, 2296);
  node(198, 1088, 2400);
  node(199, 1088, 2504);
  node(200, 1424, 1576);
  node(201, 1376, 1728);
  node(202, 1376, 1832);
  node(203, 1376, 1936);
  node(204, 1376, 2040);
  node(205, 1376, 2144);
  node(206, 1376, 2248);
  node(207, 1376, 2352);
  node(208, 1712, 1632);
  node(209, 1664, 1784);
  node(210, 2000, 1688);
  node(211, 1952, 1840);
  node(212, 2288, 1744);
  node(213, 2240, 1896);
  node(214, 2576, 1800);
  node(215, 2528, 1952);
  node(216, 2864, 1856);
  node(217, 2816, 2008);
  node(218, 3152, 1912);
  node(219, 3104, 2064);
  node(220, 3440, 1968);
  node(221, 3392, 2120);
  node(222, 3728, 2024);
  node(223, 3680, 2176);
  node(12, 560, 2240);
  node(224, 848, 2296);
  node(225, 800, 2448);
  node(226, 800, 2552);
  node(227, 800, 2656);
  node(228, 800, 2760);
  node(229, 800, 2864);
  node(230, 800, 2968);
  node(231, 800, 3072);
  node(232, 800, 3176);
  node(233, 800, 3280);
  node(234, 800, 3384);
  node(235, 800, 3488);
  node(236, 800, 3592);
  node(237, 800, 3696);
  node(238, 800, 3800);
  node(239, 1136, 2352);
  node(240, 1088, 2504);
  node(241, 1088, 2608);
  node(242, 1088, 2712);
  node(243, 1088, 2816);
  node(244, 1088, 2920);
  node(245, 1088, 3024);
  node(246, 1088, 3128);
  node(247, 1088, 3232);
  node(248, 1088, 3336);
  node(249, 1088, 3440);
  node(250, 1424, 2408);
  node(251, 1376, 2560);
  node(252, 1712, 2464);
  node(253, 1664, 2616);
  node(254, 2000, 2520);
  node(255, 1952, 2672);
  node(256, 2288, 2576);
  node(257, 2240, 2728);
  node(258, 2240, 2832);
  node(259, 2240, 2936);
  node(260, 2576, 2632);
  node(261, 2528, 2784);
  node(262, 2864, 2688);
  node(263, 2816, 2840);
  node(264, 3152, 2744);
  node(265, 3104, 2896);
  node(266, 3440, 2800);
  node(267, 3392, 2952);
  node(268, 3392, 3056);
  node(269, 3392, 3160);
  node(270, 3728, 2856);
  node(271, 3680, 3008);
  node(272, 3680, 3112);
  node(273, 3680, 3216);
  node(274, 4016, 2912);
  node(275, 3968, 3064);
  node(276, 3968, 3168);
  node(277, 3968, 3272);
  node(278, 4304, 2968);
  node(279, 4256, 3120);
  node(280, 4256, 3224);
  node(281, 4256, 3328);
  node(282, 4592, 3024);
  node(283, 4544, 3176);
  node(284, 4544, 3280);
  node(285, 4544, 3384);
  node(286, 4880, 3080);
  node(287, 4832, 3232);
  node(288, 4832, 3336);
  node(289, 4832, 3440);
  node(290, 5168, 3136);
  node(291, 5120, 3288);
  node(292, 5120, 3392);
  node(293, 5120, 3496);
  node(294, 5456, 3192);
  node(295, 5408, 3344);
  node(296, 5408, 3448);
  node(297, 5408, 3552);
  node(298, 5744, 3248);
  node(299, 5696, 3400);
  node(300, 5696, 3504);
  node(301, 5696, 3608);
  node(302, 0, 6832);
  node(303, 288, 6888);
  node(304, 240, 7040);
  node(305, 576, 6944);
  node(306, 528, 7096);
  node(307, 528, 7200);
  node(308, 864, 7000);
  node(309, 816, 7152);
  node(310, 816, 7256);
  node(311, 1152, 7056);
  node(312, 1104, 7208);
  node(313, 1104, 7312);
  node(314, 1440, 7112);
  node(315, 1728, 7168);
  node(316, 0, 7592);
  node(317, 288, 7648);
  node(318, 240, 7800);
  node(319, 240, 7904);
  node(320, 576, 7704);
  node(321, 864, 7760);
  node(322, 816, 7912);
  node(323, 816, 8016);
  node(324, 1152, 7816);
  node(325, 1440, 7872);
  node(326, 1728, 7928);
  node(327, 1680, 8080);
  node(328, 1680, 8184);
  node(329, 2016, 7984);
  node(330, 1968, 8136);
  node(331, 1968, 8240);
  node(332, 1968, 8344);
  node(333, 1968, 8448);
  node(334, 1968, 8552);
  node(335, 1968, 8656);
  node(336, 2304, 8040);
  node(337, 2256, 8192);
  node(338, 2256, 8296);
  node(339, 2592, 8096);
  node(13, 2880, 8152);
  node(14, 3168, 8208);
  node(340, 3456, 8264);
  node(341, 3408, 8416);
  node(342, 3408, 8520);
  node(343, 3408, 8624);
  node(344, 3744, 8320);
  node(345, 3696, 8472);
  node(346, 0, 8936);
  node(347, 288, 8992);
  node(15, 576, 9048);
  node(348, 528, 9200);
  node(349, 528, 9304);
  node(350, 528, 9408);
  node(351, 864, 9104);
  node(352, 816, 9256);
  node(353, 1152, 9160);
  node(354, 1104, 9312);
  node(16, 1120, 3720);
  node(355, 1408, 3776);
  node(356, 0, 9688);
  node(357, 288, 9744);
  node(17, 576, 9800);
  node(358, 528, 9952);
  node(359, 528, 10056);
  node(360, 864, 9856);
  node(361, 816, 10008);
  node(362, 816, 10112);
  node(363, 1152, 9912);
  node(364, 1104, 10064);
  node(365, 1104, 10168);
  node(18, 1960, 4010);
  node(366, 2248, 4064);
  node(367, 0, 10448);
  node(368, 288, 10504);
  node(19, 576, 10560);
  node(369, 864, 10616);
  node(370, 816, 10768);
  node(371, 816, 10872);
  node(372, 816, 10976);
  node(373, 816, 11080);
  node(374, 816, 11184);
  node(375, 816, 11288);
  node(376, 816, 11392);
  node(377, 816, 11496);
  node(378, 816, 11600);
  node(379, 816, 11704);
  node(380, 1152, 10672);
  node(381, 1104, 10824);
  node(382, 0, 11984);
  node(383, 288, 12040);
  node(20, 576, 12096);
  node(21, 864, 12152);
  node(384, 1152, 12208);
  node(385, 1104, 12360);
  node(386, 1104, 12464);
  node(387, 1104, 12568);
  node(388, 1104, 12672);
  node(389, 1440, 12264);
  node(390, 1392, 12416);
  node(391, 0, 12952);
  node(392, 288, 13008);
  node(22, 576, 13064);
  node(393, 864, 13120);
  node(394, 816, 13272);
  node(395, 816, 13376);
  node(396, 1152, 13176);
  node(397, 1104, 13328);
  node(398, 0, 13656);
  node(399, 288, 13712);
  node(400, 240, 13864);
  node(401, 0, 14144);
  node(402, 288, 14200);
  node(403, 240, 14352);
  node(404, 240, 14456);
  node(405, 576, 14256);
  node(406, 528, 14408);
  node(407, 528, 14512);
  node(408, 528, 14616);
  node(409, 528, 14720);
  node(410, 528, 14824);
  node(411, 864, 14312);
  node(23, 1152, 14368);
  node(412, 1104, 14520);
  node(413, 1104, 14624);
  node(414, 1440, 14424);
  node(415, 1392, 14576);
  node(416, 1392, 14680);
  node(417, 1392, 14784);
  node(418, 1392, 14888);
  node(419, 1728, 14480);
  node(420, 1680, 14632);
  node(421, 1680, 14736);
  node(422, 1680, 14840);
  node(423, 1680, 14944);
  node(424, 2016, 14536);
  node(425, 1968, 14688);
  node(426, 1968, 14792);
  node(427, 1968, 14896);
  node(428, 1968, 15000);
  node(429, 1968, 15104);
  node(430, 2304, 14592);
  node(431, 2592, 14648);
  node(432, 2544, 14800);
  node(433, 2544, 14904);
  node(434, 2544, 15008);
  node(435, 2880, 14704);
  node(436, 0, 15384);
  node(437, 288, 15440);
  node(24, 576, 15496);
  node(438, 864, 15552);
  node(439, 816, 15704);
  node(440, 0, 15984);
  node(441, 288, 16040);
  node(442, 576, 16096);
  node(443, 528, 16248);
  node(444, 528, 16352);
  node(445, 864, 16152);
  node(446, 816, 16304);
  node(447, 816, 16408);
  node(448, 816, 16512);
  node(449, 816, 16616);
  node(450, 816, 16720);
  node(451, 1152, 16208);
  node(452, 1104, 16360);
  node(453, 1104, 16464);
  node(454, 1104, 16568);
  node(455, 1104, 16672);
  node(456, 1104, 16776);
  node(457, 1440, 16264);
  node(458, 1392, 16416);
  node(459, 1392, 16520);
  node(460, 1728, 16320);
  node(461, 0, 17056);
  node(25, 288, 17112);
  node(462, 240, 17264);
  node(463, 240, 17368);
  node(464, 240, 17472);
  node(465, 576, 17168);
  node(466, 528, 17320);
  node(467, 864, 17224);
  node(468, 816, 17376);
  node(469, 816, 17480);
  node(470, 1152, 17280);
  node(471, 1440, 17336);
  node(472, 0, 17760);
  node(473, 288, 17816);
  node(26, 576, 17872);
  node(474, 864, 17928);
  node(475, 816, 18080);
  node(476, 816, 18184);
  node(477, 816, 18288);
  node(478, 816, 18392);
  node(479, 816, 18496);
  node(480, 816, 18600);
  node(481, 816, 18704);
  node(68, 1120, 1410);
  node(482, 1072, 1560);
  node(483, 1072, 1664);
  node(484, 1408, 1464);
  node(485, 1360, 1616);
  node(486, 1360, 1720);
  node(487, 1696, 1520);
  node(488, 1648, 1672);
  node(489, 1648, 1776);
  node(490, 0, 18984);
  node(491, 288, 19040);
  node(27, 576, 19096);
  node(492, 864, 19152);
  node(493, 816, 19304);
  node(494, 816, 19408);
  node(495, 816, 19512);
  node(496, 816, 19616);
  node(497, 816, 19720);
  node(498, 816, 19824);
  node(499, 816, 19928);
  node(500, 1152, 19208);
  node(501, 1104, 19360);
  node(502, 1104, 19464);
  node(503, 1440, 19264);
  node(504, 1392, 19416);
  node(505, 1392, 19520);
  node(506, 1728, 19320);
  node(507, 1680, 19472);
  node(508, 1680, 19576);
  node(509, 1680, 19680);
  node(510, 0, 20208);
  node(511, 288, 20264);
  node(28, 576, 20320);
  node(512, 864, 20376);
  node(513, 1152, 20432);
  node(514, 1104, 20584);
  node(515, 1104, 20688);
  node(516, 1104, 20792);
  node(517, 1104, 20896);
  node(518, 1104, 21000);
  node(29, 1440, 20488);
  node(519, 1728, 20544);
  node(520, 1680, 20696);
  node(521, 0, 21280);
  node(522, 288, 21336);
  node(30, 576, 21392);
  node(523, 864, 21448);
  node(524, 816, 21600);
  node(31, 1152, 21504);
  node(525, 1440, 21560);
  node(526, 1392, 21712);
  node(527, 0, 21992);
  node(528, 288, 22048);
  node(529, 576, 22104);
  node(530, 864, 22160);
  node(531, 816, 22312);
  node(532, 816, 22416);
  node(32, 1152, 22216);
  node(533, 1440, 22272);
  node(534, 1728, 22328);
  node(535, 1680, 22480);
  node(536, 1680, 22584);
  node(537, 1680, 22688);
  node(538, 1680, 22792);
  node(539, 1680, 22896);
  node(129, 1560, 4610);
  node(33, 2016, 22384);
  node(130, 2240, 4010);
  node(540, 2528, 4064);
  node(132, 2400, 4160);
  node(133, 2400, 4250);
  node(134, 2800, 4010);
  node(135, 2680, 4160);
  node(136, 0, 4750);
  node(137, 280, 4750);
  node(138, 560, 4750);
  node(139, 840, 4750);
  node(140, 1120, 4750);
  node(141, 1000, 4900);
  node(142, 1000, 4990);
  node(34, 1408, 4808);
  node(143, 1680, 4750);
  node(144, 1960, 4750);
  node(145, 1840, 4900);
  node(146, 1840, 4990);
  node(147, 1840, 5080);
  node(148, 1840, 5170);
  node(149, 1840, 5260);
  node(150, 1840, 5350);
  node(35, 2248, 4808);
  node(151, 2520, 4750);
  node(152, 2800, 4750);
  node(153, 2680, 4900);
  node(154, 2680, 4990);
  node(155, 3080, 4750);
  node(156, 2960, 4900);
  node(157, 0, 5490);
  node(158, 280, 5490);
  node(159, 160, 5640);
  node(160, 560, 5490);
  node(161, 440, 5640);
  node(162, 440, 5730);
  node(36, 848, 5544);
  node(163, 1120, 5490);
  node(164, 1000, 5640);
  node(541, 0, 23176);
  node(542, 288, 23232);
  node(543, 240, 23384);
  node(37, 576, 23288);
  node(544, 864, 23344);
  node(545, 0, 23664);
  node(546, 288, 23720);
  node(547, 240, 23872);
  node(38, 576, 23776);
  node(548, 864, 23832);
  node(549, 0, 24152);
  node(550, 288, 24208);
  node(551, 240, 24360);
  node(39, 576, 24264);
  node(552, 864, 24320);
  node(553, 0, 24640);
  node(554, 288, 24696);
  node(555, 240, 24848);
  node(556, 240, 24952);
  node(557, 240, 25056);
  node(558, 576, 24752);
  node(559, 528, 24904);
  node(560, 528, 25008);
  node(561, 528, 25112);
  node(562, 528, 25216);
  node(40, 864, 24808);
  node(563, 1152, 24864);
  node(41, 1440, 24920);
  node(564, 1728, 24976);
  node(42, 2016, 25032);
  node(565, 2304, 25088);
  node(43, 2592, 25144);
  node(566, 2880, 25200);
  node(44, 3168, 25256);
  node(567, 3456, 25312);
  node(45, 3744, 25368);
  node(568, 4032, 25424);
  node(46, 4320, 25480);
  node(569, 4608, 25536);
  node(47, 4896, 25592);
  node(570, 5184, 25648);
  node(571, 5472, 25704);
  node(48, 5760, 25760);
  node(572, 6048, 25816);
  node(49, 6336, 25872);
  node(573, 6624, 25928);
  node(50, 6912, 25984);
  node(574, 7200, 26040);
  node(51, 7488, 26096);
  node(575, 7776, 26152);
  node(52, 8064, 26208);
  node(576, 8352, 26264);
  node(53, 8640, 26320);
  node(577, 8928, 26376);
  node(54, 9216, 26432);
  node(578, 9504, 26488);
  node(55, 9792, 26544);
  node(579, 10080, 26600);
  node(56, 10368, 26656);
  node(580, 10656, 26712);
  node(57, 10944, 26768);
  node(581, 11232, 26824);
  node(58, 11520, 26880);
  node(582, 11808, 26936);
  node(59, 12096, 26992);
  node(583, 12384, 27048);
  node(584, 12672, 27104);
  node(585, 12624, 27256);
  node(586, 0, 27536);
  node(587, 288, 27592);
  node(588, 240, 27744);
  node(589, 576, 27648);
  node(590, 528, 27800);
  node(591, 864, 27704);
  node(592, 816, 27856);
  node(593, 1152, 27760);
  node(594, 1104, 27912);
  node(60, 1440, 27816);
  node(595, 1728, 27872);
  node(61, 2016, 27928);
  node(596, 2304, 27984);
  node(62, 2592, 28040);
  node(597, 2880, 28096);
  node(63, 3168, 28152);
  node(598, 3456, 28208);
  node(64, 3744, 28264);
  node(599, 4032, 28320);
  node(65, 4320, 28376);
  node(600, 4608, 28432);
  node(66, 4896, 28488);
  node(601, 5184, 28544);
  node(67, 5472, 28600);
  node(602, 5760, 28656);
  node(603, 6048, 28712);
  node(604, 6000, 28864);
  node(605, 6000, 28968);
  node(606, 6336, 28768);
  node(607, 6288, 28920);
  node(608, 6288, 29024);
  node(69, 6624, 28824);
  node(609, 6912, 28880);
  node(610, 7200, 28936);
  node(611, 7152, 29088);
  node(612, 7152, 29192);
  node(613, 7152, 29296);
  node(614, 7152, 29400);
  node(615, 7488, 28992);
  node(616, 7440, 29144);
  node(70, 7776, 29048);
  node(617, 8064, 29104);
  node(71, 8352, 29160);
  node(618, 8640, 29216);
  node(72, 8928, 29272);
  node(619, 9216, 29328);
  node(73, 9504, 29384);
  node(620, 9792, 29440);
  node(74, 10080, 29496);
  node(621, 10368, 29552);
  node(622, 10656, 29608);
  node(75, 10944, 29664);
  node(623, 11232, 29720);
  node(76, 11520, 29776);
  node(624, 11808, 29832);
  node(77, 12096, 29888);
  node(625, 12384, 29944);
  node(78, 12672, 30000);
  node(626, 12960, 30056);
  node(79, 13248, 30112);
  node(627, 13536, 30168);
  node(80, 13824, 30224);
  node(628, 14112, 30280);
  node(81, 14400, 30336);
  node(629, 14688, 30392);
  node(82, 14976, 30448);
  node(630, 15264, 30504);
  node(83, 15552, 30560);
  node(631, 15840, 30616);
  node(84, 16128, 30672);
  node(632, 16416, 30728);
  node(633, 16704, 30784);
  node(634, 16656, 30936);
  node(635, 0, 31216);
  node(636, 288, 31272);
  node(637, 576, 31328);
  node(638, 864, 31384);
  node(639, 816, 31536);
  node(640, 816, 31640);
  node(85, 1152, 31440);
  node(641, 1440, 31496);
  node(642, 1728, 31552);
  node(643, 1680, 31704);
  node(644, 1680, 31808);
  node(645, 1680, 31912);
  node(646, 1680, 32016);
  node(647, 1680, 32120);
  node(648, 1680, 32224);
  node(86, 2016, 31608);
  node(649, 2304, 31664);
  node(650, 2592, 31720);
  node(651, 2544, 31872);
  node(652, 2544, 31976);
  node(653, 2880, 31776);
  node(654, 2832, 31928);
  node(655, 0, 32504);
  node(656, 288, 32560);
  node(657, 240, 32712);
  node(658, 576, 32616);
  node(659, 864, 32672);
  node(660, 1152, 32728);
  node(661, 1104, 32880);
  node(662, 1440, 32784);
  node(663, 1392, 32936);
  node(664, 1392, 33040);
  node(87, 1728, 32840);
  node(665, 2016, 32896);
  node(666, 1968, 33048);
  node(88, 2304, 32952);
  node(667, 2592, 33008);
  node(89, 2880, 33064);
  node(668, 3168, 33120);
  node(90, 3456, 33176);
  node(669, 3744, 33232);
  node(91, 4032, 33288);
  node(670, 4320, 33344);
  node(92, 4608, 33400);
  node(671, 4896, 33456);
  node(672, 5184, 33512);
  node(673, 0, 33792);
  node(674, 288, 33848);
  node(675, 240, 34000);
  node(676, 576, 33904);
  node(677, 528, 34056);
  node(678, 528, 34160);
  node(679, 864, 33960);
  node(680, 1152, 34016);
  node(681, 1440, 34072);
  node(93, 1728, 34128);
  node(682, 2016, 34184);
  node(94, 2304, 34240);
  node(683, 2592, 34296);
  node(95, 2880, 34352);
  node(684, 3168, 34408);
  node(685, 3456, 34464);
  node(686, 3744, 34520);
  node(687, 3696, 34672);
  node(688, 4032, 34576);
  node(689, 3984, 34728);
  node(690, 3984, 34832);
  node(96, 4320, 34632);
  node(97, 4608, 34688);
  node(691, 4896, 34744);
  node(692, 4848, 34896);
  node(693, 4848, 35000);
  node(694, 4848, 35104);
  node(695, 4848, 35208);
  node(696, 5184, 34800);
  node(697, 5136, 34952);
  node(98, 5472, 34856);
  node(698, 5760, 34912);
  node(99, 6048, 34968);
  node(699, 6336, 35024);
  node(700, 6624, 35080);
  node(701, 6576, 35232);
  node(702, 6576, 35336);
  node(703, 6912, 35136);
  node(704, 7200, 35192);
  node(705, 7152, 35344);
  node(706, 7152, 35448);
  node(707, 7152, 35552);
  node(708, 7488, 35248);
  node(709, 7440, 35400);
  node(100, 7776, 35304);
  node(710, 8064, 35360);
  node(711, 8352, 35416);
  node(712, 8304, 35568);
  node(713, 8640, 35472);
  node(714, 8592, 35624);
  node(715, 8928, 35528);
  node(716, 8880, 35680);
  node(717, 8880, 35784);
  node(718, 9216, 35584);
  node(719, 0, 36064);
  node(101, 288, 36120);
  node(720, 576, 36176);
  node(721, 864, 36232);
  node(722, 816, 36384);
  node(723, 1152, 36288);
  node(724, 1104, 36440);
  node(725, 1104, 36544);
  node(726, 1440, 36344);
  node(727, 1392, 36496);
  node(728, 1392, 36600);
  node(729, 1728, 36400);
  node(730, 1680, 36552);
  node(731, 1680, 36656);
  node(732, 2016, 36456);
  node(733, 1968, 36608);
  node(734, 1968, 36712);
  node(102, 2304, 36512);
  node(735, 2592, 36568);
  node(736, 0, 36992);
  node(737, 288, 37048);
  node(738, 240, 37200);
  node(739, 240, 37304);
  node(740, 576, 37104);
  node(741, 528, 37256);
  node(742, 528, 37360);
  node(743, 0, 37640);
  node(744, 288, 37696);
  node(745, 240, 37848);
  node(746, 576, 37752);
  node(747, 528, 37904);
  node(748, 864, 37808);
  node(749, 816, 37960);
  node(750, 816, 38064);
  node(751, 1152, 37864);
  node(752, 1104, 38016);
  node(753, 1440, 37920);
  node(754, 1392, 38072);
  node(755, 1392, 38176);
  node(756, 1728, 37976);
  node(757, 1680, 38128);
  node(758, 2016, 38032);
  node(759, 2304, 38088);
  node(760, 2592, 38144);
  node(761, 2880, 38200);
  node(762, 2832, 38352);
  node(103, 3168, 38256);
  node(763, 3456, 38312);
  node(104, 3744, 38368);
  node(764, 4032, 38424);
  node(765, 3984, 38576);
  node(766, 4320, 38480);
  node(767, 4608, 38536);
  node(768, 4560, 38688);
  node(769, 4896, 38592);
  node(770, 0, 38968);
  node(771, 288, 39024);
  node(772, 576, 39080);
  node(773, 528, 39232);
  node(120, 864, 39136);
  node(774, 1152, 39192);
  node(775, 1104, 39344);
  node(776, 1440, 39248);
  node(777, 1728, 39304);
  node(778, 1680, 39456);
  node(779, 2016, 39360);
  node(780, 1968, 39512);
  node(781, 2304, 39416);
  node(782, 2256, 39568);
  node(783, 2592, 39472);
  node(784, 2544, 39624);
  node(785, 2880, 39528);
  node(786, 2832, 39680);
  node(787, 3168, 39584);
  node(788, 3120, 39736);
  node(789, 3120, 39840);
  node(790, 3456, 39640);
  node(791, 3408, 39792);
  node(792, 3408, 39896);
  node(793, 3744, 39696);
  node(794, 3696, 39848);
  node(795, 4032, 39752);
  node(796, 3984, 39904);
  node(797, 3984, 40008);
  node(798, 4320, 39808);
  node(799, 4272, 39960);
  node(800, 4608, 39864);
  node(801, 4896, 39920);
  node(802, 5184, 39976);
  node(803, 5472, 40032);
  node(804, 5760, 40088);
  node(121, 6048, 40144);
  node(805, 6336, 40200);
  node(806, 6624, 40256);
  node(807, 6576, 40408);
  node(808, 6912, 40312);
  node(809, 6864, 40464);
  node(122, 7200, 40368);
  node(810, 7488, 40424);
  node(123, 7776, 40480);
  node(811, 8064, 40536);
  node(812, 8352, 40592);
  node(813, 8304, 40744);
  node(124, 8640, 40648);
  node(814, 8928, 40704);
  node(815, 9216, 40760);
  node(816, 9504, 40816);
  node(817, 9792, 40872);
  node(818, 9744, 41024);
  node(125, 10080, 40928);
  node(819, 10368, 40984);
  node(126, 10656, 41040);
  node(820, 10944, 41096);
  node(127, 11232, 41152);
  node(821, 11520, 41208);
  node(822, 11472, 41360);
  node(823, 11808, 41264);
  node(824, 12096, 41320);
  node(825, 12048, 41472);
  node(826, 12048, 41576);
  node(827, 12048, 41680);
  node(828, 12048, 41784);
  node(829, 12048, 41888);
  node(830, 12048, 41992);
  node(831, 12048, 42096);
  node(832, 0, 42376);
  node(833, 288, 42432);
  node(834, 240, 42584);
  node(835, 576, 42488);
  node(836, 528, 42640);
  node(837, 864, 42544);
  node(838, 816, 42696);
  node(839, 816, 42800);
  node(840, 1152, 42600);
  node(841, 1104, 42752);
  node(842, 1440, 42656);
  node(843, 1392, 42808);
  node(844, 1392, 42912);
  node(845, 1728, 42712);
  node(846, 1680, 42864);
  node(847, 2016, 42768);
  node(848, 2304, 42824);
  node(849, 2592, 42880);
  node(850, 2880, 42936);
  node(851, 2832, 43088);
  node(105, 3168, 42992);
  node(852, 3456, 43048);
  node(106, 3744, 43104);
  node(853, 4032, 43160);
  node(854, 3984, 43312);
  node(855, 4320, 43216);
  node(856, 4608, 43272);
  node(857, 4560, 43424);
  node(858, 4560, 43528);
  node(859, 4560, 43632);
  node(860, 4896, 43328);
  node(861, 4848, 43480);
  node(862, 5184, 43384);
  node(863, 5136, 43536);
  node(107, 5472, 43440);
  node(864, 5760, 43496);
  node(865, 6048, 43552);
  node(866, 6000, 43704);
  node(867, 6336, 43608);
  node(868, 6288, 43760);
  node(869, 6624, 43664);
  node(870, 6576, 43816);
  node(108, 6912, 43720);
  node(871, 7200, 43776);
  node(872, 7488, 43832);
  node(109, 7776, 43888);
  node(873, 8064, 43944);
  node(874, 8016, 44096);
  node(875, 8016, 44200);
  node(876, 8016, 44304);
  node(877, 8352, 44000);
  node(878, 8640, 44056);
  node(110, 8928, 44112);
  node(879, 9216, 44168);
  node(880, 9504, 44224);
  node(881, 9456, 44376);
  node(882, 9792, 44280);
  node(883, 9744, 44432);
  node(111, 10080, 44336);
  node(884, 10368, 44392);
  node(885, 10656, 44448);
  node(886, 10608, 44600);
  node(887, 10944, 44504);
  node(888, 11232, 44560);
  node(889, 11520, 44616);
  node(890, 11808, 44672);
  node(891, 11760, 44824);
  node(112, 12096, 44728);
  node(892, 12384, 44784);
  node(113, 12672, 44840);
  node(893, 12960, 44896);
  node(894, 12912, 45048);
  node(895, 13248, 44952);
  node(896, 13536, 45008);
  node(897, 13488, 45160);
  node(898, 13488, 45264);
  node(899, 13488, 45368);
  node(900, 13824, 45064);
  node(901, 13776, 45216);
  node(902, 14112, 45120);
  node(903, 14064, 45272);
  node(114, 14400, 45176);
  node(904, 14688, 45232);
  node(905, 14976, 45288);
  node(906, 14928, 45440);
  node(907, 15264, 45344);
  node(908, 15552, 45400);
  node(909, 15840, 45456);
  node(910, 16128, 45512);
  node(115, 16416, 45568);
  node(911, 16704, 45624);
  node(912, 16992, 45680);
  node(913, 16944, 45832);
  node(914, 17280, 45736);
  node(915, 17232, 45888);
  node(116, 17568, 45792);
  node(916, 17856, 45848);
  node(917, 18144, 45904);
  node(918, 18096, 46056);
  node(117, 18432, 45960);
  node(919, 18720, 46016);
  node(920, 19008, 46072);
  node(921, 18960, 46224);
  node(118, 19296, 46128);
  node(922, 19584, 46184);
  node(923, 19872, 46240);
  node(924, 19824, 46392);
  node(119, 20160, 46296);
  node(925, 20448, 46352);
  node(926, 20400, 46504);
  node(927, 20736, 46408);
  node(928, 20688, 46560);
  node(929, 20688, 46664);
  node(128, 21024, 46464);
  node(930, 21312, 46520);
  node(931, 0, 46944);
  node(932, 288, 47000);
  node(131, 576, 47056);
  node(933, 528, 47208);
  node(934, 528, 47312);
  node(935, 864, 47112);
  node(936, 816, 47264);
  node(937, 816, 47368);
  node(938, 1152, 47168);
  node(939, 1104, 47320);
  node(940, 1440, 47224);
  node(941, 1392, 47376);
  node(942, 1392, 47480);
  node(943, 1728, 47280);
  node(944, 1680, 47432);
  node(945, 1680, 47536);
  node(946, 2016, 47336);
  node(947, 1968, 47488);
  node(948, 1968, 47592);
  node(949, 2304, 47392);
  node(950, 2256, 47544);
  node(951, 2256, 47648);
  node(952, 2592, 47448);
  node(953, 2544, 47600);
  node(954, 2544, 47704);
  node(955, 2880, 47504);
  node(956, 2832, 47656);
  node(957, 2832, 47760);
  node(958, 3168, 47560);
  node(959, 3120, 47712);
  node(960, 3120, 47816);
  node(961, 3456, 47616);
  node(962, 3408, 47768);
  node(963, 3408, 47872);
  node(964, 3744, 47672);
  node(965, 3696, 47824);
  node(966, 3696, 47928);
  node(967, 4032, 47728);
  node(968, 3984, 47880);
  node(969, 3984, 47984);
  node(970, 4320, 47784);
  node(971, 4272, 47936);
  node(972, 4272, 48040);
  node(973, 4608, 47840);
  node(974, 4560, 47992);
  node(975, 4560, 48096);
  node(976, 4896, 47896);
  node(977, 4848, 48048);
  node(978, 4848, 48152);
  node(979, 5184, 47952);
  node(980, 5136, 48104);
  node(981, 5136, 48208);
  node(982, 5472, 48008);
  node(983, 5424, 48160);
  node(984, 5424, 48264);
  node(985, 5424, 48368);
  node(986, 5424, 48472);
  node(987, 5760, 48064);
  node(988, 6048, 48120);
  node(989, 6000, 48272);
  node(990, 6000, 48376);
  node(991, 6336, 48176);
  node(992, 6288, 48328);
  node(993, 6288, 48432);
  node(994, 6288, 48536);
  node(995, 6624, 48232);
  node(996, 6576, 48384);
  node(997, 6576, 48488);
  node(998, 6576, 48592);
  node(999, 6576, 48696);
  node(1000, 6912, 48288);
  node(1001, 6864, 48440);
  next_id(1002);
}


}  // namespace kern_blueprint_6cb2cf0da8e6380e

// ---------------------------------------------------------------------------
// The Blueprint of actor "qr_code_cpp"
namespace kern_blueprint_fe59a0c6af5496ca {

using qr_code = ::kern_blueprint_f1248d4ad2e4cc95::qr_code;
using shapes = ::kern_blueprint_6025b4a819f918d1::shapes;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct qr_code_cpp : virtual kern::Library, virtual qr_code, virtual shapes {
  // ---- types ----
  KERN_STRUCT(qr_Grid) {
    Int size = 0;
    List<Int> m = List<Int>{};
    List<Int> fn = List<Int>{};
  };

  // ---- variables ----
  /// Level L per version (ISO/IEC 18004 table 9): EC codewords per block, number of blocks.
  KERN_CONST Fixed<Int, 41> kEccPerBlock = Fixed<Int, 41>{-1, 7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
  KERN_CONST Fixed<Int, 41> kBlocks = Fixed<Int, 41>{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8, 9, 9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25};
  /// format bits of level L
  KERN_CONST Int kLevelL = 1;

  // ---- functions ----
  struct raw_modulesResult { Int Result = 0; };
  KERN_PURE raw_modulesResult raw_modules(Int v = 0) {
    Int r = 0;
    Int n = 0;
    r = kern::to<Int>(kern::formula([](Float v) -> Float { return (16.0 * v + 128.0) * v + 64.0; }, kern::to<Float>(v)));
    if (math_compare<">=">(kern::to<Float>(v), 2.0)) {
      n = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::trunc(kern::math::div(v, 7.0)) + 2.0; }, kern::to<Float>(v)));
      r = kern::to<Int>(kern::formula([](Float r, Float n) -> Float { return r - ((25.0 * n - 10.0) * n - 55.0); }, kern::to<Float>(r), kern::to<Float>(n)));
      if (math_compare<">=">(kern::to<Float>(v), 7.0)) {
        r = kern::to<Int>(kern::formula([](Float r) -> Float { return r - 36.0; }, kern::to<Float>(r)));
      }
    }
    return {r};
  }

  struct data_codewordsResult { Int Result = 0; };
  KERN_PURE data_codewordsResult data_codewords(Int v = 0) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::trunc(kern::math::div(a, 8.0)) - a_2 * a_3; }, kern::to<Float>(raw_modules(v).Result), kern::to<Float>(list_get<Int>(kEccPerBlock, v)), kern::to<Float>(list_get<Int>(kBlocks, v))))};
  }

  struct gf_mulResult { Int Result = 0; };
  KERN_PURE gf_mulResult gf_mul(Int x = 0, Int y = 0) {
    Int z = 0;
    Int i = 0;
    z = 0;
    i = 7;
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      z = kern::to<Int>(kern::formula([](Float z) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(z, 1.0)), kern::math::mul32(kern::math::shr(z, 7.0), 285.0)); }, kern::to<Float>(z)));
      z = kern::to<Int>(kern::formula([](Float z, Float y, Float i, Float x) -> Float { return kern::math::bxor(z, kern::math::mul32(kern::math::band(kern::math::wrap32(kern::math::shr(y, i)), 1.0), x)); }, kern::to<Float>(z), kern::to<Float>(y), kern::to<Float>(i), kern::to<Float>(x)));
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
    }
    return {kern::to<Int>(kern::formula([](Float z) -> Float { return kern::math::band(z, 255.0); }, kern::to<Float>(z)))};
  }

  struct rs_divisorResult { List<Int> Result = List<Int>{}; };
  KERN_PURE rs_divisorResult rs_divisor(Int degree = 0) {
    List<Int> r = List<Int>{};
    List<Int> made = List<Int>{};
    Int root = 0;
    Int j = 0;
    made = List<Int>{};
    list_resize(made, degree, 0);
    r = std::move(made);
    r[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(r))))] = 1;
    root = 1;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(degree)))); it10.next();) {
      j = 0;
      while (loop(math_compare<">">(kern::formula([](Float j, Float a) -> Float { return (j < 0.0) != (a < 0.0) ? a < 0.0 : j < a; }, kern::to<Float>(j), kern::to<Float>(list_length<Int>(r))), 0.5))) {
        r[j] = gf_mul(r[j], root).Result;
        if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float j) -> Float { return j + 1.0; }, kern::to<Float>(j))))), kern::to<Float>(list_length<Int>(r))), 0.5)) {
          r[j] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::bxor(a, a_2); }, kern::to<Float>(r[j]), kern::to<Float>(r[kern::to<Int>(kern::formula([](Float j) -> Float { return j + 1.0; }, kern::to<Float>(j)))])));
        }
        j += 1;
      }
      root = gf_mul(root, 2).Result;
    }
    return {std::move(r)};
  }

  struct rs_remainderResult { List<Int> Result = List<Int>{}; };
  KERN_PURE rs_remainderResult rs_remainder(const List<Int>& data, const List<Int>& the_div) {
    List<Int> r = List<Int>{};
    List<Int> made = List<Int>{};
    Int b = 0;
    Int f = 0;
    Int i = 0;
    made = List<Int>{};
    list_resize(made, list_length<Int>(the_div), 0);
    r = std::move(made);
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(data))))); it11.next();) {
      b = data[n11.index];
      f = kern::to<Int>(kern::formula([](Float b, Float a) -> Float { return kern::math::band(kern::math::bxor(b, a), 255.0); }, kern::to<Float>(b), kern::to<Float>(r[0])));
      list_remove_at(r, 0);
      list_add(r, 0);
      i = 0;
      while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (i < 0.0) != (a < 0.0) ? a < 0.0 : i < a; }, kern::to<Float>(i), kern::to<Float>(list_length<Int>(r))), 0.5))) {
        r[i] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::bxor(a, a_2); }, kern::to<Float>(r[i]), kern::to<Float>(gf_mul(the_div[i], f).Result)));
        i += 1;
      }
    }
    return {std::move(r)};
  }

  struct alignment_positionsResult { List<Int> Result = List<Int>{}; };
  KERN_PURE alignment_positionsResult alignment_positions(Int v = 0) {
    Int n = 0;
    Int size = 0;
    Int step = 0;
    List<Int> r = List<Int>{};
    List<Int> made = List<Int>{};
    Int i = 0;
    Int pos = 0;
    if (math_compare<"==">(kern::to<Float>(v), 1.0)) {
      return {List<Int>{}};
    }
    n = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::trunc(kern::math::div(v, 7.0)) + 2.0; }, kern::to<Float>(v)));
    size = kern::to<Int>(kern::formula([](Float v) -> Float { return v * 4.0 + 17.0; }, kern::to<Float>(v)));
    step = kern::to<Int>(kern::formula([](Float v, Float n) -> Float { return v == 32.0 ? 26.0 : kern::math::trunc(kern::math::div(v * 4.0 + n * 2.0 + 1.0, n * 2.0 - 2.0)) * 2.0; }, kern::to<Float>(v), kern::to<Float>(n)));
    made = List<Int>{};
    list_resize(made, n, 0);
    r = std::move(made);
    r[0] = 6;
    i = kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)));
    pos = kern::to<Int>(kern::formula([](Float size) -> Float { return size - 7.0; }, kern::to<Float>(size)));
    while (loop(math_compare<">=">(kern::to<Float>(i), 1.0))) {
      r[i] = pos;
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
      pos = kern::to<Int>(kern::formula([](Float pos, Float step) -> Float { return pos - step; }, kern::to<Float>(pos), kern::to<Float>(step)));
    }
    return {std::move(r)};
  }

  void Grid_set(qr_Grid& self, Int x = 0, Int y = 0, Bool dark = false) {
    self.m[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return y * size + x; }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))] = kern::to<Int>(dark);
    self.fn[kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return y * size + x; }, kern::to<Float>(y), kern::to<Float>(self.size), kern::to<Float>(x)))] = 1;
  }

  void finder(qr_Grid& g, Int cx = 0, Int cy = 0) {
    Bool skip = false;
    Int x = 0;
    Int y = 0;
    Int d = 0;
    for (auto it12 = flow_for(n12, -4, 4); it12.next();) {
      for (auto it13 = flow_for(n13, -4, 4); it13.next();) {
        skip = false;
        x = kern::to<Int>(kern::formula([](Float cx, Float index) -> Float { return cx + index; }, kern::to<Float>(cx), kern::to<Float>(n13.index)));
        y = kern::to<Int>(kern::formula([](Float cy, Float index) -> Float { return cy + index; }, kern::to<Float>(cy), kern::to<Float>(n12.index)));
        if (logic_or(logic_or(logic_or(math_compare<"<">(kern::to<Float>(x), 0.0), math_compare<"<">(kern::to<Float>(y), 0.0)), math_compare<">=">(kern::to<Float>(x), kern::to<Float>(g.size))), math_compare<">=">(kern::to<Float>(y), kern::to<Float>(g.size)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          d = kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return kern::math::max(kern::math::abs(index), kern::math::abs(index_2)); }, kern::to<Float>(n13.index), kern::to<Float>(n12.index)));
          Grid_set(g, x, y, logic_and(math_compare<"!=">(kern::to<Float>(d), 2.0), math_compare<"!=">(kern::to<Float>(d), 4.0)));
        }
      }
    }
  }

  struct format_bits_bitResult { Bool Result = false; };
  KERN_PURE format_bits_bitResult format_bits_bit(Int bits = 0, Int i = 0) {
    return {math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float bits, Float i) -> Float { return kern::math::band(kern::math::shr(bits, i), 1.0); }, kern::to<Float>(bits), kern::to<Float>(i))))), 0.0)};
  }

  void format_bits(qr_Grid& g, Int mask = 0) {
    Int data = 0;
    Int rem = 0;
    Int bits = 0;
    data = kern::to<Int>(kern::formula([](Float kLevelL, Float mask) -> Float { return kern::math::bor(kern::math::wrap32(kern::math::shl(kLevelL, 3.0)), mask); }, kern::to<Float>(kLevelL), kern::to<Float>(mask)));
    rem = data;
    for (auto it14 = flow_for(n14, 0, 9); it14.next();) {
      rem = kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(rem, 1.0)), kern::math::mul32(kern::math::shr(rem, 9.0), 1335.0)); }, kern::to<Float>(rem)));
    }
    bits = kern::to<Int>(kern::formula([](Float data, Float rem) -> Float { return kern::math::bxor(kern::math::bor(kern::math::wrap32(kern::math::shl(data, 10.0)), rem), 21522.0); }, kern::to<Float>(data), kern::to<Float>(rem)));
    for (auto it15 = flow_for(n15, 0, 5); it15.next();) {
      Grid_set(g, 8, n15.index, format_bits_bit(bits, n15.index).Result);
    }
    Grid_set(g, 8, 7, format_bits_bit(bits, 6).Result);
    Grid_set(g, 8, 8, format_bits_bit(bits, 7).Result);
    Grid_set(g, 7, 8, format_bits_bit(bits, 8).Result);
    for (auto it16 = flow_for(n16, 9, 14); it16.next();) {
      Grid_set(g, kern::to<Int>(kern::formula([](Float index) -> Float { return 14.0 - index; }, kern::to<Float>(n16.index))), 8, format_bits_bit(bits, n16.index).Result);
    }
    for (auto it17 = flow_for(n17, 0, 7); it17.next();) {
      Grid_set(g, kern::to<Int>(kern::formula([](Float size, Float index) -> Float { return size - 1.0 - index; }, kern::to<Float>(g.size), kern::to<Float>(n17.index))), 8, format_bits_bit(bits, n17.index).Result);
    }
    for (auto it18 = flow_for(n18, 8, 14); it18.next();) {
      Grid_set(g, 8, kern::to<Int>(kern::formula([](Float size, Float index) -> Float { return size - 15.0 + index; }, kern::to<Float>(g.size), kern::to<Float>(n18.index))), format_bits_bit(bits, n18.index).Result);
    }
    Grid_set(g, 8, kern::to<Int>(kern::formula([](Float size) -> Float { return size - 8.0; }, kern::to<Float>(g.size))), true);
  }

  void encode_put(List<Bool>& bits, Int val = 0, Int len = 0) {
    Int i = 0;
    i = kern::to<Int>(kern::formula([](Float len) -> Float { return len - 1.0; }, kern::to<Float>(len)));
    while (loop(math_compare<">=">(kern::to<Float>(i), 0.0))) {
      list_add(bits, math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float val, Float i) -> Float { return kern::math::band(kern::math::shr(val, i), 1.0); }, kern::to<Float>(val), kern::to<Float>(i))))), 0.0));
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return i - 1.0; }, kern::to<Float>(i)));
    }
  }

  struct encodeResult { Bool Result = false; };
  encodeResult encode(const Text& text, qr_Code& out) {
    Int n = 0;
    Int v = 0;
    Bool stop = false;
    Int count_bits = 0;
    List<Bool> bits = List<Bool>{};
    Int c = 0;
    Int cap = 0;
    Int pad = 0;
    List<Int> data = List<Int>{};
    List<Int> made = List<Int>{};
    Int blocks = 0;
    Int ecc = 0;
    Int raw = 0;
    Int short_blocks = 0;
    Int short_len = 0;
    List<Int> the_div = List<Int>{};
    List<Int_row> bl = List<Int_row>{};
    Int i = 0;
    Int k = 0;
    Int len = 0;
    List<Int> d = List<Int>{};
    List<Int> made_2 = List<Int>{};
    Int first = 0;
    Int last = 0;
    List<Int> e = List<Int>{};
    Int place = 0;
    List<Int> words = List<Int>{};
    qr_Grid g = qr_Grid{0, List<Int>{}, List<Int>{}};
    Int count = 0;
    Int fill = 0;
    Int count_2 = 0;
    Int fill_2 = 0;
    Int i_2 = 0;
    List<Int> al = List<Int>{};
    Bool skip = false;
    Int rem = 0;
    Int vb = 0;
    Bool b = false;
    Int a = 0;
    Int c_2 = 0;
    Int i_3 = 0;
    Int right = 0;
    Int vert = 0;
    Bool skip_2 = false;
    Int x = 0;
    Bool up = false;
    Int y = 0;
    Int at = 0;
    n = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(text_length(text))));
    v = 1;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<=">(kern::to<Float>(v), 40.0)))) {
      count_bits = kern::to<Int>(kern::formula([](Float v) -> Float { return v <= 9.0 ? 8.0 : 16.0; }, kern::to<Float>(v)));
      if (logic_and(math_compare<"<=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float count_bits, Float n) -> Float { return 4.0 + count_bits + 8.0 * n; }, kern::to<Float>(count_bits), kern::to<Float>(n))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a * 8.0; }, kern::to<Float>(data_codewords(v).Result)))))), logic_or(math_compare<"==">(kern::to<Float>(count_bits), 16.0), math_compare<"<">(kern::to<Float>(n), 256.0)))) {
        stop = true;
      }
      if (logic_not(stop)) {
        v += 1;
      }
    }
    if (math_compare<">">(kern::to<Float>(v), 40.0)) {
      return {false};
    }
    bits = List<Bool>{};
    encode_put(bits, 4, 4);
    encode_put(bits, kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::wrap32(n); }, kern::to<Float>(n))), kern::to<Int>(kern::formula([](Float v) -> Float { return v <= 9.0 ? 8.0 : 16.0; }, kern::to<Float>(v))));
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(text))))); it19.next();) {
      c = text_code_at(text, n19.index);
      encode_put(bits, c, 8);
    }
    cap = kern::to<Int>(kern::formula([](Float a) -> Float { return a * 8.0; }, kern::to<Float>(data_codewords(v).Result)));
    encode_put(bits, 0, kern::to<Int>(kern::formula([](Float cap, Float a) -> Float { return kern::math::wrap32s(kern::math::min(4.0, cap - a)); }, kern::to<Float>(cap), kern::to<Float>(list_length<Bool>(bits)))));
    while (loop(math_compare<">">(kern::formula([](Float a) -> Float { return kern::math::mod(a, 8.0) != 0.0; }, kern::to<Float>(list_length<Bool>(bits))), 0.5))) {
      list_add(bits, false);
    }
    pad = 236;
    while (loop(math_compare<">">(kern::formula([](Float a, Float cap) -> Float { return (a < 0.0) != (cap < 0.0) ? cap < 0.0 : a < cap; }, kern::to<Float>(list_length<Bool>(bits)), kern::to<Float>(cap)), 0.5))) {
      encode_put(bits, pad, 8);
      pad = kern::to<Int>(kern::formula([](Float pad) -> Float { return kern::math::bxor(pad, kern::math::bxor(236.0, 17.0)); }, kern::to<Float>(pad)));
    }
    made = List<Int>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div(a, 8.0)); }, kern::to<Float>(list_length<Bool>(bits)))), 0);
    data = std::move(made);
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Bool>(bits))))); it20.next();) {
      if (bits[n20.index]) {
        data[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 3.0); }, kern::to<Float>(n20.index)))] = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return kern::math::bor(a, kern::math::band(kern::math::wrap32s(kern::math::shl(1.0, 7.0 - kern::math::band(index, 7.0))), 255.0)); }, kern::to<Float>(data[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 3.0); }, kern::to<Float>(n20.index)))]), kern::to<Float>(n20.index)));
      }
    }
    blocks = list_get<Int>(kBlocks, v);
    ecc = list_get<Int>(kEccPerBlock, v);
    raw = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div(a, 8.0)); }, kern::to<Float>(raw_modules(v).Result)));
    short_blocks = kern::to<Int>(kern::formula([](Float blocks, Float raw) -> Float { return blocks - kern::math::mod(raw, blocks); }, kern::to<Float>(blocks), kern::to<Float>(raw)));
    short_len = kern::to<Int>(kern::formula([](Float raw, Float blocks) -> Float { return kern::math::trunc(kern::math::div(raw, blocks)); }, kern::to<Float>(raw), kern::to<Float>(blocks)));
    the_div = rs_divisor(ecc).Result;
    bl = List<Int_row>{};
    i = 0;
    k = 0;
    while (loop(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(blocks)))) {
      len = kern::to<Int>(kern::formula([](Float short_len, Float ecc, Float i, Float short_blocks) -> Float { return short_len - ecc + (i < short_blocks ? 0.0 : 1.0); }, kern::to<Float>(short_len), kern::to<Float>(ecc), kern::to<Float>(i), kern::to<Float>(short_blocks)));
      made_2 = List<Int>{};
      first = k;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(k), kern::to<Float>(len)))))));
      list_reserve(made_2, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(last), kern::to<Float>(first))))))));
      for (auto it21 = flow_for(n21, first, last); it21.next();) {
        list_add(made_2, list_get<Int>(data, n21.index));
      }
      d = std::move(made_2);
      k += len;
      e = rs_remainder(d, the_div).Result;
      if (math_compare<"<">(kern::to<Float>(i), kern::to<Float>(short_blocks))) {
        list_add(d, 0);
      }
      place = list_length<Int>(d);
      list_insert_list(d, place, std::move(e));
      list_add(bl, struct_make<Int_row>(d));
      i += 1;
    }
    words = List<Int>{};
    for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int>(bl[0].items))))); it22.next();) {
      for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int_row>(bl))))); it23.next();) {
        if (logic_or(math_compare<"!=">(kern::to<Float>(n22.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float short_len, Float ecc) -> Float { return short_len - ecc; }, kern::to<Float>(short_len), kern::to<Float>(ecc)))))), math_compare<">">(kern::formula([](Float index, Float short_blocks) -> Float { return !((index < 0.0) != (short_blocks < 0.0) ? short_blocks < 0.0 : index < short_blocks); }, kern::to<Float>(n23.index), kern::to<Float>(short_blocks)), 0.5))) {
          list_add(words, bl[n23.index].items[n22.index]);
        }
      }
    }
    g = struct_make<qr_Grid>(kern::to<Int>(kern::formula([](Float v) -> Float { return v * 4.0 + 17.0; }, kern::to<Float>(v))), List<Int>{}, List<Int>{});
    count = kern::to<Int>(kern::formula([](Float size) -> Float { return size * size; }, kern::to<Float>(g.size)));
    fill = 0;
    list_clear(g.m);
    list_resize(g.m, count, fill);
    count_2 = list_length<Int>(g.m);
    fill_2 = 0;
    list_clear(g.fn);
    list_resize(g.fn, count_2, fill_2);
    i_2 = 0;
    while (loop(math_compare<"<">(kern::to<Float>(i_2), kern::to<Float>(g.size)))) {
      Grid_set(g, 6, i_2, math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::mod(i_2, 2.0); }, kern::to<Float>(i_2))))), 0.0));
      Grid_set(g, i_2, 6, math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::mod(i_2, 2.0); }, kern::to<Float>(i_2))))), 0.0));
      i_2 += 1;
    }
    finder(g, 3, 3);
    finder(g, kern::to<Int>(kern::formula([](Float size) -> Float { return size - 4.0; }, kern::to<Float>(g.size))), 3);
    finder(g, 3, kern::to<Int>(kern::formula([](Float size) -> Float { return size - 4.0; }, kern::to<Float>(g.size))));
    al = alignment_positions(v).Result;
    for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int>(al))))); it24.next();) {
      for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int>(al))))); it25.next();) {
        skip = false;
        if (logic_or(logic_or(logic_and(math_compare<"==">(kern::to<Float>(n24.index), 0.0), math_compare<"==">(kern::to<Float>(n25.index), 0.0)), logic_and(math_compare<"==">(kern::to<Float>(n24.index), 0.0), math_compare<"==">(kern::to<Float>(n25.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(al))))))))), logic_and(math_compare<"==">(kern::to<Float>(n24.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(al))))))), math_compare<"==">(kern::to<Float>(n25.index), 0.0)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it26 = flow_for(n26, -2, 2); it26.next();) {
            for (auto it27 = flow_for(n27, -2, 2); it27.next();) {
              Grid_set(g, kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return a + index; }, kern::to<Float>(al[n24.index]), kern::to<Float>(n27.index))), kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return a + index; }, kern::to<Float>(al[n25.index]), kern::to<Float>(n26.index))), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return kern::math::max(kern::math::abs(index), kern::math::abs(index_2)); }, kern::to<Float>(n27.index), kern::to<Float>(n26.index))))), 1.0));
            }
          }
        }
      }
    }
    format_bits(g, 0);
    if (math_compare<">=">(kern::to<Float>(v), 7.0)) {
      rem = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(v); }, kern::to<Float>(v)));
      for (auto it28 = flow_for(n28, 0, 11); it28.next();) {
        rem = kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(rem, 1.0)), kern::math::mul32(kern::math::shr(rem, 11.0), 7973.0)); }, kern::to<Float>(rem)));
      }
      vb = kern::to<Int>(kern::formula([](Float v, Float rem) -> Float { return kern::math::bor(kern::math::wrap32(kern::math::shl(kern::math::wrap32(v), 12.0)), rem); }, kern::to<Float>(v), kern::to<Float>(rem)));
      for (auto it29 = flow_for(n29, 0, 17); it29.next();) {
        b = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float vb, Float index) -> Float { return kern::math::band(kern::math::shr(vb, index), 1.0); }, kern::to<Float>(vb), kern::to<Float>(n29.index))))), 0.0);
        a = kern::to<Int>(kern::formula([](Float size, Float index) -> Float { return size - 11.0 + kern::math::mod(index, 3.0); }, kern::to<Float>(g.size), kern::to<Float>(n29.index)));
        c_2 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::trunc(kern::math::div(index, 3.0)); }, kern::to<Float>(n29.index)));
        Grid_set(g, a, c_2, b);
        Grid_set(g, c_2, a, b);
      }
    }
    i_3 = 0;
    right = kern::to<Int>(kern::formula([](Float size) -> Float { return size - 1.0; }, kern::to<Float>(g.size)));
    while (loop(math_compare<">=">(kern::to<Float>(right), 1.0))) {
      if (math_compare<"==">(kern::to<Float>(right), 6.0)) {
        right = 5;
      }
      vert = 0;
      while (loop(math_compare<"<">(kern::to<Float>(vert), kern::to<Float>(g.size)))) {
        for (auto it30 = flow_for(n30, 0, 1); it30.next();) {
          skip_2 = false;
          x = kern::to<Int>(kern::formula([](Float right, Float index) -> Float { return right - index; }, kern::to<Float>(right), kern::to<Float>(n30.index)));
          up = math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float right) -> Float { return kern::math::band(right + 1.0, 2.0); }, kern::to<Float>(right))))), 0.0);
          y = kern::to<Int>(kern::formula([](Float up, Float size, Float vert) -> Float { return up != 0.0 ? size - 1.0 - vert : vert; }, kern::to<Float>(up), kern::to<Float>(g.size), kern::to<Float>(vert)));
          at = kern::to<Int>(kern::formula([](Float y, Float size, Float x) -> Float { return y * size + x; }, kern::to<Float>(y), kern::to<Float>(g.size), kern::to<Float>(x)));
          if (math_compare<"!=">(kern::to<Float>(g.fn[at]), 0.0)) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            if (math_compare<">">(kern::formula([](Float i_3, Float a) -> Float { return (i_3 < 0.0) != (a < 0.0) ? a < 0.0 : i_3 < a; }, kern::to<Float>(i_3), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a * 8.0; }, kern::to<Float>(list_length<Int>(words))))))), 0.5)) {
              g.m[at] = kern::to<Int>(kern::formula([](Float a, Float i_3) -> Float { return kern::math::band(kern::math::band(kern::math::shr(a, 7.0 - kern::math::band(i_3, 7.0)), 1.0), 255.0); }, kern::to<Float>(words[kern::to<Int>(kern::formula([](Float i_3) -> Float { return kern::math::shr(i_3, 3.0); }, kern::to<Float>(i_3)))]), kern::to<Float>(i_3)));
              i_3 += 1;
            }
            if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x, Float y) -> Float { return kern::math::mod(x + y, 2.0); }, kern::to<Float>(x), kern::to<Float>(y))))), 0.0)) {
              g.m[at] = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::bxor(a, 1.0); }, kern::to<Float>(g.m[at])));
            }
          }
        }
        vert += 1;
      }
      right = kern::to<Int>(kern::formula([](Float right) -> Float { return right - 2.0; }, kern::to<Float>(right)));
    }
    format_bits(g, 0);
    out.version = v;
    out.size = g.size;
    var_take(n31, g.m);
    out.modules = std::move(n31.value);
    return {true};
  }

  struct read_formatResult { Bool Result = false; };
  read_formatResult read_format(const qr_Code& c, Int& level_bits, Int& mask) {
    Int bits = 0;
    Int rem = 0;
    bits = 0;
    for (auto it32 = flow_for(n32, 0, 5); it32.next();) {
      bits = kern::to<Int>(kern::formula([](Float bits, Float a, Float index) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, index))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 8, n32.index).Result), kern::to<Float>(n32.index)));
    }
    bits = kern::to<Int>(kern::formula([](Float bits, Float a) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, 6.0))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 8, 7).Result)));
    bits = kern::to<Int>(kern::formula([](Float bits, Float a) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, 7.0))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 8, 8).Result)));
    bits = kern::to<Int>(kern::formula([](Float bits, Float a) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, 8.0))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, 7, 8).Result)));
    for (auto it33 = flow_for(n33, 9, 14); it33.next();) {
      bits = kern::to<Int>(kern::formula([](Float bits, Float a, Float index) -> Float { return kern::math::bor(bits, kern::math::wrap32(kern::math::shl(a, index))); }, kern::to<Float>(bits), kern::to<Float>(qr_code::Code_dark(c, kern::to<Int>(kern::formula([](Float index) -> Float { return 14.0 - index; }, kern::to<Float>(n33.index))), 8).Result), kern::to<Float>(n33.index)));
    }
    bits = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::bxor(bits, 21522.0); }, kern::to<Float>(bits)));
    level_bits = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::shr(bits, 13.0); }, kern::to<Float>(bits)));
    mask = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::band(kern::math::shr(bits, 10.0), 7.0); }, kern::to<Float>(bits)));
    rem = kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::shr(bits, 10.0); }, kern::to<Float>(bits)));
    for (auto it34 = flow_for(n34, 0, 9); it34.next();) {
      rem = kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::bxor(kern::math::wrap32(kern::math::shl(rem, 1.0)), kern::math::mul32(kern::math::shr(rem, 9.0), 1335.0)); }, kern::to<Float>(rem)));
    }
    return {math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float rem) -> Float { return kern::math::band(rem, 1023.0); }, kern::to<Float>(rem))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float bits) -> Float { return kern::math::band(bits, 1023.0); }, kern::to<Float>(bits))))))};
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
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct { List<Int> value = List<Int>{}; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct : kern::Node<34> { Int index = 0; } n34;
};

KERN_LAYOUT(qr_code_cpp) {
  node(361, 0, 8304);
  node(362, 288, 8360);
  node(363, 240, 8512);
  node(364, 576, 8416);
  node(365, 528, 8568);
  node(366, 864, 8472);
  node(367, 816, 8624);
  node(368, 1152, 8528);
  node(369, 1104, 8680);
  node(370, 1104, 8784);
  node(371, 1104, 8888);
  node(372, 1440, 8584);
  node(373, 1392, 8736);
  node(35, 1680, 0);
  node(36, 1560, 150);
  node(37, 1560, 240);
  node(38, 1960, 0);
  node(39, 1840, 150);
  node(40, 0, 470);
  node(41, 280, 470);
  node(42, 160, 620);
  node(43, 160, 710);
  node(44, 160, 800);
  node(45, 160, 890);
  node(46, 160, 980);
  node(47, 160, 1070);
  node(48, 0, 1210);
  node(49, 280, 1210);
  node(50, 560, 1210);
  node(51, 840, 1210);
  node(52, 720, 1360);
  node(53, 720, 1450);
  node(54, 1120, 1210);
  node(55, 1000, 1360);
  node(56, 1000, 1450);
  node(57, 1400, 1210);
  node(58, 1280, 1360);
  node(59, 1280, 1450);
  node(60, 1280, 1540);
  node(61, 1680, 1210);
  node(62, 1560, 1360);
  node(63, 1560, 1450);
  node(64, 1960, 1210);
  node(65, 1840, 1360);
  node(66, 1840, 1450);
  node(67, 0, 1680);
  node(70, 840, 1680);
  node(358, 1128, 1736);
  node(78, 1680, 1680);
  node(79, 1560, 1830);
  node(80, 1960, 1680);
  node(81, 1840, 1830);
  node(82, 1840, 1920);
  node(83, 1840, 2010);
  node(84, 2240, 1680);
  node(10, 2520, 1680);
  node(85, 2400, 1830);
  node(86, 2800, 1680);
  node(87, 3080, 1680);
  node(88, 2960, 1830);
  node(89, 2960, 1920);
  node(90, 2960, 2010);
  node(91, 2960, 2100);
  node(92, 2960, 2190);
  node(93, 3360, 1680);
  node(94, 3240, 1830);
  node(95, 3240, 1920);
  node(96, 3240, 2010);
  node(97, 3240, 2100);
  node(98, 3240, 2190);
  node(99, 3640, 1680);
  node(100, 3520, 1830);
  node(101, 3520, 1920);
  node(102, 3520, 2010);
  node(103, 3520, 2100);
  node(104, 3520, 2190);
  node(105, 3520, 2280);
  node(106, 3520, 2370);
  node(107, 3920, 1680);
  node(108, 3800, 1830);
  node(109, 3800, 1920);
  node(110, 3800, 2010);
  node(111, 3800, 2100);
  node(112, 3800, 2190);
  node(113, 3800, 2280);
  node(114, 3800, 2370);
  node(115, 4200, 1680);
  node(116, 4480, 1680);
  node(117, 4360, 1830);
  node(118, 4360, 1920);
  node(119, 4760, 1680);
  node(120, 4640, 1830);
  node(121, 0, 2510);
  node(123, 560, 2510);
  node(359, 848, 2568);
  node(124, 440, 2660);
  node(125, 440, 2750);
  node(134, 1680, 2510);
  node(135, 1560, 2660);
  node(11, 1960, 2510);
  node(136, 1840, 2660);
  node(137, 1840, 2750);
  node(138, 1840, 2840);
  node(139, 2240, 2510);
  node(140, 2120, 2660);
  node(141, 2520, 2510);
  node(142, 2400, 2660);
  node(143, 2400, 2750);
  node(144, 2400, 2840);
  node(145, 2800, 2510);
  node(146, 3080, 2510);
  node(147, 3360, 2510);
  node(148, 3640, 2510);
  node(149, 3520, 2660);
  node(150, 3520, 2750);
  node(151, 3520, 2840);
  node(152, 3520, 2930);
  node(153, 3520, 3020);
  node(154, 3920, 2510);
  node(155, 3800, 2660);
  node(156, 3800, 2750);
  node(157, 3800, 2840);
  node(158, 3800, 2930);
  node(159, 3800, 3020);
  node(160, 3800, 3110);
  node(161, 3800, 3200);
  node(162, 3800, 3290);
  node(163, 4200, 2510);
  node(164, 4480, 2510);
  node(165, 4360, 2660);
  node(166, 0, 3430);
  node(167, 280, 3430);
  node(168, 160, 3580);
  node(169, 560, 3430);
  node(170, 840, 3430);
  node(171, 720, 3580);
  node(172, 1120, 3430);
  node(173, 1000, 3580);
  node(174, 1400, 3430);
  node(175, 1280, 3580);
  node(176, 1280, 3670);
  node(178, 1960, 3430);
  node(360, 2248, 3488);
  node(179, 1840, 3580);
  node(186, 2800, 3430);
  node(187, 2680, 3580);
  node(188, 3080, 3430);
  node(189, 3360, 3430);
  node(190, 3240, 3580);
  node(191, 3240, 3670);
  node(192, 3640, 3430);
  node(193, 3520, 3580);
  node(194, 3520, 3670);
  node(195, 3920, 3430);
  node(196, 3800, 3580);
  node(197, 3800, 3670);
  node(198, 4200, 3430);
  node(199, 4080, 3580);
  node(200, 4080, 3670);
  node(201, 4480, 3430);
  node(202, 4360, 3580);
  node(203, 4360, 3670);
  node(204, 4760, 3430);
  node(205, 4640, 3580);
  node(206, 4640, 3670);
  node(207, 4640, 3760);
  node(208, 5040, 3430);
  node(209, 4920, 3580);
  node(210, 0, 3990);
  node(211, 280, 3990);
  node(212, 160, 4140);
  node(213, 160, 4230);
  node(214, 560, 3990);
  node(215, 440, 4140);
  node(216, 440, 4230);
  node(217, 0, 4370);
  node(12, 280, 4370);
  node(13, 560, 4370);
  node(218, 840, 4370);
  node(219, 1120, 4370);
  node(220, 1000, 4520);
  node(221, 1400, 4370);
  node(222, 1280, 4520);
  node(223, 1680, 4370);
  node(224, 1560, 4520);
  node(225, 1560, 4610);
  node(226, 1560, 4700);
  node(227, 1560, 4790);
  node(228, 1560, 4880);
  node(229, 1560, 4970);
  node(230, 1560, 5060);
  node(231, 1560, 5150);
  node(232, 1560, 5240);
  node(233, 1560, 5330);
  node(234, 1560, 5420);
  node(235, 1560, 5510);
  node(236, 1560, 5600);
  node(237, 1960, 4370);
  node(238, 2240, 4370);
  node(239, 2120, 4520);
  node(240, 2120, 4610);
  node(241, 2520, 4370);
  node(242, 2400, 4520);
  node(243, 2800, 4370);
  node(244, 2680, 4520);
  node(245, 2680, 4610);
  node(246, 2680, 4700);
  node(247, 2680, 4790);
  node(248, 2680, 4880);
  node(249, 2680, 4970);
  node(250, 2680, 5060);
  node(251, 0, 5740);
  node(252, 280, 5740);
  node(253, 160, 5890);
  node(254, 160, 5980);
  node(255, 160, 6070);
  node(256, 0, 6210);
  node(257, 280, 6210);
  node(258, 160, 6360);
  node(259, 160, 6450);
  node(260, 560, 6210);
  node(261, 440, 6360);
  node(14, 840, 6210);
  node(262, 1120, 6210);
  node(263, 1000, 6360);
  node(264, 1000, 6450);
  node(265, 1400, 6210);
  node(266, 1280, 6360);
  node(267, 1280, 6450);
  node(268, 1280, 6540);
  node(15, 1680, 6210);
  node(269, 1960, 6210);
  node(270, 1840, 6360);
  node(271, 1840, 6450);
  node(272, 2240, 6210);
  node(273, 2120, 6360);
  node(274, 2120, 6450);
  node(275, 2520, 6210);
  node(276, 2400, 6360);
  node(277, 2400, 6450);
  node(278, 2800, 6210);
  node(279, 2680, 6360);
  node(280, 2680, 6450);
  node(16, 3080, 6210);
  node(281, 3360, 6210);
  node(282, 3240, 6360);
  node(283, 3240, 6450);
  node(284, 3240, 6540);
  node(17, 3640, 6210);
  node(285, 3920, 6210);
  node(286, 3800, 6360);
  node(287, 3800, 6450);
  node(288, 3800, 6540);
  node(289, 3800, 6630);
  node(18, 4200, 6210);
  node(290, 4480, 6210);
  node(291, 4360, 6360);
  node(292, 4360, 6450);
  node(293, 4360, 6540);
  node(294, 4360, 6630);
  node(295, 4760, 6210);
  node(296, 4640, 6360);
  node(297, 4640, 6450);
  node(298, 0, 6770);
  node(299, 280, 6770);
  node(300, 160, 6920);
  node(301, 560, 6770);
  node(302, 440, 6920);
  node(303, 440, 7010);
  node(304, 840, 6770);
  node(305, 720, 6920);
  node(306, 720, 7010);
  node(307, 720, 7100);
  node(308, 720, 7190);
  node(309, 1120, 6770);
  node(310, 1000, 6920);
  node(311, 1000, 7010);
  node(374, 0, 9168);
  node(375, 288, 9224);
  node(376, 240, 9376);
  node(377, 240, 9480);
  node(378, 240, 9584);
  node(379, 576, 9280);
  node(380, 864, 9336);
  node(381, 1152, 9392);
  node(382, 1104, 9544);
  node(383, 1104, 9648);
  node(384, 1104, 9752);
  node(385, 1104, 9856);
  node(386, 1104, 9960);
  node(387, 1440, 9448);
  node(388, 1392, 9600);
  node(389, 1392, 9704);
  node(390, 1728, 9504);
  node(391, 1680, 9656);
  node(392, 1680, 9760);
  node(393, 1680, 9864);
  node(394, 1680, 9968);
  node(395, 1680, 10072);
  node(396, 1680, 10176);
  node(397, 1680, 10280);
  node(398, 1680, 10384);
  node(399, 1680, 10488);
  node(400, 1680, 10592);
  node(401, 1680, 10696);
  node(402, 1680, 10800);
  node(403, 1680, 10904);
  node(404, 1680, 11008);
  node(405, 1680, 11112);
  node(406, 2016, 9560);
  node(407, 2304, 9616);
  node(408, 2256, 9768);
  node(409, 2256, 9872);
  node(410, 2592, 9672);
  node(411, 2880, 9728);
  node(412, 2832, 9880);
  node(413, 2832, 9984);
  node(414, 3168, 9784);
  node(415, 3456, 9840);
  node(416, 3744, 9896);
  node(417, 4032, 9952);
  node(418, 3984, 10104);
  node(419, 3984, 10208);
  node(420, 3984, 10312);
  node(421, 3984, 10416);
  node(19, 560, 7330);
  node(422, 512, 7480);
  node(423, 512, 7584);
  node(424, 512, 7688);
  node(425, 848, 7384);
  node(426, 800, 7536);
  node(427, 800, 7640);
  node(428, 1136, 7440);
  node(429, 1088, 7592);
  node(430, 1424, 7496);
  node(431, 1376, 7648);
  node(432, 1376, 7752);
  node(433, 1376, 7856);
  node(434, 1712, 7552);
  node(435, 1664, 7704);
  node(436, 1664, 7808);
  node(437, 1664, 7912);
  node(438, 1664, 8016);
  node(439, 2000, 7608);
  node(440, 1952, 7760);
  node(441, 1952, 7864);
  node(442, 1952, 7968);
  node(443, 1952, 8072);
  node(444, 2288, 7664);
  node(445, 2576, 7720);
  node(446, 2864, 7776);
  node(447, 2816, 7928);
  node(448, 2816, 8032);
  node(449, 2816, 8136);
  node(450, 2816, 8240);
  node(451, 2816, 8344);
  node(452, 3152, 7832);
  node(453, 3104, 7984);
  node(454, 3440, 7888);
  node(455, 3392, 8040);
  node(456, 3392, 8144);
  node(457, 3728, 7944);
  node(458, 4016, 8000);
  node(459, 3968, 8152);
  node(460, 3968, 8256);
  node(461, 3968, 8360);
  node(462, 4304, 8056);
  node(463, 4256, 8208);
  node(20, 1960, 7330);
  node(464, 1912, 7480);
  node(465, 1912, 7584);
  node(466, 1912, 7688);
  node(467, 2248, 7384);
  node(468, 2200, 7536);
  node(469, 2536, 7440);
  node(470, 2488, 7592);
  node(471, 2488, 7696);
  node(472, 2488, 7800);
  node(473, 2488, 7904);
  node(474, 2824, 7496);
  node(475, 2776, 7648);
  node(476, 2776, 7752);
  node(477, 2776, 7856);
  node(478, 3112, 7552);
  node(479, 3064, 7704);
  node(480, 3064, 7808);
  node(481, 3064, 7912);
  node(482, 3400, 7608);
  node(483, 3352, 7760);
  node(484, 3352, 7864);
  node(485, 3352, 7968);
  node(486, 3688, 7664);
  node(487, 3640, 7816);
  node(488, 3640, 7920);
  node(489, 3640, 8024);
  node(490, 3976, 7720);
  node(491, 3928, 7872);
  node(492, 3928, 7976);
  node(493, 3928, 8080);
  node(494, 4264, 7776);
  node(495, 4216, 7928);
  node(496, 4216, 8032);
  node(497, 4552, 7832);
  node(498, 4840, 7888);
  node(499, 5128, 7944);
  node(500, 5416, 8000);
  node(501, 5368, 8152);
  node(502, 5368, 8256);
  node(503, 5368, 8360);
  node(504, 5704, 8056);
  node(505, 5656, 8208);
  node(506, 5656, 8312);
  node(507, 5656, 8416);
  node(508, 5656, 8520);
  node(509, 5656, 8624);
  node(510, 5992, 8112);
  node(511, 6280, 8168);
  node(512, 6232, 8320);
  node(513, 6568, 8224);
  node(514, 6520, 8376);
  node(515, 6520, 8480);
  node(516, 6520, 8584);
  node(517, 6520, 8688);
  node(518, 6520, 8792);
  node(519, 6856, 8280);
  node(520, 6808, 8432);
  node(521, 6808, 8536);
  node(522, 6808, 8640);
  node(523, 6808, 8744);
  node(524, 6808, 8848);
  node(21, 3640, 7330);
  node(525, 3592, 7480);
  node(526, 3592, 7584);
  node(527, 3928, 7384);
  node(528, 3880, 7536);
  node(529, 3880, 7640);
  node(530, 4216, 7440);
  node(531, 4168, 7592);
  node(532, 4504, 7496);
  node(533, 4456, 7648);
  node(534, 4792, 7552);
  node(535, 4744, 7704);
  node(536, 5080, 7608);
  node(537, 5032, 7760);
  node(538, 5032, 7864);
  node(539, 5032, 7968);
  node(540, 5368, 7664);
  node(541, 5656, 7720);
  node(542, 5608, 7872);
  node(543, 5608, 7976);
  node(544, 5944, 7776);
  node(545, 5896, 7928);
  node(546, 5896, 8032);
  node(547, 6232, 7832);
  node(548, 6184, 7984);
  node(549, 6184, 8088);
  node(550, 6520, 7888);
  node(551, 6808, 7944);
  node(22, 7096, 8000);
  node(552, 7048, 8152);
  node(553, 7048, 8256);
  node(554, 7048, 8360);
  node(23, 7384, 8056);
  node(555, 7336, 8208);
  node(556, 7336, 8312);
  node(557, 7336, 8416);
  node(558, 7672, 8112);
  node(559, 7624, 8264);
  node(560, 7624, 8368);
  node(561, 7624, 8472);
  node(562, 7624, 8576);
  node(563, 7624, 8680);
  node(564, 7624, 8784);
  node(565, 7624, 8888);
  node(566, 7624, 8992);
  node(567, 7624, 9096);
  node(568, 7960, 8168);
  node(569, 7912, 8320);
  node(570, 8248, 8224);
  node(571, 8200, 8376);
  node(572, 8200, 8480);
  node(573, 8200, 8584);
  node(574, 8536, 8280);
  node(575, 8488, 8432);
  node(576, 8488, 8536);
  node(577, 8824, 8336);
  node(578, 9112, 8392);
  node(579, 9400, 8448);
  node(580, 9352, 8600);
  node(581, 9352, 8704);
  node(582, 9688, 8504);
  node(583, 9640, 8656);
  node(584, 9640, 8760);
  node(585, 9976, 8560);
  node(586, 10264, 8616);
  node(587, 10552, 8672);
  node(588, 10504, 8824);
  node(589, 10504, 8928);
  node(590, 10840, 8728);
  node(591, 11128, 8784);
  node(592, 11080, 8936);
  node(593, 11080, 9040);
  node(594, 11080, 9144);
  node(595, 11416, 8840);
  node(596, 11368, 8992);
  node(597, 11368, 9096);
  node(598, 11368, 9200);
  node(599, 11368, 9304);
  node(600, 11368, 9408);
  node(601, 11704, 8896);
  node(602, 11656, 9048);
  node(603, 11656, 9152);
  node(604, 11656, 9256);
  node(605, 11656, 9360);
  node(606, 11656, 9464);
  node(607, 11992, 8952);
  node(608, 12280, 9008);
  node(609, 12568, 9064);
  node(610, 12520, 9216);
  node(611, 12520, 9320);
  node(612, 12856, 9120);
  node(613, 12808, 9272);
  node(614, 12808, 9376);
  node(615, 13144, 9176);
  node(616, 13096, 9328);
  node(617, 13096, 9432);
  node(24, 13432, 9232);
  node(618, 13384, 9384);
  node(619, 13384, 9488);
  node(620, 13384, 9592);
  node(25, 13720, 9288);
  node(621, 13672, 9440);
  node(622, 13672, 9544);
  node(623, 13672, 9648);
  node(624, 14008, 9344);
  node(625, 14296, 9400);
  node(626, 14248, 9552);
  node(627, 14248, 9656);
  node(628, 14248, 9760);
  node(629, 14248, 9864);
  node(630, 14248, 9968);
  node(631, 14248, 10072);
  node(632, 14248, 10176);
  node(633, 14248, 10280);
  node(634, 14248, 10384);
  node(635, 14248, 10488);
  node(636, 14248, 10592);
  node(637, 14248, 10696);
  node(638, 14248, 10800);
  node(639, 14248, 10904);
  node(640, 14248, 11008);
  node(641, 14248, 11112);
  node(642, 14248, 11216);
  node(643, 14248, 11320);
  node(644, 14248, 11424);
  node(645, 14584, 9456);
  node(646, 14872, 9512);
  node(647, 14824, 9664);
  node(648, 14824, 9768);
  node(26, 15160, 9568);
  node(27, 15448, 9624);
  node(649, 15736, 9680);
  node(650, 15688, 9832);
  node(651, 15688, 9936);
  node(652, 15688, 10040);
  node(653, 15688, 10144);
  node(654, 15688, 10248);
  node(655, 15688, 10352);
  node(656, 15688, 10456);
  node(657, 16024, 9736);
  node(658, 16312, 9792);
  node(659, 16264, 9944);
  node(660, 16264, 10048);
  node(661, 16600, 9848);
  node(662, 16552, 10000);
  node(663, 16552, 10104);
  node(28, 16888, 9904);
  node(664, 17176, 9960);
  node(665, 17128, 10112);
  node(666, 17128, 10216);
  node(667, 17464, 10016);
  node(668, 17416, 10168);
  node(669, 17416, 10272);
  node(670, 17416, 10376);
  node(29, 17752, 10072);
  node(671, 18040, 10128);
  node(672, 17992, 10280);
  node(673, 17992, 10384);
  node(674, 17992, 10488);
  node(675, 17992, 10592);
  node(676, 18328, 10184);
  node(677, 18280, 10336);
  node(678, 18280, 10440);
  node(679, 18616, 10240);
  node(680, 18568, 10392);
  node(681, 18904, 10296);
  node(682, 18856, 10448);
  node(683, 18856, 10552);
  node(684, 18856, 10656);
  node(685, 19192, 10352);
  node(686, 19144, 10504);
  node(687, 19144, 10608);
  node(688, 19144, 10712);
  node(689, 19480, 10408);
  node(690, 19768, 10464);
  node(691, 19720, 10616);
  node(692, 19720, 10720);
  node(693, 20056, 10520);
  node(694, 20008, 10672);
  node(695, 20008, 10776);
  node(696, 20344, 10576);
  node(697, 20296, 10728);
  node(698, 20296, 10832);
  node(699, 20632, 10632);
  node(700, 20920, 10688);
  node(701, 21208, 10744);
  node(702, 21160, 10896);
  node(703, 21160, 11000);
  node(704, 21160, 11104);
  node(30, 21496, 10800);
  node(705, 21784, 10856);
  node(706, 22072, 10912);
  node(707, 22024, 11064);
  node(708, 22024, 11168);
  node(709, 22360, 10968);
  node(710, 22312, 11120);
  node(711, 22312, 11224);
  node(712, 22312, 11328);
  node(713, 22312, 11432);
  node(714, 22648, 11024);
  node(715, 22600, 11176);
  node(716, 22600, 11280);
  node(717, 22600, 11384);
  node(718, 22600, 11488);
  node(719, 22936, 11080);
  node(720, 22888, 11232);
  node(721, 22888, 11336);
  node(722, 22888, 11440);
  node(723, 22888, 11544);
  node(724, 23224, 11136);
  node(725, 23176, 11288);
  node(726, 23176, 11392);
  node(727, 23176, 11496);
  node(728, 23512, 11192);
  node(729, 23800, 11248);
  node(730, 23752, 11400);
  node(731, 23752, 11504);
  node(732, 24088, 11304);
  node(733, 24040, 11456);
  node(734, 24040, 11560);
  node(735, 24040, 11664);
  node(736, 24040, 11768);
  node(737, 24040, 11872);
  node(738, 24040, 11976);
  node(739, 24040, 12080);
  node(740, 24376, 11360);
  node(741, 24328, 11512);
  node(742, 24328, 11616);
  node(743, 24328, 11720);
  node(744, 24328, 11824);
  node(745, 24328, 11928);
  node(746, 24328, 12032);
  node(747, 24664, 11416);
  node(748, 24952, 11472);
  node(749, 24904, 11624);
  node(750, 24904, 11728);
  node(751, 24904, 11832);
  node(752, 24904, 11936);
  node(753, 24904, 12040);
  node(754, 25240, 11528);
  node(755, 25192, 11680);
  node(756, 25192, 11784);
  node(757, 25192, 11888);
  node(758, 25192, 11992);
  node(759, 25528, 11584);
  node(760, 25816, 11640);
  node(761, 25768, 11792);
  node(762, 25768, 11896);
  node(763, 26104, 11696);
  node(764, 26392, 11752);
  node(765, 26344, 11904);
  node(766, 26680, 11808);
  node(767, 26632, 11960);
  node(31, 26968, 11864);
  node(768, 27256, 11920);
  node(769, 27544, 11976);
  node(312, 0, 7330);
  node(313, 280, 7330);
  node(32, 568, 7384);
  node(314, 840, 7330);
  node(315, 720, 7480);
  node(316, 720, 7570);
  node(317, 720, 7660);
  node(318, 1120, 7330);
  node(319, 1000, 7480);
  node(320, 1000, 7570);
  node(321, 1000, 7660);
  node(322, 1400, 7330);
  node(323, 1280, 7480);
  node(324, 1280, 7570);
  node(325, 1280, 7660);
  node(326, 1680, 7330);
  node(327, 1560, 7480);
  node(328, 1560, 7570);
  node(329, 1560, 7660);
  node(33, 1968, 7384);
  node(330, 2240, 7330);
  node(331, 2120, 7480);
  node(332, 2120, 7570);
  node(333, 2120, 7660);
  node(334, 2120, 7750);
  node(335, 2520, 7330);
  node(336, 2400, 7480);
  node(337, 2400, 7570);
  node(338, 2800, 7330);
  node(339, 2680, 7480);
  node(340, 2680, 7570);
  node(341, 3080, 7330);
  node(342, 2960, 7480);
  node(343, 2960, 7570);
  node(344, 3360, 7330);
  node(345, 3240, 7480);
  node(346, 3240, 7570);
  node(34, 3648, 7384);
  node(347, 3920, 7330);
  node(348, 3800, 7480);
  node(349, 3800, 7570);
  node(350, 4200, 7330);
  node(351, 4080, 7480);
  node(352, 4080, 7570);
  node(353, 4080, 7660);
  node(354, 4080, 7750);
  node(355, 4080, 7840);
  node(356, 4080, 7930);
  node(357, 4080, 8020);
  next_id(770);
}


}  // namespace kern_blueprint_fe59a0c6af5496ca

// ---------------------------------------------------------------------------
// The Blueprint of actor "lamps_cpp"
namespace kern_blueprint_4ba64ee8b255fbb9 {

using blueprint_call = ::kern_blueprint_0eb7c15494c83360::blueprint_call;
using lamps = ::kern_blueprint_315081d4e46950aa::lamps;
using shapes = ::kern_blueprint_6025b4a819f918d1::shapes;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct lamps_cpp : virtual kern::Library, virtual blueprint_call, virtual lamps, virtual shapes {
  // ---- variables ----
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kGridM = 10.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kSpacing = 34.0;
  KERN_CONST Int kMax = 900;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kKerb = 1.15;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kArm = 1.42;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kHeadZ = 6.0;

  // ---- functions ----
  struct Lamps_WorseResult { Bool worse = false; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("Worse") Lamps_WorseResult Lamps_Worse(Int state = 0, Int to = 0);

  struct Lamps_ShotAtResult { Bool hit = false; Float along = 0.0; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("ShotAt") Lamps_ShotAtResult Lamps_ShotAt(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Vector head = Vector{0.0, 0.0, 0.0}, Int state = 0);

  struct Lamps_BlastReachResult { Float reach = 0.0; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("BlastReach") Lamps_BlastReachResult Lamps_BlastReach(Float radius = 0.0);

  struct Lamps_BlastFateResult { Int state = 0; Vector push = Vector{0.0, 0.0, 0.0}; Float speed = 0.0; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("BlastFate") Lamps_BlastFateResult Lamps_BlastFate(Vector foot = Vector{0.0, 0.0, 0.0}, Vector at = Vector{0.0, 0.0, 0.0}, Float reach = 0.0);

  struct Lamps_CarFateResult { Bool hit = false; Bool topple = false; Vector impulse = Vector{0.0, 0.0, 0.0}; Bool clang = false; };
  /// (of the Blueprint of the actor Lamps, called on it)
  KERN_EXTERN_AS("CarFate") Lamps_CarFateResult Lamps_CarFate(Vector foot = Vector{0.0, 0.0, 0.0}, Int state = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector velocity = Vector{0.0, 0.0, 0.0}, Float halfLength = 0.0, Float halfWidth = 0.0, Float mass = 0.0);

  struct worseResult { Bool Result = false; };
  /// The Blueprint's function Worse: may a lamp in `state` become `to`?
  worseResult worse(Int state = 0, Int to = 0) {
    Bool worse_2 = false;
    worse_2 = false;
    func_call_on<"Lamps_Worse">(actor_find("Lamps"), kern::out(n10.worse), state, to);
    worse_2 = n10.worse;
    return {worse_2};
  }

  struct grid_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE grid_keyResult grid_key(Float x = 0.0, Float y = 0.0) {
    return {int64_xor(int64_shift_left(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float x, Float kGridM) -> Float { return kern::math::floor(kern::math::div(x, kGridM)); }, x, kGridM))), 32), int64_and(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float y, Float kGridM) -> Float { return kern::math::floor(kern::math::div(y, kGridM)); }, y, kGridM))), Int64{0xffffffff}))};
  }

  struct bindResult { Bool Result = false; };
  KERN_PURE bindResult bind() {
    return {func_has_blueprint(actor_find("Lamps"))};
  }

  struct pole_radiusResult { Float Result = 0.0; };
  KERN_PURE pole_radiusResult pole_radius() {
    return {func_get_on<"PoleRadius", Float>(actor_find("Lamps"))};
  }

  struct cell_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE cell_keyResult cell_key(Float x = 0.0, Float y = 0.0) {
    return {int64_xor(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::round(kern::math::div(x, 8.0)); }, x))), 32), int64_and(int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::round(kern::math::div(y, 8.0)); }, y))), Int64{0xffffffff}))};
  }

  void Field_place_road(lamps_Field& self, const List<Vector>& pts, Float width = 0.0, Int lane = 0, Float ground = 0.0) {
    Float carry = 0.0;
    Float side = 0.0;
    Int i = 0;
    Bool skip = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Float dx = 0.0;
    Float dy = 0.0;
    Float l = 0.0;
    Float ux = 0.0;
    Float uy = 0.0;
    Float nx = 0.0;
    Float ny = 0.0;
    Float t = 0.0;
    Bool skip_2 = false;
    Float off = 0.0;
    Float x = 0.0;
    Float y = 0.0;
    Int64 key = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    lamps_Lamp lp = lamps_Lamp{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, 0, Int64{0x0}, 0};
    Int br = 0;
    List<Int> t_2 = List<Int>{};
    Int64 key_3 = Int64{0x0};
    List<Int> put = List<Int>{};
    carry = kern::formula([](Float kSpacing) -> Float { return kSpacing * 0.5; }, kSpacing);
    side = 1.0;
    i = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return i + 1.0; }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(pts))), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float kMax) -> Float { return kern::math::mul32(kMax, 8.0); }, kern::to<Float>(kMax)))))), 0.5)))) {
      skip = false;
      a = pts[i];
      b = pts[kern::to<Int>(kern::formula([](Float i) -> Float { return i + 1.0; }, kern::to<Float>(i)))];
      dx = kern::formula([](Float x, Float x_2) -> Float { return x - x_2; }, vector_break(b).x, vector_break(a).x);
      dy = kern::formula([](Float y, Float y_2) -> Float { return y - y_2; }, vector_break(b).y, vector_break(a).y);
      l = kern::formula([](Float dx, Float dy) -> Float { return kern::math::sqrt(dx * dx + dy * dy); }, dx, dy);
      if (math_compare<"<">(l, 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        ux = kern::formula([](Float dx, Float l) -> Float { return kern::math::div(dx, l); }, dx, l);
        uy = kern::formula([](Float dy, Float l) -> Float { return kern::math::div(dy, l); }, dy, l);
        nx = kern::formula([](Float uy) -> Float { return -uy; }, uy);
        ny = ux;
        t = carry;
        while (loop(math_compare<"<">(t, l))) {
          skip_2 = false;
          side = kern::formula([](Float side) -> Float { return -side; }, side);
          off = kern::formula([](Float width, Float kKerb) -> Float { return kern::math::div(width, 2.0) + kKerb; }, width, kKerb);
          x = kern::formula([](Float x, Float ux, Float t, Float nx, Float off, Float side) -> Float { return x + ux * t + nx * off * side; }, vector_break(a).x, ux, t, nx, off, side);
          y = kern::formula([](Float y, Float uy, Float t, Float ny, Float off, Float side) -> Float { return y + uy * t + ny * off * side; }, vector_break(a).y, uy, t, ny, off, side);
          key = cell_key(x, y).Result;
          key_2 = key;
          set_add(n11, self.seen_, key_2);
          if (logic_not(struct_break<Int_Bool_pair>(struct_make<Int_Bool_pair>(n11.index, n11.added)).second)) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            lp = lamps_Lamp{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, 0, Int64{0x0}, 0};
            lp.foot = vector_make(x, y, ground);
            lp.yaw = kern::formula([](Float ny, Float side, Float nx) -> Float { return kern::math::atan2(-ny * side, -nx * side); }, ny, side, nx);
            lp.head = vector_make(kern::formula([](Float x, Float yaw, Float kArm) -> Float { return x + kern::math::cos(yaw) * kArm; }, x, lp.yaw, kArm), kern::formula([](Float y, Float yaw, Float kArm) -> Float { return y + kern::math::sin(yaw) * kArm; }, y, lp.yaw, kArm), 0.0);
            lp.key = key;
            lp.lane = lane;
            br = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Int>(self.broken_, key).index), kern::to<Float>(map_length<Int64, Int>(self.broken_))));
            if (logic_not(math_compare<"==">(kern::to<Float>(br), kern::to<Float>(map_length<Int64, Int>(self.broken_))))) {
              lp.state = self.broken_[br];
            }
            key_3 = grid_key(x, y).Result;
            map_add(n12, self.grid_, key_3);
            put = std::move(t_2);
            t_2 = self.grid_[n12.index];
            list_add(t_2, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_)))));
            self.grid_[n12.index] = std::move(put);
            list_add(self.lamps_, lp);
          }
          t += kSpacing;
        }
        carry = kern::formula([](Float carry, Float l, Float kSpacing) -> Float { return kern::math::mod(kern::math::mod(carry - l, kSpacing) + kSpacing, kSpacing); }, carry, l, kSpacing);
      }
      i += 1;
    }
  }

  struct Field_nearResult { List<Int> Result = List<Int>{}; };
  KERN_PURE Field_nearResult Field_near(const lamps_Field& self, Float x = 0.0, Float y = 0.0, Float r = 0.0) {
    List<Int> out = List<Int>{};
    Int64 g0 = Int64{0x0};
    Int64 g1 = Int64{0x0};
    Int64 h0 = Int64{0x0};
    Int64 h1 = Int64{0x0};
    Int64 i = Int64{0x0};
    Int64 j = Int64{0x0};
    Bool skip = false;
    Int it = 0;
    Int at = 0;
    Int k = 0;
    out = List<Int>{};
    g0 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float x, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div(x - r, kGridM)); }, x, r, kGridM)));
    g1 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float x, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div(x + r, kGridM)); }, x, r, kGridM)));
    h0 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float y, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div(y - r, kGridM)); }, y, r, kGridM)));
    h1 = int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float y, Float r, Float kGridM) -> Float { return kern::math::floor(kern::math::div(y + r, kGridM)); }, y, r, kGridM)));
    i = g0;
    while (loop(logic_not(int64_less(g1, i)))) {
      j = h0;
      while (loop(logic_not(int64_less(h1, j)))) {
        skip = false;
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(self.grid_, int64_xor(int64_shift_left(i, 32), int64_and(j, Int64{0xffffffff}))).index), kern::to<Float>(map_length<Int64, List<Int>>(self.grid_))));
        if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(self.grid_)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          at = it;
          for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(self.grid_[at]))))); it15.next();) {
            k = self.grid_[at][n15.index];
            if (math_compare<"<=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt((x - x_2) * (x - x_2) + (y - y_2) * (y - y_2)); }, vector_break(self.lamps_[k].foot).x, x, vector_break(self.lamps_[k].foot).y, y), r)) {
              list_add(out, k);
            }
          }
        }
        j = int64_add(j, Int64{0x1});
      }
      i = int64_add(i, Int64{0x1});
    }
    return {std::move(out)};
  }

  struct Field_set_stateResult { Bool Result = false; };
  Field_set_stateResult Field_set_state(lamps_Field& self, Int i = 0, Int state = 0) {
    Int put = 0;
    Int64 key = Int64{0x0};
    if (logic_or(math_compare<"<">(kern::to<Float>(i), 0.0), math_compare<">=">(kern::to<Float>(i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_))))))))) {
      return {false};
    }
    n16 = worse(self.lamps_[i].state, state);
    if (logic_not(n16.Result)) {
      return {false};
    }
    self.lamps_[i].state = state;
    put = state;
    key = self.lamps_[i].key;
    map_add(n17, self.broken_, key);
    self.broken_[n17.index] = put;
    return {true};
  }

  struct Field_merge_brokenResult { Bool Result = false; };
  Field_merge_brokenResult Field_merge_broken(lamps_Field& self, Int64 key = Int64{0x0}, Int state = 0) {
    Int was = 0;
    Int put = 0;
    Int64 key_2 = Int64{0x0};
    Bool both = false;
    was = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Int>(self.broken_, key).index), kern::to<Float>(map_length<Int64, Int>(self.broken_))));
    n18 = worse(kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(a != 0.0 ? 0.0 : a_2, 255.0); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(was), kern::to<Float>(map_length<Int64, Int>(self.broken_)))), kern::to<Float>(self.broken_[was]))), state);
    if (logic_not(n18.Result)) {
      return {false};
    }
    put = state;
    key_2 = key;
    map_add(n19, self.broken_, key_2);
    self.broken_[n19.index] = put;
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(self.lamps_))))); it20.next();) {
      both = int64_equal(self.lamps_[n20.index].key, key);
      if (both) {
        n22 = worse(self.lamps_[n20.index].state, state);
        both = n22.Result;
      }
      if (both) {
        self.lamps_[n20.index].state = state;
      }
    }
    return {true};
  }

  struct Field_shotResult { Int Result = 0; };
  Field_shotResult Field_shot(const lamps_Field& self, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    Float L = 0.0;
    Float mx = 0.0;
    Float my = 0.0;
    Int best = 0;
    Float first = 0.0;
    List<Int> list = List<Int>{};
    Bool skip = false;
    Int k = 0;
    Bool hit = false;
    Float along = 0.0;
    L = vector_length(vector_sub(b, a));
    mx = kern::formula([](Float x, Float x_2) -> Float { return kern::math::div(x + x_2, 2.0); }, vector_break(a).x, vector_break(b).x);
    my = kern::formula([](Float y, Float y_2) -> Float { return kern::math::div(y + y_2, 2.0); }, vector_break(a).y, vector_break(b).y);
    best = -1;
    first = 1000000000.0;
    list = Field_near(self, mx, my, kern::formula([](Float l) -> Float { return kern::math::div(l, 2.0) + 2.0; }, L)).Result;
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(list))))); it23.next();) {
      k = list[n23.index];
      skip = false;
      hit = false;
      along = 0.0;
      func_call_on<"Lamps_ShotAt">(actor_find("Lamps"), kern::out(n24.hit, n24.along), a, b, vector_make(vector_break(self.lamps_[k].head).x, vector_break(self.lamps_[k].head).y, kern::formula([](Float z, Float kHeadZ) -> Float { return z + kHeadZ; }, vector_break(self.lamps_[k].foot).z, kHeadZ)), self.lamps_[k].state);
      hit = n24.hit;
      along = n24.along;
      if (logic_not(hit)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (math_compare<"<">(along, first)) {
          first = along;
          best = k;
        }
      }
    }
    return {best};
  }

  struct Field_blastResult { List<lamps_Field_BlastHit> Result = List<lamps_Field_BlastHit>{}; };
  Field_blastResult Field_blast(const lamps_Field& self, Vector p = Vector{0.0, 0.0, 0.0}, Float radius = 0.0) {
    List<lamps_Field_BlastHit> out = List<lamps_Field_BlastHit>{};
    Float blast_reach = 0.0;
    Float reach = 0.0;
    List<Int> list = List<Int>{};
    Int k = 0;
    Int state = 0;
    Vector push = Vector{0.0, 0.0, 0.0};
    Float speed = 0.0;
    out = List<lamps_Field_BlastHit>{};
    blast_reach = 0.0;
    func_call_on<"Lamps_BlastReach">(actor_find("Lamps"), kern::out(n25.reach), blueprint_call::decimal(radius).Result);
    blast_reach = n25.reach;
    reach = blast_reach;
    list = Field_near(self, vector_break(p).x, vector_break(p).y, reach).Result;
    for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(list))))); it26.next();) {
      k = list[n26.index];
      state = 0;
      push = Vector{0.0, 0.0, 0.0};
      speed = 0.0;
      func_call_on<"Lamps_BlastFate">(actor_find("Lamps"), kern::out(n27.state, n27.push, n27.speed), self.lamps_[k].foot, p, blueprint_call::decimal(reach).Result);
      state = n27.state;
      push = n27.push;
      speed = n27.speed;
      list_add(out, struct_make<lamps_Field_BlastHit>(k, state, vector_make(vector_break(push).x, vector_break(push).y, 0.0), speed));
    }
    return {std::move(out)};
  }

  struct Field_carResult { List<lamps_Field_CarHit> Result = List<lamps_Field_CarHit>{}; };
  Field_carResult Field_car(const lamps_Field& self, Vector pos = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector v = Vector{0.0, 0.0, 0.0}, Float hl = 0.0, Float hw = 0.0, Float m = 0.0) {
    List<lamps_Field_CarHit> out = List<lamps_Field_CarHit>{};
    List<Int> list = List<Int>{};
    Bool skip = false;
    Int k = 0;
    Bool hit = false;
    lamps_Field_CarHit h = lamps_Field_CarHit{-1, false, Vector{0.0, 0.0, 0.0}, false};
    out = List<lamps_Field_CarHit>{};
    list = Field_near(self, vector_break(pos).x, vector_break(pos).y, kern::formula([](Float hl) -> Float { return hl + 1.0; }, hl)).Result;
    for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(list))))); it28.next();) {
      k = list[n28.index];
      skip = false;
      hit = false;
      h = lamps_Field_CarHit{-1, false, Vector{0.0, 0.0, 0.0}, false};
      func_call_on<"Lamps_CarFate">(actor_find("Lamps"), kern::out(n29.hit, n29.topple, n29.impulse, n29.clang), self.lamps_[k].foot, self.lamps_[k].state, pos, blueprint_call::decimal(yaw).Result, v, blueprint_call::decimal(hl).Result, blueprint_call::decimal(hw).Result, blueprint_call::decimal(m).Result);
      hit = n29.hit;
      h.topple = n29.topple;
      h.impulse = n29.impulse;
      h.clang = n29.clang;
      if (logic_not(hit)) {
        skip = true;
      }
      if (logic_not(skip)) {
        h.lamp = k;
        list_add(out, h);
      }
    }
    return {std::move(out)};
  }

  // ---- what nodes give and keep ----
  struct { Bool worse = false; } n10;
  struct { Bool added = false; Int index = 0; } n11;
  struct { Bool added = false; Int index = 0; } n12;
  struct : kern::Node<15> { Int index = 0; } n15;
  worseResult n16;
  struct { Bool added = false; Int index = 0; } n17;
  worseResult n18;
  struct { Bool added = false; Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  worseResult n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct { Bool hit = false; Float along = 0.0; } n24;
  struct { Float reach = 0.0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct { Int state = 0; Vector push = Vector{0.0, 0.0, 0.0}; Float speed = 0.0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct { Bool hit = false; Bool topple = false; Vector impulse = Vector{0.0, 0.0, 0.0}; Bool clang = false; } n29;
};

KERN_LAYOUT(lamps_cpp) {
  node(125, 0, 3712);
  node(126, 288, 3768);
  node(10, 576, 3824);
  node(127, 528, 3976);
  node(128, 864, 3880);
  node(129, 1152, 3936);
  node(130, 1104, 4088);
  node(131, 0, 4368);
  node(132, 288, 4424);
  node(133, 240, 4576);
  node(134, 240, 4680);
  node(135, 240, 4784);
  node(136, 240, 4888);
  node(137, 240, 4992);
  node(138, 240, 5096);
  node(139, 240, 5200);
  node(140, 240, 5304);
  node(21, 160, 870);
  node(141, 240, 5408);
  node(142, 240, 5512);
  node(143, 0, 5792);
  node(144, 288, 5848);
  node(145, 240, 6000);
  node(146, 240, 6104);
  node(147, 0, 6384);
  node(148, 288, 6440);
  node(149, 240, 6592);
  node(150, 240, 6696);
  node(151, 0, 6976);
  node(152, 288, 7032);
  node(153, 240, 7184);
  node(154, 240, 7288);
  node(155, 240, 7392);
  node(156, 240, 7496);
  node(157, 240, 7600);
  node(158, 240, 7704);
  node(159, 240, 7808);
  node(160, 0, 8088);
  node(161, 288, 8144);
  node(162, 240, 8296);
  node(163, 240, 8400);
  node(164, 576, 8200);
  node(165, 864, 8256);
  node(166, 1152, 8312);
  node(167, 1104, 8464);
  node(168, 1104, 8568);
  node(169, 1104, 8672);
  node(30, 160, 1700);
  node(31, 160, 1790);
  node(170, 1104, 8776);
  node(171, 1104, 8880);
  node(172, 1104, 8984);
  node(173, 1104, 9088);
  node(174, 1104, 9192);
  node(175, 1104, 9296);
  node(176, 1104, 9400);
  node(34, 160, 2060);
  node(177, 1104, 9504);
  node(178, 1104, 9608);
  node(179, 1440, 8368);
  node(180, 1728, 8424);
  node(181, 1680, 8576);
  node(182, 1680, 8680);
  node(183, 2016, 8480);
  node(184, 1968, 8632);
  node(185, 1968, 8736);
  node(186, 1968, 8840);
  node(187, 2304, 8536);
  node(188, 2256, 8688);
  node(189, 2256, 8792);
  node(190, 2256, 8896);
  node(191, 2256, 9000);
  node(192, 2256, 9104);
  node(193, 2592, 8592);
  node(194, 2544, 8744);
  node(195, 2544, 8848);
  node(196, 2544, 8952);
  node(197, 2544, 9056);
  node(198, 2544, 9160);
  node(199, 2880, 8648);
  node(200, 2832, 8800);
  node(201, 2832, 8904);
  node(202, 2832, 9008);
  node(203, 3168, 8704);
  node(204, 3120, 8856);
  node(205, 3120, 8960);
  node(206, 3456, 8760);
  node(207, 3744, 8816);
  node(208, 3696, 8968);
  node(209, 3696, 9072);
  node(210, 4032, 8872);
  node(211, 3984, 9024);
  node(212, 3984, 9128);
  node(213, 3984, 9232);
  node(214, 4320, 8928);
  node(215, 4272, 9080);
  node(216, 4272, 9184);
  node(217, 4272, 9288);
  node(218, 4608, 8984);
  node(219, 4560, 9136);
  node(220, 4560, 9240);
  node(221, 4896, 9040);
  node(222, 4848, 9192);
  node(223, 5184, 9096);
  node(224, 5136, 9248);
  node(225, 5472, 9152);
  node(226, 5424, 9304);
  node(227, 5424, 9408);
  node(228, 5424, 9512);
  node(229, 5760, 9208);
  node(230, 6048, 9264);
  node(231, 6000, 9416);
  node(232, 6000, 9520);
  node(233, 6336, 9320);
  node(234, 6288, 9472);
  node(235, 6288, 9576);
  node(236, 6624, 9376);
  node(237, 6576, 9528);
  node(238, 6576, 9632);
  node(239, 6576, 9736);
  node(240, 6576, 9840);
  node(241, 6576, 9944);
  node(242, 6576, 10048);
  node(243, 6576, 10152);
  node(244, 6576, 10256);
  node(245, 6912, 9432);
  node(35, 160, 2150);
  node(246, 6864, 9584);
  node(247, 6864, 9688);
  node(248, 6864, 9792);
  node(249, 6864, 9896);
  node(250, 6864, 10000);
  node(251, 6864, 10104);
  node(252, 6864, 10208);
  node(253, 7200, 9488);
  node(254, 7152, 9640);
  node(255, 7152, 9744);
  node(256, 7152, 9848);
  node(257, 7488, 9544);
  node(258, 7440, 9696);
  node(11, 7776, 9600);
  node(259, 7728, 9752);
  node(260, 8064, 9656);
  node(261, 8016, 9808);
  node(262, 8016, 9912);
  node(36, 160, 2240);
  node(263, 8352, 9712);
  node(264, 8640, 9768);
  node(265, 8592, 9920);
  node(266, 8592, 10024);
  node(267, 8928, 9824);
  node(268, 9216, 9880);
  node(269, 9168, 10032);
  node(270, 9168, 10136);
  node(271, 9168, 10240);
  node(272, 9504, 9936);
  node(273, 9456, 10088);
  node(274, 9456, 10192);
  node(275, 9456, 10296);
  node(276, 9456, 10400);
  node(277, 9792, 9992);
  node(278, 9744, 10144);
  node(279, 9744, 10248);
  node(280, 9744, 10352);
  node(281, 9744, 10456);
  node(282, 9744, 10560);
  node(283, 9744, 10664);
  node(284, 9744, 10768);
  node(285, 9744, 10872);
  node(286, 9744, 10976);
  node(287, 10080, 10048);
  node(288, 10032, 10200);
  node(289, 10368, 10104);
  node(290, 10656, 10160);
  node(291, 10608, 10312);
  node(292, 10608, 10416);
  node(293, 10608, 10520);
  node(294, 10608, 10624);
  node(295, 10608, 10728);
  node(296, 10608, 10832);
  node(297, 10944, 10216);
  node(298, 10896, 10368);
  node(299, 10896, 10472);
  node(300, 10896, 10576);
  node(301, 10896, 10680);
  node(302, 10896, 10784);
  node(303, 11232, 10272);
  node(304, 11184, 10424);
  node(305, 11184, 10528);
  node(306, 11520, 10328);
  node(307, 11472, 10480);
  node(308, 11472, 10584);
  node(309, 11472, 10688);
  node(12, 11808, 10384);
  node(310, 11760, 10536);
  node(311, 12096, 10440);
  node(312, 12048, 10592);
  node(313, 12384, 10496);
  node(314, 12336, 10648);
  node(315, 12672, 10552);
  node(316, 12624, 10704);
  node(317, 12624, 10808);
  node(318, 12624, 10912);
  node(319, 12960, 10608);
  node(320, 12912, 10760);
  node(321, 13248, 10664);
  node(322, 13200, 10816);
  node(323, 13536, 10720);
  node(324, 13488, 10872);
  node(325, 13824, 10776);
  node(326, 13776, 10928);
  node(327, 13776, 11032);
  node(328, 13776, 11136);
  node(329, 13776, 11240);
  node(330, 14112, 10832);
  node(37, 0, 2380);
  node(38, 280, 2380);
  node(39, 560, 2380);
  node(331, 512, 2536);
  node(41, 440, 2620);
  node(42, 440, 2710);
  node(43, 440, 2800);
  node(44, 840, 2380);
  node(332, 792, 2536);
  node(46, 720, 2620);
  node(47, 720, 2710);
  node(48, 720, 2800);
  node(49, 1120, 2380);
  node(333, 1072, 2536);
  node(51, 1000, 2620);
  node(52, 1000, 2710);
  node(53, 1000, 2800);
  node(54, 1400, 2380);
  node(334, 1352, 2536);
  node(56, 1280, 2620);
  node(57, 1280, 2710);
  node(58, 1280, 2800);
  node(59, 1680, 2380);
  node(60, 1560, 2530);
  node(61, 1960, 2380);
  node(62, 1840, 2530);
  node(335, 1912, 2536);
  node(64, 1840, 2710);
  node(65, 1840, 2800);
  node(66, 2240, 2380);
  node(67, 2120, 2530);
  node(68, 2520, 2380);
  node(69, 2400, 2530);
  node(336, 2472, 2536);
  node(71, 2400, 2710);
  node(72, 2400, 2800);
  node(73, 2800, 2380);
  node(74, 3080, 2380);
  node(75, 2960, 2530);
  node(337, 3032, 2536);
  node(77, 2960, 2710);
  node(338, 3032, 2640);
  node(339, 3032, 2744);
  node(80, 2960, 2980);
  node(340, 3032, 2848);
  node(82, 2960, 3160);
  node(341, 3032, 2952);
  node(85, 2960, 3430);
  node(86, 3360, 2380);
  node(87, 3240, 2530);
  node(88, 3240, 2620);
  node(342, 3312, 2536);
  node(90, 3240, 2800);
  node(91, 3640, 2380);
  node(92, 3920, 2380);
  node(93, 3800, 2530);
  node(94, 3800, 2620);
  node(95, 4200, 2380);
  node(96, 4080, 2530);
  node(15, 4488, 2440);
  node(97, 4360, 2530);
  node(98, 4360, 2620);
  node(99, 4360, 2710);
  node(100, 4360, 2800);
  node(101, 4760, 2380);
  node(102, 4640, 2530);
  node(103, 4640, 2620);
  node(104, 5040, 2380);
  node(105, 4920, 2530);
  node(106, 4920, 2620);
  node(107, 4920, 2710);
  node(108, 4920, 2800);
  node(109, 4920, 2890);
  node(110, 4920, 2980);
  node(111, 4920, 3070);
  node(112, 4920, 3160);
  node(113, 5320, 2380);
  node(114, 5200, 2530);
  node(115, 5600, 2380);
  node(343, 5552, 2536);
  node(117, 5480, 2620);
  node(119, 5880, 2380);
  node(344, 5832, 2536);
  node(121, 5760, 2620);
  node(123, 6160, 2380);
  node(124, 6040, 2530);
  node(345, 0, 11520);
  node(346, 288, 11576);
  node(347, 240, 11728);
  node(348, 240, 11832);
  node(349, 240, 11936);
  node(350, 240, 12040);
  node(351, 240, 12144);
  node(352, 240, 12248);
  node(353, 240, 12352);
  node(354, 576, 11632);
  node(16, 864, 11688);
  node(355, 816, 11840);
  node(356, 1152, 11744);
  node(357, 1104, 11896);
  node(358, 1440, 11800);
  node(359, 1728, 11856);
  node(360, 2016, 11912);
  node(361, 2304, 11968);
  node(362, 2256, 12120);
  node(17, 2592, 12024);
  node(363, 2544, 12176);
  node(364, 2880, 12080);
  node(365, 2832, 12232);
  node(366, 3168, 12136);
  node(367, 0, 12632);
  node(368, 288, 12688);
  node(369, 240, 12840);
  node(370, 240, 12944);
  node(371, 240, 13048);
  node(372, 240, 13152);
  node(373, 240, 13256);
  node(18, 576, 12744);
  node(374, 528, 12896);
  node(375, 528, 13000);
  node(376, 528, 13104);
  node(377, 528, 13208);
  node(378, 528, 13312);
  node(379, 528, 13416);
  node(380, 528, 13520);
  node(381, 864, 12800);
  node(382, 816, 12952);
  node(383, 1152, 12856);
  node(384, 1440, 12912);
  node(385, 1728, 12968);
  node(19, 2016, 13024);
  node(386, 1968, 13176);
  node(387, 2304, 13080);
  node(388, 2256, 13232);
  node(20, 2592, 13136);
  node(389, 2544, 13288);
  node(390, 2544, 13392);
  node(391, 2544, 13496);
  node(392, 2880, 13192);
  node(393, 2832, 13344);
  node(394, 2832, 13448);
  node(395, 3168, 13248);
  node(396, 3120, 13400);
  node(22, 3456, 13304);
  node(397, 3408, 13456);
  node(398, 3744, 13360);
  node(399, 4032, 13416);
  node(400, 3984, 13568);
  node(401, 4320, 13472);
  node(402, 4608, 13528);
  node(403, 0, 13848);
  node(404, 288, 13904);
  node(405, 240, 14056);
  node(406, 240, 14160);
  node(407, 576, 13960);
  node(408, 528, 14112);
  node(409, 528, 14216);
  node(410, 528, 14320);
  node(411, 864, 14016);
  node(412, 816, 14168);
  node(413, 816, 14272);
  node(414, 816, 14376);
  node(415, 1152, 14072);
  node(416, 1440, 14128);
  node(417, 1728, 14184);
  node(418, 1680, 14336);
  node(419, 1680, 14440);
  node(420, 1680, 14544);
  node(421, 1680, 14648);
  node(422, 1680, 14752);
  node(23, 2016, 14240);
  node(423, 1968, 14392);
  node(424, 1968, 14496);
  node(425, 1968, 14600);
  node(426, 2304, 14296);
  node(427, 2256, 14448);
  node(428, 2592, 14352);
  node(429, 2880, 14408);
  node(430, 3168, 14464);
  node(24, 3456, 14520);
  node(431, 3408, 14672);
  node(432, 3408, 14776);
  node(433, 3408, 14880);
  node(434, 3408, 14984);
  node(435, 3408, 15088);
  node(436, 3408, 15192);
  node(437, 3408, 15296);
  node(438, 3408, 15400);
  node(439, 3408, 15504);
  node(440, 3408, 15608);
  node(441, 3408, 15712);
  node(442, 3408, 15816);
  node(443, 3408, 15920);
  node(444, 3408, 16024);
  node(445, 3408, 16128);
  node(446, 3744, 14576);
  node(447, 4032, 14632);
  node(448, 4320, 14688);
  node(449, 4272, 14840);
  node(450, 4272, 14944);
  node(451, 4608, 14744);
  node(452, 4896, 14800);
  node(453, 4848, 14952);
  node(454, 4848, 15056);
  node(455, 5184, 14856);
  node(456, 5136, 15008);
  node(457, 5136, 15112);
  node(458, 5136, 15216);
  node(459, 5472, 14912);
  node(460, 5424, 15064);
  node(461, 5760, 14968);
  node(462, 5712, 15120);
  node(463, 6048, 15024);
  node(464, 6000, 15176);
  node(465, 0, 16408);
  node(466, 288, 16464);
  node(467, 576, 16520);
  node(25, 864, 16576);
  node(468, 816, 16728);
  node(469, 816, 16832);
  node(470, 1152, 16632);
  node(471, 1440, 16688);
  node(472, 1392, 16840);
  node(473, 1728, 16744);
  node(474, 1680, 16896);
  node(475, 1680, 17000);
  node(476, 1680, 17104);
  node(477, 1680, 17208);
  node(26, 2016, 16800);
  node(478, 1968, 16952);
  node(479, 1968, 17056);
  node(480, 1968, 17160);
  node(481, 2304, 16856);
  node(482, 2256, 17008);
  node(483, 2592, 16912);
  node(484, 2880, 16968);
  node(485, 3168, 17024);
  node(27, 3456, 17080);
  node(486, 3408, 17232);
  node(487, 3408, 17336);
  node(488, 3408, 17440);
  node(489, 3408, 17544);
  node(490, 3408, 17648);
  node(491, 3744, 17136);
  node(492, 4032, 17192);
  node(493, 4320, 17248);
  node(494, 4608, 17304);
  node(495, 4560, 17456);
  node(496, 4560, 17560);
  node(497, 4560, 17664);
  node(498, 4560, 17768);
  node(499, 4560, 17872);
  node(500, 4560, 17976);
  node(501, 4560, 18080);
  node(502, 4560, 18184);
  node(503, 4560, 18288);
  node(504, 4896, 17360);
  node(505, 4848, 17512);
  node(506, 0, 18568);
  node(507, 288, 18624);
  node(508, 576, 18680);
  node(509, 528, 18832);
  node(510, 528, 18936);
  node(511, 528, 19040);
  node(512, 528, 19144);
  node(28, 864, 18736);
  node(513, 816, 18888);
  node(514, 816, 18992);
  node(515, 816, 19096);
  node(516, 1152, 18792);
  node(517, 1104, 18944);
  node(518, 1440, 18848);
  node(519, 1728, 18904);
  node(520, 2016, 18960);
  node(29, 2304, 19016);
  node(521, 2256, 19168);
  node(522, 2256, 19272);
  node(523, 2256, 19376);
  node(524, 2256, 19480);
  node(525, 2256, 19584);
  node(526, 2256, 19688);
  node(527, 2256, 19792);
  node(528, 2256, 19896);
  node(529, 2256, 20000);
  node(530, 2592, 19072);
  node(531, 2880, 19128);
  node(532, 3168, 19184);
  node(533, 3456, 19240);
  node(534, 3744, 19296);
  node(535, 3696, 19448);
  node(536, 3696, 19552);
  node(537, 4032, 19352);
  node(538, 4320, 19408);
  node(539, 4272, 19560);
  node(540, 4272, 19664);
  node(541, 4608, 19464);
  node(542, 4560, 19616);
  node(543, 4896, 19520);
  node(544, 4848, 19672);
  node(545, 5184, 19576);
  node(546, 5136, 19728);
  next_id(547);
}


}  // namespace kern_blueprint_4ba64ee8b255fbb9

// ---------------------------------------------------------------------------
// The Blueprint of actor "mvt_cpp"
namespace kern_blueprint_02f7c0a1316e6ec3 {

using mvt = ::kern_blueprint_5eb1d61c7210d38b::mvt;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct mvt_cpp : virtual kern::Library, virtual mvt {
  // ---- types ----
  KERN_STRUCT(mvt_Reader) {
    Int p = 0;
    Int end = 0;
    Bool ok = true;
  };

  // ---- functions ----
  struct Reader_moreResult { Bool Result = false; };
  KERN_PURE Reader_moreResult Reader_more(const mvt_Reader& self) {
    return {logic_and(self.ok, math_compare<">">(kern::formula([](Float p, Float end) -> Float { return (p < 0.0) != (end < 0.0) ? end < 0.0 : p < end; }, kern::to<Float>(self.p), kern::to<Float>(self.end)), 0.5))};
  }

  struct Reader_varintResult { Int64 Result = Int64{0x0}; };
  Reader_varintResult Reader_varint(mvt_Reader& self, List<Int> bytes = List<Int>{}) {
    Int64 v = Int64{0x0};
    Int shift = 0;
    Int b = 0;
    v = Int64{0x0};
    shift = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float p, Float end) -> Float { return (p < 0.0) != (end < 0.0) ? end < 0.0 : p < end; }, kern::to<Float>(self.p), kern::to<Float>(self.end)), 0.5), math_compare<"<">(kern::to<Float>(shift), 64.0)))) {
      b = bytes[self.p];
      self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return p + 1.0; }, kern::to<Float>(self.p)));
      v = int64_or(v, int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float b) -> Float { return kern::math::band(b, 127.0); }, kern::to<Float>(b)))), shift));
      if (logic_not(math_compare<">">(kern::formula([](Float b) -> Float { return kern::math::band(b, 128.0) != 0.0; }, kern::to<Float>(b)), 0.5))) {
        return {v};
      }
      shift += 7;
    }
    self.ok = false;
    return {Int64{0x0}};
  }

  struct Reader_subResult { mvt_Reader Result = mvt_Reader{0, 0, true}; };
  Reader_subResult Reader_sub(mvt_Reader& self, List<Int> bytes = List<Int>{}) {
    Int64 n = Int64{0x0};
    mvt_Reader r = mvt_Reader{0, 0, true};
    n10 = Reader_varint(self, std::move(bytes));
    n = n10.Result;
    if (logic_or(logic_not(self.ok), int64_less_unsigned(int64_of_int(kern::to<Int>(kern::formula([](Float end, Float p) -> Float { return end - p; }, kern::to<Float>(self.end), kern::to<Float>(self.p)))), n))) {
      self.ok = false;
      return {struct_make<mvt_Reader>(self.end, self.end, false)};
    }
    r = struct_make<mvt_Reader>(self.p, int64_to_int(int64_add(int64_of_int(self.p), n)), true);
    self.p = int64_to_int(int64_add(int64_of_int(self.p), n));
    return {r};
  }

  void Reader_skip(mvt_Reader& self, List<Int> bytes = List<Int>{}, Int wire = 0) {
    Int selected = 0;
    Int64 n = Int64{0x0};
    selected = wire;
    if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
      Reader_varint(self, std::move(bytes));
    } else if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
      self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return p + 8.0; }, kern::to<Float>(self.p)));
    } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
      n13 = Reader_varint(self, std::move(bytes));
      n = n13.Result;
      if (int64_less_unsigned(int64_of_int(kern::to<Int>(kern::formula([](Float end, Float p) -> Float { return end - p; }, kern::to<Float>(self.end), kern::to<Float>(self.p)))), n)) {
        self.ok = false;
      } else {
        self.p = int64_to_int(int64_add(int64_of_int(self.p), n));
      }
    } else if (math_compare<"==">(kern::to<Float>(selected), 5.0)) {
      self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return p + 4.0; }, kern::to<Float>(self.p)));
    } else {
      self.ok = false;
    }
    if (math_compare<">">(kern::formula([](Float end, Float p) -> Float { return (end < 0.0) != (p < 0.0) ? p < 0.0 : end < p; }, kern::to<Float>(self.end), kern::to<Float>(self.p)), 0.5)) {
      self.ok = false;
    }
  }

  struct Reader_le32Result { Int Result = 0; };
  /// (little-endian, as protobuf writes them: the low byte first)
  KERN_PURE Reader_le32Result Reader_le32(List<Int> bytes = List<Int>{}, Int at = 0) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::bor(kern::math::bor(kern::math::bor(a, kern::math::wrap32(kern::math::shl(a_2, 8.0))), kern::math::wrap32(kern::math::shl(a_3, 16.0))), kern::math::wrap32(kern::math::shl(a_4, 24.0))); }, kern::to<Float>(bytes[at]), kern::to<Float>(bytes[kern::to<Int>(kern::formula([](Float at) -> Float { return at + 1.0; }, kern::to<Float>(at)))]), kern::to<Float>(bytes[kern::to<Int>(kern::formula([](Float at) -> Float { return at + 2.0; }, kern::to<Float>(at)))]), kern::to<Float>(bytes[kern::to<Int>(kern::formula([](Float at) -> Float { return at + 3.0; }, kern::to<Float>(at)))])))};
  }

  struct Reader_fixed64Result { Float Result = 0.0; };
  Reader_fixed64Result Reader_fixed64(mvt_Reader& self, List<Int> bytes = List<Int>{}) {
    Float d = 0.0;
    if (math_compare<">">(kern::formula([](Float end, Float a) -> Float { return (end < 0.0) != (a < 0.0) ? a < 0.0 : end < a; }, kern::to<Float>(self.end), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return p + 8.0; }, kern::to<Float>(self.p)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    d = int64_float_of_bits(int64_or(int64_shift_left(int64_of_int(Reader_le32(bytes, kern::to<Int>(kern::formula([](Float p) -> Float { return p + 4.0; }, kern::to<Float>(self.p)))).Result), 32), int64_of_int(Reader_le32(bytes, self.p).Result)));
    self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return p + 8.0; }, kern::to<Float>(self.p)));
    return {d};
  }

  struct Reader_fixed32Result { Float Result = 0.0; };
  Reader_fixed32Result Reader_fixed32(mvt_Reader& self, List<Int> bytes = List<Int>{}) {
    Float f = 0.0;
    if (math_compare<">">(kern::formula([](Float end, Float a) -> Float { return (end < 0.0) != (a < 0.0) ? a < 0.0 : end < a; }, kern::to<Float>(self.end), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return p + 4.0; }, kern::to<Float>(self.p)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    f = math_float32_of_bits(Reader_le32(bytes, self.p).Result);
    self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return p + 4.0; }, kern::to<Float>(self.p)));
    return {f};
  }

  struct zigzagResult { Int64 Result = Int64{0x0}; };
  KERN_PURE zigzagResult zigzag(Int64 v = Int64{0x0}) {
    return {int64_xor(int64_shift_right_unsigned(v, 1), int64_sub(Int64{0x0}, int64_and(v, Int64{0x1})))};
  }

  void read_geometry(List<Vector_row>& parts, List<Int> bytes = List<Int>{}, mvt_Reader r = mvt_Reader{0, 0, true}, mvt_GeomType type = mvt_GeomType::unknown) {
    mvt_Reader r_ = mvt_Reader{0, 0, true};
    Int64 x = Int64{0x0};
    Int64 y = Int64{0x0};
    Bool open = false;
    Int64 c = Int64{0x0};
    Int id = 0;
    Int count = 0;
    Int i = 0;
    r_ = r;
    x = Int64{0x0};
    y = Int64{0x0};
    open = false;
    while (loop(Reader_more(r_).Result)) {
      n15 = Reader_varint(r_, bytes);
      c = n15.Result;
      id = int64_low(int64_and(c, Int64{0x7}));
      count = int64_low(int64_shift_right_unsigned(c, 3));
      if (logic_or(math_compare<"==">(kern::to<Float>(id), 1.0), math_compare<"==">(kern::to<Float>(id), 2.0))) {
        i = 0;
        while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), kern::to<Float>(count)), Reader_more(r_).Result))) {
          n16 = Reader_varint(r_, bytes);
          x = int64_add(x, zigzag(n16.Result).Result);
          n17 = Reader_varint(r_, bytes);
          y = int64_add(y, zigzag(n17.Result).Result);
          if (logic_or(math_compare<"==">(kern::to<Float>(id), 1.0), logic_not(open))) {
            list_add(parts, Vector_row{List<Vector>{}});
            open = true;
          }
          list_add(parts[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(parts))))].items, vector_make(int64_to_float(x), int64_to_float(y), 0.0));
          i += 1;
        }
      } else if (math_compare<"==">(kern::to<Float>(id), 7.0)) {
      } else {
        return;
      }
    }
  }

  struct Layer_tagResult { Int Result = 0; };
  KERN_PURE Layer_tagResult Layer_tag(const mvt_Layer& self, const mvt_Feature& f, Text key = "") {
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Int_pair>(f.tags))))); it18.next();) {
      if (logic_and(logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(f.tags[n18.index].first), kern::to<Float>(list_length<Text>(self.keys))), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(f.tags[n18.index].second), kern::to<Float>(list_length<mvt_Value>(self.values))), 0.5)), text_equal(key, self.keys[f.tags[n18.index].first]))) {
        return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(f.tags[n18.index].second)))};
      }
    }
    return {-1};
  }

  struct Layer_textResult { Text Result = ""; };
  KERN_PURE Layer_textResult Layer_text(const mvt_Layer& self, const mvt_Feature& f, Text key = "") {
    Int v = 0;
    v = Layer_tag(self, f, key).Result;
    return {logic_select<Text>(logic_and(math_compare<">=">(kern::to<Float>(v), 0.0), enum_equal<mvt_Value_Kind>(self.values[v].kind, mvt_Value_Kind::string)), self.values[v].s, "")};
  }

  struct Layer_numberResult { Float Result = 0.0; };
  KERN_PURE Layer_numberResult Layer_number(const mvt_Layer& self, const mvt_Feature& f, Text key = "", Float fallback = 0.0) {
    Int v = 0;
    v = Layer_tag(self, f, key).Result;
    return {kern::formula([](Float v, Float a, Float a_2, Float fallback) -> Float { return v >= 0.0 && a != 0.0 ? a_2 : fallback; }, kern::to<Float>(v), kern::to<Float>(enum_equal<mvt_Value_Kind>(self.values[v].kind, mvt_Value_Kind::number)), self.values[v].n, fallback)};
  }

  struct Tile_layerResult { Int Result = 0; };
  KERN_PURE Tile_layerResult Tile_layer(const mvt_Tile& self, Text name = "") {
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<mvt_Layer>(self.layers))))); it19.next();) {
      if (text_equal(name, self.layers[n19.index].name)) {
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n19.index)))};
      }
    }
    return {-1};
  }

  struct ring_areaResult { Float Result = 0.0; };
  KERN_PURE ring_areaResult ring_area(const List<Vector>& ring) {
    Float a = 0.0;
    Int n = 0;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    a = 0.0;
    n = list_length<Vector>(ring);
    i = 0;
    j = kern::to<Int>(kern::formula([](Float n) -> Float { return n != 0.0 ? n - 1.0 : 0.0; }, kern::to<Float>(n)));
    while (loop(math_compare<">">(kern::formula([](Float i, Float n) -> Float { return (i < 0.0) != (n < 0.0) ? n < 0.0 : i < n; }, kern::to<Float>(i), kern::to<Float>(n)), 0.5))) {
      a += kern::formula([](Float x, Float y, Float x_2, Float y_2) -> Float { return x * y - x_2 * y_2; }, vector_break(ring[j]).x, vector_break(ring[i]).y, vector_break(ring[i]).x, vector_break(ring[j]).y);
      was = i;
      i += 1;
      j = was;
    }
    return {kern::formula([](Float a) -> Float { return a * 0.5; }, a)};
  }

  struct wanted_layerResult { Bool Result = false; };
  KERN_PURE wanted_layerResult wanted_layer(const Text& name, const List<Text>& wanted) {
    if (math_compare<"==">(kern::to<Float>(list_length<Text>(wanted)), 0.0)) {
      return {true};
    }
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Text>(wanted))))); it20.next();) {
      if (text_equal(wanted[n20.index], name)) {
        return {true};
      }
    }
    return {false};
  }

  struct read_featureResult { Bool Result = false; };
  read_featureResult read_feature(mvt_Feature& f, List<Int> bytes = List<Int>{}, mvt_Reader fr = mvt_Reader{0, 0, true}) {
    mvt_Reader fr_ = mvt_Reader{0, 0, true};
    mvt_Reader geometry = mvt_Reader{0, 0, true};
    Bool has_geometry = false;
    Int64 k = Int64{0x0};
    Int field = 0;
    Int wire = 0;
    mvt_Reader t = mvt_Reader{0, 0, true};
    Int a = 0;
    Int b = 0;
    fr_ = fr;
    geometry = mvt_Reader{0, 0, true};
    has_geometry = false;
    while (loop(Reader_more(fr_).Result)) {
      n22 = Reader_varint(fr_, bytes);
      k = n22.Result;
      field = int64_low(int64_shift_right_unsigned(k, 3));
      wire = int64_low(int64_and(k, Int64{0x7}));
      if (logic_and(math_compare<"==">(kern::to<Float>(field), 1.0), math_compare<"==">(kern::to<Float>(wire), 0.0))) {
        n23 = Reader_varint(fr_, bytes);
        f.id = n23.Result;
      } else if (logic_and(math_compare<"==">(kern::to<Float>(field), 2.0), math_compare<"==">(kern::to<Float>(wire), 2.0))) {
        n24 = Reader_sub(fr_, bytes);
        t = n24.Result;
        while (loop(Reader_more(t).Result)) {
          n25 = Reader_varint(t, bytes);
          a = int64_low(n25.Result);
          n26 = Reader_varint(t, bytes);
          b = int64_low(n26.Result);
          list_add(f.tags, struct_make<Int_Int_pair>(a, b));
        }
      } else if (logic_and(math_compare<"==">(kern::to<Float>(field), 3.0), math_compare<"==">(kern::to<Float>(wire), 0.0))) {
        n27 = Reader_varint(fr_, bytes);
        f.type = enum_from_index<mvt_GeomType>(int64_to_int(n27.Result)).value;
      } else if (logic_and(math_compare<"==">(kern::to<Float>(field), 4.0), math_compare<"==">(kern::to<Float>(wire), 2.0))) {
        n28 = Reader_sub(fr_, bytes);
        geometry = n28.Result;
        has_geometry = true;
      } else {
        Reader_skip(fr_, bytes, wire);
      }
    }
    if (logic_not(fr_.ok)) {
      return {false};
    }
    if (has_geometry) {
      read_geometry(f.parts, std::move(bytes), geometry, f.type);
    }
    return {true};
  }

  void put_varint(List<Int>& o, Int64 v = Int64{0x0}) {
    Int64 v_ = Int64{0x0};
    v_ = v;
    while (loop(logic_not(int64_less_unsigned(v_, Int64{0x80})))) {
      list_add(o, kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::band(x, 255.0); }, kern::to<Float>(int64_low(int64_or(v_, Int64{0x80}))))));
      v_ = int64_shift_right_unsigned(v_, 7);
    }
    list_add(o, kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::band(x, 255.0); }, kern::to<Float>(int64_low(v_)))));
  }

  void put_key(List<Int>& o, Int field = 0, Int wire = 0) {
    put_varint(o, int64_or(int64_shift_left(int64_of_int(field), 3), int64_of_int(wire)));
  }

  /// a length-delimited field: bytes (a packed field, a message) or a text (a name, a key, a value)
  void put_bytes(List<Int>& o, const List<Int>& data, Int field = 0) {
    Int place = 0;
    put_key(o, field, 2);
    put_varint(o, int64_of_int(list_length<Int>(data)));
    place = list_length<Int>(o);
    list_insert_list(o, place, data);
  }

  struct zzResult { Int64 Result = Int64{0x0}; };
  KERN_PURE zzResult zz(Int64 v = Int64{0x0}) {
    return {int64_xor(int64_shift_left(v, 1), int64_shift_right(v, 63))};
  }

  void Writer_begin_layer(mvt_Writer& self, const Text& name, Int extent = 0) {
    self.layer_ = struct_make<mvt_Writer_PendingLayer>("", 4096, List<Text>{}, List<mvt_Value>{}, List<Int_row>{});
    self.layer_.name = name;
    self.layer_.extent = extent;
  }

  void Writer_feature(mvt_Writer& self, const List<Vector_row>& parts, const List<Text_mvt_Value_pair>& tags, mvt_GeomType type = mvt_GeomType::unknown, Int64 id = Int64{0x0}) {
    List<Int> f = List<Int>{};
    List<Int> t = List<Int>{};
    Int ki = 0;
    Int vi = 0;
    Bool stop = false;
    List<Int> g = List<Int>{};
    Int64 x = Int64{0x0};
    Int64 y = Int64{0x0};
    Bool skip = false;
    Int64 px = Int64{0x0};
    Int64 py = Int64{0x0};
    Int64 qx = Int64{0x0};
    Int64 qy = Int64{0x0};
    f = List<Int>{};
    if (logic_not(int64_equal(id, Int64{0x0}))) {
      put_key(f, 1, 0);
      put_varint(f, id);
    }
    t = List<Int>{};
    for (auto it32 = flow_for(n32, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Text_mvt_Value_pair>(tags))))); it32.next();) {
      ki = 0;
      vi = 0;
      while (loop(logic_and(math_compare<">">(kern::formula([](Float ki, Float a) -> Float { return (ki < 0.0) != (a < 0.0) ? a < 0.0 : ki < a; }, kern::to<Float>(ki), kern::to<Float>(list_length<Text>(self.layer_.keys))), 0.5), logic_not(text_equal(self.layer_.keys[ki], tags[n32.index].first))))) {
        ki += 1;
      }
      if (math_compare<"==">(kern::to<Float>(ki), kern::to<Float>(list_length<Text>(self.layer_.keys)))) {
        list_add(self.layer_.keys, tags[n32.index].first);
      }
      stop = false;
      while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float vi, Float a) -> Float { return (vi < 0.0) != (a < 0.0) ? a < 0.0 : vi < a; }, kern::to<Float>(vi), kern::to<Float>(list_length<mvt_Value>(self.layer_.values))), 0.5)))) {
        if (logic_and(logic_and(logic_and(enum_equal<mvt_Value_Kind>(self.layer_.values[vi].kind, tags[n32.index].second.kind), text_equal(self.layer_.values[vi].s, tags[n32.index].second.s)), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return a == a_2; }, self.layer_.values[vi].n, tags[n32.index].second.n), 0.5)), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(self.layer_.values[vi].b))), kern::to<Float>(value_int(kern::to<Int>(tags[n32.index].second.b)))))) {
          stop = true;
        }
        if (logic_not(stop)) {
          vi += 1;
        }
      }
      if (math_compare<"==">(kern::to<Float>(vi), kern::to<Float>(list_length<mvt_Value>(self.layer_.values)))) {
        list_add(self.layer_.values, tags[n32.index].second);
      }
      put_varint(t, int64_of_int(ki));
      put_varint(t, int64_of_int(vi));
    }
    put_bytes(f, t, 2);
    put_key(f, 3, 0);
    put_varint(f, int64_of_int(enum_index<mvt_GeomType>(type)));
    g = List<Int>{};
    x = Int64{0x0};
    y = Int64{0x0};
    for (auto it33 = flow_for(n33, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(parts))))); it33.next();) {
      skip = false;
      if (math_compare<"==">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(parts[n33.index]).items)), 0.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        put_varint(g, Int64{0x9});
        px = int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::round(x); }, vector_break(parts[n33.index].items[0]).x)));
        py = int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::round(y); }, vector_break(parts[n33.index].items[0]).y)));
        put_varint(g, zz(int64_sub(px, x)).Result);
        put_varint(g, zz(int64_sub(py, y)).Result);
        x = px;
        y = py;
        if (math_compare<">">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(parts[n33.index]).items)), 1.0)) {
          put_varint(g, int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(parts[n33.index]).items))))), 3), Int64{0x2}));
          for (auto it34 = flow_for(n34, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(parts[n33.index]).items))))); it34.next();) {
            qx = int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::round(x); }, vector_break(parts[n33.index].items[n34.index]).x)));
            qy = int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::round(y); }, vector_break(parts[n33.index].items[n34.index]).y)));
            put_varint(g, zz(int64_sub(qx, x)).Result);
            put_varint(g, zz(int64_sub(qy, y)).Result);
            x = qx;
            y = qy;
          }
        }
        if (enum_equal<mvt_GeomType>(type, mvt_GeomType::polygon)) {
          put_varint(g, Int64{0xf});
        }
      }
    }
    put_bytes(f, g, 4);
    list_add(self.layer_.features, struct_make<Int_row>(f));
  }

  // ---- what nodes give and keep ----
  Reader_varintResult n10;
  Reader_varintResult n13;
  Reader_varintResult n15;
  Reader_varintResult n16;
  Reader_varintResult n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  Reader_varintResult n22;
  Reader_varintResult n23;
  Reader_subResult n24;
  Reader_varintResult n25;
  Reader_varintResult n26;
  Reader_varintResult n27;
  Reader_subResult n28;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct : kern::Node<34> { Int index = 0; } n34;
};

KERN_LAYOUT(mvt_cpp) {
  node(106, 0, 4600);
  node(107, 288, 4656);
  node(108, 240, 4808);
  node(109, 240, 4912);
  node(110, 240, 5016);
  node(111, 240, 5120);
  node(112, 240, 5224);
  node(113, 240, 5328);
  node(114, 0, 5608);
  node(115, 288, 5664);
  node(116, 576, 5720);
  node(117, 864, 5776);
  node(118, 816, 5928);
  node(119, 816, 6032);
  node(120, 816, 6136);
  node(121, 816, 6240);
  node(122, 816, 6344);
  node(123, 816, 6448);
  node(124, 816, 6552);
  node(125, 1152, 5832);
  node(126, 1104, 5984);
  node(127, 1104, 6088);
  node(128, 1440, 5888);
  node(129, 1392, 6040);
  node(130, 1392, 6144);
  node(131, 1728, 5944);
  node(132, 1680, 6096);
  node(133, 1680, 6200);
  node(134, 1680, 6304);
  node(135, 1680, 6408);
  node(136, 1680, 6512);
  node(137, 1680, 6616);
  node(138, 1680, 6720);
  node(139, 2016, 6000);
  node(140, 1968, 6152);
  node(141, 1968, 6256);
  node(142, 1968, 6360);
  node(143, 1968, 6464);
  node(144, 2304, 6056);
  node(145, 2256, 6208);
  node(146, 2592, 6112);
  node(147, 2880, 6168);
  node(148, 3168, 6224);
  node(149, 0, 7000);
  node(10, 288, 7056);
  node(150, 576, 7112);
  node(151, 864, 7168);
  node(152, 816, 7320);
  node(153, 816, 7424);
  node(154, 816, 7528);
  node(155, 816, 7632);
  node(156, 816, 7736);
  node(157, 816, 7840);
  node(158, 816, 7944);
  node(159, 816, 8048);
  node(160, 816, 8152);
  node(161, 1152, 7224);
  node(162, 1440, 7280);
  node(163, 1392, 7432);
  node(164, 1392, 7536);
  node(165, 1392, 7640);
  node(166, 1728, 7336);
  node(167, 1680, 7488);
  node(168, 1680, 7592);
  node(169, 1680, 7696);
  node(170, 1680, 7800);
  node(171, 1680, 7904);
  node(172, 1680, 8008);
  node(173, 1680, 8112);
  node(174, 2016, 7392);
  node(175, 1968, 7544);
  node(176, 1968, 7648);
  node(177, 1968, 7752);
  node(178, 1968, 7856);
  node(179, 1968, 7960);
  node(180, 2304, 7448);
  node(181, 2256, 7600);
  node(182, 0, 8432);
  node(183, 288, 8488);
  node(184, 576, 8544);
  node(185, 528, 8696);
  node(186, 528, 8800);
  node(12, 864, 8600);
  node(187, 1152, 8656);
  node(188, 1104, 8808);
  node(189, 1104, 8912);
  node(190, 1440, 8712);
  node(191, 1392, 8864);
  node(192, 1392, 8968);
  node(193, 1728, 8768);
  node(194, 1680, 8920);
  node(195, 1680, 9024);
  node(13, 2016, 8824);
  node(196, 2304, 8880);
  node(197, 2592, 8936);
  node(198, 2544, 9088);
  node(199, 2544, 9192);
  node(200, 2544, 9296);
  node(201, 2544, 9400);
  node(202, 2544, 9504);
  node(203, 2544, 9608);
  node(204, 2880, 8992);
  node(205, 3168, 9048);
  node(206, 3120, 9200);
  node(207, 3120, 9304);
  node(208, 3120, 9408);
  node(209, 3120, 9512);
  node(210, 3120, 9616);
  node(211, 3456, 9104);
  node(212, 3408, 9256);
  node(213, 3408, 9360);
  node(214, 3744, 9160);
  node(215, 3696, 9312);
  node(216, 3696, 9416);
  node(217, 4032, 9216);
  node(218, 4320, 9272);
  node(219, 4272, 9424);
  node(220, 4272, 9528);
  node(221, 4272, 9632);
  node(222, 4272, 9736);
  node(223, 4608, 9328);
  node(224, 0, 10016);
  node(225, 288, 10072);
  node(226, 240, 10224);
  node(227, 240, 10328);
  node(228, 240, 10432);
  node(229, 240, 10536);
  node(230, 240, 10640);
  node(231, 240, 10744);
  node(232, 240, 10848);
  node(233, 240, 10952);
  node(234, 0, 11232);
  node(235, 288, 11288);
  node(236, 240, 11440);
  node(237, 240, 11544);
  node(238, 240, 11648);
  node(239, 240, 11752);
  node(240, 240, 11856);
  node(241, 240, 11960);
  node(242, 576, 11344);
  node(243, 864, 11400);
  node(244, 1152, 11456);
  node(245, 1104, 11608);
  node(246, 1104, 11712);
  node(247, 1104, 11816);
  node(248, 1104, 11920);
  node(249, 1104, 12024);
  node(250, 1104, 12128);
  node(251, 1104, 12232);
  node(252, 1104, 12336);
  node(253, 1104, 12440);
  node(254, 1104, 12544);
  node(255, 1440, 11512);
  node(256, 1392, 11664);
  node(257, 1392, 11768);
  node(258, 1728, 11568);
  node(259, 1680, 11720);
  node(260, 0, 12824);
  node(261, 288, 12880);
  node(262, 240, 13032);
  node(263, 240, 13136);
  node(264, 240, 13240);
  node(265, 240, 13344);
  node(266, 240, 13448);
  node(267, 240, 13552);
  node(268, 576, 12936);
  node(269, 864, 12992);
  node(270, 1152, 13048);
  node(271, 1104, 13200);
  node(272, 1104, 13304);
  node(273, 1104, 13408);
  node(274, 1440, 13104);
  node(275, 1392, 13256);
  node(276, 1392, 13360);
  node(277, 1728, 13160);
  node(278, 1680, 13312);
  node(11, 0, 0);
  node(279, 288, 56);
  node(280, 240, 208);
  node(281, 240, 312);
  node(282, 240, 416);
  node(283, 240, 520);
  node(284, 0, 13832);
  node(285, 288, 13888);
  node(286, 576, 13944);
  node(287, 864, 14000);
  node(288, 1152, 14056);
  node(289, 1440, 14112);
  node(290, 1392, 14264);
  node(15, 1728, 14168);
  node(291, 2016, 14224);
  node(292, 2304, 14280);
  node(293, 2256, 14432);
  node(294, 2256, 14536);
  node(295, 2256, 14640);
  node(296, 2592, 14336);
  node(297, 2544, 14488);
  node(298, 2544, 14592);
  node(299, 2544, 14696);
  node(300, 2880, 14392);
  node(301, 2832, 14544);
  node(302, 2832, 14648);
  node(303, 2832, 14752);
  node(304, 2832, 14856);
  node(305, 2832, 14960);
  node(306, 3168, 14448);
  node(307, 3456, 14504);
  node(308, 3408, 14656);
  node(309, 3408, 14760);
  node(310, 3408, 14864);
  node(311, 3408, 14968);
  node(312, 3408, 15072);
  node(16, 3744, 14560);
  node(313, 4032, 14616);
  node(314, 3984, 14768);
  node(315, 3984, 14872);
  node(316, 3984, 14976);
  node(17, 4320, 14672);
  node(317, 4608, 14728);
  node(318, 4560, 14880);
  node(319, 4560, 14984);
  node(320, 4560, 15088);
  node(321, 4896, 14784);
  node(322, 4848, 14936);
  node(323, 4848, 15040);
  node(324, 4848, 15144);
  node(325, 4848, 15248);
  node(326, 4848, 15352);
  node(327, 5184, 14840);
  node(328, 5472, 14896);
  node(329, 5760, 14952);
  node(330, 5712, 15104);
  node(331, 5712, 15208);
  node(332, 5712, 15312);
  node(333, 5712, 15416);
  node(334, 5712, 15520);
  node(335, 5712, 15624);
  node(336, 5712, 15728);
  node(337, 5712, 15832);
  node(338, 6048, 15008);
  node(339, 6336, 15064);
  node(340, 6288, 15216);
  node(341, 6288, 15320);
  node(342, 6624, 15120);
  node(343, 0, 16112);
  node(18, 288, 16168);
  node(344, 240, 16320);
  node(345, 240, 16424);
  node(346, 240, 16528);
  node(347, 576, 16224);
  node(348, 528, 16376);
  node(349, 528, 16480);
  node(350, 528, 16584);
  node(351, 528, 16688);
  node(352, 528, 16792);
  node(353, 528, 16896);
  node(354, 528, 17000);
  node(355, 528, 17104);
  node(356, 528, 17208);
  node(357, 528, 17312);
  node(358, 528, 17416);
  node(359, 528, 17520);
  node(360, 528, 17624);
  node(361, 528, 17728);
  node(362, 528, 17832);
  node(363, 864, 16280);
  node(364, 816, 16432);
  node(365, 816, 16536);
  node(366, 1152, 16336);
  node(367, 0, 18112);
  node(368, 288, 18168);
  node(369, 240, 18320);
  node(370, 576, 18224);
  node(371, 528, 18376);
  node(372, 528, 18480);
  node(373, 528, 18584);
  node(374, 528, 18688);
  node(375, 528, 18792);
  node(376, 528, 18896);
  node(377, 528, 19000);
  node(378, 528, 19104);
  node(379, 528, 19208);
  node(380, 0, 19488);
  node(381, 288, 19544);
  node(14, 160, 240);
  node(382, 576, 19600);
  node(383, 528, 19752);
  node(384, 528, 19856);
  node(385, 528, 19960);
  node(386, 528, 20064);
  node(387, 528, 20168);
  node(388, 528, 20272);
  node(389, 528, 20376);
  node(390, 0, 20656);
  node(19, 288, 20712);
  node(391, 240, 20864);
  node(392, 240, 20968);
  node(393, 240, 21072);
  node(394, 576, 20768);
  node(395, 528, 20920);
  node(396, 528, 21024);
  node(397, 864, 20824);
  node(398, 816, 20976);
  node(399, 1152, 20880);
  node(400, 0, 21352);
  node(401, 288, 21408);
  node(402, 576, 21464);
  node(21, 440, 800);
  node(403, 528, 21616);
  node(404, 864, 21520);
  node(405, 1152, 21576);
  node(406, 1104, 21728);
  node(407, 1104, 21832);
  node(408, 1440, 21632);
  node(409, 1392, 21784);
  node(29, 1280, 890);
  node(30, 1280, 980);
  node(31, 1280, 1070);
  node(410, 1728, 21688);
  node(411, 1680, 21840);
  node(412, 1680, 21944);
  node(35, 1560, 980);
  node(36, 1560, 1070);
  node(37, 1560, 1160);
  node(38, 1560, 1250);
  node(39, 1560, 1340);
  node(40, 1560, 1430);
  node(41, 1560, 1520);
  node(42, 1560, 1610);
  node(43, 1560, 1700);
  node(44, 1560, 1790);
  node(45, 1560, 1880);
  node(46, 1960, 650);
  node(47, 1840, 800);
  node(48, 2240, 650);
  node(49, 2520, 650);
  node(50, 2400, 800);
  node(51, 2800, 650);
  node(52, 2680, 800);
  node(53, 2680, 890);
  node(54, 0, 2020);
  node(55, 280, 2020);
  node(56, 160, 2170);
  node(57, 160, 2260);
  node(58, 160, 2350);
  node(59, 560, 2020);
  node(20, 848, 2080);
  node(60, 720, 2170);
  node(61, 720, 2260);
  node(62, 720, 2350);
  node(63, 1120, 2020);
  node(64, 1000, 2170);
  node(65, 1000, 2260);
  node(66, 1000, 2350);
  node(67, 1400, 2020);
  node(68, 1680, 2020);
  node(413, 0, 22224);
  node(414, 288, 22280);
  node(415, 576, 22336);
  node(416, 864, 22392);
  node(417, 1152, 22448);
  node(418, 1104, 22600);
  node(22, 1440, 22504);
  node(419, 1728, 22560);
  node(420, 2016, 22616);
  node(421, 1968, 22768);
  node(422, 1968, 22872);
  node(423, 1968, 22976);
  node(424, 2304, 22672);
  node(425, 2256, 22824);
  node(426, 2256, 22928);
  node(427, 2256, 23032);
  node(428, 2592, 22728);
  node(429, 2544, 22880);
  node(430, 2544, 22984);
  node(431, 2544, 23088);
  node(432, 2544, 23192);
  node(433, 2544, 23296);
  node(23, 2880, 22784);
  node(434, 3168, 22840);
  node(435, 3456, 22896);
  node(436, 3408, 23048);
  node(437, 3408, 23152);
  node(438, 3408, 23256);
  node(439, 3408, 23360);
  node(440, 3408, 23464);
  node(24, 3744, 22952);
  node(441, 4032, 23008);
  node(442, 4320, 23064);
  node(443, 4272, 23216);
  node(25, 4608, 23120);
  node(444, 4896, 23176);
  node(445, 4848, 23328);
  node(26, 5184, 23232);
  node(446, 5472, 23288);
  node(447, 5424, 23440);
  node(448, 5760, 23344);
  node(449, 5712, 23496);
  node(450, 5712, 23600);
  node(451, 5712, 23704);
  node(452, 6048, 23400);
  node(453, 6000, 23552);
  node(454, 6000, 23656);
  node(455, 6000, 23760);
  node(456, 6000, 23864);
  node(457, 6000, 23968);
  node(27, 6336, 23456);
  node(458, 6624, 23512);
  node(459, 6576, 23664);
  node(460, 6576, 23768);
  node(461, 6912, 23568);
  node(462, 6864, 23720);
  node(463, 6864, 23824);
  node(464, 6864, 23928);
  node(465, 6864, 24032);
  node(466, 6864, 24136);
  node(28, 7200, 23624);
  node(467, 7488, 23680);
  node(468, 7776, 23736);
  node(469, 8064, 23792);
  node(470, 8016, 23944);
  node(471, 8352, 23848);
  node(472, 8304, 24000);
  node(473, 8304, 24104);
  node(474, 8640, 23904);
  node(475, 8928, 23960);
  node(476, 8880, 24112);
  node(477, 9216, 24016);
  node(478, 9168, 24168);
  node(479, 9168, 24272);
  node(480, 9504, 24072);
  node(69, 0, 2490);
  node(70, 280, 2490);
  node(71, 560, 2490);
  node(72, 440, 2640);
  node(481, 512, 2640);
  node(74, 440, 2820);
  node(76, 840, 2490);
  node(77, 720, 2640);
  node(482, 792, 2640);
  node(483, 792, 2744);
  node(80, 720, 2910);
  node(82, 1120, 2490);
  node(484, 1072, 2640);
  node(84, 1000, 2730);
  node(85, 1400, 2490);
  node(86, 1280, 2640);
  node(485, 1352, 2640);
  node(88, 1280, 2820);
  node(89, 0, 3140);
  node(90, 280, 3140);
  node(486, 232, 3296);
  node(487, 232, 3400);
  node(488, 232, 3504);
  node(489, 232, 3608);
  node(490, 0, 24552);
  node(491, 288, 24608);
  node(492, 576, 24664);
  node(493, 528, 24816);
  node(494, 528, 24920);
  node(495, 528, 25024);
  node(496, 864, 24720);
  node(497, 816, 24872);
  node(498, 816, 24976);
  node(499, 1152, 24776);
  node(500, 1104, 24928);
  node(501, 1104, 25032);
  node(95, 0, 3700);
  node(96, 280, 3700);
  node(502, 232, 3856);
  node(503, 232, 3960);
  node(504, 232, 4064);
  node(100, 0, 4170);
  node(101, 280, 4170);
  node(102, 160, 4320);
  node(103, 560, 4170);
  node(104, 440, 4320);
  node(105, 840, 4170);
  node(505, 0, 25312);
  node(506, 288, 25368);
  node(507, 576, 25424);
  node(508, 528, 25576);
  node(509, 528, 25680);
  node(510, 864, 25480);
  node(511, 1152, 25536);
  node(512, 1440, 25592);
  node(32, 1728, 25648);
  node(513, 1680, 25800);
  node(514, 1680, 25904);
  node(515, 1680, 26008);
  node(516, 2016, 25704);
  node(517, 2304, 25760);
  node(518, 2592, 25816);
  node(519, 2544, 25968);
  node(520, 2544, 26072);
  node(521, 2544, 26176);
  node(522, 2544, 26280);
  node(523, 2544, 26384);
  node(524, 2544, 26488);
  node(525, 2544, 26592);
  node(526, 2544, 26696);
  node(527, 2544, 26800);
  node(528, 2544, 26904);
  node(529, 2544, 27008);
  node(530, 2880, 25872);
  node(531, 3168, 25928);
  node(532, 3120, 26080);
  node(533, 3120, 26184);
  node(534, 3120, 26288);
  node(535, 3120, 26392);
  node(536, 3456, 25984);
  node(537, 3408, 26136);
  node(538, 3744, 26040);
  node(539, 4032, 26096);
  node(540, 3984, 26248);
  node(541, 3984, 26352);
  node(542, 3984, 26456);
  node(543, 3984, 26560);
  node(544, 3984, 26664);
  node(545, 3984, 26768);
  node(546, 3984, 26872);
  node(547, 3984, 26976);
  node(548, 4320, 26152);
  node(549, 4272, 26304);
  node(550, 4272, 26408);
  node(551, 4272, 26512);
  node(552, 4272, 26616);
  node(553, 4272, 26720);
  node(554, 4272, 26824);
  node(555, 4272, 26928);
  node(556, 4272, 27032);
  node(557, 4272, 27136);
  node(558, 4272, 27240);
  node(559, 4272, 27344);
  node(560, 4272, 27448);
  node(561, 4272, 27552);
  node(562, 4272, 27656);
  node(563, 4272, 27760);
  node(564, 4272, 27864);
  node(565, 4272, 27968);
  node(566, 4272, 28072);
  node(567, 4272, 28176);
  node(568, 4272, 28280);
  node(569, 4272, 28384);
  node(570, 4272, 28488);
  node(571, 4608, 26208);
  node(572, 4896, 26264);
  node(573, 4848, 26416);
  node(574, 4848, 26520);
  node(575, 5184, 26320);
  node(576, 5472, 26376);
  node(577, 5424, 26528);
  node(578, 5424, 26632);
  node(579, 5424, 26736);
  node(580, 5424, 26840);
  node(581, 5760, 26432);
  node(582, 5712, 26584);
  node(583, 6048, 26488);
  node(584, 6000, 26640);
  node(585, 6000, 26744);
  node(586, 6336, 26544);
  node(587, 6288, 26696);
  node(588, 6288, 26800);
  node(589, 6624, 26600);
  node(590, 6912, 26656);
  node(591, 7200, 26712);
  node(592, 7152, 26864);
  node(593, 7152, 26968);
  node(594, 7488, 26768);
  node(595, 7776, 26824);
  node(596, 8064, 26880);
  node(33, 8352, 26936);
  node(597, 8304, 27088);
  node(598, 8304, 27192);
  node(599, 8304, 27296);
  node(600, 8640, 26992);
  node(601, 8928, 27048);
  node(602, 8880, 27200);
  node(603, 8880, 27304);
  node(604, 8880, 27408);
  node(605, 8880, 27512);
  node(606, 9216, 27104);
  node(607, 9504, 27160);
  node(608, 9456, 27312);
  node(609, 9456, 27416);
  node(610, 9792, 27216);
  node(611, 10080, 27272);
  node(612, 10032, 27424);
  node(613, 10032, 27528);
  node(614, 10032, 27632);
  node(615, 10032, 27736);
  node(616, 10368, 27328);
  node(617, 10320, 27480);
  node(618, 10320, 27584);
  node(619, 10320, 27688);
  node(620, 10320, 27792);
  node(621, 10656, 27384);
  node(622, 10608, 27536);
  node(623, 10608, 27640);
  node(624, 10608, 27744);
  node(625, 10608, 27848);
  node(626, 10944, 27440);
  node(627, 10896, 27592);
  node(628, 10896, 27696);
  node(629, 10896, 27800);
  node(630, 10896, 27904);
  node(631, 11232, 27496);
  node(632, 11184, 27648);
  node(633, 11520, 27552);
  node(634, 11472, 27704);
  node(635, 11808, 27608);
  node(636, 11760, 27760);
  node(637, 11760, 27864);
  node(638, 11760, 27968);
  node(639, 11760, 28072);
  node(640, 12096, 27664);
  node(641, 12048, 27816);
  node(642, 12048, 27920);
  node(643, 12048, 28024);
  node(644, 12048, 28128);
  node(645, 12048, 28232);
  node(646, 12048, 28336);
  node(647, 12048, 28440);
  node(34, 12384, 27720);
  node(648, 12336, 27872);
  node(649, 12336, 27976);
  node(650, 12336, 28080);
  node(651, 12336, 28184);
  node(652, 12672, 27776);
  node(653, 12624, 27928);
  node(654, 12624, 28032);
  node(655, 12624, 28136);
  node(656, 12624, 28240);
  node(657, 12960, 27832);
  node(658, 12912, 27984);
  node(659, 12912, 28088);
  node(660, 12912, 28192);
  node(661, 12912, 28296);
  node(662, 13248, 27888);
  node(663, 13200, 28040);
  node(664, 13200, 28144);
  node(665, 13200, 28248);
  node(666, 13200, 28352);
  node(667, 13536, 27944);
  node(668, 13488, 28096);
  node(669, 13488, 28200);
  node(670, 13488, 28304);
  node(671, 13488, 28408);
  node(672, 13824, 28000);
  node(673, 13776, 28152);
  node(674, 14112, 28056);
  node(675, 14064, 28208);
  node(676, 14400, 28112);
  node(677, 14352, 28264);
  node(678, 14688, 28168);
  node(679, 14976, 28224);
  node(680, 15264, 28280);
  node(681, 15216, 28432);
  node(682, 15216, 28536);
  next_id(683);
}


}  // namespace kern_blueprint_02f7c0a1316e6ec3

// ---------------------------------------------------------------------------
// The Blueprint of actor "road_graph_cpp"
namespace kern_blueprint_280419937f3ad3b5 {

using city_stream = ::kern_blueprint_0265b76c2a3d1e83::city_stream;
using road_graph = ::kern_blueprint_3a4ae1d57fed8576::road_graph;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct road_graph_cpp : virtual kern::Library, virtual city_stream, virtual road_graph {
  // ---- variables ----
  /// vertices closer than this (same grid cell or a neighbour) merge
  KERN_CONST Float kCell = 3.5;

  // ---- functions ----
  struct cell_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE cell_keyResult cell_key(Int x = 0, Int y = 0) {
    return {int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(x); }, kern::to<Float>(x)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(y); }, kern::to<Float>(y)))))};
  }

  struct distResult { Float Result = 0.0; };
  KERN_PURE distResult dist(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt((x - x_2) * (x - x_2) + (y - y_2) * (y - y_2)); }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  struct lane_signatureResult { Int64 Result = Int64{0x0}; };
  KERN_PURE lane_signatureResult lane_signature(const List<Lane>& lanes) {
    Int64 h = Int64{0x0};
    h = Int64{0x14650fb0739d0383};
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Lane>(lanes))))); it10.next();) {
      h = int64_xor(h, int64_or(int64_shift_left(int64_of_int(n10.index), 1), int64_of_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a != 0.0 ? 1.0 : 0.0; }, kern::to<Float>(lanes[n10.index].alive))))));
      h = int64_mul(h, Int64{0x100000001b3});
    }
    return {h};
  }

  struct RoadGraph_node_atResult { Int Result = 0; };
  RoadGraph_node_atResult RoadGraph_node_at(RoadGraph& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    Int cx = 0;
    Int cy = 0;
    Int it = 0;
    Int id = 0;
    Int put = 0;
    Int64 key = Int64{0x0};
    cx = kern::to<Int>(kern::formula([](Float x, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kCell))); }, vector_break(p).x, kCell));
    cy = kern::to<Int>(kern::formula([](Float y, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kCell))); }, vector_break(p).y, kCell));
    for (auto it11 = flow_for(n11, -1, 1); it11.next();) {
      for (auto it12 = flow_for(n12, -1, 1); it12.next();) {
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Int>(self.cells_, cell_key(kern::to<Int>(kern::formula([](Float cx, Float index) -> Float { return cx + index; }, kern::to<Float>(cx), kern::to<Float>(n11.index))), kern::to<Int>(kern::formula([](Float cy, Float index) -> Float { return cy + index; }, kern::to<Float>(cy), kern::to<Float>(n12.index)))).Result).index), kern::to<Float>(map_length<Int64, Int>(self.cells_))));
        if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Int>(self.cells_)))), math_compare<"<">(dist(self.nodes_[self.cells_[it]].p, p).Result, kCell))) {
          return {self.cells_[it]};
        }
      }
    }
    id = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<RoadGraph_Node>(self.nodes_))));
    list_add(self.nodes_, struct_make<RoadGraph_Node>(p, List<Int_Float_pair>{}));
    put = id;
    key = cell_key(cx, cy).Result;
    map_add(n13, self.cells_, key);
    self.cells_[n13.index] = put;
    return {id};
  }

  void RoadGraph_rebuild_link(RoadGraph& self, Float len = 0.0, Int a = 0, Int b = 0) {
    Int at = 0;
    at = a;
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(self.nodes_[at].edges))))); it18.next();) {
      if (math_compare<"==">(kern::to<Float>(self.nodes_[at].edges[n18.index].first), kern::to<Float>(b))) {
        return;
      }
    }
    list_add(self.nodes_[a].edges, struct_make<Int_Float_pair>(b, len));
  }

  void RoadGraph_rebuild(RoadGraph& self, const List<Lane>& lanes) {
    Bool skip = false;
    Int prev = 0;
    Bool skip_2 = false;
    Int cur = 0;
    Float len = 0.0;
    list_clear(self.nodes_);
    map_clear(self.cells_);
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(lanes))))); it14.next();) {
      skip = false;
      if (logic_or(logic_not(lanes[n14.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(lanes[n14.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        n15 = RoadGraph_node_at(self, lanes[n14.index].points[0]);
        prev = n15.Result;
        for (auto it16 = flow_for(n16, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(lanes[n14.index].points))))); it16.next();) {
          skip_2 = false;
          n17 = RoadGraph_node_at(self, lanes[n14.index].points[n16.index]);
          cur = n17.Result;
          if (math_compare<"==">(kern::to<Float>(cur), kern::to<Float>(prev))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            len = dist(self.nodes_[prev].p, self.nodes_[cur].p).Result;
            RoadGraph_rebuild_link(self, len, prev, cur);
            RoadGraph_rebuild_link(self, len, cur, prev);
            prev = cur;
          }
        }
      }
    }
    self.built_from = lane_signature(lanes).Result;
  }

  struct RoadGraph_nearestResult { Int Result = 0; };
  KERN_PURE RoadGraph_nearestResult RoadGraph_nearest(const RoadGraph& self, Vector p = Vector{0.0, 0.0, 0.0}, Float max_m = 0.0) {
    Int best = 0;
    Float bd = 0.0;
    Float d = 0.0;
    best = -1;
    bd = max_m;
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<RoadGraph_Node>(self.nodes_))))); it19.next();) {
      d = dist(self.nodes_[n19.index].p, p).Result;
      if (math_compare<"<">(d, bd)) {
        bd = d;
        best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n19.index)));
      }
    }
    return {best};
  }

  struct RoadGraph_pursuit_pointResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE RoadGraph_pursuit_pointResult RoadGraph_pursuit_point(const List<Vector>& path, Vector p = Vector{0.0, 0.0, 0.0}, Float look = 0.0) {
    Int bi = 0;
    Float bt = 0.0;
    Float bd = 0.0;
    Int i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    Float t = 0.0;
    Float dd = 0.0;
    Float left = 0.0;
    Float t_2 = 0.0;
    Int i_2 = 0;
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Vector b_2 = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float rest = 0.0;
    if (math_compare<"==">(kern::to<Float>(list_length<Vector>(path)), 0.0)) {
      return {p};
    }
    if (math_compare<"<">(kern::to<Float>(list_length<Vector>(path)), 2.0)) {
      return {path[0]};
    }
    bi = 0;
    bt = 0.0;
    bd = 1e+30;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return i + 1.0; }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(path))), 0.5))) {
      a = path[i];
      b = path[kern::to<Int>(kern::formula([](Float i) -> Float { return i + 1.0; }, kern::to<Float>(i)))];
      d = vector_sub(b, a);
      l2 = kern::formula([](Float x, Float y) -> Float { return kern::math::max(1e-06, x * x + y * y); }, vector_break(d).x, vector_break(d).y);
      t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : hi < v ? hi : v; }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((x - x_2) * x_3 + (y - y_2) * y_3, l2); }, vector_break(p).x, vector_break(a).x, vector_break(d).x, vector_break(p).y, vector_break(a).y, vector_break(d).y, l2), 0.0, 1.0));
      dd = dist(p, vector_add(a, vector_scale(d, t))).Result;
      if (math_compare<"<">(dd, bd)) {
        bd = dd;
        bi = i;
        bt = t;
      }
      i += 1;
    }
    left = look;
    t_2 = bt;
    i_2 = bi;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return i_2 + 1.0; }, kern::to<Float>(i_2))))), kern::to<Float>(list_length<Vector>(path))), 0.5))) {
      a_2 = path[i_2];
      b_2 = path[kern::to<Int>(kern::formula([](Float i_2) -> Float { return i_2 + 1.0; }, kern::to<Float>(i_2)))];
      l = kern::formula([](Float the_result) -> Float { return kern::math::max(1e-06, the_result); }, dist(a_2, b_2).Result);
      rest = kern::formula([](Float t_2, Float l) -> Float { return (1.0 - t_2) * l; }, t_2, l);
      if (math_compare<">=">(rest, left)) {
        return {vector_add(a_2, vector_scale(vector_sub(b_2, a_2), kern::formula([](Float t_2, Float left, Float l) -> Float { return t_2 + kern::math::div(left, l); }, t_2, left, l)))};
      }
      left = kern::formula([](Float left, Float rest) -> Float { return left - rest; }, left, rest);
      t_2 = 0.0;
      i_2 += 1;
    }
    return {path[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(path))))]};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { Bool added = false; Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  RoadGraph_node_atResult n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  RoadGraph_node_atResult n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
};

KERN_LAYOUT(road_graph_cpp) {
  node(192, 0, 3728);
  node(193, 288, 3784);
  node(339, 240, 3936);
  node(340, 240, 4040);
  node(341, 240, 4144);
  node(198, 240, 4352);
  node(342, 240, 4248);
  node(201, 240, 4664);
  node(202, 0, 4944);
  node(203, 288, 5000);
  node(204, 240, 5152);
  node(343, 240, 5152);
  node(205, 240, 5256);
  node(20, 160, 420);
  node(21, 160, 510);
  node(22, 0, 650);
  node(23, 280, 650);
  node(10, 560, 650);
  node(25, 440, 800);
  node(26, 440, 890);
  node(27, 440, 980);
  node(28, 840, 650);
  node(344, 792, 800);
  node(30, 720, 890);
  node(345, 792, 904);
  node(346, 792, 1008);
  node(347, 792, 1112);
  node(348, 792, 1216);
  node(36, 720, 1430);
  node(37, 720, 1520);
  node(38, 1120, 650);
  node(349, 1072, 800);
  node(40, 1000, 890);
  node(42, 1400, 650);
  node(43, 1280, 800);
  node(206, 0, 5536);
  node(207, 288, 5592);
  node(208, 240, 5744);
  node(209, 240, 5848);
  node(210, 240, 5952);
  node(211, 576, 5648);
  node(212, 528, 5800);
  node(213, 528, 5904);
  node(214, 528, 6008);
  node(11, 864, 5704);
  node(12, 1152, 5760);
  node(215, 1440, 5816);
  node(216, 1392, 5968);
  node(350, 1392, 5968);
  node(218, 1392, 6176);
  node(219, 1392, 6280);
  node(220, 1392, 6384);
  node(221, 1392, 6488);
  node(222, 1392, 6592);
  node(223, 1392, 6696);
  node(351, 1392, 6072);
  node(225, 1392, 6904);
  node(226, 1728, 5872);
  node(227, 1680, 6024);
  node(228, 1680, 6128);
  node(229, 1680, 6232);
  node(230, 1680, 6336);
  node(352, 1680, 6024);
  node(232, 1680, 6544);
  node(233, 1680, 6648);
  node(234, 1680, 6752);
  node(235, 1680, 6856);
  node(236, 1680, 6960);
  node(237, 1680, 7064);
  node(238, 1680, 7168);
  node(239, 2016, 5928);
  node(240, 1968, 6080);
  node(241, 1968, 6184);
  node(242, 2304, 5984);
  node(243, 2256, 6136);
  node(244, 2256, 6240);
  node(245, 2256, 6344);
  node(246, 2592, 6040);
  node(247, 2544, 6192);
  node(248, 2880, 6096);
  node(249, 2832, 6248);
  node(250, 3168, 6152);
  node(251, 3120, 6304);
  node(252, 3120, 6408);
  node(253, 3120, 6512);
  node(13, 3456, 6208);
  node(268, 4560, 6688);
  node(269, 4896, 6488);
  node(271, 4848, 6744);
  node(272, 5184, 6544);
  node(273, 5136, 6696);
  node(274, 0, 7448);
  node(275, 288, 7504);
  node(18, 848, 1720);
  node(276, 528, 7712);
  node(277, 528, 7816);
  node(278, 528, 7920);
  node(279, 528, 8024);
  node(280, 864, 7616);
  node(281, 816, 7768);
  node(282, 816, 7872);
  node(283, 816, 7976);
  node(284, 1152, 7672);
  node(285, 1440, 7728);
  node(286, 1392, 7880);
  node(287, 0, 8304);
  node(288, 288, 8360);
  node(353, 576, 8416);
  node(14, 864, 8472);
  node(290, 816, 8624);
  node(291, 816, 8728);
  node(292, 816, 8832);
  node(293, 1152, 8528);
  node(294, 1440, 8584);
  node(295, 1392, 8736);
  node(296, 1392, 8840);
  node(297, 1392, 8944);
  node(298, 1392, 9048);
  node(299, 1392, 9152);
  node(300, 1392, 9256);
  node(301, 1728, 8640);
  node(302, 2016, 8696);
  node(303, 1968, 8848);
  node(304, 1968, 8952);
  node(15, 2304, 8752);
  node(305, 2256, 8904);
  node(306, 2592, 8808);
  node(16, 2880, 8864);
  node(307, 2832, 9016);
  node(308, 2832, 9120);
  node(309, 2832, 9224);
  node(310, 3168, 8920);
  node(17, 3456, 8976);
  node(311, 3408, 9128);
  node(312, 3744, 9032);
  node(313, 4032, 9088);
  node(314, 3984, 9240);
  node(315, 3984, 9344);
  node(316, 3984, 9448);
  node(317, 4320, 9144);
  node(318, 4608, 9200);
  node(319, 4560, 9352);
  node(320, 4560, 9456);
  node(321, 4896, 9256);
  node(322, 4848, 9408);
  node(323, 4848, 9512);
  node(324, 4848, 9616);
  node(325, 4848, 9720);
  node(326, 4848, 9824);
  node(327, 5184, 9312);
  node(328, 5136, 9464);
  node(329, 5136, 9568);
  node(330, 5136, 9672);
  node(331, 5472, 9368);
  node(332, 5424, 9520);
  node(333, 5424, 9624);
  node(334, 5424, 9728);
  node(335, 5760, 9424);
  node(336, 5712, 9576);
  node(337, 6048, 9480);
  node(338, 6000, 9632);
  node(44, 0, 1660);
  node(45, 280, 1660);
  node(46, 560, 1660);
  node(19, 848, 1720);
  node(47, 720, 1810);
  node(48, 720, 1900);
  node(49, 720, 1990);
  node(50, 1120, 1660);
  node(51, 1000, 1810);
  node(52, 1000, 1900);
  node(53, 1400, 1660);
  node(54, 1280, 1810);
  node(55, 1280, 1900);
  node(56, 1280, 1990);
  node(57, 1680, 1660);
  node(58, 1560, 1810);
  node(59, 1960, 1660);
  node(60, 1840, 1810);
  node(61, 2240, 1660);
  node(62, 2120, 1810);
  node(63, 0, 2130);
  node(64, 280, 2130);
  node(65, 160, 2280);
  node(66, 160, 2370);
  node(67, 160, 2460);
  node(68, 560, 2130);
  node(69, 840, 2130);
  node(70, 720, 2280);
  node(71, 720, 2370);
  node(72, 720, 2460);
  node(73, 1120, 2130);
  node(74, 1000, 2280);
  node(75, 1400, 2130);
  node(76, 1680, 2130);
  node(77, 1960, 2130);
  node(78, 2240, 2130);
  node(79, 2520, 2130);
  node(80, 2400, 2280);
  node(81, 2400, 2370);
  node(82, 2400, 2460);
  node(83, 2400, 2550);
  node(84, 2400, 2640);
  node(85, 2400, 2730);
  node(86, 2400, 2820);
  node(87, 2800, 2130);
  node(88, 2680, 2280);
  node(89, 2680, 2370);
  node(90, 3080, 2130);
  node(91, 2960, 2280);
  node(92, 2960, 2370);
  node(93, 2960, 2460);
  node(94, 3360, 2130);
  node(95, 3240, 2280);
  node(96, 3240, 2370);
  node(97, 3240, 2460);
  node(98, 3640, 2130);
  node(99, 3520, 2280);
  node(100, 3520, 2370);
  node(101, 3520, 2460);
  node(102, 3520, 2550);
  node(103, 3520, 2640);
  node(104, 3920, 2130);
  node(105, 3800, 2280);
  node(106, 3800, 2370);
  node(107, 3800, 2460);
  node(108, 3800, 2550);
  node(109, 3800, 2640);
  node(110, 3800, 2730);
  node(111, 3800, 2820);
  node(112, 3800, 2910);
  node(113, 3800, 3000);
  node(114, 3800, 3090);
  node(115, 3800, 3180);
  node(116, 3800, 3270);
  node(117, 3800, 3360);
  node(118, 3800, 3450);
  node(119, 4200, 2130);
  node(120, 4080, 2280);
  node(121, 4080, 2370);
  node(122, 4080, 2460);
  node(123, 4080, 2550);
  node(124, 4080, 2640);
  node(125, 4080, 2730);
  node(126, 4480, 2130);
  node(127, 4360, 2280);
  node(128, 4360, 2370);
  node(129, 4360, 2460);
  node(130, 4760, 2130);
  node(131, 4640, 2280);
  node(132, 5040, 2130);
  node(133, 4920, 2280);
  node(134, 5320, 2130);
  node(135, 5200, 2280);
  node(136, 5600, 2130);
  node(137, 5880, 2130);
  node(138, 6160, 2130);
  node(139, 6040, 2280);
  node(140, 6440, 2130);
  node(141, 6320, 2280);
  node(142, 6720, 2130);
  node(143, 6600, 2280);
  node(144, 6600, 2370);
  node(145, 6600, 2460);
  node(146, 6600, 2550);
  node(147, 6600, 2640);
  node(148, 6600, 2730);
  node(149, 6600, 2820);
  node(150, 7000, 2130);
  node(151, 6880, 2280);
  node(152, 6880, 2370);
  node(153, 7280, 2130);
  node(154, 7160, 2280);
  node(155, 7160, 2370);
  node(156, 7160, 2460);
  node(157, 7560, 2130);
  node(158, 7440, 2280);
  node(159, 7440, 2370);
  node(160, 7440, 2460);
  node(161, 7440, 2550);
  node(162, 7840, 2130);
  node(163, 7720, 2280);
  node(164, 7720, 2370);
  node(165, 7720, 2460);
  node(166, 8120, 2130);
  node(167, 8000, 2280);
  node(168, 8000, 2370);
  node(169, 8000, 2460);
  node(170, 8400, 2130);
  node(171, 8280, 2280);
  node(172, 8280, 2370);
  node(173, 8280, 2460);
  node(174, 8280, 2550);
  node(175, 8280, 2640);
  node(176, 8280, 2730);
  node(177, 8280, 2820);
  node(178, 8280, 2910);
  node(179, 8280, 3000);
  node(180, 8280, 3090);
  node(181, 8680, 2130);
  node(182, 8560, 2280);
  node(183, 8560, 2370);
  node(184, 8560, 2460);
  node(185, 8960, 2130);
  node(186, 9240, 2130);
  node(187, 9520, 2130);
  node(188, 9400, 2280);
  node(189, 9400, 2370);
  node(190, 9400, 2460);
  node(191, 9400, 2550);
  next_id(354);
}


}  // namespace kern_blueprint_280419937f3ad3b5

// ---------------------------------------------------------------------------
// The Blueprint of actor "perf_cpp"
namespace kern_blueprint_c4a27d6984fae299 {

using common = ::kern_blueprint_416f27166223668d::common;
using perf = ::kern_blueprint_1b97e0faae1f6c60::perf;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct perf_cpp : virtual kern::Library, virtual common, virtual perf {
  // ---- variables ----
  KERN_CONST Int kTiers = 5;
  /// s after any change before the next decision (new level settles)
  KERN_CONST Float kSettle = 3.0;
  /// s after start / reset without decisions (loading, shader warm-up)
  KERN_CONST Float kWarmup = 3.0;
  /// The governor's limits.
  KERN_CONST Float kAverageTau = 1.0;
  /// below this (averaged) ...
  KERN_CONST Float kDownFps = 50.0;
  /// at or above this (vsync 60 Hz shows ~59-60) ...
  KERN_CONST Float kUpFps = 57.0;
  /// ... for this long: one level down
  KERN_CONST Float kDownHold = 2.0;
  /// a level that was too heavy is not tried again for 30 s ...
  KERN_CONST Float kBlockFirst = 30.0;
  /// ... doubling each time it fails again, up to 4 min
  KERN_CONST Float kBlockMax = 240.0;
  /// ... for this long: one level up
  KERN_CONST Float kUpHold = 8.0;

  // ---- functions ----
  struct slot_nameResult { Text Result = ""; };
  KERN_PURE slot_nameResult slot_name(perf_Slot s = perf_Slot::logic) {
    switch (flow_switch_enum<perf_Slot>(s)) {
      case perf_Slot::logic: {
        return {"logic"};
      } break;
      case perf_Slot::physics: {
        return {"physics"};
      } break;
      case perf_Slot::stream: {
        return {"stream"};
      } break;
      case perf_Slot::draw: {
        return {"draw"};
      } break;
      case perf_Slot::count: {
        return {"?"};
      } break;
    }
    return {};
  }

  void Meter_lap(perf_Meter& self, perf_Slot s = perf_Slot::logic, Float now_ms = 0.0) {
    if (math_compare<">=">(kern::to<Float>(enum_index<perf_Slot>(s)), kern::to<Float>(enum_index<perf_Slot>(perf_Slot::count)))) {
      return;
    }
    self.cur_[enum_index<perf_Slot>(s)] = kern::formula([](Float a, Float now_ms, Float last_) -> Float { return a + (now_ms - last_); }, self.cur_[enum_index<perf_Slot>(s)], now_ms, self.last_);
    self.last_ = now_ms;
  }

  void Meter_end_frame(perf_Meter& self, Float now_ms = 0.0, Float render_cpu_ms = 0.0, Float gpu_ms = 0.0) {
    Float a = 0.0;
    a = 0.1;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<perf_Slot>(perf_Slot::count))))); it10.next();) {
      self.avg_[n10.index] = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return a + (a_2 - a) * a_3; }, self.avg_[n10.index], self.cur_[n10.index], a);
      self.cur_[n10.index] = 0.0;
    }
    self.update_ = kern::formula([](Float update_, Float now_ms, Float start_, Float a) -> Float { return update_ + (now_ms - start_ - update_) * a; }, self.update_, now_ms, self.start_, a);
    if (math_compare<">=">(render_cpu_ms, 0.0)) {
      self.render_ = kern::formula([](Float render_, Float render_cpu_ms, Float a) -> Float { return render_ + (render_cpu_ms - render_) * a; }, self.render_, render_cpu_ms, a);
    }
    self.gpu_ = kern::formula([](Float gpu_ms, Float gpu_, Float a) -> Float { return gpu_ms < 0.0 ? -1.0 : gpu_ < 0.0 ? gpu_ms : gpu_ + (gpu_ms - gpu_) * a; }, gpu_ms, self.gpu_, a);
  }

  void Governor_reset(perf_Governor& self, Int tier = 0, Int max_tier = 0) {
    self.max_ = kern::to<Int>(kern::formula([](Float max_tier, Float kTiers) -> Float { return max_tier < 0.0 ? 0.0 : max_tier >= kTiers ? kTiers - 1.0 : max_tier; }, kern::to<Float>(max_tier), kern::to<Float>(kTiers)));
    self.tier_ = kern::to<Int>(kern::formula([](Float tier, Float max_) -> Float { return tier < 0.0 ? 0.0 : tier > max_ ? max_ : tier; }, kern::to<Float>(tier), kern::to<Float>(self.max_)));
    self.auto_ = true;
    self.avg_dt_ = 0.016666666666666666;
    self.clock_ = 0.0;
    self.since_change_ = kern::formula([](Float kSettle, Float kWarmup) -> Float { return kSettle - kWarmup; }, kSettle, kWarmup);
    self.high_ = 0.0;
    self.low_ = self.high_;
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kTiers)))); it11.next();) {
      self.block_len_[n11.index] = 0.0;
      self.block_until_[n11.index] = self.block_len_[n11.index];
    }
  }

  void Governor_set_fixed(perf_Governor& self, Int tier = 0) {
    self.auto_ = false;
    self.tier_ = kern::to<Int>(kern::formula([](Float tier, Float kTiers) -> Float { return tier < 0.0 ? 0.0 : tier >= kTiers ? kTiers - 1.0 : tier; }, kern::to<Float>(tier), kern::to<Float>(kTiers)));
  }

  struct Governor_updateResult { Bool Result = false; };
  Governor_updateResult Governor_update(perf_Governor& self, Float dt = 0.0) {
    Float fps = 0.0;
    if (logic_not(math_compare<">">(dt, 0.0))) {
      return {false};
    }
    self.clock_ = kern::formula([](Float clock_, Float dt) -> Float { return clock_ + dt; }, self.clock_, dt);
    self.since_change_ = kern::formula([](Float since_change_, Float dt) -> Float { return since_change_ + dt; }, self.since_change_, dt);
    self.avg_dt_ = kern::formula([](Float avg_dt_, Float dt, Float the_result) -> Float { return avg_dt_ + (dt - avg_dt_) * the_result; }, self.avg_dt_, dt, common::damp_alpha(kern::formula([](Float kAverageTau) -> Float { return kern::math::div(1.0, kAverageTau); }, kAverageTau), dt).Result);
    if (logic_not(self.auto_)) {
      return {false};
    }
    if (math_compare<"<">(self.since_change_, kSettle)) {
      self.high_ = 0.0;
      self.low_ = self.high_;
      return {false};
    }
    fps = kern::formula([](Float avg_dt_) -> Float { return kern::math::div(1.0, avg_dt_); }, self.avg_dt_);
    self.low_ = kern::formula([](Float fps, Float kDownFps, Float low_, Float dt) -> Float { return fps < kDownFps ? low_ + dt : 0.0; }, fps, kDownFps, self.low_, dt);
    self.high_ = kern::formula([](Float fps, Float kUpFps, Float high_, Float dt) -> Float { return fps >= kUpFps ? high_ + dt : 0.0; }, fps, kUpFps, self.high_, dt);
    if (logic_and(math_compare<">=">(self.low_, kDownHold), math_compare<">">(kern::to<Float>(self.tier_), 0.0))) {
      self.block_len_[self.tier_] = kern::formula([](Float a, Float kBlockFirst, Float kBlockMax) -> Float { return a <= 0.0 ? kBlockFirst : kern::math::min(kBlockMax, a * 2.0); }, self.block_len_[self.tier_], kBlockFirst, kBlockMax);
      self.block_until_[self.tier_] = kern::formula([](Float clock_, Float a) -> Float { return clock_ + a; }, self.clock_, self.block_len_[self.tier_]);
      self.tier_ = kern::to<Int>(kern::formula([](Float tier_) -> Float { return tier_ - 1.0; }, kern::to<Float>(self.tier_)));
      self.since_change_ = 0.0;
      self.high_ = 0.0;
      self.low_ = self.high_;
      self.changes_ = kern::to<Int>(kern::formula([](Float changes_) -> Float { return kern::math::wrap32(changes_ + 1.0); }, kern::to<Float>(self.changes_)));
      return {true};
    }
    if (logic_and(logic_and(math_compare<">=">(self.high_, kUpHold), math_compare<"<">(kern::to<Float>(self.tier_), kern::to<Float>(self.max_))), math_compare<">=">(self.clock_, self.block_until_[kern::to<Int>(kern::formula([](Float tier_) -> Float { return tier_ + 1.0; }, kern::to<Float>(self.tier_)))]))) {
      self.tier_ = kern::to<Int>(kern::formula([](Float tier_) -> Float { return tier_ + 1.0; }, kern::to<Float>(self.tier_)));
      self.since_change_ = 0.0;
      self.high_ = 0.0;
      self.low_ = self.high_;
      self.changes_ = kern::to<Int>(kern::formula([](Float changes_) -> Float { return kern::math::wrap32(changes_ + 1.0); }, kern::to<Float>(self.changes_)));
      return {true};
    }
    return {false};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
};

KERN_LAYOUT(perf_cpp) {
  node(12, 0, 0);
  node(13, 280, 0);
  node(14, 560, 0);
  node(15, 840, 0);
  node(16, 1120, 0);
  node(17, 1400, 0);
  node(18, 1680, 0);
  node(19, 0, 200);
  node(20, 280, 200);
  node(21, 160, 350);
  node(22, 160, 440);
  node(23, 160, 530);
  node(24, 560, 200);
  node(25, 840, 200);
  node(26, 720, 350);
  node(27, 720, 440);
  node(28, 720, 530);
  node(29, 720, 620);
  node(30, 720, 710);
  node(31, 1120, 200);
  node(32, 0, 850);
  node(33, 280, 850);
  node(10, 560, 850);
  node(34, 440, 1000);
  node(35, 440, 1090);
  node(36, 840, 850);
  node(37, 720, 1000);
  node(38, 720, 1090);
  node(39, 720, 1180);
  node(40, 720, 1270);
  node(41, 1120, 850);
  node(42, 1400, 850);
  node(43, 1280, 1000);
  node(44, 1280, 1090);
  node(45, 1280, 1180);
  node(46, 1280, 1270);
  node(47, 1680, 850);
  node(48, 1560, 1000);
  node(49, 1960, 850);
  node(50, 1840, 1000);
  node(51, 1840, 1090);
  node(52, 1840, 1180);
  node(53, 2240, 850);
  node(54, 2120, 1000);
  node(55, 2120, 1090);
  node(56, 2120, 1180);
  node(57, 0, 1410);
  node(58, 280, 1410);
  node(59, 160, 1560);
  node(60, 160, 1650);
  node(61, 560, 1410);
  node(62, 440, 1560);
  node(63, 440, 1650);
  node(64, 840, 1410);
  node(65, 1120, 1410);
  node(66, 1400, 1410);
  node(67, 1680, 1410);
  node(68, 1560, 1560);
  node(69, 1560, 1650);
  node(70, 1560, 1740);
  node(71, 1960, 1410);
  node(72, 2240, 1410);
  node(73, 2120, 1560);
  node(11, 2520, 1410);
  node(74, 2400, 1560);
  node(75, 2400, 1650);
  node(76, 2800, 1410);
  node(77, 3080, 1410);
  node(78, 2960, 1560);
  node(79, 0, 1880);
  node(80, 280, 1880);
  node(81, 560, 1880);
  node(82, 440, 2030);
  node(83, 440, 2120);
  node(84, 0, 2260);
  node(85, 280, 2260);
  node(86, 160, 2410);
  node(87, 160, 2500);
  node(88, 560, 2260);
  node(89, 840, 2260);
  node(90, 720, 2410);
  node(91, 720, 2500);
  node(92, 1120, 2260);
  node(93, 1000, 2410);
  node(94, 1000, 2500);
  node(95, 1400, 2260);
  node(96, 1280, 2410);
  node(97, 1280, 2500);
  node(98, 1280, 2590);
  node(99, 1280, 2680);
  node(100, 1280, 2770);
  node(101, 1680, 2260);
  node(102, 1560, 2410);
  node(103, 1560, 2500);
  node(104, 1960, 2260);
  node(105, 2240, 2260);
  node(106, 2120, 2410);
  node(107, 2120, 2500);
  node(108, 2120, 2590);
  node(109, 2520, 2260);
  node(110, 2800, 2260);
  node(111, 2680, 2410);
  node(112, 3080, 2260);
  node(113, 3360, 2260);
  node(114, 3240, 2410);
  node(115, 3240, 2500);
  node(116, 3640, 2260);
  node(117, 3520, 2410);
  node(118, 3520, 2500);
  node(119, 3520, 2590);
  node(120, 3520, 2680);
  node(121, 3920, 2260);
  node(122, 3800, 2410);
  node(123, 3800, 2500);
  node(124, 3800, 2590);
  node(125, 3800, 2680);
  node(126, 4200, 2260);
  node(127, 4080, 2410);
  node(128, 4080, 2500);
  node(129, 4080, 2590);
  node(130, 4080, 2680);
  node(131, 4080, 2770);
  node(132, 4080, 2860);
  node(133, 4480, 2260);
  node(134, 4360, 2410);
  node(135, 4360, 2500);
  node(136, 4360, 2590);
  node(137, 4360, 2680);
  node(138, 4360, 2770);
  node(139, 4360, 2860);
  node(140, 4760, 2260);
  node(141, 4640, 2410);
  node(142, 4640, 2500);
  node(143, 4640, 2590);
  node(144, 4640, 2680);
  node(145, 4640, 2770);
  node(146, 5040, 2260);
  node(147, 4920, 2410);
  node(148, 4920, 2500);
  node(149, 5320, 2260);
  node(150, 5600, 2260);
  node(151, 5880, 2260);
  node(152, 5760, 2410);
  node(153, 6160, 2260);
  node(154, 6040, 2410);
  node(155, 6040, 2500);
  node(156, 6440, 2260);
  node(157, 6720, 2260);
  node(158, 6600, 2410);
  node(159, 6600, 2500);
  node(160, 6600, 2590);
  node(161, 6600, 2680);
  node(162, 6600, 2770);
  node(163, 6600, 2860);
  node(164, 6600, 2950);
  node(165, 6600, 3040);
  node(166, 6600, 3130);
  node(167, 6600, 3220);
  node(168, 6600, 3310);
  node(169, 6600, 3400);
  node(170, 6600, 3490);
  node(171, 7000, 2260);
  node(172, 6880, 2410);
  node(173, 6880, 2500);
  node(174, 7280, 2260);
  node(175, 7560, 2260);
  node(176, 7840, 2260);
  node(177, 7720, 2410);
  node(178, 8120, 2260);
  node(179, 8000, 2410);
  node(180, 8000, 2500);
  node(181, 8400, 2260);
  node(182, 8680, 2260);
  next_id(183);
}


}  // namespace kern_blueprint_c4a27d6984fae299

// ---------------------------------------------------------------------------
// The Blueprint of actor "combat_audio"
namespace kern_blueprint_84d2fe0d34f70805 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct combat_audio : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(combat_audio_Kind, shot, pistol, hit, crack, damage, scream, explosion, step, bounce, reward, fail, count);
};

KERN_LAYOUT(combat_audio) {
  next_id(1);
}


}  // namespace kern_blueprint_84d2fe0d34f70805

// ---------------------------------------------------------------------------
// The Blueprint of actor "combat_audio_cpp"
namespace kern_blueprint_adfee05bc649fab0 {

using combat_audio = ::kern_blueprint_84d2fe0d34f70805::combat_audio;
using shapes = ::kern_blueprint_6025b4a819f918d1::shapes;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct combat_audio_cpp : virtual kern::Library, virtual combat_audio, virtual shapes {
  // ---- types ----
  KERN_STRUCT(combat_audio_Random) {
    Int s = 0;
  };

  // ---- variables ----
  KERN_CONST Float TAU = 6.283185307179586;
  /// Reward / fail cues in E-flat minor at ~176 BPM, 16ths, a filtered pulse over a sine sub and a dotted-8th echo (renderCue).
  KERN_CONST Float kSixteenth = 0.08522727272727272;

  // ---- functions ----
  struct Random_newResult { combat_audio_Random Result = combat_audio_Random{0}; };
  KERN_PURE Random_newResult Random_new(Int seed = 0) {
    combat_audio_Random self = combat_audio_Random{0};
    self.s = kern::to<Int>(kern::formula([](Float seed) -> Float { return seed != 0.0 ? seed : 1.0; }, kern::to<Float>(seed)));
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
    return {kern::formula([](Float t, Float attack, Float duration, Float release) -> Float { return kern::math::min(1.0, kern::math::div(t, kern::math::max(0.001, attack))) * kern::math::min(1.0, kern::math::div(duration - t, kern::math::max(0.001, release))); }, t, attack, duration, release)};
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
    out = std::move(made);
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float>(d))))); it11.next();) {
      out[n11.index] = kern::formula([](Float a, Float gain) -> Float { return kern::math::div(kern::math::exp(2.0 * (a * gain * 1.35)) - 1.0, kern::math::exp(2.0 * (a * gain * 1.35)) + 1.0); }, d[n11.index], gain);
    }
    return {std::move(out)};
  }

  struct samplesResult { Int Result = 0; };
  KERN_PURE samplesResult samples(Float duration = 0.0, Int rate = 0) {
    return {kern::to<Int>(kern::formula([](Float duration, Float rate) -> Float { return kern::math::trunc(kern::math::ceil(duration * rate)); }, duration, kern::to<Float>(rate)))};
  }

  struct nwave_atResult { Float Result = 0.0; };
  KERN_PURE nwave_atResult nwave_at(Float t = 0.0, Float rise = 0.0, Float fall = 0.0) {
    if (math_compare<"<">(t, rise)) {
      return {kern::formula([](Float t, Float rise) -> Float { return kern::math::div(t, rise); }, t, rise)};
    }
    if (math_compare<"<">(t, kern::formula([](Float rise, Float fall) -> Float { return rise + fall; }, rise, fall))) {
      return {kern::formula([](Float t, Float rise, Float fall) -> Float { return 1.0 - kern::math::div(1.6 * (t - rise), fall); }, t, rise, fall)};
    }
    if (math_compare<"<">(t, kern::formula([](Float rise, Float fall) -> Float { return rise + fall * 1.9; }, rise, fall))) {
      return {kern::formula([](Float t, Float rise, Float fall) -> Float { return -0.6 * (1.0 - kern::math::div(t - rise - fall, fall * 0.9)); }, t, rise, fall)};
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
    duration = kern::formula([](Float v) -> Float { return 0.2 + v * 0.01; }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = std::move(made);
    made_2 = List<Float>{};
    list_resize(made_2, list_length<Float>(data), 0.0);
    dry = std::move(made_2);
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(5905421.0 + kern::math::mul32(v, 4049.0)); }, kern::to<Float>(v)))).Result;
    lpA = 0.0;
    lpB = 0.0;
    phase = 0.0;
    rise = 0.00024;
    fall = kern::formula([](Float v) -> Float { return 0.0009 + v * 5e-05; }, kern::to<Float>(v));
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float>(data))))); it12.next();) {
      t = kern::formula([](Float index, Float rate) -> Float { return kern::math::div(index < 0.0 ? index + 1.8446744073709552e+19 : index, rate); }, kern::to<Float>(n12.index), kern::to<Float>(rate));
      n13 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return the_result * 2.0 - 1.0; }, n13.Result);
      nwave = nwave_at(t, rise, fall).Result;
      lpA += kern::formula([](Float white, Float lpA) -> Float { return 0.6 * (white - lpA); }, white, lpA);
      lpB += kern::formula([](Float lpA, Float lpB) -> Float { return 0.25 * (lpA - lpB); }, lpA, lpB);
      blast = kern::formula([](Float lpA, Float lpB, Float white, Float t) -> Float { return ((lpA - lpB) * 0.9 + (white - lpA) * 0.5) * kern::math::exp(kern::math::div(-t, 0.008)); }, lpA, lpB, white, t);
      phase += kern::formula([](Float tAU, Float t, Float rate) -> Float { return kern::math::div(tAU * (185.0 * kern::math::exp(-t * 30.0) + 70.0), rate); }, TAU, t, kern::to<Float>(rate));
      thump = kern::formula([](Float phase, Float t) -> Float { return kern::math::sin(phase) * kern::math::exp(kern::math::div(-t, 0.02)) * (1.0 - kern::math::exp(kern::math::div(-t, 0.0008))); }, phase, t);
      u = kern::formula([](Float t, Float v) -> Float { return t - 0.017 - v * 0.001; }, t, kern::to<Float>(v));
      if (logic_and(math_compare<">">(u, 0.0), math_compare<"<">(u, 0.012))) {
        n15 = Random_call(random);
        chosen = kern::formula([](Float tAU, Float u, Float the_result) -> Float { return (kern::math::sin(tAU * 2700.0 * u) + 0.5 * (the_result * 2.0 - 1.0)) * kern::math::exp(kern::math::div(-u, 0.003)) * 0.25; }, TAU, u, n15.Result);
      } else {
        chosen = 0.0;
      }
      bolt = chosen;
      dry[n12.index] = kern::formula([](Float nwave, Float blast, Float thump, Float bolt) -> Float { return nwave * 1.2 + blast * 0.9 + thump * 0.7 + bolt; }, nwave, blast, thump, bolt);
    }
    taps = list_make<Float_2_row, "3">(struct_make<Float_2_row>(list_make<Float, "2">(0.011, 0.32)), struct_make<Float_2_row>(list_make<Float, "2">(0.019, 0.22)), struct_make<Float_2_row>(list_make<Float, "2">(0.031, 0.14)));
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_2_row>(taps))))); it16.next();) {
      k = kern::to<Int>(kern::formula([](Float a, Float v, Float rate) -> Float { return kern::math::trunc((a + v * 0.0006) * rate); }, taps[n16.index].items[0], kern::to<Float>(v), kern::to<Float>(rate)));
      n = list_length<Float>(data);
      go = true;
      while (loop(go)) {
        was = n;
        n = kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)));
        if (math_compare<">">(kern::formula([](Float k, Float was) -> Float { return (k < 0.0) != (was < 0.0) ? was < 0.0 : k < was; }, kern::to<Float>(k), kern::to<Float>(was)), 0.5)) {
          data[n] = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return a + a_2 * a_3; }, data[n], dry[kern::to<Int>(kern::formula([](Float n, Float k) -> Float { return n - k; }, kern::to<Float>(n), kern::to<Float>(k)))], taps[n16.index].items[1]);
        } else {
          go = false;
        }
      }
    }
    lp = 0.0;
    n_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float n_2, Float a) -> Float { return (n_2 < 0.0) != (a < 0.0) ? a < 0.0 : n_2 < a; }, kern::to<Float>(n_2), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      lp += kern::formula([](Float a, Float lp) -> Float { return 0.5 * (a - lp); }, data[n_2], lp);
      data[n_2] = kern::formula([](Float a, Float lp) -> Float { return kern::math::div(kern::math::exp(2.0 * ((a + lp * 0.8) * 2.1)) - 1.0, kern::math::exp(2.0 * ((a + lp * 0.8) * 2.1)) + 1.0); }, dry[n_2], lp);
      n_2 += 1;
    }
    n17 = finish(data, 0.95);
    return {std::move(n17.Result)};
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
    duration = kern::formula([](Float v) -> Float { return 0.58 + v * 0.012; }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = std::move(made);
    made_2 = List<Float>{};
    list_resize(made_2, list_length<Float>(data), 0.0);
    dry = std::move(made_2);
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(6406311.0 + kern::math::mul32(v, 7919.0)); }, kern::to<Float>(v)))).Result;
    rise = kern::formula([](Float v) -> Float { return 0.00028 + v * 2e-05; }, kern::to<Float>(v));
    fall = kern::formula([](Float v) -> Float { return 0.00105 + v * 6e-05; }, kern::to<Float>(v));
    hp = 0.0;
    lpA = 0.0;
    lpB = 0.0;
    tail = 0.0;
    phase = 0.0;
    ring2 = 0.0;
    mech = list_make<Float_4_row, "3">(struct_make<Float_4_row>(list_make<Float, "4">(kern::formula([](Float v) -> Float { return 0.0215 + v * 0.0011; }, kern::to<Float>(v)), 2450.0, 3810.0, 1.0)), struct_make<Float_4_row>(list_make<Float, "4">(kern::formula([](Float v) -> Float { return 0.0452 + v * 0.0017; }, kern::to<Float>(v)), 2080.0, 3260.0, 0.8)), struct_make<Float_4_row>(list_make<Float, "4">(kern::formula([](Float v) -> Float { return 0.0705 + v * 0.0013; }, kern::to<Float>(v)), 2900.0, 4400.0, 0.35)));
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float>(data))))); it18.next();) {
      t = kern::formula([](Float index, Float rate) -> Float { return kern::math::div(index < 0.0 ? index + 1.8446744073709552e+19 : index, rate); }, kern::to<Float>(n18.index), kern::to<Float>(rate));
      n19 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return the_result * 2.0 - 1.0; }, n19.Result);
      nwave = nwave_at(t, rise, fall).Result;
      lpA += kern::formula([](Float white, Float lpA) -> Float { return 0.55 * (white - lpA); }, white, lpA);
      hp = kern::formula([](Float white, Float lpA) -> Float { return white - lpA; }, white, lpA);
      lpB += kern::formula([](Float lpA, Float lpB) -> Float { return 0.22 * (lpA - lpB); }, lpA, lpB);
      the_band = kern::formula([](Float lpA, Float lpB) -> Float { return lpA - lpB; }, lpA, lpB);
      blast = kern::formula([](Float the_band, Float hp, Float t) -> Float { return (the_band * 0.9 + hp * 0.55) * kern::math::exp(kern::math::div(-t, 0.0105)); }, the_band, hp, t);
      f = kern::formula([](Float t) -> Float { return 185.0 * kern::math::exp(-t * 22.0) + 50.0; }, t);
      phase += kern::formula([](Float tAU, Float f, Float rate) -> Float { return kern::math::div(tAU * f, rate); }, TAU, f, kern::to<Float>(rate));
      thump = kern::formula([](Float phase, Float t) -> Float { return kern::math::sin(phase) * kern::math::exp(kern::math::div(-t, 0.042)) * (1.0 - kern::math::exp(kern::math::div(-t, 0.0009))); }, phase, t);
      boom = kern::formula([](Float tAU, Float t) -> Float { return kern::math::sin(tAU * 44.0 * t) * kern::math::exp(kern::math::div(-t, 0.11)) * (1.0 - kern::math::exp(kern::math::div(-t, 0.004))); }, TAU, t);
      if (math_compare<"<">(t, 0.0018)) {
        n21 = Random_call(random);
        chosen = kern::formula([](Float the_result, Float t) -> Float { return (the_result * 2.0 - 1.0) * (1.0 - kern::math::div(t, 0.0018)); }, n21.Result, t);
      } else {
        chosen = 0.0;
      }
      snap = chosen;
      m = 0.0;
      for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_4_row>(mech))))); it22.next();) {
        u = kern::formula([](Float t, Float a) -> Float { return t - a; }, t, mech[n22.index].items[0]);
        if (logic_and(math_compare<">=">(u, 0.0), math_compare<"<">(u, 0.02))) {
          n23 = Random_call(random);
          m += kern::formula([](Float a, Float tAU, Float a_2, Float u, Float a_3, Float the_result) -> Float { return a * (kern::math::sin(tAU * a_2 * u) + 0.6 * kern::math::sin(tAU * a_3 * u) + 0.4 * (the_result * 2.0 - 1.0)) * kern::math::exp(kern::math::div(-u, 0.0042)); }, mech[n22.index].items[3], TAU, mech[n22.index].items[1], u, mech[n22.index].items[2], n23.Result);
        }
      }
      dry[n18.index] = kern::formula([](Float nwave, Float snap, Float blast, Float thump, Float boom, Float m) -> Float { return nwave * 1.3 + snap * 0.7 + blast * 1.0 + thump * 1.15 + boom * 0.38 + m * 0.36; }, nwave, snap, blast, thump, boom, m);
    }
    taps = list_make<Float_2_row, "5">(struct_make<Float_2_row>(list_make<Float, "2">(0.0142, 0.42)), struct_make<Float_2_row>(list_make<Float, "2">(0.0235, 0.3)), struct_make<Float_2_row>(list_make<Float, "2">(0.0371, 0.24)), struct_make<Float_2_row>(list_make<Float, "2">(0.0516, 0.17)), struct_make<Float_2_row>(list_make<Float, "2">(0.0784, 0.11)));
    d = List<Int>{};
    for (auto it24 = flow_for(n24, 0, 4); it24.next();) {
      d[n24.index] = kern::to<Int>(kern::formula([](Float a, Float v, Float rate) -> Float { return kern::math::trunc((a + v * 0.0007) * rate); }, taps[n24.index].items[0], kern::to<Float>(v), kern::to<Float>(rate)));
    }
    n = 0;
    while (loop(math_compare<">">(kern::formula([](Float n, Float a) -> Float { return (n < 0.0) != (a < 0.0) ? a < 0.0 : n < a; }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t_2 = kern::formula([](Float n, Float rate) -> Float { return kern::math::div(n < 0.0 ? n + 1.8446744073709552e+19 : n, rate); }, kern::to<Float>(n), kern::to<Float>(rate));
      val = dry[n];
      for (auto it25 = flow_for(n25, 0, 4); it25.next();) {
        if (math_compare<">">(kern::formula([](Float n, Float a) -> Float { return !((n < 0.0) != (a < 0.0) ? a < 0.0 : n < a); }, kern::to<Float>(n), kern::to<Float>(d[n25.index])), 0.5)) {
          val += kern::formula([](Float a, Float a_2) -> Float { return a * a_2 * 0.8; }, taps[n25.index].items[1], dry[kern::to<Int>(kern::formula([](Float n, Float a) -> Float { return n - a; }, kern::to<Float>(n), kern::to<Float>(d[n25.index])))]);
        }
      }
      n26 = Random_call(random);
      ring2 += kern::formula([](Float the_result, Float ring2) -> Float { return 0.08 * (the_result * 2.0 - 1.0 - ring2); }, n26.Result, ring2);
      tail += kern::formula([](Float ring2, Float tail) -> Float { return 0.15 * (ring2 - tail); }, ring2, tail);
      diffuse = kern::formula([](Float t_2, Float tail) -> Float { return t_2 > 0.012 ? tail * 0.42 * kern::math::exp(kern::math::div(-(t_2 - 0.012), 0.16)) : 0.0; }, t_2, tail);
      data[n] = kern::formula([](Float val, Float diffuse) -> Float { return val + diffuse; }, val, diffuse);
      n += 1;
    }
    lp = 0.0;
    n_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float n_2, Float a) -> Float { return (n_2 < 0.0) != (a < 0.0) ? a < 0.0 : n_2 < a; }, kern::to<Float>(n_2), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      lp += kern::formula([](Float a, Float lp) -> Float { return 0.45 * (a - lp); }, data[n_2], lp);
      data[n_2] = kern::formula([](Float a, Float lp) -> Float { return a + (lp - a * 0.45); }, dry[n_2], lp);
      n_2 += 1;
    }
    for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(data))))); it27.next();) {
      data[n27.index] = kern::formula([](Float a) -> Float { return kern::math::div(kern::math::exp(2.0 * (a * 2.6)) - 1.0, kern::math::exp(2.0 * (a * 2.6)) + 1.0); }, data[n27.index]);
    }
    n28 = finish(data, 0.99);
    return {std::move(n28.Result)};
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
    duration = kern::formula([](Float v) -> Float { return 0.105 + v * 0.009; }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = std::move(made);
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(9555439.0 + kern::math::mul32(v, 733.0)); }, kern::to<Float>(v)))).Result;
    delay = kern::to<Int>(kern::formula([](Float rate, Float v) -> Float { return kern::math::trunc(rate * (0.008 + v * 0.001)); }, kern::to<Float>(rate), kern::to<Float>(v)));
    low = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (i < 0.0) != (a < 0.0) ? a < 0.0 : i < a; }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div(i < 0.0 ? i + 1.8446744073709552e+19 : i, rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n29 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return the_result * 2.0 - 1.0; }, n29.Result);
      low += kern::formula([](Float white, Float low) -> Float { return 0.2 * (white - low); }, white, low);
      surface = kern::formula([](Float white, Float low, Float t, Float v) -> Float { return (white * 0.35 + low * 0.65) * kern::math::exp(-t * (37.0 - v * 2.0)); }, white, low, t, kern::to<Float>(v));
      frequency = kern::formula([](Float v, Float t) -> Float { return kern::math::wrap32(112.0 + kern::math::mul32(v, 9.0)) * kern::math::exp(-t * 13.0) + 46.0; }, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float frequency, Float rate) -> Float { return kern::math::div(tAU * frequency, rate); }, TAU, frequency, kern::to<Float>(rate));
      thud = kern::formula([](Float phase, Float t) -> Float { return kern::math::sin(phase) * kern::math::exp(-t * 30.0); }, phase, t);
      slap = kern::formula([](Float i, Float delay, Float a) -> Float { return i >= delay ? a * 0.16 : 0.0; }, kern::to<Float>(i), kern::to<Float>(delay), data[kern::to<Int>(kern::formula([](Float i, Float delay) -> Float { return i - delay; }, kern::to<Float>(i), kern::to<Float>(delay)))]);
      data[i] = kern::formula([](Float surface, Float thud, Float slap) -> Float { return surface * 0.78 + thud * 0.58 + slap; }, surface, thud, slap);
      i += 1;
    }
    n30 = finish(data, 0.82);
    return {std::move(n30.Result)};
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
    duration = kern::formula([](Float v) -> Float { return 0.082 + v * 0.006; }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = std::move(made);
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(12659399.0 + kern::math::mul32(v, 991.0)); }, kern::to<Float>(v)))).Result;
    low = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (i < 0.0) != (a < 0.0) ? a < 0.0 : i < a; }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div(i < 0.0 ? i + 1.8446744073709552e+19 : i, rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n31 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return the_result * 2.0 - 1.0; }, n31.Result);
      low += kern::formula([](Float white, Float low) -> Float { return 0.08 * (white - low); }, white, low);
      snap = kern::formula([](Float white, Float low, Float t, Float v) -> Float { return (white - low) * kern::math::exp(-t * (92.0 - v * 4.0)); }, white, low, t, kern::to<Float>(v));
      frequency = kern::formula([](Float v, Float t) -> Float { return kern::math::wrap32(760.0 + kern::math::mul32(v, 85.0)) * kern::math::exp(-t * 24.0) + kern::math::wrap32(135.0 + kern::math::mul32(v, 13.0)); }, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float frequency, Float rate) -> Float { return kern::math::div(tAU * frequency, rate); }, TAU, frequency, kern::to<Float>(rate));
      wood = kern::formula([](Float phase, Float t, Float v) -> Float { return (kern::math::sin(phase) + 0.28 * kern::math::sin(phase * 2.07 + 0.4)) * kern::math::exp(-t * (42.0 - v * 2.0)); }, phase, t, kern::to<Float>(v));
      data[i] = kern::formula([](Float snap, Float wood) -> Float { return snap * 0.82 + wood * 0.52; }, snap, wood);
      i += 1;
    }
    n32 = finish(data, 0.84);
    return {std::move(n32.Result)};
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
    duration = kern::formula([](Float v) -> Float { return 0.21 + v * 0.035; }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = std::move(made);
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(8048817.0 + kern::math::mul32(v, 811.0)); }, kern::to<Float>(v)))).Result;
    low = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (i < 0.0) != (a < 0.0) ? a < 0.0 : i < a; }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div(i < 0.0 ? i + 1.8446744073709552e+19 : i, rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n33 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return the_result * 2.0 - 1.0; }, n33.Result);
      low += kern::formula([](Float white, Float low) -> Float { return 0.065 * (white - low); }, white, low);
      frequency = kern::formula([](Float v, Float t) -> Float { return kern::math::wrap32(92.0 + kern::math::mul32(v, 12.0)) * kern::math::exp(-t * 8.0) + 31.0; }, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float frequency, Float rate) -> Float { return kern::math::div(tAU * frequency, rate); }, TAU, frequency, kern::to<Float>(rate));
      pressure = kern::formula([](Float phase, Float t) -> Float { return kern::math::sin(phase + 0.3 * kern::math::sin(phase * 0.5)) * kern::math::exp(-t * 13.0); }, phase, t);
      rush = kern::formula([](Float low, Float t) -> Float { return low * kern::math::exp(-t * 8.0); }, low, t);
      data[i] = kern::formula([](Float pressure, Float rush) -> Float { return pressure * 0.7 + rush * 0.76; }, pressure, rush);
      i += 1;
    }
    n34 = finish(data, 0.86);
    return {std::move(n34.Result)};
  }

  struct gaussianResult { Float Result = 0.0; };
  KERN_PURE gaussianResult gaussian(Float value = 0.0, Float center = 0.0, Float width = 0.0) {
    Float x = 0.0;
    x = kern::formula([](Float value, Float center, Float width) -> Float { return kern::math::div(value - center, width); }, value, center, width);
    return {kern::formula([](Float x) -> Float { return kern::math::exp(-0.5 * x * x); }, x)};
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
    duration = kern::formula([](Float v) -> Float { return 0.56 + v * 0.055; }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = std::move(made);
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(12648430.0 + kern::math::mul32(v, 1297.0)); }, kern::to<Float>(v)))).Result;
    bases = list_make<Float, "4">(238.0, 276.0, 218.0, 258.0);
    formants = list_make<Float_3_row, "4">(struct_make<Float_3_row>(list_make<Float, "3">(760.0, 1280.0, 2480.0)), struct_make<Float_3_row>(list_make<Float, "3">(870.0, 1430.0, 2670.0)), struct_make<Float_3_row>(list_make<Float, "3">(690.0, 1130.0, 2320.0)), struct_make<Float_3_row>(list_make<Float, "3">(820.0, 1510.0, 2580.0)));
    base = bases[kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 4.0); }, kern::to<Float>(v)))];
    which = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 4.0); }, kern::to<Float>(v)));
    phase = 0.0;
    breath = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (i < 0.0) != (a < 0.0) ? a < 0.0 : i < a; }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div(i < 0.0 ? i + 1.8446744073709552e+19 : i, rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      p = kern::formula([](Float t, Float duration) -> Float { return kern::math::div(t, duration); }, t, duration);
      rise = kern::formula([](Float p) -> Float { return kern::math::sin(kern::math::div(kern::math::min(1.0, kern::math::div(p, 0.34)) * 3.141592653589793, 2.0)); }, p);
      fall = kern::formula([](Float p) -> Float { return kern::math::max(0.0, kern::math::div(p - 0.42, 0.58)); }, p);
      f0 = kern::formula([](Float base, Float rise, Float fall, Float tAU, Float v, Float t) -> Float { return base * (0.82 + 0.34 * rise - 0.35 * fall) * (1.0 + 0.018 * kern::math::sin(tAU * (5.1 + v * 0.35) * t)); }, base, rise, fall, TAU, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float f0, Float rate) -> Float { return kern::math::div(tAU * f0, rate); }, TAU, f0, kern::to<Float>(rate));
      voice = 0.0;
      for (auto it35 = flow_for(n35, 1, 12); it35.next();) {
        hz = kern::formula([](Float f0, Float index) -> Float { return f0 * index; }, f0, kern::to<Float>(n35.index));
        shape = kern::formula([](Float the_result, Float the_result_2, Float the_result_3) -> Float { return 0.18 + 1.35 * the_result + 1.02 * the_result_2 + 0.55 * the_result_3; }, gaussian(hz, formants[which].items[0], 260.0).Result, gaussian(hz, formants[which].items[1], 390.0).Result, gaussian(hz, formants[which].items[2], 620.0).Result);
        voice += kern::formula([](Float phase, Float index, Float v, Float shape) -> Float { return kern::math::div(kern::math::sin(phase * index + index * 0.31 * v) * shape, kern::math::pow(index, 0.82)); }, phase, kern::to<Float>(n35.index), kern::to<Float>(v), shape);
      }
      n36 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return the_result * 2.0 - 1.0; }, n36.Result);
      breath += kern::formula([](Float white, Float breath) -> Float { return 0.14 * (white - breath); }, white, breath);
      air = kern::formula([](Float white, Float breath, Float rise) -> Float { return (white - breath) * (0.18 + 0.22 * rise); }, white, breath, rise);
      amp = kern::formula([](Float the_result, Float p) -> Float { return the_result * (0.72 + 0.28 * kern::math::sin(3.141592653589793 * p)); }, envelope(t, 0.024, 0.15, duration).Result, p);
      data[i] = kern::formula([](Float voice, Float air, Float amp) -> Float { return (voice * 0.3 + air) * amp; }, voice, air, amp);
      i += 1;
    }
    n37 = finish(data, 0.9);
    return {std::move(n37.Result)};
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
    duration = kern::formula([](Float v) -> Float { return 0.76 + v * 0.08; }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = std::move(made);
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(720983.0 + kern::math::mul32(v, 1901.0)); }, kern::to<Float>(v)))).Result;
    rumble = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (i < 0.0) != (a < 0.0) ? a < 0.0 : i < a; }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div(i < 0.0 ? i + 1.8446744073709552e+19 : i, rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n38 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return the_result * 2.0 - 1.0; }, n38.Result);
      rumble += kern::formula([](Float white, Float rumble) -> Float { return 0.035 * (white - rumble); }, white, rumble);
      crk = kern::formula([](Float white, Float rumble, Float t) -> Float { return (white - rumble) * kern::math::exp(-t * 24.0); }, white, rumble, t);
      frequency = kern::formula([](Float v, Float t) -> Float { return kern::math::wrap32(88.0 + kern::math::mul32(v, 13.0)) * kern::math::exp(-t * 5.7) + 22.0; }, kern::to<Float>(v), t);
      phase += kern::formula([](Float tAU, Float frequency, Float rate) -> Float { return kern::math::div(tAU * frequency, rate); }, TAU, frequency, kern::to<Float>(rate));
      boom = kern::formula([](Float phase, Float t) -> Float { return kern::math::sin(phase + 0.45 * kern::math::sin(phase * 0.37)) * kern::math::exp(-t * 4.7); }, phase, t);
      tail = kern::formula([](Float rumble, Float t) -> Float { return rumble * kern::math::exp(-t * 3.1); }, rumble, t);
      data[i] = kern::formula([](Float crk, Float boom, Float tail) -> Float { return crk * 0.5 + boom * 0.82 + tail * 1.1; }, crk, boom, tail);
      i += 1;
    }
    n39 = finish(data, 0.94);
    return {std::move(n39.Result)};
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
    duration = kern::formula([](Float v) -> Float { return 0.075 + v * 0.008; }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = std::move(made);
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(22505.0 + kern::math::mul32(v, 431.0)); }, kern::to<Float>(v)))).Result;
    low = 0.0;
    phase = 0.0;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (i < 0.0) != (a < 0.0) ? a < 0.0 : i < a; }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div(i < 0.0 ? i + 1.8446744073709552e+19 : i, rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n40 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return the_result * 2.0 - 1.0; }, n40.Result);
      low += kern::formula([](Float white, Float low) -> Float { return 0.09 * (white - low); }, white, low);
      phase += kern::formula([](Float tAU, Float v, Float t, Float rate) -> Float { return kern::math::div(tAU * kern::math::wrap32(78.0 + kern::math::mul32(v, 9.0)) * kern::math::exp(-t * 12.0), rate); }, TAU, kern::to<Float>(v), t, kern::to<Float>(rate));
      data[i] = kern::formula([](Float low, Float phase, Float t) -> Float { return (low * 0.92 + kern::math::sin(phase) * 0.36) * kern::math::exp(-t * 38.0); }, low, phase, t);
      i += 1;
    }
    n41 = finish(data, 0.72);
    return {std::move(n41.Result)};
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
    duration = kern::formula([](Float v) -> Float { return 0.16 + v * 0.01; }, kern::to<Float>(v));
    made = List<Float>{};
    list_resize(made, samples(duration, rate).Result, 0.0);
    data = std::move(made);
    random = Random_new(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(53444.0 + kern::math::mul32(v, 977.0)); }, kern::to<Float>(v)))).Result;
    lp = 0.0;
    phase = 0.0;
    ring = 0.0;
    f0 = kern::formula([](Float v) -> Float { return kern::math::wrap32(150.0 + kern::math::mul32(v, 18.0)); }, kern::to<Float>(v));
    fr = kern::formula([](Float v) -> Float { return kern::math::wrap32(1850.0 + kern::math::mul32(v, 240.0)); }, kern::to<Float>(v));
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (i < 0.0) != (a < 0.0) ? a < 0.0 : i < a; }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div(i < 0.0 ? i + 1.8446744073709552e+19 : i, rate); }, kern::to<Float>(i), kern::to<Float>(rate));
      n42 = Random_call(random);
      white = kern::formula([](Float the_result) -> Float { return the_result * 2.0 - 1.0; }, n42.Result);
      lp += kern::formula([](Float white, Float lp) -> Float { return 0.35 * (white - lp); }, white, lp);
      click = kern::formula([](Float white, Float lp, Float t) -> Float { return (white - lp) * kern::math::exp(kern::math::div(-t, 0.0022)); }, white, lp, t);
      phase += kern::formula([](Float tAU, Float f0, Float t, Float rate) -> Float { return kern::math::div(tAU * (f0 * (1.0 + 0.6 * kern::math::exp(-t * 90.0))), rate); }, TAU, f0, t, kern::to<Float>(rate));
      knock = kern::formula([](Float phase, Float t) -> Float { return kern::math::sin(phase) * kern::math::exp(kern::math::div(-t, 0.026)) * (1.0 - kern::math::exp(kern::math::div(-t, 0.0006))); }, phase, t);
      ring += kern::formula([](Float tAU, Float fr, Float rate) -> Float { return kern::math::div(tAU * fr, rate); }, TAU, fr, kern::to<Float>(rate));
      shell = kern::formula([](Float ring, Float t) -> Float { return kern::math::sin(ring) * kern::math::exp(kern::math::div(-t, 0.012)) * 0.35; }, ring, t);
      n43 = Random_call(random);
      grit = kern::formula([](Float lp, Float t, Float the_result) -> Float { return lp * kern::math::exp(kern::math::div(-t, 0.05)) * 0.18 * (the_result < 0.04 ? 2.5 : 1.0); }, lp, t, n43.Result);
      data[i] = kern::formula([](Float click, Float knock, Float shell, Float grit) -> Float { return click * 0.9 + knock * 1.1 + shell + grit; }, click, knock, shell, grit);
      i += 1;
    }
    n44 = finish(data, 0.82);
    return {std::move(n44.Result)};
  }

  struct note_hzResult { Float Result = 0.0; };
  KERN_PURE note_hzResult note_hz(Int n = 0) {
    return {kern::formula([](Float n) -> Float { return 440.0 * kern::math::pow(2.0, kern::math::div(n - 69.0, 12.0)); }, kern::to<Float>(n))};
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
    total = kern::formula([](Float total, Float kSixteenth) -> Float { return total * kSixteenth; }, total, kSixteenth);
    made = List<Float>{};
    list_resize(made, samples(kern::formula([](Float total, Float kSixteenth) -> Float { return total + kSixteenth * 6.0; }, total, kSixteenth), rate).Result, 0.0);
    dry = std::move(made);
    t0 = 0.0;
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int>(notes))))); it46.next();) {
      skip = false;
      len = kern::formula([](Float a, Float kSixteenth) -> Float { return a * kSixteenth; }, lengths[n46.index], kSixteenth);
      if (math_compare<"<">(kern::to<Float>(notes[n46.index]), 0.0)) {
        t0 += len;
        skip = true;
      }
      if (logic_not(skip)) {
        f = note_hz(notes[n46.index]).Result;
        dur = kern::formula([](Float len) -> Float { return len * 1.6; }, len);
        start = kern::to<Int>(kern::formula([](Float t0, Float rate) -> Float { return kern::math::trunc(t0 * rate); }, t0, kern::to<Float>(rate)));
        count = kern::to<Int>(kern::formula([](Float dur, Float rate) -> Float { return kern::math::trunc(dur * rate); }, dur, kern::to<Float>(rate)));
        phase = 0.0;
        sub = 0.0;
        lp = 0.0;
        i = 0;
        while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float count) -> Float { return (i < 0.0) != (count < 0.0) ? count < 0.0 : i < count; }, kern::to<Float>(i), kern::to<Float>(count)), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float start, Float i) -> Float { return start + i; }, kern::to<Float>(start), kern::to<Float>(i))))), kern::to<Float>(list_length<Float>(dry))), 0.5)))) {
          t = kern::formula([](Float i, Float rate) -> Float { return kern::math::div(i < 0.0 ? i + 1.8446744073709552e+19 : i, rate); }, kern::to<Float>(i), kern::to<Float>(rate));
          env = kern::formula([](Float t, Float len) -> Float { return kern::math::min(1.0, kern::math::div(t, 0.006)) * kern::math::exp(kern::math::div(-t, len * 0.75)); }, t, len);
          phase += kern::formula([](Float f, Float rate) -> Float { return kern::math::div(f, rate); }, f, kern::to<Float>(rate));
          sub += kern::formula([](Float f, Float rate) -> Float { return kern::math::div(f * 0.5, rate); }, f, kern::to<Float>(rate));
          pulse = kern::formula([](Float phase) -> Float { return kern::math::mod(phase, 1.0) < 0.5 ? 1.0 : -1.0; }, phase);
          alpha = kern::formula([](Float tAU, Float cutoff, Float t, Float dur, Float rate) -> Float { return kern::math::min(1.0, kern::math::div(tAU * cutoff * (1.0 - kern::math::div(0.5 * t, dur)), rate)); }, TAU, cutoff, t, dur, kern::to<Float>(rate));
          lp += kern::formula([](Float alpha, Float pulse, Float lp) -> Float { return alpha * (pulse - lp); }, alpha, pulse, lp);
          dry[kern::to<Int>(kern::formula([](Float start, Float i) -> Float { return start + i; }, kern::to<Float>(start), kern::to<Float>(i)))] = kern::formula([](Float a, Float lp, Float tAU, Float sub, Float env) -> Float { return a + (lp * 0.55 + kern::math::sin(tAU * sub) * 0.28) * env; }, dry[kern::to<Int>(kern::formula([](Float start, Float i) -> Float { return start + i; }, kern::to<Float>(start), kern::to<Float>(i)))], lp, TAU, sub, env);
          i += 1;
        }
        t0 += len;
      }
    }
    made_2 = List<Float>{};
    list_resize(made_2, list_length<Float>(dry), 0.0);
    data = std::move(made_2);
    delay = kern::to<Int>(kern::formula([](Float kSixteenth, Float rate) -> Float { return kern::math::trunc(kSixteenth * 3.0 * rate); }, kSixteenth, kern::to<Float>(rate)));
    i_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float i_2, Float a) -> Float { return (i_2 < 0.0) != (a < 0.0) ? a < 0.0 : i_2 < a; }, kern::to<Float>(i_2), kern::to<Float>(list_length<Float>(data))), 0.5))) {
      e1 = kern::formula([](Float i_2, Float delay, Float a, Float echo) -> Float { return i_2 >= delay ? a * echo : 0.0; }, kern::to<Float>(i_2), kern::to<Float>(delay), dry[kern::to<Int>(kern::formula([](Float i_2, Float delay) -> Float { return i_2 - delay; }, kern::to<Float>(i_2), kern::to<Float>(delay)))], echo);
      e2 = kern::formula([](Float i_2, Float delay, Float a, Float echo) -> Float { return i_2 >= 2.0 * delay ? a * echo * echo : 0.0; }, kern::to<Float>(i_2), kern::to<Float>(delay), dry[kern::to<Int>(kern::formula([](Float i_2, Float delay) -> Float { return i_2 - 2.0 * delay; }, kern::to<Float>(i_2), kern::to<Float>(delay)))], echo);
      data[i_2] = kern::formula([](Float a, Float e1, Float e2) -> Float { return a + e1 + e2; }, dry[i_2], e1, e2);
      i_2 += 1;
    }
    fade = kern::to<Int>(kern::formula([](Float rate) -> Float { return kern::math::trunc(0.04 * rate); }, kern::to<Float>(rate)));
    i_3 = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i_3, Float fade) -> Float { return (i_3 < 0.0) != (fade < 0.0) ? fade < 0.0 : i_3 < fade; }, kern::to<Float>(i_3), kern::to<Float>(fade)), 0.5), math_compare<">">(kern::formula([](Float i_3, Float a) -> Float { return (i_3 < 0.0) != (a < 0.0) ? a < 0.0 : i_3 < a; }, kern::to<Float>(i_3), kern::to<Float>(list_length<Float>(data))), 0.5)))) {
      data[kern::to<Int>(kern::formula([](Float a, Float i_3) -> Float { return a - 1.0 - i_3; }, kern::to<Float>(list_length<Float>(data)), kern::to<Float>(i_3)))] = kern::formula([](Float a, Float i_3, Float fade) -> Float { return a * kern::math::div(i_3 < 0.0 ? i_3 + 1.8446744073709552e+19 : i_3, fade < 0.0 ? fade + 1.8446744073709552e+19 : fade); }, data[kern::to<Int>(kern::formula([](Float a, Float i_3) -> Float { return a - 1.0 - i_3; }, kern::to<Float>(list_length<Float>(data)), kern::to<Float>(i_3)))], kern::to<Float>(i_3), kern::to<Float>(fade));
      i_3 += 1;
    }
    n47 = finish(data, level);
    return {std::move(n47.Result)};
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
    return {};
  }

  struct renderResult { List<Float> Result = List<Float>{}; };
  renderResult render(combat_audio_Kind k = combat_audio_Kind::shot, Int v = 0, Int rate = 0) {
    Int rate_ = 0;
    rate_ = rate;
    rate_ = kern::to<Int>(kern::formula([](Float rate_) -> Float { return rate_ < 8000.0 ? 8000.0 : rate_; }, kern::to<Float>(rate_)));
    switch (flow_switch_enum<combat_audio_Kind>(k)) {
      case combat_audio_Kind::shot: {
        n48 = shot(rate_, v);
        return {std::move(n48.Result)};
      } break;
      case combat_audio_Kind::pistol: {
        n49 = pistol(rate_, v);
        return {std::move(n49.Result)};
      } break;
      case combat_audio_Kind::hit: {
        n50 = hit(rate_, v);
        return {std::move(n50.Result)};
      } break;
      case combat_audio_Kind::crack: {
        n51 = crack(rate_, v);
        return {std::move(n51.Result)};
      } break;
      case combat_audio_Kind::damage: {
        n52 = damage(rate_, v);
        return {std::move(n52.Result)};
      } break;
      case combat_audio_Kind::scream: {
        n53 = scream(rate_, v);
        return {std::move(n53.Result)};
      } break;
      case combat_audio_Kind::explosion: {
        n54 = explosion(rate_, v);
        return {std::move(n54.Result)};
      } break;
      case combat_audio_Kind::step: {
        n55 = step(rate_, v);
        return {std::move(n55.Result)};
      } break;
      case combat_audio_Kind::bounce: {
        n56 = bounce(rate_, v);
        return {std::move(n56.Result)};
      } break;
      case combat_audio_Kind::reward: {
        if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 3.0); }, kern::to<Float>(v))))), 0.0)) {
          n57 = cue(rate_, list_make<Int, "2">(70, 75), list_make<Float, "2">(1.0, 2.0), 2400.0, 0.3, 0.75);
          return {std::move(n57.Result)};
        }
        if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 3.0); }, kern::to<Float>(v))))), 1.0)) {
          n58 = cue(rate_, list_make<Int, "4">(63, 66, 70, 75), list_make<Float, "4">(1.0, 1.0, 1.0, 2.0), 2800.0, 0.3, 0.75);
          return {std::move(n58.Result)};
        }
        n59 = cue(rate_, list_make<Int, "6">(66, 70, 73, 78, -1, 82), list_make<Float, "6">(1.0, 1.0, 1.0, 1.0, 1.0, 3.0), 3200.0, 0.36, 0.75);
        return {std::move(n59.Result)};
      } break;
      case combat_audio_Kind::fail: {
        if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mod(v, 2.0); }, kern::to<Float>(v))))), 0.0)) {
          n60 = cue(rate_, list_make<Int, "3">(70, 69, 66), list_make<Float, "3">(1.0, 1.0, 3.0), 1400.0, 0.22, 0.7);
          return {std::move(n60.Result)};
        }
        n61 = cue(rate_, list_make<Int, "3">(63, 58, 51), list_make<Float, "3">(1.0, 1.0, 4.0), 1100.0, 0.2, 0.72);
        return {std::move(n61.Result)};
      } break;
      case combat_audio_Kind::count: {
        return {List<Float>{}};
      } break;
    }
    return {};
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

KERN_LAYOUT(combat_audio_cpp) {
  node(848, 0, 9552);
  node(849, 288, 9608);
  node(850, 240, 9760);
  node(851, 576, 9664);
  node(852, 528, 9816);
  node(853, 0, 10096);
  node(854, 288, 10152);
  node(855, 240, 10304);
  node(856, 240, 10408);
  node(857, 576, 10208);
  node(858, 528, 10360);
  node(859, 528, 10464);
  node(860, 864, 10264);
  node(861, 816, 10416);
  node(862, 816, 10520);
  node(863, 1152, 10320);
  node(864, 1104, 10472);
  node(865, 1104, 10576);
  node(866, 0, 10856);
  node(867, 288, 10912);
  node(868, 240, 11064);
  node(869, 0, 11344);
  node(870, 288, 11400);
  node(10, 560, 960);
  node(871, 512, 1112);
  node(872, 512, 1216);
  node(873, 512, 1320);
  node(874, 848, 1016);
  node(875, 800, 1168);
  node(62, 1120, 960);
  node(63, 1000, 1110);
  node(64, 1000, 1200);
  node(65, 1000, 1290);
  node(66, 1400, 960);
  node(67, 1280, 1110);
  node(68, 1280, 1200);
  node(70, 1960, 960);
  node(837, 2248, 1016);
  node(71, 1840, 1110);
  node(72, 1840, 1200);
  node(79, 2800, 960);
  node(80, 2680, 1110);
  node(11, 3080, 960);
  node(81, 2960, 1110);
  node(82, 2960, 1200);
  node(83, 2960, 1290);
  node(84, 3360, 960);
  node(85, 3240, 1110);
  node(86, 3240, 1200);
  node(87, 3240, 1290);
  node(88, 3640, 960);
  node(89, 3520, 1110);
  node(90, 0, 1520);
  node(91, 280, 1520);
  node(92, 160, 1670);
  node(93, 0, 1810);
  node(94, 280, 1810);
  node(95, 160, 1960);
  node(96, 560, 1810);
  node(97, 440, 1960);
  node(98, 840, 1810);
  node(99, 720, 1960);
  node(100, 720, 2050);
  node(101, 1120, 1810);
  node(102, 1000, 1960);
  node(103, 1400, 1810);
  node(104, 1280, 1960);
  node(105, 1280, 2050);
  node(106, 1680, 1810);
  node(107, 1560, 1960);
  node(108, 1960, 1810);
  node(109, 0, 2190);
  node(110, 280, 2190);
  node(111, 160, 2340);
  node(113, 840, 2190);
  node(838, 1128, 2248);
  node(114, 720, 2340);
  node(115, 720, 2430);
  node(122, 1680, 2190);
  node(123, 1560, 2340);
  node(125, 2240, 2190);
  node(839, 2528, 2248);
  node(126, 2120, 2340);
  node(127, 2120, 2430);
  node(134, 3080, 2190);
  node(135, 2960, 2340);
  node(136, 3360, 2190);
  node(137, 3240, 2340);
  node(138, 3240, 2430);
  node(139, 3640, 2190);
  node(140, 3920, 2190);
  node(141, 4200, 2190);
  node(142, 4480, 2190);
  node(143, 4760, 2190);
  node(144, 4640, 2340);
  node(12, 5040, 2190);
  node(145, 4920, 2340);
  node(146, 4920, 2430);
  node(147, 4920, 2520);
  node(148, 5320, 2190);
  node(149, 5200, 2340);
  node(13, 5600, 2190);
  node(150, 5880, 2190);
  node(151, 5760, 2340);
  node(152, 6160, 2190);
  node(153, 6040, 2340);
  node(154, 6040, 2430);
  node(155, 6040, 2520);
  node(156, 6040, 2610);
  node(157, 6440, 2190);
  node(158, 6320, 2340);
  node(159, 6320, 2430);
  node(160, 6320, 2520);
  node(161, 6720, 2190);
  node(162, 6600, 2340);
  node(163, 6600, 2430);
  node(164, 6600, 2520);
  node(165, 7000, 2190);
  node(166, 6880, 2340);
  node(167, 6880, 2430);
  node(168, 6880, 2520);
  node(169, 6880, 2610);
  node(170, 6880, 2700);
  node(171, 7280, 2190);
  node(172, 7160, 2340);
  node(173, 7160, 2430);
  node(174, 7160, 2520);
  node(175, 7560, 2190);
  node(176, 7440, 2340);
  node(177, 7440, 2430);
  node(178, 7440, 2520);
  node(179, 7840, 2190);
  node(180, 7720, 2340);
  node(181, 7720, 2430);
  node(182, 8120, 2190);
  node(183, 8000, 2340);
  node(184, 8000, 2430);
  node(185, 8000, 2520);
  node(186, 8000, 2610);
  node(187, 8000, 2700);
  node(15, 8400, 2190);
  node(188, 8680, 2190);
  node(189, 8560, 2340);
  node(190, 8560, 2430);
  node(191, 8560, 2520);
  node(192, 8960, 2190);
  node(193, 9240, 2190);
  node(194, 9120, 2340);
  node(195, 9520, 2190);
  node(196, 9400, 2340);
  node(197, 9400, 2430);
  node(198, 9400, 2520);
  node(199, 9400, 2610);
  node(200, 9400, 2700);
  node(201, 9800, 2190);
  node(202, 9680, 2340);
  node(203, 9680, 2430);
  node(204, 9680, 2520);
  node(205, 9680, 2610);
  node(206, 9680, 2700);
  node(207, 9680, 2790);
  node(208, 9680, 2880);
  node(16, 10080, 2190);
  node(209, 9960, 2340);
  node(210, 9960, 2430);
  node(211, 9960, 2520);
  node(212, 10360, 2190);
  node(213, 10240, 2340);
  node(214, 10240, 2430);
  node(215, 10640, 2190);
  node(216, 10520, 2340);
  node(217, 10520, 2430);
  node(218, 10920, 2190);
  node(219, 11200, 2190);
  node(220, 11080, 2340);
  node(221, 11480, 2190);
  node(222, 11360, 2340);
  node(223, 11760, 2190);
  node(224, 11640, 2340);
  node(225, 11640, 2430);
  node(226, 12040, 2190);
  node(227, 11920, 2340);
  node(228, 11920, 2430);
  node(229, 11920, 2520);
  node(230, 11920, 2610);
  node(231, 12320, 2190);
  node(232, 12200, 2340);
  node(233, 12200, 2430);
  node(234, 12200, 2520);
  node(235, 12200, 2610);
  node(236, 12200, 2700);
  node(237, 12200, 2790);
  node(238, 12200, 2880);
  node(239, 12200, 2970);
  node(240, 12200, 3060);
  node(241, 12600, 2190);
  node(242, 12880, 2190);
  node(243, 13160, 2190);
  node(244, 13440, 2190);
  node(245, 13320, 2340);
  node(246, 13320, 2430);
  node(247, 13320, 2520);
  node(248, 13320, 2610);
  node(249, 13320, 2700);
  node(250, 13720, 2190);
  node(251, 13600, 2340);
  node(252, 13600, 2430);
  node(253, 13600, 2520);
  node(254, 13600, 2610);
  node(255, 14000, 2190);
  node(256, 13880, 2340);
  node(257, 13880, 2430);
  node(258, 13880, 2520);
  node(259, 13880, 2610);
  node(260, 13880, 2700);
  node(261, 14280, 2190);
  node(17, 14560, 2190);
  node(262, 14840, 2190);
  node(876, 0, 11680);
  node(877, 288, 11736);
  node(878, 240, 11888);
  node(879, 576, 11792);
  node(880, 864, 11848);
  node(881, 816, 12000);
  node(882, 816, 12104);
  node(883, 1152, 11904);
  node(884, 1104, 12056);
  node(885, 1440, 11960);
  node(886, 1728, 12016);
  node(887, 1680, 12168);
  node(888, 1680, 12272);
  node(889, 2016, 12072);
  node(890, 1968, 12224);
  node(891, 2304, 12128);
  node(892, 2256, 12280);
  node(893, 2256, 12384);
  node(894, 2592, 12184);
  node(895, 2544, 12336);
  node(896, 2880, 12240);
  node(897, 2832, 12392);
  node(898, 3168, 12296);
  node(899, 3456, 12352);
  node(900, 3744, 12408);
  node(901, 4032, 12464);
  node(902, 4320, 12520);
  node(903, 4608, 12576);
  node(904, 4896, 12632);
  node(905, 4848, 12784);
  node(906, 4848, 12888);
  node(907, 4848, 12992);
  node(908, 4848, 13096);
  node(909, 4848, 13200);
  node(910, 4848, 13304);
  node(911, 4848, 13408);
  node(912, 4848, 13512);
  node(913, 4848, 13616);
  node(914, 4848, 13720);
  node(18, 5184, 12688);
  node(915, 5136, 12840);
  node(916, 5136, 12944);
  node(917, 5136, 13048);
  node(918, 5472, 12744);
  node(919, 5424, 12896);
  node(19, 6720, 3200);
  node(920, 7008, 3256);
  node(921, 6960, 3408);
  node(922, 7296, 3312);
  node(923, 7248, 3464);
  node(924, 7248, 3568);
  node(925, 7248, 3672);
  node(926, 7248, 3776);
  node(927, 7584, 3368);
  node(928, 7536, 3520);
  node(929, 7536, 3624);
  node(930, 7536, 3728);
  node(931, 7872, 3424);
  node(932, 7824, 3576);
  node(933, 7824, 3680);
  node(934, 7824, 3784);
  node(935, 8160, 3480);
  node(936, 8112, 3632);
  node(937, 8112, 3736);
  node(938, 8112, 3840);
  node(939, 8448, 3536);
  node(940, 8400, 3688);
  node(941, 8400, 3792);
  node(942, 8400, 3896);
  node(943, 8736, 3592);
  node(944, 8688, 3744);
  node(945, 8688, 3848);
  node(946, 8688, 3952);
  node(947, 8688, 4056);
  node(948, 9024, 3648);
  node(949, 8976, 3800);
  node(950, 8976, 3904);
  node(951, 9312, 3704);
  node(952, 9264, 3856);
  node(953, 9264, 3960);
  node(954, 9264, 4064);
  node(955, 9600, 3760);
  node(956, 9552, 3912);
  node(957, 9552, 4016);
  node(958, 9552, 4120);
  node(959, 9888, 3816);
  node(960, 9840, 3968);
  node(961, 9840, 4072);
  node(962, 9840, 4176);
  node(963, 10176, 3872);
  node(964, 10128, 4024);
  node(965, 10128, 4128);
  node(21, 6160, 4030);
  node(966, 6448, 4088);
  node(967, 6400, 4240);
  node(968, 6400, 4344);
  node(969, 6736, 4144);
  node(970, 7024, 4200);
  node(971, 6976, 4352);
  node(972, 7312, 4256);
  node(22, 7600, 4312);
  node(973, 7552, 4464);
  node(974, 7552, 4568);
  node(975, 7552, 4672);
  node(976, 7888, 4368);
  node(977, 7840, 4520);
  node(978, 7840, 4624);
  node(979, 7840, 4728);
  node(980, 8176, 4424);
  node(981, 8128, 4576);
  node(982, 8128, 4680);
  node(983, 8128, 4784);
  node(984, 8128, 4888);
  node(985, 8128, 4992);
  node(23, 6160, 4680);
  node(986, 6448, 4736);
  node(987, 6400, 4888);
  node(988, 6400, 4992);
  node(989, 6400, 5096);
  node(990, 6400, 5200);
  node(991, 6400, 5304);
  node(992, 6400, 5408);
  node(993, 6736, 4792);
  node(994, 6688, 4944);
  node(995, 6688, 5048);
  node(996, 6688, 5152);
  node(997, 6688, 5256);
  node(998, 6688, 5360);
  node(999, 6688, 5464);
  node(1000, 6688, 5568);
  node(1001, 7024, 4848);
  node(1002, 6976, 5000);
  node(1003, 6976, 5104);
  node(1004, 6976, 5208);
  node(1005, 6976, 5312);
  node(1006, 6976, 5416);
  node(1007, 6976, 5520);
  node(1008, 6976, 5624);
  node(1009, 6976, 5728);
  node(1010, 6976, 5832);
  node(1011, 6976, 5936);
  node(1012, 6976, 6040);
  node(1013, 7312, 4904);
  node(24, 7600, 4960);
  node(1014, 7888, 5016);
  node(1015, 7840, 5168);
  node(1016, 7840, 5272);
  node(1017, 8176, 5072);
  node(1018, 8464, 5128);
  node(1019, 8416, 5280);
  node(1020, 8416, 5384);
  node(1021, 8416, 5488);
  node(1022, 8416, 5592);
  node(1023, 8416, 5696);
  node(1024, 8752, 5184);
  node(1025, 8704, 5336);
  node(1026, 8704, 5440);
  node(1027, 9040, 5240);
  node(1028, 8992, 5392);
  node(1029, 8992, 5496);
  node(25, 9328, 5296);
  node(1030, 9616, 5352);
  node(1031, 9568, 5504);
  node(1032, 9568, 5608);
  node(1033, 9568, 5712);
  node(1034, 9568, 5816);
  node(1035, 9904, 5408);
  node(1036, 9856, 5560);
  node(1037, 9856, 5664);
  node(1038, 9856, 5768);
  node(1039, 9856, 5872);
  node(1040, 9856, 5976);
  node(1041, 9856, 6080);
  node(26, 3640, 6360);
  node(1042, 3928, 6416);
  node(1043, 3880, 6568);
  node(1044, 3880, 6672);
  node(1045, 4216, 6472);
  node(1046, 4168, 6624);
  node(1047, 4168, 6728);
  node(1048, 4168, 6832);
  node(1049, 4504, 6528);
  node(1050, 4456, 6680);
  node(1051, 4456, 6784);
  node(1052, 4456, 6888);
  node(1053, 4792, 6584);
  node(1054, 4744, 6736);
  node(1055, 4744, 6840);
  node(1056, 4744, 6944);
  node(1057, 4744, 7048);
  node(1058, 5080, 6640);
  node(1059, 5368, 6696);
  node(1060, 5656, 6752);
  node(1061, 5944, 6808);
  node(1062, 5896, 6960);
  node(1063, 5896, 7064);
  node(1064, 5896, 7168);
  node(1065, 5896, 7272);
  node(1066, 5896, 7376);
  node(1067, 6232, 6864);
  node(1068, 6184, 7016);
  node(1069, 6184, 7120);
  node(1070, 6184, 7224);
  node(1071, 6184, 7328);
  node(1072, 6520, 6920);
  node(1073, 6472, 7072);
  node(1074, 6472, 7176);
  node(1075, 6472, 7280);
  node(1076, 6472, 7384);
  node(1077, 6472, 7488);
  node(1078, 6808, 6976);
  node(27, 7096, 7032);
  node(1079, 7048, 7184);
  node(1080, 7048, 7288);
  node(1081, 7048, 7392);
  node(1082, 7384, 7088);
  node(1083, 7336, 7240);
  node(1084, 7336, 7344);
  node(28, 4480, 7010);
  node(1085, 4768, 7064);
  node(263, 0, 3200);
  node(264, 280, 3200);
  node(265, 160, 3350);
  node(267, 840, 3200);
  node(840, 1128, 3256);
  node(268, 720, 3350);
  node(269, 720, 3440);
  node(276, 1680, 3200);
  node(277, 1560, 3350);
  node(278, 1960, 3200);
  node(279, 1840, 3350);
  node(280, 1840, 3440);
  node(281, 2240, 3200);
  node(282, 2120, 3350);
  node(283, 2520, 3200);
  node(284, 2800, 3200);
  node(285, 3080, 3200);
  node(286, 3360, 3200);
  node(287, 3240, 3350);
  node(288, 3240, 3440);
  node(289, 3240, 3530);
  node(290, 3240, 3620);
  node(291, 3240, 3710);
  node(292, 3640, 3200);
  node(293, 3520, 3350);
  node(294, 3520, 3440);
  node(29, 6720, 7010);
  node(295, 4200, 3200);
  node(296, 4080, 3350);
  node(297, 4480, 3200);
  node(298, 4360, 3350);
  node(299, 4360, 3440);
  node(300, 4360, 3530);
  node(301, 4760, 3200);
  node(302, 4640, 3350);
  node(303, 4640, 3440);
  node(304, 4640, 3530);
  node(305, 4640, 3620);
  node(306, 5040, 3200);
  node(307, 4920, 3350);
  node(308, 4920, 3440);
  node(309, 5320, 3200);
  node(310, 5200, 3350);
  node(311, 5200, 3440);
  node(312, 5200, 3530);
  node(313, 5600, 3200);
  node(314, 5480, 3350);
  node(315, 5480, 3440);
  node(316, 5480, 3530);
  node(317, 5880, 3200);
  node(318, 5760, 3350);
  node(319, 5760, 3440);
  node(320, 5760, 3530);
  node(321, 5760, 3620);
  node(322, 5760, 3710);
  node(323, 5760, 3800);
  node(324, 5760, 3890);
  node(325, 6160, 3200);
  node(326, 6040, 3350);
  node(327, 6040, 3440);
  node(328, 6040, 3530);
  node(329, 6040, 3620);
  node(330, 6040, 3710);
  node(331, 6440, 3200);
  node(30, 7840, 7010);
  node(332, 7000, 3200);
  node(333, 0, 4030);
  node(334, 280, 4030);
  node(335, 160, 4180);
  node(337, 840, 4030);
  node(841, 1128, 4088);
  node(338, 720, 4180);
  node(339, 720, 4270);
  node(346, 1680, 4030);
  node(347, 1560, 4180);
  node(348, 1960, 4030);
  node(349, 1840, 4180);
  node(350, 1840, 4270);
  node(351, 2240, 4030);
  node(352, 2520, 4030);
  node(353, 2800, 4030);
  node(354, 3080, 4030);
  node(355, 2960, 4180);
  node(356, 2960, 4270);
  node(357, 2960, 4360);
  node(358, 2960, 4450);
  node(359, 2960, 4540);
  node(360, 3360, 4030);
  node(361, 3240, 4180);
  node(362, 3240, 4270);
  node(31, 3648, 4088);
  node(363, 3920, 4030);
  node(364, 3800, 4180);
  node(365, 4200, 4030);
  node(366, 4080, 4180);
  node(367, 4080, 4270);
  node(368, 4080, 4360);
  node(369, 4480, 4030);
  node(370, 4360, 4180);
  node(371, 4360, 4270);
  node(372, 4360, 4360);
  node(373, 4360, 4450);
  node(374, 4760, 4030);
  node(375, 4640, 4180);
  node(376, 4640, 4270);
  node(377, 5040, 4030);
  node(378, 4920, 4180);
  node(379, 4920, 4270);
  node(380, 4920, 4360);
  node(381, 5320, 4030);
  node(382, 5200, 4180);
  node(383, 5200, 4270);
  node(384, 5200, 4360);
  node(385, 5600, 4030);
  node(386, 5480, 4180);
  node(387, 5480, 4270);
  node(388, 5480, 4360);
  node(389, 5480, 4450);
  node(390, 5880, 4030);
  node(32, 6168, 4088);
  node(391, 6440, 4030);
  node(392, 0, 4680);
  node(393, 280, 4680);
  node(394, 160, 4830);
  node(396, 840, 4680);
  node(842, 1128, 4736);
  node(397, 720, 4830);
  node(398, 720, 4920);
  node(405, 1680, 4680);
  node(406, 1560, 4830);
  node(407, 1960, 4680);
  node(408, 1840, 4830);
  node(409, 1840, 4920);
  node(410, 2240, 4680);
  node(411, 2520, 4680);
  node(412, 2800, 4680);
  node(413, 3080, 4680);
  node(414, 2960, 4830);
  node(415, 2960, 4920);
  node(416, 2960, 5010);
  node(417, 2960, 5100);
  node(418, 2960, 5190);
  node(419, 3360, 4680);
  node(420, 3240, 4830);
  node(421, 3240, 4920);
  node(33, 15400, 8040);
  node(422, 3920, 4680);
  node(423, 3800, 4830);
  node(424, 4200, 4680);
  node(425, 4080, 4830);
  node(426, 4080, 4920);
  node(427, 4080, 5010);
  node(428, 4480, 4680);
  node(429, 4360, 4830);
  node(430, 4360, 4920);
  node(431, 4760, 4680);
  node(432, 4640, 4830);
  node(433, 4640, 4920);
  node(434, 4640, 5010);
  node(435, 5040, 4680);
  node(436, 4920, 4830);
  node(437, 4920, 4920);
  node(438, 4920, 5010);
  node(439, 5320, 4680);
  node(440, 5200, 4830);
  node(441, 5200, 4920);
  node(442, 5200, 5010);
  node(443, 5600, 4680);
  node(444, 5480, 4830);
  node(445, 5480, 4920);
  node(446, 5480, 5010);
  node(447, 5480, 5100);
  node(448, 5880, 4680);
  node(34, 6168, 4736);
  node(449, 6440, 4680);
  node(450, 0, 5330);
  node(451, 280, 5330);
  node(452, 160, 5480);
  node(453, 560, 5330);
  node(454, 440, 5480);
  node(455, 440, 5570);
  node(1086, 0, 14000);
  node(1087, 288, 14056);
  node(1088, 240, 14208);
  node(1089, 576, 14112);
  node(1090, 864, 14168);
  node(1091, 816, 14320);
  node(1092, 816, 14424);
  node(1093, 1152, 14224);
  node(1094, 1104, 14376);
  node(1095, 1440, 14280);
  node(1096, 1392, 14432);
  node(1097, 1392, 14536);
  node(1098, 1728, 14336);
  node(1099, 1680, 14488);
  node(1100, 2016, 14392);
  node(1101, 1968, 14544);
  node(1102, 1968, 14648);
  node(1103, 1968, 14752);
  node(1104, 1968, 14856);
  node(1105, 1968, 14960);
  node(1106, 1968, 15064);
  node(1107, 1968, 15168);
  node(1108, 1968, 15272);
  node(1109, 1968, 15376);
  node(1110, 2304, 14448);
  node(1111, 2256, 14600);
  node(1112, 2256, 14704);
  node(1113, 2592, 14504);
  node(1114, 2544, 14656);
  node(1115, 2880, 14560);
  node(1116, 3168, 14616);
  node(1117, 3456, 14672);
  node(1118, 3744, 14728);
  node(1119, 3696, 14880);
  node(1120, 3696, 14984);
  node(1121, 3696, 15088);
  node(1122, 3696, 15192);
  node(1123, 3696, 15296);
  node(1124, 4032, 14784);
  node(1125, 3984, 14936);
  node(1126, 3984, 15040);
  node(1127, 4320, 14840);
  node(1128, 4272, 14992);
  node(1129, 4272, 15096);
  node(1130, 4272, 15200);
  node(1131, 4608, 14896);
  node(1132, 4560, 15048);
  node(1133, 4560, 15152);
  node(1134, 4896, 14952);
  node(1135, 4848, 15104);
  node(1136, 4848, 15208);
  node(1137, 5184, 15008);
  node(1138, 5136, 15160);
  node(1139, 5136, 15264);
  node(1140, 5136, 15368);
  node(1141, 5136, 15472);
  node(1142, 5136, 15576);
  node(1143, 5136, 15680);
  node(1144, 5472, 15064);
  node(1145, 5424, 15216);
  node(1146, 5424, 15320);
  node(1147, 5424, 15424);
  node(1148, 5760, 15120);
  node(35, 6048, 15176);
  node(1149, 6336, 15232);
  node(1150, 6288, 15384);
  node(1151, 6288, 15488);
  node(1152, 6624, 15288);
  node(1153, 6576, 15440);
  node(1154, 6576, 15544);
  node(1155, 6576, 15648);
  node(1156, 6576, 15752);
  node(1157, 6576, 15856);
  node(1158, 6576, 15960);
  node(1159, 6576, 16064);
  node(1160, 6576, 16168);
  node(1161, 6576, 16272);
  node(1162, 6576, 16376);
  node(1163, 6576, 16480);
  node(1164, 6576, 16584);
  node(1165, 6576, 16688);
  node(1166, 6912, 15344);
  node(1167, 6864, 15496);
  node(1168, 6864, 15600);
  node(1169, 6864, 15704);
  node(36, 7200, 15400);
  node(1170, 7488, 15456);
  node(1171, 7440, 15608);
  node(1172, 7776, 15512);
  node(1173, 7728, 15664);
  node(1174, 7728, 15768);
  node(1175, 7728, 15872);
  node(1176, 8064, 15568);
  node(1177, 8016, 15720);
  node(1178, 8016, 15824);
  node(1179, 8016, 15928);
  node(1180, 8016, 16032);
  node(1181, 8352, 15624);
  node(1182, 8304, 15776);
  node(1183, 8304, 15880);
  node(1184, 8304, 15984);
  node(1185, 8304, 16088);
  node(1186, 8304, 16192);
  node(1187, 8640, 15680);
  node(1188, 8592, 15832);
  node(1189, 8592, 15936);
  node(1190, 8592, 16040);
  node(1191, 8592, 16144);
  node(1192, 8592, 16248);
  node(1193, 8928, 15736);
  node(37, 9216, 15792);
  node(1194, 9504, 15848);
  node(456, 0, 5710);
  node(457, 280, 5710);
  node(458, 160, 5860);
  node(460, 840, 5710);
  node(843, 1128, 5768);
  node(461, 720, 5860);
  node(462, 720, 5950);
  node(469, 1680, 5710);
  node(470, 1560, 5860);
  node(471, 1960, 5710);
  node(472, 1840, 5860);
  node(473, 1840, 5950);
  node(474, 2240, 5710);
  node(475, 2520, 5710);
  node(476, 2800, 5710);
  node(477, 3080, 5710);
  node(478, 2960, 5860);
  node(479, 2960, 5950);
  node(480, 2960, 6040);
  node(481, 2960, 6130);
  node(482, 2960, 6220);
  node(483, 3360, 5710);
  node(484, 3240, 5860);
  node(485, 3240, 5950);
  node(38, 3648, 5768);
  node(486, 3920, 5710);
  node(487, 3800, 5860);
  node(488, 4200, 5710);
  node(489, 4080, 5860);
  node(490, 4080, 5950);
  node(491, 4080, 6040);
  node(492, 4480, 5710);
  node(493, 4360, 5860);
  node(494, 4360, 5950);
  node(495, 4360, 6040);
  node(496, 4360, 6130);
  node(497, 4760, 5710);
  node(498, 4640, 5860);
  node(499, 4640, 5950);
  node(500, 5040, 5710);
  node(501, 4920, 5860);
  node(502, 4920, 5950);
  node(503, 4920, 6040);
  node(504, 5320, 5710);
  node(505, 5200, 5860);
  node(506, 5200, 5950);
  node(507, 5200, 6040);
  node(508, 5600, 5710);
  node(509, 5480, 5860);
  node(510, 5480, 5950);
  node(511, 5480, 6040);
  node(512, 5880, 5710);
  node(513, 5760, 5860);
  node(514, 5760, 5950);
  node(515, 5760, 6040);
  node(516, 5760, 6130);
  node(517, 5760, 6220);
  node(518, 6160, 5710);
  node(39, 6448, 5768);
  node(519, 6720, 5710);
  node(520, 0, 6360);
  node(521, 280, 6360);
  node(522, 160, 6510);
  node(524, 840, 6360);
  node(844, 1128, 6416);
  node(525, 720, 6510);
  node(526, 720, 6600);
  node(533, 1680, 6360);
  node(534, 1560, 6510);
  node(535, 1960, 6360);
  node(536, 1840, 6510);
  node(537, 1840, 6600);
  node(538, 2240, 6360);
  node(539, 2520, 6360);
  node(540, 2800, 6360);
  node(541, 3080, 6360);
  node(542, 2960, 6510);
  node(543, 2960, 6600);
  node(544, 2960, 6690);
  node(545, 2960, 6780);
  node(546, 2960, 6870);
  node(547, 3360, 6360);
  node(548, 3240, 6510);
  node(549, 3240, 6600);
  node(40, 3648, 6416);
  node(550, 3920, 6360);
  node(551, 3800, 6510);
  node(552, 4200, 6360);
  node(553, 4080, 6510);
  node(554, 4080, 6600);
  node(555, 4080, 6690);
  node(556, 4480, 6360);
  node(557, 4360, 6510);
  node(558, 4360, 6600);
  node(559, 4360, 6690);
  node(560, 4760, 6360);
  node(561, 4640, 6510);
  node(562, 4640, 6600);
  node(563, 4640, 6690);
  node(564, 4640, 6780);
  node(565, 4640, 6870);
  node(566, 5040, 6360);
  node(41, 5328, 6416);
  node(567, 5600, 6360);
  node(568, 0, 7010);
  node(569, 280, 7010);
  node(570, 160, 7160);
  node(572, 840, 7010);
  node(845, 1128, 7064);
  node(573, 720, 7160);
  node(574, 720, 7250);
  node(581, 1680, 7010);
  node(582, 1560, 7160);
  node(583, 1960, 7010);
  node(584, 1840, 7160);
  node(585, 1840, 7250);
  node(586, 2240, 7010);
  node(587, 2520, 7010);
  node(588, 2800, 7010);
  node(589, 3080, 7010);
  node(590, 2960, 7160);
  node(591, 3360, 7010);
  node(592, 3240, 7160);
  node(593, 3640, 7010);
  node(594, 3920, 7010);
  node(595, 3800, 7160);
  node(596, 3800, 7250);
  node(597, 3800, 7340);
  node(598, 3800, 7430);
  node(599, 3800, 7520);
  node(600, 4200, 7010);
  node(601, 4080, 7160);
  node(602, 4080, 7250);
  node(42, 4488, 7064);
  node(603, 4760, 7010);
  node(604, 4640, 7160);
  node(605, 5040, 7010);
  node(606, 4920, 7160);
  node(607, 4920, 7250);
  node(608, 4920, 7340);
  node(609, 5320, 7010);
  node(610, 5200, 7160);
  node(611, 5200, 7250);
  node(612, 5200, 7340);
  node(613, 5200, 7430);
  node(614, 5600, 7010);
  node(615, 5480, 7160);
  node(616, 5480, 7250);
  node(617, 5480, 7340);
  node(618, 5480, 7430);
  node(619, 5880, 7010);
  node(620, 5760, 7160);
  node(621, 5760, 7250);
  node(622, 5760, 7340);
  node(623, 6160, 7010);
  node(624, 6040, 7160);
  node(625, 6040, 7250);
  node(626, 6040, 7340);
  node(627, 6440, 7010);
  node(628, 6320, 7160);
  node(629, 6320, 7250);
  node(630, 6320, 7340);
  node(43, 6728, 7064);
  node(631, 7000, 7010);
  node(632, 6880, 7160);
  node(633, 6880, 7250);
  node(634, 6880, 7340);
  node(635, 7280, 7010);
  node(636, 7160, 7160);
  node(637, 7160, 7250);
  node(638, 7160, 7340);
  node(639, 7160, 7430);
  node(640, 7160, 7520);
  node(641, 7160, 7610);
  node(642, 7560, 7010);
  node(44, 7848, 7064);
  node(643, 8120, 7010);
  node(644, 0, 7750);
  node(645, 280, 7750);
  node(646, 160, 7900);
  node(647, 0, 8040);
  node(648, 280, 8040);
  node(45, 568, 8096);
  node(649, 440, 8190);
  node(650, 440, 8280);
  node(651, 840, 8040);
  node(652, 720, 8190);
  node(653, 1120, 8040);
  node(654, 1000, 8190);
  node(655, 1400, 8040);
  node(656, 1280, 8190);
  node(657, 1280, 8280);
  node(658, 1280, 8370);
  node(660, 1960, 8040);
  node(846, 2248, 8096);
  node(661, 1840, 8190);
  node(662, 1840, 8280);
  node(663, 1840, 8370);
  node(664, 1840, 8460);
  node(671, 2800, 8040);
  node(672, 2680, 8190);
  node(673, 3080, 8040);
  node(46, 3368, 8096);
  node(674, 3240, 8190);
  node(675, 3240, 8280);
  node(676, 3640, 8040);
  node(677, 3920, 8040);
  node(678, 3800, 8190);
  node(679, 3800, 8280);
  node(680, 3800, 8370);
  node(681, 4200, 8040);
  node(682, 4080, 8190);
  node(683, 4080, 8280);
  node(684, 4480, 8040);
  node(685, 4360, 8190);
  node(686, 4760, 8040);
  node(687, 5040, 8040);
  node(688, 4920, 8190);
  node(689, 4920, 8280);
  node(690, 5320, 8040);
  node(691, 5200, 8190);
  node(692, 5200, 8280);
  node(693, 5600, 8040);
  node(694, 5480, 8190);
  node(695, 5480, 8280);
  node(696, 5880, 8040);
  node(697, 5760, 8190);
  node(698, 5760, 8280);
  node(699, 6160, 8040);
  node(700, 6040, 8190);
  node(701, 6040, 8280);
  node(702, 6440, 8040);
  node(703, 6720, 8040);
  node(704, 7000, 8040);
  node(705, 7280, 8040);
  node(706, 7560, 8040);
  node(707, 7440, 8190);
  node(708, 7440, 8280);
  node(709, 7440, 8370);
  node(710, 7440, 8460);
  node(711, 7440, 8550);
  node(712, 7440, 8640);
  node(713, 7440, 8730);
  node(714, 7440, 8820);
  node(715, 7440, 8910);
  node(716, 7440, 9000);
  node(717, 7440, 9090);
  node(718, 7440, 9180);
  node(719, 7440, 9270);
  node(720, 7840, 8040);
  node(721, 7720, 8190);
  node(722, 7720, 8280);
  node(723, 8120, 8040);
  node(724, 8000, 8190);
  node(725, 8000, 8280);
  node(726, 8000, 8370);
  node(727, 8400, 8040);
  node(728, 8280, 8190);
  node(729, 8280, 8280);
  node(730, 8680, 8040);
  node(731, 8560, 8190);
  node(732, 8560, 8280);
  node(733, 8960, 8040);
  node(734, 8840, 8190);
  node(735, 8840, 8280);
  node(736, 9240, 8040);
  node(737, 9120, 8190);
  node(738, 9120, 8280);
  node(739, 9120, 8370);
  node(740, 9120, 8460);
  node(741, 9520, 8040);
  node(742, 9400, 8190);
  node(743, 9400, 8280);
  node(744, 9400, 8370);
  node(745, 9400, 8460);
  node(746, 9800, 8040);
  node(747, 9680, 8190);
  node(748, 9680, 8280);
  node(749, 9680, 8370);
  node(750, 9680, 8460);
  node(751, 9680, 8550);
  node(752, 9680, 8640);
  node(753, 9680, 8730);
  node(754, 9680, 8820);
  node(755, 9680, 8910);
  node(756, 9680, 9000);
  node(757, 9680, 9090);
  node(758, 9680, 9180);
  node(759, 10080, 8040);
  node(760, 10360, 8040);
  node(761, 10240, 8190);
  node(763, 10920, 8040);
  node(847, 11208, 8096);
  node(764, 10800, 8190);
  node(765, 10800, 8280);
  node(772, 11760, 8040);
  node(773, 11640, 8190);
  node(774, 12040, 8040);
  node(775, 11920, 8190);
  node(776, 11920, 8280);
  node(777, 12320, 8040);
  node(778, 12600, 8040);
  node(779, 12480, 8190);
  node(780, 12480, 8280);
  node(781, 12480, 8370);
  node(782, 12480, 8460);
  node(783, 12480, 8550);
  node(784, 12880, 8040);
  node(785, 12760, 8190);
  node(786, 12760, 8280);
  node(787, 12760, 8370);
  node(788, 12760, 8460);
  node(789, 12760, 8550);
  node(790, 12760, 8640);
  node(791, 12760, 8730);
  node(792, 13160, 8040);
  node(793, 13040, 8190);
  node(794, 13040, 8280);
  node(795, 13040, 8370);
  node(796, 13040, 8460);
  node(797, 13040, 8550);
  node(798, 13040, 8640);
  node(799, 13040, 8730);
  node(800, 13440, 8040);
  node(801, 13320, 8190);
  node(802, 13320, 8280);
  node(803, 13320, 8370);
  node(804, 13320, 8460);
  node(805, 13320, 8550);
  node(806, 13320, 8640);
  node(807, 13720, 8040);
  node(808, 14000, 8040);
  node(809, 13880, 8190);
  node(810, 14280, 8040);
  node(811, 14560, 8040);
  node(812, 14440, 8190);
  node(813, 14440, 8280);
  node(814, 14440, 8370);
  node(815, 14440, 8460);
  node(816, 14440, 8550);
  node(817, 14440, 8640);
  node(818, 14440, 8730);
  node(819, 14440, 8820);
  node(820, 14440, 8910);
  node(821, 14440, 9000);
  node(822, 14840, 8040);
  node(823, 14720, 8190);
  node(824, 14720, 8280);
  node(825, 14720, 8370);
  node(826, 14720, 8460);
  node(827, 14720, 8550);
  node(828, 14720, 8640);
  node(829, 14720, 8730);
  node(830, 14720, 8820);
  node(831, 14720, 8910);
  node(832, 14720, 9000);
  node(833, 14720, 9090);
  node(834, 14720, 9180);
  node(835, 15120, 8040);
  node(47, 15408, 8096);
  node(836, 15680, 8040);
  node(1195, 0, 16968);
  node(1196, 288, 17024);
  node(1197, 576, 17080);
  node(1198, 864, 17136);
  node(1199, 1152, 17192);
  node(1200, 1440, 17248);
  node(1201, 1728, 17304);
  node(1202, 2016, 17360);
  node(1203, 2304, 17416);
  node(1204, 2592, 17472);
  node(1205, 2880, 17528);
  node(1206, 3168, 17584);
  node(1207, 3456, 17640);
  node(1208, 3744, 17696);
  node(1209, 0, 17976);
  node(1210, 288, 18032);
  node(1211, 576, 18088);
  node(1212, 528, 18240);
  node(1213, 528, 18344);
  node(1214, 864, 18144);
  node(48, 1152, 18200);
  node(1215, 1104, 18352);
  node(1216, 1440, 18256);
  node(49, 1728, 18312);
  node(1217, 1680, 18464);
  node(1218, 2016, 18368);
  node(50, 2304, 18424);
  node(1219, 2256, 18576);
  node(1220, 2592, 18480);
  node(51, 2880, 18536);
  node(1221, 2832, 18688);
  node(1222, 3168, 18592);
  node(52, 3456, 18648);
  node(1223, 3408, 18800);
  node(1224, 3744, 18704);
  node(53, 4032, 18760);
  node(1225, 3984, 18912);
  node(1226, 4320, 18816);
  node(54, 4608, 18872);
  node(1227, 4560, 19024);
  node(1228, 4896, 18928);
  node(55, 5184, 18984);
  node(1229, 5136, 19136);
  node(1230, 5472, 19040);
  node(56, 5760, 19096);
  node(1231, 5712, 19248);
  node(1232, 6048, 19152);
  node(1233, 6336, 19208);
  node(1234, 6288, 19360);
  node(1235, 6288, 19464);
  node(1236, 6288, 19568);
  node(57, 6624, 19264);
  node(1237, 6576, 19416);
  node(1238, 6576, 19520);
  node(1239, 6576, 19624);
  node(1240, 6912, 19320);
  node(1241, 7200, 19376);
  node(1242, 7152, 19528);
  node(1243, 7152, 19632);
  node(1244, 7152, 19736);
  node(58, 7488, 19432);
  node(1245, 7440, 19584);
  node(1246, 7440, 19688);
  node(1247, 7440, 19792);
  node(1248, 7776, 19488);
  node(59, 8064, 19544);
  node(1249, 8016, 19696);
  node(1250, 8016, 19800);
  node(1251, 8016, 19904);
  node(1252, 8352, 19600);
  node(1253, 8640, 19656);
  node(1254, 8592, 19808);
  node(1255, 8592, 19912);
  node(1256, 8592, 20016);
  node(60, 8928, 19712);
  node(1257, 8880, 19864);
  node(1258, 8880, 19968);
  node(1259, 8880, 20072);
  node(1260, 9216, 19768);
  node(61, 9504, 19824);
  node(1261, 9456, 19976);
  node(1262, 9456, 20080);
  node(1263, 9456, 20184);
  node(1264, 9792, 19880);
  node(1265, 10080, 19936);
  next_id(1266);
}


}  // namespace kern_blueprint_adfee05bc649fab0

// ---------------------------------------------------------------------------
// The Blueprint of actor "settings_cpp"
namespace kern_blueprint_aff4d98a6dcb3e3f {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct settings_cpp : virtual kern::Library {
  // ---- functions ----
  void escape_into(Text& out, Text s = "") {
    Int c = 0;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(s))))); it10.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(s, n10.index))));
      if (logic_or(math_compare<"==">(kern::to<Float>(c), 34.0), math_compare<"==">(kern::to<Float>(c), 92.0))) {
        out = text_append(out, text_of_code(92), "");
        out = text_append(out, text_of_code(c), "");
      } else if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
        out = text_append(out, text_of_code(32), "");
      } else {
        out = text_append(out, text_of_code(c), "");
      }
    }
  }

  struct numResult { Text Result = ""; };
  KERN_PURE numResult num(Float v = 0.0) {
    Text b = "";
    b = "";
    b = text_format<"%g">(31, v);
    return {std::move(b)};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};

KERN_LAYOUT(settings_cpp) {
  node(11, 0, 0);
  node(10, 280, 0);
  node(12, 160, 150);
  node(13, 160, 240);
  node(14, 560, 0);
  node(15, 440, 150);
  node(16, 440, 240);
  node(17, 840, 0);
  node(18, 720, 150);
  node(19, 720, 240);
  node(20, 720, 330);
  node(21, 720, 420);
  node(22, 720, 510);
  node(23, 1120, 0);
  node(24, 1000, 150);
  node(25, 1000, 240);
  node(26, 1000, 330);
  node(27, 1400, 0);
  node(28, 1280, 150);
  node(29, 1280, 240);
  node(30, 1280, 330);
  node(31, 1280, 420);
  node(32, 1680, 0);
  node(33, 1560, 150);
  node(34, 1560, 240);
  node(35, 1560, 330);
  node(36, 1560, 420);
  node(37, 1960, 0);
  node(38, 1840, 150);
  node(39, 1840, 240);
  node(40, 1840, 330);
  node(41, 2240, 0);
  node(42, 2120, 150);
  node(43, 2120, 240);
  node(44, 2120, 330);
  node(45, 2120, 420);
  node(46, 0, 650);
  node(47, 280, 650);
  node(48, 560, 650);
  node(49, 440, 800);
  node(50, 840, 650);
  node(51, 720, 800);
  next_id(52);
}


}  // namespace kern_blueprint_aff4d98a6dcb3e3f

// ---------------------------------------------------------------------------
// The Blueprint of actor "jet_seats_cpp"
namespace kern_blueprint_9f819966c48b83f6 {

using jet_seats = ::kern_blueprint_9e16ff6b0cc7a88d::jet_seats;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct jet_seats_cpp : virtual kern::Library, virtual jet_seats {
  // ---- functions ----
  struct JetSeats_ResultNameResult { Text name = ""; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("ResultName") JetSeats_ResultNameResult JetSeats_ResultName(the_seats_Result_ result = the_seats_Result_::ok);

  struct JetSeats_GetResult { seats_Row row = seats_Row{253, 253, 253}; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Get") JetSeats_GetResult JetSeats_Get(List<seats_Row> rows = List<seats_Row>{}, Int owner = 0);

  struct JetSeats_SeatOfResult { Bool found = false; seats_Row row = seats_Row{253, 253, 253}; seats_Seat seat = seats_Seat::pilot; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("SeatOf") JetSeats_SeatOfResult JetSeats_SeatOf(List<seats_Row> rows = List<seats_Row>{}, Int player = 0);

  struct JetSeats_ClaimResult { List<seats_Row> rows = List<seats_Row>{}; the_seats_Result_ result = the_seats_Result_::ok; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Claim") JetSeats_ClaimResult JetSeats_Claim(List<seats_Row> rows = List<seats_Row>{}, Int owner = 0, Int player = 0, seats_Seat seat = seats_Seat::pilot);

  struct JetSeats_ReleaseResult { List<seats_Row> rows = List<seats_Row>{}; the_seats_Result_ result = the_seats_Result_::ok; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Release") JetSeats_ReleaseResult JetSeats_Release(List<seats_Row> rows = List<seats_Row>{}, Int owner = 0, Int player = 0, seats_Seat seat = seats_Seat::pilot);

  struct JetSeats_DropResult { List<seats_Row> rows = List<seats_Row>{}; Bool changed = false; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Drop") JetSeats_DropResult JetSeats_Drop(List<seats_Row> rows = List<seats_Row>{}, Int player = 0);

  struct JetSeats_ValidResult { Bool ok = false; };
  /// (of the Blueprint of the actor JetSeats, called on it)
  KERN_EXTERN_AS("Valid") JetSeats_ValidResult JetSeats_Valid(List<seats_Row> rows = List<seats_Row>{});

  struct bindResult { Bool Result = false; };
  KERN_PURE bindResult bind() {
    return {func_has_blueprint(actor_find("JetSeats"))};
  }

  struct result_nameResult { Text Result = ""; };
  result_nameResult result_name(the_seats_Result_ r = the_seats_Result_::ok) {
    Text name = "";
    name = "";
    func_call_on<"JetSeats_ResultName">(actor_find("JetSeats"), kern::out(n10.name), r);
    name = std::move(n10.name);
    return {std::move(name)};
  }

  struct Table_getResult { seats_Row Result = seats_Row{253, 253, 253}; };
  Table_getResult Table_get(const seats_Table& self, Int owner = 0) {
    seats_Row row = seats_Row{253, 253, 253};
    row = seats_Row{253, 253, 253};
    func_call_on<"JetSeats_Get">(actor_find("JetSeats"), kern::out(n11.row), self.rows_, owner);
    row = n11.row;
    return {row};
  }

  struct Table_seat_ofResult { Bool Result = false; };
  Table_seat_ofResult Table_seat_of(const seats_Table& self, seats_Row& row, seats_Seat& seat, Int player = 0) {
    Bool found = false;
    seats_Row r = seats_Row{253, 253, 253};
    seats_Seat s = seats_Seat::pilot;
    found = false;
    r = seats_Row{253, 253, 253};
    s = seats_Seat::pilot;
    func_call_on<"JetSeats_SeatOf">(actor_find("JetSeats"), kern::out(n12.found, n12.row, n12.seat), self.rows_, player);
    found = n12.found;
    r = n12.row;
    s = n12.seat;
    if (logic_not(found)) {
      return {false};
    }
    row = r;
    seat = s;
    return {true};
  }

  struct Table_claimResult { the_seats_Result_ Result = the_seats_Result_::ok; };
  Table_claimResult Table_claim(seats_Table& self, Int owner = 0, Int player = 0, seats_Seat seat = seats_Seat::pilot) {
    List<seats_Row> rows = List<seats_Row>{};
    the_seats_Result_ r = the_seats_Result_::ok;
    rows = List<seats_Row>{};
    r = the_seats_Result_::ok;
    func_call_on<"JetSeats_Claim">(actor_find("JetSeats"), kern::out(n13.rows, n13.result), self.rows_, owner, player, seat);
    rows = std::move(n13.rows);
    r = n13.result;
    if (enum_equal<the_seats_Result_>(r, the_seats_Result_::ok)) {
      self.rows_ = std::move(rows);
      self.revision_ = kern::to<Int>(kern::formula([](Float revision_) -> Float { return kern::math::wrap32(revision_ + 1.0); }, kern::to<Float>(self.revision_)));
    }
    return {r};
  }

  struct Table_releaseResult { the_seats_Result_ Result = the_seats_Result_::ok; };
  Table_releaseResult Table_release(seats_Table& self, Int owner = 0, Int player = 0, seats_Seat seat = seats_Seat::pilot) {
    List<seats_Row> rows = List<seats_Row>{};
    the_seats_Result_ r = the_seats_Result_::ok;
    rows = List<seats_Row>{};
    r = the_seats_Result_::ok;
    func_call_on<"JetSeats_Release">(actor_find("JetSeats"), kern::out(n14.rows, n14.result), self.rows_, owner, player, seat);
    rows = std::move(n14.rows);
    r = n14.result;
    if (enum_equal<the_seats_Result_>(r, the_seats_Result_::ok)) {
      self.rows_ = std::move(rows);
      self.revision_ = kern::to<Int>(kern::formula([](Float revision_) -> Float { return kern::math::wrap32(revision_ + 1.0); }, kern::to<Float>(self.revision_)));
    }
    return {r};
  }

  struct Table_dropResult { Bool Result = false; };
  Table_dropResult Table_drop(seats_Table& self, Int player = 0) {
    List<seats_Row> rows = List<seats_Row>{};
    Bool changed = false;
    rows = List<seats_Row>{};
    changed = false;
    func_call_on<"JetSeats_Drop">(actor_find("JetSeats"), kern::out(n15.rows, n15.changed), self.rows_, player);
    rows = std::move(n15.rows);
    changed = n15.changed;
    if (logic_not(changed)) {
      return {false};
    }
    self.rows_ = std::move(rows);
    self.revision_ = kern::to<Int>(kern::formula([](Float revision_) -> Float { return kern::math::wrap32(revision_ + 1.0); }, kern::to<Float>(self.revision_)));
    return {true};
  }

  struct Table_loadResult { Bool Result = false; };
  Table_loadResult Table_load(seats_Table& self, const List<seats_Row>& rows, Int revision = 0) {
    func_call_on<"JetSeats_Valid">(actor_find("JetSeats"), kern::out(n16.ok), rows);
    if (logic_not(n16.ok)) {
      return {false};
    }
    self.rows_ = rows;
    self.revision_ = revision;
    return {true};
  }

  // ---- what nodes give and keep ----
  struct { Text name = ""; } n10;
  struct { seats_Row row = seats_Row{253, 253, 253}; } n11;
  struct { Bool found = false; seats_Row row = seats_Row{253, 253, 253}; seats_Seat seat = seats_Seat::pilot; } n12;
  struct { List<seats_Row> rows = List<seats_Row>{}; the_seats_Result_ result = the_seats_Result_::ok; } n13;
  struct { List<seats_Row> rows = List<seats_Row>{}; the_seats_Result_ result = the_seats_Result_::ok; } n14;
  struct { List<seats_Row> rows = List<seats_Row>{}; Bool changed = false; } n15;
  struct { Bool ok = false; } n16;
};

KERN_LAYOUT(jet_seats_cpp) {
  node(17, 0, 0);
  node(18, 280, 0);
  node(19, 160, 150);
  node(20, 160, 240);
  node(21, 0, 380);
  node(22, 280, 380);
  node(10, 560, 380);
  node(23, 440, 530);
  node(24, 840, 380);
  node(25, 1120, 380);
  node(26, 1000, 530);
  node(27, 0, 670);
  node(28, 280, 670);
  node(11, 560, 670);
  node(29, 440, 820);
  node(30, 440, 910);
  node(31, 840, 670);
  node(32, 1120, 670);
  node(33, 1000, 820);
  node(34, 0, 1050);
  node(35, 280, 1050);
  node(36, 560, 1050);
  node(37, 840, 1050);
  node(12, 1120, 1050);
  node(38, 1000, 1200);
  node(39, 1000, 1290);
  node(40, 1400, 1050);
  node(41, 1680, 1050);
  node(42, 1960, 1050);
  node(43, 2240, 1050);
  node(44, 2120, 1200);
  node(45, 2120, 1290);
  node(46, 2520, 1050);
  node(47, 2800, 1050);
  node(48, 2680, 1200);
  node(49, 3080, 1050);
  node(50, 2960, 1200);
  node(51, 3360, 1050);
  node(52, 0, 1430);
  node(53, 280, 1430);
  node(54, 560, 1430);
  node(13, 840, 1430);
  node(55, 720, 1580);
  node(56, 720, 1670);
  node(57, 1120, 1430);
  node(58, 1400, 1430);
  node(59, 1680, 1430);
  node(60, 1560, 1580);
  node(61, 1560, 1670);
  node(62, 1960, 1430);
  node(63, 1840, 1580);
  node(64, 2240, 1430);
  node(65, 2120, 1580);
  node(66, 2120, 1670);
  node(67, 2520, 1430);
  node(68, 2400, 1580);
  node(69, 0, 1810);
  node(70, 280, 1810);
  node(71, 560, 1810);
  node(14, 840, 1810);
  node(72, 720, 1960);
  node(73, 720, 2050);
  node(74, 1120, 1810);
  node(75, 1400, 1810);
  node(76, 1680, 1810);
  node(77, 1560, 1960);
  node(78, 1560, 2050);
  node(79, 1960, 1810);
  node(80, 1840, 1960);
  node(81, 2240, 1810);
  node(82, 2120, 1960);
  node(83, 2120, 2050);
  node(84, 2520, 1810);
  node(85, 2400, 1960);
  node(86, 0, 2190);
  node(87, 280, 2190);
  node(88, 560, 2190);
  node(15, 840, 2190);
  node(89, 720, 2340);
  node(90, 720, 2430);
  node(91, 1120, 2190);
  node(92, 1400, 2190);
  node(93, 1680, 2190);
  node(94, 1560, 2340);
  node(95, 1560, 2430);
  node(96, 1960, 2190);
  node(97, 2240, 2190);
  node(98, 2120, 2340);
  node(99, 2520, 2190);
  node(100, 2400, 2340);
  node(101, 2400, 2430);
  node(102, 2800, 2190);
  node(103, 0, 2570);
  node(16, 280, 2570);
  node(104, 160, 2720);
  node(105, 160, 2810);
  node(106, 560, 2570);
  node(107, 440, 2720);
  node(108, 840, 2570);
  node(109, 1120, 2570);
  node(110, 1000, 2720);
  node(111, 1400, 2570);
  node(112, 1680, 2570);
  next_id(113);
}


}  // namespace kern_blueprint_9f819966c48b83f6

// ---------------------------------------------------------------------------
// The Blueprint of actor "wanted_cpp"
namespace kern_blueprint_b4e15abf1e3958e8 {

using blueprint_call = ::kern_blueprint_0eb7c15494c83360::blueprint_call;
using wanted = ::kern_blueprint_abcbfbad223ba2d6::wanted;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct wanted_cpp : virtual kern::Library, virtual blueprint_call, virtual wanted {
  // ---- functions ----
  struct Wanted_SeverityResult { Int severity = 0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Severity") Wanted_SeverityResult Wanted_Severity(Int crime = 0);

  struct Wanted_StarsForHeatResult { Int stars = 0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("StarsForHeat") Wanted_StarsForHeatResult Wanted_StarsForHeat(Float heat_2 = 0.0);

  struct Wanted_PoliceCountResult { Int count = 0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("PoliceCount") Wanted_PoliceCountResult Wanted_PoliceCount(Int stars_2 = 0);

  struct Wanted_DetectionRadiusResult { Float meters = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("DetectionRadius") Wanted_DetectionRadiusResult Wanted_DetectionRadius(Int stars_2 = 0);

  struct Wanted_EscapeMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("EscapeMs") Wanted_EscapeMsResult Wanted_EscapeMs(Int stars_2 = 0);

  struct Wanted_PoliceDamageResult { Float damage = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("PoliceDamage") Wanted_PoliceDamageResult Wanted_PoliceDamage(Int stars_2 = 0);

  struct Wanted_SpawnRadiusResult { Float meters = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("SpawnRadius") Wanted_SpawnRadiusResult Wanted_SpawnRadius(Int index = 0);

  struct Wanted_ArrivalDelayMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("ArrivalDelayMs") Wanted_ArrivalDelayMsResult Wanted_ArrivalDelayMs(Int stars_2 = 0);

  struct Wanted_EngageDelayMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("EngageDelayMs") Wanted_EngageDelayMsResult Wanted_EngageDelayMs(Int stars_2 = 0);

  struct Wanted_WaveBreakMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("WaveBreakMs") Wanted_WaveBreakMsResult Wanted_WaveBreakMs(Int stars_2 = 0);

  struct Wanted_FireRangeResult { Float meters = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("FireRange") Wanted_FireRangeResult Wanted_FireRange(Int stars_2 = 0);

  struct Wanted_FireIntervalMsResult { Float ms = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("FireIntervalMs") Wanted_FireIntervalMsResult Wanted_FireIntervalMs(Int stars_2 = 0);

  struct Wanted_HitChanceResult { Float chance = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("HitChance") Wanted_HitChanceResult Wanted_HitChance(Int stars_2 = 0, Float distance = 0.0, Float speed = 0.0);

  struct Wanted_EmpImpulseResult { Float impulse = 0.0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("EmpImpulse") Wanted_EmpImpulseResult Wanted_EmpImpulse(Float distance = 0.0);

  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Report") void Wanted_Report(Int crime = 0, Bool witnessed = false);

  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Update") void Wanted_Update(Float dt = 0.0, Bool seen = false);

  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Clear") void Wanted_Clear();

  struct Wanted_StarsResult { Int stars = 0; };
  /// (of the Blueprint of the actor Wanted, called on it)
  KERN_EXTERN_AS("Stars") Wanted_StarsResult Wanted_Stars();

  struct bindResult { Bool Result = false; };
  KERN_PURE bindResult bind() {
    return {func_has_blueprint(actor_find("Wanted"))};
  }

  struct max_starsResult { Int Result = 0; };
  KERN_PURE max_starsResult max_stars() {
    return {func_get_on<"MaxStars", Int>(actor_find("Wanted"))};
  }

  struct emp_rangeResult { Float Result = 0.0; };
  KERN_PURE emp_rangeResult emp_range() {
    return {func_get_on<"EmpRange", Float>(actor_find("Wanted"))};
  }

  struct emp_cooldown_msResult { Float Result = 0.0; };
  KERN_PURE emp_cooldown_msResult emp_cooldown_ms() {
    return {func_get_on<"EmpCooldownMs", Float>(actor_find("Wanted"))};
  }

  struct max_fire_rangeResult { Float Result = 0.0; };
  KERN_PURE max_fire_rangeResult max_fire_range() {
    return {func_get_on<"MaxFireRange", Float>(actor_find("Wanted"))};
  }

  struct severityResult { Int Result = 0; };
  severityResult severity(wanted_Crime c = wanted_Crime::person) {
    Int r = 0;
    r = 0;
    func_call_on<"Wanted_Severity">(actor_find("Wanted"), kern::out(n10.severity), enum_index<wanted_Crime>(c));
    r = n10.severity;
    return {r};
  }

  struct stars_for_heatResult { Int Result = 0; };
  stars_for_heatResult stars_for_heat(Float heat_2 = 0.0) {
    Int r = 0;
    r = 0;
    func_call_on<"Wanted_StarsForHeat">(actor_find("Wanted"), kern::out(n11.stars), blueprint_call::decimal(heat_2).Result);
    r = n11.stars;
    return {r};
  }

  struct police_countResult { Int Result = 0; };
  police_countResult police_count(Int stars_2 = 0) {
    Int r = 0;
    r = 0;
    func_call_on<"Wanted_PoliceCount">(actor_find("Wanted"), kern::out(n12.count), stars_2);
    r = n12.count;
    return {r};
  }

  struct detection_radiusResult { Float Result = 0.0; };
  detection_radiusResult detection_radius(Int stars_2 = 0) {
    func_call_on<"Wanted_DetectionRadius">(actor_find("Wanted"), kern::out(n13.meters), stars_2);
    return {n13.meters};
  }

  struct escape_msResult { Float Result = 0.0; };
  escape_msResult escape_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_EscapeMs">(actor_find("Wanted"), kern::out(n14.ms), stars_2);
    return {n14.ms};
  }

  struct police_damageResult { Float Result = 0.0; };
  police_damageResult police_damage(Int stars_2 = 0) {
    func_call_on<"Wanted_PoliceDamage">(actor_find("Wanted"), kern::out(n15.damage), stars_2);
    return {n15.damage};
  }

  struct spawn_radiusResult { Float Result = 0.0; };
  spawn_radiusResult spawn_radius(Int index = 0) {
    func_call_on<"Wanted_SpawnRadius">(actor_find("Wanted"), kern::out(n16.meters), index);
    return {n16.meters};
  }

  struct arrival_delay_msResult { Float Result = 0.0; };
  arrival_delay_msResult arrival_delay_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_ArrivalDelayMs">(actor_find("Wanted"), kern::out(n17.ms), stars_2);
    return {n17.ms};
  }

  struct engage_delay_msResult { Float Result = 0.0; };
  engage_delay_msResult engage_delay_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_EngageDelayMs">(actor_find("Wanted"), kern::out(n18.ms), stars_2);
    return {n18.ms};
  }

  struct wave_break_msResult { Float Result = 0.0; };
  wave_break_msResult wave_break_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_WaveBreakMs">(actor_find("Wanted"), kern::out(n19.ms), stars_2);
    return {n19.ms};
  }

  struct fire_rangeResult { Float Result = 0.0; };
  fire_rangeResult fire_range(Int stars_2 = 0) {
    func_call_on<"Wanted_FireRange">(actor_find("Wanted"), kern::out(n20.meters), stars_2);
    return {n20.meters};
  }

  struct fire_interval_msResult { Float Result = 0.0; };
  fire_interval_msResult fire_interval_ms(Int stars_2 = 0) {
    func_call_on<"Wanted_FireIntervalMs">(actor_find("Wanted"), kern::out(n21.ms), stars_2);
    return {n21.ms};
  }

  struct hit_chanceResult { Float Result = 0.0; };
  hit_chanceResult hit_chance(Int stars_2 = 0, Float distance_m = 0.0, Float player_speed = 0.0) {
    func_call_on<"Wanted_HitChance">(actor_find("Wanted"), kern::out(n22.chance), stars_2, blueprint_call::decimal(distance_m).Result, blueprint_call::decimal(player_speed).Result);
    return {n22.chance};
  }

  struct emp_impulseResult { Float Result = 0.0; };
  emp_impulseResult emp_impulse(Float distance_m = 0.0) {
    func_call_on<"Wanted_EmpImpulse">(actor_find("Wanted"), kern::out(n23.impulse), blueprint_call::decimal(distance_m).Result);
    return {n23.impulse};
  }

  void report(wanted_Crime c = wanted_Crime::person, Bool witnessed = false) {
    func_call_on<"Wanted_Report">(actor_find("Wanted"), kern::out(), enum_index<wanted_Crime>(c), witnessed);
  }

  void update(Float dt = 0.0, Bool seen = false) {
    func_call_on<"Wanted_Update">(actor_find("Wanted"), kern::out(), blueprint_call::decimal(dt).Result, seen);
  }

  void clear() {
    func_call_on<"Wanted_Clear">(actor_find("Wanted"), kern::out());
  }

  struct hidden_msResult { Float Result = 0.0; };
  KERN_PURE hidden_msResult hidden_ms() {
    return {func_get_on<"HiddenMs", Float>(actor_find("Wanted"))};
  }

  struct heatResult { Float Result = 0.0; };
  KERN_PURE heatResult heat() {
    return {func_get_on<"Heat", Float>(actor_find("Wanted"))};
  }

  struct starsResult { Int Result = 0; };
  starsResult stars() {
    Int r = 0;
    r = 0;
    func_call_on<"Wanted_Stars">(actor_find("Wanted"), kern::out(n27.stars));
    r = n27.stars;
    return {r};
  }

  // ---- what nodes give and keep ----
  struct { Int severity = 0; } n10;
  struct { Int stars = 0; } n11;
  struct { Int count = 0; } n12;
  struct { Float meters = 0.0; } n13;
  struct { Float ms = 0.0; } n14;
  struct { Float damage = 0.0; } n15;
  struct { Float meters = 0.0; } n16;
  struct { Float ms = 0.0; } n17;
  struct { Float ms = 0.0; } n18;
  struct { Float ms = 0.0; } n19;
  struct { Float meters = 0.0; } n20;
  struct { Float ms = 0.0; } n21;
  struct { Float chance = 0.0; } n22;
  struct { Float impulse = 0.0; } n23;
  struct { Int stars = 0; } n27;
};

KERN_LAYOUT(wanted_cpp) {
  node(28, 0, 0);
  node(29, 280, 0);
  node(30, 160, 150);
  node(31, 160, 240);
  node(32, 0, 380);
  node(33, 280, 380);
  node(34, 160, 530);
  node(35, 160, 620);
  node(36, 0, 760);
  node(37, 280, 760);
  node(38, 160, 910);
  node(39, 160, 1000);
  node(40, 0, 1140);
  node(41, 280, 1140);
  node(42, 160, 1290);
  node(43, 160, 1380);
  node(44, 0, 1520);
  node(45, 280, 1520);
  node(46, 160, 1670);
  node(47, 160, 1760);
  node(48, 0, 1900);
  node(49, 280, 1900);
  node(10, 560, 1900);
  node(50, 440, 2050);
  node(51, 440, 2140);
  node(52, 840, 1900);
  node(53, 1120, 1900);
  node(54, 1000, 2050);
  node(55, 0, 2280);
  node(56, 280, 2280);
  node(11, 560, 2280);
  node(57, 440, 2430);
  node(58, 440, 2520);
  node(59, 840, 2280);
  node(60, 1120, 2280);
  node(61, 1000, 2430);
  node(62, 0, 2660);
  node(63, 280, 2660);
  node(12, 560, 2660);
  node(64, 440, 2810);
  node(65, 840, 2660);
  node(66, 1120, 2660);
  node(67, 1000, 2810);
  node(68, 0, 2950);
  node(13, 280, 2950);
  node(69, 160, 3100);
  node(70, 560, 2950);
  node(71, 0, 3240);
  node(14, 280, 3240);
  node(72, 160, 3390);
  node(73, 560, 3240);
  node(74, 0, 3530);
  node(15, 280, 3530);
  node(75, 160, 3680);
  node(76, 560, 3530);
  node(77, 0, 3820);
  node(16, 280, 3820);
  node(78, 160, 3970);
  node(79, 560, 3820);
  node(80, 0, 4110);
  node(17, 280, 4110);
  node(81, 160, 4260);
  node(82, 560, 4110);
  node(83, 0, 4400);
  node(18, 280, 4400);
  node(84, 160, 4550);
  node(85, 560, 4400);
  node(86, 0, 4690);
  node(19, 280, 4690);
  node(87, 160, 4840);
  node(88, 560, 4690);
  node(89, 0, 4980);
  node(20, 280, 4980);
  node(90, 160, 5130);
  node(91, 560, 4980);
  node(92, 0, 5270);
  node(21, 280, 5270);
  node(93, 160, 5420);
  node(94, 560, 5270);
  node(95, 0, 5560);
  node(22, 280, 5560);
  node(96, 160, 5710);
  node(97, 160, 5800);
  node(98, 160, 5890);
  node(99, 560, 5560);
  node(100, 0, 6030);
  node(23, 280, 6030);
  node(101, 160, 6180);
  node(102, 160, 6270);
  node(103, 560, 6030);
  node(104, 0, 6410);
  node(105, 280, 6410);
  node(106, 160, 6560);
  node(107, 160, 6650);
  node(108, 0, 6790);
  node(109, 280, 6790);
  node(110, 160, 6940);
  node(111, 160, 7030);
  node(112, 0, 7170);
  node(113, 280, 7170);
  node(114, 160, 7320);
  node(115, 0, 7460);
  node(116, 280, 7460);
  node(117, 160, 7610);
  node(118, 160, 7700);
  node(119, 0, 7840);
  node(120, 280, 7840);
  node(121, 160, 7990);
  node(122, 160, 8080);
  node(123, 0, 8220);
  node(124, 280, 8220);
  node(27, 560, 8220);
  node(125, 440, 8370);
  node(126, 840, 8220);
  node(127, 1120, 8220);
  node(128, 1000, 8370);
  next_id(129);
}


}  // namespace kern_blueprint_b4e15abf1e3958e8

// ---------------------------------------------------------------------------
// The Blueprint of actor "wire"
namespace kern_blueprint_df1fc2456b534203 {

using engine_math_math = ::kern_blueprint_388516bead732334::engine_math_math;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct wire : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_STRUCT(wire_F32) {
    Float v = 0.0;
  };
  KERN_STRUCT(wire_F64) {
    Float v = 0.0;
  };
  KERN_STRUCT(wire_I16) {
    Int v = 0;
  };
  KERN_STRUCT(wire_I64) {
    Int64 v = Int64{0x0};
  };
  KERN_STRUCT(wire_I8) {
    Int v = 0;
  };
  KERN_STRUCT(wire_Q4) {
    Float x = 0.0;
    Float y = 0.0;
    Float z = 0.0;
    Float w = 0.0;
  };
  KERN_STRUCT(wire_Reader) {
    Int at = 0;
    Bool ok = true;
  };
  KERN_STRUCT(wire_U16) {
    Int v = 0;
  };
  KERN_STRUCT(wire_U32) {
    Int v = 0;
  };
  KERN_STRUCT(wire_U64) {
    Int64 v = Int64{0x0};
  };
  KERN_STRUCT(wire_V3) {
    Float x = 0.0;
    Float y = 0.0;
    Float z = 0.0;
  };

  // ---- functions ----
  struct f32_bytesResult { Fixed<Int, 4> Result = Fixed<Int, 4>{}; };
  /// The four bytes of a 32-bit float, the low one first.
  KERN_PURE f32_bytesResult f32_bytes(Float v = 0.0) {
    Fixed<Int, 4> bytes = Fixed<Int, 4>{};
    bytes = struct_bytes<wire_F32, "4: f32@0">(struct_make<wire_F32>(v));
    return {std::move(bytes)};
  }

  /// ---- writing: each number's bytes go on the end of the message (one growth of the list each, as inserting them in one go does)
  void put_u8(List<Int>& b, Int v = 0) {
    list_add(b, v);
  }

  void put_i8(List<Int>& b, Int v = 0) {
    Fixed<Int, 1> x = Fixed<Int, 1>{};
    Fixed<Int, 1> bytes = Fixed<Int, 1>{};
    Int place = 0;
    bytes = struct_bytes<wire_I8, "1: i8@0">(struct_make<wire_I8>(v));
    x = std::move(bytes);
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  void put_u16(List<Int>& b, Int v = 0) {
    Fixed<Int, 2> x = Fixed<Int, 2>{};
    Fixed<Int, 2> bytes = Fixed<Int, 2>{};
    Int place = 0;
    bytes = struct_bytes<wire_U16, "2: u16@0">(struct_make<wire_U16>(v));
    x = std::move(bytes);
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  void put_i16(List<Int>& b, Int v = 0) {
    Fixed<Int, 2> x = Fixed<Int, 2>{};
    Fixed<Int, 2> bytes = Fixed<Int, 2>{};
    Int place = 0;
    bytes = struct_bytes<wire_I16, "2: i16@0">(struct_make<wire_I16>(v));
    x = std::move(bytes);
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  void put_u32(List<Int>& b, Int v = 0) {
    Fixed<Int, 4> x = Fixed<Int, 4>{};
    Fixed<Int, 4> bytes = Fixed<Int, 4>{};
    Int place = 0;
    bytes = struct_bytes<wire_U32, "4: u32@0">(struct_make<wire_U32>(v));
    x = std::move(bytes);
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  void put_u64(List<Int>& b, Int64 v = Int64{0x0}) {
    Fixed<Int, 8> x = Fixed<Int, 8>{};
    Fixed<Int, 8> bytes = Fixed<Int, 8>{};
    Int place = 0;
    bytes = struct_bytes<wire_U64, "8: u64@0">(struct_make<wire_U64>(v));
    x = std::move(bytes);
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  void put_i64(List<Int>& b, Int64 v = Int64{0x0}) {
    Fixed<Int, 8> x = Fixed<Int, 8>{};
    Fixed<Int, 8> bytes = Fixed<Int, 8>{};
    Int place = 0;
    bytes = struct_bytes<wire_I64, "8: i64@0">(struct_make<wire_I64>(v));
    x = std::move(bytes);
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  void put_f32(List<Int>& b, Float v = 0.0) {
    Fixed<Int, 4> x = Fixed<Int, 4>{};
    Int place = 0;
    x = f32_bytes(v).Result;
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  void put_f64(List<Int>& b, Float v = 0.0) {
    Fixed<Int, 8> x = Fixed<Int, 8>{};
    Fixed<Int, 8> bytes = Fixed<Int, 8>{};
    Int place = 0;
    bytes = struct_bytes<wire_F64, "8: f64@0">(struct_make<wire_F64>(v));
    x = std::move(bytes);
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  void put_vec3(List<Int>& b, Vector v = Vector{0.0, 0.0, 0.0}) {
    Fixed<Int, 12> x = Fixed<Int, 12>{};
    Fixed<Int, 12> bytes = Fixed<Int, 12>{};
    Int place = 0;
    bytes = struct_bytes<wire_V3, "12: f32@0 f32@4 f32@8">(struct_make<wire_V3>(vector_break(v).x, vector_break(v).y, vector_break(v).z));
    x = std::move(bytes);
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  void put_quat(List<Int>& b, Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
    Fixed<Int, 16> x = Fixed<Int, 16>{};
    Fixed<Int, 16> bytes = Fixed<Int, 16>{};
    Int place = 0;
    bytes = struct_bytes<wire_Q4, "16: f32@0 f32@4 f32@8 f32@12">(struct_make<wire_Q4>(q.x, q.y, q.z, q.w));
    x = std::move(bytes);
    place = list_length<Int>(b);
    list_insert_list(b, place, std::move(x));
  }

  /// A list of bytes: its length (u16), then the bytes.
  void put_bytes(List<Int>& b, const List<Int>& v) {
    Int place = 0;
    put_u16(b, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 65535.0); }, kern::to<Float>(list_length<Int>(v)))));
    place = list_length<Int>(b);
    list_insert_list(b, place, v);
  }

  struct Reader_get_u8Result { Int Result = 0; };
  Reader_get_u8Result Reader_get_u8(wire_Reader& self, const List<Int>& b) {
    Int v = 0;
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 1.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    v = b[self.at];
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 1.0; }, kern::to<Float>(self.at)));
    return {v};
  }

  struct Reader_get_i8Result { Int Result = 0; };
  Reader_get_i8Result Reader_get_i8(wire_Reader& self, const List<Int>& b) {
    wire_I8 x = wire_I8{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 1.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_I8{0};
    x = struct_of_bytes<wire_I8, "1: i8@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 1.0; }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_u16Result { Int Result = 0; };
  Reader_get_u16Result Reader_get_u16(wire_Reader& self, const List<Int>& b) {
    wire_U16 x = wire_U16{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 2.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_U16{0};
    x = struct_of_bytes<wire_U16, "2: u16@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 2.0; }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_i16Result { Int Result = 0; };
  Reader_get_i16Result Reader_get_i16(wire_Reader& self, const List<Int>& b) {
    wire_I16 x = wire_I16{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 2.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_I16{0};
    x = struct_of_bytes<wire_I16, "2: i16@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 2.0; }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_u32Result { Int Result = 0; };
  Reader_get_u32Result Reader_get_u32(wire_Reader& self, const List<Int>& b) {
    wire_U32 x = wire_U32{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 4.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_U32{0};
    x = struct_of_bytes<wire_U32, "4: u32@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 4.0; }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_u64Result { Int64 Result = Int64{0x0}; };
  Reader_get_u64Result Reader_get_u64(wire_Reader& self, const List<Int>& b) {
    wire_U64 x = wire_U64{Int64{0x0}};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 8.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {Int64{0x0}};
    }
    x = wire_U64{Int64{0x0}};
    x = struct_of_bytes<wire_U64, "8: u64@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 8.0; }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_i64Result { Int64 Result = Int64{0x0}; };
  Reader_get_i64Result Reader_get_i64(wire_Reader& self, const List<Int>& b) {
    wire_I64 x = wire_I64{Int64{0x0}};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 8.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {Int64{0x0}};
    }
    x = wire_I64{Int64{0x0}};
    x = struct_of_bytes<wire_I64, "8: i64@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 8.0; }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_f32Result { Float Result = 0.0; };
  Reader_get_f32Result Reader_get_f32(wire_Reader& self, const List<Int>& b) {
    wire_F32 x = wire_F32{0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 4.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    x = wire_F32{0.0};
    x = struct_of_bytes<wire_F32, "4: f32@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 4.0; }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_f64Result { Float Result = 0.0; };
  Reader_get_f64Result Reader_get_f64(wire_Reader& self, const List<Int>& b) {
    wire_F64 x = wire_F64{0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 8.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    x = wire_F64{0.0};
    x = struct_of_bytes<wire_F64, "8: f64@0">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 8.0; }, kern::to<Float>(self.at)));
    return {x.v};
  }

  struct Reader_get_vec3Result { Vector Result = Vector{0.0, 0.0, 0.0}; };
  Reader_get_vec3Result Reader_get_vec3(wire_Reader& self, const List<Int>& b) {
    wire_V3 x = wire_V3{0.0, 0.0, 0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 12.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {Vector{0.0, 0.0, 0.0}};
    }
    x = wire_V3{0.0, 0.0, 0.0};
    x = struct_of_bytes<wire_V3, "12: f32@0 f32@4 f32@8">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 12.0; }, kern::to<Float>(self.at)));
    return {vector_make(x.x, x.y, x.z)};
  }

  struct Reader_get_quatResult { Quat Result = Quat{0.0, 0.0, 0.0, 1.0}; };
  Reader_get_quatResult Reader_get_quat(wire_Reader& self, const List<Int>& b) {
    wire_Q4 x = wire_Q4{0.0, 0.0, 0.0, 0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at) -> Float { return at + 16.0; }, kern::to<Float>(self.at)))))), 0.5)) {
      self.ok = false;
      return {struct_make<Quat>(0.0, 0.0, 0.0, 1.0)};
    }
    x = wire_Q4{0.0, 0.0, 0.0, 0.0};
    x = struct_of_bytes<wire_Q4, "16: f32@0 f32@4 f32@8 f32@12">(b, self.at);
    self.at = kern::to<Int>(kern::formula([](Float at) -> Float { return at + 16.0; }, kern::to<Float>(self.at)));
    return {struct_make<Quat>(x.x, x.y, x.z, x.w)};
  }

  struct Reader_get_bytesResult { List<Int> Result = List<Int>{}; };
  /// A list of bytes (put_bytes): empty where it is not all there.
  Reader_get_bytesResult Reader_get_bytes(wire_Reader& self, const List<Int>& b) {
    Int n = 0;
    List<Int> v = List<Int>{};
    List<Int> made = List<Int>{};
    Int first = 0;
    Int last = 0;
    n10 = Reader_get_u16(self, b);
    n = n10.Result;
    if (logic_or(logic_not(self.ok), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a < 0.0) != (a_2 < 0.0) ? a_2 < 0.0 : a < a_2; }, kern::to<Float>(list_length<Int>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at, Float n) -> Float { return at + n; }, kern::to<Float>(self.at), kern::to<Float>(n)))))), 0.5))) {
      self.ok = false;
      return {List<Int>{}};
    }
    made = List<Int>{};
    first = self.at;
    last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float at, Float n) -> Float { return at + n; }, kern::to<Float>(self.at), kern::to<Float>(n)))))));
    list_reserve(made, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(last), kern::to<Float>(first))))))));
    for (auto it11 = flow_for(n11, first, last); it11.next();) {
      list_add(made, list_get<Int>(b, n11.index));
    }
    v = std::move(made);
    self.at = kern::to<Int>(kern::formula([](Float at, Float n) -> Float { return at + n; }, kern::to<Float>(self.at), kern::to<Float>(n)));
    return {std::move(v)};
  }

  struct Reader_doneResult { Bool Result = false; };
  /// Every number was there, and the message has no more.
  KERN_PURE Reader_doneResult Reader_done(const wire_Reader& self, const List<Int>& b) {
    return {logic_and(self.ok, math_compare<"==">(kern::to<Float>(self.at), kern::to<Float>(list_length<Int>(b))))};
  }

  // ---- what nodes give and keep ----
  Reader_get_u16Result n10;
  struct : kern::Node<11> { Int index = 0; } n11;
};

KERN_LAYOUT(wire) {
  node(12, 0, 0);
  node(13, 280, 0);
  node(14, 160, 150);
  node(15, 160, 240);
  node(16, 560, 0);
  node(17, 440, 150);
  node(18, 0, 380);
  node(19, 280, 380);
  node(20, 0, 580);
  node(21, 280, 580);
  node(22, 160, 730);
  node(23, 160, 820);
  node(24, 560, 580);
  node(25, 440, 730);
  node(26, 840, 580);
  node(27, 720, 730);
  node(28, 720, 820);
  node(29, 1120, 580);
  node(30, 1000, 730);
  node(31, 1000, 820);
  node(32, 0, 960);
  node(33, 280, 960);
  node(34, 160, 1110);
  node(35, 160, 1200);
  node(36, 560, 960);
  node(37, 440, 1110);
  node(38, 840, 960);
  node(39, 720, 1110);
  node(40, 720, 1200);
  node(41, 1120, 960);
  node(42, 1000, 1110);
  node(43, 1000, 1200);
  node(44, 0, 1340);
  node(45, 280, 1340);
  node(46, 160, 1490);
  node(47, 160, 1580);
  node(48, 560, 1340);
  node(49, 440, 1490);
  node(50, 840, 1340);
  node(51, 720, 1490);
  node(52, 720, 1580);
  node(53, 1120, 1340);
  node(54, 1000, 1490);
  node(55, 1000, 1580);
  node(56, 0, 1720);
  node(57, 280, 1720);
  node(58, 160, 1870);
  node(59, 160, 1960);
  node(60, 560, 1720);
  node(61, 440, 1870);
  node(62, 840, 1720);
  node(63, 720, 1870);
  node(64, 720, 1960);
  node(65, 1120, 1720);
  node(66, 1000, 1870);
  node(67, 1000, 1960);
  node(68, 0, 2100);
  node(69, 280, 2100);
  node(70, 160, 2250);
  node(71, 160, 2340);
  node(72, 560, 2100);
  node(73, 440, 2250);
  node(74, 840, 2100);
  node(75, 720, 2250);
  node(76, 720, 2340);
  node(77, 1120, 2100);
  node(78, 1000, 2250);
  node(79, 1000, 2340);
  node(80, 0, 2480);
  node(81, 280, 2480);
  node(82, 160, 2630);
  node(83, 160, 2720);
  node(84, 560, 2480);
  node(85, 440, 2630);
  node(86, 840, 2480);
  node(87, 720, 2630);
  node(88, 720, 2720);
  node(89, 1120, 2480);
  node(90, 1000, 2630);
  node(91, 1000, 2720);
  node(92, 0, 2860);
  node(93, 280, 2860);
  node(94, 160, 3010);
  node(95, 560, 2860);
  node(96, 440, 3010);
  node(97, 440, 3100);
  node(98, 840, 2860);
  node(99, 720, 3010);
  node(100, 720, 3100);
  node(101, 0, 3240);
  node(102, 280, 3240);
  node(103, 160, 3390);
  node(104, 160, 3480);
  node(105, 560, 3240);
  node(106, 440, 3390);
  node(107, 840, 3240);
  node(108, 720, 3390);
  node(109, 720, 3480);
  node(110, 1120, 3240);
  node(111, 1000, 3390);
  node(112, 1000, 3480);
  node(113, 0, 3620);
  node(114, 280, 3620);
  node(115, 160, 3770);
  node(116, 160, 3860);
  node(117, 160, 3950);
  node(118, 160, 4040);
  node(119, 160, 4130);
  node(120, 560, 3620);
  node(121, 440, 3770);
  node(122, 840, 3620);
  node(123, 720, 3770);
  node(124, 720, 3860);
  node(125, 1120, 3620);
  node(126, 1000, 3770);
  node(127, 1000, 3860);
  node(128, 0, 4270);
  node(129, 280, 4270);
  node(130, 160, 4420);
  node(131, 160, 4510);
  node(132, 160, 4600);
  node(133, 160, 4690);
  node(134, 160, 4780);
  node(135, 160, 4870);
  node(136, 560, 4270);
  node(137, 440, 4420);
  node(138, 840, 4270);
  node(139, 720, 4420);
  node(140, 720, 4510);
  node(141, 1120, 4270);
  node(142, 1000, 4420);
  node(143, 1000, 4510);
  node(144, 0, 5010);
  node(145, 280, 5010);
  node(146, 160, 5160);
  node(147, 160, 5250);
  node(148, 160, 5340);
  node(149, 560, 5010);
  node(150, 440, 5160);
  node(151, 440, 5250);
  node(152, 840, 5010);
  node(153, 720, 5160);
  node(154, 720, 5250);
  node(155, 0, 5480);
  node(156, 280, 5480);
  node(157, 160, 5630);
  node(158, 160, 5720);
  node(159, 160, 5810);
  node(160, 160, 5900);
  node(161, 160, 5990);
  node(162, 160, 6080);
  node(163, 160, 6170);
  node(164, 560, 5480);
  node(165, 840, 5480);
  node(166, 1120, 5480);
  node(167, 1000, 5630);
  node(168, 1000, 5720);
  node(169, 1400, 5480);
  node(170, 1280, 5630);
  node(171, 1280, 5720);
  node(172, 1680, 5480);
  node(173, 1560, 5630);
  node(174, 0, 6310);
  node(175, 280, 6310);
  node(176, 160, 6460);
  node(177, 160, 6550);
  node(178, 160, 6640);
  node(179, 160, 6730);
  node(180, 160, 6820);
  node(181, 160, 6910);
  node(182, 160, 7000);
  node(183, 560, 6310);
  node(184, 840, 6310);
  node(185, 1120, 6310);
  node(186, 1400, 6310);
  node(187, 1280, 6460);
  node(188, 1280, 6550);
  node(189, 1280, 6640);
  node(190, 1680, 6310);
  node(191, 1560, 6460);
  node(192, 1560, 6550);
  node(193, 1960, 6310);
  node(194, 1840, 6460);
  node(195, 0, 7140);
  node(196, 280, 7140);
  node(197, 160, 7290);
  node(198, 160, 7380);
  node(199, 160, 7470);
  node(200, 160, 7560);
  node(201, 160, 7650);
  node(202, 160, 7740);
  node(203, 160, 7830);
  node(204, 560, 7140);
  node(205, 840, 7140);
  node(206, 1120, 7140);
  node(207, 1400, 7140);
  node(208, 1280, 7290);
  node(209, 1280, 7380);
  node(210, 1280, 7470);
  node(211, 1680, 7140);
  node(212, 1560, 7290);
  node(213, 1560, 7380);
  node(214, 1960, 7140);
  node(215, 1840, 7290);
  node(216, 0, 7970);
  node(217, 280, 7970);
  node(218, 160, 8120);
  node(219, 160, 8210);
  node(220, 160, 8300);
  node(221, 160, 8390);
  node(222, 160, 8480);
  node(223, 160, 8570);
  node(224, 160, 8660);
  node(225, 560, 7970);
  node(226, 840, 7970);
  node(227, 1120, 7970);
  node(228, 1400, 7970);
  node(229, 1280, 8120);
  node(230, 1280, 8210);
  node(231, 1280, 8300);
  node(232, 1680, 7970);
  node(233, 1560, 8120);
  node(234, 1560, 8210);
  node(235, 1960, 7970);
  node(236, 1840, 8120);
  node(237, 0, 8800);
  node(238, 280, 8800);
  node(239, 160, 8950);
  node(240, 160, 9040);
  node(241, 160, 9130);
  node(242, 160, 9220);
  node(243, 160, 9310);
  node(244, 160, 9400);
  node(245, 160, 9490);
  node(246, 560, 8800);
  node(247, 840, 8800);
  node(248, 1120, 8800);
  node(249, 1400, 8800);
  node(250, 1280, 8950);
  node(251, 1280, 9040);
  node(252, 1280, 9130);
  node(253, 1680, 8800);
  node(254, 1560, 8950);
  node(255, 1560, 9040);
  node(256, 1960, 8800);
  node(257, 1840, 8950);
  node(258, 0, 9630);
  node(259, 280, 9630);
  node(260, 160, 9780);
  node(261, 160, 9870);
  node(262, 160, 9960);
  node(263, 160, 10050);
  node(264, 160, 10140);
  node(265, 160, 10230);
  node(266, 160, 10320);
  node(267, 560, 9630);
  node(268, 840, 9630);
  node(269, 1120, 9630);
  node(270, 1400, 9630);
  node(271, 1280, 9780);
  node(272, 1280, 9870);
  node(273, 1280, 9960);
  node(274, 1680, 9630);
  node(275, 1560, 9780);
  node(276, 1560, 9870);
  node(277, 1960, 9630);
  node(278, 1840, 9780);
  node(279, 0, 10460);
  node(280, 280, 10460);
  node(281, 160, 10610);
  node(282, 160, 10700);
  node(283, 160, 10790);
  node(284, 160, 10880);
  node(285, 160, 10970);
  node(286, 160, 11060);
  node(287, 160, 11150);
  node(288, 560, 10460);
  node(289, 840, 10460);
  node(290, 1120, 10460);
  node(291, 1400, 10460);
  node(292, 1280, 10610);
  node(293, 1280, 10700);
  node(294, 1280, 10790);
  node(295, 1680, 10460);
  node(296, 1560, 10610);
  node(297, 1560, 10700);
  node(298, 1960, 10460);
  node(299, 1840, 10610);
  node(300, 0, 11290);
  node(301, 280, 11290);
  node(302, 160, 11440);
  node(303, 160, 11530);
  node(304, 160, 11620);
  node(305, 160, 11710);
  node(306, 160, 11800);
  node(307, 160, 11890);
  node(308, 160, 11980);
  node(309, 560, 11290);
  node(310, 840, 11290);
  node(311, 1120, 11290);
  node(312, 1400, 11290);
  node(313, 1280, 11440);
  node(314, 1280, 11530);
  node(315, 1280, 11620);
  node(316, 1680, 11290);
  node(317, 1560, 11440);
  node(318, 1560, 11530);
  node(319, 1960, 11290);
  node(320, 1840, 11440);
  node(321, 0, 12120);
  node(322, 280, 12120);
  node(323, 160, 12270);
  node(324, 160, 12360);
  node(325, 160, 12450);
  node(326, 160, 12540);
  node(327, 160, 12630);
  node(328, 160, 12720);
  node(329, 160, 12810);
  node(330, 560, 12120);
  node(331, 840, 12120);
  node(332, 1120, 12120);
  node(333, 1400, 12120);
  node(334, 1280, 12270);
  node(335, 1280, 12360);
  node(336, 1280, 12450);
  node(337, 1680, 12120);
  node(338, 1560, 12270);
  node(339, 1560, 12360);
  node(340, 1960, 12120);
  node(341, 1840, 12270);
  node(342, 0, 12950);
  node(343, 280, 12950);
  node(344, 160, 13100);
  node(345, 160, 13190);
  node(346, 160, 13280);
  node(347, 160, 13370);
  node(348, 160, 13460);
  node(349, 160, 13550);
  node(350, 160, 13640);
  node(351, 560, 12950);
  node(352, 840, 12950);
  node(353, 1120, 12950);
  node(354, 1400, 12950);
  node(355, 1280, 13100);
  node(356, 1280, 13190);
  node(357, 1280, 13280);
  node(358, 1680, 12950);
  node(359, 1560, 13100);
  node(360, 1560, 13190);
  node(361, 1960, 12950);
  node(362, 1840, 13100);
  node(363, 1840, 13190);
  node(364, 1840, 13280);
  node(365, 1840, 13370);
  node(366, 0, 13780);
  node(367, 280, 13780);
  node(368, 160, 13930);
  node(369, 160, 14020);
  node(370, 160, 14110);
  node(371, 160, 14200);
  node(372, 160, 14290);
  node(373, 160, 14380);
  node(374, 160, 14470);
  node(375, 560, 13780);
  node(376, 840, 13780);
  node(377, 720, 13930);
  node(378, 1120, 13780);
  node(379, 1400, 13780);
  node(380, 1280, 13930);
  node(381, 1280, 14020);
  node(382, 1280, 14110);
  node(383, 1680, 13780);
  node(384, 1560, 13930);
  node(385, 1560, 14020);
  node(386, 1960, 13780);
  node(387, 1840, 13930);
  node(388, 1840, 14020);
  node(389, 1840, 14110);
  node(390, 1840, 14200);
  node(391, 1840, 14290);
  node(392, 0, 14610);
  node(10, 280, 14610);
  node(393, 560, 14610);
  node(394, 840, 14610);
  node(395, 720, 14760);
  node(396, 720, 14850);
  node(397, 720, 14940);
  node(398, 720, 15030);
  node(399, 720, 15120);
  node(400, 720, 15210);
  node(401, 720, 15300);
  node(402, 720, 15390);
  node(403, 720, 15480);
  node(404, 720, 15570);
  node(405, 720, 15660);
  node(406, 1120, 14610);
  node(407, 1400, 14610);
  node(408, 1680, 14610);
  node(409, 1960, 14610);
  node(410, 1840, 14760);
  node(411, 2240, 14610);
  node(412, 2120, 14760);
  node(413, 2120, 14850);
  node(414, 2120, 14940);
  node(415, 2120, 15030);
  node(416, 2120, 15120);
  node(417, 2520, 14610);
  node(418, 2400, 14760);
  node(419, 2400, 14850);
  node(420, 2400, 14940);
  node(421, 2400, 15030);
  node(422, 2400, 15120);
  node(11, 2800, 14610);
  node(423, 2680, 14760);
  node(424, 2680, 14850);
  node(425, 3080, 14610);
  node(426, 2960, 14760);
  node(427, 2960, 14850);
  node(428, 3360, 14610);
  node(429, 3240, 14760);
  node(430, 3640, 14610);
  node(431, 3520, 14760);
  node(432, 3520, 14850);
  node(433, 3520, 14940);
  node(434, 3920, 14610);
  node(435, 3800, 14760);
  node(436, 0, 15800);
  node(437, 280, 15800);
  node(438, 160, 15950);
  node(439, 160, 16040);
  node(440, 160, 16130);
  node(441, 160, 16220);
  node(442, 160, 16310);
  node(443, 160, 16400);
  next_id(444);
}


}  // namespace kern_blueprint_df1fc2456b534203

namespace blueprints {
using blueprint_call = ::kern_blueprint_0eb7c15494c83360::blueprint_call;
using shapes = ::kern_blueprint_6025b4a819f918d1::shapes;
using engine_geo_roads = ::kern_blueprint_c57f7a6bcfc775fb::engine_geo_roads;
using engine_math_math = ::kern_blueprint_388516bead732334::engine_math_math;
using jet_seats = ::kern_blueprint_9e16ff6b0cc7a88d::jet_seats;
using person_hit = ::kern_blueprint_80253b21ddcc8149::person_hit;
using qr_code = ::kern_blueprint_f1248d4ad2e4cc95::qr_code;
using engine_physics_physics = ::kern_blueprint_ba8d70e522c7d944::engine_physics_physics;
using common = ::kern_blueprint_416f27166223668d::common;
using lamps = ::kern_blueprint_315081d4e46950aa::lamps;
using mvt = ::kern_blueprint_5eb1d61c7210d38b::mvt;
using road_graph = ::kern_blueprint_3a4ae1d57fed8576::road_graph;
using perf = ::kern_blueprint_1b97e0faae1f6c60::perf;
using tiles = ::kern_blueprint_508646ea29b97f65::tiles;
using terrain = ::kern_blueprint_9a4bfed9ca9dc51a::terrain;
using city_stream = ::kern_blueprint_0265b76c2a3d1e83::city_stream;
using wanted = ::kern_blueprint_abcbfbad223ba2d6::wanted;
using nostr_cpp = ::kern_blueprint_6cb2cf0da8e6380e::nostr_cpp;
using qr_code_cpp = ::kern_blueprint_fe59a0c6af5496ca::qr_code_cpp;
using lamps_cpp = ::kern_blueprint_4ba64ee8b255fbb9::lamps_cpp;
using mvt_cpp = ::kern_blueprint_02f7c0a1316e6ec3::mvt_cpp;
using road_graph_cpp = ::kern_blueprint_280419937f3ad3b5::road_graph_cpp;
using perf_cpp = ::kern_blueprint_c4a27d6984fae299::perf_cpp;
using combat_audio = ::kern_blueprint_84d2fe0d34f70805::combat_audio;
using combat_audio_cpp = ::kern_blueprint_adfee05bc649fab0::combat_audio_cpp;
using settings_cpp = ::kern_blueprint_aff4d98a6dcb3e3f::settings_cpp;
using jet_seats_cpp = ::kern_blueprint_9f819966c48b83f6::jet_seats_cpp;
using wanted_cpp = ::kern_blueprint_b4e15abf1e3958e8::wanted_cpp;
using wire = ::kern_blueprint_df1fc2456b534203::wire;
}  // namespace blueprints
