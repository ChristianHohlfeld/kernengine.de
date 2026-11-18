Paket „p“ (Leistung der exportierten Klasse). Lies zuerst $S/PAKET_COMMON3.md (S=/home/claude/kernengine.de/koordination) ganz (Nachträge 15:40 mag, 16:20, 16:50), dann
kern/docs/blueprint_cpp.md ganz und in kern/games/spawncity/BLUEPRINTS.md „Der Weg ab hier“ (Grundsatz: jeder Baustein „kostet in der Klasse,
was das C++ kostet, das er darstellt“; „Der Beweis je Schritt: … Zeit im Export-Build“). Zweig work-p, Worktree /home/claude/w-p.

Befund (Paket s5, 18.11.): Die erste umgeschaltete Datei, qr_code, ist als exportierte Klasse rund 17× langsamer als ihr altes C++
(encode der v5-URL, -O2: C++ 36–47 µs, Klasse 730–830 µs, Ausgabe gleich). Chris' Anforderung ist wörtlich „native alles exportierbar in c++,
full performance“. Der Test zählt bisher nur Speicheranforderungen, nicht Zeit. So darf das Spiel nicht umschalten.

Auftrag: Die exportierte Klasse eines Blueprints läuft so schnell wie das C++, aus dem er erzeugt ist – allgemein (Generator der Klassen:
tools/blueprint_cpp.cpp, kern/src/logic/notation.cpp, kern/include/kern/logic/nodes.h, blueprint.h, engine_direct.h), nichts für SpawnCity.
1. Messen: ein Zeit-Vergleich je konvertierter Funktion, allgemein im Test-Schreiber-Rahmen NICHT (der gehört t1/t2) – baue ein eigenes kleines
   Werkzeug/Testprogramm (z. B. tools/blueprint_bench oder ein Ziel in kern/games/spawncity/tests), das für eine Liste von Funktionen den
   Quelltext und die Klasse mit denselben Eingaben im Release-Bau (-O2, wie build.sh) misst und das Verhältnis ausgibt. Beginne mit qr encode
   (Stand vor dem Umschalten: origin/work hat qr_code.cpp noch; s5 hat die Klasse: origin/work-s5) und nimm 20–30 weitere Funktionen
   verschiedener Art dazu (Schleifen über Listen, Vektor-Rechnung, Strukturen, Text, Maps), die auf origin/work ganz geschrieben sind.
2. Ursachen finden (perf/Zeitprofil, erzeugten Code lesen): z. B. Zahlen als double statt ganzer Zahlen in Schleifen und Indizes, Prüfungen
   je Listenzugriff, Kopien von Werten/Listen, Aufrufe über Zeiger statt direkt, fehlendes Inlining über Namensräume, Schleifenwächter (budget)
   je Durchlauf, Wandlungen f32↔f64 an Feldern. Ordne jede Ursache nach Anteil.
3. Beheben – in der Erzeugung, für jeden Blueprint, bitgleich in jeder Stufe (Interpreter, JIT, Klasse: die Klasse darf schneller rechnen, aber
   nichts anderes ausrechnen; die Zahlen der Sprache bleiben 64 Bit – kein Float32). Was nur mit einer Aussage über den Graphen geht (ein Index
   ist beweisbar ganzzahlig und im Bereich), leitet der Generator am Graphen her; keine Annahmen, kein Abschalten von Prüfungen, die das
   Verhalten festlegen.
4. Beweis: die Engine-Suite samt nativer Stufe (`$S/mag.sh engine origin/work-p`) und die Spiel-Suite (`$S/mag.sh spiel origin/work-p`) grün
   – also weiter bitgleich; die Messung vorher/nachher je Funktion; Ziel: Verhältnis ≤ 1,0 im Rahmen des Messrauschens für jede gemessene
   Funktion. Was darüber bleibt: Funktion, Verhältnis, Ursache, was es bräuchte – kein „wartet“ ohne Begründung.
5. Danach: die Zeit wird Teil des Beweises (ein Test, der fehlschlägt, wenn eine Klasse langsamer ist als ihr Quelltext, mit einer Schwelle,
   die das Rauschen der Maschine deckt – begründe die Schwelle; so, dass CI ihn tragen kann).
Melde dich beim Koordinator (SendMessage an main) mit der Ursachenliste nach Schritt 2, bevor du behebst.

