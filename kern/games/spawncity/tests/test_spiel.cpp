// The small game, played natively: the game (sc::game(), spiel.h) as its App runs on an app runtime of the engine with its project
// (the embedded one: level/spawncity.kproject and its level), as kern::run runs it - offline, in the stand-in town, a
// fixed frame time - and gets ONLY keys, as a player gives them. An autopilot presses them: it waits for the ground,
// arms the drone (R), climbs (Space), flies forward (W), turns (D under the right stick: the arrows), and lands (R in
// the air). At the end stands an expected state - placed, armed in the air, height and way flown, the city built, the
// drone down and disarmed after the landing - and a second run gives the same track to the bit.
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "../src/spiel.h"
#include "kern/app/runtime.h"
#include "kern/game/embedded.h"
#include "kern/game/program.h"
#include "kern/game/project.h"
#include "kern/test/test.h"

namespace {

// The App of the game as main.cpp gives it to the engine (SpawnCityApp: the entry is not linked into the tests).
struct GameApp final : kern::App {
  bool start(kern::Engine& e) override { return sc::game().start(e); }
  void update(kern::Engine& e, const kern::Frame& f) override { sc::game().update(e, f); }
  void stop(kern::Engine& e) override { sc::game().stop(e); }
};

struct Flight {
  std::vector<std::string> track;   // one line a second
  bool placed = false, armed_high = false, down_disarmed = false, started = false;
  int placed_at = -1;               // the frame the drone stood on the ground
  float top = -1e9f, way = 0;       // the highest point over the start, the way flown on the ground (m)
  unsigned buildings = 0;
  std::string hud;
};

Flight fly_once() {
  Flight f;
  kern::game::ProgramApp program;
  std::string error;
  if (!kern::game::from_text(kern::game::embedded_project_text(), program.project, &error)) { std::printf("the project does not read: %s\n", error.c_str()); return f; }
  GameApp app;
  program.given = &app;
  kern::Renderer renderer;
  kern::AppRuntime rt(renderer);
  const char* args = "offline=1&dem=0&mobile=0";
  rt.engine().set_args(args, static_cast<kern::u32>(std::strlen(args)));
  if (rt.give(program, program.project.settings) != 0) return f;
  if (!rt.start(320, 240)) { std::printf("the game does not start: %s\n", kern::last_error()); return f; }
  f.started = true;
  kern::Input& in = rt.engine().input();
  const double ms = 1000.0 / 60;
  kern::Vec3 start{};
  // the autopilot's plan, in seconds after the drone stands on the ground
  for (int frame = 0; frame < 60 * 90; ++frame) {
    const double t = f.placed_at < 0 ? -1 : (frame - f.placed_at) / 60.0;
    auto hold = [&](kern::Key k, bool down) { in.key(k, down); };
    if (f.placed_at >= 0) {
      hold(kern::Key::r, (t > 3.0 && t < 3.05) || (t > 22.0 && t < 22.05));   // arm after the calibration, land at the end
      hold(kern::Key::space, t > 4.0 && t < 7.0);                             // climb
      hold(kern::Key::w, t > 8.0 && t < 16.0);                                // forward
      hold(kern::Key::right, t > 12.0 && t < 13.0);                           // a turn
    }
    rt.frame(frame * ms, false);
    const sc::SpawnCity& g = sc::game();
    if (!f.placed && g.placed()) { f.placed = true; f.placed_at = frame; start = g.drone_position(); }
    if (!f.placed) continue;
    const kern::Vec3 p = g.drone_position();
    f.top = std::fmax(f.top, p.z - start.z);
    f.way = std::fmax(f.way, std::hypot(p.x - start.x, p.y - start.y));
    if (t > 15 && t < 16 && g.drone().telemetry().armed && p.z - start.z > 3) f.armed_high = true;
    if (frame % 60 == 0) {
      char line[160];
      std::snprintf(line, sizeof line, "%d %.6f %.6f %.6f %d %llu", frame, p.x, p.y, p.z, g.drone().telemetry().armed ? 1 : 0, static_cast<unsigned long long>(g.steps()));
      f.track.push_back(line);
    }
    if (t > 40) {
      f.down_disarmed = !g.drone().telemetry().armed && p.z - start.z < 0.5f;
      break;
    }
  }
  f.hud = sc::game().hud_json();
  f.buildings = 0;
  const std::size_t at = f.hud.find("\"buildings\":");
  if (at != std::string::npos) f.buildings = static_cast<unsigned>(std::atoi(f.hud.c_str() + at + 12));
  rt.stop();
  return f;
}

}  // namespace

