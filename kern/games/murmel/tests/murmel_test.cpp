// Murmel, nativ gespielt: die App läuft in der App-Laufzeit der Engine (kern_app_start, kern_app_frame), Frame für
// Frame mit fester Zeit, und bekommt NUR Tasten (kern_app_key) - wie ein Spieler. Ein Autopilot drückt sie: er rollt
// zur nächsten Münze, dann ins Ziel. Der Lauf ist deterministisch; am Ende steht ein erwarteter Zustand (Level-Wechsel
// im erwarteten Frame, Punkte, Töne, "Geschafft!"), und dieselbe Spur ergibt sich bei einem zweiten Lauf noch einmal.
// Die App ist der Programm-Blueprint Murmel des Projekts (src/main.cpp: kern::run(embedded_project())); den Stand liest
// der Test aus dem laufenden Programm (game::running_program): die Variablen und Funktionen des Blueprints, seine Welt.
// Mit MURMEL_QUELLE ist die App ihre Quelle src/murmel.cpp, nativ gebaut (kern::run(murmel::spiel(), projekt), ein
// Projekt ohne Programm-Blueprints): derselbe Lauf - die Quelle rechnet mit den Zahlen der Sprache wie ihr Blueprint
// (kern/docs/blueprint_cpp.md, „Zahlen“), tests/run.sh vergleicht die drei Wege.
//
//   murmel_test [--spur] [--ergebnis <datei>]   (--spur: jede Sekunde eine Zeile; --ergebnis: der Lauf als Text, Zeile
//                                                für Zeile - die Spur, die Frames, Punkte, Töne, Widgets)
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "kern/app/run.h"
#include "kern/app/runtime.h"
#include "kern/game/embedded.h"
#include "kern/game/program.h"
#include "kern/game/runtime.h"
#include "kern/logic/engine_host.h"
#include "kern/logic/logic.h"
#ifdef MURMEL_QUELLE
#include "murmel.h"
#endif

extern "C" kern::i32 kern_app_start(kern::i32 width, kern::i32 height);
extern "C" kern::i32 kern_app_frame(kern::f64 now_ms, kern::i32 page_hidden);
extern "C" void kern_app_key(kern::u32 key, kern::i32 down);
extern "C" void kern_app_stop();
extern "C" void kern_app_set_args(const char* text, kern::u32 length);
extern "C" void kern_app_ui_event(const char* id, kern::u32 length, kern::i32 index);
extern "C" const kern::f32* kern_app_audio(kern::u32 frames, kern::u32* out_len);

using namespace kern;

