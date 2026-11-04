// SpawnCity's settings table (see settings.h for the sources of every row).
#include "settings.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace sc {

namespace {
// key, section, label, help, min, max, step, default, toggle, live
constexpr OptSpec kSpecs[] = {
    // mouse_settings.mjs: 20..300 %, default 100 %, invert Y
    {"mouse.sensitivity", "maus", "Maus-Empfindlichkeit", "Drehgeschwindigkeit der Maus in allen Modi (zu Fuss, Drohne, Auto, Jet).", 20, 300, 5, 100, false, true},
    {"mouse.invertY", "maus", "Maus Y invertieren", "Maus nach oben = nach unten schauen (Flugsimulator-Stil).", 0, 1, 1, 0, true, true},
    // control_semantics.mjs DEFAULT_PHONE_SETTINGS (fineness 1..10 = 10, 90 km/h, hover 1.2 m)
    {"drone.leftFineness", "drohne", "Feinheit Hoehe / Drehen", "Kurve von Hoehe und Gieren. Hoeher = feiner um die Mitte, volle Rate erst am Anschlag.", 1, 10, 1, 10, false, true},
    {"drone.rightFineness", "drohne", "Feinheit Nicken / Rollen", "Kurve von Nicken und Rollen. Hoeher fuer ruhiges Schweben und Zielen, niedriger fuer schnelle Manoever.", 1, 10, 1, 10, false, true},
    {"drone.invertYaw", "drohne", "Gieren invertieren", "Vertauscht links/rechts beim Drehen.", 0, 1, 1, 0, true, true},
    {"drone.invertRoll", "drohne", "Rollen invertieren", "Vertauscht links/rechts beim Seitwaertsflug.", 0, 1, 1, 0, true, true},
    {"drone.invertPitch", "drohne", "Nicken invertieren", "Vertauscht vor/zurueck.", 0, 1, 1, 0, true, true},
    {"drone.maxSpeedKmh", "drohne", "Hoechstgeschwindigkeit", "Was der Flugregler im GAME-Modus erlaubt. Langsamer ist leichter zwischen Haeusern.", 5, 90, 1, 90, false, true},
    {"drone.hoverAgl", "drohne", "Schwebehoehe", "Hoehe ueber Grund, auf der die Drohne nach dem Start von selbst haelt.", 0.5f, 10, 0.1f, 1.2f, false, true},
    // first_person_control_settings.mjs
    {"foot.moveFineness", "fuss", "Feinheit Laufen", "Kurve des Lauf-Sticks. Hoeher macht langsames Gehen leicht; Sprint braucht vollen Ausschlag.", 1, 10, 1, 1, false, true},
    {"foot.lookFineness", "fuss", "Feinheit Umschauen", "Kurve des Blick-Sticks. Hoeher macht kleine Zielkorrekturen leicht.", 1, 10, 1, 6, false, true},
    {"foot.lookH", "fuss", "Blick horizontal", "Drehtempo links/rechts fuer Stick, Touch und Maus.", 50, 150, 1, 100, false, true},
    {"foot.lookV", "fuss", "Blick vertikal", "Blicktempo hoch/runter fuer Stick, Touch und Maus.", 50, 150, 1, 100, false, true},
    {"foot.deadzone", "fuss", "Totzone rechter Stick", "Nur Xbox: ignoriert kleines Stick-Driften. Erhoehen, wenn die Sicht von selbst wandert.", 2, 20, 1, 5, false, true},
    {"foot.invertMoveH", "fuss", "Seitwaerts invertieren", "Vertauscht links/rechts beim Seitwaertsgehen.", 0, 1, 1, 0, true, true},
    {"foot.invertLookH", "fuss", "Drehen invertieren", "Vertauscht links/rechts beim Umschauen (Stick, Touch, Maus).", 0, 1, 1, 0, true, true},
    {"foot.invertLookV", "fuss", "Blick Y invertieren", "Flugstil: nach oben druecken = nach unten schauen.", 0, 1, 1, 0, true, true},
    // vehicle_control_settings.mjs: 55..150 %, 45..180 km/h (115), reverse 15..55 (32), camera 3.5..8 m (5.4), 1.35..3.6 m (2.15)
    {"car.steering", "auto", "Lenkung", "Wie stark die Raeder Stick und Tasten folgen. Niedriger ist ruhiger bei Tempo.", 55, 150, 1, 100, false, true},
    {"car.throttle", "auto", "Gas / Bremse", "Wie schnell Gas und Bremse aufbauen. Niedriger gibt sanfteres Anfahren.", 55, 150, 1, 100, false, true},
    {"car.maxKmh", "auto", "Hoechstgeschwindigkeit", "Spitze deines Autos.", 45, 180, 1, 115, false, true},
    {"car.reverseKmh", "auto", "Rueckwaerts", "Spitze im Rueckwaertsgang.", 15, 55, 1, 32, false, true},
    {"car.camDistance", "auto", "Kamera-Abstand", "Wie weit die Verfolgerkamera hinter dem Auto sitzt.", 3.5f, 8, 0.1f, 5.4f, false, true},
    {"car.camHeight", "auto", "Kamera-Hoehe", "Wie hoch die Verfolgerkamera ueber dem Auto sitzt.", 1.35f, 3.6f, 0.05f, 2.15f, false, true},
    // camera_settings.mjs: tilt -15..50 (-15), fov 50..120 (105), third 1.5..6 (1.5); stereo_sbs.mjs
    {"cam.fpvTilt", "kamera", "FPV-Neigung", "Neigung der FPV-Kamera. Hoch (20-40 Grad) fuer schnelles Fliegen, runter fuer den Boden.", -15, 50, 1, -15, false, true},
    {"cam.fov", "kamera", "Sichtfeld", "Sichtfeld der Drohnenkamera. Weit = Ueberblick und Tempogefuehl, eng = Zoom.", 50, 120, 1, 105, false, true},
    {"cam.thirdDistance", "kamera", "Third-Person-Abstand", "Abstand der Third-Person-Kamera der Drohne.", 1.5f, 6, 0.1f, 1.5f, false, true},
    {"cam.stereo", "kamera", "3D nebeneinander (Schielen)", "Teilt das Bild fuer Kreuzblick-3D: schielen, bis beide Haelften zu einem scharfen Bild verschmelzen.", 0, 1, 1, 0, true, true},
    // audio_settings.mjs DEFAULT_AUDIO_SETTINGS (mix v5)
    {"audio.on", "ton", "Ton", "Hauptschalter fuer alle Effekte.", 0, 1, 1, 1, true, true},
    {"audio.drone", "ton", "Drohne", "Motorgeraeusch der Drohne.", 0, 100, 1, 50, false, true},
    {"audio.shots", "ton", "Schuesse", "Schuesse und Einschlaege.", 0, 100, 1, 82, false, true},
    {"audio.fx", "ton", "Effekte", "Explosionen und andere Effekte.", 0, 100, 1, 72, false, true},
    {"audio.steps", "ton", "Schritte", "Schritte, deine und die der anderen.", 0, 100, 1, 30, false, true},
    {"audio.vehicle", "ton", "Fahrzeuge", "Motoren und Reifen.", 0, 100, 1, 58, false, true},
    {"audio.ambient", "ton", "Umgebung", "Stadtgeraeusche und alles andere.", 0, 100, 1, 46, false, true},
    {"music.on", "ton", "Musik", "Musik im Menue und Autoradio an oder aus.", 0, 1, 1, 1, true, true},
    {"music.volume", "ton", "Musik-Lautstaerke", "Lautstaerke von Menuemusik und Autoradio.", 0, 100, 1, 32, false, true},
    // world_options.mjs WORLD_OPTIONS (everything on, friendly fire off)
    {"world.people", "welt", "Leute", "Passanten, die zu ihren eigenen Zielen laufen.", 0, 1, 1, 1, true, true},
    {"world.traffic", "welt", "Verkehr", "Autos und Busse auf den Strassen.", 0, 1, 1, 1, true, true},
    {"world.dogs", "welt", "Hunde", "Hunde, die durch das Viertel streifen.", 0, 1, 1, 1, true, false},
    {"world.cats", "welt", "Katzen", "Katzen, auch die schwarze.", 0, 1, 1, 1, true, false},
    {"world.birds", "welt", "Voegel", "Schwaerme am Himmel.", 0, 1, 1, 1, true, false},
    {"world.zombies", "welt", "Zombies", "Zombienaechte nach Einbruch der Dunkelheit.", 0, 1, 1, 1, true, true},
    {"world.police", "welt", "Polizei", "Polizeidrohnen und Beamte, wenn du gesucht wirst.", 0, 1, 1, 1, true, true},
    {"world.military", "welt", "Militaer", "Soldaten, Panzer und Hubschrauber bei hoher Fahndung.", 0, 1, 1, 1, true, true},
    {"world.flak", "welt", "Flak", "Ab und zu ein Flakgeschuetz, das auf Drohne und Jet feuert.", 0, 1, 1, 1, true, true},
    {"world.jetpack", "welt", "Jetpack", "Zweimal springen fuer einen kurzen Jetpack-Schub ueber Haeuser. Laedt sich am Boden auf.", 0, 1, 1, 1, true, true},
    {"world.friendlyFire", "welt", "Friendly Fire", "Spielerwaffen treffen andere Spieler.", 0, 1, 1, 0, true, true},
    // first_person_control_settings.mjs aimAssistStrengthPercent 0..100 (55); settings_help.mjs view rows
    {"foot.aimAssist", "fuss", "Zielhilfe", "Sanfter Zug zu Zielen nahe am Fadenkreuz mit Xbox-Pad und Touch. 0 schaltet sie aus.", 0, 100, 1, 55, false, true},
    {"view.grid", "kamera", "1-m-Raster", "Zeichnet ein Referenzraster von 1 m. Nur Anzeige; hilft, Abstand und Hoehe zu schaetzen.", 0, 1, 1, 0, true, true},
    {"view.keepLook", "kamera", "Blick halten", "AUS: der Blick schwingt nach vorn zurueck, wenn du loslaesst. AN: er bleibt, wo du ihn gelassen hast.", 0, 1, 1, 0, true, true},
    {"view.minimapNorth", "kamera", "Minimap nach Norden", "Haelt die Minimap genordet, statt sie mit dir zu drehen.", 0, 1, 1, 0, true, true},
    {"debug.colliders", "kamera", "Box3D-Kollision zeigen", "Zeigt die Box3D-Kollisionsformen als Drahtgitter. Nur Anzeige, kostet Bildrate.", 0, 1, 1, 0, true, true},
};
static_assert(sizeof kSpecs / sizeof kSpecs[0] == static_cast<u32>(Opt::count), "one row per option");

void escape_into(std::string& out, const char* s) {
  for (; *s; ++s) {
    if (*s == '"' || *s == '\\') { out += '\\'; out += *s; }
    else if (static_cast<unsigned char>(*s) < 0x20) out += ' ';
    else out += *s;
  }
}
std::string num(f32 v) {
  char b[32];
  std::snprintf(b, sizeof b, "%g", static_cast<double>(v));
  return b;
}
}  // namespace

