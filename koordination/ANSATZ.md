# Der Ansatz – vollständig, verbindlich (Pflichtlektüre für jeden Agenten)

Chris (14.11.2026, 13:18/13:26): „Hört erst auf, wenn ihr vollständig fertig seid. 100% saubere
Lösung. Da, wo Claude Fable aufgehört hat. Und der Ansatz muss natürlich vollständig von euch
verstanden sein, damit ihr nicht in die falsche Richtung rennt." – „Lese auch den gesamten Chat,
damit du es verstehst. Gib es auch an die Agenten so weiter."

Lies dieses Dokument ganz, dann **kern/games/spawncity/BLUEPRINTS.md ganz** (der Plan; besonders
„Stufen", Abschnitt 4 „Der Weg ab hier" mit der Tabelle „Die Abbildung", und „Regeln für jeden
Schritt") und die Abschnitte von **kern/docs/blueprint_cpp.md**, die dein Paket berühren. Erst dann
arbeite. Wenn dein Auftrag einer Aussage hier oder in BLUEPRINTS.md widerspricht, gilt dieses
Dokument und der Plan – melde den Widerspruch im Bericht.

## 1. Das Ziel

- SpawnCity auf Kern (kern/games/spawncity) ist der vollständige, exakte Drop-in-Ersatz für
  spawncity.de (das JavaScript-Original). „State of the Art", „100%", „nicht hacky, sauber",
  „Keine Kompromisse".
- **Das ganze Spiel wird Blueprints.** Das Projekt ist ein Level + Blueprints + Daten, geladen vom
  Editor (Kern Studio); `src/` wird am Ende leer.
- **Nichts von SpawnCity wandert in die Engine** („definitiv falsch"). Engine und Sprache bekommen
  nur, was allgemein ist und jedem Spiel dient.

## 2. Wie – in Chris' Worten (wörtlich, nicht verhandelbar)

- „die idee war c++ subset interchangeable realtime als blueprint und native alles exportierbar in
  c++, full performance. dazu wollten wir die subset constraints der c++ codes mit script
  umwandelbar machen damit wir generisch blueprints aus constraint conformem cpp haben."
- „blueprint == c++ , auf constraintbasis für beide: nur die erlaubten constraints aus c++ sind
  gestattet"
- „warum, wir wollten doch generieren, nicht händisch alles umschreiben oder?!?" – Blueprints werden
  **generiert** (tools/cpp_to_blueprint.py), nie von Hand gebaut oder geändert.
- „mach weiter, der konverter kann py bleiben." · „kein caching." · „Konsequent nicht hacky." ·
  „No excuses, keine Sonderregelung" · „Finde den saubersten weg. Implementierung danach." ·
  „Sei streng im Ansatz, sauber und der Umsetzung, schnell im finalisieren." · „All the way."
- „Konsistenz bei rechnen ist wichtiger als perfekte Abbildungen aller Operationen." Vergleich
  **bit für bit, ohne Toleranz** („Inakzeptabel" sonst). Kein Schummeln in Tests.
- „Melden bringt mir nichts. Exaktheit und voll durchlaufen schon."
- Kein Bridge von „Funktionsaufrufen" aus einem Blueprint in C++ des Programms.

## 3. Die Mechanik (was der Plan festlegt)

- Ein Blueprint und sein C++-Text sind **dasselbe in zwei Schreibweisen**; die Schreibweise ist ein
  Teil von C++ (die erlaubte Teilmenge). Jeder Baustein: (1) hat einen Knoten, (2) bedeutet in
  jeder Stufe dasselbe (Interpreter, JIT, die eigene C++-Klasse = nativer Export), (3) kostet in
  der Klasse, was das C++ kostet, das er darstellt.
- Für jede Stelle des Spiels, die der Konverter noch nicht schreiben kann, gibt es **genau zwei
  Antworten**: (a) die Sprache bekommt den Baustein – allgemein, in jeder Stufe, mit Knoten,
  Schreibweise, Regeln (`check`), Doku und Tests der Engine; oder (b) die Stelle des Spiels wird in
  erlaubtes, gewöhnliches C++ **gleicher Kosten und bit-genau gleichen Verhaltens** umgeschrieben.
  **Eine Form, die Gestalt oder Kosten ändert, ist keine von beiden.**
- **Die Abbildung** (BLUEPRINTS.md): Klasse mit Feldern → Struktur des Projekts; Methode →
  Funktion mit Referenz auf ihr Objekt; **`Game` (das eine Objekt mit dem Zustand des Spiels, über
  alle seine Dateien) → EIN Blueprint: seine Felder sind die Variablen, seine Methoden die
  Funktionen** (also *keine* Struktur – die Grenze von 96 Feldern je Struktur betrifft Game nicht);
  Quelldatei/Header → je ein Blueprint, ohne Zustand eine Bibliothek; Zeiger als Parameter →
  Referenz; Zeiger auf ein Objekt der Engine → Handle; Zeiger als Feld auf ein Objekt des Spiels →
  Parameter der Funktionen, die es brauchen; `std::vector` → Liste; Map/Set → Map, Set (Schritt
  5); `u64`/`i64` als Bitmuster → `Int64` (Schritt 6); Funktion der Engine → Knoten der Engine.
- **Reihenfolge des Plans:** Plätze, Referenz-Parameter, Kosten, Bibliotheken (erledigt) → **`Game`
  als Blueprint** → Map/Set → Int64 → Engine-Aufrufe direkt → der Rest (Text formatieren, Lambdas,
  Zustand einer Datei, Rückrufe der Engine als Events …).
- **Der Beweis je Schritt:** bitgleich (Quelltext, Interpreter, Klasse); **Kosten:** die Klasse
  fordert nie öfter Speicher an als der Quelltext (der Test zählt; `cost:`-Zeilen werden zum
  Fehler, sobald sie leer sind); Zeit im Export-Build.
- **Es gibt zu jeder Zeit genau eine Quelle:** Bis eine Datei ganz durchläuft und umschaltet
  (Stufe 5), ist ihr C++ die Quelle und der Blueprint daraus erzeugt, durch den Vergleichstest
  gebunden. Werte und Verhalten bleiben die des Originals.

## 4. Feste Regeln

- Box3D nie tunen; Physik nach Erin Cattos Doku, keine Hacks. Die Firmware der Drohne
  (kern/games/spawncity/firmware, mit dem echten Fluggerät geteilt) wird nicht verändert.
- Nicht anfassen: die 22 als „Unchanged from the original SpawnCity" markierten Web-Dateien;
  Runden/XP/Teilen/öffentliche Lobby/Missionen (zurückgenommen, nicht wieder einbauen); „Gewalt
  und Passanten" bleiben, wie sie sind; TON/MUSIK/RESET-Knöpfe bleiben, wo sie sind; das Training
  der Ente bleibt unbeeinflusst („ente muss neutral bleiben").
- Keine Umgehung blockierter Hosts/Rechte. GitHub über `gh`. Pushen nur eigene Zweige, nie main
  oder work, kein Force-Push. **Zwischenstände regelmäßig pushen** („immer zwischenschritte
  regelmäßig pushen. wichtig.").
- Arbeitsweise jetzt (Chris, 11:16/11:18): **„alles fertig schreiben, ohne jedes Mal zu testen …
  danach vollständig einen Testlauf … dann jeden korrigieren"; „Tests am Ende wenn alles fertig
  ist"; „Fortschritt immer pushen regelmäßig".** Also: keine Prüfläufe in der Schreibphase, nur die
  schnellen Kontrollen aus FORK_RULES.md; der Hauptlauf macht am Ende den vollständigen Lauf.
- Antworten und Berichte auf Deutsch. Keine Aufwandsschätzungen in Entwickler-Tagen.

## 5. Wo wir stehen (14.11.2026, 13:30)

- Auf main und live: Übergeben statt kopieren, Listen in einem Zug. Bit für bit verglichen: 602 von
  1.851 Funktionen; Kosten: 70 teurer als der Quelltext (Stand vor den Schritten unten).
- Auf `work` geschrieben (ungeprüft): Listen fester Länge (`Fixed<T, N>`, bis 65.536, Text/JSON
  sagen nur bis zum letzten Eintrag ≠ 0), Text des Programms (`logic::Text`, Literale ohne Kopie),
  Format Text (`text_format<"…">`, printf-genau), Code At / Text of Code, Sort List (`list_sort`,
  stabil, Order = reine Funktion `&Klasse::Fn`), Getter-als-Ort (`Emit.dissolve`), Engine-Methoden
  von Objekten als Knoten, constexpr-Funktionen/Konstruktoren als Konstanten, Call on Actor für
  Regel-Blueprints (`kern::logic::ActorBlueprint`), Zeiger aus Kacheln/Sichten/Parametern,
  Konverter-Fähigkeiten (find/find_if/min/max_element, remove_if, ++ an Plätzen, memset/memcpy,
  …), Game-Felder ohne shared_ptr/Deleter/Konstruktor-Defaults.
- **Korrektur 13:30:** Die Umgliederung von `Game` in Sub-Structs (Commit 74129c1, „247 → 87
  Felder") widerspricht der Abbildung (Game ist ein Blueprint mit Variablen, keine Struktur) und
  wird zurückgenommen. Game behält seine Felder, wie der Quelltext sie hat.

## 6. Was noch fehlt (in der Reihenfolge des Plans)

1. **`Game` als Blueprint** (Konverter): Game's Felder werden Variablen *eines* Blueprints, seine
   Methoden (über alle game_*.cpp) dessen Funktionen ohne `self`; dazu die Tests über Dateigrenzen
   (Paket „tests").
2. Map/Set, Int64, Engine-Aufrufe direkt, dann der Rest (siehe Plan Schritt 8).
3. Schlusslauf: Engine-Suite, volle Konvertierung, Bit-für-Bit-Vergleich, Spiel-Suite; jede Abweichung
   sauber beheben; dann main, CI, Deploy.
