// Murmel: ein kleines Geschicklichkeitsspiel auf Kern - der zweite Maßstab der Engine neben SpawnCity.
// Eine Kugel rollt über zwei Levels, sammelt Münzen und rollt ins Ziel; danach kommt das nächste Level, der
// Punktestand bleibt. Die Welt (Physik, Münzen, Ziel, Töne) ist die GameWorld der Engine; das Programm ist die App:
// es lädt die Levels des Projekts, macht aus der Tastatur die Eingabe der Welt, zeigt den Stand als Text und wechselt
// das Level. Ihre Töne spielt die Welt auf dem Mixer der Engine (engine.mixer()).
#pragma once
#include <memory>
#include <string>

#include "kern/app/app.h"
#include "kern/game/level.h"
#include "kern/game/runtime.h"
#include "kern/game/sound.h"

namespace murmel {

/// Die Levels des Spiels, in ihrer Reihenfolge (wie das Projekt sie nennt).
constexpr int kLevels = 2;
/// Der Pfad des Levels `nummer` (0, 1) im Projekt.
const char* level_pfad(int nummer);

/// Was die Eingabe eines Frames für die Welt heißt: die Achsen moveX, moveY rollen, die Aktion jump springt - welche
/// Tasten, Knöpfe und Finger das sind, sagt das Projekt (settings.input, kern/docs/app.md 4.).
kern::game::PlayInput steuerung(const kern::Engine& engine);

/// Der Text der Anzeige: Level, Punkte, Münzen und was gerade los ist.
std::string anzeige(int level, kern::f32 punkte, kern::u32 muenzen, kern::u32 alle, bool fertig, const std::string& meldung, kern::i64 bestwert);

/// Das Spiel: eine App der Engine.
class Murmel final : public kern::App {
 public:
  bool start(kern::Engine& engine) override;
  void update(kern::Engine& engine, const kern::Frame& frame) override;
  void stop(kern::Engine& engine) override;

  /// Das laufende Level (0, 1), die Punkte aller fertigen Levels und des laufenden, ob alles geschafft ist.
  int level() const { return level_; }
  kern::f32 punkte() const;
  bool fertig() const { return fertig_; }
  /// Wie viele Töne die Welt bisher verlangt hat.
  kern::u32 toene() const { return toene_; }
  const std::string& text() const { return text_; }
  /// Der Bestwert: gespeichert im Stand "murmel" unter "best" (kern/docs/app.md 5.), über einen Neustart hinaus.
  kern::i64 bestwert() const { return bestwert_; }
  /// Die Welt des laufenden Levels (zum Ansehen, etwa im Test).
  const kern::game::GameWorld* welt() const { return welt_.get(); }

 private:
  /// Startet das Level `nummer`: die laufende Welt endet, die nächste beginnt.
  bool lade(int nummer);

  kern::Renderer* renderer_ = nullptr;
  std::unique_ptr<kern::game::GameWorld> welt_;
  kern::game::Level geladen_;
  // (die Töne spielen auf dem Mixer der Engine: die App-Laufzeit macht ihn hörbar - kern/docs/app.md 3.)
  std::unique_ptr<kern::game::SoundBank> bank_;
  std::unique_ptr<kern::game::GameAudio> audio_;
  kern::Camera kamera_;
  int level_ = 0;
  kern::f32 vorher_ = 0;   // die Punkte der fertigen Levels
  bool fertig_ = false;
  kern::f32 gewonnen_seit_ = -1;   // Sekunden seit dem Ziel (< 0: noch nicht)
  kern::u32 toene_ = 0;
  kern::i64 bestwert_ = 0;
  std::string text_;
};

/// Das eine Objekt des Spiels (der Einstieg gibt es der Engine).
Murmel& spiel();

}  // namespace murmel