## Nachtrag 17:50 (neuer Koordinator) – Entscheidung E1 zur Ursachenliste deines Vorgängers ($S/uebergabe/p.md)
1. **Ein C++ je Blueprint bleibt.** Die exportierte Klasse ist und bleibt Wort für Wort der Text von `to_cpp` (BLUEPRINTS.md Stufe 1: „Ein Blueprint hat genau
   ein C++ – seine Klasse“). „Nur die Klasse anders erzeugen“ scheidet aus – es gibt keinen zweiten Erzeuger und keinen Text, den der Leser nicht zurückliest.
2. **Die Bedeutung bleibt:** Zahlen 64 Bit, kein Float32, kein neuer Zahlentyp; `Int` ist, was `logic::normal`/`whole` festlegen (exakt bis 2^53, sonst wie double,
   nie über die Enden). Schneller rechnen ja – etwas anderes ausrechnen nie, für keinen Wert (auch nicht NaN, ±0, ±2^53, die Enden von i64).
3. **Ohne Rückfrage, in dieser Reihenfolge** (alles in blueprint.h/nodes.h/logic.h, der Text bleibt gleich; nach jedem Schritt messen):
   a. Ursache 3 (`math_compare` löst den Operator zur Übersetzungszeit auf, `if constexpr` – dieselben Ausdrücke),
   b. Ursache 2 (`kern::to<Int>`/`whole`/`normal` werden eingebettet),
   c. Ursache 1, soweit der Text bleibt: der geprüfte ganzzahlige Weg **in der Umsetzung der Formel-Funktionen** (`kern::math::band/bor/bxor/shl/shr/wrap32/wrap32s/
      mul32`, dazu +, −, ×, Vergleich auf ganzen Zahlen) – etwa über einen Werttyp „exakte ganze Zahl“, den diese Funktionen einander reichen (zu `Float` wird er erst,
      wo ein Float gebraucht wird) und über Überladungen von `kern::to`/`kern::formula`, die ihn durchreichen; er rechnet in i64, solange jeder Operand und jedes
      Zwischenergebnis in ±2^53 liegt, und nimmt sonst genau die heutige Formel. Bit für Bit dasselbe ist dann eine Eigenschaft der Umsetzung, die der Engine-Test
      über die Ränder beweist (jede dieser Funktionen, Klasse gegen Interpreter: 0, ±1, ±(2^31), ±(2^32), ±(2^53−1), ±2^53, ±(2^53+2), die Enden von i64, NaN, ±∞, −0).
   d. Ursachen 4–6 nur als Straffung ohne Bedeutungsänderung (der Wächter zählt weiter je Durchlauf an einer Stelle und stoppt in jeder Stufe beim selben Durchlauf;
      die Prüfung „neben der Liste“ fällt nur weg, wo der Generator am Graphen beweist, dass der Index in der Liste liegt – und dann steht das im Text, s. 4).
4. **Nur mit Entwurf an mich, vor dem Bau:** alles, was die **Schreibweise** ändert (was `to_cpp` schreibt und `from_cpp` liest) – etwa eine Formel mit
   ganzzahligen Ein-/Ausgängen (`[](Int z) -> Int`), ein am Graphen bewiesener Index, ein Bereich. Das wäre Antwort (a): ein Baustein der Sprache – allgemein, in jeder
   Stufe mit derselben Bedeutung (Interpreter/JIT rechnen, was sie heute rechnen), Knoten/Einstellung, `check()`, Leser, Fixpunkt-Tests, Doku, Editor, Konverter.
   Der Entwurf nennt: was genau im Text steht, was es in jeder Stufe bedeutet, warum es für jeden Wert dasselbe gibt, was es gemessen bringt (nach 3a–d), was es
   an Graph/Editor/Konverter kostet. Liefere ihn, sobald die Messung nach 3 zeigt, wie viel an der Float-Grenze der Formel (hinein `kern::to<Float>`, heraus
   `kern::to<Int>`) hängen bleibt.
5. **Zuerst messen** (Schritt 1 des Auftrags ist offen): das Messwerkzeug über 20–30 Funktionen verschiedener Art (ganze Zahlen, Float/Vektor, Strukturen, Listen,
   Text, Maps, Int64, Aufrufe über Bibliotheken) im Release-Bau des Spiels (clang, `-O3 -flto=thin -DNDEBUG -ffp-contract=off -fno-exceptions -fno-rtti`), und die
   Ursachenliste für diese Arten vervollständigen (dein Vorgänger hat nur qr encode profiliert). Die vollständige Liste mit Anteilen kommt in `$S/stand/p.md` und
   in deinen Bericht. `tools/blueprint_cpp.sh --classes` (04f1a57) ist ungetestet und berührt dieselbe Datei wie work-s5 (d5132bc): prüfe es, halte es klein.
