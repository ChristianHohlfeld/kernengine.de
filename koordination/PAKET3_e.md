Paket „e“ (Rekursion: eine Funktion eines Blueprints ruft sich selbst, direkt oder über andere). Lies zuerst $S/PAKET_COMMON3.md
(S=/home/claude/kernengine.de/koordination) ganz (alle Nachträge), $S/UEBERGABE.md Abschnitte 1–9, dann kern/docs/blueprint_cpp.md ganz,
in $S/stand/g.md die Abschnitte „Wurzel des Kreises“ und „Rekursion (für den Entwurfspunkt Stufe 6)“, und $S/wartet_055004.txt
(Zeilen mit „went round“ und „ctl_strike_confirm“). Zweig work-e, Worktree /home/claude/w-e (steht auf work-zusammen).

Befund (Paket g, Lauf 055004): die Sprache verbietet Rekursion – der Leser (kern/src/logic/notation.cpp, „a function that calls itself,
directly or through others“) weist sie ab; erlaubt ist nur Wiedereintritt über ein Event (logic_cases.h `function_reentry`, Events höchstens
64 tief). Das Spiel hat echte Rekursion über Teile hinweg: Game::explosion → blast_cars → wreck_car → explosion und explosion → combat_blast
→ … → explosion (Kettenreaktion). Daran hängen 18 Funktionen direkt („waits for good … went round“) und rund 17 weitere über Aufrufketten
(ctl_strike_confirm, hit_scan, step_heli …) – etwa 35 Funktionen, 2500 Zeilen: der größte Block der noch nicht geschriebenen Funktionen.

Auftrag: die allgemeine Fähigkeit „eine Funktion ruft sich selbst, direkt oder über andere“ – Sprache, Leser, Interpreter, JIT, Klasse
(to_cpp/from_cpp), Editor-Prüfung, Doku, Engine-Fälle; danach der Konverter (Ablauf: ein Aufruf eines Teils desselben Blueprints, der nur
im Kreis mit dem Rufer wartet, gilt mit der Gestalt seines Eintrags als geschrieben, geprüft am Fixpunkt – Vorschlag g) – für jedes Spiel,
nichts von SpawnCity in der Engine.

1. ENTWURF zuerst (SendMessage an main, bevor du baust; kurz, mit Zahlen). Vorgaben des Koordinators, an denen der Entwurf gemessen wird:
   a) Blueprint == C++-Teilmenge in beide Richtungen: der Blueprint verhält sich für JEDE Tiefe, die das C++ des Spiels in seinem Bau
      (WASM und nativ) erreicht, wie das C++. Eine feste kleine Grenze (64 wie bei Events), die eine Kettenreaktion im Spiel überschreiten
      kann, erfüllt das NICHT – außer du zeigst am Code, dass die Tiefe im Spiel beweisbar darunter bleibt. Miss, was Interpreter und JIT
      je Aufruf an Stapel brauchen und wie tief sie auf dem kleinsten Stapel (WASM-Bau der Engine, Schalter nachsehen) kommen; wenn das
      nicht reicht, schlage vor, wie die Rahmen des Interpreters vom Stapel der Maschine unabhängig werden.
   b) Interpreter = JIT = Klasse, Bit für Bit, bei jedem Aufruf im erlaubten Bereich. Jeder Aufruf hat seine eigenen Parameter und Locals
      (wie function_reentry).
   c) Eine Rekursion, die nie endet, ist im Interpreter und im JIT ein definierter Fehler (kein Absturz des Editors); was die Klasse dann
      tut und wie der Vergleichstest solche Aufrufe behandelt („C++ leaves open“: aufgegeben und neu gezogen, nie still gleich), sagt der
      Entwurf ausdrücklich.
   d) Die Klasse kostet für einen rekursiven Aufruf nicht mehr als der C++-Aufruf (Ziel des Projekts: Klasse so schnell wie das C++).
   e) Keine Sonderregel, keine Namensliste; was heute für Events gilt, bleibt.
   Nenne im Entwurf: was sich in Sprache/Leser/Interpreter/JIT/Klasse/Doku ändert, welche Engine-Fälle (direkt, über andere, tief, endlos)
   es belegen, was der Konverter und der Test-Schreiber (Paket t1 – nur als Befund, nicht selbst ändern) brauchen.
2. Nach meinem Ja: bauen in kleinen Commits; Engine-Suite (`cd $S && ./mag.sh engine origin/work-e`) grün; dann der Konverter-Teil mit
   Beleg `./mag.sh konvert origin/work-e 9` bzw. `vergleich` über die Game-Dateien des Kreises.
Stand in $S/stand/e.md, Übergabe vor dem Ende in $S/uebergabe/e.md. Du pushst nur auf work-e.
