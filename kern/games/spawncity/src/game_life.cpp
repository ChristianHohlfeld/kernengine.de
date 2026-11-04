// SpawnCity on Kern, milestone 3 (life): animals and street life, ragdolls, walkable buildings
// in the game (see game_life.h). The simulation is in life.cpp, ragdoll.cpp and interiors.cpp;
// this file feeds them the world and carries out what they ask for.
#include <cmath>
#include <cstdio>

#include "game.h"
#include "game_life.h"
#include "kern/audio/synth.h"

namespace sc {

std::string json_escape(const std::string& s);  // game.cpp

namespace {

using kern::audio::SynthPatch;
using kern::audio::Wave;
using Sound = LifeOut::Sound;

std::vector<f32> layered(std::initializer_list<std::pair<SynthPatch, f32>> layers, u32 rate, f32 peak = 0.8f) {
  std::vector<f32> out;
  for (const auto& [p, at] : layers) kern::audio::mix_into(out, kern::audio::synthesize(p, rate), at, rate, 1);
  kern::audio::normalize(out, peak);
  return out;
}

// The voices of street life as synth patches (animal_audio.mjs, cows.mjs moo, pigeons.mjs coo and
// wing clatter, jogger_chain.mjs stomp and jingle, street_voices.mjs horn and scream).
void make_sounds(kern::audio::Mixer& mixer, kern::audio::ClipId* clips) {
  const u32 rate = mixer.sample_rate();
  auto clip = [&](Sound s, std::vector<f32> v, const char* name) { clips[static_cast<u32>(s)] = mixer.adopt_clip(std::move(v), 1, name); };
  {  // moo: two detuned saws 105..135 Hz through a band-pass that opens (cows.mjs)
    SynthPatch m;
    m.wave = Wave::saw; m.frequency = 112; m.slide = -0.2f; m.attack = 0.18f; m.sustain = 0.95f; m.release = 0.35f;
    m.vibrato_depth = 0.3f; m.vibrato_rate = 4; m.lowpass = 1400; m.resonance = 0.5f; m.volume = 0.6f;
    SynthPatch m2 = m;
    m2.frequency = 113.5f;
    clip(Sound::moo, layered({{m, 0}, {m2, 0}}, rate), "spawncity:life_moo");
    m.frequency = 165; m.sustain = 0.45f; m.release = 0.2f; m2.frequency = 167; m2.sustain = 0.45f; m2.release = 0.2f;
    clip(Sound::moo_pain, layered({{m, 0}, {m2, 0}}, rate), "spawncity:life_moo_pain");
  }
  {  // coo "gruu-gurr": sines with a fast trill and a falling pitch (pigeons.mjs)
    SynthPatch c;
    c.wave = Wave::sine; c.frequency = 330; c.slide = -0.9f; c.attack = 0.05f; c.sustain = 0.22f; c.release = 0.08f;
    c.tremolo_depth = 0.45f; c.tremolo_rate = 26; c.lowpass = 900; c.volume = 0.5f;
    SynthPatch c2 = c;
    c2.frequency = 300; c2.sustain = 0.4f;
    clip(Sound::coo, layered({{c, 0}, {c2, 0.38f}}, rate, 0.6f), "spawncity:life_coo");
    SynthPatch w;  // wing claps: noise pulses that thin out
    w.wave = Wave::noise; w.sustain = 0.5f; w.release = 0.4f; w.tremolo_depth = 0.95f; w.tremolo_rate = 22; w.lowpass = 2600; w.highpass = 600; w.volume = 0.6f; w.seed = 9;
    clip(Sound::flap, layered({{w, 0}}, rate, 0.6f), "spawncity:life_flap");
  }
  {  // bird squawk (animal_audio.mjs bird): two square chirps 2600 -> 1400, 2900 -> 1100 Hz
    SynthPatch s;
    s.wave = Wave::square; s.frequency = 2600; s.slide = -7.4f; s.attack = 0.004f; s.sustain = 0.08f; s.release = 0.04f; s.lowpass = 3400; s.volume = 0.4f;
    SynthPatch s2 = s;
    s2.frequency = 2900; s2.slide = -8.6f; s2.sustain = 0.11f;
    clip(Sound::squawk, layered({{s, 0}, {s2, 0.13f}}, rate, 0.55f), "spawncity:life_squawk");
  }
  {  // thud (a round into a body), stomp and accent (the runners' beat)
    SynthPatch t;
    t.wave = Wave::sine; t.frequency = 140; t.slide = -5; t.attack = 0.002f; t.sustain = 0.04f; t.release = 0.12f; t.volume = 0.7f;
    clip(Sound::thud, layered({{t, 0}}, rate), "spawncity:life_thud");
    t.frequency = 78; t.slide = -3.5f;
    clip(Sound::stomp, layered({{t, 0}}, rate, 0.5f), "spawncity:life_stomp");
    SynthPatch clap;
    clap.wave = Wave::noise; clap.attack = 0.001f; clap.sustain = 0.01f; clap.release = 0.05f; clap.lowpass = 2600; clap.highpass = 1000; clap.volume = 0.6f;
    t.frequency = 92;
    clip(Sound::stomp_accent, layered({{t, 0}, {clap, 0.01f}}, rate, 0.7f), "spawncity:life_stomp_accent");
  }
  {  // jingle (KOMPLETTE LAUFGRUPPE): 523 659 784 1047 784 1047 Hz, 0.11 s apart
    std::vector<f32> out;
    const f32 notes[6] = {523, 659, 784, 1047, 784, 1047};
    for (int i = 0; i < 6; ++i) {
      SynthPatch n;
      n.wave = i % 2 ? Wave::square : Wave::triangle; n.frequency = notes[i]; n.attack = 0.01f; n.sustain = 0.05f; n.release = 0.16f; n.volume = 0.4f;
      kern::audio::mix_into(out, kern::audio::synthesize(n, rate), 0.11f * static_cast<f32>(i), rate);
    }
    kern::audio::normalize(out, 0.6f);
    clip(Sound::jingle, std::move(out), "spawncity:life_jingle");
  }
  {  // horn: two detuned saw reeds a major third apart through a bright band (street_voices.mjs)
    SynthPatch h;
    h.wave = Wave::saw; h.frequency = 415; h.attack = 0.012f; h.sustain = 0.22f; h.release = 0.03f; h.lowpass = 2200; h.highpass = 700; h.volume = 0.45f;
    SynthPatch h2 = h;
    h2.frequency = 523;
    clip(Sound::horn, layered({{h, 0}, {h2, 0}}, rate, 0.7f), "spawncity:life_horn");
    h.frequency = 233; h2.frequency = 294; h.lowpass = h2.lowpass = 1300; h.highpass = h2.highpass = 350; h.sustain = h2.sustain = 0.5f;
    clip(Sound::horn_bus, layered({{h, 0}, {h2, 0}}, rate, 0.75f), "spawncity:life_horn_bus");
  }
  for (int f = 0; f < 2; ++f) {  // scream "aaah": a buzzy voice through a vowel formant, pitch up and wavering
    SynthPatch v;
    v.wave = Wave::saw; v.frequency = f ? 520 : 330; v.slide = 1.6f; v.attack = 0.04f; v.sustain = 0.12f; v.release = 0.01f;
    v.vibrato_depth = 0.6f; v.vibrato_rate = 7; v.lowpass = f ? 2600 : 2300; v.resonance = 0.6f; v.highpass = f ? 700 : 500; v.volume = 0.6f;
    SynthPatch tail = v;
    tail.frequency = v.frequency * 1.18f; tail.slide = -0.5f; tail.attack = 0.01f; tail.sustain = 0.45f; tail.release = 0.25f;
    SynthPatch breath;
    breath.wave = Wave::noise; breath.sustain = 0.6f; breath.release = 0.2f; breath.lowpass = 3000; breath.highpass = 1500; breath.volume = 0.15f;
    clip(f ? Sound::scream_f : Sound::scream, layered({{v, 0}, {tail, 0.12f}, {breath, 0}}, rate, 0.7f), f ? "spawncity:life_scream_f" : "spawncity:life_scream");
  }
}

f32 dist2d(Vec3 a, Vec3 b) { return std::hypot(a.x - b.x, a.y - b.y); }

}  // namespace

// ---------------------------------------------------------------------------------------------
// per frame

void Game::life_update(f32 dt) {
  if (!world_ || !world_placed_ || !engine_) return;
  if (!glife_) glife_ = std::make_unique<GameLife>();
  GameLife& G = *glife_;
  if (!G.ready) {
    G.life.init(world_->physics(), mobile_);
    G.art_ready = G.art.init(engine_->renderer);
    G.interiors.init(engine_->renderer, world_->physics());
    G.ready = true;
  }
  if (!G.sounds_ready && mixer_.ready()) { make_sounds(mixer_, G.clips); G.sounds_ready = true; }
  const f32 now = world_->sim_time();
  kern::physics::World& w = world_->physics();
  // the world as street life sees it
  LifeView v;
  v.physics = &w;
  v.city = &city_;
  v.focus = focus_position();
  v.player = player_position();
  if (mode_ == Mode::foot) v.player.z -= 0.73f;
  v.eye = cam_.eye;
  v.on_foot = mode_ == Mode::foot;
  const Vec3 pv = mode_ == Mode::foot ? world_->actor_velocity(soldier_) : Vec3{};
  v.player_speed = std::hypot(pv.x, pv.y);
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    const TrafficCar& c = cars_[i];
    if (!c.placed || !world_->actor_alive(c.actor)) continue;
    const BodyState s = state_of(c.actor);
    LifeCar lc;
    lc.position = s.position;
    lc.velocity = s.velocity;
    lc.rotation = s.rotation;
    lc.half = {c.car->spec().half.x, c.car->spec().half.y};
    lc.mass = w.mass(body(c.actor));
    lc.car = static_cast<i32>(i);
    lc.player = mode_ == Mode::car && static_cast<i32>(i) == car_index_;
    lc.driver = c.driver && !c.wreck;
    v.cars.push_back(lc);
  }
  LifeOut& out = G.out;
  out.clear();
  // ---- ragdolls for the people who fell (people_impacts.mjs, world_person_ragdoll.mjs)
  std::vector<People::Fall> falls;
  people_.take_falls(falls);
  for (const People::Fall& f : falls) {
    if (f.index < 0 || f.index >= static_cast<i32>(people_.all().size())) continue;
    People::Person& p = people_.at(f.index);
    Vec3 vel;
    if (f.car) {
      vel = f.push;
      if (!f.alive) {
        // run over and killed: the car's momentum (as people_impacts.mjs, at most 15 m/s)
        const f32 sp = std::hypot(f.push.x, f.push.y);
        vel = Vec3{f.push.x, f.push.y, 0} * 1.05f + Vec3{0, 0, 1.2f + sp * 0.16f};
        const f32 l = kern::length(vel);
        if (l > 15) vel = vel * (15 / l);
      }
      // momentum exchange: the car that hit loses what it gave the person (75 kg, 0.55)
      const f32 sp = std::hypot(vel.x, vel.y);
      for (const LifeCar& c : v.cars) {
        if (dist2d(c.position, p.pos) > 6 || sp < 0.5f) continue;
        world_->push_actor(cars_[c.car].actor, {-c.velocity.x * 75 * 0.55f, -c.velocity.y * 75 * 0.55f, 0}, true);
        if (c.player) world_->shake_camera(0.18f);
        break;
      }
      if (G.sounds_ready) {
        kern::audio::PlayParams pp;
        pp.volume = clampf(0.25f + sp * 0.04f, 0, 0.9f);
        pp.spatial = true;
        pp.position = p.pos + Vec3{0, 0, 1};
        pp.max_distance = 60;
        mixer_.play(G.clips[static_cast<u32>(Sound::thud)], pp);
      }
    } else {
      Vec3 d = f.push;
      d.z = 0;
      const f32 l = std::hypot(d.x, d.y);
      d = l > 1e-3f ? d * (1 / l) : Vec3{std::cos(p.yaw), std::sin(p.yaw), 0};
      vel = d * 2.5f + Vec3{0, 0, 1.4f};  // a round (jogger_chain.mjs)
      for (const GameLife::Blast& b : G.blasts) {
        const f32 dd = dist2d(b.at, p.pos);
        if (dd < b.radius * 1.2f) {
          const f32 k = (1 - dd / (b.radius * 1.2f)) * 7;
          vel = d * (2 + k) + Vec3{0, 0, 4 + 3 * (1 - dd / (b.radius * 1.2f))};
        }
      }
    }
    Ragdolls::Colors col{p.shirt, p.pants, p.skin};
    const u64 key = static_cast<u64>(f.index) + 1;
    if (G.life.ragdolls().spawn(key, p.pos, p.yaw, vel, col, f.alive, now)) {
      ++G.falls;
      bool known = false;
      for (u64 k : G.person_dolls) known = known || k == key;
      if (!known) G.person_dolls.push_back(key);
    }
  }
  G.blasts.clear();
  // ---- street life
  G.life.update(dt, now, v, rng_, out);
  // people who got up again walk on from there
  std::vector<Ragdolls::Up> ups;
  G.life.ragdolls().take_ups(ups);
  for (const Ragdolls::Up& u : ups) {
    if (u.key == 0 || u.key > people_.all().size()) continue;
    const i32 i = static_cast<i32>(u.key - 1);
    if (people_.at(i).knocked_until > 0 && people_.at(i).state != People::State::down) {
      people_.stand_up(i, {u.feet.x, u.feet.y, city_.ground_height(u.feet.x, u.feet.y)}, u.yaw);
      ++G.getups;
    }
  }
  // bodies follow their ragdoll; a person who is gone (or a new one in the slot) drops it
  for (std::size_t k = 0; k < G.person_dolls.size();) {
    const u64 key = G.person_dolls[k];
    const i32 i = static_cast<i32>(key - 1);
    const bool valid = i < static_cast<i32>(people_.all().size());
    Vec3 pel;
    f32 yaw;
    if (!valid || !G.life.ragdolls().where(key, pel, yaw)) { G.person_dolls.erase(G.person_dolls.begin() + static_cast<std::ptrdiff_t>(k)); continue; }
    People::Person& p = people_.at(i);
    const bool keep = p.state == People::State::down || now < p.knocked_until;
    if (!keep) { G.life.ragdolls().remove(key); G.person_dolls.erase(G.person_dolls.begin() + static_cast<std::ptrdiff_t>(k)); continue; }
    if (G.life.ragdolls().physical(key)) {
      p.pos = {pel.x, pel.y, city_.ground_height(pel.x, pel.y)};
      p.fall_dir = {std::cos(yaw), std::sin(yaw), 0};
    }
    ++k;
  }
  // ---- street voices (street_voices.mjs): what the people think of you
  const std::vector<People::Person>& all = people_.all();
  if (G.prev_state.size() < all.size()) G.prev_state.resize(all.size(), 255);
  const Vec3 me = v.player;
  for (std::size_t i = 0; i < all.size(); ++i) {
    const People::Person& p = all[i];
    const u8 st = static_cast<u8>(p.state), was = G.prev_state[i];
    G.prev_state[i] = st;
    if (p.kind != People::Kind::citizen || p.state == People::State::down || p.state == People::State::gone) continue;
    const f32 hear = kern::length(p.pos + Vec3{0, 0, 1.6f} - cam_.eye);
    if (hear > 55) continue;
    const u32 seed = static_cast<u32>(i) * 2654435761u + p.seed;
    if (p.state == People::State::flee && was != st && was != 255) {
      if ((seed >> 7) % 3 == 0) G.life.scream(p.pos, seed, now, hear, out);
      else G.life.shout(p.pos, "scared", seed, now, hear, out);
    } else if (p.state == People::State::fight && was != st) {
      G.life.shout(p.pos, "angry", seed, now, hear, out);
    } else if (mode_ == Mode::foot && v.player_speed > 1.2f && dist2d(p.pos, me) < 0.8f) {
      G.life.shout(p.pos, "bump", seed, now, hear, out);
    } else if (mode_ == Mode::car && car_index_ >= 0) {
      // a near miss: the car flies past within a step (not into the person)
      const LifeCar* mine = nullptr;
      for (const LifeCar& c : v.cars) if (c.player) mine = &c;
      if (mine && std::hypot(mine->velocity.x, mine->velocity.y) > 7 && dist2d(mine->position, p.pos) < 6) {
        const Vec3 l = to_local(mine->rotation, p.pos - mine->position);
        if (std::fabs(l.x) < mine->half.x + 0.5f && std::fabs(l.y) > mine->half.y + 0.4f && std::fabs(l.y) < mine->half.y + 2.2f) G.life.shout(p.pos, "nearmiss", seed, now, hear, out);
      }
    }
  }
  // impatient drivers: a traffic car standing behind the player (or someone in the lane) honks
  if (now >= G.next_honk_check) {
    G.next_honk_check = now + 0.25f;
    for (const LifeCar& c : v.cars) {
      if (c.player || !c.driver || cars_[c.car].police || std::hypot(c.velocity.x, c.velocity.y) > 1.0f) continue;
      auto blocks = [&](Vec3 q) {
        const Vec3 l = to_local(c.rotation, q - c.position);
        return l.x > c.half.x && l.x < c.half.x + 7 && std::fabs(l.y) < 1.6f && std::fabs(l.z) < 3;
      };
      i32 who = -2;
      if (mode_ == Mode::foot && blocks(me)) who = -1;
      for (std::size_t i = 0; i < all.size() && who == -2; ++i)
        if (all[i].kind == People::Kind::citizen && all[i].state != People::State::down && all[i].state != People::State::gone && blocks(all[i].pos)) who = static_cast<i32>(i);
      if (who == -2) continue;
      const bool bus = c.half.x > 3;
      if (G.life.honk(c.position, bus, static_cast<u32>(c.car), now, kern::length(c.position - cam_.eye), out) && who >= 0)
        G.life.shout(all[who].pos, "honked", static_cast<u32>(who) * 2654435761u, now, kern::length(all[who].pos - cam_.eye), out);
    }
  }
  // ---- walkable buildings
  G.interiors.update(dt, now, mode_ == Mode::foot ? v.player : Vec3{1e9f, 1e9f, 0}, mode_ == Mode::foot, city_);
  // ---- what street life asked for
  for (const LifeOut::Play& s : out.sounds) {
    if (!G.sounds_ready) break;
    kern::audio::PlayParams pp;
    pp.volume = s.volume;
    pp.pitch = s.pitch;
    pp.spatial = true;
    pp.position = s.at;
    pp.min_distance = s.sound == Sound::horn_bus ? 9 : 4;
    pp.max_distance = s.sound == Sound::horn || s.sound == Sound::horn_bus ? 160 : s.sound == Sound::jingle ? 400 : 60;
    if (s.sound == Sound::jingle) pp.spatial = false;
    mixer_.play(G.clips[static_cast<u32>(s.sound)], pp);
  }
  for (const LifeOut::Burst& b : out.bursts) world_->particle_burst(b.at, b.color);
  for (const LifeOut::Impulse& im : out.impulses)
    if (im.car >= 0 && im.car < static_cast<i32>(cars_.size())) world_->push_actor(cars_[im.car].actor, im.impulse, true);
  for (const LifeOut::Say& s : out.speech) G.speech.push_back(s);
  if (G.speech.size() > 8) G.speech.erase(G.speech.begin(), G.speech.end() - 8);
  if (!out.banner.empty()) { G.banner = out.banner; G.banner_until = now + out.banner_for; }
}

