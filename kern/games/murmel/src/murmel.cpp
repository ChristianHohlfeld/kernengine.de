// Murmel: das Programm (siehe murmel.h).
#include "murmel.h"

#include <cstdio>

#include "kern/core/base.h"
#include "kern/game/embedded.h"

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

std::string anzeige(int level, f32 punkte, u32 muenzen, u32 alle, bool fertig, const std::string& meldung) {
  char zeile[160];
  if (fertig) {
    std::snprintf(zeile, sizeof zeile, "Geschafft! %d Punkte - Enter: noch einmal", static_cast<int>(punkte));
    return zeile;
  }
  std::snprintf(zeile, sizeof zeile, "Level %d/%d   Punkte %d   Muenzen %u/%u", level + 1, kLevels, static_cast<int>(punkte), muenzen, alle);
  std::string text = zeile;
  if (!meldung.empty()) text += "\n" + meldung;
  return text;
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
  welt_ = std::make_unique<game::GameWorld>(engine.renderer());
  bank_ = std::make_unique<game::SoundBank>(engine.mixer());
  audio_ = std::make_unique<game::GameAudio>(engine.mixer(), *bank_);
  kamera_ = engine.renderer().camera();
  kamera_.fov = 60 * kPi / 180;
  level_ = 0;
  vorher_ = 0;
  fertig_ = false;
  toene_ = 0;
  return lade(0);
}

bool Murmel::lade(int nummer) {
  if (welt_->running()) {
    audio_->end();
    welt_->stop();
  }
  const std::string_view text = game::embedded_level(level_pfad(nummer));
  std::string fehler;
  // (der Grund mit kern::fail: startet die App deshalb nicht, nennt die App-Laufzeit ihn - kern/docs/app.md 1.)
  if (!game::from_text(text, geladen_, &fehler)) return fail("murmel: das Level %s: %s", level_pfad(nummer), fehler.c_str());
  level_ = nummer;
  gewonnen_seit_ = -1;
  welt_->sync(geladen_);
  if (!welt_->start(geladen_, game::SimMode::play)) return fail("murmel: das Level %s startet nicht", level_pfad(nummer));
  bank_->request_level(geladen_);
  audio_->begin(*welt_);
  return true;
}

void Murmel::update(Engine& engine, const Frame& frame) {
  const game::PlayInput in = steuerung(engine);
  if (fertig_) {
    if (engine.action_pressed("again")) {
      vorher_ = 0;
      fertig_ = false;
      if (!lade(0)) log(LogLevel::error, "%s", last_error());
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
  text_ = anzeige(level_, punkte(), hud.pickups, hud.pickups_total, fertig_, hud.message_age < 3.0f ? hud.message : std::string());
  engine.set_status(text_.c_str());
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
