// The classes of the engine's own libraries (kern/logic/engine_libraries.h), once for every program: written by
// tools/blueprint_cpp --engine-classes (tools/engine_api.py --libraries) - not edited by hand. The classes of a
// level's Blueprints include this header and name these classes; none writes them again.
#pragma once

#include "kern/logic/blueprint.h"
#include "kern/logic/blueprint_class.h"

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_math_math"
namespace kern_blueprint_64c1acf056c7521a {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_math_math : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(Aabb) {
    Vector the_min = Vector{1e+30, 1e+30, 1e+30};
    Vector the_max = Vector{-1e+30, -1e+30, -1e+30};
  };
  KERN_STRUCT(Mat4) {
    Fixed<Float, 16> m = Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  };
  KERN_STRUCT(Quat) {
    Float x = 0.0;
    Float y = 0.0;
    Float z = 0.0;
    Float w = 1.0;
  };
  KERN_STRUCT(Sphere) {
    Vector center = Vector{0.0, 0.0, 0.0};
    Float radius = -1.0;
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

  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct dotResult { Float Result = 0.0; };
  KERN_PURE dotResult dot(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return x * x_2 + y * y_2; }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y)};
  }

  struct crossResult { Float Result = 0.0; };
  KERN_PURE crossResult cross(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return x * y - y_2 * x_2; }, vector_break(a).x, vector_break(b).y, vector_break(a).y, vector_break(b).x)};
  }

  struct lengthResult { Float Result = 0.0; };
  KERN_PURE lengthResult length(Vector a = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float a) -> Float { return kern::math::sqrt(a); }, vector_dot(a, a))};
  }

  struct Vec3_loadResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Vec3_loadResult Vec3_load(const List<Float>& p, Int p_at = 0) {
    return {vector_make(p[p_at], p[kern::to<Int>(kern::formula([](Int a) -> Int { return a + 1.0; }, p_at))], p[kern::to<Int>(kern::formula([](Int a) -> Int { return a + 2.0; }, p_at))])};
  }

  struct dot_2Result { Float Result = 0.0; };
  KERN_PURE dot_2Result dot_2(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float x_2, Float y, Float y_2, Float z, Float z_2) -> Float { return x * x_2 + y * y_2 + z * z_2; }, vector_break(a).x, vector_break(b).x, vector_break(a).y, vector_break(b).y, vector_break(a).z, vector_break(b).z)};
  }

  struct cross_2Result { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE cross_2Result cross_2(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {vector_make(kern::formula([](Float y, Float z, Float z_2, Float y_2) -> Float { return y * z - z_2 * y_2; }, vector_break(a).y, vector_break(b).z, vector_break(a).z, vector_break(b).y), kern::formula([](Float z, Float x, Float x_2, Float z_2) -> Float { return z * x - x_2 * z_2; }, vector_break(a).z, vector_break(b).x, vector_break(a).x, vector_break(b).z), kern::formula([](Float x, Float y, Float y_2, Float x_2) -> Float { return x * y - y_2 * x_2; }, vector_break(a).x, vector_break(b).y, vector_break(a).y, vector_break(b).x))};
  }

  struct length_2Result { Float Result = 0.0; };
  KERN_PURE length_2Result length_2(Vector a = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float a) -> Float { return kern::math::sqrt(a); }, vector_dot(a, a))};
  }

  struct length_sqResult { Float Result = 0.0; };
  KERN_PURE length_sqResult length_sq(Vector a = Vector{0.0, 0.0, 0.0}) {
    return {vector_dot(a, a)};
  }

  struct distanceResult { Float Result = 0.0; };
  KERN_PURE distanceResult distance(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {vector_length(vector_sub(a, b))};
  }

  struct distance_sqResult { Float Result = 0.0; };
  KERN_PURE distance_sqResult distance_sq(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {vector_dot(vector_sub(a, b), vector_sub(a, b))};
  }

  struct finiteResult { Bool Result = false; };
  KERN_PURE finiteResult finite(Vector a = Vector{0.0, 0.0, 0.0}) {
    return {logic_and(logic_and(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(a).x), 0.0), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(a).y), 0.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(a).z), 0.0))};
  }

  struct the_lerpResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE the_lerpResult the_lerp(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Float t = 0.0) {
    return {vector_add(a, vector_scale(vector_sub(b, a), t))};
  }

  struct vminResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE vminResult vmin(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {vector_make(kern::formula([](Float x, Float x_2) -> Float { return kern::math::min(x, x_2); }, vector_break(a).x, vector_break(b).x), kern::formula([](Float y, Float y_2) -> Float { return kern::math::min(y, y_2); }, vector_break(a).y, vector_break(b).y), kern::formula([](Float z, Float z_2) -> Float { return kern::math::min(z, z_2); }, vector_break(a).z, vector_break(b).z))};
  }

  struct vmaxResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE vmaxResult vmax(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    return {vector_make(kern::formula([](Float x, Float x_2) -> Float { return kern::math::max(x, x_2); }, vector_break(a).x, vector_break(b).x), kern::formula([](Float y, Float y_2) -> Float { return kern::math::max(y, y_2); }, vector_break(a).y, vector_break(b).y), kern::formula([](Float z, Float z_2) -> Float { return kern::math::max(z, z_2); }, vector_break(a).z, vector_break(b).z))};
  }

  struct normalizeResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// Unit vector, or `fallback` for (near) zero input.
  KERN_PURE normalizeResult normalize(Vector a = Vector{0.0, 0.0, 0.0}, Vector fallback = Vector{0.0, 0.0, 0.0}) {
    Float n = 0.0;
    n = vector_length(a);
    return {logic_select<Vector>(math_compare<">">(n, 1e-09), vector_make(kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, vector_break(a).x, n), kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, vector_break(a).y, n), kern::formula([](Float a, Float b) -> Float { return kern::math::div(a, kern::math::single(b)); }, vector_break(a).z, n)), fallback)};
  }

  struct hex_colorResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// 0xRRGGBB to 0..1 RGB (no colour space change).
  KERN_PURE hex_colorResult hex_color(U32 h = 0) {
    return {vector_make(kern::formula([](Int h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, h), kern::formula([](Int h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, h), kern::formula([](Int h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, h))};
  }

  struct Vec4_newResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE Vec4_newResult Vec4_new(Float x_ = 0.0, Float y_ = 0.0, Float z_ = 0.0, Float w_ = 0.0) {
    Vec4 self = Vec4{0.0, 0.0, 0.0, 0.0};
    self.x = x_;
    self.y = y_;
    self.z = z_;
    self.w = w_;
    return {self};
  }

  struct Vec4_new_2Result { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE Vec4_new_2Result Vec4_new_2(Vector v = Vector{0.0, 0.0, 0.0}, Float w_ = 0.0) {
    Vec4 self = Vec4{0.0, 0.0, 0.0, 0.0};
    self.x = vector_break(v).x;
    self.y = vector_break(v).y;
    self.z = vector_break(v).z;
    self.w = w_;
    return {self};
  }

  struct Vec4_xyzResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Vec4_xyzResult Vec4_xyz(const Vec4& self) {
    return {vector_make(self.x, self.y, self.z)};
  }

  struct Vec4_equalResult { Bool Result = false; };
  KERN_PURE Vec4_equalResult Vec4_equal(const Vec4& self, const Vec4& o) {
    return {logic_and(logic_and(logic_and(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return x == x_2; }, self.x, o.x), 0.5), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return y == y_2; }, self.y, o.y), 0.5)), math_compare<">">(kern::formula([](Float z, Float z_2) -> Float { return z == z_2; }, self.z, o.z), 0.5)), math_compare<">">(kern::formula([](Float w, Float w_2) -> Float { return w == w_2; }, self.w, o.w), 0.5))};
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

  struct Mat4_identityResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE Mat4_identityResult Mat4_identity() {
    return {struct_make<Mat4>(Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0})};
  }

  struct Mat4_loadResult { Mat4 Result = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE Mat4_loadResult Mat4_load(const List<Float>& p, Int p_at = 0) {
    Mat4 r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    List<Float> a = List<Float>{};
    r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a = p;
    list_copy_bytes<"f32", "f32">(r.m, 0, std::move(a), p_at, 64);
    return {std::move(r)};
  }

  struct Mat4_at_2Result { Float Result = 0.0; };
  KERN_PURE Mat4_at_2Result Mat4_at_2(const Mat4& self, I32 i = 0) {
    return {self.m[i]};
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
    struct : kern::Node<10> { Int index = 0; } n10;
    struct : kern::Node<11> { Int index = 0; } n11;
    r = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    for (auto it10 = flow_for(n10, 0, 3); it10.next();) {
      b0 = b.m[kern::to<Int>(kern::formula([](Int index) -> Int { return 4.0 * index; }, n10.index))];
      b1 = b.m[kern::to<Int>(kern::formula([](Int index) -> Int { return 4.0 * index + 1.0; }, n10.index))];
      b2 = b.m[kern::to<Int>(kern::formula([](Int index) -> Int { return 4.0 * index + 2.0; }, n10.index))];
      b3 = b.m[kern::to<Int>(kern::formula([](Int index) -> Int { return 4.0 * index + 3.0; }, n10.index))];
      for (auto it11 = flow_for(n11, 0, 3); it11.next();) {
        r.m[kern::to<Int>(kern::formula([](Int index, Int index_2) -> Int { return 4.0 * index + index_2; }, n10.index, n11.index))] = kern::formula([](Float a, Float b0, Float a_2, Float b1, Float a_3, Float b2, Float a_4, Float b3) -> Float { return a * b0 + a_2 * b1 + a_3 * b2 + a_4 * b3; }, self.m[n11.index], b0, self.m[kern::to<Int>(kern::formula([](Int index) -> Int { return 4.0 + index; }, n11.index))], b1, self.m[kern::to<Int>(kern::formula([](Int index) -> Int { return 8.0 + index; }, n11.index))], b2, self.m[kern::to<Int>(kern::formula([](Int index) -> Int { return 12.0 + index; }, n11.index))], b3);
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
    Mat4_look_atResult n12;
    ok = false;
    n12 = Mat4_look_at(ok, eye, target, up);
    return {std::move(n12.Result)};
  }

  struct Sphere_validResult { Bool Result = false; };
  KERN_PURE Sphere_validResult Sphere_valid(const Sphere& self) {
    return {math_compare<">=">(self.radius, 0.0)};
  }

  struct Aabb_validResult { Bool Result = false; };
  KERN_PURE Aabb_validResult Aabb_valid(const Aabb& self) {
    return {math_compare<"<=">(vector_break(self.the_min).x, vector_break(self.the_max).x)};
  }

  struct Aabb_centerResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Aabb_centerResult Aabb_center(const Aabb& self) {
    return {vector_scale(vector_add(self.the_min, self.the_max), 0.5)};
  }

  struct Aabb_extentResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Aabb_extentResult Aabb_extent(const Aabb& self) {
    return {vector_sub(self.the_max, self.the_min)};
  }

  struct camera_validResult { Bool Result = false; };
  /// True when the camera's projection parameters are usable.
  KERN_PURE camera_validResult camera_valid(const Camera& c) {
    if (logic_not(math_compare<">">(c.far, c.near))) {
      return {false};
    }
    if (c.orthographic) {
      return {logic_and(logic_and(math_compare<">">(c.ortho_height, 0.0), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, c.ortho_height), 0.0)), math_compare<">=">(c.near, 0.0))};
    }
    return {logic_and(logic_and(math_compare<">">(c.near, 0.0), math_compare<">">(c.fov, 0.0)), math_compare<"<">(c.fov, kPi))};
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
};

KERN_LAYOUT(engine_math_math) {
  node(13, 0, 0);
  node(14, 280, 0);
  node(15, 160, 150);
  node(16, 160, 240);
  node(17, 160, 330);
  node(18, 160, 420);
  node(19, 160, 510);
  node(20, 0, 650);
  node(21, 280, 650);
  node(22, 160, 800);
  node(23, 160, 890);
  node(24, 160, 980);
  node(25, 160, 1070);
  node(26, 160, 1160);
  node(27, 0, 1300);
  node(28, 280, 1300);
  node(29, 160, 1450);
  node(30, 160, 1540);
  node(31, 0, 1680);
  node(32, 280, 1680);
  node(33, 160, 1830);
  node(34, 160, 1920);
  node(35, 160, 2010);
  node(36, 160, 2100);
  node(37, 160, 2190);
  node(38, 160, 2280);
  node(39, 0, 2420);
  node(40, 280, 2420);
  node(41, 160, 2570);
  node(42, 160, 2660);
  node(43, 160, 2750);
  node(44, 160, 2840);
  node(45, 160, 2930);
  node(46, 160, 3020);
  node(47, 160, 3110);
  node(48, 0, 3250);
  node(49, 280, 3250);
  node(50, 160, 3400);
  node(51, 160, 3490);
  node(52, 160, 3580);
  node(53, 160, 3670);
  node(54, 160, 3760);
  node(55, 160, 3850);
  node(56, 160, 3940);
  node(57, 160, 4030);
  node(58, 160, 4120);
  node(59, 160, 4210);
  node(60, 160, 4300);
  node(61, 160, 4390);
  node(62, 160, 4480);
  node(63, 160, 4570);
  node(64, 160, 4660);
  node(65, 160, 4750);
  node(66, 0, 4890);
  node(67, 280, 4890);
  node(68, 160, 5040);
  node(69, 160, 5130);
  node(70, 0, 5270);
  node(71, 280, 5270);
  node(72, 160, 5420);
  node(73, 0, 5560);
  node(74, 280, 5560);
  node(75, 160, 5710);
  node(76, 160, 5800);
  node(77, 0, 5940);
  node(78, 280, 5940);
  node(79, 160, 6090);
  node(80, 160, 6180);
  node(81, 160, 6270);
  node(82, 0, 6410);
  node(83, 280, 6410);
  node(84, 160, 6560);
  node(85, 160, 6650);
  node(86, 160, 6740);
  node(87, 160, 6830);
  node(88, 160, 6920);
  node(89, 160, 7010);
  node(90, 160, 7100);
  node(91, 160, 7190);
  node(92, 160, 7280);
  node(93, 160, 7370);
  node(94, 160, 7460);
  node(95, 0, 7600);
  node(96, 280, 7600);
  node(97, 160, 7750);
  node(98, 160, 7840);
  node(99, 160, 7930);
  node(100, 0, 8070);
  node(101, 280, 8070);
  node(102, 160, 8220);
  node(103, 160, 8310);
  node(104, 160, 8400);
  node(105, 160, 8490);
  node(106, 160, 8580);
  node(107, 160, 8670);
  node(108, 160, 8760);
  node(109, 160, 8850);
  node(110, 160, 8940);
  node(111, 160, 9030);
  node(112, 0, 9170);
  node(113, 280, 9170);
  node(114, 160, 9320);
  node(115, 160, 9410);
  node(116, 160, 9500);
  node(117, 160, 9590);
  node(118, 160, 9680);
  node(119, 160, 9770);
  node(120, 160, 9860);
  node(121, 160, 9950);
  node(122, 160, 10040);
  node(123, 160, 10130);
  node(124, 0, 10270);
  node(125, 280, 10270);
  node(126, 160, 10420);
  node(127, 560, 10270);
  node(128, 440, 10420);
  node(129, 440, 10510);
  node(130, 440, 10600);
  node(131, 440, 10690);
  node(132, 440, 10780);
  node(133, 440, 10870);
  node(134, 440, 10960);
  node(135, 440, 11050);
  node(136, 440, 11140);
  node(137, 440, 11230);
  node(138, 440, 11320);
  node(139, 440, 11410);
  node(140, 440, 11500);
  node(141, 0, 11640);
  node(142, 280, 11640);
  node(143, 160, 11790);
  node(144, 160, 11880);
  node(145, 160, 11970);
  node(146, 160, 12060);
  node(147, 0, 12200);
  node(148, 280, 12200);
  node(149, 560, 12200);
  node(150, 840, 12200);
  node(151, 1120, 12200);
  node(152, 1400, 12200);
  node(153, 1280, 12350);
  node(154, 0, 12490);
  node(155, 280, 12490);
  node(156, 160, 12640);
  node(157, 560, 12490);
  node(158, 440, 12640);
  node(159, 840, 12490);
  node(160, 720, 12640);
  node(161, 1120, 12490);
  node(162, 1400, 12490);
  node(163, 1280, 12640);
  node(164, 0, 12780);
  node(165, 280, 12780);
  node(166, 160, 12930);
  node(167, 160, 13020);
  node(168, 160, 13110);
  node(169, 160, 13200);
  node(170, 0, 13340);
  node(171, 280, 13340);
  node(172, 160, 13490);
  node(173, 160, 13580);
  node(174, 160, 13670);
  node(175, 160, 13760);
  node(176, 160, 13850);
  node(177, 160, 13940);
  node(178, 160, 14030);
  node(179, 160, 14120);
  node(180, 160, 14210);
  node(181, 160, 14300);
  node(182, 160, 14390);
  node(183, 160, 14480);
  node(184, 160, 14570);
  node(185, 160, 14660);
  node(186, 160, 14750);
  node(187, 160, 14840);
  node(188, 160, 14930);
  node(189, 160, 15020);
  node(190, 160, 15110);
  node(191, 0, 15250);
  node(192, 280, 15250);
  node(193, 160, 15400);
  node(194, 160, 15490);
  node(195, 160, 15580);
  node(196, 160, 15670);
  node(197, 560, 15250);
  node(198, 440, 15400);
  node(199, 840, 15250);
  node(200, 720, 15400);
  node(201, 720, 15490);
  node(202, 720, 15580);
  node(203, 720, 15670);
  node(204, 720, 15760);
  node(205, 720, 15850);
  node(206, 720, 15940);
  node(207, 720, 16030);
  node(208, 720, 16120);
  node(209, 720, 16210);
  node(210, 720, 16300);
  node(211, 720, 16390);
  node(212, 720, 16480);
  node(213, 720, 16570);
  node(214, 0, 16710);
  node(215, 280, 16710);
  node(216, 160, 16860);
  node(217, 160, 16950);
  node(218, 160, 17040);
  node(219, 160, 17130);
  node(220, 160, 17220);
  node(221, 160, 17310);
  node(222, 160, 17400);
  node(223, 160, 17490);
  node(224, 160, 17580);
  node(225, 160, 17670);
  node(226, 160, 17760);
  node(227, 160, 17850);
  node(228, 160, 17940);
  node(229, 160, 18030);
  node(230, 160, 18120);
  node(231, 160, 18210);
  node(232, 160, 18300);
  node(233, 160, 18390);
  node(234, 160, 18480);
  node(235, 160, 18570);
  node(236, 160, 18660);
  node(237, 160, 18750);
  node(238, 160, 18840);
  node(239, 160, 18930);
  node(240, 160, 19020);
  node(241, 160, 19110);
  node(242, 160, 19200);
  node(243, 160, 19290);
  node(244, 160, 19380);
  node(245, 160, 19470);
  node(246, 160, 19560);
  node(247, 160, 19650);
  node(248, 160, 19740);
  node(249, 160, 19830);
  node(250, 160, 19920);
  node(251, 160, 20010);
  node(252, 160, 20100);
  node(253, 0, 20240);
  node(254, 280, 20240);
  node(255, 160, 20390);
  node(256, 160, 20480);
  node(257, 160, 20570);
  node(258, 160, 20660);
  node(259, 560, 20240);
  node(260, 440, 20390);
  node(261, 440, 20480);
  node(262, 440, 20570);
  node(263, 840, 20240);
  node(264, 720, 20390);
  node(265, 720, 20480);
  node(266, 720, 20570);
  node(267, 720, 20660);
  node(268, 720, 20750);
  node(269, 720, 20840);
  node(270, 720, 20930);
  node(271, 720, 21020);
  node(272, 0, 21160);
  node(273, 280, 21160);
  node(274, 160, 21310);
  node(275, 0, 21450);
  node(276, 280, 21450);
  node(277, 560, 21450);
  node(278, 440, 21600);
  node(279, 840, 21450);
  node(280, 720, 21600);
  node(281, 1120, 21450);
  node(282, 1000, 21600);
  node(283, 0, 21740);
  node(284, 280, 21740);
  node(285, 160, 21890);
  node(286, 0, 22030);
  node(287, 280, 22030);
  node(288, 160, 22180);
  node(289, 160, 22270);
  node(290, 160, 22360);
  node(291, 160, 22450);
  node(292, 0, 22590);
  node(293, 280, 22590);
  node(10, 560, 22590);
  node(294, 840, 22590);
  node(295, 720, 22740);
  node(296, 720, 22830);
  node(297, 1120, 22590);
  node(298, 1000, 22740);
  node(299, 1000, 22830);
  node(300, 1400, 22590);
  node(301, 1280, 22740);
  node(302, 1280, 22830);
  node(303, 1680, 22590);
  node(304, 1560, 22740);
  node(305, 1560, 22830);
  node(11, 1960, 22590);
  node(306, 2240, 22590);
  node(307, 2120, 22740);
  node(308, 2120, 22830);
  node(309, 2120, 22920);
  node(310, 2120, 23010);
  node(311, 2120, 23100);
  node(312, 2120, 23190);
  node(313, 2120, 23280);
  node(314, 2120, 23370);
  node(315, 2120, 23460);
  node(316, 2120, 23550);
  node(317, 2120, 23640);
  node(318, 2120, 23730);
  node(319, 2120, 23820);
  node(320, 2520, 22590);
  node(321, 2400, 22740);
  node(322, 0, 23960);
  node(323, 280, 23960);
  node(324, 160, 24110);
  node(325, 160, 24200);
  node(326, 160, 24290);
  node(327, 160, 24380);
  node(328, 160, 24470);
  node(329, 160, 24560);
  node(330, 160, 24650);
  node(331, 160, 24740);
  node(332, 160, 24830);
  node(333, 160, 24920);
  node(334, 160, 25010);
  node(335, 160, 25100);
  node(336, 160, 25190);
  node(337, 160, 25280);
  node(338, 160, 25370);
  node(339, 160, 25460);
  node(340, 160, 25550);
  node(341, 160, 25640);
  node(342, 160, 25730);
  node(343, 160, 25820);
  node(344, 160, 25910);
  node(345, 160, 26000);
  node(346, 160, 26090);
  node(347, 160, 26180);
  node(348, 160, 26270);
  node(349, 0, 26410);
  node(350, 280, 26410);
  node(351, 160, 26560);
  node(352, 160, 26650);
  node(353, 160, 26740);
  node(354, 160, 26830);
  node(355, 160, 26920);
  node(356, 160, 27010);
  node(357, 160, 27100);
  node(358, 160, 27190);
  node(359, 160, 27280);
  node(360, 160, 27370);
  node(361, 160, 27460);
  node(362, 160, 27550);
  node(363, 160, 27640);
  node(364, 160, 27730);
  node(365, 160, 27820);
  node(366, 160, 27910);
  node(367, 160, 28000);
  node(368, 160, 28090);
  node(369, 160, 28180);
  node(370, 160, 28270);
  node(371, 160, 28360);
  node(372, 160, 28450);
  node(373, 0, 28590);
  node(374, 280, 28590);
  node(375, 160, 28740);
  node(376, 160, 28830);
  node(377, 160, 28920);
  node(378, 160, 29010);
  node(379, 160, 29100);
  node(380, 160, 29190);
  node(381, 160, 29280);
  node(382, 160, 29370);
  node(383, 160, 29460);
  node(384, 160, 29550);
  node(385, 160, 29640);
  node(386, 160, 29730);
  node(387, 160, 29820);
  node(388, 160, 29910);
  node(389, 160, 30000);
  node(390, 160, 30090);
  node(391, 160, 30180);
  node(392, 160, 30270);
  node(393, 160, 30360);
  node(394, 160, 30450);
  node(395, 160, 30540);
  node(396, 160, 30630);
  node(397, 160, 30720);
  node(398, 160, 30810);
  node(399, 160, 30900);
  node(400, 160, 30990);
  node(401, 160, 31080);
  node(402, 160, 31170);
  node(403, 160, 31260);
  node(404, 160, 31350);
  node(405, 160, 31440);
  node(406, 160, 31530);
  node(407, 160, 31620);
  node(408, 160, 31710);
  node(409, 160, 31800);
  node(410, 160, 31890);
  node(411, 160, 31980);
  node(412, 0, 32120);
  node(413, 280, 32120);
  node(414, 560, 32120);
  node(415, 440, 32270);
  node(416, 840, 32120);
  node(417, 720, 32270);
  node(418, 1120, 32120);
  node(419, 1000, 32270);
  node(420, 1400, 32120);
  node(421, 1280, 32270);
  node(422, 0, 32410);
  node(423, 280, 32410);
  node(424, 560, 32410);
  node(425, 440, 32560);
  node(426, 840, 32410);
  node(427, 720, 32560);
  node(428, 1120, 32410);
  node(429, 1000, 32560);
  node(430, 1400, 32410);
  node(431, 1280, 32560);
  node(432, 0, 32700);
  node(433, 280, 32700);
  node(434, 160, 32850);
  node(435, 160, 32940);
  node(436, 560, 32700);
  node(437, 440, 32850);
  node(438, 440, 32940);
  node(439, 840, 32700);
  node(440, 720, 32850);
  node(441, 720, 32940);
  node(442, 1120, 32700);
  node(443, 1000, 32850);
  node(444, 1000, 32940);
  node(445, 1000, 33030);
  node(446, 1400, 32700);
  node(447, 1280, 32850);
  node(448, 1280, 32940);
  node(449, 1280, 33030);
  node(450, 1680, 32700);
  node(451, 1560, 32850);
  node(452, 1560, 32940);
  node(453, 1560, 33030);
  node(454, 1960, 32700);
  node(455, 1840, 32850);
  node(456, 1840, 32940);
  node(457, 1840, 33030);
  node(458, 2240, 32700);
  node(459, 2120, 32850);
  node(460, 2120, 32940);
  node(461, 2120, 33030);
  node(462, 2520, 32700);
  node(463, 2400, 32850);
  node(464, 2400, 32940);
  node(465, 2400, 33030);
  node(466, 2800, 32700);
  node(467, 2680, 32850);
  node(468, 2680, 32940);
  node(469, 2680, 33030);
  node(470, 3080, 32700);
  node(471, 2960, 32850);
  node(472, 2960, 32940);
  node(473, 2960, 33030);
  node(474, 3360, 32700);
  node(475, 3240, 32850);
  node(476, 3240, 32940);
  node(477, 3240, 33030);
  node(478, 3640, 32700);
  node(479, 3920, 32700);
  node(480, 3800, 32850);
  node(481, 3800, 32940);
  node(482, 3800, 33030);
  node(483, 3800, 33120);
  node(484, 4200, 32700);
  node(485, 4080, 32850);
  node(486, 4080, 32940);
  node(487, 4080, 33030);
  node(488, 4080, 33120);
  node(489, 4480, 32700);
  node(490, 4360, 32850);
  node(491, 4360, 32940);
  node(492, 4360, 33030);
  node(493, 4360, 33120);
  node(494, 4760, 32700);
  node(495, 5040, 32700);
  node(496, 4920, 32850);
  node(497, 4920, 32940);
  node(498, 4920, 33030);
  node(499, 4920, 33120);
  node(500, 5320, 32700);
  node(501, 5200, 32850);
  node(502, 5200, 32940);
  node(503, 5200, 33030);
  node(504, 5200, 33120);
  node(505, 5600, 32700);
  node(506, 5480, 32850);
  node(507, 5480, 32940);
  node(508, 5480, 33030);
  node(509, 5480, 33120);
  node(510, 5880, 32700);
  node(511, 6160, 32700);
  node(512, 6040, 32850);
  node(513, 6040, 32940);
  node(514, 6040, 33030);
  node(515, 6040, 33120);
  node(516, 6440, 32700);
  node(517, 6320, 32850);
  node(518, 6320, 32940);
  node(519, 6320, 33030);
  node(520, 6320, 33120);
  node(521, 6720, 32700);
  node(522, 6600, 32850);
  node(523, 6600, 32940);
  node(524, 6600, 33030);
  node(525, 6600, 33120);
  node(526, 7000, 32700);
  node(527, 7280, 32700);
  node(528, 7160, 32850);
  node(529, 7560, 32700);
  node(530, 7440, 32850);
  node(531, 7840, 32700);
  node(532, 7720, 32850);
  node(533, 8120, 32700);
  node(534, 8400, 32700);
  node(535, 8280, 32850);
  node(536, 0, 33260);
  node(537, 280, 33260);
  node(538, 160, 33410);
  node(539, 560, 33260);
  node(540, 440, 33410);
  node(541, 840, 33260);
  node(542, 1120, 33260);
  node(543, 1400, 33260);
  node(544, 1280, 33410);
  node(545, 1280, 33500);
  node(546, 1680, 33260);
  node(547, 1560, 33410);
  node(548, 1960, 33260);
  node(549, 1840, 33410);
  node(550, 1840, 33500);
  node(551, 2240, 33260);
  node(552, 2520, 33260);
  node(553, 2400, 33410);
  node(554, 2400, 33500);
  node(555, 2800, 33260);
  node(556, 2680, 33410);
  node(557, 0, 33640);
  node(558, 280, 33640);
  node(559, 560, 33640);
  node(560, 840, 33640);
  node(561, 720, 33790);
  node(562, 1120, 33640);
  node(563, 1000, 33790);
  node(564, 1400, 33640);
  node(565, 1280, 33790);
  node(566, 1680, 33640);
  node(567, 1560, 33790);
  node(568, 1960, 33640);
  node(569, 1840, 33790);
  node(570, 2240, 33640);
  node(571, 2120, 33790);
  node(572, 2520, 33640);
  node(573, 2800, 33640);
  node(574, 2680, 33790);
  node(575, 0, 33930);
  node(576, 280, 33930);
  node(577, 160, 34080);
  node(578, 560, 33930);
  node(579, 440, 34080);
  node(580, 440, 34170);
  node(581, 840, 33930);
  node(582, 1120, 33930);
  node(583, 1000, 34080);
  node(584, 1000, 34170);
  node(585, 1000, 34260);
  node(586, 1400, 33930);
  node(587, 1680, 33930);
  node(588, 1560, 34080);
  node(589, 1960, 33930);
  node(590, 1840, 34080);
  node(591, 1840, 34170);
  node(592, 1840, 34260);
  node(593, 1840, 34350);
  node(594, 1840, 34440);
  node(595, 1840, 34530);
  node(596, 1840, 34620);
  node(597, 1840, 34710);
  node(598, 1840, 34800);
  node(599, 1840, 34890);
  node(600, 1840, 34980);
  node(601, 1840, 35070);
  node(602, 1840, 35160);
  node(603, 2240, 33930);
  node(604, 2120, 34080);
  node(605, 2120, 34170);
  node(606, 2520, 33930);
  node(607, 2400, 34080);
  node(608, 2400, 34170);
  node(609, 2800, 33930);
  node(610, 2680, 34080);
  node(611, 2680, 34170);
  node(612, 2680, 34260);
  node(613, 3080, 33930);
  node(614, 3360, 33930);
  node(615, 3240, 34080);
  node(616, 3640, 33930);
  node(617, 3520, 34080);
  node(618, 3520, 34170);
  node(619, 3520, 34260);
  node(620, 3520, 34350);
  node(621, 3520, 34440);
  node(622, 3520, 34530);
  node(623, 3520, 34620);
  node(624, 3520, 34710);
  node(625, 3520, 34800);
  node(626, 3520, 34890);
  node(627, 3520, 34980);
  node(628, 3520, 35070);
  node(629, 3520, 35160);
  node(630, 3920, 33930);
  node(631, 3800, 34080);
  node(632, 3800, 34170);
  node(633, 3800, 34260);
  node(634, 4200, 33930);
  node(635, 4080, 34080);
  node(636, 4080, 34170);
  node(637, 4480, 33930);
  node(638, 4360, 34080);
  node(639, 4360, 34170);
  node(640, 4760, 33930);
  node(641, 4640, 34080);
  node(642, 4640, 34170);
  node(643, 5040, 33930);
  node(644, 5320, 33930);
  node(645, 5200, 34080);
  node(646, 5200, 34170);
  node(647, 5600, 33930);
  node(648, 5480, 34080);
  node(649, 5480, 34170);
  node(650, 5880, 33930);
  node(651, 5760, 34080);
  node(652, 5760, 34170);
  node(653, 6160, 33930);
  node(654, 6440, 33930);
  node(655, 6320, 34080);
  node(656, 6320, 34170);
  node(657, 6720, 33930);
  node(658, 6600, 34080);
  node(659, 6600, 34170);
  node(660, 7000, 33930);
  node(661, 6880, 34080);
  node(662, 6880, 34170);
  node(663, 7280, 33930);
  node(664, 7560, 33930);
  node(665, 7440, 34080);
  node(666, 7440, 34170);
  node(667, 7440, 34260);
  node(668, 7840, 33930);
  node(669, 7720, 34080);
  node(670, 7720, 34170);
  node(671, 7720, 34260);
  node(672, 8120, 33930);
  node(673, 8000, 34080);
  node(674, 8000, 34170);
  node(675, 8000, 34260);
  node(676, 8400, 33930);
  node(677, 8680, 33930);
  node(678, 8960, 33930);
  node(679, 8840, 34080);
  node(680, 0, 35300);
  node(681, 280, 35300);
  node(12, 560, 35300);
  node(682, 840, 35300);
  node(683, 0, 35500);
  node(684, 280, 35500);
  node(685, 160, 35650);
  node(686, 160, 35740);
  node(687, 0, 35880);
  node(688, 280, 35880);
  node(689, 160, 36030);
  node(690, 160, 36120);
  node(691, 160, 36210);
  node(692, 160, 36300);
  node(693, 160, 36390);
  node(694, 0, 36530);
  node(695, 280, 36530);
  node(696, 160, 36680);
  node(697, 160, 36770);
  node(698, 160, 36860);
  node(699, 160, 36950);
  node(700, 0, 37090);
  node(701, 280, 37090);
  node(702, 160, 37240);
  node(703, 160, 37330);
  node(704, 160, 37420);
  node(705, 0, 37560);
  node(706, 280, 37560);
  node(707, 160, 37710);
  node(708, 160, 37800);
  node(709, 160, 37890);
  node(710, 160, 37980);
  node(711, 560, 37560);
  node(712, 840, 37560);
  node(713, 720, 37710);
  node(714, 1120, 37560);
  node(715, 1000, 37710);
  node(716, 1000, 37800);
  node(717, 1000, 37890);
  node(718, 1000, 37980);
  node(719, 1000, 38070);
  node(720, 1000, 38160);
  node(721, 1000, 38250);
  node(722, 1000, 38340);
  node(723, 1000, 38430);
  node(724, 1400, 37560);
  node(725, 1280, 37710);
  node(726, 1280, 37800);
  node(727, 1280, 37890);
  node(728, 1280, 37980);
  node(729, 1280, 38070);
  node(730, 1280, 38160);
  node(731, 1280, 38250);
  node(732, 1280, 38340);
  node(733, 1280, 38430);
  node(734, 0, 38570);
  node(735, 280, 38570);
  node(736, 160, 38720);
  node(737, 560, 38570);
  node(738, 440, 38720);
  node(739, 840, 38570);
  node(740, 720, 38720);
  node(741, 720, 38810);
  node(742, 1120, 38570);
  node(743, 1000, 38720);
  node(744, 1000, 38810);
  node(745, 1000, 38900);
  node(746, 1000, 38990);
  node(747, 1400, 38570);
  node(748, 1280, 38720);
  node(749, 1280, 38810);
  node(750, 1680, 38570);
  node(751, 1560, 38720);
  node(752, 1560, 38810);
  node(753, 1960, 38570);
  node(754, 1840, 38720);
  node(755, 1840, 38810);
  node(756, 1840, 38900);
  node(757, 1840, 38990);
  node(758, 1840, 39080);
  node(759, 1840, 39170);
  node(760, 1840, 39260);
  node(761, 1840, 39350);
  node(762, 1840, 39440);
  next_id(763);
}

}  // namespace kern_blueprint_64c1acf056c7521a

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_gfx_passes"
namespace kern_blueprint_33fb79d97c280ace {

using engine_math_math = ::kern_blueprint_64c1acf056c7521a::engine_math_math;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_gfx_passes : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_ENUM(AoQuality, low, medium, high, ultra);
  KERN_ENUM(BloomState, off, active, disabled_slow_device, unsupported);
  KERN_STRUCT(BloomOptions) {
    Bool enabled = true;
    Bool forced = false;
    Bool grade = true;
  };
  KERN_STRUCT(ShadowOptions) {
    I32 resolution = 512;
    Float extent = 120.0;
    Float bias = 0.0012;
    I32 filter = 9;
  };
  KERN_STRUCT(SkyAtmosphere) {
    Vector fog_color = Vector{0.474, 0.584, 0.716};
    Vector sun_color = Vector{2.8, 2.47, 2.11};
    Vector sun_direction = Vector{-0.45, -0.52, 0.72};
    Float sun_scatter = 0.0;
    Float horizon = 0.0;
  };
  KERN_STRUCT(SkyState) {
    Vector sun = Vector{-0.45, -0.52, 0.72};
    Vector moon = Vector{0.4, 0.5, 0.76};
    Float night = 0.0;
    Bool clouds = true;
    Vector blast_direction = Vector{0.0, 0.0, -1.0};
    Float blast_age = -1.0;
    Vector wind = Vector{0.012, 0.004, 0.0};
  };
  KERN_STRUCT(AoOptions) {
    Bool enabled = true;
    AoQuality quality = AoQuality::medium;
    Bool half_resolution = true;
    Float radius = 1.6;
    Float power = 2.2;
    Bool adaptive = true;
  };
  KERN_STRUCT(BloomPass_Occlusion) {
    U32 texture = 0;
    Float scale_x = 1.0;
    Float scale_y = 1.0;
    Mat4 projection = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
  };
};