void Game::life_draw() {
  if (!glife_ || !glife_->art_ready) return;
  GameLife& G = *glife_;
  const f32 now = world_ ? world_->sim_time() : 0;
  G.art.begin();
  if (world_placed_) {
    G.art.life(G.life, now, cam_.eye, 170);
    G.interiors.draw(G.art, now, cam_.eye);
  }
  G.art.end();
}

void Game::life_draw_people() {
  // The running club in the town's people drawing (people_art.h): neon shirts, dark shorts.
  if (!glife_) return;
  const Life::Chain* c = glife_->life.chain();
  if (!c) return;
  const f32 now = world_->sim_time();
  const u32 skins[5] = {0xe0b08c, 0xc48e6c, 0xa87052, 0x7b4a33, 0xd2a27e};
  for (u32 i = 0; i < 10; ++i) {
    const Life::Runner& r = c->runners[i];
    if (!r.alive || kern::length(r.pos - cam_.eye) > 220) continue;
    People::Person p;
    p.pos = r.pos;
    p.yaw = r.yaw_s;
    p.v = 2.9f;
    p.phase = r.phase;
    p.shirt = kern::hex_color(0xd8ff3a);
    p.pants = kern::hex_color(0x1c2a3a);
    p.skin = kern::hex_color(skins[i % 5]);
    people_art_.person(p, now);
  }
}

