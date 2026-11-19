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