KERN_LAYOUT(engine_gfx_passes) {
  next_id(1);
}

}  // namespace kern_blueprint_33fb79d97c280ace

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_gfx_renderer"
namespace kern_blueprint_6093af1c7e680998 {

using engine_gfx_passes = ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_gfx_renderer : virtual kern::Library, virtual engine_gfx_passes {
  // ---- types ----
  KERN_STRUCT(Lighting) {
    Vector sun_direction = Vector{-0.45, -0.52, 0.72};
    Vector sun_color = Vector{2.8, 2.47, 2.11};
    Vector sky_color = Vector{0.469, 0.609, 0.85};
    Vector ground_color = Vector{0.125, 0.094, 0.054};
    Vector fog_color = Vector{0.474, 0.584, 0.716};
    Float fog_density = 0.00085;
    Float exposure = 1.0;
    Float env_intensity = 0.9;
    Vector env_zenith = Vector{0.05, 0.2, 0.62};
    Vector env_horizon = Vector{0.52, 0.66, 0.86};
    Vector env_ground = Vector{0.105, 0.127, 0.084};
    Float fog_height_falloff = 0.0;
    Float fog_sun_scatter = 0.0;
    Float fog_horizon = 0.0;
  };
  KERN_STRUCT(PointLights) {
    U32 count = 0;
    Fixed<Float, 64> pos_range = Fixed<Float, 64>{};
    Fixed<Float, 64> color_intensity = Fixed<Float, 64>{};
  };
  KERN_STRUCT(Viewport) {
    I32 x = 0;
    I32 y = 0;
    I32 width = 0;
    I32 height = 0;
  };
  KERN_STRUCT(RenderParams) {
    Vec4 clear_color = Vec4{0.045, 0.07, 0.12, 1.0};
    Bool has_viewport = false;
    Viewport viewport = Viewport{0, 0, 0, 0};
    Bool page_hidden = false;
    U32 layers = 4294967295;
    Bool sky = true;
    Bool reuse_shadow = false;
    U32 framebuffer = 0;
  };
  KERN_STRUCT(RenderStats) {
    U32 draw_calls = 0;
    U32 shadow_draw_calls = 0;
    U32 shadow_resolution = 0;
    Bool ao = false;
    Bool shadow_reused = false;
    Float triangles = 0.0;
    U32 instances = 0;
    U32 culled_meshes = 0;
    U32 visible_meshes = 0;
    Float cpu_ms = 0.0;
    Float gpu_ms = -1.0;
    BloomState bloom = BloomState::off;
  };
  KERN_STRUCT(RendererOptions) {
    I32 width = 960;
    I32 height = 540;
    Bool shadows = false;
    ShadowOptions shadow = ShadowOptions{512, 120.0, 0.0012, 9};
    BloomOptions bloom = BloomOptions{true, false, true};
    AoOptions ao = AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true};
    Bool grade = true;
  };
};