bool Game::life_hides_person(i32 index) const {
  return glife_ && glife_->life.ragdolls().has(static_cast<u64>(index) + 1);
}

bool Game::life_shot(Vec3 from, Vec3& to, f32 damage) {
  if (!glife_ || !glife_->ready) return false;
  const f32 now = world_->sim_time();
  glife_->life.heard_shot(from, to, now);
  return glife_->life.shot(from, to, damage, now, glife_->out);
}

void Game::life_blast(Vec3 at, f32 radius) {
  if (!glife_ || !glife_->ready) return;
  glife_->blasts.push_back({at, radius, world_->sim_time()});
  glife_->life.blast(at, radius, world_->sim_time(), glife_->out);
}

void Game::life_hud(std::string& s) {
  if (!glife_ || !glife_->ready) return;
  GameLife& G = *glife_;
  const f32 now = world_->sim_time();
  const Life& L = G.life;
  u32 dead = 0, air = 0, birds = 0;
  for (const Life::Cow& c : L.cows()) dead += c.dead ? 1 : 0;
  for (const Life::Flock& f : L.flocks()) air += f.air ? 1 : 0;
  u32 pigeons = 0;
  for (const Life::Flock& f : L.flocks()) pigeons += static_cast<u32>(f.birds.size());
  for (const Life::Bird& b : L.birds()) birds += now >= b.dead_until ? 1 : 0;
  u32 joggers = 0;
  if (const Life::Chain* c = L.chain())
    for (const Life::Runner& r : c->runners) joggers += r.alive ? 1 : 0;
  char b[640];
  std::snprintf(b, sizeof b,
                ",\"life\":{\"cows\":%u,\"cowsDead\":%u,\"cowHits\":%u,\"flocks\":%u,\"flocksAir\":%u,\"pigeons\":%u,\"birds\":%u,\"birdHits\":%u,\"trees\":%u,"
                "\"joggers\":%u,\"joggerStreak\":%u,\"ragdolls\":%u,\"ragdollSpawns\":%u,\"falls\":%u,\"getups\":%u,\"shouts\":%u,\"honks\":%u",
                static_cast<u32>(L.cows().size()), dead, L.cow_hits(), static_cast<u32>(L.flocks().size()), air, pigeons, birds, L.bird_hits(), static_cast<u32>(L.trees().size()),
                joggers, L.jogger_streak(), L.ragdolls().active(), L.ragdolls().spawned(), G.falls, G.getups, L.shouts(), L.honks());
  s += b;
  if (now < G.banner_until) s += ",\"banner\":\"" + json_escape(G.banner) + "\"";
  // speech for the browser (handed out once)
  s += ",\"say\":[";
  for (std::size_t i = 0; i < G.speech.size(); ++i) {
    const LifeOut::Say& x = G.speech[i];
    std::snprintf(b, sizeof b, "%s[\"%s\",%.2f,%.2f,%.2f,%.2f]", i ? "," : "", json_escape(x.text).c_str(), x.pitch, x.rate, x.volume, x.delay);
    s += b;
  }
  G.speech.clear();
  // speech bubbles over the speakers' heads, projected here (x, y in 0..1 of the view)
  s += "],\"bubbles\":[";
  bool first = true;
  const f32 aspect = frame_h_ > 0 ? frame_w_ / frame_h_ : 16.0f / 9;
  const kern::Mat4 vp = kern::projection_matrix(cam_, aspect) * kern::Mat4::look_at(cam_.eye, cam_.target, cam_.up);
  for (const Life::Voice& vc : L.voices()) {
    const kern::Vec4 c = vp.transform({vc.at.x, vc.at.y, vc.at.z, 1});
    if (c.w <= 0.1f) continue;
    const f32 x = c.x / c.w, y = c.y / c.w;
    if (std::fabs(x) > 1.1f || std::fabs(y) > 1.1f) continue;
    std::snprintf(b, sizeof b, "%s[%.4f,%.4f,\"%s\",\"%s\",%s]", first ? "" : ",", (x + 1) / 2, (1 - y) / 2, json_escape(vc.text).c_str(), vc.mood.c_str(), vc.until - now < 0.35f ? "true" : "false");
    s += b;
    first = false;
  }
  s += "]";
  G.interiors.hud(s);
  s += "}";
}

