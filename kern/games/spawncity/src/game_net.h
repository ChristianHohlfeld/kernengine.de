// State of this player's multiplayer presentation (game_net.cpp): tracers and flashes, the kill
// feed, hit markers, voice, the spawn anchor, the big map and its route. One member of Game
// (`mp_`), made when it is first needed.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#pragma once

#include <string>
#include <vector>

#include "common.h"
#include "mp_art.h"
#include "mp_rules.h"
#include "net_session.h"

namespace sc {

/// Per-player multiplayer presentation state (owned by Game through mp_).
struct MpLocal {
  MpArt art;
  f32 world_own[16] = {};  // our own WORLD switches while the host's apply
  bool world_saved = false;
  mp::PingBoard pings;
  struct Tracer { Vec3 p, v; f32 left; u32 color; };
  std::vector<Tracer> tracers;
  struct Flash { Vec3 p; f32 r; f64 born, life; };
  std::vector<Flash> flashes;
  struct Feed { net::Kill k; f64 at; };
  std::vector<Feed> feed;
  f64 hit_at = -1e12, hurt_at = -1e12, last_hurt = -1e12, regen_clock = 0, last_by_at = -1e12;
  bool hit_kill = false, hit_head = false;
  u8 shot_zone = 0;  // the zone of our last round on a mate (tests)
  Vec3 hurt_from;
  u8 last_by = net::kNobody, last_weapon = 0, last_zone = 0;
  f32 last_hp = -1;
  bool was_dead = false;
  Vec3 death_feet;
  // voice (proximity_voice.mjs): explicit consent - the page opens the microphone only while mic is on
  mp::VoiceCapture capture;
  bool mic = false, muted = false, everywhere = false;
  u32 seq = 0;
  bool r3_was = false, r3_latched = false, y_was = false;
  // the joining player's spawn anchor
  bool anchor_pending = false;
  Vec3 anchor;
  // the host left: 1 we take over, 2 we join the one who does (lan_vs.mjs authority handover)
  i32 migrate = 0;
  bool rejoined = false;  // back in after a handover: no spawn anchor (we stay where we are)
  // big map + navigation (world_map_full.mjs)
  bool map_open = false;
  bool has_dest = false;
  f64 dest_lon = 0, dest_lat = 0;
  Vec2 dest;
  std::string dest_name;
  std::vector<Vec2> route;
  bool route_streets = false;
  f64 route_at = -1e12;
  u32 fetch_id = 0;          // > 0: the page should fetch a road route (no street graph to the goal)
  std::string fetch_json;
  u32 frames = 0;
  bool ff_setting = false, ff_rule = false;  // the FRIENDLY FIRE setting / the session's rule as last seen
};

}  // namespace sc