KERN_LAYOUT(engine_gfx_renderer) {
  next_id(1);
}

}  // namespace kern_blueprint_6093af1c7e680998

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_app_app"
namespace kern_blueprint_fbd4d5d49d8b5cdd {

using engine_gfx_renderer = ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_app_app : virtual kern::Library, virtual engine_gfx_renderer {
  // ---- types ----
  KERN_ENUM(Key, unknown, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, digit0, digit1, digit2, digit3, digit4, digit5, digit6, digit7, digit8, digit9, left, right, up, down, space, enter, escape, tab, backspace, shift_left, shift_right, control_left, control_right, alt_left, alt_right, count);
  KERN_STRUCT(Frame) {
    Float time = 0.0;
    Float dt = 0.0;
    Int64 index = Int64{0x0};
  };
  KERN_STRUCT(Touch) {
    I32 id = -1;
    Vector start = Vector{0.0, 0.0, 0.0};
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector previous = Vector{0.0, 0.0, 0.0};
  };

  // ---- functions ----
  struct App_configureResult { RendererOptions Result = RendererOptions{960, 540, false, ShadowOptions{512, 120.0, 0.0012, 9}, BloomOptions{true, false, true}, AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true}, true}; };
  /// Renderer configuration, asked once before start().
  KERN_PURE App_configureResult App_configure() {
    return {struct_make<RendererOptions>(960, 540, false, struct_make<ShadowOptions>(512, 120.0, 0.0012, 9), struct_make<BloomOptions>(true, false, true), struct_make<AoOptions>(true, AoQuality::medium, true, 1.6, 2.2, true), true)};
  }

