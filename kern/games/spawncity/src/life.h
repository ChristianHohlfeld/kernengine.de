// Street life and animals (SpawnCity cows.mjs, pigeons.mjs, animal_audio.mjs, jogger_chain.mjs,
// world_liveliness.mjs, street_voices.mjs): cows on the meadows, pigeon flocks on the pavement,
// birds circling over the roofs, trees along the streets, the running club and what the people
// on the street say. Pure simulation (no GameWorld): the game feeds it the world each frame and
// carries out what it asks for (sounds, speech, impulses on cars, ragdolls, particles).
#pragma once

#include <string>
#include <vector>

#include "city_stream.h"
#include "common.h"
#include "ragdoll.h"

namespace sc {

/// A vehicle as street life sees it.
struct LifeCar {
  Vec3 position, velocity;
  Quat rotation;
  Vec2 half;        // footprint half extents (body X forward)
  f32 mass = 1400;
  i32 car = -1;     // the game's traffic car index (-1: other vehicle)
  bool player = false, airborne = false, stuck = false, driver = true;
};

struct LifeView {
  const kern::physics::World* physics = nullptr;
  const CityStream* city = nullptr;
  Vec3 focus, player;   // player: his feet (or vehicle)
  Vec3 eye;             // the camera (hearing distance)
  f32 player_speed = 0;
  bool on_foot = true, people_on = true, birds_on = true;  // WORLD options (world.people, world.birds)
  std::vector<LifeCar> cars;
};

/// What the game should do for street life this frame.
struct LifeOut {
  enum class Sound : u8 { moo, moo_pain, coo, flap, squawk, thud, stomp, stomp_accent, jingle, horn, horn_bus, scream, scream_f };
  struct Play { Sound sound; Vec3 at; f32 volume, pitch; };
  struct Say { std::string text; f32 pitch = 1, rate = 1, volume = 1, delay = 0; };  // browser speech (de-DE)
  struct Bubble { Vec3 at; std::string text; std::string mood; f32 until; };
  struct Impulse { i32 car; Vec3 impulse; };
  struct Burst { Vec3 at; Vec3 color; };
  std::vector<Play> sounds;
  std::vector<Say> speech;
  std::vector<Impulse> impulses;
  std::vector<Burst> bursts;
  std::string banner;   // big call-out (jogger streak)
  f32 banner_for = 0;
  void clear() { sounds.clear(); speech.clear(); impulses.clear(); bursts.clear(); banner.clear(); banner_for = 0; }
};

class Life {
 public:
  struct Cow {
    u64 key = 0;
    i32 meadow = -1;
    Vec2 home, off;
    Vec3 pos;
    f32 yaw = 0, hp = 100, phase = 0, next_moo = 0, flee_until = 0, flinch_at = -9, last_hit = -9;
    Vec2 flee_v;
    Vec3 color;
    Quat rot;  // the dead body's orientation
    bool dead = false, walking = false;
    kern::physics::Body body;  // the dead cow: a 600 kg Box3D body
  };
  struct Pigeon { Vec2 o, v; Vec3 pos; f32 yaw = 0, walk_until = 0, peck = 0, flap = 0, lag = 0; };
  struct Flock {
    bool air = false;
    Vec2 at;
    f32 next_flight = 0, t = 0, coo_at = 0;
    // flight: loops round a centre, then a glide onto the destination
    Vec2 c, from, dest;
    f32 r = 12, h = 12, dir = 1, a0 = 0, dur = 10;
    std::vector<Pigeon> birds;
  };
  struct Bird { u32 seed = 0; Vec3 pos; f32 yaw = 0, flap = 1, dead_until = -1, dying_until = -1, spin = 0; Vec3 fall_v; Vec2 center; };
  struct Runner { Vec3 pos; f32 yaw = 0, yaw_s = 0, lat = 0, phase = 0; bool alive = true; };
  struct Chain {
    u32 id = 0;
    std::vector<Vec2> path;  // the pavement line
    std::vector<f32> cum;
    f32 len = 0, s = 0, born = 0, next_call = 0, next_beat = 0, off = 4;
    i32 call = 0, beat = 0, side = 1;
    Runner runners[10];
  };
  struct Tree { Vec3 foot; f32 scale = 1; u32 lane = 0; };
  struct Voice { Vec3 at; std::string text, mood; f32 until; };

  void init(kern::physics::World& w, bool mobile);
  void clear();
  void update(f32 dt, f32 now, const LifeView& view, Rng& rng, LifeOut& out);

