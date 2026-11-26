# Schaden messen: Vergleich gegen die ECHTE Quelle (Agent projekt, 26.11.; erste Zahlen, unvollständig)

Zweiter Durchgang des Testrahmens: `KERN_TEST_SOURCE_NUMBERS=1` beim Konverter (tools/cpp_to_blueprint.py, TestWriter):
Die Test-Kopie der Quelle wird nicht umgeschrieben (`widen` gibt den Text, wie er ist: f32 bleibt f32, 1.0f bleibt), und das
Vorspiel nennt die Typen der Engine (kern::f32, kern::Vec3 …) statt converted::wide. Ohne die Variable bleibt alles wie bisher
(erster Durchgang unverändert). Lokal: Fall-Lauf $S/case3.sh, Bau und Lauf der Vergleiche $S/vt.sh / $S/vtm.sh (kq-Objekte).

## Fall $S/typcase/typen.{h,cpp} (4 Funktionen, alle geschrieben)
- erster Durchgang (Zahlen der Sprache): 11 checks, 0 failures (grün).
- zweiter Durchgang (echte Quelle): **2 von 4 weichen ab**
  - past_one_after (f32-Summe 1/60 wie Murmels gewonnen_seit_): 1 von 300 Aufrufen; die Quelle false, Blueprint true (Interpreter und Klasse).
  - product_is_exact (f32-Zwischenwert a*b): 175 von 300 Aufrufen, z. B. (0.0969…, −1.21): Quelle false, Blueprint true.
  - hash32 (U32, mul32) und lcg (Int64): gleich.

## Murmel (src/murmel.cpp + main.cpp)
- Der zweite Durchgang ist noch NICHT messfähig: Ohne widen kompiliert der Zustand State_Murmel in der Test-Kopie nicht
  („unknown type name 'State_Murmel'“, „use of undeclared identifier 'draw_state_Murmel'“, steuerung: „no matching function for
  call to 'defined'“). Dadurch werden nur 2 von 13 verglichen (level_pfad, anzeige: beide gleich). Weil das Schreiben vom
  Vergleich abhängt, warten in diesem Durchgang zudem Funktionen (Murmel::update „it calls steuerung(), which waits“).
  Nächster Schritt: die Ursache im Zustand finden (TestWriter, Zeile ~15931: den Text mit KEEP ausgeben und das erste Compiler-Problem
  lesen; vermutlich erwarten der State-Kleber oder `converted::defined/arg` die wide-Typen (Num32, double) für f32-Felder).
  Danach Murmel messen (erwartet: Murmel::update wegen gewonnen_seit_).
- Wichtig für den Bau: Der zweite Durchgang darf das SCHREIBEN nicht ändern (heute: was der Test nicht vergleicht, wartet). Für die
  Messung braucht es eine Trennung: schreiben wie im ersten Durchgang, im zweiten nur vergleichen und zählen.

## SpawnCity-Probe (game.cpp+main.cpp, car.cpp/drone.cpp)
- Noch nicht gefahren: Sie braucht zuerst die zwei Punkte oben (Zustände kompilieren, Schreiben unabhängig). Der Job `probe` müsste dazu die
  Variable weiterreichen (ci.yml: `KERN_TEST_SOURCE_NUMBERS` als Eingabe der Probe).