  struct App_extensionsResult { List<Int> Result = List<Int>{}; };
  /// Shader extensions the game uses (must outlive the app).
  KERN_PURE App_extensionsResult App_extensions() {
    return {List<Int>{}};
  }

  KERN_PURE void App_resized(Int engine = 0) {
  }

  KERN_PURE void App_stop(Int engine = 0) {
  }
};

KERN_LAYOUT(engine_app_app) {
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 160, 150);
  node(4, 160, 240);
  node(5, 160, 330);
  node(6, 160, 420);
  node(7, 0, 560);
  node(8, 280, 560);
  node(9, 0, 760);
  node(10, 0, 960);
  next_id(11);
}

}  // namespace kern_blueprint_fbd4d5d49d8b5cdd

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_audio_audio"
namespace kern_blueprint_0581e7282166936f {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_audio_audio : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(audio_Bus, master, music, sfx, ambience, ui, voice, count);
  KERN_ENUM(audio_Rolloff, inverse, linear, exponential);
  KERN_STRUCT(audio_BusSettings) {
    Float volume = 1.0;
    Bool muted = false;
    Float lowpass_hz = 0.0;
    Float reverb_send = 0.0;
  };
  KERN_STRUCT(audio_ClipInfo) {
    Text name = "";
    U32 frames = 0;
    U32 channels = 1;
    Float duration = 0.0;
    Float peak = 0.0;
    Bool loopable = false;
  };
  KERN_STRUCT(audio_Listener) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector forward = Vector{1.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 1.0};
    Vector velocity = Vector{0.0, 0.0, 0.0};
  };
  KERN_STRUCT(audio_Meter) {
    Fixed<Float, 2> peak = Fixed<Float, 2>{};
    Fixed<Float, 2> rms = Fixed<Float, 2>{};
  };
  KERN_STRUCT(audio_PlayParams) {
    Float volume = 1.0;
    Float pitch = 1.0;
    Float pan = 0.0;
    Bool the_loop = false;
    audio_Bus bus = audio_Bus::sfx;
    Float fade_in = 0.0;
    Float start = 0.0;
    I32 priority = 0;
    Bool spatial = false;
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector velocity = Vector{0.0, 0.0, 0.0};
    Float min_distance = 1.0;
    Float max_distance = 50.0;
    audio_Rolloff rolloff = audio_Rolloff::inverse;
    Float spread = 0.0;
    Float doppler = 1.0;
  };
  KERN_STRUCT(audio_Stats) {
    U32 voices = 0;
    U32 audible = 0;
    U32 virtual_voices = 0;
    U32 stolen = 0;
    U32 clips = 0;
    Int64 frames_rendered = Int64{0x0};
    Float limiter_reduction_db = 0.0;
    audio_Meter master = audio_Meter{Fixed<Float, 2>{}, Fixed<Float, 2>{}};
    Fixed<audio_Meter, 6> buses = Fixed<audio_Meter, 6>{};
    Fixed<Float, 2> peak_hold = Fixed<Float, 2>{};
  };

  // ---- functions ----
  struct db_to_gainResult { Float Result = 0.0; };
  KERN_PURE db_to_gainResult db_to_gain(Float db = 0.0) {
    return {kern::formula([](Float db) -> Float { return kern::math::pow(10.0, kern::math::div(db, 20.0)); }, db)};
  }

  struct gain_to_dbResult { Float Result = 0.0; };
  KERN_PURE gain_to_dbResult gain_to_db(Float g = 0.0) {
    return {kern::formula([](Float g) -> Float { return g > 1e-09 ? 20.0 * kern::math::div(kern::math::log(g), 2.302585092994046) : -180.0; }, g)};
  }
};

KERN_LAYOUT(engine_audio_audio) {
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 160, 150);
  node(4, 0, 290);
  node(5, 280, 290);
  node(6, 160, 440);
  next_id(7);
}

}  // namespace kern_blueprint_0581e7282166936f

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_audio_synth"
namespace kern_blueprint_3f8740a3424253d4 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_audio_synth : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(audio_Wave, square, saw, sine, triangle, noise, pink, pulse);
  KERN_STRUCT(audio_BuiltinSound) {
    Text name = "";
    Text label = "";
    Text category = "";
    Bool the_loop = false;
  };
  KERN_STRUCT(audio_SynthPatch) {
    audio_Wave wave = audio_Wave::square;
    Float volume = 0.5;
    Float attack = 0.005;
    Float sustain = 0.1;
    Float punch = 0.0;
    Float release = 0.2;
    Float frequency = 440.0;
    Float slide = 0.0;
    Float slide_accel = 0.0;
    Float min_frequency = 20.0;
    Float vibrato_depth = 0.0;
    Float vibrato_rate = 6.0;
    Float arp_ratio = 1.0;
    Float arp_time = 0.0;
    Float duty = 0.5;
    Float duty_sweep = 0.0;
    Float fm_ratio = 0.0;
    Float fm_depth = 0.0;
    Float lowpass = 0.0;
    Float lowpass_sweep = 0.0;
    Float resonance = 0.0;
    Float highpass = 0.0;
    Float tremolo_depth = 0.0;
    Float tremolo_rate = 0.0;
    Float repeat = 0.0;
    Float bitcrush = 0.0;
    U32 seed = 1;
  };
};

