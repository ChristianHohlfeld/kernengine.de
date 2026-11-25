// The whole game as classes, started (tools/all_program.sh): the level whose every Blueprint is a class of the program,
// linked with the engine, played without a window - Begin and then <ticks> steps of 1/60 s, as the game world runs it.
// Says what started, which Blueprints run as their classes and which in the interpreter (and why it says so), what the
// runtime reported, and whether blueprints::bind found every class of the header.
//
//   all_program_run <level.klevel> [<ticks>]
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>

#include "gl_stub.h"
#include "kern/game/runtime.h"
#include "kern/kern.h"
#include "kern/logic/logic.h"
#include "level_blueprints.h"

kern::App* kern_create_app() { return nullptr; }  // (the app runtime is linked in; it needs a factory)

int main(int argc, char** argv) {
  using namespace kern;
  if (argc < 2) {
    std::fprintf(stderr, "usage: all_program_run <level.klevel> [<ticks>]\n");
    return 2;
  }
  const int ticks = argc > 2 ? std::atoi(argv[2]) : 600;
  std::string text, error;
  if (FILE* f = std::fopen(argv[1], "rb")) {
    char b[65536];
    std::size_t n;
    while ((n = std::fread(b, 1, sizeof b, f)) > 0) text.append(b, n);
    std::fclose(f);
  }
  game::Level level;
  if (!game::from_text(text, level, &error)) {
    std::printf("FAIL read: %s\n", error.empty() ? "cannot read it" : error.c_str());
    return 1;
  }
  Renderer renderer;
  RendererOptions o;
  renderer.init(o, {});
  auto world = std::make_unique<game::GameWorld>(renderer, nullptr);
  world->logic_native = true;
  if (!world->start(level, game::SimMode::play) || !world->logic_runtime()) {
    std::printf("FAIL start: %s\n", last_error());
    return 1;
  }
  logic::Runtime& rt = *world->logic_runtime();
  rt.jit = false;
  std::printf("started: %zu actors, %zu Blueprint instances\n", level.actors.size(), rt.instances().size());
  std::printf("blueprints::bind: %s\n", blueprints::bind(rt) ? "every class of the header bound" : "NOT every class of the header bound");
  game::PlayInput in;
  Camera cam;
  int done = 0;
  for (; done < ticks; ++done) {
    world->step(1.0f / 60, in);
    world->update_camera(cam, 1.0f / 60, in);
  }
  std::printf("ticks: %d of %d\n", done, ticks);
  std::map<std::string, int> tiers;
  for (const auto& inst : rt.instances()) {
    tiers[inst->tier()] += 1;
    if (std::string(inst->tier()) != "native C++") std::printf("  not native: %s: %s\n", inst->name().c_str(), inst->tier());
  }
  for (const auto& [tier, n] : tiers) std::printf("tier %s: %d\n", tier.c_str(), n);
  const char* e = last_error();
  std::printf("last error: %s\n", e && *e ? e : "(none)");
  world->stop();
  world.reset();
  renderer.shutdown();
  return 0;
}
