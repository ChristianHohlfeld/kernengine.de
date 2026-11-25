// The glue of the classes of the engine's own libraries (kern/logic/engine_libraries.gen.h): written by
// tools/blueprint_cpp --engine-classes - not edited by hand.
#include "kern/logic/blueprint.h"
#include "kern/logic/blueprint_class.h"
#include "kern/logic/engine_libraries.gen.h"

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_math_math" (its class: in the header)
namespace kern_blueprint_64c1acf056c7521a {

// How the engine reaches the Blueprint engine_math_math by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_64c1acf056c7521a
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_64c1acf056c7521a::engine_math_math::Aabb> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)); }
  static ::kern_blueprint_64c1acf056c7521a::engine_math_math::Aabb from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1))};
  }
  static Value to(const ::kern_blueprint_64c1acf056c7521a::engine_math_math::Aabb& s) { return Value::structure({to_value(s.the_min), to_value(s.the_max)}); }
};
template <> struct Held<::kern_blueprint_64c1acf056c7521a::engine_math_math::Mat4> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::List<kern::f64>>(*v.field(0)); }
  static ::kern_blueprint_64c1acf056c7521a::engine_math_math::Mat4 from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0))};
  }
  static Value to(const ::kern_blueprint_64c1acf056c7521a::engine_math_math::Mat4& s) { return Value::structure({to_value(s.m)}); }
};
template <> struct Held<::kern_blueprint_64c1acf056c7521a::engine_math_math::Quat> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::kern_blueprint_64c1acf056c7521a::engine_math_math::Quat from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_64c1acf056c7521a::engine_math_math::Quat& s) { return Value::structure({to_value(s.x), to_value(s.y), to_value(s.z), to_value(s.w)}); }
};
template <> struct Held<::kern_blueprint_64c1acf056c7521a::engine_math_math::Sphere> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::f64>(*v.field(1)); }
  static ::kern_blueprint_64c1acf056c7521a::engine_math_math::Sphere from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::f64>(*v.field(1))};
  }
  static Value to(const ::kern_blueprint_64c1acf056c7521a::engine_math_math::Sphere& s) { return Value::structure({to_value(s.center), to_value(s.radius)}); }
};
template <> struct Held<::kern_blueprint_64c1acf056c7521a::engine_math_math::Vec4> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::kern_blueprint_64c1acf056c7521a::engine_math_math::Vec4 from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_64c1acf056c7521a::engine_math_math::Vec4& s) { return Value::structure({to_value(s.x), to_value(s.y), to_value(s.z), to_value(s.w)}); }
};
template <> struct Held<::kern_blueprint_64c1acf056c7521a::engine_math_math::Camera> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 10 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<::kern_blueprint_64c1acf056c7521a::engine_math_math::Mat4>(*v.field(9)); }
  static ::kern_blueprint_64c1acf056c7521a::engine_math_math::Camera from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<::kern_blueprint_64c1acf056c7521a::engine_math_math::Mat4>(*v.field(9))};
  }
  static Value to(const ::kern_blueprint_64c1acf056c7521a::engine_math_math::Camera& s) { return Value::structure({to_value(s.eye), to_value(s.target), to_value(s.up), to_value(s.fov), to_value(s.near), to_value(s.far), to_value(s.orthographic), to_value(s.ortho_height), to_value(s.has_projection), to_value(s.projection)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_64c1acf056c7521a {
namespace {
struct engine_math_math_Native final : kern::logic::NativeBlueprint {
  engine_math_math self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_math_math> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "dot") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.dot(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "cross") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.cross(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "length") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.length(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Vec3_load") {
      kern::logic::NativeParam<&engine_math_math::Vec3_load, 0> inner0{};
      auto& place0 = busy_Vec3_load ? inner0 : place_Vec3_load_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Vec3_load, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Vec3_load, 0>{});
      kern::i64 in1 = native_arg<kern::i64>(args, 1, 0);
      const bool was_busy = busy_Vec3_load;
      busy_Vec3_load = true;
      kern::logic::native_call(true);
      const auto out = self.Vec3_load(place0, std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Vec3_load = was_busy;
      return true;
    }
    if (function == "dot_2") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.dot_2(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "cross_2") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.cross_2(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "length_2") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.length_2(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "length_sq") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.length_sq(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "distance") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.distance(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "distance_sq") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.distance_sq(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "finite") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.finite(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "the_lerp") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::f64 in2 = native_arg<kern::f64>(args, 2, 0.0);
      kern::logic::native_call(true);
      const auto out = self.the_lerp(std::move(in0), std::move(in1), std::move(in2));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "vmin") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.vmin(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "vmax") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.vmax(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "normalize") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.normalize(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "hex_color") {
      kern::i64 in0 = native_arg<kern::i64>(args, 0, 0);
      kern::logic::native_call(true);
      const auto out = self.hex_color(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Vec4_new") {
      kern::f64 in0 = native_arg<kern::f64>(args, 0, 0.0);
      kern::f64 in1 = native_arg<kern::f64>(args, 1, 0.0);
      kern::f64 in2 = native_arg<kern::f64>(args, 2, 0.0);
      kern::f64 in3 = native_arg<kern::f64>(args, 3, 0.0);
      kern::logic::native_call(true);
      const auto out = self.Vec4_new(std::move(in0), std::move(in1), std::move(in2), std::move(in3));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Vec4_new_2") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::f64 in1 = native_arg<kern::f64>(args, 1, 0.0);
      kern::logic::native_call(true);
      const auto out = self.Vec4_new_2(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Vec4_xyz") {
      kern::logic::NativeParam<&engine_math_math::Vec4_xyz, 0> inner0{};
      auto& place0 = busy_Vec4_xyz ? inner0 : place_Vec4_xyz_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Vec4_xyz, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Vec4_xyz, 0>{});
      const bool was_busy = busy_Vec4_xyz;
      busy_Vec4_xyz = true;
      kern::logic::native_call(true);
      const auto out = self.Vec4_xyz(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Vec4_xyz = was_busy;
      return true;
    }
    if (function == "Vec4_equal") {
      kern::logic::NativeParam<&engine_math_math::Vec4_equal, 0> inner0{};
      auto& place0 = busy_Vec4_equal ? inner0 : place_Vec4_equal_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Vec4_equal, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Vec4_equal, 0>{});
      kern::logic::NativeParam<&engine_math_math::Vec4_equal, 1> inner1{};
      auto& place1 = busy_Vec4_equal ? inner1 : place_Vec4_equal_1;
      place1 = native_arg<kern::logic::NativeParam<&engine_math_math::Vec4_equal, 1>>(args, 1, kern::logic::NativeParam<&engine_math_math::Vec4_equal, 1>{});
      const bool was_busy = busy_Vec4_equal;
      busy_Vec4_equal = true;
      kern::logic::native_call(true);
      const auto out = self.Vec4_equal(place0, place1);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Vec4_equal = was_busy;
      return true;
    }
    if (function == "Quat_axis_angle") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::f64 in1 = native_arg<kern::f64>(args, 1, 0.0);
      kern::logic::native_call(true);
      const auto out = self.Quat_axis_angle(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Quat_mul") {
      kern::logic::NativeParam<&engine_math_math::Quat_mul, 0> inner0{};
      auto& place0 = busy_Quat_mul ? inner0 : place_Quat_mul_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Quat_mul, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Quat_mul, 0>{});
      kern::logic::NativeParam<&engine_math_math::Quat_mul, 1> inner1{};
      auto& place1 = busy_Quat_mul ? inner1 : place_Quat_mul_1;
      place1 = native_arg<kern::logic::NativeParam<&engine_math_math::Quat_mul, 1>>(args, 1, kern::logic::NativeParam<&engine_math_math::Quat_mul, 1>{});
      const bool was_busy = busy_Quat_mul;
      busy_Quat_mul = true;
      kern::logic::native_call(true);
      const auto out = self.Quat_mul(place0, place1);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Quat_mul = was_busy;
      return true;
    }
    if (function == "Quat_rotate") {
      kern::logic::NativeParam<&engine_math_math::Quat_rotate, 0> inner0{};
      auto& place0 = busy_Quat_rotate ? inner0 : place_Quat_rotate_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Quat_rotate, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Quat_rotate, 0>{});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      const bool was_busy = busy_Quat_rotate;
      busy_Quat_rotate = true;
      kern::logic::native_call(true);
      const auto out = self.Quat_rotate(place0, std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Quat_rotate = was_busy;
      return true;
    }
    if (function == "Mat4_identity") {
      kern::logic::native_call(true);
      const auto out = self.Mat4_identity();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Mat4_load") {
      kern::logic::NativeParam<&engine_math_math::Mat4_load, 0> inner0{};
      auto& place0 = busy_Mat4_load ? inner0 : place_Mat4_load_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Mat4_load, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Mat4_load, 0>{});
      kern::i64 in1 = native_arg<kern::i64>(args, 1, 0);
      const bool was_busy = busy_Mat4_load;
      busy_Mat4_load = true;
      kern::logic::native_call(true);
      const auto out = self.Mat4_load(place0, std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Mat4_load = was_busy;
      return true;
    }
    if (function == "Mat4_at_2") {
      kern::logic::NativeParam<&engine_math_math::Mat4_at_2, 0> inner0{};
      auto& place0 = busy_Mat4_at_2 ? inner0 : place_Mat4_at_2_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Mat4_at_2, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Mat4_at_2, 0>{});
      kern::i64 in1 = native_arg<kern::i64>(args, 1, 0);
      const bool was_busy = busy_Mat4_at_2;
      busy_Mat4_at_2 = true;
      kern::logic::native_call(true);
      const auto out = self.Mat4_at_2(place0, std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Mat4_at_2 = was_busy;
      return true;
    }
    if (function == "Mat4_translation") {
      kern::logic::NativeParam<&engine_math_math::Mat4_translation, 0> inner0{};
      auto& place0 = busy_Mat4_translation ? inner0 : place_Mat4_translation_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Mat4_translation, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Mat4_translation, 0>{});
      const bool was_busy = busy_Mat4_translation;
      busy_Mat4_translation = true;
      kern::logic::native_call(true);
      const auto out = self.Mat4_translation(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Mat4_translation = was_busy;
      return true;
    }
    if (function == "Mat4_mul") {
      kern::logic::NativeParam<&engine_math_math::Mat4_mul, 0> inner0{};
      auto& place0 = busy_Mat4_mul ? inner0 : place_Mat4_mul_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Mat4_mul, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Mat4_mul, 0>{});
      kern::logic::NativeParam<&engine_math_math::Mat4_mul, 1> inner1{};
      auto& place1 = busy_Mat4_mul ? inner1 : place_Mat4_mul_1;
      place1 = native_arg<kern::logic::NativeParam<&engine_math_math::Mat4_mul, 1>>(args, 1, kern::logic::NativeParam<&engine_math_math::Mat4_mul, 1>{});
      const bool was_busy = busy_Mat4_mul;
      busy_Mat4_mul = true;
      kern::logic::native_call(true);
      const auto out = self.Mat4_mul(place0, place1);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Mat4_mul = was_busy;
      return true;
    }
    if (function == "Mat4_transform_point") {
      kern::logic::NativeParam<&engine_math_math::Mat4_transform_point, 0> inner0{};
      auto& place0 = busy_Mat4_transform_point ? inner0 : place_Mat4_transform_point_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Mat4_transform_point, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Mat4_transform_point, 0>{});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      const bool was_busy = busy_Mat4_transform_point;
      busy_Mat4_transform_point = true;
      kern::logic::native_call(true);
      const auto out = self.Mat4_transform_point(place0, std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Mat4_transform_point = was_busy;
      return true;
    }
    if (function == "Mat4_transform_vector") {
      kern::logic::NativeParam<&engine_math_math::Mat4_transform_vector, 0> inner0{};
      auto& place0 = busy_Mat4_transform_vector ? inner0 : place_Mat4_transform_vector_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Mat4_transform_vector, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Mat4_transform_vector, 0>{});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      const bool was_busy = busy_Mat4_transform_vector;
      busy_Mat4_transform_vector = true;
      kern::logic::native_call(true);
      const auto out = self.Mat4_transform_vector(place0, std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Mat4_transform_vector = was_busy;
      return true;
    }
    if (function == "Mat4_transform") {
      kern::logic::NativeParam<&engine_math_math::Mat4_transform, 0> inner0{};
      auto& place0 = busy_Mat4_transform ? inner0 : place_Mat4_transform_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Mat4_transform, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Mat4_transform, 0>{});
      engine_math_math::Vec4 in1 = native_arg<engine_math_math::Vec4>(args, 1, engine_math_math::Vec4{0.0, 0.0, 0.0, 0.0});
      const bool was_busy = busy_Mat4_transform;
      busy_Mat4_transform = true;
      kern::logic::native_call(true);
      const auto out = self.Mat4_transform(place0, std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Mat4_transform = was_busy;
      return true;
    }
    if (function == "Mat4_translate") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.Mat4_translate(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Mat4_scale") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.Mat4_scale(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Mat4_trs") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      engine_math_math::Quat in1 = native_arg<engine_math_math::Quat>(args, 1, engine_math_math::Quat{0.0, 0.0, 0.0, 1.0});
      kern::Vec3 in2 = native_arg<kern::Vec3>(args, 2, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.Mat4_trs(std::move(in0), std::move(in1), std::move(in2));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Mat4_perspective") {
      kern::f64 in0 = native_arg<kern::f64>(args, 0, 0.0);
      kern::f64 in1 = native_arg<kern::f64>(args, 1, 0.0);
      kern::f64 in2 = native_arg<kern::f64>(args, 2, 0.0);
      kern::f64 in3 = native_arg<kern::f64>(args, 3, 0.0);
      kern::logic::native_call(true);
      const auto out = self.Mat4_perspective(std::move(in0), std::move(in1), std::move(in2), std::move(in3));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Mat4_ortho") {
      kern::f64 in0 = native_arg<kern::f64>(args, 0, 0.0);
      kern::f64 in1 = native_arg<kern::f64>(args, 1, 0.0);
      kern::f64 in2 = native_arg<kern::f64>(args, 2, 0.0);
      kern::f64 in3 = native_arg<kern::f64>(args, 3, 0.0);
      kern::f64 in4 = native_arg<kern::f64>(args, 4, 0.0);
      kern::f64 in5 = native_arg<kern::f64>(args, 5, 0.0);
      kern::logic::native_call(true);
      const auto out = self.Mat4_ortho(std::move(in0), std::move(in1), std::move(in2), std::move(in3), std::move(in4), std::move(in5));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Mat4_look_at") {
      kern::logic::NativeParam<&engine_math_math::Mat4_look_at, 0> inner0{};
      auto& place0 = busy_Mat4_look_at ? inner0 : place_Mat4_look_at_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Mat4_look_at, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Mat4_look_at, 0>{});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in2 = native_arg<kern::Vec3>(args, 2, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in3 = native_arg<kern::Vec3>(args, 3, kern::Vec3{0.0, 0.0, 0.0});
      const bool was_busy = busy_Mat4_look_at;
      busy_Mat4_look_at = true;
      kern::logic::native_call(true);
      const auto out = self.Mat4_look_at(place0, std::move(in1), std::move(in2), std::move(in3));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      results.push_back(to_value(place0));
      busy_Mat4_look_at = was_busy;
      return true;
    }
    if (function == "Mat4_look_at_2") {
      kern::Vec3 in0 = native_arg<kern::Vec3>(args, 0, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in2 = native_arg<kern::Vec3>(args, 2, kern::Vec3{0.0, 0.0, 0.0});
      kern::logic::native_call(true);
      const auto out = self.Mat4_look_at_2(std::move(in0), std::move(in1), std::move(in2));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "Sphere_valid") {
      kern::logic::NativeParam<&engine_math_math::Sphere_valid, 0> inner0{};
      auto& place0 = busy_Sphere_valid ? inner0 : place_Sphere_valid_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Sphere_valid, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Sphere_valid, 0>{});
      const bool was_busy = busy_Sphere_valid;
      busy_Sphere_valid = true;
      kern::logic::native_call(true);
      const auto out = self.Sphere_valid(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Sphere_valid = was_busy;
      return true;
    }
    if (function == "Aabb_valid") {
      kern::logic::NativeParam<&engine_math_math::Aabb_valid, 0> inner0{};
      auto& place0 = busy_Aabb_valid ? inner0 : place_Aabb_valid_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Aabb_valid, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Aabb_valid, 0>{});
      const bool was_busy = busy_Aabb_valid;
      busy_Aabb_valid = true;
      kern::logic::native_call(true);
      const auto out = self.Aabb_valid(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Aabb_valid = was_busy;
      return true;
    }
    if (function == "Aabb_center") {
      kern::logic::NativeParam<&engine_math_math::Aabb_center, 0> inner0{};
      auto& place0 = busy_Aabb_center ? inner0 : place_Aabb_center_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Aabb_center, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Aabb_center, 0>{});
      const bool was_busy = busy_Aabb_center;
      busy_Aabb_center = true;
      kern::logic::native_call(true);
      const auto out = self.Aabb_center(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Aabb_center = was_busy;
      return true;
    }
    if (function == "Aabb_extent") {
      kern::logic::NativeParam<&engine_math_math::Aabb_extent, 0> inner0{};
      auto& place0 = busy_Aabb_extent ? inner0 : place_Aabb_extent_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::Aabb_extent, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::Aabb_extent, 0>{});
      const bool was_busy = busy_Aabb_extent;
      busy_Aabb_extent = true;
      kern::logic::native_call(true);
      const auto out = self.Aabb_extent(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Aabb_extent = was_busy;
      return true;
    }
    if (function == "camera_valid") {
      kern::logic::NativeParam<&engine_math_math::camera_valid, 0> inner0{};
      auto& place0 = busy_camera_valid ? inner0 : place_camera_valid_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::camera_valid, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::camera_valid, 0>{});
      const bool was_busy = busy_camera_valid;
      busy_camera_valid = true;
      kern::logic::native_call(true);
      const auto out = self.camera_valid(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_camera_valid = was_busy;
      return true;
    }
    if (function == "projection_matrix") {
      kern::logic::NativeParam<&engine_math_math::projection_matrix, 0> inner0{};
      auto& place0 = busy_projection_matrix ? inner0 : place_projection_matrix_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_math_math::projection_matrix, 0>>(args, 0, kern::logic::NativeParam<&engine_math_math::projection_matrix, 0>{});
      kern::f64 in1 = native_arg<kern::f64>(args, 1, 0.0);
      const bool was_busy = busy_projection_matrix;
      busy_projection_matrix = true;
      kern::logic::native_call(true);
      const auto out = self.projection_matrix(place0, std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_projection_matrix = was_busy;
      return true;
    }
    return false;
  }
  kern::logic::NativeParam<&engine_math_math::Vec3_load, 0> place_Vec3_load_0{};
  bool busy_Vec3_load = false;
  kern::logic::NativeParam<&engine_math_math::Vec4_xyz, 0> place_Vec4_xyz_0{};
  bool busy_Vec4_xyz = false;
  kern::logic::NativeParam<&engine_math_math::Vec4_equal, 0> place_Vec4_equal_0{};
  kern::logic::NativeParam<&engine_math_math::Vec4_equal, 1> place_Vec4_equal_1{};
  bool busy_Vec4_equal = false;
  kern::logic::NativeParam<&engine_math_math::Quat_mul, 0> place_Quat_mul_0{};
  kern::logic::NativeParam<&engine_math_math::Quat_mul, 1> place_Quat_mul_1{};
  bool busy_Quat_mul = false;
  kern::logic::NativeParam<&engine_math_math::Quat_rotate, 0> place_Quat_rotate_0{};
  bool busy_Quat_rotate = false;
  kern::logic::NativeParam<&engine_math_math::Mat4_load, 0> place_Mat4_load_0{};
  bool busy_Mat4_load = false;
  kern::logic::NativeParam<&engine_math_math::Mat4_at_2, 0> place_Mat4_at_2_0{};
  bool busy_Mat4_at_2 = false;
  kern::logic::NativeParam<&engine_math_math::Mat4_translation, 0> place_Mat4_translation_0{};
  bool busy_Mat4_translation = false;
  kern::logic::NativeParam<&engine_math_math::Mat4_mul, 0> place_Mat4_mul_0{};
  kern::logic::NativeParam<&engine_math_math::Mat4_mul, 1> place_Mat4_mul_1{};
  bool busy_Mat4_mul = false;
  kern::logic::NativeParam<&engine_math_math::Mat4_transform_point, 0> place_Mat4_transform_point_0{};
  bool busy_Mat4_transform_point = false;
  kern::logic::NativeParam<&engine_math_math::Mat4_transform_vector, 0> place_Mat4_transform_vector_0{};
  bool busy_Mat4_transform_vector = false;
  kern::logic::NativeParam<&engine_math_math::Mat4_transform, 0> place_Mat4_transform_0{};
  bool busy_Mat4_transform = false;
  kern::logic::NativeParam<&engine_math_math::Mat4_look_at, 0> place_Mat4_look_at_0{};
  bool busy_Mat4_look_at = false;
  kern::logic::NativeParam<&engine_math_math::Sphere_valid, 0> place_Sphere_valid_0{};
  bool busy_Sphere_valid = false;
  kern::logic::NativeParam<&engine_math_math::Aabb_valid, 0> place_Aabb_valid_0{};
  bool busy_Aabb_valid = false;
  kern::logic::NativeParam<&engine_math_math::Aabb_center, 0> place_Aabb_center_0{};
  bool busy_Aabb_center = false;
  kern::logic::NativeParam<&engine_math_math::Aabb_extent, 0> place_Aabb_extent_0{};
  bool busy_Aabb_extent = false;
  kern::logic::NativeParam<&engine_math_math::camera_valid, 0> place_camera_valid_0{};
  bool busy_camera_valid = false;
  kern::logic::NativeParam<&engine_math_math::projection_matrix, 0> place_projection_matrix_0{};
  bool busy_projection_matrix = false;
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    if (variable == "kPi") { out = kern::logic::to_value(self.kPi); return true; }
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_math_math_registered =
    (kern::logic::register_native_blueprint(0x356aad6eac7ac306ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_math_math_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_64c1acf056c7521a

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_gfx_passes" (its class: in the header)
namespace kern_blueprint_33fb79d97c280ace {

// How the engine reaches the Blueprint engine_gfx_passes by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_33fb79d97c280ace
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::BloomOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<bool>(*v.field(0)) && is_value<bool>(*v.field(1)) && is_value<bool>(*v.field(2)); }
  static ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::BloomOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<bool>(*v.field(0)), from_value<bool>(*v.field(1)), from_value<bool>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::BloomOptions& s) { return Value::structure({to_value(s.enabled), to_value(s.forced), to_value(s.grade)}); }
};
template <> struct Held<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::ShadowOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)); }
  static ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::ShadowOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::i64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::ShadowOptions& s) { return Value::structure({to_value(s.resolution), to_value(s.extent), to_value(s.bias), to_value(s.filter)}); }
};
template <> struct Held<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::SkyAtmosphere> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)); }
  static ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::SkyAtmosphere from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4))};
  }
  static Value to(const ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::SkyAtmosphere& s) { return Value::structure({to_value(s.fog_color), to_value(s.sun_color), to_value(s.sun_direction), to_value(s.sun_scatter), to_value(s.horizon)}); }
};
template <> struct Held<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::SkyState> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::Vec3>(*v.field(6)); }
  static ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::SkyState from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::Vec3>(*v.field(6))};
  }
  static Value to(const ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::SkyState& s) { return Value::structure({to_value(s.sun), to_value(s.moon), to_value(s.night), to_value(s.clouds), to_value(s.blast_direction), to_value(s.blast_age), to_value(s.wind)}); }
};
template <> struct Held<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::AoOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<bool>(*v.field(0)) && is_value<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::AoQuality>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<bool>(*v.field(5)); }
  static ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::AoOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<bool>(*v.field(0)), from_value<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::AoQuality>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<bool>(*v.field(5))};
  }
  static Value to(const ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::AoOptions& s) { return Value::structure({to_value(s.enabled), to_value(s.quality), to_value(s.half_resolution), to_value(s.radius), to_value(s.power), to_value(s.adaptive)}); }
};
template <> struct Held<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::BloomPass_Occlusion> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::Mat4>(*v.field(3)); }
  static ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::BloomPass_Occlusion from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::Mat4>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_33fb79d97c280ace::engine_gfx_passes::BloomPass_Occlusion& s) { return Value::structure({to_value(s.texture), to_value(s.scale_x), to_value(s.scale_y), to_value(s.projection)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_33fb79d97c280ace {
namespace {
struct engine_gfx_passes_Native final : kern::logic::NativeBlueprint {
  engine_gfx_passes self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_gfx_passes> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_gfx_passes_registered =
    (kern::logic::register_native_blueprint(0x9e3aa6bfe0cb7510ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_gfx_passes_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_33fb79d97c280ace

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_gfx_renderer" (its class: in the header)
namespace kern_blueprint_6093af1c7e680998 {

// How the engine reaches the Blueprint engine_gfx_renderer by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_6093af1c7e680998
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Lighting> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 14 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::Vec3>(*v.field(8)) && is_value<kern::Vec3>(*v.field(9)) && is_value<kern::Vec3>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<kern::f64>(*v.field(13)); }
  static ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Lighting from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::Vec3>(*v.field(8)), from_value<kern::Vec3>(*v.field(9)), from_value<kern::Vec3>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<kern::f64>(*v.field(13))};
  }
  static Value to(const ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Lighting& s) { return Value::structure({to_value(s.sun_direction), to_value(s.sun_color), to_value(s.sky_color), to_value(s.ground_color), to_value(s.fog_color), to_value(s.fog_density), to_value(s.exposure), to_value(s.env_intensity), to_value(s.env_zenith), to_value(s.env_horizon), to_value(s.env_ground), to_value(s.fog_height_falloff), to_value(s.fog_sun_scatter), to_value(s.fog_horizon)}); }
};
template <> struct Held<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::PointLights> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::i64>(*v.field(0)) && is_value<kern::List<kern::f64>>(*v.field(1)) && is_value<kern::List<kern::f64>>(*v.field(2)); }
  static ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::PointLights from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::List<kern::f64>>(*v.field(1)), from_value<kern::List<kern::f64>>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::PointLights& s) { return Value::structure({to_value(s.count), to_value(s.pos_range), to_value(s.color_intensity)}); }
};
template <> struct Held<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Viewport> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)); }
  static ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Viewport from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Viewport& s) { return Value::structure({to_value(s.x), to_value(s.y), to_value(s.width), to_value(s.height)}); }
};
template <> struct Held<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::RenderParams> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 8 && is_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Vec4>(*v.field(0)) && is_value<bool>(*v.field(1)) && is_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Viewport>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<kern::i64>(*v.field(7)); }
  static ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::RenderParams from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Vec4>(*v.field(0)), from_value<bool>(*v.field(1)), from_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::Viewport>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<kern::i64>(*v.field(7))};
  }
  static Value to(const ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::RenderParams& s) { return Value::structure({to_value(s.clear_color), to_value(s.has_viewport), to_value(s.viewport), to_value(s.page_hidden), to_value(s.layers), to_value(s.sky), to_value(s.reuse_shadow), to_value(s.framebuffer)}); }
};
template <> struct Held<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::RenderStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 12 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<kern::i64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::BloomState>(*v.field(11)); }
  static ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::RenderStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<kern::i64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::BloomState>(*v.field(11))};
  }
  static Value to(const ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::RenderStats& s) { return Value::structure({to_value(s.draw_calls), to_value(s.shadow_draw_calls), to_value(s.shadow_resolution), to_value(s.ao), to_value(s.shadow_reused), to_value(s.triangles), to_value(s.instances), to_value(s.culled_meshes), to_value(s.visible_meshes), to_value(s.cpu_ms), to_value(s.gpu_ms), to_value(s.bloom)}); }
};
template <> struct Held<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::RendererOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::ShadowOptions>(*v.field(3)) && is_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::BloomOptions>(*v.field(4)) && is_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::AoOptions>(*v.field(5)) && is_value<bool>(*v.field(6)); }
  static ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::RendererOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::ShadowOptions>(*v.field(3)), from_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::BloomOptions>(*v.field(4)), from_value<::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::AoOptions>(*v.field(5)), from_value<bool>(*v.field(6))};
  }
  static Value to(const ::kern_blueprint_6093af1c7e680998::engine_gfx_renderer::RendererOptions& s) { return Value::structure({to_value(s.width), to_value(s.height), to_value(s.shadows), to_value(s.shadow), to_value(s.bloom), to_value(s.ao), to_value(s.grade)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_6093af1c7e680998 {
namespace {
struct engine_gfx_renderer_Native final : kern::logic::NativeBlueprint {
  engine_gfx_renderer self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_gfx_renderer> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_gfx_renderer_registered =
    (kern::logic::register_native_blueprint(0xbdf1b1ad4c010064ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_gfx_renderer_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_6093af1c7e680998

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_app_app" (its class: in the header)
namespace kern_blueprint_fbd4d5d49d8b5cdd {

// How the engine reaches the Blueprint engine_app_app by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_fbd4d5d49d8b5cdd
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_fbd4d5d49d8b5cdd::engine_app_app::Frame> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::logic::Int64>(*v.field(2)); }
  static ::kern_blueprint_fbd4d5d49d8b5cdd::engine_app_app::Frame from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::logic::Int64>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_fbd4d5d49d8b5cdd::engine_app_app::Frame& s) { return Value::structure({to_value(s.time), to_value(s.dt), to_value(s.index)}); }
};
template <> struct Held<::kern_blueprint_fbd4d5d49d8b5cdd::engine_app_app::Touch> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)); }
  static ::kern_blueprint_fbd4d5d49d8b5cdd::engine_app_app::Touch from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_fbd4d5d49d8b5cdd::engine_app_app::Touch& s) { return Value::structure({to_value(s.id), to_value(s.start), to_value(s.position), to_value(s.previous)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_fbd4d5d49d8b5cdd {
namespace {
struct engine_app_app_Native final : kern::logic::NativeBlueprint {
  engine_app_app self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_app_app> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "App_configure") {
      kern::logic::native_call(true);
      const auto out = self.App_configure();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "App_extensions") {
      kern::logic::native_call(true);
      const auto out = self.App_extensions();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "App_resized") {
      kern::i64 in0 = native_arg<kern::i64>(args, 0, 0);
      kern::logic::native_call(true);
      self.App_resized(std::move(in0));
      kern::logic::native_call(false);
      return true;
    }
    if (function == "App_stop") {
      kern::i64 in0 = native_arg<kern::i64>(args, 0, 0);
      kern::logic::native_call(true);
      self.App_stop(std::move(in0));
      kern::logic::native_call(false);
      return true;
    }
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_app_app_registered =
    (kern::logic::register_native_blueprint(0x3d464206fc0704c3ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_app_app_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_fbd4d5d49d8b5cdd

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_audio_audio" (its class: in the header)
namespace kern_blueprint_0581e7282166936f {

// How the engine reaches the Blueprint engine_audio_audio by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_0581e7282166936f
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_BusSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<bool>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_BusSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<bool>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_BusSettings& s) { return Value::structure({to_value(s.volume), to_value(s.muted), to_value(s.lowpass_hz), to_value(s.reverb_send)}); }
};
template <> struct Held<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_ClipInfo> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<bool>(*v.field(5)); }
  static ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_ClipInfo from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<bool>(*v.field(5))};
  }
  static Value to(const ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_ClipInfo& s) { return Value::structure({to_value(s.name), to_value(s.frames), to_value(s.channels), to_value(s.duration), to_value(s.peak), to_value(s.loopable)}); }
};
template <> struct Held<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Listener> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)); }
  static ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Listener from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Listener& s) { return Value::structure({to_value(s.position), to_value(s.forward), to_value(s.up), to_value(s.velocity)}); }
};
template <> struct Held<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Meter> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::List<kern::f64>>(*v.field(0)) && is_value<kern::List<kern::f64>>(*v.field(1)); }
  static ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Meter from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0)), from_value<kern::List<kern::f64>>(*v.field(1))};
  }
  static Value to(const ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Meter& s) { return Value::structure({to_value(s.peak), to_value(s.rms)}); }
};
template <> struct Held<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_PlayParams> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 16 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Bus>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<kern::Vec3>(*v.field(9)) && is_value<kern::Vec3>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Rolloff>(*v.field(13)) && is_value<kern::f64>(*v.field(14)) && is_value<kern::f64>(*v.field(15)); }
  static ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_PlayParams from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Bus>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<kern::Vec3>(*v.field(9)), from_value<kern::Vec3>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Rolloff>(*v.field(13)), from_value<kern::f64>(*v.field(14)), from_value<kern::f64>(*v.field(15))};
  }
  static Value to(const ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_PlayParams& s) { return Value::structure({to_value(s.volume), to_value(s.pitch), to_value(s.pan), to_value(s.the_loop), to_value(s.bus), to_value(s.fade_in), to_value(s.start), to_value(s.priority), to_value(s.spatial), to_value(s.position), to_value(s.velocity), to_value(s.min_distance), to_value(s.max_distance), to_value(s.rolloff), to_value(s.spread), to_value(s.doppler)}); }
};
template <> struct Held<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Stats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 10 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::logic::Int64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Meter>(*v.field(7)) && is_value<kern::List<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Meter>>(*v.field(8)) && is_value<kern::List<kern::f64>>(*v.field(9)); }
  static ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Stats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::logic::Int64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Meter>(*v.field(7)), from_value<kern::List<::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Meter>>(*v.field(8)), from_value<kern::List<kern::f64>>(*v.field(9))};
  }
  static Value to(const ::kern_blueprint_0581e7282166936f::engine_audio_audio::audio_Stats& s) { return Value::structure({to_value(s.voices), to_value(s.audible), to_value(s.virtual_voices), to_value(s.stolen), to_value(s.clips), to_value(s.frames_rendered), to_value(s.limiter_reduction_db), to_value(s.master), to_value(s.buses), to_value(s.peak_hold)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_0581e7282166936f {
namespace {
struct engine_audio_audio_Native final : kern::logic::NativeBlueprint {
  engine_audio_audio self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_audio_audio> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "db_to_gain") {
      kern::f64 in0 = native_arg<kern::f64>(args, 0, 0.0);
      kern::logic::native_call(true);
      const auto out = self.db_to_gain(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "gain_to_db") {
      kern::f64 in0 = native_arg<kern::f64>(args, 0, 0.0);
      kern::logic::native_call(true);
      const auto out = self.gain_to_db(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_audio_audio_registered =
    (kern::logic::register_native_blueprint(0x7606f1a8a169677dull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_audio_audio_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_0581e7282166936f

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_audio_synth" (its class: in the header)
namespace kern_blueprint_3f8740a3424253d4 {

// How the engine reaches the Blueprint engine_audio_synth by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_3f8740a3424253d4
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_3f8740a3424253d4::engine_audio_synth::audio_BuiltinSound> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::logic::Text>(*v.field(2)) && is_value<bool>(*v.field(3)); }
  static ::kern_blueprint_3f8740a3424253d4::engine_audio_synth::audio_BuiltinSound from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::logic::Text>(*v.field(2)), from_value<bool>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_3f8740a3424253d4::engine_audio_synth::audio_BuiltinSound& s) { return Value::structure({to_value(s.name), to_value(s.label), to_value(s.category), to_value(s.the_loop)}); }
};
template <> struct Held<::kern_blueprint_3f8740a3424253d4::engine_audio_synth::audio_SynthPatch> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 27 && is_value<::kern_blueprint_3f8740a3424253d4::engine_audio_synth::audio_Wave>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<kern::f64>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<kern::f64>(*v.field(16)) && is_value<kern::f64>(*v.field(17)) && is_value<kern::f64>(*v.field(18)) && is_value<kern::f64>(*v.field(19)) && is_value<kern::f64>(*v.field(20)) && is_value<kern::f64>(*v.field(21)) && is_value<kern::f64>(*v.field(22)) && is_value<kern::f64>(*v.field(23)) && is_value<kern::f64>(*v.field(24)) && is_value<kern::f64>(*v.field(25)) && is_value<kern::i64>(*v.field(26)); }
  static ::kern_blueprint_3f8740a3424253d4::engine_audio_synth::audio_SynthPatch from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_3f8740a3424253d4::engine_audio_synth::audio_Wave>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<kern::f64>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<kern::f64>(*v.field(16)), from_value<kern::f64>(*v.field(17)), from_value<kern::f64>(*v.field(18)), from_value<kern::f64>(*v.field(19)), from_value<kern::f64>(*v.field(20)), from_value<kern::f64>(*v.field(21)), from_value<kern::f64>(*v.field(22)), from_value<kern::f64>(*v.field(23)), from_value<kern::f64>(*v.field(24)), from_value<kern::f64>(*v.field(25)), from_value<kern::i64>(*v.field(26))};
  }
  static Value to(const ::kern_blueprint_3f8740a3424253d4::engine_audio_synth::audio_SynthPatch& s) { return Value::structure({to_value(s.wave), to_value(s.volume), to_value(s.attack), to_value(s.sustain), to_value(s.punch), to_value(s.release), to_value(s.frequency), to_value(s.slide), to_value(s.slide_accel), to_value(s.min_frequency), to_value(s.vibrato_depth), to_value(s.vibrato_rate), to_value(s.arp_ratio), to_value(s.arp_time), to_value(s.duty), to_value(s.duty_sweep), to_value(s.fm_ratio), to_value(s.fm_depth), to_value(s.lowpass), to_value(s.lowpass_sweep), to_value(s.resonance), to_value(s.highpass), to_value(s.tremolo_depth), to_value(s.tremolo_rate), to_value(s.repeat), to_value(s.bitcrush), to_value(s.seed)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_3f8740a3424253d4 {
namespace {
struct engine_audio_synth_Native final : kern::logic::NativeBlueprint {
  engine_audio_synth self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_audio_synth> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_audio_synth_registered =
    (kern::logic::register_native_blueprint(0xad9c56332c6e8e0aull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_audio_synth_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_3f8740a3424253d4

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_game_level" (its class: in the header)
namespace kern_blueprint_402ced772ba1edd9 {

// How the engine reaches the Blueprint engine_game_level by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_402ced772ba1edd9
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_402ced772ba1edd9::engine_game_level::game_Transform> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)); }
  static ::kern_blueprint_402ced772ba1edd9::engine_game_level::game_Transform from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_402ced772ba1edd9::engine_game_level::game_Transform& s) { return Value::structure({to_value(s.position), to_value(s.rotation), to_value(s.scale)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_402ced772ba1edd9 {
namespace {
struct engine_game_level_Native final : kern::logic::NativeBlueprint {
  engine_game_level self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_game_level> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_game_level_registered =
    (kern::logic::register_native_blueprint(0x5a27993cee70894bull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_game_level_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_402ced772ba1edd9

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_game_runtime" (its class: in the header)
namespace kern_blueprint_fdc39c3af1f3c0bb {

// How the engine reaches the Blueprint engine_game_runtime by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_fdc39c3af1f3c0bb
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_BlueprintRunning> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<bool>(*v.field(4)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_BlueprintRunning from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<bool>(*v.field(4))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_BlueprintRunning& s) { return Value::structure({to_value(s.name), to_value(s.tier), to_value(s.functions), to_value(s.nodes), to_value(s.disabled)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_ArtStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<bool>(*v.field(4)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_ArtStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<bool>(*v.field(4))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_ArtStats& s) { return Value::structure({to_value(s.rigs), to_value(s.decals), to_value(s.tracers), to_value(s.chips), to_value(s.view_model)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_DetectorModel> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::logic::Int64>(*v.field(3)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_DetectorModel from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::logic::Int64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_DetectorModel& s) { return Value::structure({to_value(s.sigma_abs), to_value(s.sigma_rel), to_value(s.dropout), to_value(s.seed)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_SpawnChange> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_SpawnChange from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_SpawnChange& s) { return Value::structure({to_value(s.source), to_value(s.id), to_value(s.position), to_value(s.rotation)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_WalkerJoint> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_WalkerJoint from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_WalkerJoint& s) { return Value::structure({to_value(s.name), to_value(s.angle), to_value(s.lower), to_value(s.upper)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_WalkerSensors> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 13 && is_value<kern::List<kern::f64>>(*v.field(0)) && is_value<kern::List<kern::logic::Sized<kern::u8>>>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::Vec3>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::i64>(*v.field(10)) && is_value<kern::List<kern::Vec3>>(*v.field(11)) && is_value<kern::i64>(*v.field(12)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_WalkerSensors from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0)), from_value<kern::List<kern::logic::Sized<kern::u8>>>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::Vec3>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::i64>(*v.field(10)), from_value<kern::List<kern::Vec3>>(*v.field(11)), from_value<kern::i64>(*v.field(12))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_WalkerSensors& s) { return Value::structure({to_value(s.range), to_value(s.zone), to_value(s.nearest), to_value(s.nearest_bearing), to_value(s.free_left), to_value(s.free_right), to_value(s.gravity), to_value(s.fallen), to_value(s.speed), to_value(s.tof_time), to_value(s.hits), to_value(s.hit), to_value(s.forward_observed)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_PlayInput> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 25 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<bool>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<bool>(*v.field(16)) && is_value<bool>(*v.field(17)) && is_value<bool>(*v.field(18)) && is_value<bool>(*v.field(19)) && is_value<bool>(*v.field(20)) && is_value<kern::f64>(*v.field(21)) && is_value<kern::f64>(*v.field(22)) && is_value<bool>(*v.field(23)) && is_value<kern::f64>(*v.field(24)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_PlayInput from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<bool>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<bool>(*v.field(16)), from_value<bool>(*v.field(17)), from_value<bool>(*v.field(18)), from_value<bool>(*v.field(19)), from_value<bool>(*v.field(20)), from_value<kern::f64>(*v.field(21)), from_value<kern::f64>(*v.field(22)), from_value<bool>(*v.field(23)), from_value<kern::f64>(*v.field(24))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_PlayInput& s) { return Value::structure({to_value(s.move_x), to_value(s.move_y), to_value(s.up), to_value(s.jump), to_value(s.boost), to_value(s.camera), to_value(s.handbrake), to_value(s.robot), to_value(s.look_x), to_value(s.look_y), to_value(s.turn_deg), to_value(s.speed_scale), to_value(s.zoom), to_value(s.view_yaw), to_value(s.has_view_yaw), to_value(s.view_pitch), to_value(s.fire), to_value(s.aim), to_value(s.reload), to_value(s.crouch), to_value(s.interact), to_value(s.look_stick_x), to_value(s.look_stick_y), to_value(s.gamepad), to_value(s.rewind)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_ShotEvent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 8 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<kern::Vec3>(*v.field(6)) && is_value<kern::i64>(*v.field(7)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_ShotEvent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<kern::Vec3>(*v.field(6)), from_value<kern::i64>(*v.field(7))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_ShotEvent& s) { return Value::structure({to_value(s.from), to_value(s.to), to_value(s.shooter), to_value(s.hit), to_value(s.impact), to_value(s.head), to_value(s.normal), to_value(s.surface)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_SimStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::logic::Int64>(*v.field(6)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_SimStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::logic::Int64>(*v.field(6))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_SimStats& s) { return Value::structure({to_value(s.bodies), to_value(s.awake), to_value(s.shapes), to_value(s.joints), to_value(s.broken_joints), to_value(s.contacts), to_value(s.steps)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_SoldierState> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 15 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::i64>(*v.field(9)) && is_value<kern::i64>(*v.field(10)) && is_value<bool>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<bool>(*v.field(13)) && is_value<kern::f64>(*v.field(14)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_SoldierState from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::i64>(*v.field(9)), from_value<kern::i64>(*v.field(10)), from_value<bool>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<bool>(*v.field(13)), from_value<kern::f64>(*v.field(14))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_SoldierState& s) { return Value::structure({to_value(s.health), to_value(s.max_health), to_value(s.armor), to_value(s.ammo), to_value(s.magazine), to_value(s.reserve), to_value(s.reload), to_value(s.spread), to_value(s.respawn_in), to_value(s.hits), to_value(s.damages), to_value(s.hit_head), to_value(s.hit_kill), to_value(s.dead), to_value(s.damage_yaw)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_WalkerProbe> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 12 && is_value<kern::Vec3>(*v.field(0)) && is_value<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::Quat>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::List<bool>>(*v.field(5)) && is_value<kern::List<kern::f64>>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<kern::List<kern::f64>>(*v.field(9)) && is_value<kern::List<kern::f64>>(*v.field(10)) && is_value<kern::List<kern::f64>>(*v.field(11)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_WalkerProbe from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::Quat>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::List<bool>>(*v.field(5)), from_value<kern::List<kern::f64>>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<kern::List<kern::f64>>(*v.field(9)), from_value<kern::List<kern::f64>>(*v.field(10)), from_value<kern::List<kern::f64>>(*v.field(11))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameWorld_WalkerProbe& s) { return Value::structure({to_value(s.position), to_value(s.rotation), to_value(s.velocity), to_value(s.spin), to_value(s.gravity), to_value(s.foot_contact), to_value(s.foot_height), to_value(s.self_contact), to_value(s.body_contact), to_value(s.joint), to_value(s.joint_speed), to_value(s.action)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_Hud> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 31 && is_value<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameStatus>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::logic::Text>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<bool>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<bool>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<kern::f64>(*v.field(16)) && is_value<kern::f64>(*v.field(17)) && is_value<kern::i64>(*v.field(18)) && is_value<kern::i64>(*v.field(19)) && is_value<kern::i64>(*v.field(20)) && is_value<kern::f64>(*v.field(21)) && is_value<kern::f64>(*v.field(22)) && is_value<kern::f64>(*v.field(23)) && is_value<bool>(*v.field(24)) && is_value<bool>(*v.field(25)) && is_value<kern::f64>(*v.field(26)) && is_value<kern::f64>(*v.field(27)) && is_value<kern::f64>(*v.field(28)) && is_value<bool>(*v.field(29)) && is_value<kern::f64>(*v.field(30)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_Hud from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_GameStatus>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::logic::Text>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<bool>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<bool>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<kern::f64>(*v.field(16)), from_value<kern::f64>(*v.field(17)), from_value<kern::i64>(*v.field(18)), from_value<kern::i64>(*v.field(19)), from_value<kern::i64>(*v.field(20)), from_value<kern::f64>(*v.field(21)), from_value<kern::f64>(*v.field(22)), from_value<kern::f64>(*v.field(23)), from_value<bool>(*v.field(24)), from_value<bool>(*v.field(25)), from_value<kern::f64>(*v.field(26)), from_value<kern::f64>(*v.field(27)), from_value<kern::f64>(*v.field(28)), from_value<bool>(*v.field(29)), from_value<kern::f64>(*v.field(30))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_Hud& s) { return Value::structure({to_value(s.status), to_value(s.title), to_value(s.message), to_value(s.message_age), to_value(s.score), to_value(s.pickups), to_value(s.pickups_total), to_value(s.elapsed), to_value(s.remaining), to_value(s.speed), to_value(s.boost), to_value(s.boosting), to_value(s.interior), to_value(s.best), to_value(s.soldier), to_value(s.health), to_value(s.max_health), to_value(s.armor), to_value(s.ammo), to_value(s.magazine), to_value(s.reserve), to_value(s.reload), to_value(s.spread), to_value(s.hit_age), to_value(s.hit_head), to_value(s.hit_kill), to_value(s.damage_age), to_value(s.damage_yaw), to_value(s.respawn_in), to_value(s.aiming), to_value(s.fov)}); }
};
template <> struct Held<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_SoundCue> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 9 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::audio_Bus>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<bool>(*v.field(8)); }
  static ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_SoundCue from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::audio_Bus>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<bool>(*v.field(8))};
  }
  static Value to(const ::kern_blueprint_fdc39c3af1f3c0bb::engine_game_runtime::game_SoundCue& s) { return Value::structure({to_value(s.sound), to_value(s.position), to_value(s.volume), to_value(s.pitch), to_value(s.spatial), to_value(s.bus), to_value(s.actor), to_value(s.voice), to_value(s.silence)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_fdc39c3af1f3c0bb {
namespace {
struct engine_game_runtime_Native final : kern::logic::NativeBlueprint {
  engine_game_runtime self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_game_runtime> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "NightWindows_name") {
      kern::logic::native_call(true);
      const auto out = self.NightWindows_name();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_game_runtime_registered =
    (kern::logic::register_native_blueprint(0x3d3784e93127e72full, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_game_runtime_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_fdc39c3af1f3c0bb

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_gfx_mesh" (its class: in the header)
namespace kern_blueprint_bb872f843a762d45 {

// How the engine reaches the Blueprint engine_gfx_mesh by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_bb872f843a762d45
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Float_row> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::List<kern::f64>>(*v.field(0)); }
  static ::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Float_row from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0))};
  }
  static Value to(const ::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Float_row& s) { return Value::structure({to_value(s.items)}); }
};
template <> struct Held<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Material> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 14 && is_value<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Vec4>(*v.field(0)) && is_value<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::AlphaMode>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::Vec3>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<bool>(*v.field(10)) && is_value<bool>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<kern::List<kern::logic::Sized<kern::i32>>>(*v.field(13)); }
  static ::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Material from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Vec4>(*v.field(0)), from_value<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::AlphaMode>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::Vec3>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<bool>(*v.field(10)), from_value<bool>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<kern::List<kern::logic::Sized<kern::i32>>>(*v.field(13))};
  }
  static Value to(const ::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Material& s) { return Value::structure({to_value(s.color), to_value(s.alpha), to_value(s.alpha_cutoff), to_value(s.roughness), to_value(s.metalness), to_value(s.emissive), to_value(s.unlit), to_value(s.double_sided), to_value(s.cast_shadow), to_value(s.frustum_culled), to_value(s.viewmodel), to_value(s.linear_colors), to_value(s.lines), to_value(s.params)}); }
};
template <> struct Held<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::MeshInput> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 16 && is_value<kern::List<kern::f64>>(*v.field(0)) && is_value<kern::List<kern::f64>>(*v.field(1)) && is_value<kern::List<kern::f64>>(*v.field(2)) && is_value<kern::List<kern::f64>>(*v.field(3)) && is_value<kern::List<kern::logic::Sized<kern::u32>>>(*v.field(4)) && is_value<kern::List<kern::f64>>(*v.field(5)) && is_value<kern::List<kern::f64>>(*v.field(6)) && is_value<kern::List<kern::f64>>(*v.field(7)) && is_value<kern::List<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Float_row>>(*v.field(8)) && is_value<kern::List<kern::f64>>(*v.field(9)) && is_value<kern::List<kern::f64>>(*v.field(10)) && is_value<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Material>(*v.field(11)) && is_value<kern::i64>(*v.field(12)) && is_value<bool>(*v.field(13)) && is_value<kern::i64>(*v.field(14)) && is_value<bool>(*v.field(15)); }
  static ::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::MeshInput from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0)), from_value<kern::List<kern::f64>>(*v.field(1)), from_value<kern::List<kern::f64>>(*v.field(2)), from_value<kern::List<kern::f64>>(*v.field(3)), from_value<kern::List<kern::logic::Sized<kern::u32>>>(*v.field(4)), from_value<kern::List<kern::f64>>(*v.field(5)), from_value<kern::List<kern::f64>>(*v.field(6)), from_value<kern::List<kern::f64>>(*v.field(7)), from_value<kern::List<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Float_row>>(*v.field(8)), from_value<kern::List<kern::f64>>(*v.field(9)), from_value<kern::List<kern::f64>>(*v.field(10)), from_value<::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::Material>(*v.field(11)), from_value<kern::i64>(*v.field(12)), from_value<bool>(*v.field(13)), from_value<kern::i64>(*v.field(14)), from_value<bool>(*v.field(15))};
  }
  static Value to(const ::kern_blueprint_bb872f843a762d45::engine_gfx_mesh::MeshInput& s) { return Value::structure({to_value(s.positions), to_value(s.normals), to_value(s.colors), to_value(s.uvs), to_value(s.indices), to_value(s.joints), to_value(s.weights), to_value(s.bones), to_value(s.extension), to_value(s.transforms), to_value(s.tints), to_value(s.material), to_value(s.texture), to_value(s.visible), to_value(s.layers), to_value(s.pick_bvh)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_bb872f843a762d45 {
namespace {
struct engine_gfx_mesh_Native final : kern::logic::NativeBlueprint {
  engine_gfx_mesh self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_gfx_mesh> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "Material_casts_shadow") {
      kern::logic::NativeParam<&engine_gfx_mesh::Material_casts_shadow, 0> inner0{};
      auto& place0 = busy_Material_casts_shadow ? inner0 : place_Material_casts_shadow_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_gfx_mesh::Material_casts_shadow, 0>>(args, 0, kern::logic::NativeParam<&engine_gfx_mesh::Material_casts_shadow, 0>{});
      const bool was_busy = busy_Material_casts_shadow;
      busy_Material_casts_shadow = true;
      kern::logic::native_call(true);
      const auto out = self.Material_casts_shadow(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Material_casts_shadow = was_busy;
      return true;
    }
    return false;
  }
  kern::logic::NativeParam<&engine_gfx_mesh::Material_casts_shadow, 0> place_Material_casts_shadow_0{};
  bool busy_Material_casts_shadow = false;
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_gfx_mesh_registered =
    (kern::logic::register_native_blueprint(0xbdc4e3fe8cad782dull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_gfx_mesh_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_bb872f843a762d45

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_geo_mesh_data" (its class: in the header)
namespace kern_blueprint_c8b7695a1b35c491 {

// How the engine reaches the Blueprint engine_geo_mesh_data by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_c8b7695a1b35c491
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::MeshData> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::List<kern::f64>>(*v.field(0)) && is_value<kern::List<kern::f64>>(*v.field(1)) && is_value<kern::List<kern::f64>>(*v.field(2)) && is_value<kern::List<kern::f64>>(*v.field(3)) && is_value<kern::List<kern::f64>>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::List<kern::logic::Sized<kern::u32>>>(*v.field(6)); }
  static ::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::MeshData from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0)), from_value<kern::List<kern::f64>>(*v.field(1)), from_value<kern::List<kern::f64>>(*v.field(2)), from_value<kern::List<kern::f64>>(*v.field(3)), from_value<kern::List<kern::f64>>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::List<kern::logic::Sized<kern::u32>>>(*v.field(6))};
  }
  static Value to(const ::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::MeshData& s) { return Value::structure({to_value(s.positions), to_value(s.normals), to_value(s.colors), to_value(s.uvs), to_value(s.extra), to_value(s.extra_size), to_value(s.indices)}); }
};
template <> struct Held<::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::WeldStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<bool>(*v.field(4)); }
  static ::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::WeldStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<bool>(*v.field(4))};
  }
  static Value to(const ::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::WeldStats& s) { return Value::structure({to_value(s.source_vertices), to_value(s.unique_vertices), to_value(s.bytes_before), to_value(s.bytes_after), to_value(s.indexed)}); }
};
template <> struct Held<::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::MeshLook> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::Material>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<bool>(*v.field(5)); }
  static ::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::MeshLook from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::Material>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<bool>(*v.field(5))};
  }
  static Value to(const ::kern_blueprint_c8b7695a1b35c491::engine_geo_mesh_data::MeshLook& s) { return Value::structure({to_value(s.material), to_value(s.extra_slot), to_value(s.texture), to_value(s.visible), to_value(s.layers), to_value(s.pick_bvh)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_c8b7695a1b35c491 {
namespace {
struct engine_geo_mesh_data_Native final : kern::logic::NativeBlueprint {
  engine_geo_mesh_data self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_geo_mesh_data> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "MeshData_vertex_count") {
      kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_vertex_count, 0> inner0{};
      auto& place0 = busy_MeshData_vertex_count ? inner0 : place_MeshData_vertex_count_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_vertex_count, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_vertex_count, 0>{});
      const bool was_busy = busy_MeshData_vertex_count;
      busy_MeshData_vertex_count = true;
      kern::logic::native_call(true);
      const auto out = self.MeshData_vertex_count(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_MeshData_vertex_count = was_busy;
      return true;
    }
    if (function == "MeshData_triangle_count") {
      kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_triangle_count, 0> inner0{};
      auto& place0 = busy_MeshData_triangle_count ? inner0 : place_MeshData_triangle_count_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_triangle_count, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_triangle_count, 0>{});
      const bool was_busy = busy_MeshData_triangle_count;
      busy_MeshData_triangle_count = true;
      kern::logic::native_call(true);
      const auto out = self.MeshData_triangle_count(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_MeshData_triangle_count = was_busy;
      return true;
    }
    if (function == "MeshData_clear") {
      kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_clear, 0> inner0{};
      auto& place0 = busy_MeshData_clear ? inner0 : place_MeshData_clear_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_clear, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_clear, 0>{});
      const bool was_busy = busy_MeshData_clear;
      busy_MeshData_clear = true;
      kern::logic::native_call(true);
      self.MeshData_clear(place0);
      kern::logic::native_call(false);
      results.push_back(to_value(place0));
      busy_MeshData_clear = was_busy;
      return true;
    }
    if (function == "MeshData_push") {
      kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push, 0> inner0{};
      auto& place0 = busy_MeshData_push ? inner0 : place_MeshData_push_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push, 0>{});
      kern::Vec3 in1 = native_arg<kern::Vec3>(args, 1, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in2 = native_arg<kern::Vec3>(args, 2, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in3 = native_arg<kern::Vec3>(args, 3, kern::Vec3{0.0, 0.0, 0.0});
      kern::Vec3 in4 = native_arg<kern::Vec3>(args, 4, kern::Vec3{0.0, 0.0, 0.0});
      const bool was_busy = busy_MeshData_push;
      busy_MeshData_push = true;
      kern::logic::native_call(true);
      self.MeshData_push(place0, std::move(in1), std::move(in2), std::move(in3), std::move(in4));
      kern::logic::native_call(false);
      results.push_back(to_value(place0));
      busy_MeshData_push = was_busy;
      return true;
    }
    if (function == "MeshData_push_extra") {
      kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push_extra, 0> inner0{};
      auto& place0 = busy_MeshData_push_extra ? inner0 : place_MeshData_push_extra_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push_extra, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push_extra, 0>{});
      kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push_extra, 1> inner1{};
      auto& place1 = busy_MeshData_push_extra ? inner1 : place_MeshData_push_extra_1;
      place1 = native_arg<kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push_extra, 1>>(args, 1, kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push_extra, 1>{});
      kern::i64 in2 = native_arg<kern::i64>(args, 2, 0);
      const bool was_busy = busy_MeshData_push_extra;
      busy_MeshData_push_extra = true;
      kern::logic::native_call(true);
      self.MeshData_push_extra(place0, place1, std::move(in2));
      kern::logic::native_call(false);
      results.push_back(to_value(place0));
      busy_MeshData_push_extra = was_busy;
      return true;
    }
    if (function == "MeshData_bytes") {
      kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_bytes, 0> inner0{};
      auto& place0 = busy_MeshData_bytes ? inner0 : place_MeshData_bytes_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_bytes, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_bytes, 0>{});
      const bool was_busy = busy_MeshData_bytes;
      busy_MeshData_bytes = true;
      kern::logic::native_call(true);
      const auto out = self.MeshData_bytes(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_MeshData_bytes = was_busy;
      return true;
    }
    if (function == "stream_count") {
      kern::logic::NativeParam<&engine_geo_mesh_data::stream_count, 0> inner0{};
      auto& place0 = busy_stream_count ? inner0 : place_stream_count_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_mesh_data::stream_count, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_mesh_data::stream_count, 0>{});
      kern::f64 in1 = native_arg<kern::f64>(args, 1, 0.0);
      kern::f64 in2 = native_arg<kern::f64>(args, 2, 0.0);
      const bool was_busy = busy_stream_count;
      busy_stream_count = true;
      kern::logic::native_call(true);
      const auto out = self.stream_count(place0, std::move(in1), std::move(in2));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      results.push_back(to_value(place0));
      busy_stream_count = was_busy;
      return true;
    }
    if (function == "stream_u32") {
      kern::f64 in0 = native_arg<kern::f64>(args, 0, 0.0);
      kern::logic::native_call(true);
      const auto out = self.stream_u32(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    return false;
  }
  kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_vertex_count, 0> place_MeshData_vertex_count_0{};
  bool busy_MeshData_vertex_count = false;
  kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_triangle_count, 0> place_MeshData_triangle_count_0{};
  bool busy_MeshData_triangle_count = false;
  kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_clear, 0> place_MeshData_clear_0{};
  bool busy_MeshData_clear = false;
  kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push, 0> place_MeshData_push_0{};
  bool busy_MeshData_push = false;
  kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push_extra, 0> place_MeshData_push_extra_0{};
  kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_push_extra, 1> place_MeshData_push_extra_1{};
  bool busy_MeshData_push_extra = false;
  kern::logic::NativeParam<&engine_geo_mesh_data::MeshData_bytes, 0> place_MeshData_bytes_0{};
  bool busy_MeshData_bytes = false;
  kern::logic::NativeParam<&engine_geo_mesh_data::stream_count, 0> place_stream_count_0{};
  bool busy_stream_count = false;
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_geo_mesh_data_registered =
    (kern::logic::register_native_blueprint(0x00c6dda723f32980ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_geo_mesh_data_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_c8b7695a1b35c491

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_geo_buildings" (its class: in the header)
namespace kern_blueprint_b3600570eb5cdba0 {

// How the engine reaches the Blueprint engine_geo_buildings by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_b3600570eb5cdba0
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::Rgb> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)); }
  static ::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::Rgb from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::Rgb& s) { return Value::structure({to_value(s.r), to_value(s.g), to_value(s.b)}); }
};
template <> struct Held<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::BuildingStyle> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::List<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::Rgb>>(*v.field(0)) && is_value<kern::List<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::Rgb>>(*v.field(1)) && is_value<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::Rgb>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::BuildingStyle from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::Rgb>>(*v.field(0)), from_value<kern::List<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::Rgb>>(*v.field(1)), from_value<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::Rgb>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::BuildingStyle& s) { return Value::structure({to_value(s.walls), to_value(s.roofs), to_value(s.trim), to_value(s.trim_mix)}); }
};
template <> struct Held<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::GeoChunk> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::MeshData>(*v.field(6)); }
  static ::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::GeoChunk from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::MeshData>(*v.field(6))};
  }
  static Value to(const ::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::GeoChunk& s) { return Value::structure({to_value(s.x), to_value(s.y), to_value(s.items), to_value(s.triangles), to_value(s.hash), to_value(s.welded), to_value(s.mesh)}); }
};
template <> struct Held<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::BuildingOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::f64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::BuildingStyle>(*v.field(4)); }
  static ::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::BuildingOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::BuildingStyle>(*v.field(4))};
  }
  static Value to(const ::kern_blueprint_b3600570eb5cdba0::engine_geo_buildings::BuildingOptions& s) { return Value::structure({to_value(s.chunk_size), to_value(s.max_buildings), to_value(s.cornice), to_value(s.wall_records), to_value(s.style)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_b3600570eb5cdba0 {
namespace {
struct engine_geo_buildings_Native final : kern::logic::NativeBlueprint {
  engine_geo_buildings self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_geo_buildings> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "Rgb_f") {
      kern::logic::NativeParam<&engine_geo_buildings::Rgb_f, 0> inner0{};
      auto& place0 = busy_Rgb_f ? inner0 : place_Rgb_f_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_buildings::Rgb_f, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_buildings::Rgb_f, 0>{});
      const bool was_busy = busy_Rgb_f;
      busy_Rgb_f = true;
      kern::logic::native_call(true);
      const auto out = self.Rgb_f(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Rgb_f = was_busy;
      return true;
    }
    return false;
  }
  kern::logic::NativeParam<&engine_geo_buildings::Rgb_f, 0> place_Rgb_f_0{};
  bool busy_Rgb_f = false;
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_geo_buildings_registered =
    (kern::logic::register_native_blueprint(0x3d8057f47b3ad3e2ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_geo_buildings_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_b3600570eb5cdba0

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_geo_roads" (its class: in the header)
namespace kern_blueprint_d57e83ccc1e29143 {

// How the engine reaches the Blueprint engine_geo_roads by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_d57e83ccc1e29143
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::GeoProjector> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)); }
  static ::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::GeoProjector from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::GeoProjector& s) { return Value::structure({to_value(s.lon0), to_value(s.lat0), to_value(s.cos_lat)}); }
};
template <> struct Held<::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::RoadOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 18 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::Rgb>(*v.field(14)) && is_value<kern::List<::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::Rgb>>(*v.field(15)) && is_value<kern::logic::Callback<bool(kern::f64, kern::f64, kern::f64)>>(*v.field(16)) && is_value<kern::f64>(*v.field(17)); }
  static ::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::RoadOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::Rgb>(*v.field(14)), from_value<kern::List<::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::Rgb>>(*v.field(15)), from_value<kern::logic::Callback<bool(kern::f64, kern::f64, kern::f64)>>(*v.field(16)), from_value<kern::f64>(*v.field(17))};
  }
  static Value to(const ::kern_blueprint_d57e83ccc1e29143::engine_geo_roads::RoadOptions& s) { return Value::structure({to_value(s.center_x), to_value(s.center_y), to_value(s.radius), to_value(s.chunk_size), to_value(s.segment_length), to_value(s.curb_extra), to_value(s.curb_z), to_value(s.surface_z), to_value(s.marking_z), to_value(s.marking_width), to_value(s.dash_start), to_value(s.dash_length), to_value(s.dash_period), to_value(s.uv_scale), to_value(s.curb_color), to_value(s.palette), to_value(s.deformed), to_value(s.fine_step)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_d57e83ccc1e29143 {
namespace {
struct engine_geo_roads_Native final : kern::logic::NativeBlueprint {
  engine_geo_roads self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_geo_roads> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "GeoProjector_init") {
      kern::logic::NativeParam<&engine_geo_roads::GeoProjector_init, 0> inner0{};
      auto& place0 = busy_GeoProjector_init ? inner0 : place_GeoProjector_init_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_roads::GeoProjector_init, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_roads::GeoProjector_init, 0>{});
      kern::f64 in1 = native_arg<kern::f64>(args, 1, 0.0);
      kern::f64 in2 = native_arg<kern::f64>(args, 2, 0.0);
      const bool was_busy = busy_GeoProjector_init;
      busy_GeoProjector_init = true;
      kern::logic::native_call(true);
      const auto out = self.GeoProjector_init(place0, std::move(in1), std::move(in2));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      results.push_back(to_value(place0));
      busy_GeoProjector_init = was_busy;
      return true;
    }
    if (function == "GeoProjector_project") {
      kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 0> inner0{};
      auto& place0 = busy_GeoProjector_project ? inner0 : place_GeoProjector_project_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 0>>(args, 0, kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 0>{});
      kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 1> inner1{};
      auto& place1 = busy_GeoProjector_project ? inner1 : place_GeoProjector_project_1;
      place1 = native_arg<kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 1>>(args, 1, kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 1>{});
      kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 2> inner2{};
      auto& place2 = busy_GeoProjector_project ? inner2 : place_GeoProjector_project_2;
      place2 = native_arg<kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 2>>(args, 2, kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 2>{});
      kern::f64 in3 = native_arg<kern::f64>(args, 3, 0.0);
      kern::f64 in4 = native_arg<kern::f64>(args, 4, 0.0);
      const bool was_busy = busy_GeoProjector_project;
      busy_GeoProjector_project = true;
      kern::logic::native_call(true);
      self.GeoProjector_project(place0, place1, place2, std::move(in3), std::move(in4));
      kern::logic::native_call(false);
      results.push_back(to_value(place1));
      results.push_back(to_value(place2));
      busy_GeoProjector_project = was_busy;
      return true;
    }
    return false;
  }
  kern::logic::NativeParam<&engine_geo_roads::GeoProjector_init, 0> place_GeoProjector_init_0{};
  bool busy_GeoProjector_init = false;
  kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 0> place_GeoProjector_project_0{};
  kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 1> place_GeoProjector_project_1{};
  kern::logic::NativeParam<&engine_geo_roads::GeoProjector_project, 2> place_GeoProjector_project_2{};
  bool busy_GeoProjector_project = false;
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    if (variable == "kRad") { out = kern::logic::to_value(self.kRad); return true; }
    if (variable == "kEarthRadius") { out = kern::logic::to_value(self.kEarthRadius); return true; }
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_geo_roads_registered =
    (kern::logic::register_native_blueprint(0x1fd384ad3f15d2e7ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_geo_roads_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_d57e83ccc1e29143

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_gfx_shader" (its class: in the header)
namespace kern_blueprint_1ce45fbbb3818596 {

// How the engine reaches the Blueprint engine_gfx_shader by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_1ce45fbbb3818596
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_1ce45fbbb3818596::engine_gfx_shader::ExtensionAttribute> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::logic::Text>(*v.field(2)); }
  static ::kern_blueprint_1ce45fbbb3818596::engine_gfx_shader::ExtensionAttribute from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::logic::Text>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_1ce45fbbb3818596::engine_gfx_shader::ExtensionAttribute& s) { return Value::structure({to_value(s.key), to_value(s.size), to_value(s.glsl)}); }
};
template <> struct Held<::kern_blueprint_1ce45fbbb3818596::engine_gfx_shader::ShaderSource> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)); }
  static ::kern_blueprint_1ce45fbbb3818596::engine_gfx_shader::ShaderSource from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1))};
  }
  static Value to(const ::kern_blueprint_1ce45fbbb3818596::engine_gfx_shader::ShaderSource& s) { return Value::structure({to_value(s.vertex), to_value(s.fragment)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_1ce45fbbb3818596 {
namespace {
struct engine_gfx_shader_Native final : kern::logic::NativeBlueprint {
  engine_gfx_shader self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_gfx_shader> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "ShaderExtension_attributes") {
      kern::logic::native_call(true);
      const auto out = self.ShaderExtension_attributes();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "ShaderExtension_vertex_declarations") {
      kern::logic::native_call(true);
      const auto out = self.ShaderExtension_vertex_declarations();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "ShaderExtension_vertex") {
      kern::logic::native_call(true);
      const auto out = self.ShaderExtension_vertex();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "ShaderExtension_fragment_declarations") {
      kern::logic::native_call(true);
      const auto out = self.ShaderExtension_fragment_declarations();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "ShaderExtension_fragment_discard") {
      kern::logic::native_call(true);
      const auto out = self.ShaderExtension_fragment_discard();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "ShaderExtension_fragment_input") {
      kern::logic::native_call(true);
      const auto out = self.ShaderExtension_fragment_input();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "ShaderExtension_fragment_surface") {
      kern::logic::native_call(true);
      const auto out = self.ShaderExtension_fragment_surface();
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "ShaderExtension_bind") {
      kern::i64 in0 = native_arg<kern::i64>(args, 0, 0);
      kern::logic::native_call(true);
      self.ShaderExtension_bind(std::move(in0));
      kern::logic::native_call(false);
      return true;
    }
    if (function == "ShaderExtension_material_uniforms") {
      kern::logic::NativeParam<&engine_gfx_shader::ShaderExtension_material_uniforms, 0> inner0{};
      auto& place0 = busy_ShaderExtension_material_uniforms ? inner0 : place_ShaderExtension_material_uniforms_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_gfx_shader::ShaderExtension_material_uniforms, 0>>(args, 0, kern::logic::NativeParam<&engine_gfx_shader::ShaderExtension_material_uniforms, 0>{});
      kern::logic::NativeParam<&engine_gfx_shader::ShaderExtension_material_uniforms, 1> inner1{};
      auto& place1 = busy_ShaderExtension_material_uniforms ? inner1 : place_ShaderExtension_material_uniforms_1;
      place1 = native_arg<kern::logic::NativeParam<&engine_gfx_shader::ShaderExtension_material_uniforms, 1>>(args, 1, kern::logic::NativeParam<&engine_gfx_shader::ShaderExtension_material_uniforms, 1>{});
      kern::i64 in2 = native_arg<kern::i64>(args, 2, 0);
      kern::i64 in3 = native_arg<kern::i64>(args, 3, 0);
      const bool was_busy = busy_ShaderExtension_material_uniforms;
      busy_ShaderExtension_material_uniforms = true;
      kern::logic::native_call(true);
      self.ShaderExtension_material_uniforms(place0, place1, std::move(in2), std::move(in3));
      kern::logic::native_call(false);
      busy_ShaderExtension_material_uniforms = was_busy;
      return true;
    }
    return false;
  }
  kern::logic::NativeParam<&engine_gfx_shader::ShaderExtension_material_uniforms, 0> place_ShaderExtension_material_uniforms_0{};
  kern::logic::NativeParam<&engine_gfx_shader::ShaderExtension_material_uniforms, 1> place_ShaderExtension_material_uniforms_1{};
  bool busy_ShaderExtension_material_uniforms = false;
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_gfx_shader_registered =
    (kern::logic::register_native_blueprint(0xe0ac0b615048d968ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_gfx_shader_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_1ce45fbbb3818596

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_logic_logic" (its class: in the header)
namespace kern_blueprint_f26fb4f0474c453a {

// How the engine reaches the Blueprint engine_logic_logic by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
namespace {
struct engine_logic_logic_Native final : kern::logic::NativeBlueprint {
  engine_logic_logic self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_logic_logic> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "width_bits") {
      kern::logic::Text in0 = native_arg<kern::logic::Text>(args, 0, "");
      kern::logic::native_call(true);
      const auto out = self.width_bits(std::move(in0));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    if (function == "wrapped") {
      kern::logic::Int64 in0 = native_arg<kern::logic::Int64>(args, 0, kern::logic::Int64{0x0});
      kern::logic::Text in1 = native_arg<kern::logic::Text>(args, 1, "");
      kern::logic::native_call(true);
      const auto out = self.wrapped(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    if (variable == "kWidthNames") { out = kern::logic::to_value(self.kWidthNames); return true; }
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_logic_logic_registered =
    (kern::logic::register_native_blueprint(0xcf8ea180c5841978ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_logic_logic_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_f26fb4f0474c453a

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_net_reliable" (its class: in the header)
namespace kern_blueprint_d7bae0e5d6425fab {

// How the engine reaches the Blueprint engine_net_reliable by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_d7bae0e5d6425fab
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_d7bae0e5d6425fab::engine_net_reliable::net_EndpointConfig> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 12 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::i64>(*v.field(11)); }
  static ::kern_blueprint_d7bae0e5d6425fab::engine_net_reliable::net_EndpointConfig from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::i64>(*v.field(11))};
  }
  static Value to(const ::kern_blueprint_d7bae0e5d6425fab::engine_net_reliable::net_EndpointConfig& s) { return Value::structure({to_value(s.max_packet_size), to_value(s.fragment_above), to_value(s.fragment_size), to_value(s.max_fragments), to_value(s.ack_buffer_size), to_value(s.sent_packets_buffer_size), to_value(s.received_packets_buffer_size), to_value(s.fragment_reassembly_buffer_size), to_value(s.rtt_smoothing), to_value(s.packet_loss_smoothing), to_value(s.bandwidth_smoothing), to_value(s.packet_header_size)}); }
};
template <> struct Held<::kern_blueprint_d7bae0e5d6425fab::engine_net_reliable::net_EndpointStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 15 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::logic::Int64>(*v.field(5)) && is_value<kern::logic::Int64>(*v.field(6)) && is_value<kern::logic::Int64>(*v.field(7)) && is_value<kern::logic::Int64>(*v.field(8)) && is_value<kern::logic::Int64>(*v.field(9)) && is_value<kern::logic::Int64>(*v.field(10)) && is_value<kern::logic::Int64>(*v.field(11)) && is_value<kern::logic::Int64>(*v.field(12)) && is_value<kern::logic::Int64>(*v.field(13)) && is_value<kern::logic::Int64>(*v.field(14)); }
  static ::kern_blueprint_d7bae0e5d6425fab::engine_net_reliable::net_EndpointStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::logic::Int64>(*v.field(5)), from_value<kern::logic::Int64>(*v.field(6)), from_value<kern::logic::Int64>(*v.field(7)), from_value<kern::logic::Int64>(*v.field(8)), from_value<kern::logic::Int64>(*v.field(9)), from_value<kern::logic::Int64>(*v.field(10)), from_value<kern::logic::Int64>(*v.field(11)), from_value<kern::logic::Int64>(*v.field(12)), from_value<kern::logic::Int64>(*v.field(13)), from_value<kern::logic::Int64>(*v.field(14))};
  }
  static Value to(const ::kern_blueprint_d7bae0e5d6425fab::engine_net_reliable::net_EndpointStats& s) { return Value::structure({to_value(s.rtt_ms), to_value(s.packet_loss), to_value(s.sent_kbps), to_value(s.received_kbps), to_value(s.acked_kbps), to_value(s.packets_sent), to_value(s.packets_received), to_value(s.packets_acked), to_value(s.packets_stale), to_value(s.packets_invalid), to_value(s.packets_too_large_to_send), to_value(s.packets_too_large_to_receive), to_value(s.fragments_sent), to_value(s.fragments_received), to_value(s.fragments_invalid)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_d7bae0e5d6425fab {
namespace {
struct engine_net_reliable_Native final : kern::logic::NativeBlueprint {
  engine_net_reliable self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_net_reliable> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_net_reliable_registered =
    (kern::logic::register_native_blueprint(0xd1347719c85eb9aaull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_net_reliable_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_d7bae0e5d6425fab

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_net_connection" (its class: in the header)
namespace kern_blueprint_98bcbda0c963ae2a {

// How the engine reaches the Blueprint engine_net_connection by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_98bcbda0c963ae2a
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_Message> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::List<kern::logic::Sized<kern::u8>>>(*v.field(2)); }
  static ::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_Message from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::List<kern::logic::Sized<kern::u8>>>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_Message& s) { return Value::structure({to_value(s.id), to_value(s.type), to_value(s.data)}); }
};
template <> struct Held<::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ChannelConfig> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 9 && is_value<::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ChannelType>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)); }
  static ::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ChannelConfig from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ChannelType>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<kern::f64>(*v.field(8))};
  }
  static Value to(const ::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ChannelConfig& s) { return Value::structure({to_value(s.type), to_value(s.send_queue_size), to_value(s.receive_queue_size), to_value(s.max_messages_per_packet), to_value(s.packet_budget), to_value(s.max_message_size), to_value(s.block_threshold), to_value(s.block_fragment_size), to_value(s.resend_time)}); }
};
template <> struct Held<::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ConnectionConfig> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::List<::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ChannelConfig>>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_EndpointConfig>(*v.field(3)); }
  static ::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ConnectionConfig from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ChannelConfig>>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_EndpointConfig>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_98bcbda0c963ae2a::engine_net_connection::net_ConnectionConfig& s) { return Value::structure({to_value(s.channels), to_value(s.max_packet_size), to_value(s.max_message_types), to_value(s.endpoint)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_98bcbda0c963ae2a {
namespace {
struct engine_net_connection_Native final : kern::logic::NativeBlueprint {
  engine_net_connection self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_net_connection> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_net_connection_registered =
    (kern::logic::register_native_blueprint(0x9a31b6881237f0f7ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_net_connection_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_98bcbda0c963ae2a

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_net_client_server" (its class: in the header)
namespace kern_blueprint_63e9260b4af96bb7 {

// How the engine reaches the Blueprint engine_net_client_server by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_63e9260b4af96bb7
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_63e9260b4af96bb7::engine_net_client_server::net_ClientServerConfig> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::logic::Int64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<::kern_blueprint_63e9260b4af96bb7::engine_net_client_server::net_ConnectionConfig>(*v.field(6)); }
  static ::kern_blueprint_63e9260b4af96bb7::engine_net_client_server::net_ClientServerConfig from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<::kern_blueprint_63e9260b4af96bb7::engine_net_client_server::net_ConnectionConfig>(*v.field(6))};
  }
  static Value to(const ::kern_blueprint_63e9260b4af96bb7::engine_net_client_server::net_ClientServerConfig& s) { return Value::structure({to_value(s.protocol_id), to_value(s.max_clients), to_value(s.timeout), to_value(s.request_interval), to_value(s.connect_timeout), to_value(s.keepalive_interval), to_value(s.connection)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_63e9260b4af96bb7 {
namespace {
struct engine_net_client_server_Native final : kern::logic::NativeBlueprint {
  engine_net_client_server self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_net_client_server> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_net_client_server_registered =
    (kern::logic::register_native_blueprint(0xeb075ea73046c169ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_net_client_server_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_63e9260b4af96bb7

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_net_transport" (its class: in the header)
namespace kern_blueprint_21ccd5517d15e265 {

// How the engine reaches the Blueprint engine_net_transport by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_21ccd5517d15e265
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Address> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::logic::Int64>(*v.field(0)); }
  static ::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Address from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0))};
  }
  static Value to(const ::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Address& s) { return Value::structure({to_value(s.id)}); }
};
template <> struct Held<::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_SimulatorSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_SimulatorSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_SimulatorSettings& s) { return Value::structure({to_value(s.latency_ms), to_value(s.jitter_ms), to_value(s.loss_percent), to_value(s.duplicate_percent)}); }
};
template <> struct Held<::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Simulator_Packet> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Address>(*v.field(0)) && is_value<::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Address>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::List<kern::logic::Sized<kern::u8>>>(*v.field(3)); }
  static ::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Simulator_Packet from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Address>(*v.field(0)), from_value<::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Address>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::List<kern::logic::Sized<kern::u8>>>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_21ccd5517d15e265::engine_net_transport::net_Simulator_Packet& s) { return Value::structure({to_value(s.from), to_value(s.to), to_value(s.deliver), to_value(s.data)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_21ccd5517d15e265 {
namespace {
struct engine_net_transport_Native final : kern::logic::NativeBlueprint {
  engine_net_transport self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_net_transport> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "Address_valid") {
      kern::logic::NativeParam<&engine_net_transport::Address_valid, 0> inner0{};
      auto& place0 = busy_Address_valid ? inner0 : place_Address_valid_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_net_transport::Address_valid, 0>>(args, 0, kern::logic::NativeParam<&engine_net_transport::Address_valid, 0>{});
      const bool was_busy = busy_Address_valid;
      busy_Address_valid = true;
      kern::logic::native_call(true);
      const auto out = self.Address_valid(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Address_valid = was_busy;
      return true;
    }
    if (function == "Transport_advance_time") {
      kern::f64 in0 = native_arg<kern::f64>(args, 0, 0.0);
      kern::logic::native_call(true);
      self.Transport_advance_time(std::move(in0));
      kern::logic::native_call(false);
      return true;
    }
    if (function == "SimulatorSettings_active") {
      kern::logic::NativeParam<&engine_net_transport::SimulatorSettings_active, 0> inner0{};
      auto& place0 = busy_SimulatorSettings_active ? inner0 : place_SimulatorSettings_active_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_net_transport::SimulatorSettings_active, 0>>(args, 0, kern::logic::NativeParam<&engine_net_transport::SimulatorSettings_active, 0>{});
      const bool was_busy = busy_SimulatorSettings_active;
      busy_SimulatorSettings_active = true;
      kern::logic::native_call(true);
      const auto out = self.SimulatorSettings_active(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_SimulatorSettings_active = was_busy;
      return true;
    }
    return false;
  }
  kern::logic::NativeParam<&engine_net_transport::Address_valid, 0> place_Address_valid_0{};
  bool busy_Address_valid = false;
  kern::logic::NativeParam<&engine_net_transport::SimulatorSettings_active, 0> place_SimulatorSettings_active_0{};
  bool busy_SimulatorSettings_active = false;
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_net_transport_registered =
    (kern::logic::register_native_blueprint(0x9c15ee354571eb0dull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_net_transport_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_21ccd5517d15e265

// ---------------------------------------------------------------------------
// The Blueprint of actor "engine_physics_physics" (its class: in the header)
namespace kern_blueprint_c124d3ceac6e0826 {

// How the engine reaches the Blueprint engine_physics_physics by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
}  // namespace kern_blueprint_c124d3ceac6e0826
namespace kern::logic::detail {
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::logic::Int64>(*v.field(0)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body& s) { return Value::structure({to_value(s.id)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Capsule> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Capsule from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Capsule& s) { return Value::structure({to_value(s.center1), to_value(s.center2), to_value(s.radius)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_CharacterMover_Settings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 14 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<kern::logic::Int64>(*v.field(13)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_CharacterMover_Settings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<kern::logic::Int64>(*v.field(13))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_CharacterMover_Settings& s) { return Value::structure({to_value(s.radius), to_value(s.height), to_value(s.step_height), to_value(s.max_speed), to_value(s.accelerate), to_value(s.friction), to_value(s.air_control), to_value(s.stop_speed), to_value(s.jump_speed), to_value(s.sprint), to_value(s.gravity), to_value(s.max_slope_deg), to_value(s.push_strength), to_value(s.mask_bits)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ContactPoint> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ContactPoint from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ContactPoint& s) { return Value::structure({to_value(s.point), to_value(s.normal), to_value(s.separation), to_value(s.impulse)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Joint> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::logic::Int64>(*v.field(0)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Joint from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Joint& s) { return Value::structure({to_value(s.id)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_MotionLocks> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::List<bool>>(*v.field(0)) && is_value<kern::List<bool>>(*v.field(1)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_MotionLocks from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<bool>>(*v.field(0)), from_value<kern::List<bool>>(*v.field(1))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_MotionLocks& s) { return Value::structure({to_value(s.linear), to_value(s.angular)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_QueryFilter> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::logic::Int64>(*v.field(0)) && is_value<kern::logic::Int64>(*v.field(1)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_QueryFilter from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0)), from_value<kern::logic::Int64>(*v.field(1))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_QueryFilter& s) { return Value::structure({to_value(s.mask_bits), to_value(s.category_bits)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::logic::Int64>(*v.field(0)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape& s) { return Value::structure({to_value(s.id)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Version> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Version from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Version& s) { return Value::structure({to_value(s.major), to_value(s.minor), to_value(s.revision)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_WorldSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 11 && is_value<kern::Vec3>(*v.field(0)) && is_value<bool>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_WorldSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<bool>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_WorldSettings& s) { return Value::structure({to_value(s.gravity), to_value(s.enable_sleep), to_value(s.enable_continuous), to_value(s.restitution_threshold), to_value(s.restitution_iterations), to_value(s.restitution_propagation), to_value(s.hit_event_threshold), to_value(s.contact_hertz), to_value(s.contact_damping_ratio), to_value(s.contact_speed), to_value(s.maximum_linear_speed)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_BodySettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 13 && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_BodyType>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::Quat>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<bool>(*v.field(10)) && is_value<bool>(*v.field(11)) && is_value<kern::logic::Int64>(*v.field(12)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_BodySettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_BodyType>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::Quat>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<bool>(*v.field(10)), from_value<bool>(*v.field(11)), from_value<kern::logic::Int64>(*v.field(12))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_BodySettings& s) { return Value::structure({to_value(s.type), to_value(s.position), to_value(s.rotation), to_value(s.linear_velocity), to_value(s.angular_velocity), to_value(s.linear_damping), to_value(s.angular_damping), to_value(s.gravity_scale), to_value(s.enable_sleep), to_value(s.awake), to_value(s.enabled), to_value(s.bullet), to_value(s.user)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_HitEvent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(0)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(1)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(2)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::Vec3>(*v.field(5)) && is_value<kern::f64>(*v.field(6)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_HitEvent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(0)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(1)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(2)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::Vec3>(*v.field(5)), from_value<kern::f64>(*v.field(6))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_HitEvent& s) { return Value::structure({to_value(s.shape_a), to_value(s.shape_b), to_value(s.body_a), to_value(s.body_b), to_value(s.point), to_value(s.normal), to_value(s.approach_speed)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_MoverPlane> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(3)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(4)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_MoverPlane from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(3)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(4))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_MoverPlane& s) { return Value::structure({to_value(s.normal), to_value(s.offset), to_value(s.point), to_value(s.shape), to_value(s.body)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Pose> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::Vec3>(*v.field(0)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::Quat>(*v.field(1)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Pose from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::Quat>(*v.field(1))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Pose& s) { return Value::structure({to_value(s.position), to_value(s.rotation)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_RayHit> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<bool>(*v.field(0)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(1)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::f64>(*v.field(5)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_RayHit from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<bool>(*v.field(0)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(1)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::f64>(*v.field(5))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_RayHit& s) { return Value::structure({to_value(s.hit), to_value(s.shape), to_value(s.body), to_value(s.point), to_value(s.normal), to_value(s.fraction)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeMaterial> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 13 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<kern::logic::Int64>(*v.field(5)) && is_value<kern::logic::Int64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<bool>(*v.field(10)) && is_value<kern::logic::Int64>(*v.field(11)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Surface>(*v.field(12)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeMaterial from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<kern::logic::Int64>(*v.field(5)), from_value<kern::logic::Int64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<bool>(*v.field(10)), from_value<kern::logic::Int64>(*v.field(11)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Surface>(*v.field(12))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeMaterial& s) { return Value::structure({to_value(s.density), to_value(s.friction), to_value(s.restitution), to_value(s.rolling_resistance), to_value(s.sensor), to_value(s.category_bits), to_value(s.mask_bits), to_value(s.group_index), to_value(s.contact_events), to_value(s.sensor_events), to_value(s.hit_events), to_value(s.user), to_value(s.surface)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_TouchEvent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(0)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(1)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(2)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(3)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_TouchEvent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(0)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Shape>(*v.field(1)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(2)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(3))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_TouchEvent& s) { return Value::structure({to_value(s.shape_a), to_value(s.shape_b), to_value(s.body_a), to_value(s.body_b)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_JointSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 40 && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_JointType>(*v.field(0)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(1)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(2)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Pose>(*v.field(3)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Pose>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<kern::f64>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<kern::Vec3>(*v.field(16)) && is_value<kern::f64>(*v.field(17)) && is_value<kern::f64>(*v.field(18)) && is_value<kern::f64>(*v.field(19)) && is_value<kern::f64>(*v.field(20)) && is_value<kern::f64>(*v.field(21)) && is_value<kern::Vec3>(*v.field(22)) && is_value<kern::Vec3>(*v.field(23)) && is_value<kern::f64>(*v.field(24)) && is_value<kern::f64>(*v.field(25)) && is_value<kern::f64>(*v.field(26)) && is_value<kern::f64>(*v.field(27)) && is_value<bool>(*v.field(28)) && is_value<bool>(*v.field(29)) && is_value<kern::f64>(*v.field(30)) && is_value<kern::f64>(*v.field(31)) && is_value<kern::f64>(*v.field(32)) && is_value<kern::f64>(*v.field(33)) && is_value<kern::f64>(*v.field(34)) && is_value<kern::f64>(*v.field(35)) && is_value<kern::f64>(*v.field(36)) && is_value<kern::f64>(*v.field(37)) && is_value<kern::f64>(*v.field(38)) && is_value<kern::logic::Int64>(*v.field(39)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_JointSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_JointType>(*v.field(0)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(1)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Body>(*v.field(2)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Pose>(*v.field(3)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_Pose>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<kern::f64>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<kern::Vec3>(*v.field(16)), from_value<kern::f64>(*v.field(17)), from_value<kern::f64>(*v.field(18)), from_value<kern::f64>(*v.field(19)), from_value<kern::f64>(*v.field(20)), from_value<kern::f64>(*v.field(21)), from_value<kern::Vec3>(*v.field(22)), from_value<kern::Vec3>(*v.field(23)), from_value<kern::f64>(*v.field(24)), from_value<kern::f64>(*v.field(25)), from_value<kern::f64>(*v.field(26)), from_value<kern::f64>(*v.field(27)), from_value<bool>(*v.field(28)), from_value<bool>(*v.field(29)), from_value<kern::f64>(*v.field(30)), from_value<kern::f64>(*v.field(31)), from_value<kern::f64>(*v.field(32)), from_value<kern::f64>(*v.field(33)), from_value<kern::f64>(*v.field(34)), from_value<kern::f64>(*v.field(35)), from_value<kern::f64>(*v.field(36)), from_value<kern::f64>(*v.field(37)), from_value<kern::f64>(*v.field(38)), from_value<kern::logic::Int64>(*v.field(39))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_JointSettings& s) { return Value::structure({to_value(s.type), to_value(s.body_a), to_value(s.body_b), to_value(s.frame_a), to_value(s.frame_b), to_value(s.collide_connected), to_value(s.enable_spring), to_value(s.hertz), to_value(s.damping_ratio), to_value(s.enable_limit), to_value(s.lower), to_value(s.upper), to_value(s.enable_motor), to_value(s.motor_speed), to_value(s.max_motor_torque), to_value(s.max_motor_force), to_value(s.motor_velocity), to_value(s.length), to_value(s.linear_hertz), to_value(s.angular_hertz), to_value(s.linear_damping_ratio), to_value(s.angular_damping_ratio), to_value(s.linear_velocity), to_value(s.angular_velocity), to_value(s.max_velocity_force), to_value(s.max_velocity_torque), to_value(s.max_spring_force), to_value(s.max_spring_torque), to_value(s.enable_steering), to_value(s.enable_steering_limit), to_value(s.steering_hertz), to_value(s.steering_damping_ratio), to_value(s.target_steering), to_value(s.max_steering_torque), to_value(s.steering_lower), to_value(s.steering_upper), to_value(s.max_torque), to_value(s.constraint_hertz), to_value(s.constraint_damping_ratio), to_value(s.user)}); }
};
template <> struct Held<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 9 && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeKind>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::Quat>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::List<kern::Vec3>>(*v.field(6)) && is_value<kern::List<kern::logic::Sized<kern::u32>>>(*v.field(7)) && is_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeMaterial>(*v.field(8)); }
  static ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeKind>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::Quat>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::List<kern::Vec3>>(*v.field(6)), from_value<kern::List<kern::logic::Sized<kern::u32>>>(*v.field(7)), from_value<::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeMaterial>(*v.field(8))};
  }
  static Value to(const ::kern_blueprint_c124d3ceac6e0826::engine_physics_physics::physics_ShapeSettings& s) { return Value::structure({to_value(s.kind), to_value(s.center), to_value(s.rotation), to_value(s.size), to_value(s.radius), to_value(s.length), to_value(s.points), to_value(s.indices), to_value(s.material)}); }
};
}  // namespace kern::logic::detail
namespace kern_blueprint_c124d3ceac6e0826 {
namespace {
struct engine_physics_physics_Native final : kern::logic::NativeBlueprint {
  engine_physics_physics self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void* object(const void* type) override { return type == &kern::logic::class_key<engine_physics_physics> ? &self : nullptr; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, std::span<const kern::logic::Value> args, kern::logic::Results& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "Body_valid") {
      kern::logic::NativeParam<&engine_physics_physics::Body_valid, 0> inner0{};
      auto& place0 = busy_Body_valid ? inner0 : place_Body_valid_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_physics_physics::Body_valid, 0>>(args, 0, kern::logic::NativeParam<&engine_physics_physics::Body_valid, 0>{});
      const bool was_busy = busy_Body_valid;
      busy_Body_valid = true;
      kern::logic::native_call(true);
      const auto out = self.Body_valid(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Body_valid = was_busy;
      return true;
    }
    if (function == "Body_equal") {
      kern::logic::NativeParam<&engine_physics_physics::Body_equal, 0> inner0{};
      auto& place0 = busy_Body_equal ? inner0 : place_Body_equal_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_physics_physics::Body_equal, 0>>(args, 0, kern::logic::NativeParam<&engine_physics_physics::Body_equal, 0>{});
      kern::logic::NativeParam<&engine_physics_physics::Body_equal, 1> inner1{};
      auto& place1 = busy_Body_equal ? inner1 : place_Body_equal_1;
      place1 = native_arg<kern::logic::NativeParam<&engine_physics_physics::Body_equal, 1>>(args, 1, kern::logic::NativeParam<&engine_physics_physics::Body_equal, 1>{});
      const bool was_busy = busy_Body_equal;
      busy_Body_equal = true;
      kern::logic::native_call(true);
      const auto out = self.Body_equal(place0, place1);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Body_equal = was_busy;
      return true;
    }
    if (function == "Shape_valid") {
      kern::logic::NativeParam<&engine_physics_physics::Shape_valid, 0> inner0{};
      auto& place0 = busy_Shape_valid ? inner0 : place_Shape_valid_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_physics_physics::Shape_valid, 0>>(args, 0, kern::logic::NativeParam<&engine_physics_physics::Shape_valid, 0>{});
      const bool was_busy = busy_Shape_valid;
      busy_Shape_valid = true;
      kern::logic::native_call(true);
      const auto out = self.Shape_valid(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Shape_valid = was_busy;
      return true;
    }
    if (function == "Shape_equal") {
      kern::logic::NativeParam<&engine_physics_physics::Shape_equal, 0> inner0{};
      auto& place0 = busy_Shape_equal ? inner0 : place_Shape_equal_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_physics_physics::Shape_equal, 0>>(args, 0, kern::logic::NativeParam<&engine_physics_physics::Shape_equal, 0>{});
      kern::logic::NativeParam<&engine_physics_physics::Shape_equal, 1> inner1{};
      auto& place1 = busy_Shape_equal ? inner1 : place_Shape_equal_1;
      place1 = native_arg<kern::logic::NativeParam<&engine_physics_physics::Shape_equal, 1>>(args, 1, kern::logic::NativeParam<&engine_physics_physics::Shape_equal, 1>{});
      const bool was_busy = busy_Shape_equal;
      busy_Shape_equal = true;
      kern::logic::native_call(true);
      const auto out = self.Shape_equal(place0, place1);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Shape_equal = was_busy;
      return true;
    }
    if (function == "Joint_valid") {
      kern::logic::NativeParam<&engine_physics_physics::Joint_valid, 0> inner0{};
      auto& place0 = busy_Joint_valid ? inner0 : place_Joint_valid_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_physics_physics::Joint_valid, 0>>(args, 0, kern::logic::NativeParam<&engine_physics_physics::Joint_valid, 0>{});
      const bool was_busy = busy_Joint_valid;
      busy_Joint_valid = true;
      kern::logic::native_call(true);
      const auto out = self.Joint_valid(place0);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Joint_valid = was_busy;
      return true;
    }
    if (function == "Joint_equal") {
      kern::logic::NativeParam<&engine_physics_physics::Joint_equal, 0> inner0{};
      auto& place0 = busy_Joint_equal ? inner0 : place_Joint_equal_0;
      place0 = native_arg<kern::logic::NativeParam<&engine_physics_physics::Joint_equal, 0>>(args, 0, kern::logic::NativeParam<&engine_physics_physics::Joint_equal, 0>{});
      kern::logic::NativeParam<&engine_physics_physics::Joint_equal, 1> inner1{};
      auto& place1 = busy_Joint_equal ? inner1 : place_Joint_equal_1;
      place1 = native_arg<kern::logic::NativeParam<&engine_physics_physics::Joint_equal, 1>>(args, 1, kern::logic::NativeParam<&engine_physics_physics::Joint_equal, 1>{});
      const bool was_busy = busy_Joint_equal;
      busy_Joint_equal = true;
      kern::logic::native_call(true);
      const auto out = self.Joint_equal(place0, place1);
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      busy_Joint_equal = was_busy;
      return true;
    }
    if (function == "QueryFilter_new") {
      kern::logic::Int64 in0 = native_arg<kern::logic::Int64>(args, 0, kern::logic::Int64{0x0});
      kern::logic::Int64 in1 = native_arg<kern::logic::Int64>(args, 1, kern::logic::Int64{0x0});
      kern::logic::native_call(true);
      const auto out = self.QueryFilter_new(std::move(in0), std::move(in1));
      kern::logic::native_call(false);
      results = {to_value(out.Result)};
      return true;
    }
    return false;
  }
  kern::logic::NativeParam<&engine_physics_physics::Body_valid, 0> place_Body_valid_0{};
  bool busy_Body_valid = false;
  kern::logic::NativeParam<&engine_physics_physics::Body_equal, 0> place_Body_equal_0{};
  kern::logic::NativeParam<&engine_physics_physics::Body_equal, 1> place_Body_equal_1{};
  bool busy_Body_equal = false;
  kern::logic::NativeParam<&engine_physics_physics::Shape_valid, 0> place_Shape_valid_0{};
  bool busy_Shape_valid = false;
  kern::logic::NativeParam<&engine_physics_physics::Shape_equal, 0> place_Shape_equal_0{};
  kern::logic::NativeParam<&engine_physics_physics::Shape_equal, 1> place_Shape_equal_1{};
  bool busy_Shape_equal = false;
  kern::logic::NativeParam<&engine_physics_physics::Joint_valid, 0> place_Joint_valid_0{};
  bool busy_Joint_valid = false;
  kern::logic::NativeParam<&engine_physics_physics::Joint_equal, 0> place_Joint_equal_0{};
  kern::logic::NativeParam<&engine_physics_physics::Joint_equal, 1> place_Joint_equal_1{};
  bool busy_Joint_equal = false;
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    return false;
  }
};
[[maybe_unused]] const bool engine_physics_physics_registered =
    (kern::logic::register_native_blueprint(0x9920f461dd709630ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new engine_physics_physics_Native()); }), true);
}  // namespace

}  // namespace kern_blueprint_c124d3ceac6e0826