bool Game::life_command(const std::string& cmd, std::string& out) {
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  if (!(starts("cows") || starts("pigeons") || starts("joggers") || starts("ragdoll") || starts("shout") || starts("honk") || starts("interior") || starts("life") ||
        starts("knock") || starts("birds")))
    return false;
  if (!glife_ || !glife_->ready) { out = "life not ready"; return true; }
  GameLife& G = *glife_;
  const f32 now = world_->sim_time();
  Vec3 me = player_position();
  if (mode_ == Mode::foot) me.z -= 0.73f;
  const Vec3 look = kern::normalize(Vec3{cam_.target.x - cam_.eye.x, cam_.target.y - cam_.eye.y, 0}, {1, 0, 0});
  char b[256];
  if (cmd == "life") {
    std::string s;
    life_hud(s);
    out = s.size() > 8 ? s.substr(8) : "{}";  // the "life" object alone
    return true;
  }
  if (cmd == "cows") {
    // Tests: a herd of 5 on a 14 m pasture 12 m ahead of the player
    const Vec3 c = me + look * 12;
    G.life.spawn_herd({c.x, c.y}, 14, 5, now);
    std::snprintf(b, sizeof b, "{\"cows\":%u,\"at\":[%.1f,%.1f]}", static_cast<u32>(G.life.cows().size()), c.x, c.y);
    out = b;
    return true;
  }
  if (cmd == "cows kill") {
    // Tests: a blast on the herd (the bodies fly as Box3D boxes)
    for (const Life::Cow& c : G.life.cows())
      if (!c.dead) { explosion(c.pos + Vec3{0.5f, 0, 0.5f}, 7, 0, false); break; }
    u32 dead = 0;
    for (const Life::Cow& c : G.life.cows()) dead += c.dead ? 1 : 0;
    std::snprintf(b, sizeof b, "{\"dead\":%u}", dead);
    out = b;
    return true;
  }
  if (cmd == "pigeons") {
    LifeView v;
    v.physics = &world_->physics();
    v.city = &city_;
    v.focus = me;
    const Vec3 c = me + look * 9;
    const bool ok = G.life.spawn_flock({c.x, c.y}, now, rng_, v);
    std::snprintf(b, sizeof b, "{\"ok\":%s,\"flocks\":%u}", ok ? "true" : "false", static_cast<u32>(G.life.flocks().size()));
    out = b;
    return true;
  }
  if (cmd == "pigeons spook") { G.life.spook_pigeons(now); out = "ok"; return true; }
  if (cmd == "joggers") {
    LifeView v;
    v.physics = &world_->physics();
    v.city = &city_;
    v.focus = v.player = me;
    v.eye = cam_.eye;
    out = G.life.spawn_joggers(v, now, rng_) ? "ok" : "no street";
    return true;
  }
  if (cmd == "ragdoll") {
    // Tests: a passer-by-like body thrown in front of the player
    Ragdolls::Colors col;
    const Vec3 at = me + look * 3;
    const bool ok = G.life.ragdolls().spawn(0x7e570000ull + G.life.ragdolls().spawned(), at, std::atan2(look.y, look.x), look * 4 + Vec3{0, 0, 2}, col, false, now);
    std::snprintf(b, sizeof b, "{\"ok\":%s,\"active\":%u}", ok ? "true" : "false", G.life.ragdolls().active());
    out = b;
    return true;
  }
  if (cmd == "ragdoll state") {
    Vec3 p;
    f32 yaw;
    const u64 key = 0x7e570000ull + G.life.ragdolls().spawned() - 1;
    if (!G.life.ragdolls().where(key, p, yaw)) { out = "none"; return true; }
    std::snprintf(b, sizeof b, "{\"pelvis\":[%.2f,%.2f,%.2f],\"ground\":%.2f}", p.x, p.y, p.z, city_.ground_height(p.x, p.y));
    out = b;
    return true;
  }
  if (cmd == "knock") {
    // Tests: the nearest walking citizen is knocked down (as by a car at 6 m/s) and gets up again
    i32 best = -1;
    f32 bd = 60;
    for (std::size_t i = 0; i < people_.all().size(); ++i) {
      const People::Person& p = people_.all()[i];
      if (p.kind != People::Kind::citizen || p.state == People::State::down || p.state == People::State::gone || now < p.knocked_until) continue;
      const f32 d = dist2d(p.pos, me);
      if (d < bd) { bd = d; best = static_cast<i32>(i); }
    }
    if (best < 0) { out = "nobody"; return true; }
    People::Person& p = people_.at(best);
    p.knocked_until = now + 6;
    p.v = 0;
    Ragdolls::Colors col{p.shirt, p.pants, p.skin};
    const u64 key = static_cast<u64>(best) + 1;
    G.life.ragdolls().spawn(key, p.pos, p.yaw, {4, 0, 2.2f}, col, true, now);
    G.person_dolls.push_back(key);
    std::snprintf(b, sizeof b, "{\"person\":%d,\"distance\":%.1f}", best, bd);
    out = b;
    return true;
  }
  if (cmd == "shout") {
    G.out.clear();
    const bool ok = G.life.shout(me + look * 3, "bump", rng_.next(), now, 3, G.out);
    for (const LifeOut::Say& s : G.out.speech) G.speech.push_back(s);
    out = ok ? "ok" : "rate limited";
    return true;
  }
  if (cmd == "honk") {
    G.out.clear();
    out = G.life.honk(me + look * 5, false, rng_.next(), now, 5, G.out) ? "ok" : "rate limited";
    if (G.sounds_ready)
      for (const LifeOut::Play& s : G.out.sounds) {
        kern::audio::PlayParams pp;
        pp.spatial = true;
        pp.position = s.at;
        pp.max_distance = 160;
        mixer_.play(G.clips[static_cast<u32>(s.sound)], pp);
      }
    return true;
  }
  if (cmd == "birds") {
    std::snprintf(b, sizeof b, "{\"birds\":%u,\"first\":[%.1f,%.1f,%.1f]}", static_cast<u32>(G.life.birds().size()), G.life.birds()[0].pos.x, G.life.birds()[0].pos.y,
                  G.life.birds()[0].pos.z);
    out = b;
    return true;
  }
  if (starts("interior")) return G.interiors.command(cmd, me, out, [&](Vec3 p, f32 yaw) {
    (void)yaw;
    if (mode_ != Mode::foot) return false;
    world_->teleport(soldier_, p + Vec3{0, 0, 0.73f});
    world_->set_actor_velocity(soldier_, {});
    return true;
  });
  return false;
}

}  // namespace sc
