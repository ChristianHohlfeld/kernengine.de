// Procedural vehicle models (see vehicle_art.h).
#include "vehicle_art.h"

#include "kern/geo/primitives.h"
#include "jet.h"

namespace sc {

namespace {
using kern::Mat4;
using kern::MeshData;
namespace prim = kern::primitives;

Mat4 rot(Vec3 axis, f32 a) { return Mat4::trs({}, Quat::axis_angle(axis, a), {1, 1, 1}); }
Mat4 at(Vec3 p) { return Mat4::translate(p); }

MeshData hull(const std::vector<Vec3>& pts, Vec3 color) {
  MeshData m;
  kern::physics::hull_mesh(kern::Span<const Vec3>(pts.data(), pts.size()), m, color);
  return m;
}

kern::Material mat(f32 roughness, f32 metalness = 0, Vec3 emissive = {}) {
  kern::Material m;
  m.roughness = roughness;
  m.metalness = metalness;
  m.emissive = emissive;
  return m;
}
}  // namespace

bool VehicleArt::make(kern::Renderer& r, Part& p, const MeshData& mesh, const kern::Material& m, bool tinted) {
  p.id = r.allocate_id();
  p.tinted = tinted;
  kern::MeshInput in = mesh.as_input(kern::kMaxExtensionAttributes);
  in.material = m;
  in.visible = false;
  static const Mat4 parked = Mat4::translate({0, 0, -1000});  // hidden until the first frame places it
  in.transforms = kern::Span<const f32>(parked.m, 16);
  if (!r.upsert(p.id, in)) { r.release_id(p.id); p.id = 0; return false; }
  return true;
}

bool VehicleArt::init(kern::Renderer& r) {
  r_ = &r;
  const Vec3 white{1, 1, 1};
  // ---- car (chassis frame: +X forward, +Y left; centre 0.54 m above the ground)
  {
    MeshData body;
    prim::append(body, prim::box({3.56f, 1.64f, 0.5f}, white), at({0, 0, -0.04f}));
    prim::append(body, prim::box({3.3f, 1.6f, 0.14f}, white), at({-0.05f, 0, 0.27f}));     // shoulder line
    prim::append(body, prim::box({0.9f, 1.5f, 0.1f}, white), at({1.25f, 0, 0.26f}));       // bonnet
    MeshData bumpers;
    prim::append(bumpers, prim::box({0.16f, 1.66f, 0.2f}, {0.12f, 0.12f, 0.13f}), at({1.79f, 0, -0.18f}));
    prim::append(bumpers, prim::box({0.16f, 1.66f, 0.2f}, {0.12f, 0.12f, 0.13f}), at({-1.79f, 0, -0.18f}));
    prim::append(body, bumpers, Mat4{});
    if (!make(r, car_body_, body, mat(0.32f, 0.35f), true)) return false;
    std::vector<Vec3> g;
    const std::vector<Vec2> prof{{-1.02f, 0.8f}, {-0.64f, 1.37f}, {0.3f, 1.37f}, {0.86f, 0.8f}};
    for (f32 y : {-0.71f, 0.71f})
      for (Vec2 p : prof) g.push_back({p.x, y, p.y - 0.54f});
    if (!make(r, car_glass_, hull(g, {0.08f, 0.1f, 0.13f}), mat(0.08f, 0.2f), false)) return false;
    MeshData lights;
    prim::append(lights, prim::box({0.06f, 0.36f, 0.12f}, white), at({1.85f, 0.56f, 0.06f}));
    prim::append(lights, prim::box({0.06f, 0.36f, 0.12f}, white), at({1.85f, -0.56f, 0.06f}));
    if (!make(r, car_lights_, lights, mat(0.2f, 0, {2.5f, 2.4f, 2.1f}), false)) return false;
    MeshData tail;
    prim::append(tail, prim::box({0.06f, 0.4f, 0.1f}, {0.7f, 0.05f, 0.04f}), at({-1.85f, 0.55f, 0.06f}));
    prim::append(tail, prim::box({0.06f, 0.4f, 0.1f}, {0.7f, 0.05f, 0.04f}), at({-1.85f, -0.55f, 0.06f}));
    if (!make(r, car_tail_, tail, mat(0.3f, 0, {0.9f, 0.05f, 0.03f}), true)) return false;
    // A wheel: tyre + rim along its axle (+Y), unit radius scaled per vehicle.
    MeshData wheel;
    prim::append(wheel, prim::cylinder(1.0f, 0.62f, 18, {0.07f, 0.07f, 0.08f}), rot({1, 0, 0}, -kPi / 2) * at({0, 0, -0.31f}));
    prim::append(wheel, prim::cylinder(0.62f, 0.66f, 10, {0.62f, 0.63f, 0.66f}), rot({1, 0, 0}, -kPi / 2) * at({0, 0, -0.33f}));
    if (!make(r, wheel_, wheel, mat(0.7f, 0.2f), false)) return false;
    // bus (half 4 x 1.17 x 1.2; centre 1.48 m above ground)
    MeshData bus;
    prim::append(bus, prim::box({8.0f, 2.42f, 2.0f}, white), at({0, 0, 0.1f}));
    prim::append(bus, prim::box({7.9f, 2.38f, 0.32f}, white), at({0, 0, 1.4f}));
    if (!make(r, bus_body_, bus, mat(0.4f, 0.2f), true)) return false;
    MeshData bus_glass;
    prim::append(bus_glass, prim::box({7.4f, 2.46f, 0.85f}, {0.08f, 0.1f, 0.13f}), at({0.1f, 0, 0.75f}));
    prim::append(bus_glass, prim::box({0.08f, 2.2f, 1.2f}, {0.08f, 0.1f, 0.13f}), at({4.0f, 0, 0.55f}));
    if (!make(r, bus_glass_, bus_glass, mat(0.08f, 0.2f), false)) return false;
  }
  // ---- jet (+X right wing, +Y nose, +Z up)
  {
    const Vec3 grey{0.46f, 0.49f, 0.52f};
    MeshData body;
    prim::append(body, hull({{-.8f, -7.2f, -.8f}, {.8f, -7.2f, -.8f}, {.8f, -7.2f, .7f}, {-.8f, -7.2f, .7f}, {-.9f, 4.5f, -.75f}, {.9f, 4.5f, -.75f},
                             {.75f, 4.5f, .65f}, {-.75f, 4.5f, .65f}, {-.12f, 9, -.1f}, {.12f, 9, -.1f}, {.1f, 9, .1f}, {-.1f, 9, .1f}},
                            grey),
                 Mat4{});
    prim::append(body, hull({{-6, -3.4f, -.15f}, {-6, -4.3f, -.15f}, {-.6f, 1.8f, -.2f}, {-.6f, -4.2f, -.2f}, {6, -3.4f, -.15f}, {6, -4.3f, -.15f},
                             {.6f, 1.8f, -.2f}, {.6f, -4.2f, -.2f}, {-6, -3.4f, -.05f}, {-6, -4.3f, -.05f}, {-.6f, 1.8f, .05f}, {-.6f, -4.2f, .05f},
                             {6, -3.4f, -.05f}, {6, -4.3f, -.05f}, {.6f, 1.8f, .05f}, {.6f, -4.2f, .05f}},
                            grey),
                 Mat4{});
    // twin fins and tailplanes
    for (f32 s : {-1.0f, 1.0f}) {
      prim::append(body, hull({{s * .7f, -7.2f, .5f}, {s * .7f, -4.6f, .5f}, {s * 1.4f, -7.4f, 3.0f}, {s * 1.35f, -6.5f, 3.0f}, {s * .78f, -7.2f, .5f}, {s * .78f, -4.6f, .5f},
                               {s * 1.48f, -7.4f, 3.0f}, {s * 1.43f, -6.5f, 3.0f}},
                              grey),
                   Mat4{});
      prim::append(body, hull({{s * .6f, -7.5f, -.1f}, {s * .6f, -5.8f, -.1f}, {s * 3.4f, -7.9f, -.15f}, {s * 3.4f, -7.0f, -.15f}, {s * .6f, -7.5f, .0f},
                               {s * .6f, -5.8f, .0f}, {s * 3.4f, -7.9f, -.05f}, {s * 3.4f, -7.0f, -.05f}},
                              grey),
                   Mat4{});
    }
    prim::append(body, prim::cylinder(0.62f, 0.9f, 16, {0.18f, 0.18f, 0.2f}), at({0, -7.0f, 0}) * rot({1, 0, 0}, kPi / 2));
    if (!make(r, jet_body_, body, mat(0.45f, 0.4f), false)) return false;
    MeshData canopy = prim::sphere(1.0f, 18, 10, {0.15f, 0.2f, 0.12f});
    if (!make(r, jet_glass_, canopy, mat(0.06f, 0.5f), false)) return false;
    MeshData gear;
    prim::append(gear, prim::cylinder(0.08f, 1.45f, 8, {0.7f, 0.7f, 0.72f}), Mat4{});
    prim::append(gear, prim::cylinder(0.38f, 0.28f, 14, {0.07f, 0.07f, 0.08f}), at({0.14f, 0, 0}) * rot({0, 1, 0}, kPi / 2));
    if (!make(r, jet_gear_, gear, mat(0.6f, 0.3f), false)) return false;
    kern::Material flame = mat(1, 0, {6.0f, 2.6f, 0.9f});
    flame.unlit = true;
    flame.cast_shadow = false;
    flame.alpha = kern::AlphaMode::blend;
    flame.color = {1, 0.75f, 0.45f, 0.55f};
    if (!make(r, jet_flame_, prim::cone(0.5f, 1.0f, 14, {1, 0.7f, 0.4f}), flame, false)) return false;
  }
  // ---- drone (-X nose)
  {
    MeshData frame;
    prim::append(frame, prim::box({0.11f, 0.06f, 0.03f}, {0.1f, 0.1f, 0.11f}), Mat4{});
    prim::append(frame, prim::box({0.045f, 0.035f, 0.035f}, {0.95f, 0.45f, 0.1f}), at({-0.07f, 0, 0.028f}));  // FPV camera
    prim::append(frame, prim::box({0.1f, 0.035f, 0.03f}, {0.2f, 0.25f, 0.6f}), at({0.015f, 0, -0.03f}));      // battery
    const f32 a = 0.225f / (2 * std::sqrt(2.0f));
    for (Vec2 m : {Vec2{-a, -a}, Vec2{-a, a}, Vec2{a, a}, Vec2{a, -a}}) {
      const f32 len = std::sqrt(m.x * m.x + m.y * m.y);
      prim::append(frame, prim::box({len, 0.016f, 0.008f}, {0.12f, 0.12f, 0.13f}), at({m.x / 2, m.y / 2, 0}) * rot({0, 0, 1}, std::atan2(m.y, m.x)));
    }
    if (!make(r, drone_frame_, frame, mat(0.5f, 0.2f), false)) return false;
    if (!make(r, drone_motor_, prim::cylinder(0.014f, 0.02f, 12, {0.35f, 0.35f, 0.38f}), mat(0.3f, 0.8f), false)) return false;
    kern::Material prop = mat(0.4f);
    prop.alpha = kern::AlphaMode::blend;
    prop.color = {1, 1, 1, 0.72f};
    prop.cast_shadow = false;
    if (!make(r, drone_prop_a_, prim::box({1.0f, 0.012f, 0.002f}, {1.0f, 0.64f, 0.3f}), prop, false)) return false;
    if (!make(r, drone_prop_b_, prim::box({1.0f, 0.012f, 0.002f}, {0.3f, 0.84f, 1.0f}), prop, false)) return false;
    kern::Material led = mat(0.4f, 0, {0.2f, 2.5f, 0.6f});
    led.unlit = true;
    if (!make(r, drone_led_, prim::box({0.012f, 0.012f, 0.006f}, white), led, true)) return false;
  }
  // ---- parachute
  {
    MeshData canopy = prim::sphere(1.0f, 20, 8, {0.95f, 0.45f, 0.1f});
    kern::Material m = mat(0.8f);
    m.double_sided = true;
    if (!make(r, chute_, canopy, m, false)) return false;
    if (!make(r, pilot_, prim::capsule(0.28f, 1.1f, 10, 6, {0.3f, 0.36f, 0.28f}), mat(0.8f), false)) return false;
  }
  return true;
}

void VehicleArt::begin() {
  for (Part* p : {&car_body_, &car_glass_, &car_lights_, &car_tail_, &wheel_, &bus_body_, &bus_glass_, &jet_body_, &jet_glass_, &jet_gear_, &jet_flame_,
                  &drone_frame_, &drone_motor_, &drone_prop_a_, &drone_prop_b_, &drone_led_, &chute_, &pilot_}) {
    p->transforms.clear();
    p->tints.clear();
  }
}

void VehicleArt::put(Part& p, const Mat4& m, Vec3 tint) {
  p.transforms.insert(p.transforms.end(), m.m, m.m + 16);
  if (p.tinted) p.tints.insert(p.tints.end(), {tint.x, tint.y, tint.z, 1});
}

void VehicleArt::car(const Mat4& chassis, const Car& c, Vec3 color, bool cockpit_view, bool brake_lights) {
  const bool bus = c.spec().radius > 0.4f;
  if (bus) {
    put(bus_body_, chassis, color);
    put(bus_glass_, chassis);
  } else {
    if (!cockpit_view) put(car_body_, chassis, color);
    put(car_glass_, chassis);
    put(car_lights_, chassis);
    put(car_tail_, chassis, brake_lights ? Vec3{3, 1, 1} : Vec3{1, 1, 1});
  }
  if (cockpit_view && !bus) return;
  const f32 r = c.spec().radius, w = c.spec().wheel_width;
  for (int i = 0; i < 4; ++i) {
    const WheelState& ws = c.wheel(i);
    // On its axle in the (interpolated) chassis frame: the suspension travel of the last step.
    const CarSpec& s = c.spec();
    const f32 travel = clampf(ws.travel, s.susp_lower, s.susp_upper);
    const f32 steer = s.wheels[i].front ? c.steer_angle() : 0;
    const Mat4 m = chassis * at({s.wheels[i].x, s.wheels[i].y, s.attach_z + travel}) * rot({0, 0, 1}, steer) * rot({0, 1, 0}, ws.angle) * Mat4::scale({r, w / 0.62f, r});
    put(wheel_, m);
  }
}

void VehicleArt::jet(const Mat4& body, bool gear_down, f32 nozzle, f32 thrust01, bool cockpit_view) {
  if (!cockpit_view) put(jet_body_, body);
  put(jet_glass_, body * at({0, 3.2f, 0.75f}) * Mat4::scale({0.62f, 1.7f, 0.55f}));
  if (gear_down) {
    const JetSpec s;
    for (const auto& g : s.gear) put(jet_gear_, body * at(g.p + Vec3{0, 0, -1.45f}));
  }
  if (thrust01 > 0.02f) {
    // Nozzle direction: aft (0) .. down (pi/2) in the body's Y-Z plane; the cone's +Z points out.
    const Mat4 n = body * at({0, -7.3f, 0}) * rot({1, 0, 0}, kPi / 2 + nozzle) * Mat4::scale({1, 1, 1.0f + 4.0f * thrust01});
    put(jet_flame_, n);
  }
}

void VehicleArt::drone(const Mat4& body, const Vec3* motors, const f32* rotor_angles, f32 prop_d, bool armed, bool fpv_view) {
  if (fpv_view) return;
  put(drone_frame_, body);
  for (int i = 0; i < 4; ++i) {
    put(drone_motor_, body * at(motors[i] + Vec3{0, 0, -0.004f}));
    const Mat4 p = body * at(motors[i] + Vec3{0, 0, 0.018f}) * rot({0, 0, 1}, rotor_angles[i]) * Mat4::scale({prop_d, 1, 1});
    put(i % 2 ? drone_prop_b_ : drone_prop_a_, p);
  }
  put(drone_led_, body * at({0.06f, 0, 0.018f}), armed ? Vec3{4, 0.2f, 0.1f} : Vec3{0.3f, 0.3f, 0.3f});
}

void VehicleArt::parachute(Vec3 pilot, f32 heading, bool open) {
  const Mat4 base = at(pilot) * rot({0, 0, 1}, heading);
  put(pilot_, base * at({0, 0, 0}));
  if (open) put(chute_, base * at({0, 0, 5.2f}) * Mat4::scale({3.6f, 2.6f, 1.1f}));
}

void VehicleArt::end() {
  for (Part* p : {&car_body_, &car_glass_, &car_lights_, &car_tail_, &wheel_, &bus_body_, &bus_glass_, &jet_body_, &jet_glass_, &jet_gear_, &jet_flame_,
                  &drone_frame_, &drone_motor_, &drone_prop_a_, &drone_prop_b_, &drone_led_, &chute_, &pilot_}) {
    if (!p->id) continue;
    if (p->transforms.empty()) { r_->set_visible(p->id, false); continue; }
    r_->update_instances(p->id, kern::Span<const f32>(p->transforms.data(), p->transforms.size()),
                         kern::Span<const f32>(p->tints.data(), p->tints.size()));
    r_->set_visible(p->id, true);
  }
}

}  // namespace sc
