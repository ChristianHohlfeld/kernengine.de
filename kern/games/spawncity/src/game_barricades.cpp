// SpawnCity on Kern: zombie barricades (zombie_nights.mjs, building_interiors.mjs barricades).
// Buildings are shelters: at night every door of the walkable building carries a barricade of six
// planks on the outside (by day there is the door and no planks). A zombie that wants the player
// inside goes to the door (1.15 m out) and smashes it plank by plank - one plank per door every
// 3.4 s however many pound on it (each zombie 0.8..1.3 x 3.4 s) - then steps in (1.8 m inside) and
// hunts the player through the building. Nobody is clawed through an intact barricade. Near a door
// (3.2 m) the player boards it up again for 10 zombie points a plank (REPARIEREN, F). Nothing is
// locked: the player walks in and out as he likes. Planks are kept per building.
#include <cmath>

#include "game.h"
#include "game_life.h"
#include "kern/geo/primitives.h"

namespace sc {

namespace {
constexpr i32 kPlanks = 6;          // building_interiors.mjs PLANKS
constexpr f32 kPlankS = 3.4f;       // zombie_nights.mjs PLANK_MS
constexpr i32 kRepairCost = 10;     // REPAIR_COST
constexpr f32 kRepairM = 3.2f;      // nearDoor
constexpr f32 kDoorW = 1.8f;        // DOOR_W
using kern::Mat4;

i64 building_key(const Interiors::Plan& p) {
  if (p.outer.empty()) return 0;
  return (static_cast<i64>(std::lround(p.outer[0].x * 10)) << 32) ^ (static_cast<i64>(std::lround(p.outer[0].y * 10)) & 0xffffffffll) ^ static_cast<i64>(p.outer.size());
}
}  // namespace

bool Game::gaps_barricade_doors(std::vector<GapsState::DoorView>& out) {
  out.clear();
  if (!gaps_ || !glife_ || !glife_->interiors.active()) return false;
  const Interiors::Plan& p = glife_->interiors.plan();
  if (p.doors.empty()) return false;
  const i64 key = building_key(p);
  std::vector<u8>& st = gaps_->planks[key];
  if (st.size() != p.doors.size()) st.assign(p.doors.size(), kPlanks);
  if (gaps_->planks.size() > 400) {  // plankState: at most 400 buildings remembered
    for (auto it = gaps_->planks.begin(); it != gaps_->planks.end(); ++it)
      if (it->first != key) { gaps_->planks.erase(it); break; }
  }
  for (std::size_t i = 0; i < p.doors.size(); ++i) {
    const Interiors::Door& d = p.doors[i];
    out.push_back({static_cast<i32>(i), d.c, d.d, d.n, static_cast<i32>(st[i]), p.levels.empty() ? p.base : p.levels[0], key});
  }
  return true;
}

Vec3 Game::gaps_zombie_goal(i32 person, Vec3 pos, Vec3 player, f32 now, bool& walled, bool& hold) {
  walled = false;
  hold = false;
  std::vector<GapsState::DoorView> doors;
  if (!gaps_barricade_doors(doors)) return player;
  GapsState& g = *gaps_;
  GapsState::ZombieDoor& z = g.zombie_doors[person];
  const Interiors& I = glife_->interiors;
  const bool p_in = I.inside(player + Vec3{0, 0, 0.3f});
  if (z.entering) {
    // stepping in through the broken barricade
    if (std::hypot(z.enter_at.x - pos.x, z.enter_at.y - pos.y) < 0.25f) { z.entering = false; z.inside = I.inside(pos + Vec3{0, 0, 0.3f}); }
    else return {z.enter_at.x, z.enter_at.y, pos.z};
  }
  if (z.inside) {
    if (!I.inside(pos + Vec3{0, 0, 0.3f})) z.inside = false;
    return player;
  }
  if (!p_in) return player;
  // doorFor: the nearest door of the shelter
  const GapsState::DoorView* d = nullptr;
  f32 dd = 1e9f;
  for (const GapsState::DoorView& v : doors) {
    const f32 k = std::hypot(pos.x - v.c.x, pos.y - v.c.y);
    if (k < dd) { dd = k; d = &v; }
  }
  if (!d) return player;
  walled = d->planks > 0;
  const Vec2 o = d->c + d->n * 1.15f;
  if (std::hypot(o.x - pos.x, o.y - pos.y) >= 0.9f) return {o.x, o.y, pos.z};
  if (d->planks > 0) {
    // breach: one plank per door every PLANK_MS, whoever pounds on it
    hold = true;
    f32& door_next = g.door_next_break[d->key * 31 + d->i];
    if (now >= z.next_plank && now >= door_next) {
      z.next_plank = now + kPlankS * (0.8f + rng_.unit() * 0.5f);
      door_next = now + kPlankS * 0.9f;
      std::vector<u8>& st = g.planks[d->key];
      if (d->i < static_cast<i32>(st.size()) && st[static_cast<std::size_t>(d->i)] > 0) --st[static_cast<std::size_t>(d->i)];
      world_->play_sound(combat_sfx(combat_audio::Kind::bounce), {d->c.x, d->c.y, d->floor + 1}, 0.5f, 0.55f, true);
      ++g.planks_broken;
    }
    return {o.x - d->n.x * 0.2f, o.y - d->n.y * 0.2f, pos.z};  // facing the door
  }
  z.entering = true;
  z.enter_at = d->c - d->n * 1.8f;
  return {z.enter_at.x, z.enter_at.y, pos.z};
}

bool Game::gaps_repair_barricade() {
  // repairNearest: the door within 3.2 m, a plank missing, 10 points
  if (!gaps_ || !combat_.zombies.active) return false;
  std::vector<GapsState::DoorView> doors;
  if (!gaps_barricade_doors(doors)) return false;
  const Vec3 me = player_position();
  const GapsState::DoorView* best = nullptr;
  f32 bd = kRepairM;
  for (const GapsState::DoorView& v : doors) {
    const f32 k = std::hypot(me.x - v.c.x, me.y - v.c.y);
    if (k < bd) { bd = k; best = &v; }
  }
  if (!best || best->planks >= kPlanks || combat_.zombies.points < kRepairCost) return false;
  ++gaps_->planks[best->key][static_cast<std::size_t>(best->i)];
  combat_.zombies.points -= kRepairCost;
  world_->play_sound(combat_sfx(combat_audio::Kind::bounce), cam_.eye, 0.4f, 0.7f, false);  // playCombatAudio "bounce" .4 / .7
  osfx("zombie_nights.repairKnock", cam_.eye);
  return true;
}

bool Game::gaps_repair_offer() const {
  // the REPAIR button shows near a door with a plank missing, during a zombie night
  if (!gaps_ || !combat_.zombies.active || !glife_ || !glife_->interiors.active()) return false;
  const Interiors::Plan& p = glife_->interiors.plan();
  const auto it = gaps_->planks.find(building_key(p));
  const Vec3 me = player_position();
  for (std::size_t i = 0; i < p.doors.size(); ++i) {
    const i32 n = it == gaps_->planks.end() || i >= it->second.size() ? kPlanks : it->second[i];
    if (n < kPlanks && std::hypot(me.x - p.doors[i].c.x, me.y - p.doors[i].c.y) < kRepairM) return true;
  }
  return false;
}

void Game::gaps_barricades_draw() {
  if (!gaps_ || !gaps_->renderer) return;
  GapsState& g = *gaps_;
  if (!g.plank_part.id) {
    // plankG: (DOOR_W + .5) x .06 x .2, 0x6b4a2c
    kern::Material m;
    m.roughness = 0.85f;
    kern::MeshData box = kern::primitives::box({kDoorW + 0.5f, 0.06f, 0.2f}, {0.42f, 0.29f, 0.17f});
    g.plank_part.id = g.renderer->allocate_id();
    kern::MeshInput in = box.as_input(kern::kMaxExtensionAttributes);
    in.material = m;
    in.visible = false;
    if (!g.renderer->upsert(g.plank_part.id, in)) { g.renderer->release_id(g.plank_part.id); g.plank_part.id = 0; return; }
  }
  g.plank_part.transforms.clear();
  g.plank_part.tints.clear();
  std::vector<GapsState::DoorView> doors;
  // barricadesShown: at night the doorway is boarded up
  if (combat_.zombies.active && gaps_barricade_doors(doors)) {
    for (const GapsState::DoorView& d : doors) {
      const f32 yaw = std::atan2(d.d.y, d.d.x);
      for (i32 k = 0; k < d.planks; ++k) {
        // position (+-.06 along, .22 out, .35 + .36 k up), turned +-(.08 + .05 (k % 3)) about the normal
        const f32 along = k % 2 ? 0.06f : -0.06f, tilt = (k % 2 ? 1.0f : -1.0f) * (0.08f + 0.05f * static_cast<f32>(k % 3));
        const Vec3 at{d.c.x + d.d.x * along + d.n.x * 0.22f, d.c.y + d.d.y * along + d.n.y * 0.22f, d.floor + 0.35f + static_cast<f32>(k) * 0.36f};
        const Mat4 m = Mat4::trs(at, Quat::axis_angle({0, 0, 1}, yaw), {1, 1, 1}) * Mat4::trs({}, Quat::axis_angle({0, 1, 0}, tilt), {1, 1, 1});
        g.plank_part.transforms.insert(g.plank_part.transforms.end(), m.m, m.m + 16);
        g.plank_part.tints.insert(g.plank_part.tints.end(), {1, 1, 1, 1});
      }
    }
  }
  if (g.plank_part.transforms.empty()) { g.renderer->set_visible(g.plank_part.id, false); return; }
  g.renderer->update_instances(g.plank_part.id, kern::Span<const f32>(g.plank_part.transforms.data(), g.plank_part.transforms.size()),
                               kern::Span<const f32>(g.plank_part.tints.data(), g.plank_part.tints.size()));
  g.renderer->set_visible(g.plank_part.id, true);
}

}  // namespace sc