KERN_LAYOUT(engine_audio_synth) {
  next_id(1);
}

}  // namespace kern_blueprint_3f8740a3424253d4

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_game_level"
namespace kern_blueprint_402ced772ba1edd9 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_game_level : virtual kern::Library {
  // ---- types ----
  KERN_ENUM(game_Shape, box, sphere, cylinder, cone, capsule, plane, torus, ramp, mesh);
  KERN_STRUCT(game_Transform) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector rotation = Vector{0.0, 0.0, 0.0};
    Vector scale = Vector{1.0, 1.0, 1.0};
  };
};

KERN_LAYOUT(engine_game_level) {
  next_id(1);
}

}  // namespace kern_blueprint_402ced772ba1edd9

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_game_runtime"
namespace kern_blueprint_fdc39c3af1f3c0bb {

using engine_audio_audio = ::kern_blueprint_0581e7282166936f::engine_audio_audio;
using engine_math_math = ::kern_blueprint_64c1acf056c7521a::engine_math_math;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_game_runtime : virtual kern::Library, virtual engine_audio_audio, virtual engine_math_math {
  // ---- types ----
  KERN_ENUM(game_GameStatus, inactive, playing, won, lost);
  KERN_ENUM(game_SimMode, none, simulate, play, replica);
  KERN_STRUCT(game_BlueprintRunning) {
    Text name = "";
    Text tier = "";
    Int functions = 0;
    Int nodes = 0;
    Bool disabled = false;
  };
  KERN_STRUCT(game_GameWorld_ArtStats) {
    U32 rigs = 0;
    U32 decals = 0;
    U32 tracers = 0;
    U32 chips = 0;
    Bool view_model = false;
  };
  KERN_STRUCT(game_GameWorld_DetectorModel) {
    Float sigma_abs = 0.0;
    Float sigma_rel = 0.0;
    Float dropout = 0.0;
    Int64 seed = Int64{0x1};
  };
  KERN_STRUCT(game_GameWorld_SpawnChange) {
    U32 source = 0;
    U32 id = 0;
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector rotation = Vector{0.0, 0.0, 0.0};
  };
  KERN_STRUCT(game_GameWorld_WalkerJoint) {
    Text name = "";
    Float angle = 0.0;
    Float lower = 0.0;
    Float upper = 0.0;
  };
  KERN_STRUCT(game_GameWorld_WalkerSensors) {
    Fixed<Float, 64> range = Fixed<Float, 64>{};
    Fixed<U8, 64> zone = Fixed<U8, 64>{};
    Float nearest = -1.0;
    Float nearest_bearing = 0.0;
    Float free_left = 0.0;
    Float free_right = 0.0;
    Vector gravity = Vector{0.0, 0.0, -1.0};
    Bool fallen = false;
    Float speed = 0.0;
    Float tof_time = -1.0;
    I32 hits = 0;
    Fixed<Vector, 64> hit = Fixed<Vector, 64>{};
    I32 forward_observed = 0;
  };
  KERN_STRUCT(game_PlayInput) {
    Float move_x = 0.0;
    Float move_y = 0.0;
    Float up = 0.0;
    Bool jump = false;
    Bool boost = false;
    Bool camera = false;
    Bool handbrake = false;
    U32 robot = 0;
    Float look_x = 0.0;
    Float look_y = 0.0;
    Float turn_deg = 0.0;
    Float speed_scale = 1.0;
    Float zoom = 0.0;
    Float view_yaw = 0.0;
    Bool has_view_yaw = false;
    Float view_pitch = 0.0;
    Bool fire = false;
    Bool aim = false;
    Bool reload = false;
    Bool crouch = false;
    Bool interact = false;
    Float look_stick_x = 0.0;
    Float look_stick_y = 0.0;
    Bool gamepad = false;
    Float rewind = 0.0;
  };
  KERN_STRUCT(game_ShotEvent) {
    Vector from = Vector{0.0, 0.0, 0.0};
    Vector to = Vector{0.0, 0.0, 0.0};
    U32 shooter = 0;
    U32 hit = 0;
    Bool impact = false;
    Bool head = false;
    Vector normal = Vector{0.0, 0.0, 1.0};
    U8 surface = 0;
  };
  KERN_STRUCT(game_SimStats) {
    U32 bodies = 0;
    U32 awake = 0;
    U32 shapes = 0;
    U32 joints = 0;
    U32 broken_joints = 0;
    U32 contacts = 0;
    Int64 steps = Int64{0x0};
  };
  KERN_STRUCT(game_SoldierState) {
    Float health = 100.0;
    Float max_health = 100.0;
    Float armor = 0.0;
    I32 ammo = 0;
    I32 magazine = 0;
    I32 reserve = 0;
    Float reload = -1.0;
    Float spread = 0.0;
    Float respawn_in = -1.0;
    U8 hits = 0;
    U8 damages = 0;
    Bool hit_head = false;
    Bool hit_kill = false;
    Bool dead = false;
    Float damage_yaw = 0.0;
  };
  KERN_STRUCT(game_GameWorld_WalkerProbe) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
    Vector velocity = Vector{0.0, 0.0, 0.0};
    Vector spin = Vector{0.0, 0.0, 0.0};
    Vector gravity = Vector{0.0, 0.0, 0.0};
    Fixed<Bool, 2> foot_contact = Fixed<Bool, 2>{};
    Fixed<Float, 2> foot_height = Fixed<Float, 2>{};
    Bool self_contact = false;
    Bool body_contact = false;
    Fixed<Float, 14> joint = Fixed<Float, 14>{};
    Fixed<Float, 14> joint_speed = Fixed<Float, 14>{};
    Fixed<Float, 14> action = Fixed<Float, 14>{};
  };
  KERN_STRUCT(game_Hud) {
    game_GameStatus status = game_GameStatus::inactive;
    Text title = "";
    Text message = "";
    Float message_age = 0.0;
    Float score = 0.0;
    U32 pickups = 0;
    U32 pickups_total = 0;
    Float elapsed = 0.0;
    Float remaining = -1.0;
    Float speed = 0.0;
    Float boost = -1.0;
    Bool boosting = false;
    Bool interior = false;
    Float best = 0.0;
    Bool soldier = false;
    Float health = -1.0;
    Float max_health = 100.0;
    Float armor = 0.0;
    I32 ammo = -1;
    I32 magazine = 0;
    I32 reserve = 0;
    Float reload = -1.0;
    Float spread = 0.0;
    Float hit_age = 99.0;
    Bool hit_head = false;
    Bool hit_kill = false;
    Float damage_age = 99.0;
    Float damage_yaw = 0.0;
    Float respawn_in = -1.0;
    Bool aiming = false;
    Float fov = 60.0;
  };
  KERN_STRUCT(game_SoundCue) {
    Text sound = "";
    Vector position = Vector{0.0, 0.0, 0.0};
    Float volume = 1.0;
    Float pitch = 1.0;
    Bool spatial = true;
    audio_Bus bus = audio_Bus::sfx;
    U32 actor = 0;
    Bool voice = false;
    Bool silence = false;
  };

  // ---- functions ----
  struct NightWindows_nameResult { Text Result = ""; };
  KERN_PURE NightWindows_nameResult NightWindows_name() {
    return {"kern-night-windows"};
  }
};

KERN_LAYOUT(engine_game_runtime) {
  node(1, 0, 0);
  node(2, 280, 0);
  next_id(3);
}

}  // namespace kern_blueprint_fdc39c3af1f3c0bb

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_gfx_mesh"
namespace kern_blueprint_bb872f843a762d45 {

using engine_math_math = ::kern_blueprint_64c1acf056c7521a::engine_math_math;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_gfx_mesh : virtual kern::Library, virtual engine_math_math {
  // ---- types ----
  KERN_ENUM(AlphaMode, opaque, mask, blend);
  KERN_STRUCT(Float_row) {
    List<Float> items = List<Float>{};
  };
  KERN_STRUCT(Material) {
    Vec4 color = Vec4{1.0, 1.0, 1.0, 1.0};
    AlphaMode alpha = AlphaMode::opaque;
    Float alpha_cutoff = 0.5;
    Float roughness = 1.0;
    Float metalness = 0.0;
    Vector emissive = Vector{0.0, 0.0, 0.0};
    Bool unlit = false;
    Bool double_sided = false;
    Bool cast_shadow = true;
    Bool frustum_culled = true;
    Bool viewmodel = false;
    Bool linear_colors = false;
    Bool lines = false;
    Fixed<I32, 8> params = Fixed<I32, 8>{};
  };
  KERN_STRUCT(MeshInput) {
    List<Float> positions = List<Float>{};
    List<Float> normals = List<Float>{};
    List<Float> colors = List<Float>{};
    List<Float> uvs = List<Float>{};
    List<U32> indices = List<U32>{};
    List<Float> joints = List<Float>{};
    List<Float> weights = List<Float>{};
    List<Float> bones = List<Float>{};
    Fixed<Float_row, 2> extension = Fixed<Float_row, 2>{};
    List<Float> transforms = List<Float>{};
    List<Float> tints = List<Float>{};
    Material material = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, Fixed<I32, 8>{}};
    U32 texture = 0;
    Bool visible = true;
    U32 layers = 1;
    Bool pick_bvh = true;
  };

  // ---- functions ----
  struct Material_casts_shadowResult { Bool Result = false; };
  KERN_PURE Material_casts_shadowResult Material_casts_shadow(const Material& self) {
    return {logic_and(logic_and(self.cast_shadow, enum_equal<AlphaMode>(self.alpha, AlphaMode::opaque)), logic_not(self.lines))};
  }
};

KERN_LAYOUT(engine_gfx_mesh) {
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 160, 150);
  node(4, 160, 240);
  node(5, 160, 330);
  node(6, 160, 420);
  node(7, 160, 510);
  node(8, 160, 600);
  node(9, 160, 690);
  next_id(10);
}

}  // namespace kern_blueprint_bb872f843a762d45

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_geo_mesh_data"
namespace kern_blueprint_c8b7695a1b35c491 {

using engine_gfx_mesh = ::kern_blueprint_bb872f843a762d45::engine_gfx_mesh;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_geo_mesh_data : virtual kern::Library, virtual engine_gfx_mesh {
  // ---- types ----
  KERN_STRUCT(MeshData) {
    List<Float> positions = List<Float>{};
    List<Float> normals = List<Float>{};
    List<Float> colors = List<Float>{};
    List<Float> uvs = List<Float>{};
    List<Float> extra = List<Float>{};
    U32 extra_size = 0;
    List<U32> indices = List<U32>{};
  };
  KERN_STRUCT(WeldStats) {
    U32 source_vertices = 0;
    U32 unique_vertices = 0;
    Int bytes_before = 0;
    Int bytes_after = 0;
    Bool indexed = false;
  };
  KERN_STRUCT(MeshLook) {
    Material material = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, Fixed<I32, 8>{}};
    U32 extra_slot = 0;
    U32 texture = 0;
    Bool visible = true;
    U32 layers = 1;
    Bool pick_bvh = true;
  };

