# Koordinator – Mitteilungen an alle (neueste oben)
- 19:50 **Stand:**
  · Voller Lauf der Zusammenführung `work-zusammen-voll-185405-1226` (auf 9931aea): **Konvertierung in 27 min** (15 Runden, 37 Kerne) – geschrieben
    **1.660 / 1.878 Funktionen (88,4 %), 18.926 / 31.103 Zeilen (60,8 %)** (f9: 1.577 / 1.876, 16.136 / 31.096); 101 Funktionen „its test cannot compare it yet“,
    9 Rückfälle „was written and waits now“. **Engine-Suite grün (rc=0)** auf dem zusammengeführten Baum. **Aber `convert rc=1`:** der Leser weist das Level ab –
    `Game.cpp: error: the input "pad" of Controls_DeviceInput has the name of a variable or a function` (controls.cpp ist ein Teil von Game und kennt Games Variable
    `pad` nicht). Spiel-Suite darum wertlos (altes Level, rc=1).
  · **Behoben im Konverter (work-zusammen c432166, allgemein):** die Erklärung einer Funktion eines Actor-Blueprints (KERN_EXTERN) wird für einen Blueprint aus
    mehreren Teilen beim Zusammensetzen festgelegt – kein Eingang heißt wie etwas des ganzen Blueprints, in jedem Teil gleich (`settled_extern`).
  · Neu abgelegt: Probe `work-zusammen-probe-194959-848` (controls + game + game_controls, mit Level), voller Lauf **`work-zusammen-voll-195003-886`**.
    lauf2.sh 9d621c4: ein voller Lauf legt jetzt auch die Texte der Blueprints nach `mag-zwischen/texts`.
  · t1-/t2-Proben über alle Quelltexte sind fertig (`work-t1-probe-174058-4177`, `work-t2-probe-163129-9216`, beide rc=0, beide vor 18:04 gestartet: ohne Level).
- 18:55 **Stand:**
  · **Lesefehler bestätigt auf `work`** (Probe `work-probe-182522-630` auf 90b5ab8: net_session_cpp 52/67, „the number 78 names two nodes“, nicht gelesen) – der
    Basislauf `work-voll-162644-5528` endet also ohne gültiges Level. Auf work-f2 liest es (`work-f2-probe-182528-670`: net_session_cpp 67/67, rc=0).
  · **work-d ist in `work` gemergt** (4283186, Baum = 5367e14): Engine-Suite auf mag zweimal grün (`work-d-engine-174905-11207`, `-174911-11261`), CI 44351625418
    grün (native, build, determinism, input, e2e), Generator ohne Abweichung (818 Knoten). F1 ist damit behoben.
  · **k: 8 gegen 40 Kerne byteweise gleich** (tests/converted, 145 Dateien) auf e5f2bf1 und auf 3dd3338; alle vier Läufe `convert rc=1` nur wegen des Lesefehlers
    (das Level blieb, wie es war – die Gleichheit des Levels sagt darum nichts).
  · **Probe-Zusammenführung `work-zusammen` (9931aea)** = work + k + t1 + t2 + f2 + g2 + g1 + von f1 nur der Konverter; die fünf Absenkungen der Game-Dateien
    (g1 7d1d656; g2 9ce8133, 1380806, ef21598, ffa277f) sind darin zurückgenommen, die Absenkungen/Tests/Werkzeug/Engine-Header von f1 nicht dabei. Doppelt Gebautes
    auf je eine Fassung: `scope_for` (f2) statt `looked_from` (g1); `case_block` (g2, Merker je Fall) statt Merker um den ganzen switch (g1); `stood` (t1) statt
    `where_of` (f1). Voller Lauf darauf: **`work-zusammen-voll-185405-1226`** (neue Fassung: Level und Tests gleich nach der Konvertierung).
    Ist er gültig und grün, ist er der Beweis für genau diesen Baum → nach `work`.
  · mag: `work-s5-engine-163459-12172` rc=0 (auf e749453 – R129 hing also wirklich vom Ablauf ab); `work-f1-spiel-174450-8421` rc=123 (Differenztests von f1
    übersetzen noch nicht); g1-/g2-Proben über alle game*.cpp fertig (rc=0, vor 18:04: ohne Level).
- 18:27 **BEFUND (aus den zwei 40-Kern-Läufen von k, e5f2bf1 und 3dd3338, beide `convert rc=1`):** das Einlesen ins Level scheitert –
  `net_session_cpp.cpp: line 611, column 20: the number 78 names two nodes` → „the Blueprints were not read: the level is as it was“. Beide Läufe gleich
  (14 Runden, 41–45 min bei 40 Kernen). Steckt das auch in `work` (90b5ab8), endet der Basislauf `work-voll-162644-5528` ebenso ohne gültiges Level.
  Zur Klärung abgelegt (Probe mit Level und Texten): `work-probe-182522-630` auf origin/work, `work-f2-probe-182528-670` auf origin/work-f2 (je net_session + jet_seats + mp_rules).
  Dazu 8 Zeilen „was written and waits now“: Game/game_look_cpp ×7 (look_begin, look_blast, look_camera, look_apply_tier – „invalid use of incomplete type Renderer“ –,
  look_lap, look_people_budget, view_zoom), Game/game_lamps_cpp gaps_lamps_draw („‘Game’ has not been declared“) – BEFUND FÜR t2.
- 18:24 CI 44351625418 auf work-d 5367e14 (Fix F1 + Zwischenstand): build, determinism, native, input, e2e grün (native-spawncity lief noch); mag
  `work-d-engine-174905-11207` lief noch. Probe mit Level funktioniert: `work-probe-180405-238` rc=0.
- 18:10: Proben auf mag haben seit **18:04** das Level (kein „which the level does not have“-Artefakt mehr); volle Läufe liefern Level und Tests gleich nach der
  Konvertierung (`mag-zwischen/`). Einzelheiten: PAKET_COMMON3.md, „Nachtrag 18:10“.
- 17:50: alle Agenten gestoppt (Unterbrechung der Sitzung); Neustart nur auf Chris' Wort.
- Nichts geht auf `work`, bevor das Ergebnis von `work-voll-162644-5528` (läuft seit 17:38:59 auf 90b5ab8) übernommen ist.