const OptSpec& opt_spec(Opt o) { return kSpecs[static_cast<u32>(o)]; }

bool find_opt(std::string_view key, Opt& out) {
  for (u32 i = 0; i < static_cast<u32>(Opt::count); ++i)
    if (key == kSpecs[i].key) { out = static_cast<Opt>(i); return true; }
  return false;
}

Settings::Settings() {
  for (u32 i = 0; i < static_cast<u32>(Opt::count); ++i) v_[i] = kSpecs[i].def;
}

void Settings::set(Opt o, f32 value) {
  const OptSpec& s = opt_spec(o);
  if (!std::isfinite(value)) return;
  f32 v = clampf(value, s.min, s.max);
  if (s.toggle) v = v > 0.5f ? 1.0f : 0.0f;
  else if (s.step > 0) v = clampf(s.min + std::round((v - s.min) / s.step) * s.step, s.min, s.max);
  if (v != v_[static_cast<u32>(o)]) { v_[static_cast<u32>(o)] = v; ++version_; }
}

bool Settings::set(std::string_view key, std::string_view value) {
  Opt o;
  if (!find_opt(key, o)) return false;
  if (value == "true" || value == "on") { set(o, 1); return true; }
  if (value == "false" || value == "off") { set(o, 0); return true; }
  const std::string text(value);
  char* end = nullptr;
  const double d = std::strtod(text.c_str(), &end);
  if (end == text.c_str()) return false;
  set(o, static_cast<f32>(d));
  return true;
}

