// SpawnCity on Kern, milestone 3: the world (see game_world.h).
#include <cstdio>
#include <cstdlib>

#include "game.h"
#include "game_world.h"

namespace sc {

using kern::game::ActorId;

void Game::hud_world(std::string& s) {
  // The ground (terrain.h): elevation loaded, its range, ground tiles, water cells, craters.
  const Terrain& t = city_.terrain();
  const TerrainStats& ts = t.stats();
  char b[320];
  std::snprintf(b, sizeof b, ",\"terrain\":{\"dem\":%s,\"settled\":%s,\"min\":%.1f,\"max\":%.1f,\"render\":%u,\"collision\":%u,\"water\":%u,\"craters\":%u}",
                ts.dem ? "true" : "false", t.settled() ? "true" : "false", ts.min_height, ts.max_height, ts.render_tiles, ts.collision_tiles, ts.water_cells,
                ts.craters);
  s += b;
}

void Game::update_world(f32 dt) { (void)dt; }

void Game::step_world(f32 h) { (void)h; }

void Game::draw_world() {}

void Game::world_blast(Vec3 at, f32 radius, f32 damage, bool by_player) { (void)at; (void)radius; (void)damage; (void)by_player; }

bool Game::world_command(const std::string& cmd, std::string& out) {
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  char b[256];
  if (starts("ground")) {
    // "ground" (under the player) or "ground X Y": terrain height, normal, water there.
    Vec3 p = player_position();
    if (cmd.size() > 7) std::sscanf(cmd.c_str() + 7, "%f %f", &p.x, &p.y);
    const Terrain& t = city_.terrain();
    const Vec3 n = t.normal(p.x, p.y);
    std::snprintf(b, sizeof b, "{\"x\":%.1f,\"y\":%.1f,\"z\":%.2f,\"elevation\":%.2f,\"normal\":[%.3f,%.3f,%.3f],\"water\":%s,\"playerZ\":%.2f}", p.x, p.y,
                  t.height(p.x, p.y), t.elevation(p.x, p.y), n.x, n.y, n.z, t.water_at(p.x, p.y) ? "true" : "false", player_position().z);
    out = b;
    return true;
  }
  return false;
}

}  // namespace sc
