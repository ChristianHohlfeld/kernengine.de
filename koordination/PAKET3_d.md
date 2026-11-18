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

## Nachtrag 22:25 – Nachfolger 3 für d: „Platz der Engine“ zu Ende bringen und beweisen
Stand: uebergabe/d.md, Abschnitt B „Stand der 7 Schritte“; stand/d.md. Zweig work-d = 2250736 (Schritt 3 fertig, nur Syntax geprüft; Schritt 4 gebaut, nie
instanziiert; Schritt 5 teilweise – Palette fehlt; Schritt 6/7 offen; keine Engine-Suite, kein CI-Lauf). Der Fix für Zeiger-Eingänge (690ea4f) ist schon in
work-zusammen (8844531a) – zuerst `git merge origin/work-zusammen`.
1. **Erst beweisen, was gebaut ist, dann weiterbauen:** Place/Ref/Field instanziieren (Engine-Test je Baustein, der Fall mit jedem Knoten, der native Fall),
   Interpreter = JIT = Klasse für Lesen, Ändern am Platz, Zurückschreiben, Freigabe dazwischen (Generation), Platz ohne Objekt (Null des Typs). Engine-Suite:
   `$S/mag.sh engine origin/work-d [filter]` (gezielt mit Filter, die ganze Suite einmal je abgeschlossenem Schritt), CI: `gh workflow run ci.yml --ref work-d`.
2. Dann Schritt 5 fertig (Palette in logic_edit.cpp, check()), Schritt 6 (Doku blueprint_cpp.md), Schritt 7 (Stellvertreter im Test, Regel des Konverters – mit t1
   abstimmen: TestWriter gehört t1/t2; Beleg mit `$S/mag.sh vergleich …`, siehe PAKET_COMMON3.md Nachtrag 22:25).
3. Offene Frage des Vorgängers (`list_append` auf einem Platz reserviert genau Länge + n; ein C++-insert wächst je nach Bibliothek anders): erst feststellen, was
   der Vergleich davon sieht (Werte nie; die `cost:`-Zeile zählt Anforderungen). Mit Befund als „ENTSCHEIDUNG NÖTIG“ melden, nicht selbst festlegen.
4. Nach jedem bewiesenen Schritt: Commit, Push, stand/d.md und uebergabe/d.md nachführen; „BEREIT ZUM MERGEN“ erst für Bewiesenes (Engine-Suite auf mag rc=0 +
   CI grün), Nachricht an main. Paket c (Callbacks) baut danach auf deinem engine_api.py auf – Änderungen dort in stand/d.md vermerken.

## Nachtrag 00:05 (19.11.) – Nachfolger 4 für d: zuerst die drei Stellvertreter-Befunde, dann Platz der Engine aktuell halten
Stand: uebergabe/d.md und stand/d.md („STAND 23:40“). Zweig work-d = ddb54b72: Platz der Engine Schritte 3–6 gebaut, Engine-Suite grün
(`work-d-engine-223942-31317` rc=0), CI e2e rot allein wegen des alten Levels (ein Draht liest dort einen Ausgang, der jetzt ein Platz ist – erst ein voller
Lauf schreibt das Level neu); Schritt 7 gebaut (e02ea24), an keiner Quelle belegt.
**Entscheidung des Koordinators zum Platz:** er wird erst gemergt, wenn `work` einen ersten grünen Stand hat (ein gültiger voller Lauf mit grüner Spiel- und
Engine-Suite) – bis dahin eine Veränderliche weniger. Halte work-d aktuell (`git merge origin/work-zusammen`), mehr nicht.
**Zuerst, in dieser Reihenfolge – jeder Fix ein eigener Commit, direkt auf origin/work-zusammen aufgesetzt (eigener kurzer Zweig, danach in work-d mischen), Beleg
mit `$S/mag.sh vergleich` über die kleinste betroffene Datei, „BEREIT ZUM MERGEN“ + Nachricht an main:**
1. **push_incoming / die Zahl neben einem Zeiger sagt ihre Länge im Typ (Count<Per>):** die Erkennung über den Namen (`count|size|len|num|^n$`; engine_api.py ca.
   Z. 457/593, Zeiger-Zweig im TestWriter) fällt weg – sie ist eine Namensliste. Die Erklärungen der Engine, die einen Zeiger mit Zahl daneben nehmen, bekommen
   Count<Per>; was danach noch ohne Count neben einem Zeiger steht, meldet der Erzeuger als Fehler (kein stilles Raten). Beleg: net_session.cpp (Session_push,
   ABBRUCH converted_net_session gegen die Engine).
2. **matrix_of:** ein Feld oder Ergebnis `T m[N]` hat in der Form und in der Antwort des Stellvertreters genau N Werte (Mat4: 16) – wie dein Fix für Parameter
   (50832229, `declared_length`). Beleg: eine Datei, die actor_matrix ruft; im Durchgang gegen die Engine darf actor_matrix nicht mehr abgewiesen werden.
3. **set_pcm – Entscheidung E14: JA zum Vorschlag des Vorgängers.** Die Zahl neben dem Zeiger zählt, was gelesen wird: `set_pcm(const f32* data, Count<1> samples,
   u32 channels)`, die Engine rechnet frames = samples / channels und weist eine Zahl ab, die nicht aufgeht. Alle Aufrufer angleichen (Engine, studio.cpp:82 gibt
   schon frames·channels; im Spiel channels = 1, die Zahl bleibt dieselbe – keine Quelle des Spiels ändert ihr Verhalten; prüfe das je Aufrufstelle), Engine-Tests
   und Doku der Funktion. Beleg: game_gaps.cpp (gaps_guided_sounds) + Engine-Suite (Filter sound).
Danach: Schritt 7 an einer Quelle belegen, die einen Platz wirklich schreibt (sobald drone_attack/jet_attack nicht mehr aus anderen Gründen warten – g/k), und
den Platz zum Mergen vorbereiten (Liste der Commits, Belege) für den Moment nach dem ersten grünen Stand.