  // ---- functions ----
  struct MeshData_vertex_countResult { U32 Result = 0; };
  KERN_PURE MeshData_vertex_countResult MeshData_vertex_count(const MeshData& self) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 3.0))); }, kern::to<Float>(list_length<Float>(self.positions))))};
  }

  struct MeshData_triangle_countResult { U32 Result = 0; };
  KERN_PURE MeshData_triangle_countResult MeshData_triangle_count(const MeshData& self) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return a != 0.0 ? kern::math::trunc(kern::math::div(a_2, 3.0)) : kern::math::wrap32(kern::math::trunc(kern::math::div(a_3, 3.0))); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<U32>(self.indices)), 0.0)), kern::to<Float>(MeshData_vertex_count(self).Result), kern::to<Float>(list_length<U32>(self.indices))))};
  }

  void MeshData_clear(MeshData& self) {
    list_clear(self.positions);
    list_clear(self.normals);
    list_clear(self.colors);
    list_clear(self.uvs);
    list_clear(self.extra);
    list_clear(self.indices);
  }

  void MeshData_push(MeshData& self, Vector p = Vector{0.0, 0.0, 0.0}, Vector n = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Vector uv = Vector{0.0, 0.0, 0.0}) {
    Int place = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Int place_2 = 0;
    Float item_4 = 0.0;
    Float item_5 = 0.0;
    Float item_6 = 0.0;
    Int place_3 = 0;
    Float item_7 = 0.0;
    Float item_8 = 0.0;
    Float item_9 = 0.0;
    Int place_4 = 0;
    Float item_10 = 0.0;
    Float item_11 = 0.0;
    place = list_length<Float>(self.positions);
    item = vector_break(p).x;
    item_2 = vector_break(p).y;
    item_3 = vector_break(p).z;
    list_insert_list(self.positions, place, list_make<Float, "3">(item, item_2, item_3));
    place_2 = list_length<Float>(self.normals);
    item_4 = vector_break(n).x;
    item_5 = vector_break(n).y;
    item_6 = vector_break(n).z;
    list_insert_list(self.normals, place_2, list_make<Float, "3">(item_4, item_5, item_6));
    place_3 = list_length<Float>(self.colors);
    item_7 = vector_break(c).x;
    item_8 = vector_break(c).y;
    item_9 = vector_break(c).z;
    list_insert_list(self.colors, place_3, list_make<Float, "3">(item_7, item_8, item_9));
    place_4 = list_length<Float>(self.uvs);
    item_10 = vector_break(uv).x;
    item_11 = vector_break(uv).y;
    list_insert_list(self.uvs, place_4, list_make<Float, "2">(item_10, item_11));
  }

  void MeshData_push_extra(MeshData& self, const List<Float>& values, Int values_at = 0) {
    Int place = 0;
    Int first = 0;
    Int last = 0;
    struct : kern::Node<10> { Int index = 0; } n10;
    place = list_length<Float>(self.extra);
    first = values_at;
    last = kern::to<Int>(kern::formula([](Int a) -> Int { return a - 1.0; }, value_int(kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return a + b; }, values_at, self.extra_size)))));
    for (auto it10 = flow_for(n10, first, last); it10.next();) {
      list_insert(self.extra, kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return a + b; }, place, value_int(kern::to<Int>(kern::formula([](Int a, Int b) -> Int { return a - b; }, n10.index, first))))), list_get<Float>(values, n10.index));
    }
  }

  struct MeshData_bytesResult { Int Result = 0; };
  KERN_PURE MeshData_bytesResult MeshData_bytes(const MeshData& self) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4, Float a_5, Float a_6, Int a_7) -> Float { return (a + a_2 + a_3 + a_4 + a_5) * 4.0 + a_6 * (a_7 <= 65536.0 ? 2.0 : 4.0); }, kern::to<Float>(list_length<Float>(self.positions)), kern::to<Float>(list_length<Float>(self.normals)), kern::to<Float>(list_length<Float>(self.colors)), kern::to<Float>(list_length<Float>(self.uvs)), kern::to<Float>(list_length<Float>(self.extra)), kern::to<Float>(list_length<U32>(self.indices)), MeshData_vertex_count(self).Result))};
  }

  struct stream_countResult { Bool Result = false; };
  /// Reads a non-negative integer count from a f64 stream value; false for NaN, negative, fractional or values above `limit` (never casts out of range).
  stream_countResult stream_count(U32& out, Float v = 0.0, Float limit = 0.0) {
    if (logic_or(logic_not(logic_and(math_compare<">=">(v, 0.0), math_compare<"<=">(v, limit))), math_compare<">">(kern::formula([](Float v, Float a) -> Float { return v != a; }, v, int64_to_float_unsigned(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, v)))), 0.5))) {
      return {false};
    }
    out = kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::trunc(v); }, v));
    return {true};
  }

  struct stream_u32Result { U32 Result = 0; };
  /// A u32 hash carried in a f64 (0 for anything that is not an integer in range).
  KERN_PURE stream_u32Result stream_u32(Float v = 0.0) {
    return {kern::to<Int>(kern::formula([](Float v) -> Float { return v >= 0.0 && v <= 4294967295.0 ? kern::math::trunc(v) : 0.0; }, v))};
  }
};

KERN_LAYOUT(engine_geo_mesh_data) {
  node(11, 0, 0);
  node(12, 280, 0);
  node(13, 160, 150);
  node(14, 160, 240);
  node(15, 160, 330);
  node(16, 0, 470);
  node(17, 280, 470);
  node(18, 160, 620);
  node(19, 160, 710);
  node(20, 160, 800);
  node(21, 160, 890);
  node(22, 160, 980);
  node(23, 160, 1070);
  node(24, 160, 1160);
  node(25, 0, 1300);
  node(26, 280, 1300);
  node(27, 560, 1300);
  node(28, 840, 1300);
  node(29, 1120, 1300);
  node(30, 1400, 1300);
  node(31, 1680, 1300);
  node(32, 0, 1500);
  node(33, 280, 1500);
  node(34, 160, 1650);
  node(35, 160, 1740);
  node(36, 560, 1500);
  node(37, 440, 1650);
  node(38, 840, 1500);
  node(39, 720, 1650);
  node(40, 1120, 1500);
  node(41, 1000, 1650);
  node(42, 1400, 1500);
  node(43, 1280, 1650);
  node(44, 1280, 1740);
  node(45, 1280, 1830);
  node(46, 1280, 1920);
  node(47, 1280, 2010);
  node(48, 1680, 1500);
  node(49, 1560, 1650);
  node(50, 1560, 1740);
  node(51, 1960, 1500);
  node(52, 1840, 1650);
  node(53, 2240, 1500);
  node(54, 2120, 1650);
  node(55, 2520, 1500);
  node(56, 2400, 1650);
  node(57, 2800, 1500);
  node(58, 2680, 1650);
  node(59, 2680, 1740);
  node(60, 2680, 1830);
  node(61, 2680, 1920);
  node(62, 2680, 2010);
  node(63, 3080, 1500);
  node(64, 2960, 1650);
  node(65, 2960, 1740);
  node(66, 3360, 1500);
  node(67, 3240, 1650);
  node(68, 3640, 1500);
  node(69, 3520, 1650);
  node(70, 3920, 1500);
  node(71, 3800, 1650);
  node(72, 4200, 1500);
  node(73, 4080, 1650);
  node(74, 4080, 1740);
  node(75, 4080, 1830);
  node(76, 4080, 1920);
  node(77, 4080, 2010);
  node(78, 4480, 1500);
  node(79, 4360, 1650);
  node(80, 4360, 1740);
  node(81, 4760, 1500);
  node(82, 4640, 1650);
  node(83, 5040, 1500);
  node(84, 4920, 1650);
  node(85, 5320, 1500);
  node(86, 5200, 1650);
  node(87, 5200, 1740);
  node(88, 5200, 1830);
  node(89, 5200, 1920);
  node(90, 0, 2150);
  node(91, 280, 2150);
  node(92, 160, 2300);
  node(93, 160, 2390);
  node(94, 560, 2150);
  node(95, 840, 2150);
  node(96, 720, 2300);
  node(97, 720, 2390);
  node(98, 720, 2480);
  node(99, 720, 2570);
  node(10, 1120, 2150);
  node(100, 1000, 2300);
  node(101, 1000, 2390);
  node(102, 1400, 2150);
  node(103, 1280, 2300);
  node(104, 1280, 2390);
  node(105, 1280, 2480);
  node(106, 1280, 2570);
  node(107, 1280, 2660);
  node(108, 1280, 2750);
  node(109, 1280, 2840);
  node(110, 0, 2980);
  node(111, 280, 2980);
  node(112, 160, 3130);
  node(113, 160, 3220);
  node(114, 160, 3310);
  node(115, 160, 3400);
  node(116, 160, 3490);
  node(117, 160, 3580);
  node(118, 160, 3670);
  node(119, 160, 3760);
  node(120, 160, 3850);
  node(121, 160, 3940);
  node(122, 160, 4030);
  node(123, 160, 4120);
  node(124, 160, 4210);
  node(125, 160, 4300);
  node(126, 0, 4440);
  node(127, 280, 4440);
  node(128, 160, 4590);
  node(129, 160, 4680);
  node(130, 160, 4770);
  node(131, 160, 4860);
  node(132, 160, 4950);
  node(133, 160, 5040);
  node(134, 160, 5130);
  node(135, 160, 5220);
  node(136, 160, 5310);
  node(137, 160, 5400);
  node(138, 560, 4440);
  node(139, 840, 4440);
  node(140, 720, 4590);
  node(141, 1120, 4440);
  node(142, 0, 5540);
  node(143, 280, 5540);
  node(144, 160, 5690);
  next_id(145);
}

}  // namespace kern_blueprint_c8b7695a1b35c491

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_geo_buildings"
namespace kern_blueprint_b3600570eb5cdba0 {

using engine_geo_mesh_data = ::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_geo_buildings : virtual kern::Library, virtual engine_geo_mesh_data {
  // ---- types ----
  KERN_STRUCT(Rgb) {
    Float r = 1.0;
    Float g = 1.0;
    Float b = 1.0;
  };
  KERN_STRUCT(BuildingStyle) {
    List<Rgb> walls = List<Rgb>{Rgb{0.75, 0.73, 0.7}};
    List<Rgb> roofs = List<Rgb>{Rgb{0.3, 0.3, 0.32}};
    Rgb trim = Rgb{0.957, 0.937, 0.894};
    Float trim_mix = 0.45;
  };
  KERN_STRUCT(GeoChunk) {
    I32 x = 0;
    I32 y = 0;
    U32 items = 0;
    U32 triangles = 0;
    U32 hash = 0;
    Bool welded = false;
    MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
  };
  KERN_STRUCT(BuildingOptions) {
    Float chunk_size = 320.0;
    U32 max_buildings = 2600;
    Bool cornice = true;
    Bool wall_records = true;
    BuildingStyle style = BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45};
  };

  // ---- functions ----
  struct Rgb_fResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Rgb_fResult Rgb_f(const Rgb& self) {
    return {vector_make(self.r, self.g, self.b)};
  }
};

