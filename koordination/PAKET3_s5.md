Paket „s5“ (Stufe 5: Umschalten – der Mechanismus, an den ersten Dateien). Lies zuerst $S/PAKET_COMMON3.md (S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad) ganz, samt Nachtrag 15:40 (mag),
dann in kern/games/spawncity/BLUEPRINTS.md „5 – Umschalten“, „6 – Das Projekt laden“, „Regeln für jeden Schritt“ und kern/docs/blueprint_cpp.md.
Zweig work-s5, Worktree /home/claude/w-s5.

Ziel der Stufe: „Eine Datei schaltet um, sobald sie ganz durchläuft: Ihr C++ wird gelöscht, das Spiel läuft auf ihrem Blueprint, und ihre
bisherigen Tests prüfen den Blueprint.“ Bisher ist keine Datei umgeschaltet; der Mechanismus fehlt. Du baust ihn – allgemein (für jedes Spiel
auf Kern, nichts SpawnCity-Spezifisches in Engine/Werkzeugen) – und schaltest damit die ersten Dateien um.

1. Bestimme aus $S/final/f9/convert.log die Einheiten, die ganz geschrieben sind (alle Funktionen „written“) UND deren jede Funktion im Test
   verglichen ist; beginne mit Blättern (Bibliotheken ohne Zustand, die nichts Wartendes rufen), z. B. kleine Header/Quelldateien der Regeln.
2. Der Mechanismus: Das Spiel bindet für eine umgeschaltete Datei die aus dem Level erzeugte Klasse (tools/blueprint_cpp.sh →
   src/level_blueprints.cpp, kern/games/spawncity/build.sh und tests/run_tests.sh erzeugen sie schon) statt des C++ der Datei. Das restliche
   C++ ruft die Funktionen der Klasse direkt und typisiert (keine Werte verpacken, kein Nachschlagen beim Namen je Aufruf – volle
   Geschwindigkeit; die Erzeugung der Klasse schreibt dafür einen Header mit den Deklarationen in den Typen des Spiels, oder das Spiel nimmt
   die Typen der Klasse – finde die sauberste Form, eine, und beschreibe sie in BLUEPRINTS.md unter Stufe 5). Die Zahlen: die Sprache rechnet
   in 64 Bit (bindend); an der Grenze zum restlichen C++ gilt, was BLUEPRINTS.md „Zahlen an der Grenze“ festlegt (rules::number/decimal).
   Achte auf Runtime::budget (BLUEPRINTS.md: das Spiel muss die Schleifengrenze beim Umschalten setzen wie der Test).
3. Für jede umgeschaltete Datei: ihr C++ wird gelöscht (Header bleibt nur, soweit ihn nicht umgeschaltetes C++ für Typen braucht – Ziel: weg),
   ihre bisherigen Tests (tests/test_*.cpp) laufen gegen den Blueprint und bleiben grün, der Konverter-Lauf zählt die Datei nicht mehr als Quelle
   (der Blueprint im Level ist jetzt die eine Quelle; tools/cpp_to_blueprint.py darf ihn bei einem vollen Lauf nicht verwerfen – „what it does
   not write any more goes“ in main: kläre und löse das allgemein).
4. Beweis je Datei: `$S/mag.sh spiel origin/work-s5` grün (alle Tests des Spiels), `$S/mag.sh engine origin/work-s5` grün; Bildzeit: nenne,
   wie du misst (SC_TEST_RELEASE=1 … oder ein vorhandener Zeit-Test), vorher/nachher. e2e und Determinismus prüft CI – sag im Bericht, worauf zu achten ist.
5. Schalte so viele Blatt-Dateien um, wie sauber gehen (Ziel: der Mechanismus steht und ist an ≥3 Dateien verschiedener Art bewiesen: eine
   Bibliothek aus einem Header, eine Quelldatei mit Klasse/Objekt, eine, die die Engine ruft). Game selbst und Dateien, in denen noch etwas
   wartet, fasst du nicht an (die gehören g1/g2/f1/f2). Kleine Commits, pushen.
Bericht: der Mechanismus in fünf Sätzen, die umgeschalteten Dateien, die Beweise, was das Umschalten der übrigen Dateien noch braucht (Liste).

## Nachtrag 16:30 (Koordinator) – die Form ist entschieden
Lies $S/uebergabe/s5.md (Vorgänger: nur untersucht, nichts gebaut). Entschieden:
- **Das Spiel nimmt die Typen der Klasse – ein Typ je Struktur**, kein zweiter Typ des Spiels daneben, keine Umwandlung von Strukturen/Listen
  je Aufruf. Referenzen binden direkt. An der Grenze zu noch nicht umgeschaltetem C++ bleiben nur einzelne Zahlen (hinein rules::decimal,
  heraus static_cast<f32>), wie BLUEPRINTS.md „Zahlen an der Grenze“ festlegt. (rules::number gibt es im Quelltext nicht – korrigiere die
  Stelle in BLUEPRINTS.md auf das, was es gibt.)
- Erzeugter Header mit den Klassen unter festen, lesbaren Namen (aus demselben Generator wie level_blueprints.cpp – eine Quelle), ein einmal
  gebundenes Objekt je Klasse, direkter typisierter Aufruf. Kein Aufruf beim Namen, kein Suchen des Actors je Aufruf.
- Umgeschaltete Blueprints stehen im Level so, dass ein voller Konverter-Lauf sie nicht verwirft und nicht neu schreibt (sie sind jetzt die
  Quelle); der Konverter schreibt Aufrufe des restlichen C++ in eine umgeschaltete Datei als Aufruf dieser Bibliothek/dieses Blueprints.
- Runtime::budget setzt das Spiel wie der Test.
- Beginne mit qr_code (zwei Funktionen nach außen, eine Struktur, zwei Aufrufstellen), dann gps_live/perf. Dateien, die die Engine rufen
  (sound_fx …), erst wenn Paket d (direkte Engine-Aufrufe in der Klasse, Zweig work-d) gemergt ist – frag den Koordinator.
- Das Level im Repo muss dem heutigen Quelltext entsprechen, bevor du umschaltest: der neueste volle Lauf auf mag liegt als Branch
  mag/ergebnis-work-voll-* (erster Commit = Level und Tests); nimm das Level von dort, wenn mag-log/when „convert rc=0“ nennt und die
  Konvertierung wirklich lief (convert.log nennt Funktionen), sonst rechne `$S/mag.sh voll origin/work-s5`.
- Die bisherigen Tests der Datei (tests/test_*.cpp) prüfen danach den Blueprint. Wo ein Test wegen 64 statt 32 Bit einen anderen Wert sieht:
  nicht den Test weich machen – melde die Stelle dem Koordinator mit beiden Werten (das Maß ist spawncity.de, das in 64 Bit rechnet).
