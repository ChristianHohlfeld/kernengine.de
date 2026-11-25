// Profile: the editor opens SpawnCity's whole level and each Blueprint in turn; how long does the state take?
#include <chrono>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <memory>
#include "gl_stub.h"
#include "kern/kern.h"
#include "kern/core/json.h"
#include "kern/studio/studio.h"
using namespace kern;
App* kern_create_app() { return nullptr; }
int main(int argc, char** argv) {
  Renderer renderer; game::NightWindows windows;
  ShaderExtension* ext[1] = {&windows};
  RendererOptions o; o.shadows = true;
  renderer.init(o, Span<ShaderExtension* const>(ext, 1));
  auto s = std::make_unique<studio::Studio>();
  s->init(&renderer, &windows, false);
  for (u32 i = 0; i < 4; ++i) s->set_viewport_rect(i, {0, 0, 960, 540}, 960, 540, 1, i == 0);
  std::ifstream in(argv[1]); std::stringstream ss; ss << in.rdbuf();
  auto now = [] { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); };
  double t = now();
  json::Value c = json::Value::object(); c.set("cmd", "open"); c.set("text", ss.str()); c.set("name", "spawncity.klevel");
  json::Value r = s->command(c);
  std::printf("open %.2f s\n", now() - t); std::fflush(stdout);
  t = now(); json::Value st = s->state(); std::printf("state %.2f s\n", now() - t);
  const char* only = argc > 2 ? argv[2] : nullptr;
  const json::Value& actors = st["actors"];
  for (u32 i = 0; i < actors.size(); ++i) {
    const std::string name = actors[i][1].as_string();
    if (only && name != only) continue;
    json::Value oc = json::Value::object(); oc.set("cmd", "logicOpen"); oc.set("owner", actors[i][0].as_number());
    t = now(); s->command(oc); const double a = now() - t;
    t = now(); json::Value l = s->state(); const double b = now() - t;
    if (!l["logic"]["exists"].as_bool()) continue;
    std::printf("%-40s open %.2f state %.2f nodes %u\n", name.c_str(), a, b, (unsigned)l["logic"]["nodes"].size()); std::fflush(stdout);
    const json::Value& sheets = l["logic"]["sheets"];
    for (u32 k = 0; k < sheets.size(); ++k) {
      const u32 nn = (u32)sheets[k]["nodes"].as_number();
      if (k >= 3 && nn < 400) continue;
      json::Value sc = json::Value::object(); sc.set("cmd", "logicSheet"); sc.set("function", sheets[k]["name"].as_string());
      s->command(sc); t = now(); json::Value ls = s->state(); const double d = now() - t;
      std::printf("   sheet %-40s state %.2f nodes %u (of %u) json %zu\n", sheets[k]["name"].as_string().c_str(), d, (unsigned)ls["logic"]["nodes"].size(), nn, json::stringify(ls).size()); std::fflush(stdout);
    }
  }
  s.reset(); renderer.shutdown();
}
