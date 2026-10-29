// Population and traffic: the interface the next milestone fills in (SpawnCity
// world_procedural_population.mjs, pedestrian_agents.mjs, tow_service.mjs).
//
// Milestone 1 already drives real traffic: cars on Box3D with SpawnCity's vehicle model, each
// with a driver (ai_drive in car.h) following the streamed street lanes; carjacking takes one
// over. What is defined here and not built yet: pedestrians with home streets and destinations,
// panic and witnesses, drivers that leave the car, animals.
#pragma once

#include "common.h"

namespace sc::population {

/// Desktop / mobile budgets (world_procedural_population.mjs:20-41).
struct Budget {
  u32 cars = 42, people = 100, buses = 8, birds = 24, pets = 20;
  static Budget mobile() { return {28, 68, 5, 16, 12}; }
};
/// Visibility rules: people appear at 170 m and fade out at 190 m; cars run physics within
/// 150 m and analytically beyond 180 m; nothing appears or vanishes inside the player's view.
constexpr f32 kPeopleShow = 170, kPeopleHide = 190, kCarPhysics = 150, kCarAnalytic = 180;
/// Drivers keep 1.6 s + 6 m to what is ahead; parked cars stay 180 s.
constexpr f32 kFollowTime = 1.6f, kFollowGap = 6, kParkedSeconds = 180;

enum class Temper : u8 { calm, panicky, aggressive, hothead };

/// A person of the town (pedestrian, driver or passenger).
struct Person {
  u32 id = 0;
  Vec3 position;
  f32 heading = 0;
  f32 fitness = 1;            // 0.75 .. 1.25 (panic sprint 6..8.5 m/s scaled)
  Temper temper = Temper::calm;
  u32 home_lane = 0;          // the street they live on
  bool driving = false;
  u32 vehicle = 0;            // traffic car id while driving
};

/// What the game tells the population (and what witnesses report to wanted::Tracker).
struct Events {
  virtual ~Events() = default;
  virtual void shots_fired(Vec3 at, f32 loudness) = 0;                 // flee within 10 m, 10..22 m some
  virtual void vehicle_hit_person(u32 person, f32 speed) = 0;
  virtual void carjacked(u32 vehicle, u32 driver, Vec3 at) = 0;       // the driver is thrown out, a witness gives a star
  virtual void low_flyover(Vec3 at, f32 height) = 0;                  // drones / jets below 14 m scatter people
};

}  // namespace sc::population
