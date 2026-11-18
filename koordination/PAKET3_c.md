Paket „c“ (Rückrufe der Engine in das Programm). Lies zuerst $S/PAKET_COMMON3.md (S=/home/claude/kernengine.de/koordination) ganz (alle Nachträge), dann kern/docs/blueprint_cpp.md
ganz und in kern/games/spawncity/BLUEPRINTS.md „Der Weg ab hier“ (Schritt 8: „… Rückrufe der Engine als Events …“). Zweig work-c, Worktree /home/claude/w-c.

Befund (Paket f1): Wo die Engine eine Funktion des Programms ruft, wartet der Konverter, und eine Absenkung gleicher Kosten gibt es nicht:
- kern::HeightSampler (Funktionszeiger + void*): Terrain::sampler() gibt ihn, CityStream::begin_roads reicht ihn dem RoadBuilder, der die Höhe
  je Punkt fragt. kern::RoadOptions hat ein std::function-Feld (deformed) und gilt darum ganz als Engine-Objekt (Handle).
- Daran hängen begin_roads, relay, update, finish_features, fail, go_offline (Aufrufkette in city_stream) und Terrain::sampler.
- Suche selbst nach allen weiteren Stellen dieser Art im Spiel und in den Headern der Engine (Funktionszeiger, std::function, Rückruf-Parameter,
  $S/f9_roots.txt: „function value“, „given back by value“, „an object of the engine … as a local“; engine_api.py: „20 have types without a
  place in the language“ – welche davon sind Rückrufe?).

Auftrag: die allgemeine Fähigkeit „die Engine ruft eine Funktion eines Blueprints“ – Sprache, Engine, Generator, Konverter – für jedes Spiel.
1. ENTWURF zuerst (SendMessage an main, bevor du baust): Was ist der Wert, den das Programm der Engine gibt? (Vorschlag zum Prüfen, nicht
   bindend: eine „Funktion eines Blueprints“ als Wert fester Gestalt – Blueprint/Instanz + Funktion + Signatur –, den nur die Engine rufen kann;
   in der Sprache kein allgemeiner Funktionswert, kein Aufruf durch einen Blueprint selbst, damit die Sprache bleibt, was sie ist.) Wie sieht er
   im Text aus (ein Teil von C++), als Knoten, in check()? Wie ruft ihn die Engine in jeder Stufe: Interpreter/JIT (Runtime ruft die Funktion,
   synchron, mit Rückgabe – auch während ein Engine-Knoten läuft: Wiedereintritt klären), Klasse (direkter Funktionszeiger auf die Methode der
   Klasse: gleiche Kosten wie heute Funktionszeiger + void*)? Was sagt die Engine-Seite (Typen wie HeightSampler/std::function-Felder bleiben
   für C++-Programme, wie sie sind – die Tabelle beschreibt sie als Rückruf mit Signatur)? Lebensdauer (der Rückruf gilt, solange die Instanz
   lebt; danach „ungebunden“, in jeder Stufe gleich). Reihenfolge und Determinismus (wann ruft die Engine: nur synchron innerhalb des Knotens,
   oder später – z. B. aus einem Job? Jobs laufen auf anderen Threads: kläre, was erlaubt ist, und was wartet, mit Grund).
2. Bauen nach bestätigtem Entwurf: in jeder Stufe bitgleich, Engine-Tests, nativer Fall, Doku, Editor (Palette/Pin-Typ), engine_api.py erzeugt
   Tür und direkte Form aus der Tabelle (Besitzer von engine_api.py ist Paket d – der arbeitet dort gerade an „Plätzen der Engine“: halte deine
   Änderungen an engine_api.py lokal und klein, eigene Funktionen; sag mir, wo es sich berührt).
3. Konverter: schreibt die Stellen des Spiels damit (klein, lokal, eigene Funktionen). Die Spiel-Quelle bleibt unverändert, außer du brauchst
   eine Absenkung – die gehört dann f1 (city_stream, terrain): melde sie.
4. Beweis: Engine-Suite und Spiel-Suite auf mag grün; die betroffenen Funktionen geschrieben und im Test verglichen (auch SC_ENGINE=real).

## Nachtrag 17:50 (neuer Koordinator) – Entscheidung E2 zum Entwurf ($S/uebergabe/c.md)
**Der Entwurf ist bestätigt** – `Callback<R(A…)>` als Wert fester Gestalt (eine zur Übersetzungszeit feste Funktion + höchstens ein gebundener Platz als
const-Referenz), nur die Engine ruft ihn, er nennt nur eine Funktion, die nichts vom Blueprint braucht; Knoten „Make Callback“, Text `callback<&Lib::fn>(platz)`;
Klasse: Thunk + `void*` (Kosten wie heute); Interpreter/JIT: Trampolin auf `Runtime::call_static`; Lebensdauer an das Objekt der Engine bzw. den Aufruf gebunden.
Auflagen:
1. **Der Beweis muss den Rückruf wirklich vergleichen:** Der Stellvertreter ruft jeden Callback, den ein Knoten bekommt, und **was der Callback gibt, geht in die
   Aufzeichnung** (und in die gewürfelte Antwort des Knotens) ein – auf allen drei Seiten (Quelltext, Interpreter, Klasse). Nicht nur mit Nullen: mit Eingängen,
   die der Stellvertreter aus dem Aufruf würfelt, mehrmals. Mit SC_ENGINE=real ruft die Engine selbst.
2. **Behaltene Rückrufe** (RoadBuilder::begin behält, step() ruft): der Stellvertreter ruft sie auch bei den späteren Knoten desselben Objekts, die laut Tabelle
   rufen dürfen – sonst wird der behaltene Weg nie verglichen. Die Tabelle sagt je Funktion „behält“/„ruft während des Aufrufs“ aus dem Header, nicht aus einer Liste.
3. Zu deinen drei Fragen: (1) `register_node` (game.cpp) ist kein Rückrufwert – einverstanden. Es wird **keine** Konverter-Regel, die an NodeDef hängt. Wie ein
   Programm einen Handler eines Events der Engine erklärt (im Blueprint `KERN_EVENT(onN, event_tick)`), ist eine eigene Frage der Stufe 6 zusammen mit `build_level`
   (das Level steht in der Level-Datei): dafür kommt ein eigener Entwurf, nicht in Paket c. (2) Jobs als **Teil 2**: die Erklärung `KERN_JOB(kind, &Lib::fn);`
   einer Bibliothek ist bestätigt – mit dem Nachweis, dass Klasse (Threads/Worker) und Interpreter/JIT (`pump` auf dem Frame-Thread) dieselben Bytes geben und der
   CI-Job determinism grün bleibt. (3) Reihenfolge: **erst Paket d fertig (Platz der Engine), dann baust du** – engine_api.py hat einen Besitzer zur Zeit.
Bis d fertig ist, bleibt Paket c angehalten; begin_roads, relay, update, finish_features (city_stream) und Terrain::sampler bleiben unverändert.
