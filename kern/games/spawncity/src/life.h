// Street life and animals (SpawnCity cows.mjs, pigeons.mjs, animal_audio.mjs, jogger_chain.mjs,
// world_liveliness.mjs, street_voices.mjs): cows on the meadows, pigeon flocks on the pavement,
// birds circling over the roofs, trees along the streets, the running club and what the people
// on the street say. Pure simulation (no GameWorld): the game feeds it the world each frame and
// carries out what it asks for (sounds, speech, impulses on cars, ragdolls, particles).
#pragma once

#include <string>
#include <unordered_set>
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

/// The world as street life sees it this frame: values. What it reads of the town itself - the
/// streamed city, where the ground changed - the functions are given by reference.
struct LifeView {
  const kern::physics::World* physics = nullptr;
  Vec3 focus, player;   // player: his feet (or vehicle)
  Vec3 eye;             // the camera (hearing distance)
  Vec3 head;            // the player's head (the black cat leaps at it)
  f64 epoch = 0;        // wall clock in seconds (SpawnCity's Date.now(): every machine the same)
  bool dogs_on = true, cats_on = true;
  f32 player_speed = 0;
  bool on_foot = true, people_on = true, birds_on = true;  // WORLD options (world.people, world.birds)
  std::vector<LifeCar> cars;
  std::vector<Vec3> mates;  // MP host: where the other players are (herds and flocks there too: one world)
};

