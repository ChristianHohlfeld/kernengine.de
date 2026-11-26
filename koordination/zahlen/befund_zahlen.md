# Befund Zahlen: f32 der Quelle gegen 64-Bit-Float der Sprache (Murmel, Agent projekt, 26.11.)

## Was abweicht
Murmels App als Programm-Blueprint (aus src/murmel.cpp konvertiert, 14/14 geschrieben, 13 verglichen), nativ gespielt
(murmel_test, Interpreter UND Klassen gleich): Level-Wechsel in Frame **348** statt 349, geschafft in Frame **770** statt 772.
Punkte 65, Töne 11, Bestwert, Widgets, Menü und die Gleichheit des zweiten Laufs bleiben gleich. tests/erwartet.txt ist NICHT geändert.

## Ursache (eine Zeile)
kern/games/murmel/src/murmel.cpp, Murmel::update (Z. 91-94):
    if (gewonnen_seit_ < 0) gewonnen_seit_ = 0;
    gewonnen_seit_ += frame.dt;          // frame.dt ist f32 (kern/app/app.h Frame::dt), 1/60
    if (gewonnen_seit_ > 1.0f) { ... nächstes Level / fertig ... }
gewonnen_seit_ ist `kern::f32` (murmel.h Z. 61). Als f32 summiert ist 1/60 erst nach **61** Frames > 1, als Float der Sprache
(64 Bit) schon nach **60**: 60 × f32(1/60) = 60 × 0,01666666753590107 = 1,00000005 > 1. Zwei Ziele (zwei Level) = 1 bzw. 2 Frames.

## f32/float in Zustand oder Rechnung der Quelle
- murmel.h 59: `kern::f32 vorher_` (Punkte der fertigen Levels) - Summe ganzer Punkte, in f32 exakt: ohne Wirkung.
- murmel.h 61: `kern::f32 gewonnen_seit_` - DIE Stelle (Summe von dt, Schwelle 1.0f in murmel.cpp 94).
- murmel.h 27/38, murmel.cpp 26/41: `f32 punkte` (anzeige-Parameter, Murmel::punkte()) - ganze Zahlen: ohne Wirkung.
- murmel.cpp 81, 125, 126: f32-Literale für ui_menu/ui_bar/ui_text-Lagen, `static_cast<f32>(pickups)/...` (Anteil für den Balken),
  `hud.message_age < 3.0f` - Werte für die Engine-Aufrufe (Parameter f32 der Engine) bzw. ein Vergleich mit einem Wert der
  Engine: ohne Wirkung auf die gemessenen Zahlen.
- Werte der Engine (f32: Frame::dt, Hud.score, message_age) sind Engine-Typen, nicht der Quelle.

## Gemessen (nur gemessen, nichts eingecheckt; $S/m64)
Alter nativer Bau (C++-App aus der Quelle, murmel_test vom Stand HEAD, Interpreter-Stufe der Level-Blueprints):
- Quelle wie eingecheckt (f32):                                  Wechsel 349, geschafft 772, 65 Punkte, 11 Töne - gut.
- Quelle mit f64 für vorher_, gewonnen_seit_, punkte, anzeige, Schwelle 1.0: Wechsel 348, geschafft 770, 65, 11.
- Nur gewonnen_seit_ f64 und Schwelle 1.0 (Rest f32):             Wechsel 348, geschafft 770, 65, 11.
=> Nutzt die Quelle den Float der Sprache (64 Bit), ergeben nativ und Blueprint DASSELBE (348/770). Die Abweichung ist allein
   gewonnen_seit_ (f32-Summe gegen 64-Bit-Summe).

## Stand der Arbeit
Murmel-Teil (App als Programm-Blueprint) liegt als Patch $S/murmel_programm.patch (git diff gegen cc4c2673, work-projekt),
im Worktree /home/claude/w-einstieg2 uncommittet. Der Zweig work-murmel-programm wurde NICHT angelegt: die Rechteprüfung hat
das Anlegen/Pushen eines neuen Zweigs verweigert (der Auftrag erlaubt nur work-projekt).