KERN_LAYOUT(engine_geo_buildings) {
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 160, 150);
  node(4, 160, 240);
  node(5, 160, 330);
  node(6, 160, 420);
  next_id(7);
}

}  // namespace kern_blueprint_b3600570eb5cdba0

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_geo_roads"
namespace kern_blueprint_d57e83ccc1e29143 {

using engine_geo_buildings = ::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_geo_roads : virtual kern::Library, virtual engine_geo_buildings {
  // ---- types ----
  KERN_STRUCT(GeoProjector) {
    Float lon0 = 0.0;
    Float lat0 = 0.0;
    Float cos_lat = 1.0;
  };
  KERN_STRUCT(RoadOptions) {
    Float center_x = 0.0;
    Float center_y = 0.0;
    Float radius = 800.0;
    Float chunk_size = 420.0;
    Float segment_length = 6.0;
    Float curb_extra = 3.4;
    Float curb_z = 0.09;
    Float surface_z = 0.112;
    Float marking_z = 0.135;
    Float marking_width = 0.42;
    Float dash_start = 1.0;
    Float dash_length = 3.0;
    Float dash_period = 7.0;
    Float uv_scale = 0.2;
    Rgb curb_color = Rgb{0.525, 0.514, 0.486};
    List<Rgb> palette = List<Rgb>{};
    Callback<Bool(Float, Float, Float)> deformed = Callback<Bool(Float, Float, Float)>{};
    Float fine_step = 0.9;
  };

  // ---- variables ----
  KERN_CONST Float kRad = 0.017453292519943295;
  KERN_CONST Float kEarthRadius = 6378137.0;

  // ---- functions ----
  struct GeoProjector_initResult { Bool Result = false; };
  GeoProjector_initResult GeoProjector_init(GeoProjector& self, Float lon = 0.0, Float lat = 0.0) {
    Bool fail = false;
    struct { Bool result = false; } n10;
    if (logic_or(logic_or(logic_or(logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, lon), 0.0)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, lat), 0.0))), math_compare<">">(kern::formula([](Float lon) -> Float { return kern::math::abs(lon); }, lon), 180.0)), math_compare<">">(kern::formula([](Float lat) -> Float { return kern::math::abs(lat); }, lat), 85.0))) {
      engine_fail(n10, text_format<"invalid projection origin">(-1));
      fail = n10.result;
      return {fail};
    }
    self.lon0 = lon;
    self.lat0 = lat;
    self.cos_lat = kern::formula([](Float lat, Float kRad) -> Float { return kern::math::max(0.01, kern::math::cos(lat * kRad)); }, lat, kRad);
    return {true};
  }

  void GeoProjector_project(const GeoProjector& self, Float& x, Float& y, Float lon = 0.0, Float lat = 0.0) {
    x = kern::formula([](Float lon, Float lon0, Float kRad, Float kEarthRadius, Float cos_lat) -> Float { return (lon - lon0) * kRad * kEarthRadius * cos_lat; }, lon, self.lon0, kRad, kEarthRadius, self.cos_lat);
    y = kern::formula([](Float lat, Float lat0, Float kRad, Float kEarthRadius) -> Float { return (lat - lat0) * kRad * kEarthRadius; }, lat, self.lat0, kRad, kEarthRadius);
  }
};

KERN_LAYOUT(engine_geo_roads) {
  node(11, 0, 0);
  node(12, 280, 0);
  node(13, 160, 150);
  node(14, 160, 240);
  node(15, 160, 330);
  node(16, 160, 420);
  node(17, 160, 510);
  node(18, 160, 600);
  node(19, 160, 690);
  node(20, 160, 780);
  node(21, 160, 870);
  node(22, 160, 960);
  node(23, 160, 1050);
  node(24, 160, 1140);
  node(25, 160, 1230);
  node(10, 560, 0);
  node(26, 440, 150);
  node(27, 840, 0);
  node(28, 1120, 0);
  node(29, 1000, 150);
  node(30, 1400, 0);
  node(31, 1680, 0);
  node(32, 1960, 0);
  node(33, 1840, 150);
  node(34, 1840, 240);
  node(35, 2240, 0);
  node(36, 0, 1370);
  node(37, 280, 1370);
  node(38, 160, 1520);
  node(39, 160, 1610);
  node(40, 160, 1700);
  node(41, 160, 1790);
  node(42, 160, 1880);
  node(43, 560, 1370);
  node(44, 440, 1520);
  node(45, 440, 1610);
  node(46, 440, 1700);
  node(47, 440, 1790);
  next_id(48);
}

}  // namespace kern_blueprint_d57e83ccc1e29143

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_gfx_shader"
namespace kern_blueprint_1ce45fbbb3818596 {

using engine_gfx_mesh = ::kern_blueprint_bb872f843a762d45::engine_gfx_mesh;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_gfx_shader : virtual kern::Library, virtual engine_gfx_mesh {
  // ---- types ----
  KERN_STRUCT(ExtensionAttribute) {
    Text key = "";
    U32 size = 0;
    Text glsl = "";
  };
  KERN_STRUCT(ShaderSource) {
    Text vertex = "";
    Text fragment = "";
  };

  // ---- functions ----
  struct ShaderExtension_attributesResult { List<ExtensionAttribute> Result = List<ExtensionAttribute>{}; };
  /// Extra vertex streams (at most kMaxExtensionAttributes over all extensions).
  KERN_PURE ShaderExtension_attributesResult ShaderExtension_attributes() {
    return {List<ExtensionAttribute>{}};
  }

  struct ShaderExtension_vertex_declarationsResult { Text Result = ""; };
  KERN_PURE ShaderExtension_vertex_declarationsResult ShaderExtension_vertex_declarations() {
    return {""};
  }

  struct ShaderExtension_vertexResult { Text Result = ""; };
  KERN_PURE ShaderExtension_vertexResult ShaderExtension_vertex() {
    return {""};
  }

  struct ShaderExtension_fragment_declarationsResult { Text Result = ""; };
  KERN_PURE ShaderExtension_fragment_declarationsResult ShaderExtension_fragment_declarations() {
    return {""};
  }

  struct ShaderExtension_fragment_discardResult { Text Result = ""; };
  KERN_PURE ShaderExtension_fragment_discardResult ShaderExtension_fragment_discard() {
    return {""};
  }

  struct ShaderExtension_fragment_inputResult { Text Result = ""; };
  KERN_PURE ShaderExtension_fragment_inputResult ShaderExtension_fragment_input() {
    return {""};
  }

  struct ShaderExtension_fragment_surfaceResult { Text Result = ""; };
  KERN_PURE ShaderExtension_fragment_surfaceResult ShaderExtension_fragment_surface() {
    return {""};
  }

  /// Resolve uniform locations after the standard program (re)links.
  KERN_PURE void ShaderExtension_bind(Int program = 0) {
  }

  /// Per draw. `params` points at this extension's material parameters.
  KERN_PURE void ShaderExtension_material_uniforms(const Material& material, const List<I32>& params, Int cache = 0, Int params_at = 0) {
  }
};

KERN_LAYOUT(engine_gfx_shader) {
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 0, 200);
  node(4, 280, 200);
  node(5, 0, 400);
  node(6, 280, 400);
  node(7, 0, 600);
  node(8, 280, 600);
  node(9, 0, 800);
  node(10, 280, 800);
  node(11, 0, 1000);
  node(12, 280, 1000);
  node(13, 0, 1200);
  node(14, 280, 1200);
  node(15, 0, 1400);
  node(16, 0, 1600);
  next_id(17);
}

}  // namespace kern_blueprint_1ce45fbbb3818596

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_logic_logic"
namespace kern_blueprint_f26fb4f0474c453a {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_logic_logic : virtual kern::Library {
  // ---- variables ----
  /// Whole numbers of a fixed width. What a place is declared as - a variable, a field of a struct, an input, an output or a local variable of a function - can be an Int of a fixed width: TypeRef::of beside PinType::integer names it - "u8", "u16", "u32" (0 .. 2^w - 1) and "i8", "i16", "i32" (-2^(w-1) .. 2^(w-1) - 1); in a Blueprint's C++ it is U8 ... I32 (std::uint8_t ...). What flows through a pin is an Int: a pin has no width. What such a place is given, it holds as C++ holds it when it stores an integer there (static_cast<u32>(x)): the low w bits of the Int, of a signed width as two's complement. Every value of a width is a whole number below 2^32 in size, which a double holds exactly - so the interpreter and the JIT, whose numbers are doubles, hold it exactly too.
  KERN_CONST Fixed<Text, 6> kWidthNames = Fixed<Text, 6>{"u8", "u16", "u32", "i8", "i16", "i32"};

  // ---- functions ----
  struct width_bitsResult { I32 Result = 0; };
  /// The bits of the width `of` names (8, 16, 32; negative: a signed width); 0: it names none.
  KERN_PURE width_bitsResult width_bits(Text of = "") {
    Fixed<I32, 6> kBits = Fixed<I32, 6>{};
    struct : kern::Node<10> { Int index = 0; } n10;
    kBits = list_make<I32, "6">(8, 16, 32, -8, -16, -32);
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, 6.0))); it10.next();) {
      if (text_equal(of, list_get<Text>(kWidthNames, n10.index))) {
        return {kBits[n10.index]};
      }
    }
    return {0};
  }

  struct wrappedResult { Int64 Result = Int64{0x0}; };
  /// What a place of the width `of` holds when it is given the Int x: its low bits (none: x).
  KERN_PURE wrappedResult wrapped(Int64 x = Int64{0x0}, Text of = "") {
    Int64 bits = Int64{0x0};
    Int selected = 0;
    bits = x;
    selected = width_bits(of).Result;
    if (math_compare<"==">(kern::to<Float>(selected), 8.0)) {
      return {int64_of_int(kern::to<Int>(kern::formula([](Int x) -> Int { return kern::math::band(x, 255.0); }, int64_low(bits))))};
    }
    if (math_compare<"==">(kern::to<Float>(selected), 16.0)) {
      return {int64_of_int(kern::to<Int>(kern::formula([](Int x) -> Int { return kern::math::band(x, 65535.0); }, int64_low(bits))))};
    }
    if (math_compare<"==">(kern::to<Float>(selected), 32.0)) {
      return {int64_of_int(int64_low(bits))};
    }
    if (math_compare<"==">(kern::to<Float>(selected), -8.0)) {
      return {int64_of_int(kern::to<Int>(kern::formula([](Int a) -> Float { return kern::math::bxor(kern::math::band(a, 255.0), 128.0) - 128.0; }, value_int(kern::to<Int>(kern::formula([](Int x) -> Int { return kern::math::band(x, 255.0); }, int64_low(bits)))))))};
    }
    if (math_compare<"==">(kern::to<Float>(selected), -16.0)) {
      return {int64_of_int(kern::to<Int>(kern::formula([](Int a) -> Float { return kern::math::bxor(kern::math::band(a, 65535.0), 32768.0) - 32768.0; }, value_int(kern::to<Int>(kern::formula([](Int x) -> Int { return kern::math::band(x, 65535.0); }, int64_low(bits)))))))};
    }
    if (math_compare<"==">(kern::to<Float>(selected), -32.0)) {
      return {int64_of_int(kern::to<Int>(kern::formula([](Int a) -> Int { return kern::math::wrap32s(a); }, int64_low(bits))))};
    }
    return {x};
  }
};

KERN_LAYOUT(engine_logic_logic) {
  node(11, 0, 0);
  node(12, 280, 0);
  node(13, 160, 150);
  node(10, 560, 0);
  node(14, 440, 150);
  node(15, 840, 0);
  node(16, 720, 150);
  node(17, 720, 240);
  node(18, 720, 330);
  node(19, 1120, 0);
  node(20, 1000, 150);
  node(21, 1400, 0);
  node(22, 0, 470);
  node(23, 280, 470);
  node(24, 560, 470);
  node(25, 440, 620);
  node(26, 840, 470);
  node(27, 720, 620);
  node(28, 720, 710);
  node(29, 1120, 470);
  node(30, 1000, 620);
  node(31, 1000, 710);
  node(32, 1000, 800);
  node(33, 1000, 890);
  node(34, 1400, 470);
  node(35, 1280, 620);
  node(36, 1280, 710);
  node(37, 1680, 470);
  node(38, 1560, 620);
  node(39, 1560, 710);
  node(40, 1560, 800);
  node(41, 1560, 890);
  node(42, 1960, 470);
  node(43, 1840, 620);
  node(44, 1840, 710);
  node(45, 2240, 470);
  node(46, 2120, 620);
  node(47, 2120, 710);
  node(48, 2120, 800);
  node(49, 2520, 470);
  node(50, 2400, 620);
  node(51, 2400, 710);
  node(52, 2800, 470);
  node(53, 2680, 620);
  node(54, 2680, 710);
  node(55, 2680, 800);
  node(56, 2680, 890);
  node(57, 2680, 980);
  node(58, 2680, 1070);
  node(59, 3080, 470);
  node(60, 2960, 620);
  node(61, 2960, 710);
  node(62, 3360, 470);
  node(63, 3240, 620);
  node(64, 3240, 710);
  node(65, 3240, 800);
  node(66, 3240, 890);
  node(67, 3240, 980);
  node(68, 3240, 1070);
  node(69, 3640, 470);
  node(70, 3520, 620);
  node(71, 3520, 710);
  node(72, 3920, 470);
  node(73, 3800, 620);
  node(74, 3800, 710);
  node(75, 3800, 800);
  node(76, 3800, 890);
  node(77, 4200, 470);
  next_id(78);
}

}  // namespace kern_blueprint_f26fb4f0474c453a

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_net_reliable"
namespace kern_blueprint_d7bae0e5d6425fab {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_net_reliable : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(net_EndpointConfig) {
    I32 max_packet_size = 16384;
    I32 fragment_above = 1024;
    I32 fragment_size = 1024;
    I32 max_fragments = 16;
    I32 ack_buffer_size = 256;
    I32 sent_packets_buffer_size = 256;
    I32 received_packets_buffer_size = 256;
    I32 fragment_reassembly_buffer_size = 64;
    Float rtt_smoothing = 0.1;
    Float packet_loss_smoothing = 0.1;
    Float bandwidth_smoothing = 0.1;
    I32 packet_header_size = 28;
  };
  KERN_STRUCT(net_EndpointStats) {
    Float rtt_ms = 0.0;
    Float packet_loss = 0.0;
    Float sent_kbps = 0.0;
    Float received_kbps = 0.0;
    Float acked_kbps = 0.0;
    Int64 packets_sent = Int64{0x0};
    Int64 packets_received = Int64{0x0};
    Int64 packets_acked = Int64{0x0};
    Int64 packets_stale = Int64{0x0};
    Int64 packets_invalid = Int64{0x0};
    Int64 packets_too_large_to_send = Int64{0x0};
    Int64 packets_too_large_to_receive = Int64{0x0};
    Int64 fragments_sent = Int64{0x0};
    Int64 fragments_received = Int64{0x0};
    Int64 fragments_invalid = Int64{0x0};
  };
};