/// What the game should do for street life this frame.
struct LifeOut {
  enum class Sound : u8 { moo, moo_pain, coo, flap, squawk, thud, stomp, stomp_accent, jingle, horn, horn_bus, scream, scream_f, dog, cat, hiss, yowl, screech, kCount };
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
  bool kill_player = false;  // the black cat got him
  struct CowDeath { Vec3 at, impulse; };
  std::vector<CowDeath> cow_deaths;  // for the other players (cows.mjs sendDeath)
  void clear() { sounds.clear(); speech.clear(); impulses.clear(); bursts.clear(); banner.clear(); banner_for = 0; kill_player = false; cow_deaths.clear(); }
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
    bool dead = false, walking = false, carried = false;  // carried: picked up / thrown alive (a VR hand)
    f32 carried_still = 0;
    kern::physics::Body body;  // the dead cow, or the carried one: a 600 kg Box3D body
    Vec3 net_to, net_v;        // MP client: where the host has it, and its pace there
    f32 net_at = 0;
    Quat net_rot;
    bool net_dead = false;
  };
  struct Pigeon { Vec2 o, v; Vec3 pos; f32 yaw = 0, walk_until = 0, peck = 0, flap = 0, lag = 0, dead_at = -1; Vec3 fall_v; Vec3 net_to, net_v; f32 net_yaw = 0, net_at = 0; };
  struct Flock {
    u32 id = 0;  // its own for good (the host's: a client knows a flock by it)
    bool air = false;
    Vec2 at;
    f32 next_flight = 0, t = 0, coo_at = 0;
    // flight: loops round a centre, then a glide onto the destination
    Vec2 c, from, dest;
    f32 r = 12, h = 12, dir = 1, a0 = 0, dur = 10;
    std::vector<Pigeon> birds;
  };
  struct Bird { u32 seed = 0; Vec3 pos; f32 yaw = 0, flap = 1, dead_until = -1, dying_until = -1, spin = 0; Vec3 fall_v; Vec2 center; bool down = false; };
  struct Runner { Vec3 pos; f32 yaw = 0, yaw_s = 0, lat = 0, phase = 0; bool alive = true; Vec3 net_to, net_v; f32 net_at = 0; };
  struct Chain {
    u32 id = 0;
    std::vector<Vec2> path;  // the pavement line
    std::vector<f32> cum;
    f32 len = 0, s = 0, born = 0, next_call = 0, next_beat = 0, off = 4;
    i32 call = 0, beat = 0, side = 1;
    Runner runners[10];
  };
  struct Tree { Vec3 foot; f32 scale = 1; u32 lane = 0; f32 fell_at = -1; Vec2 fall_dir{1, 0}; u64 tile = 0; u32 index = 0; };  // fell_at: blown over (falls about its foot); tile / index: which of the map tile's trees
  // dogs and cats (world_procedural_population.mjs ambient animals; #1 is the black cat now and then)
  enum class Species : u8 { dog, cat, black_cat };
  struct Pet {
    f32 radius = 20, omega = 0.05f, phase = 0, wobble_omega = 0.2f, wobble_r = 2, scale = 1;
    Vec3 pos;
    f32 yaw = 0, gait = 0, speed = 0, dead_at = -1;
    bool shown = false, has_pose = false, carried = false;  // carried: picked up / thrown alive (a VR hand)
    f32 carried_still = 0;
    Vec2 off;  // where a throw left it, off its round (back to it over some seconds)
    i32 punches = 0;
    Vec3 color;
    kern::physics::Body body;  // the dead animal
    Quat rot;
    Quat net_rot;  // MP client: the host's carried / dead pose
  };
  struct CatAttack { bool on = false; i32 phase = 0; Vec3 p, from; f32 yaw = 0, start = 0, leap_at = 0, last_cry = -9, away_yaw = 0, leave_at = 0; i32 cries = 0; };
  struct Voice { Vec3 at; std::string text, mood; f32 until; };

  void init(kern::physics::World& w, bool mobile);
  void clear();
  void update(f32 dt, f32 now, const LifeView& view, const CityStream& city, const std::vector<kern::Vec4>& ground_changes, Rng& rng, LifeOut& out);

  /// A bullet from `from` to `to`: cows, birds and joggers. Returns true when something was hit
  /// (`to` shortened to the hit).
  bool shot(Vec3 from, Vec3& to, f32 damage, f32 now, LifeOut& out, bool melee = false);
  /// Another player's cow died (cows.mjs onFx): the cow there falls the same way.
  void remote_cow_death(Vec3 at, Vec3 impulse, f32 now, LifeOut& out);
  /// A shot was fired / passed (scares cows and pigeons).
  void heard_shot(Vec3 from, Vec3 to, f32 now);
  void blast(Vec3 at, f32 radius, f32 now, LifeOut& out);
  /// Street voices (street_voices.mjs): a short spoken line with a bubble, rate-limited.
  bool shout(Vec3 at, const char* mood, u32 seed, f32 now, f32 hear_distance, LifeOut& out);
  bool honk(Vec3 at, bool bus, u32 source, f32 now, f32 hear_distance, LifeOut& out);
  bool scream(Vec3 at, u32 seed, f32 now, f32 hear_distance, LifeOut& out);

  // tests / commands
  bool spawn_herd(Vec2 center, f32 radius, u32 n, f32 now);
  bool spawn_joggers(const LifeView& view, const CityStream& city, f32 now, Rng& rng);
  void spook_pigeons(f32 now);
  bool spawn_flock(Vec2 at, f32 now, Rng& rng, const LifeView& view, const CityStream& city);

  Ragdolls& ragdolls() { return ragdolls_; }
  const Ragdolls& ragdolls() const { return ragdolls_; }
  const std::vector<Cow>& cows() const { return cows_; }
  const std::vector<Flock>& flocks() const { return flocks_; }
  u32 trees_down() const { return trees_down_; }
  const std::vector<Bird>& birds() const { return birds_; }
  const std::vector<Tree>& trees() const { return trees_; }
  const std::vector<Pet>& pets() const { return pets_; }
  Species species(u32 i, f64 epoch) const;
  const CatAttack& cat_attack() const { return cat_; }
  f64 epoch() const { return epoch_; }
  bool provoke_cat(f32 now);  // tests: the black cat charges
  const Chain* chain() const { return chain_alive_ ? &chain_ : nullptr; }
  /// MP client: the running club is the host's (game_worldsync.cpp) - no own chain, his runners
  /// eased to where his snapshots have them; none heard for 1.5 s: gone.
  void set_remote_joggers(bool on) { if (on != remote_joggers_) { remote_joggers_ = on; chain_alive_ = false; } }
  void apply_joggers(u32 id, const Vec3* pos, const f32* yaw, const bool* alive, u32 n, f32 now, f32 at);
  /// MP client: the animals are the host's too (game_worldsync.cpp): no herds, pets or flocks of
  /// our own; his cows (by key - the same on every machine), dogs and cats (by his index) and
  /// pigeon flocks (by his index) eased to where his snapshots have them. An animal that is a
  /// body here (in our hand, thrown by us, killed here) is ours until it stands or lies again.
  struct NetCow { u64 key = 0; Vec3 pos; Quat rot; f32 yaw = 0; bool dead = false, walking = false, carried = false; };
  struct NetPet { u8 idx = 0; Vec3 pos; Quat rot; f32 yaw = 0, speed = 0; Vec2 off; bool shown = false, dead = false, carried = false; };
  struct NetFlock { u32 id = 0; bool air = false; Vec2 at; std::vector<Vec3> birds; std::vector<f32> yaws; std::vector<u8> dead; };
  /// Which animal a body is (a carried / thrown one): 1 a cow (id: its key), 2 a dog or cat (id:
  /// its index); 0 none.
  u8 animal_of(kern::physics::Body b, u64& id) const {
    for (const Cow& c : cows_) if (c.body == b && b.valid()) { id = c.key; return 1; }
    for (std::size_t i = 0; i < pets_.size(); ++i) if (pets_[i].body == b && b.valid()) { id = i; return 2; }
    return 0;
  }
  kern::physics::Body cow_body(u64 key) const { for (const Cow& c : cows_) if (c.key == key) return c.body; return {}; }
  kern::physics::Body pet_body(u32 i) const { return i < pets_.size() ? pets_[i].body : kern::physics::Body{}; }
  void set_remote_animals(bool on);
  bool remote_animals() const { return remote_animals_; }
  /// The host's animals as he has them around us (game_worldsync.cpp), each list whole: what a
  /// list does not name is not there. `at`: when the list was taken, on our clock.
  void apply_cows(const std::vector<NetCow>& cows, f32 now, f32 at);
  void apply_pets(const std::vector<NetPet>& pets, u32 pet_count, const std::vector<Vec2>& pet_anchors, f32 now);
  void apply_flocks(const std::vector<NetFlock>& flocks, f32 now, f32 at);
  /// The neighbourhoods of the dogs and cats: the player's first, then (host) one round every mate
  /// far from it; pet i belongs to anchor i / base.
  const std::vector<Vec2>& pet_anchors() const { return pet_anchors_; }
  Vec2 anchor_of(u32 i) const { const u32 k = pet_base_ ? i / pet_base_ : 0; return k > 0 && k < pet_anchors_.size() ? pet_anchors_[k] : pet_anchor_; }
  /// Thrown things (game_toss.cpp): the body of an animal another player throws, made here so
  /// it flies here too (the host's cow by key / pet by index); invalid when there is none.
  kern::physics::Body net_carry_cow(u64 key);
  kern::physics::Body net_carry_pet(u32 index);
  /// A pigeon in another player's hand / let go by him (`key` his and the bird's): drawn here.
  void net_bird(u64 key, Vec3 at, f32 yaw, Vec3 vel, bool held);
  /// A dog or cat killed on a client: the host's dies too (and every snapshot says so).
  void net_kill_pet(u32 index, Vec3 dir, f32 now, LifeOut& out) { if (index < pets_.size() && pets_[index].dead_at < 0) kill_pet(index, dir, now, out); }
  const std::vector<Voice>& voices() const { return voices_; }
  u32 cow_hits() const { return cow_hits_; }
  u32 bird_hits() const { return bird_hits_; }
  u32 jogger_streak() const { return streak_n_; }
  u32 shouts() const { return shouts_; }
  u32 honks() const { return honks_; }

  /// Pure helpers (tests).
  static bool in_rings(const std::vector<std::vector<Vec2>>& rings, Vec2 p);
  static f32 ring_area(const std::vector<Vec2>& r);

  /// A hand grabs the nearest live animal within `reach` of `at` (a dog, a cat, a cow): it becomes a
  /// Box3D body for as long as it is held and flies; it gets up where it lands. The body, or none.
  kern::physics::Body grab_animal(Vec3 at, f32 reach);
  /// A pigeon caught by hand (VR): crept up on, the hand slow and right on it - and even then
  /// only every other try, a miss sends the flock up. Returns its id (-1: it got away).
  i32 catch_pigeon(Vec3 hand, f32 hand_speed, f32 now, Rng& rng);
  /// The animal with this body is in a hand this frame: it does not stand up there (held still).
  void hold_animal(kern::physics::Body b) {
    for (Cow& c : cows_) if (c.carried && c.body == b) c.carried_still = 0;
    for (Pet& p : pets_) if (p.carried && p.body == b) p.carried_still = 0;
  }
  void hold_pigeon(i32 id, Vec3 at, f32 yaw);
  /// Let go: it flies off from there, with the hand's throw.
  void release_pigeon(i32 id, Vec3 velocity);
  struct LooseBird { i32 id = 0; Vec3 pos, vel; f32 yaw = 0, flap = 0, t = 0; bool held = true; u64 net = 0; };
  const std::vector<LooseBird>& loose_birds() const { return loose_; }
 private:
  std::vector<LooseBird> loose_;
  i32 loose_serial_ = 0;
  bool remote_animals_ = false;
  std::vector<Vec2> pet_anchors_;
  u32 pet_base_ = 20;
  f32 remote_animals_at_ = -9;
  std::vector<Vec3> pet_net_to_;
  std::vector<f32> pet_net_yaw_;
  std::vector<u64> loose_keys_;  // net_bird: whose loose bird (0: ours)
  bool remote_joggers_ = false;
  f32 remote_joggers_at_ = -9;
  struct Meadow { std::vector<std::vector<Vec2>> rings; Vec2 lo, hi, centre; f32 area = 0; u64 key = 0; bool alive = true; };
  void rebuild_herds(const LifeView& view, const CityStream& city, f32 now);
  Vec2 graze_spot(const Cow& c, i64 k) const;
  void kill_cow(Cow& c, Vec3 impulse, f32 now, LifeOut& out);
  void scare_cow(Cow& c, Vec2 from, f32 now, f32 secs);
  void update_cows(f32 dt, f32 now, const LifeView& view, const CityStream& city, LifeOut& out);
  void update_pigeons(f32 dt, f32 now, const LifeView& view, const CityStream& city, Rng& rng, LifeOut& out);
  void spook(Flock& f, Vec2 from, f32 now, Rng& rng, const LifeView& view, const CityStream& city, LifeOut& out);
  bool blocked(const LifeView& view, const CityStream& city, Vec2 p) const;
  bool spot_near(const LifeView& view, const CityStream& city, Vec2 c, f32 r0, f32 r1, Rng& rng, Vec2& out) const;
  void update_birds(f32 dt, f32 now, const LifeView& view, const CityStream& city, Rng& rng, LifeOut& out);
  void update_trees(const LifeView& view, const CityStream& city, const std::vector<kern::Vec4>& ground_changes, f32 now);
  void update_pets(f32 dt, f32 now, const LifeView& view, const CityStream& city, LifeOut& out);
  void kill_pet(u32 i, Vec3 dir, f32 now, LifeOut& out);
  static void pet_params(Pet& p, u32 i);
  void update_joggers(f32 dt, f32 now, const LifeView& view, const CityStream& city, Rng& rng, LifeOut& out);
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
  f32 last_herd_check_ = -99, last_moo_ = -99, herds_at_ = -99;
  std::vector<Flock> flocks_;
  u32 flock_serial_ = 0;
  u32 trees_down_ = 0, tree_ground_cursor_ = 0;
  std::vector<Bird> birds_;
  std::vector<Tree> trees_;
  std::vector<Pet> pets_;
  CatAttack cat_;
  Vec2 pet_anchor_{1e9f, 1e9f};
  f64 epoch_ = 0;
  u32 tree_serial_ = ~0u;                       // the city's furniture set the trees were taken from
  std::unordered_set<u64> trees_gone_;          // blown over and cleared away (stays so while its tile is loaded)
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
