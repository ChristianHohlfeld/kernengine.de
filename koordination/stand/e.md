# Paket e – Rekursion (Stand 19.11.)

Zweig work-e = work-zusammen 821208ac, noch nichts gebaut. Messprogramme: $S/e/ (mess_rekursion.cpp: Interpreter nativ -O2, Prüfung im Leser
für die Messung abgeschaltet; zyklen.py: grobe Aufrufzyklen der Game-Methoden; klasse_fib.cpp: Klasse heute gegen C++).

## ENTSCHEIDUNG NÖTIG: Entwurf Rekursion (wartet auf Ja des Koordinators)

**Messungen**
- Interpreter (nativ, -O2): **1.504 B Maschinenstapel je Aufruf** (kleinste Funktion). 8 MiB → Absturz zwischen 5.000 und 10.000 Ebenen (gerechnet ~5.500).
- C++ des Spiels (-fstack-usage, -O2): explosion 248 B + combat_blast 152 B = **~400 B je Ebene** → nativ 8 MiB ≈ 20.000 Ebenen, WASM (build.sh:153
  `-z stack-size=1048576`) ≈ 2.600. Der Interpreter käme in WASM auf ≈ 350 Ebenen (≈ 7× weniger als das C++). JIT: nicht lokal messbar (kein wasi-sdk);
  er geht heute für jeden Aufruf durch dieselben C++-Rahmen (func.call → call_function → run_from → run_compiled → neue wasm-Ausführung), plus V8-Stapel.
- **Heute ist kein Stufe je Aufruf getrennt**: Fib(10) mit abgeschalteter Prüfung → Interpreter −80, Klasse 5, C++ 55 (Fib(5): −15 / 3 / 5).
  Interpreter: Parameter (Ausgänge des Entry), Ergebnisse von Aufrufen und Zustände aller Knoten liegen je Instanz; Klasse: `n24 = Fib(…)` ist ein Mitglied.
- Spiel: echte Zyklen nur {explosion, combat_blast} (Tiefe ≤ 1 + Zahl der Panzer, die nicht wreck sind – das Flag fällt vor dem inneren Aufruf; die Liste
  wird nie kleiner, also KEINE feste Schranke beweisbar) und {command, ui_command, ctl_discrete, …} (21 Funktionen, command ruft sich mit festen Texten).
  wreck_car/blast_cars sind KEIN Zyklus (damage_car setzt nur den Brand; wreck_car ruft update_town).

Entwurf und Begründung: siehe Bericht an main (Handback), gleichlautend unten nach dem Ja.

## E16: JA (07:35). Schritt 1 – neue Form der Klasse
- a020e227 (gepusht): Shape::locals – Schrittwert/Schleifenzustand einer Funktion lokal in der Methode, wenn alle Leser auf dem Weg der Funktion
  nach dem Schritt stehen (kein Weg vom Entry am Schritt vorbei); sonst Member. Leser: lokale Deklarationen im Rumpf; Member-Form weiter
  angenommen (Konverter-Texte, alte Texte = derselbe Blueprint); Deklaration im falschen Rumpf abgewiesen. Engine-Fall
  logic_notation_values_of_a_call; Doku „Werte eines Aufrufs“. Lokal: alle 43 logic_cases + Fib-Graph Rundreise = Fixpunkt.
- Belege laufen: mag work-e-engine-074752-4918; Bench vorher/nachher lokal ($S/e/bench/base.txt, neu.txt). Danach vergleich car/drone/city_stream.
- 08:10: Engine-Auftrag neu: work-e-engine-080904-22468 (074752 war verloren). Level der Grundlage: das .klevel hält Graphen (JSON), keine
  Klassentexte – die Form ist dort nicht gespeichert; blueprint_cpp meines Stands schrieb alle Klassen aus dem alten Level ($S/p/lvl) ohne Fehler
  (Bench). Konverter-Texte (Member-Form) liest der Leser weiter. Kein konvert nötig.
- Bench (29 Fälle, 9 Runden, je beide Stände nacheinander unter beiden Sperren; $S/e/bench/base.txt, neu.txt): geom. Mittel Klasse/C++ (min)
  2,100 → 2,086. Besser >5 %: nostr_mulm 7,1→6,1, nostr_from_hex, common_rng_next, common_wrap_pi, common_slerp. Schlechter >5 %: lamps_grid_key,
  settings_escape (Klasse gleich, nur C++-Zeit schwankt), lamps_near 2,4→2,85. lamps_near: in Field_near ändert sich nur n14 (Schleife lokal);
  Gegenproben ($S/e/bench/exp2, exp3, je 5× abwechselnd): neu mit n14 als Member 42,0 µs, neu 35,2, alt 31,0, alt mit n14 lokal 36,6 – die Lage
  des Codes (LTO) streut ±15–30 %, n14 allein erklärt es nicht. Ehrlich: lamps_near ist im Mittel langsamer, Ursache nicht eindeutig.
- 08:40 Koordinator: nur Schritt 1 belegen, dann Ende. Schritt 2 war schon gebaut: Zweig work-e-tramp @ b482049a (NICHT belegt, nicht mergen
  ohne Suiten) – alles für Schritt 2–5 in uebergabe/e.md.

## 09:20 – Schritte 2–5 gebaut auf work-e-tramp (auf work-e b7abff9d), letzter Commit 26212023
- Engine-Suite Schritt 1: work-e-engine-080904-22468 rc=1, einziger Fehler logic_libraries (erwarteter Garage-Text in alter Form) →
  b7abff9d korrigiert; gezielte Probe work-e-engine-084557-30600 läuft.
- Schritt 2 Interpreter (3e351c85, b482049a): siehe uebergabe/e.md. Schritt 4 Leser/Fälle/Doku (733f19bd, 26212023). Schritt 3 JIT (4352dd1f):
  Stapel über Zelle + Wachsen, Call Function als Rahmen. Schritt 5 Konverter (6ee40d65): Rekursion im Teil, Anlehnen über Teile, pin_circles.
- CI 44483698325 auf 4352dd1f: **Logic JIT: alle 47 Fälle Interpreter = JIT** (auch recursion_direct/through/deep/endless); JIT 178× schneller als
  Interpreter wie zuvor. native rot nur an zwei alten Erwartungen (Fact-Verbot, gekürzter Fehlertext) → 26212023. native-spawncity/e2e/input liefen noch.
