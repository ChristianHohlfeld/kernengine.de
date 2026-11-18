Paket „d“ (Stufe 5, Voraussetzung). Lies zuerst $S/PAKET_COMMON3.md (S=/home/claude/kernengine.de/koordination) ganz und was es nennt. Sperre: heavy3.lock. Zweig work-d, Worktree /home/claude/w-d.

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

## Nachtrag 17:50 (neuer Koordinator) – Reihenfolge für den Nachfolger
**Zuerst (vor allem anderen): die Engine-Suite ist auf `work` (90b5ab8, Baum = 30f63e4, dein Teil 1) auf mag rot** – dreimal gleich
(`$S/mag.sh holen work-d-engine-163138-9400`, ebenso -161602-25272, -154346-16923; engine.log.xz):
`FAIL Engine on the engine itself, the class calling the engine itself: after Free1: R129=[1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]  through the door: R129=[0, 0, 0, 0]`
(ebenso Free2, Free3); „not the same twice through the door, so not compared: R118 R120 R121 R122_0 R126“. In CI (44337346531, wenige Kerne) stand R129
(jobs_take_queued) unter den nicht wiederholbaren und wurde nicht verglichen; auf mag (56 Threads) gibt die Tür zweimal dasselbe und die Klasse etwas anderes –
16 Einträge statt 4. Das ist entweder eine echte Abweichung Klasse/Tür (z. B. wie viel Platz der Ausgang bekommt: Tabelle `EnginePin::count` gegen den direkten
Aufruf) oder ein Vergleich, der vom Lauf der Job-Threads abhängt. Finde die Ursache am Code, nicht am Symptom:
- Ist es eine Abweichung: beheben (Generator/engine_direct.h), so dass Klasse und Tür für jeden Aufruf dasselbe geben.
- Hängt der Wert vom zeitlichen Ablauf ab: der Vergleich muss für solche Knoten **eindeutig gemacht** werden (z. B. die Jobs des Falls auf dem Frame-Thread
  rechnen/abwarten, bevor genommen wird – allgemein im Test, nicht je Knoten ausgenommen); „zweimal gleich durch die Tür“ ist kein Beweis der Wiederholbarkeit.
  Kein Knoten wird stillschweigend aus dem Vergleich genommen; was grundsätzlich nicht wiederholbar ist (Uhren: now_ms, wall_ms), nennt der Test beim Namen aus
  einer Eigenschaft der Tabelle, nicht aus einer Beobachtung zweier Läufe.
Beweis: `$S/mag.sh engine origin/work-d` grün (zweimal hintereinander) und CI-Job native. Melde das als „BEREIT ZUM MERGEN (Engine-Suite)“ für sich – es geht
sofort nach `work`, getrennt vom Platz der Engine.
**Danach:** „Platz der Engine“ weiter ab Schritt 3 von 7 ($S/uebergabe/d.md, bestätigter Entwurf und Auflagen gelten unverändert). Paket c (Rückrufe) baut erst,
wenn du fertig bist – es berührt engine_api.py (HeightSampler/RoadOptions wären keine Handles mehr); schreib in `$S/stand/d.md`, sobald deine Änderungen dort stehen.
Neuer Bedarf an der Engine aus g1 (allgemein, nicht SpawnCity): `Mixer::bus()` gibt `BusSettings&` (ein Platz der Engine nach deiner Regel? – prüfe, ob dein
Erkenner ihn nimmt: Rumpf, Eingang `Bus`; heute „keine weiteren Eingänge“) und `TextureDesc::rgba` (Zeiger auf Daten in einer Struktur der Engine). Sag in
`$S/stand/d.md`, was davon dein Entwurf trägt und was nicht.