  /// A bullet from `from` to `to`: cows, birds and joggers. Returns true when something was hit
  /// (`to` shortened to the hit).
  bool shot(Vec3 from, Vec3& to, f32 damage, f32 now, LifeOut& out);
  /// A shot was fired / passed (scares cows and pigeons).
  void heard_shot(Vec3 from, Vec3 to, f32 now);
  void blast(Vec3 at, f32 radius, f32 now, LifeOut& out);
  /// Street voices (street_voices.mjs): a short spoken line with a bubble, rate-limited.
  bool shout(Vec3 at, const char* mood, u32 seed, f32 now, f32 hear_distance, LifeOut& out);
  bool honk(Vec3 at, bool bus, u32 source, f32 now, f32 hear_distance, LifeOut& out);
  bool scream(Vec3 at, u32 seed, f32 now, f32 hear_distance, LifeOut& out);

  // tests / commands
  bool spawn_herd(Vec2 center, f32 radius, u32 n, f32 now);
  bool spawn_joggers(const LifeView& view, f32 now, Rng& rng);
  void spook_pigeons(f32 now);
  bool spawn_flock(Vec2 at, f32 now, Rng& rng, const LifeView& view);

  Ragdolls& ragdolls() { return ragdolls_; }
  const Ragdolls& ragdolls() const { return ragdolls_; }
  const std::vector<Cow>& cows() const { return cows_; }
  const std::vector<Flock>& flocks() const { return flocks_; }
  const std::vector<Bird>& birds() const { return birds_; }
  const std::vector<Tree>& trees() const { return trees_; }
  const Chain* chain() const { return chain_alive_ ? &chain_ : nullptr; }
  const std::vector<Voice>& voices() const { return voices_; }
  u32 cow_hits() const { return cow_hits_; }
  u32 bird_hits() const { return bird_hits_; }
  u32 jogger_streak() const { return streak_n_; }
  u32 shouts() const { return shouts_; }
  u32 honks() const { return honks_; }

  /// Pure helpers (tests).
  static bool in_rings(const std::vector<std::vector<Vec2>>& rings, Vec2 p);
  static f32 ring_area(const std::vector<Vec2>& r);

 private:
  struct Meadow { std::vector<std::vector<Vec2>> rings; Vec2 lo, hi, centre; f32 area = 0; u64 key = 0; bool alive = true; };
  void rebuild_herds(const LifeView& view, f32 now);
  Vec2 graze_spot(const Cow& c, i64 k) const;
  void kill_cow(Cow& c, Vec3 impulse, f32 now, LifeOut& out);
  void scare_cow(Cow& c, Vec2 from, f32 now, f32 secs);
  void update_cows(f32 dt, f32 now, const LifeView& view, LifeOut& out);
  void update_pigeons(f32 dt, f32 now, const LifeView& view, Rng& rng, LifeOut& out);
  void spook(Flock& f, Vec2 from, f32 now, Rng& rng, const LifeView& view, LifeOut& out);
  bool blocked(const LifeView& view, Vec2 p) const;
  bool spot_near(const LifeView& view, Vec2 c, f32 r0, f32 r1, Rng& rng, Vec2& out) const;
  void update_birds(f32 dt, f32 now, const LifeView& view);
  void update_trees(const LifeView& view);
  void update_joggers(f32 dt, f32 now, const LifeView& view, Rng& rng, LifeOut& out);
  void kill_runner(i32 i, Vec3 velocity, f32 now, LifeOut& out);
  Vec3 sample(f32 s, f32& yaw, Vec2& normal) const;
  void update_voices(f32 dt, f32 now, const LifeView& view, LifeOut& out);

  kern::physics::World* world_ = nullptr;
  bool mobile_ = false;
  Ragdolls ragdolls_;
  std::vector<Meadow> meadows_;
  std::vector<Cow> cows_;
  std::vector<u64> dead_cows_;
  Vec2 herds_for_{1e9f, 1e9f};
  f32 last_herd_check_ = -99, last_moo_ = -99;
  std::vector<Flock> flocks_;
  std::vector<Bird> birds_;
  std::vector<Tree> trees_;
  std::size_t tree_lanes_ = 0;
  Chain chain_;
  bool chain_alive_ = false;
  f32 next_chain_ = -1, streak_at_ = -99;
  u32 chain_serial_ = 0, streak_n_ = 0, streak_chain_ = 0;
  std::vector<Voice> voices_;
  f32 last_shout_ = -99, last_honk_ = -99, last_scream_ = -99, last_coo_ = -99, ambient_chirp_ = 0;
  std::vector<std::pair<u32, f32>> source_gap_;
  u32 cow_hits_ = 0, bird_hits_ = 0, shouts_ = 0, honks_ = 0;
};

}  // namespace sc
