// Procedural vehicle models (SpawnCity draws every model from code too: vehicle_models.mjs,
// simulator.mjs airframe, jet_mode.mjs) as instanced Kern meshes: one mesh per part, one instance
// per vehicle, poses from the interpolated Box3D bodies every frame.
#pragma once

#include <vector>

#include "car.h"
#include "kern/gfx/renderer.h"

namespace sc {

class VehicleArt {
 public:
  bool init(kern::Renderer& r);
  /// Starts a frame: forget last frame's instances.
  void begin();
  /// A car or bus: chassis pose, body colour, wheels (hub world positions from the suspension).
  void car(const kern::Mat4& chassis, const Car& c, Vec3 color, bool cockpit_view, bool brake_lights);
  /// The jet (body frame +Y nose) with gear when down.
  void jet(const kern::Mat4& body, bool gear_down, f32 nozzle, f32 thrust01, bool cockpit_view);
  /// The quad (body frame -X nose); rotor angles per motor.
  void drone(const kern::Mat4& body, const Vec3* motors, const f32* rotor_angles, f32 prop_d, bool armed, bool fpv_view);
  /// A pilot under the canopy (open: the chute is out).
  void parachute(Vec3 pilot, f32 heading, bool open);
  /// Uploads all instance lists.
  void end();

 private:
  struct Part {
    u32 id = 0;
    std::vector<f32> transforms, tints;
    bool tinted = false;
  };
  bool make(kern::Renderer& r, Part& p, const kern::MeshData& mesh, const kern::Material& m, bool tinted);
  void put(Part& p, const kern::Mat4& m, Vec3 tint = {1, 1, 1});

  kern::Renderer* r_ = nullptr;
  Part car_body_, car_glass_, car_lights_, car_tail_, wheel_, bus_body_, bus_glass_;
  Part jet_body_, jet_glass_, jet_gear_, jet_flame_;
  Part drone_frame_, drone_motor_, drone_prop_a_, drone_prop_b_, drone_led_;
  Part chute_, chute_lines_, pilot_;
};

}  // namespace sc
