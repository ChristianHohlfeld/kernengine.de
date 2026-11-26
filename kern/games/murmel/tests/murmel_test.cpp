// Murmel, nativ gespielt: die App läuft in der App-Laufzeit der Engine (kern_app_start, kern_app_frame), Frame für
// Frame mit fester Zeit, und bekommt NUR Tasten (kern_app_key) - wie ein Spieler. Ein Autopilot drückt sie: er rollt
// zur nächsten Münze, dann ins Ziel. Der Lauf ist deterministisch; am Ende steht ein erwarteter Zustand (Level-Wechsel
// im erwarteten Frame, Punkte, Töne, "Geschafft!"), und dieselbe Spur ergibt sich bei einem zweiten Lauf noch einmal.
//
//   murmel_test [--spur]     (--spur: jede Sekunde eine Zeile)
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "kern/app/run.h"
#include "kern/game/embedded.h"
#include "murmel.h"

extern "C" kern::i32 kern_app_start(kern::i32 width, kern::i32 height);
extern "C" kern::i32 kern_app_frame(kern::f64 now_ms, kern::i32 page_hidden);
extern "C" void kern_app_key(kern::u32 key, kern::i32 down);
extern "C" void kern_app_stop();

using namespace kern;

namespace {

struct Lauf {
  std::vector<std::string> spur;   // eine Zeile je Sekunde
  int wechsel = -1;                // der Frame, in dem Level 2 begann
  int geschafft = -1;              // der Frame, in dem "Geschafft!" kam
  f32 punkte = 0;
  u32 toene = 0;
  std::string text;
};

bool unten[static_cast<u32>(Key::count)] = {};
void taste(Key k, bool down) {
  if (unten[static_cast<u32>(k)] == down) return;
  unten[static_cast<u32>(k)] = down;
  kern_app_key(static_cast<u32>(k), down ? 1 : 0);
}

/// Der Autopilot: die Tasten, die die Murmel zum nächsten Ziel bringen (Münzen nach Id, dann das Ziel).
void steuere(const game::GameWorld& w) {
  Mat4 m;
  if (!w.actor_matrix(w.player(), m)) return;
  const Vec3 p{m.m[12], m.m[13], m.m[14]};
  const game::Actor* ziel = nullptr;
  for (const game::Actor& a : w.level().actors)
    if (a.pickup && w.actor_alive(a.id)) { ziel = &a; break; }
  if (!ziel)
    for (const game::Actor& a : w.level().actors)
      if (a.trigger) { ziel = &a; break; }
  if (!ziel) return;
  // (wohin: zum Ziel mit höchstens 3 m/s, minus was die Murmel schon an Fahrt hat - sie kreist nicht um die Münze)
  const Vec3 v = w.actor_velocity(w.player());
  const Vec3 weg{ziel->transform.position.x - p.x, ziel->transform.position.y - p.y, 0};
  const f32 lang = std::sqrt(weg.x * weg.x + weg.y * weg.y) + 1e-6f;
  const f32 soll = std::fmin(3.0f, 1.5f * lang);
  const Vec3 d{weg.x / lang * soll - v.x, weg.y / lang * soll - v.y, 0};
  const f32 yaw = w.camera_yaw() * kPi / 180;
  const Vec3 vor{std::cos(yaw), std::sin(yaw), 0}, rechts{vor.y, -vor.x, 0};
  const f32 f = d.x * vor.x + d.y * vor.y, r = d.x * rechts.x + d.y * rechts.y;
  const f32 n = std::sqrt(f * f + r * r) + 1e-6f;
  if (n < 0.3f) { taste(Key::up, false); taste(Key::down, false); taste(Key::right, false); taste(Key::left, false); return; }
  taste(Key::up, f > 0.35f * n);
  taste(Key::down, f < -0.35f * n);
  taste(Key::right, r > 0.35f * n);
  taste(Key::left, r < -0.35f * n);
}

Lauf spiele(murmel::Murmel& spiel, int frames, bool zeigen) {
  Lauf l;
  for (bool& b : unten) b = false;
  if (!kern_app_start(320, 240)) {
    std::printf("FEHLER: kern_app_start: %s\n", last_error());
    return l;
  }
  for (int i = 0; i < frames; ++i) {
    if (spiel.welt() && !spiel.fertig()) steuere(*spiel.welt());
    const int vorher = spiel.level();
    const bool war_fertig = spiel.fertig();
    kern_app_frame(i * (1000.0 / 60.0), 0);
    if (spiel.level() == 1 && vorher == 0 && l.wechsel < 0) l.wechsel = i;
    if (spiel.fertig() && !war_fertig && l.geschafft < 0) l.geschafft = i;
    if (i % 60 == 0) {
      char z[200];
      Mat4 m;
      const bool hat = spiel.welt() && spiel.welt()->actor_matrix(spiel.welt()->player(), m);
      std::snprintf(z, sizeof z, "t=%3d level=%d punkte=%.0f toene=%u murmel=(%.3f %.3f %.3f)", i / 60, spiel.level(), spiel.punkte(), spiel.toene(),
                    hat ? m.m[12] : 0, hat ? m.m[13] : 0, hat ? m.m[14] : 0);
      l.spur.push_back(z);
      if (zeigen) std::printf("%s\n", z);
    }
  }
  l.punkte = spiel.punkte();
  l.toene = spiel.toene();
  l.text = spiel.text();
  for (u32 k = 0; k < static_cast<u32>(Key::count); ++k) taste(static_cast<Key>(k), false);
  kern_app_stop();
  return l;
}

int fehler = 0;
void pruefe(bool ok, const char* was) {
  if (!ok) {
    ++fehler;
    std::printf("FEHLER: %s\n", was);
  }
}

}  // namespace

int main(int argc, char** argv) {
  const bool zeigen = argc > 1 && std::strcmp(argv[1], "--spur") == 0;
  static murmel::Murmel spiel;
  pruefe(run(spiel, game::embedded_project().settings) == 0, "kern::run nimmt die App");
  const int frames = 60 * 45;
  const Lauf a = spiele(spiel, frames, zeigen);
  std::printf("Lauf 1: Wechsel in Frame %d, geschafft in Frame %d, %.0f Punkte, %u Toene, Anzeige \"%s\"\n", a.wechsel, a.geschafft, a.punkte, a.toene, a.text.c_str());
  // der erwartete Zustand (ein Lauf ohne Zufall: dieselben Frames, dieselben Punkte)
  pruefe(a.wechsel == MURMEL_WECHSEL, "Level 2 beginnt im erwarteten Frame");
  pruefe(a.geschafft == MURMEL_GESCHAFFT, "geschafft im erwarteten Frame");
  pruefe(a.punkte == MURMEL_PUNKTE, "die erwarteten Punkte (6 Muenzen und das Sprungfeld)");
  pruefe(a.toene == MURMEL_TOENE, "die erwarteten Toene");
  pruefe(a.text.find("Geschafft!") == 0, "die Anzeige sagt: Geschafft!");
  // ein zweiter Lauf derselben App (die App-Laufzeit startet sie neu): dieselbe Spur, Zeile für Zeile
  const Lauf b = spiele(spiel, frames, false);
  pruefe(a.spur == b.spur && a.wechsel == b.wechsel && a.geschafft == b.geschafft, "der zweite Lauf ist derselbe");
  std::printf(fehler ? "murmel_test: %d Fehler\n" : "murmel_test: gut\n", fehler);
  return fehler ? 1 : 0;
}
