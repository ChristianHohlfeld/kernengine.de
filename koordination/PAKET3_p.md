Paket „p“ (Leistung der exportierten Klasse). Lies zuerst $S/PAKET_COMMON3.md (S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad) ganz (Nachträge 15:40 mag, 16:20, 16:50), dann
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
