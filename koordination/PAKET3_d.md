Paket „d“ (Stufe 5, Voraussetzung). Lies zuerst $S/PAKET_COMMON3.md (S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad) ganz und was es nennt. Sperre: heavy3.lock. Zweig work-d, Worktree /home/claude/w-d.

Auftrag: BLUEPRINTS.md, Stufe 4, Punkt (d) „Volle Geschwindigkeit im Export“ – vor dem Umschalten der ersten Datei:
In der exportierten Klasse (tools/blueprint_cpp.cpp, kern/src/logic/notation.cpp; kern/docs/blueprint_cpp.md) ruft die Funktion eines
Engine-Knotens die Funktion der Engine direkt und typisiert (z. B. world_of(welt)->cast_ray(a, b)), ohne Werte zu verpacken (kein logic::Value,
kein Host::engine je Aufruf). Die Tür (Host::engine → logic::call_engine, kern/include/kern/logic/engine_host.h, kern/src/*/engine_calls.gen.cpp)
bleibt für Interpreter, JIT und Tests. Derselbe Generator (tools/engine_api.py) schreibt beides: aus derselben Tabelle die Aufrufe hinter der Tür
und die typisierten Aufrufe der Klasse – eine Quelle, keine zweite Liste. Handles: die Klasse löst ein Handle über dieselben EngineObjects auf
wie die Tür (ein Nachschlagen, keine Verpackung der Argumente).
Beweis: (1) bitgleich – der native Fall „Engine“ (kern/tests/blueprint_native.h) und die Engine-Tests engine_calls_* vergleichen Klasse (direkt)
gegen Interpreter (Tür) für jeden Knoten, den der Fall ruft; (2) Kosten – ein Test zählt, dass der direkte Aufruf keinen Speicher anfordert, wo
die Funktion der Engine selbst keinen anfordert; (3) -Werror, libstdc++ und libc++ (keine Plattform-Typnamen: siehe fixed_width in engine_api.py).
Ändere keine Spiel-Quelle. Lokal nur Syntax und einzelne Dateien übersetzen (2 Kerne, 8 GB – die native Stufe als Ganzes läuft in CI: sag im
Bericht genau, welche CI-Jobs das prüfen müssen und worauf zu achten ist).