6. **Beweis:** Engine-Suite samt nativer Stufe und Spiel-Suite auf mag (zur roten Engine-Suite auf `work` siehe PAKET_COMMON3.md, Nachtrag 17:45, Punkt 9),
   Messung vorher/nachher je Funktion; danach der Zeit-Test (Schritt 5 des Auftrags) – begründete Schwelle, CI-tauglich.

## Nachtrag 21:50 – Entscheidungen des Koordinators zu ps Fragen (stand/p.md „ENTSCHEIDUNG NÖTIG (E2)“ und „(E3)“)
**E4 – Schreibweise (ps E2):**
- **E2.1 Formel mit ganzzahligen Ein-/Ausgängen: JA.** `kern::formula([](Int s) -> Int { … }, self.s)`. Bedingungen:
  1. Welche Formel-Funktion aus ganzen Zahlen exakt eine ganze Zahl macht, steht EINMAL bei der Funktion selbst (ihre Erklärung/Tabelle der Formel-Funktionen);
     Schreiber, Leser und Klasse leiten daraus ab. Keine Liste von Namen im Schreiber.
  2. Bedeutung in jeder Stufe unverändert, bewiesen: Engine-Test, der für jede solche Funktion Interpreter, JIT und Klasse an den Rändern vergleicht
     (0, ±1, 2^31, 2^32, 2^53 und darüber, negative, Schiebeweiten ≥ 32/64) – auch mit Int-Eingängen außerhalb des 32-Bit-Bereichs; dazu der Fixpunkt
     to_cpp → from_cpp → to_cpp und ein Text, den der Leser ablehnen muss (`-> Int` bei einem Ausdruck, der das nicht hergibt).
  3. Zahlliterale im Ausdruck und gemischte Eingänge (Int und Float) sind in blueprint_cpp.md „Formeln“ beschrieben; ein Float-Eingang macht die Formel zur
     Float-Formel wie heute.
  4. +, −, × zwischen Ints (geprüfter Weg) erst als zweiter Schritt, nachdem der erste bewiesen und gemessen ist.
- **E2.2 Weitergabe beim Anhängen (`o = text_append(std::move(o), x, "")`): JA** – unter denselben Bedingungen, unter denen hands_over heute weitergibt
  (niemand liest den alten Wert danach); Fixpunkt und Engine-Test, Messung settings_escape vorher/nachher.
- **E2.3 Index am Graphen bewiesen (`at_known`): ZURÜCKGESTELLT.** Neue Schreibweise mit Beweispflicht im Leser (er müsste die Behauptung am Graphen
  nachprüfen und sonst ablehnen) bei 10–30 %. Nach E2.1/E2.2 neu messen; nur wenn Listen-Schleifen dann noch über der Schwelle liegen, als eigener Entwurf.
**E5 – Speicher (ps E3):**
- **A (Aufruf an einen Actor, ~74 von 93): JA,** p darf den Aufrufweg ändern (`blueprint.h func_call_on`, `Runtime::call_on`, `Instance::call` in
  logic.cpp/logic.h): Text der Klasse unverändert, Bedeutung in jeder Stufe unverändert; Reihenfolge NACH E2.1/E2.2. Beweis: Engine-Suite auf mag und CI,
  und die `cost:`-Zeilen dieser Funktionen sind in der Spiel-Suite weg. (d ist beendet; sein Zwischenstand berührt blueprint.h an anderer Stelle – vor dem
  Bauen origin/work-zusammen holen.)
- **B (Listen wachsen anders): BEFUND FÜR t1** – t1 prüft, ob die 1–2 Anforderungen im Test beim Umwandeln der Argumente entstehen (gemeldet).
- **C (Firmware Arondight45_*):** Firmware bleibt unverändert; Ursache lesen und melden, dann entscheiden.
**E6 – Zeit-Test:** bauen (`tools/blueprint_bench.sh --check <schwelle>`). Aber: (1) keine Liste „bekannt über der Schwelle“, die der Test zum Bestehen
heranzieht – solange Fälle darüber liegen, ist der Test rot und läuft als Bericht auf mag, nicht als CI-Pflicht; in die CI kommt er, wenn kein Fall mehr
darüber liegt. Die offenen Fälle mit Ursache stehen in stand/p.md. (2) Rauschen: prüfe, ob das Verhältnis der KLEINSTEN Zeiten (beste von N Runden, beide
Seiten im Wechsel gemessen) stabiler ist als der Median – Rauschen macht nur langsamer; ist es das, die Schwelle daran so eng wie möglich begründen.