KERN_LAYOUT(engine_net_reliable) {
  next_id(1);
}

}  // namespace kern_blueprint_d7bae0e5d6425fab

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_net_connection"
namespace kern_blueprint_98bcbda0c963ae2a {

using engine_net_reliable = ::kern_blueprint_d7bae0e5d6425fab::engine_net_reliable;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_net_connection : virtual kern::Library, virtual engine_net_reliable {
  // ---- types ----
  KERN_ENUM(net_ChannelType, reliable_ordered, unreliable_unordered);
  KERN_ENUM(net_ConnectionError, none, desync, send_queue_full, message_too_large, bad_packet, out_of_memory);
  KERN_STRUCT(net_Message) {
    U16 id = 0;
    U16 type = 0;
    List<U8> data = List<U8>{};
  };
  KERN_STRUCT(net_ChannelConfig) {
    net_ChannelType type = net_ChannelType::reliable_ordered;
    I32 send_queue_size = 1024;
    I32 receive_queue_size = 1024;
    I32 max_messages_per_packet = 256;
    I32 packet_budget = 0;
    I32 max_message_size = 65536;
    I32 block_threshold = 512;
    I32 block_fragment_size = 1024;
    Float resend_time = 0.1;
  };
  KERN_STRUCT(net_ConnectionConfig) {
    List<net_ChannelConfig> channels = List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}};
    I32 max_packet_size = 8192;
    I32 max_message_types = 256;
    net_EndpointConfig endpoint = net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28};
  };
};

KERN_LAYOUT(engine_net_connection) {
  next_id(1);
}

}  // namespace kern_blueprint_98bcbda0c963ae2a

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_net_client_server"
namespace kern_blueprint_63e9260b4af96bb7 {

using engine_net_connection = ::kern_blueprint_98bcbda0c963ae2a::engine_net_connection;

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_net_client_server : virtual kern::Library, virtual engine_net_connection {
  // ---- types ----
  KERN_ENUM(net_ClientState, denied, timed_out, error, disconnected, connecting, challenged, connected);
  KERN_STRUCT(net_ClientServerConfig) {
    Int64 protocol_id = Int64{0x4b45524e30303031};
    I32 max_clients = 16;
    Float timeout = 6.0;
    Float request_interval = 0.1;
    Float connect_timeout = 6.0;
    Float keepalive_interval = 0.1;
    net_ConnectionConfig connection = net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}};
  };
};

KERN_LAYOUT(engine_net_client_server) {
  next_id(1);
}

}  // namespace kern_blueprint_63e9260b4af96bb7

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_net_transport"
namespace kern_blueprint_21ccd5517d15e265 {

// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct engine_net_transport : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(net_Address) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(net_SimulatorSettings) {
    Float latency_ms = 0.0;
    Float jitter_ms = 0.0;
    Float loss_percent = 0.0;
    Float duplicate_percent = 0.0;
  };
  KERN_STRUCT(net_Simulator_Packet) {
    net_Address from = net_Address{Int64{0x0}};
    net_Address to = net_Address{Int64{0x0}};
    Float deliver = 0.0;
    List<U8> data = List<U8>{};
  };

  // ---- functions ----
  struct Address_validResult { Bool Result = false; };
  KERN_PURE Address_validResult Address_valid(const net_Address& self) {
    return {logic_not(int64_equal(self.id, Int64{0x0}))};
  }

  /// Moves simulated time (delivers delayed packets).
  KERN_PURE void Transport_advance_time(Float time = 0.0) {
  }

  struct SimulatorSettings_activeResult { Bool Result = false; };
  KERN_PURE SimulatorSettings_activeResult SimulatorSettings_active(const net_SimulatorSettings& self) {
    return {logic_or(logic_or(logic_or(math_compare<">">(self.latency_ms, 0.0), math_compare<">">(self.jitter_ms, 0.0)), math_compare<">">(self.loss_percent, 0.0)), math_compare<">">(self.duplicate_percent, 0.0))};
  }
};

KERN_LAYOUT(engine_net_transport) {
  node(1, 0, 0);
  node(2, 280, 0);
  node(3, 160, 150);
  node(4, 160, 240);
  node(5, 160, 330);
  node(6, 0, 470);
  node(7, 0, 670);
  node(8, 280, 670);
  node(9, 160, 820);
  node(10, 160, 910);
  node(11, 160, 1000);
  node(12, 160, 1090);
  node(13, 160, 1180);
  node(14, 160, 1270);
  node(15, 160, 1360);
  node(16, 160, 1450);
  node(17, 160, 1540);
  node(18, 160, 1630);
  node(19, 160, 1720);
  next_id(20);
}

}  // namespace kern_blueprint_21ccd5517d15e265

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_physics_physics"
namespace kern_blueprint_c124d3ceac6e0826 {

using engine_math_math = ::kern_blueprint_64c1acf056c7521a::engine_math_math;

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
  KERN_STRUCT(physics_Capsule) {
    Vector center1 = Vector{0.0, 0.0, 0.0};
    Vector center2 = Vector{0.0, 0.0, 0.0};
    Float radius = 0.3;
  };
  KERN_STRUCT(physics_CharacterMover_Settings) {
    Float radius = 0.4;
    Float height = 1.8;
    Float step_height = 0.45;
    Float max_speed = 6.0;
    Float accelerate = 10.0;
    Float friction = 8.0;
    Float air_control = 0.25;
    Float stop_speed = 3.0;
    Float jump_speed = 6.0;
    Float sprint = 1.6;
    Float gravity = 20.0;
    Float max_slope_deg = 50.0;
    Float push_strength = 1.0;
    Int64 mask_bits = Int64{0xffffffffffffffff};
  };
  KERN_STRUCT(physics_ContactPoint) {
    Vector point = Vector{0.0, 0.0, 0.0};
    Vector normal = Vector{0.0, 0.0, 0.0};
    Float separation = 0.0;
    Float impulse = 0.0;
  };
  KERN_STRUCT(physics_Joint) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(physics_MotionLocks) {
    Fixed<Bool, 3> linear = Fixed<Bool, 3>{};
    Fixed<Bool, 3> angular = Fixed<Bool, 3>{};
  };
  KERN_STRUCT(physics_QueryFilter) {
    Int64 mask_bits = Int64{0xffffffffffffffff};
    Int64 category_bits = Int64{0xffffffffffffffff};
  };
  KERN_STRUCT(physics_Shape) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(physics_Version) {
    I32 major = 0;
    I32 minor = 0;
    I32 revision = 0;
  };
  KERN_STRUCT(physics_WorldSettings) {
    Vector gravity = Vector{0.0, 0.0, -9.80665};
    Bool enable_sleep = true;
    Bool enable_continuous = true;
    Float restitution_threshold = 1.0;
    I32 restitution_iterations = 2;
    Bool restitution_propagation = false;
    Float hit_event_threshold = 1.0;
    Float contact_hertz = 30.0;
    Float contact_damping_ratio = 10.0;
    Float contact_speed = 3.0;
    Float maximum_linear_speed = 400.0;
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
  KERN_STRUCT(physics_HitEvent) {
    physics_Shape shape_a = physics_Shape{Int64{0x0}};
    physics_Shape shape_b = physics_Shape{Int64{0x0}};
    physics_Body body_a = physics_Body{Int64{0x0}};
    physics_Body body_b = physics_Body{Int64{0x0}};
    Vector point = Vector{0.0, 0.0, 0.0};
    Vector normal = Vector{0.0, 0.0, 0.0};
    Float approach_speed = 0.0;
  };
  KERN_STRUCT(physics_MoverPlane) {
    Vector normal = Vector{0.0, 0.0, 0.0};
    Float offset = 0.0;
    Vector point = Vector{0.0, 0.0, 0.0};
    physics_Shape shape = physics_Shape{Int64{0x0}};
    physics_Body body = physics_Body{Int64{0x0}};
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
    I32 group_index = 0;
    Bool contact_events = true;
    Bool sensor_events = true;
    Bool hit_events = false;
    Int64 user = Int64{0x0};
    physics_Surface surface = physics_Surface::none;
  };
  KERN_STRUCT(physics_TouchEvent) {
    physics_Shape shape_a = physics_Shape{Int64{0x0}};
    physics_Shape shape_b = physics_Shape{Int64{0x0}};
    physics_Body body_a = physics_Body{Int64{0x0}};
    physics_Body body_b = physics_Body{Int64{0x0}};
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
    List<U32> indices = List<U32>{};
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

  struct Shape_equalResult { Bool Result = false; };
  KERN_PURE Shape_equalResult Shape_equal(const physics_Shape& self, const physics_Shape& o) {
    return {int64_equal(self.id, o.id)};
  }

  struct Joint_validResult { Bool Result = false; };
  KERN_PURE Joint_validResult Joint_valid(const physics_Joint& self) {
    return {logic_not(int64_equal(self.id, Int64{0x0}))};
  }

  struct Joint_equalResult { Bool Result = false; };
  KERN_PURE Joint_equalResult Joint_equal(const physics_Joint& self, const physics_Joint& o) {
    return {int64_equal(self.id, o.id)};
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
  node(4, 160, 240);
  node(5, 160, 330);
  node(6, 0, 470);
  node(7, 280, 470);
  node(8, 160, 620);
  node(9, 160, 710);
  node(10, 160, 800);
  node(11, 0, 940);
  node(12, 280, 940);
  node(13, 160, 1090);
  node(14, 160, 1180);
  node(15, 160, 1270);
  node(16, 0, 1410);
  node(17, 280, 1410);
  node(18, 160, 1560);
  node(19, 160, 1650);
  node(20, 160, 1740);
  node(21, 0, 1880);
  node(22, 280, 1880);
  node(23, 160, 2030);
  node(24, 160, 2120);
  node(25, 160, 2210);
  node(26, 0, 2350);
  node(27, 280, 2350);
  node(28, 160, 2500);
  node(29, 160, 2590);
  node(30, 160, 2680);
  node(31, 0, 2820);
  node(32, 280, 2820);
  node(33, 560, 2820);
  node(34, 840, 2820);
  node(35, 720, 2970);
  next_id(36);
}

}  // namespace kern_blueprint_c124d3ceac6e0826