namespace {

struct Lauf {
  std::vector<std::string> spur;   // eine Zeile je Sekunde
  int wechsel = -1;                // der Frame, in dem Level 2 begann
  int geschafft = -1;              // der Frame, in dem "Geschafft!" kam
  f32 punkte = 0;
  u32 toene = 0;
  std::string text;
  std::string erster_text;          // die Anzeige nach dem ersten Frame
  std::string widgets_mitte;        // die Widgets der Seite in Frame 200 (Level 1) und am Ende
  std::string widgets_ende;
  int nach_wahl_level = -1;         // das Level, nachdem im Menü am Ende "Nur Level 2" gewählt wurde
  bool nach_wahl_fertig = true;
  std::string stufen;               // wie die Blueprints der Welt laufen (am Ende, im Level 2)
  f32 lautest = 0;                  // der lauteste Wert, den die App-Laufzeit aus dem Mixer der Engine gab
  i64 bestwert = -1;                // der Bestwert am Ende (vor dem Menü: das Programm endet mit der App)
};

#ifdef MURMEL_QUELLE
/// Der Stand des Spiels, wie die App der Quelle ihn hat (murmel::spiel()).
struct Spiel {
  static constexpr const char* kApp = "die App ist die Quelle src/murmel.cpp, nativ gebaut";
  static bool app() { return true; }
  int level() const { return murmel::spiel().level(); }
  bool fertig() const { return murmel::spiel().fertig(); }
  u32 toene() const { return murmel::spiel().toene(); }
  i64 bestwert() const { return murmel::spiel().bestwert(); }
  std::string text() const { return murmel::spiel().text(); }
  f32 punkte() const { return static_cast<f32>(murmel::spiel().punkte()); }
  game::GameWorld* welt() const { return const_cast<game::GameWorld*>(murmel::spiel().welt()); }
  static int starte() { return run(murmel::spiel(), game::embedded_project()); }
};
#else
/// Der Stand des Spiels, wie das laufende Programm ihn hat: der Programm-Blueprint Murmel (die App) - seine Variablen
/// (Instance::get), seine Funktion punkte (Instance::call) und die Welt, die er in welt_ hält (ein Objekt der Engine
/// des Programms). Ohne laufendes Programm: nichts (Level -1, keine Welt).
struct Spiel {
  static logic::Instance* murmel() {
    game::Program* p = game::running_program();
    if (!p || !p->runtime()) return nullptr;
    for (const auto& i : p->runtime()->instances())
      if (i->name() == "Murmel") return i.get();
    return nullptr;
  }
  static logic::Value var(const char* name) {
    logic::Instance* m = murmel();
    return m ? m->get(name) : logic::Value();
  }
  int level() const { return murmel() ? static_cast<int>(var("level_").as_int()) : -1; }
  bool fertig() const { return var("fertig_").as_bool(); }
  u32 toene() const { return static_cast<u32>(var("toene_").as_int()); }
  i64 bestwert() const { return var("bestwert_").as_int(); }
  std::string text() const { return var("text_").as_text(); }
  f32 punkte() const {
    logic::Instance* m = murmel();
    std::vector<logic::Value> r;
    return m && m->call("punkte", {}, r) && !r.empty() ? static_cast<f32>(r[0].as_number()) : -1;
  }
  game::GameWorld* welt() const {
    game::Program* p = game::running_program();
    logic::EngineObjects* o = p && p->runtime() ? p->runtime()->host().engine_objects() : nullptr;
    const logic::ObjectRef w = var("welt_").as_object();
    return o && w.valid() ? o->get<game::GameWorld>(w) : nullptr;
  }
  static constexpr const char* kApp = "die App ist der Programm-Blueprint Murmel";
  static bool app() { return murmel() != nullptr; }
  static int starte() { return run(game::embedded_project()); }
};
#endif

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

Lauf spiele(const Spiel& spiel, int frames, bool zeigen) {
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
    // (wie die Seite: die App-Laufzeit gibt den Mix des Mixers der Engine, ein Frame Ton je Bild)
    u32 n = 0;
    const f32* pcm = kern_app_audio(800, &n);
    for (u32 k = 0; k < n; ++k) l.lautest = std::fmax(l.lautest, std::fabs(pcm[k]));
    if (i == 0) l.erster_text = spiel.text();
    if (i == 200) l.widgets_mitte = app_runtime().engine().ui_json();
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
  // (das Programm: die App; die Welt: ihre Blueprints)
  if (game::Program* p = game::running_program(); p && p->runtime())
    for (const game::BlueprintRunning& b : game::blueprints_running(*p->runtime())) l.stufen += b.name + ": " + b.tier + "; ";
  if (game::GameWorld* w = spiel.welt(); w && w->logic_runtime())
    for (const game::BlueprintRunning& b : game::blueprints_running(*w->logic_runtime())) l.stufen += b.name + ": " + b.tier + "; ";
  l.punkte = spiel.punkte();
  l.toene = spiel.toene();
  l.text = spiel.text();
  l.bestwert = spiel.bestwert();
  l.widgets_ende = app_runtime().engine().ui_json();
  // das Menü am Ende, wie die Seite es meldet: "Nur Level 2" (Eintrag 1) - im nächsten Frame beginnt Level 2
  kern_app_ui_event("weiter", 6, 1);
  kern_app_frame(frames * (1000.0 / 60.0), 0);
  kern_app_frame((frames + 1) * (1000.0 / 60.0), 0);
  l.nach_wahl_level = spiel.level();
  l.nach_wahl_fertig = spiel.fertig();
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
  bool zeigen = false;
  const char* ergebnis = nullptr;
  for (int i = 1; i < argc; ++i) {
    if (std::strcmp(argv[i], "--spur") == 0) {
      zeigen = true;
    } else if (std::strcmp(argv[i], "--ergebnis") == 0 && i + 1 < argc) {
      ergebnis = argv[++i];
    } else {
      std::printf("murmel_test: %s: unbekannt (--spur, --ergebnis <datei>)\n", argv[i]);
      return 2;
    }
  }
  const Spiel spiel;
  pruefe(Spiel::starte() == 0, "kern::run startet das Programm des Projekts");
  pruefe(Spiel::app(), Spiel::kApp);
  // die Stände in einem Ordner für den Test, ohne Bestwert zu Beginn (kern/docs/app.md 5.)
  const std::string staende = std::string(std::getenv("TMPDIR") ? std::getenv("TMPDIR") : "/tmp") + "/murmel-test-saves";
  const std::string args = "saves=" + staende;
  kern_app_set_args(args.c_str(), static_cast<u32>(args.size()));
  std::remove((staende + "/murmel.kern-save").c_str());
  const int frames = 60 * 45;
  const Lauf a = spiele(spiel, frames, zeigen);
  std::printf("Lauf 1: Wechsel in Frame %d, geschafft in Frame %d, %.0f Punkte, %u Toene, Anzeige \"%s\"\n", a.wechsel, a.geschafft, a.punkte, a.toene, a.text.c_str());
  // der erwartete Zustand (ein Lauf ohne Zufall: dieselben Frames, dieselben Punkte)
  pruefe(a.wechsel == MURMEL_WECHSEL, "Level 2 beginnt im erwarteten Frame");
  pruefe(a.geschafft == MURMEL_GESCHAFFT, "geschafft im erwarteten Frame");
  pruefe(a.punkte == MURMEL_PUNKTE, "die erwarteten Punkte (6 Muenzen und das Sprungfeld)");
  pruefe(a.toene == MURMEL_TOENE, "die erwarteten Toene");
  pruefe(a.text.find("Geschafft!") == 0, "die Anzeige sagt: Geschafft!");
  std::printf("Ton: lautester Wert %.3f\n", a.lautest);
  pruefe(a.lautest > 0.01f, "die Toene sind im Mix der Engine zu hoeren (engine.mixer, kern_app_audio)");
  // die App und die Blueprints der Welt laufen in der Stufe, die der Bau sagt: ohne ihre Klassen im Interpreter, mit
  // ihnen als C++
  std::printf("Stufen: %s\n", a.stufen.c_str());
#ifdef MURMEL_QUELLE
  const std::string programm;   // (die App ist C++: das Programm hat keine Blueprints)
#else
  const std::string programm = std::string("murmel_cpp: ") + MURMEL_STUFE + "; Murmel: " + MURMEL_STUFE + "; ";
#endif
  pruefe(a.stufen == programm + "Level: " + MURMEL_STUFE + "; Sprungfeld: " + MURMEL_STUFE + "; ", "die Blueprints laufen als " MURMEL_STUFE);
  pruefe(a.bestwert == MURMEL_PUNKTE, "der Bestwert ist der Punktestand des Laufs");
  // die Anzeige sind Widgets der Engine (kern/docs/app.md 6.): Text, Balken, am Ende das Menü
  std::printf("Widgets in Frame 200: %s\nWidgets am Ende: %s\n", a.widgets_mitte.c_str(), a.widgets_ende.c_str());
  pruefe(a.widgets_mitte.find("\"kind\":\"text\",\"id\":\"stand\"") != std::string::npos && a.widgets_mitte.find("\"kind\":\"bar\",\"id\":\"muenzen\"") != std::string::npos,
         "im Spiel: der Stand als Text, die Muenzen als Balken");
  pruefe(a.widgets_ende.find("\"kind\":\"menu\",\"id\":\"weiter\"") != std::string::npos && a.widgets_ende.find("Nur Level 2") != std::string::npos, "am Ende: das Menue");
  pruefe(a.nach_wahl_level == 1 && !a.nach_wahl_fertig, "die Wahl im Menue startet Level 2");
  pruefe(a.text.find("(Bestwert 65)") != std::string::npos, "die Anzeige nennt den Bestwert");
  // ein zweiter Lauf derselben App (die App-Laufzeit startet sie neu): dieselbe Spur, Zeile für Zeile - und der
  // Bestwert kommt aus dem Stand, den der erste schrieb
  const Lauf b = spiele(spiel, frames, false);
  pruefe(a.erster_text.find("Bestwert 0") != std::string::npos, "der erste Lauf beginnt ohne Bestwert");
  pruefe(b.erster_text.find("Bestwert 65") != std::string::npos, "der Bestwert überlebt den Neustart (der Stand murmel beim Host)");
  pruefe(a.spur == b.spur && a.wechsel == b.wechsel && a.geschafft == b.geschafft, "der zweite Lauf ist derselbe");
  // der Lauf als Text: tests/run.sh vergleicht ihn zwischen den Wegen (Quelle nativ, Interpreter, Klassen)
  if (ergebnis) {
    if (FILE* f = std::fopen(ergebnis, "w")) {
      for (const std::string& z : a.spur) std::fprintf(f, "%s\n", z.c_str());
      std::fprintf(f, "wechsel=%d geschafft=%d punkte=%.0f toene=%u bestwert=%lld\nanzeige=%s\nmitte=%s\nende=%s\nnach_wahl=%d %d\n", a.wechsel, a.geschafft,
                   a.punkte, a.toene, static_cast<long long>(a.bestwert), a.text.c_str(), a.widgets_mitte.c_str(), a.widgets_ende.c_str(), a.nach_wahl_level,
                   a.nach_wahl_fertig ? 1 : 0);
      std::fclose(f);
    } else {
      pruefe(false, "die Datei des Ergebnisses ist zu schreiben");
    }
  }
  std::printf(fehler ? "murmel_test: %d Fehler\n" : "murmel_test: gut\n", fehler);
  return fehler ? 1 : 0;
}