void Settings::reset_section(std::string_view section) {
  for (u32 i = 0; i < static_cast<u32>(Opt::count); ++i)
    if (section.empty() || section == kSpecs[i].section) set(static_cast<Opt>(i), kSpecs[i].def);
}

std::string Settings::values_json() const {
  std::string o = "{";
  for (u32 i = 0; i < static_cast<u32>(Opt::count); ++i) {
    if (i) o += ',';
    o += '"';
    o += kSpecs[i].key;
    o += "\":";
    o += num(v_[i]);
  }
  return o + "}";
}

std::string Settings::schema_json() const {
  std::string o = "[";
  for (u32 i = 0; i < static_cast<u32>(Opt::count); ++i) {
    const OptSpec& s = kSpecs[i];
    if (i) o += ',';
    o += "{\"key\":\"";
    escape_into(o, s.key);
    o += "\",\"section\":\"";
    escape_into(o, s.section);
    o += "\",\"label\":\"";
    escape_into(o, s.label);
    o += "\",\"help\":\"";
    escape_into(o, s.help);
    o += "\",\"min\":" + num(s.min) + ",\"max\":" + num(s.max) + ",\"step\":" + num(s.step) + ",\"def\":" + num(s.def);
    o += std::string(",\"toggle\":") + (s.toggle ? "true" : "false") + ",\"live\":" + (s.live ? "true" : "false") + ",\"value\":" + num(v_[i]) + "}";
  }
  return o + "]";
}

f32 fineness_axis(f32 x, f32 fineness) {
  x = clampf(x, -1, 1);
  const f32 level = clampf(std::round(fineness), 1, 10);
  const f32 e = 0.70f * ((level - 1) / 9) * 0.38f;  // MAX_PHONE_EXPO .70, PHONE_EXPO_RESPONSE_SCALE .38
  return clampf(x * (1 - e) + x * x * x * e, -1, 1);
}

}  // namespace sc
