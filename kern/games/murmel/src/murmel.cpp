// Murmel: das Programm (siehe murmel.h).
#include "murmel.h"

#include <cstdio>

#include "kern/core/base.h"

namespace murmel {

using namespace kern;

const char* level_pfad(int nummer) {
  if (nummer == 0) return "levels/wiese.klevel";
  return "levels/bruecke.klevel";
}

game::PlayInput steuerung(const Engine& engine) {
  game::PlayInput in;
  in.move_x = engine.axis("moveX");
  in.move_y = engine.axis("moveY");
  in.jump = engine.action("jump");
  return in;
}

std::string anzeige(int level, f32 punkte, bool fertig, i64 bestwert) {
  char zeile[200];
  if (fertig) {
    std::snprintf(zeile, sizeof zeile, "Geschafft! %d Punkte (Bestwert %lld)", static_cast<int>(punkte), static_cast<long long>(bestwert));
    return zeile;
  }
  std::snprintf(zeile, sizeof zeile, "Level %d/%d   Punkte %d   Bestwert %lld", level + 1, kLevels, static_cast<int>(punkte), static_cast<long long>(bestwert));
  return zeile;
}

Murmel& spiel() {
  static Murmel s;
  return s;
}

f32 Murmel::punkte() const {
  if (!welt_ || fertig_) return vorher_;   // (fertig: die Punkte des letzten Levels sind schon in vorher_)
  return vorher_ + welt_->hud().score;
}

bool Murmel::start(Engine& engine) {
  renderer_ = &engine.renderer();
  welt_ = std::make_unique<game::GameWorld>(engine);   // (eine Welt des Programms: sie hat sein Projekt, also seine Levels)
  bank_ = std::make_unique<game::SoundBank>(engine.mixer());
  audio_ = std::make_unique<game::GameAudio>(engine.mixer(), *bank_);
  kamera_ = engine.renderer().camera();
  kamera_.fov = 60 * kPi / 180;
  level_ = 0;
  vorher_ = 0;
  fertig_ = false;
  toene_ = 0;
  clear_error();
  bestwert_ = engine.load_int("murmel", "best", 0);   // (keiner: 0; ein Stand, der nicht lesbar ist: 0 und der Grund im Log)
  if (*last_error()) log(LogLevel::warn, "%s", last_error());
  return lade(0);
}

bool Murmel::lade(int nummer) {
  if (welt_->running()) {
    audio_->end();
    welt_->stop();
  }
  level_ = nummer;
  gewonnen_seit_ = -1;
  // (das Level beim Pfad im Projekt: die Welt hat ihr Projekt - GameWorld::start_level sagt Ort und Grund, wenn nicht;
  // startet die App deshalb nicht, nennt die App-Laufzeit den Grund - kern/docs/app.md 1.)
  if (!welt_->start_level(level_pfad(nummer), game::SimMode::play)) return false;
  audio_->begin(*welt_);   // (die Töne der Welt: GameAudio lädt die Klänge ihres Levels)
  return true;
}

void Murmel::update(Engine& engine, const Frame& frame) {
  const game::PlayInput in = steuerung(engine);
  if (fertig_) {
    // das Menü am Ende (kern/docs/app.md 6.: gewählt im Frame davor) - oder die Aktion again
    const i32 wahl = engine.ui_menu("weiter", "Noch einmal\nNur Level 2", 0.5f, 0.6f);
    if (wahl >= 0 || engine.action_pressed("again")) {
      vorher_ = 0;
      fertig_ = false;
      if (!lade(wahl == 1 ? 1 : 0)) log(LogLevel::error, "%s", last_error());
    }
  } else {
    welt_->step(frame.dt, in);
    const game::Hud& hud = welt_->hud();
    if (hud.status == game::GameStatus::won) {
      if (gewonnen_seit_ < 0) gewonnen_seit_ = 0;
      gewonnen_seit_ += frame.dt;
      // (einen Augenblick im Ziel, dann das nächste Level - die Punkte bleiben)
      if (gewonnen_seit_ > 1.0f) {
        vorher_ += hud.score;
        if (level_ + 1 < kLevels) {
          if (!lade(level_ + 1)) log(LogLevel::error, "%s", last_error());
        } else {
          fertig_ = true;
          // der Bestwert bleibt über einen Neustart (der Stand "murmel" beim Host: Seite oder Datei)
          if (static_cast<i64>(vorher_) > bestwert_) {
            bestwert_ = static_cast<i64>(vorher_);
            if (!engine.save_int("murmel", "best", bestwert_)) log(LogLevel::error, "%s", last_error());
          }
        }
      }
    } else if (hud.status == game::GameStatus::lost) {
      if (!lade(level_)) log(LogLevel::error, "%s", last_error());
    }
  }
  welt_->update_camera(kamera_, frame.dt, in);
  engine.renderer().set_camera(kamera_);
  welt_->apply_lighting(kamera_.eye);
  welt_->present(frame.time * 1000.0, kamera_.eye);
  toene_ += static_cast<u32>(welt_->sound_cues().size());
  bank_->prepare(4);
  audio_->update(*welt_, kamera_, frame.dt);
  const game::Hud& hud = welt_->hud();
  // die Anzeige: der Stand oben links, die Münzen als Balken oben rechts, eine Meldung der Welt in der Mitte
  text_ = anzeige(level_, punkte(), fertig_, bestwert_);
  engine.ui_text("stand", text_.c_str(), 0, 0);
  if (!fertig_) {
    char muenzen[64];
    std::snprintf(muenzen, sizeof muenzen, "Muenzen %u/%u", hud.pickups, hud.pickups_total);
    engine.ui_bar("muenzen", muenzen, hud.pickups_total ? static_cast<f32>(hud.pickups) / static_cast<f32>(hud.pickups_total) : 0.0f, 1, 0, 0.25f);
    if (hud.message_age < 3.0f && !hud.message.empty()) engine.ui_text("meldung", hud.message.c_str(), 0.5f, 0.2f);
  }
}

void Murmel::stop(Engine& engine) {
  if (welt_ && welt_->running()) {
    audio_->end();
    welt_->stop();
  }
  audio_.reset();
  bank_.reset();
  welt_.reset();
}

}  // namespace murmel