KERN_TEST(spiel_flies_the_drone_over_the_town) {
  const Flight a = fly_once();
  KERN_CHECK(a.started, "the game starts on the app runtime with its project");
  KERN_CHECK(a.placed && a.placed_at >= 0 && a.placed_at < 60 * 30, "the drone stands on the ground of the stand-in town within 30 s (frame %d)", a.placed_at);
  KERN_CHECK(a.buildings > 0, "the town has buildings (%u): %s", a.buildings, a.hud.substr(0, 200).c_str());
  KERN_CHECK(a.armed_high, "armed and over 3 m high while it flies forward");
  KERN_CHECK(a.top > 3 && a.top < 60, "it climbed (highest %.2f m over the start)", a.top);
  KERN_CHECK(a.way > 10, "it flew forward (%.2f m from the start)", a.way);
  KERN_CHECK(a.down_disarmed, "it landed and disarmed after R in the air");
  KERN_CHECK(a.hud.find("\"mode\":\"drone\"") != std::string::npos && a.hud.find("\"placed\":true") != std::string::npos, "the HUD says drone and placed");
  const Flight b = fly_once();
  KERN_CHECK(b.track == a.track && b.placed_at == a.placed_at, "a second run flies the same track (%zu / %zu lines; placed at %d / %d)", b.track.size(), a.track.size(), b.placed_at,
             a.placed_at);
  for (std::size_t i = 0; i < a.track.size() && i < b.track.size(); ++i)
    if (a.track[i] != b.track[i]) { std::printf("  first difference: %s | %s\n", a.track[i].c_str(), b.track[i].c_str()); break; }
}

KERN_TEST(spiel_commands) {
  kern::game::ProgramApp program;
  std::string error;
  KERN_CHECK(kern::game::from_text(kern::game::embedded_project_text(), program.project, &error), "the project reads: %s", error.c_str());
  GameApp app;
  program.given = &app;
  kern::Renderer renderer;
  kern::AppRuntime rt(renderer);
  const char* args = "offline=1&dem=0";
  rt.engine().set_args(args, static_cast<kern::u32>(std::strlen(args)));
  KERN_CHECK(rt.give(program, program.project.settings) == 0 && rt.start(320, 240), "the game starts: %s", kern::last_error());
  sc::SpawnCity& g = sc::game();
  KERN_CHECK(g.command("settings schema").find("drone") != std::string::npos, "the settings' schema has the drone's settings");
  KERN_CHECK(g.command("set audio.on 0") == "ok" && g.command("get audio.on") == "0", "TON off: %s", g.command("get audio.on").c_str());
  KERN_CHECK(g.command("set music.on 0") == "ok" && g.command("get music.on") == "0", "MUSIK off");
  KERN_CHECK(g.command("set no.such 1") == "unknown setting", "an unknown setting is said");
  KERN_CHECK(g.command("cam follow") == "follow" && g.command("cam") == "third" && g.command("cam") == "fpv", "the drone's cameras in turn");
  KERN_CHECK(g.command("pause 1") == "paused" && g.command("pause 0") == "running", "pause");
  KERN_CHECK(g.command("start 99 0") == "usage: start <lat> <lon> [name]", "a start place outside the map is refused");
  KERN_CHECK(g.command("net host") == "unknown command" && g.command("mode foot") == "unknown command", "what the small game has not is said, not pretended");
  const std::string frame = g.command("audio frame");
  KERN_CHECK(frame.front() == '{' && frame.find("\"set\":{\"soundEnabled\":0,\"musicEnabled\":0") != std::string::npos, "the original's sound reads the settings: %s", frame.substr(0, 160).c_str());
  for (int i = 0; i < 60 * 30 && !g.placed(); ++i) rt.frame(i * 1000.0 / 60, false);
  KERN_CHECK(g.placed(), "placed");
  KERN_CHECK(g.command("reset") == "ok" && g.hud_json().find("\"resets\":1") != std::string::npos, "RESET brings the drone back to the start");
  rt.stop();
}
