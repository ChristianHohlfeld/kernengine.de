# Übergabe Paket k („die Kette“), 18.11.2026, etwa 17:20

## Stand
- Zweig `work-k`, letzter Commit **e5f2bf1** (gepusht). Er liegt auf origin/work 955503b; origin/work ist inzwischen 9 Commits weiter. Vor dem Mergen muss rebased bzw. gemergt werden. Die Änderung ist lokal auf main/job_of begrenzt und sollte deshalb sauber gehen.
- Commits:
  - 0181688: Ablauf in Runden.
  - e5f2bf1: Meldung „<Einheit>: f() was written and waits now (<Grund>)“.
- Der Stand übersetzt (py_compile grün). Geändert ist nur tools/cpp_to_blueprint.py:
  - main: der Abschnitt „the Blueprints, each after those it calls“
  - job_of: need_of ist herausgelöst
  - neue Hilfsfunktion convert_timed nach convert_unit
- Unit und TestWriter sind unverändert.
- Zweig `work-k-zeit` (bc0ddda, vom Vorgänger) enthält nur Zeitmessung. Nicht mergen.

## Der Entwurf (was main jetzt tut)
- **convert_unit ist eine reine Funktion seiner Eingabe** (job_of). Die Eingabe besteht aus:
  - den Dateien der Einheit,
  - wanted[u] (nur bei Engine-Headern),
  - tables[d] und exports[d] für alle d in need(u). need(u) ist die transitive Hülle von deps[u]; im Zyklus gehört u selbst dazu.
- **Fassung einer Einheit:** `gave[u]` ist die Liste ihrer verschiedenen Ergebnisse (table, exports), verglichen mit ==. `version[u]` ist der Index des aktuellen Ergebnisses darin.
- **Signatur:** `asked_of(u)` = (Dateien, wanted, ((d, version[d]) für d in sorted(need))). Die Signatur legt die Eingabe exakt fest, ganz ohne Hash.
- **Neu umwandeln:** nur wenn die Signatur sich gegenüber `given[u]` (Signatur der letzten Umwandlung) geändert hat. Gleiche Eingabe heißt gleiches Ergebnis, nichts wird doppelt gerechnet. Es gibt keinen Zwischenspeicher zwischen Läufen.
- **Runde:**
  - `new` = die Einheiten mit geänderter Signatur.
  - `now` = die Einheiten aus `new`, bei denen kein d in need(u) ∩ new außerhalb des eigenen Zyklus liegt (`u in needs[d]` heißt: gleicher Zyklus).
  - Alle jobs von `now` werden aus demselben Stand gebaut und zugleich abgegeben, die längsten zuerst: nach der Dauer vom letzten Mal (`took`), beim ersten Mal nach der Dateigröße. Die Reihenfolge der Abgabe ändert am Ergebnis nichts.
  - Erst wenn alle fertig sind, werden die Ergebnisse in Namensreihenfolge eingesetzt.
  - Danach werden die „missing“ ausgewertet: deps[u] bekommt d dazu; ist d ein Engine-Header und fehlt key in tables[d], kommt key nach wanted[d]. Ist d noch unbekannt, wird es angelegt.
- **Was wegfällt:** cut_calls, „goes first“, „is converted again“, reaches.
- **Sicherung:** Kehrt ein Gesamtstand (asks, given) wieder, endet der Lauf mit SystemExit und den Namen.
- **Protokoll:**
  - je Runde eine Zeile „round N: K Blueprints“,
  - am Ende je Einheit einmal das Endergebnis, sortiert nach Namen, mit r["errors"], der Zeile „n / m functions“ und mit FUNCTIONS je Funktion „written“ oder dem Grund,
  - die Summenzeilen unverändert.

## Warum der Lauf endet und warum das Ergebnis der größte Stand ist (so dem Koordinator gesagt)
- deps und wanted wachsen nur.
- In den Runden wachsen die Einträge der Partner: aus „fehlt“ wird „wartet“, aus „wartet“ wird „geschrieben“, solange die Umwandlung monoton ist. Dann bilden die Runden (Jacobi von unten) eine aufsteigende Kette über endlich viele Funktionen. Sie endet im kleinsten Fixpunkt.
- Jeder Ablauf „eine nach der anderen“, wie im alten Lauf, bleibt unter diesem Fixpunkt. Beweis per Induktion: aus x ≤ lfp folgt F(x) ≤ F(lfp) = lfp. Was der Basislauf schreibt, schreibt also auch der neue Lauf. Keine Funktion wartet nur, weil ihr Partner später dran war. Echte Rekursion über Teile hinweg bleibt wartend, wie eine Funktion, die sich selbst ruft.
- Monotonie wird **nicht erzwungen**. Erzwingen hieße, ein altes Ergebnis festzuhalten, und das wäre ein Eingriff in das, was geschrieben wird.
- Ein Bruch der Monotonie wird gemeldet: „<Einheit>: f() was written and waits now (…)“. Das ist ein Befund am Inhalt für die Pakete an Unit/TestWriter. Ein bekannter Kandidat aus f9: Game/game_look_cpp fällt von 7/13 auf 0/13, sobald game_vr fertig ist, weil der Test dann als Ganzes nicht mehr übersetzt.
- Schwingt etwas, greift die Sicherung mit Namen. Das ist dann ebenfalls ein Befund, kein vorgesehener Ausgang.

## Entscheidungen und Vorgaben des Koordinators (sinngemäß)
- Nachtrag 16:40, in dieser Reihenfolge:
  1. eindeutig: gleiches Ergebnis bei jeder Kernzahl
  2. schnell
- Beweis:
  - zwei volle Läufe mit verschiedener Kernzahl müssen Level, tests/converted und convert.log byteweise gleich liefern,
  - gegenüber dem Basislauf ist keine Funktion verloren; was mehr geschrieben ist, wird aufgelistet,
  - die Dauer vorher und nachher wird genannt.
- Neue Auftragsart vom Koordinator: `$S/mag.sh konvert origin/work-k <kerne>`. Sie rechnet nur die Konvertierung des ganzen Spiels mit genau dieser Kernzahl. Der erste Commit von mag/ergebnis-<name> enthält Level und tests/converted, dazu mag-log/convert.log. Verglichen werden die Bäume der beiden ersten Commits. Die Auftragsart greift, sobald auf mag die neue Schleife läuft (nach dem vollen Basislauf).
- Zum Protokoll: je Funktion „written“ oder ihr Grund und die Summenzeilen müssen bleiben, weil andere Skripte sie lesen. Das ist erfüllt.
- Zum Enden: Die Sicherung darf nur Sicherung sein. Das Enden muss begründet werden (siehe oben), ein Schwingen ist als Befund mit Namen zu melden.
- Nachtrag 17:10: sparsam arbeiten. Der Stand steht in $S/stand/k.md. Nachrichten an main nur für eine Entscheidung, für „bereit zum Mergen“ mit Beweis oder für einen Befund, der andere betrifft.

## Bewiesen
- Lokal, klein: src/rules.cpp, spawn_guard.cpp, wanted.cpp mit KERN_JOBS=1, alter gegen neuen Konverter.
  - Ergebnis: Level, tests/converted, texts und das Log (ohne die Ablaufzeilen, sortiert) sind identisch („GLEICH“).
  - Ablauf: 2 Runden zu je 4 Einheiten.
  - Ablage: $S/k/lokal/ (c_old.log, c_new.log, t_*, texts_*, l_*.klevel).
- Hinweis zum Test-Verzeichnis: Es muss neben converted.h liegen (z. B. tests/converted_k im Spielordner, danach entfernen). Sonst steht „converted.h: No such file“ in allen Gründen.

## Unbewiesen / wartet (mag-Aufträge, nichts mehr abfragt)
- `work-k-konvert-165842-30470`: konvert origin/work-k 8 (abgelegt auf 0181688 oder e5f2bf1, je nachdem, wann mag ihn holt).
- `work-k-konvert-165847-30555`: konvert origin/work-k 40.
- `work-k-probe-162316-2567`: Probe über game.cpp, game_lamps, game_look.
- Ergebnisse kommen als Branches `mag/ergebnis-<name>`. Abfrage: `git ls-remote origin 'refs/heads/mag/ergebnis-work-k*'`. mag hing bis 17:10 an anderen Aufträgen und am vollen Basislauf `mag/ergebnis-work-voll-153849-12713` (auf 955503b, Start 15:39; um 17:00 noch ohne „convert rc=0“). Dieser Basislauf ist der Vergleich für „nichts verloren“ und liefert die Vorher-Dauer (mag-log/when, „real“ in convert.log).
- Wurden die Aufträge auf e5f2bf1 gerechnet, gelten sie für den heutigen Stand. Wurden sie auf 0181688 gerechnet, liegt der einzige Unterschied in der Meldezeile; Level und Tests sind davon nicht berührt.
- Nächste Schritte:
  1. Die ersten Commits der beiden konvert-Branches vergleichen: `git diff --stat A B`, der Baum muss gleich sein. Dazu convert.log beider Läufe ohne „round“-Zeilen und ohne „real/user/sys“ vergleichen.
  2. Gegen den Basislauf: je Einheit den letzten Block nehmen und prüfen, dass jede dort „written“-Funktion auch neu „written“ ist. Mehr Geschriebenes auflisten.
  3. Die Meldungen „was written and waits now“ heraussuchen und als Befund an main geben.
  4. Die Dauer nennen.
  5. „bereit zum Mergen“ an main.
  - Für den Abschlussbericht: Zweig + Commit, was geändert ist, Beweis, Dauer vorher/nachher, zusätzlich geschriebene Funktionen.

## Fallen
- Der alte Basislauf hängt selbst vom zeitlichen Ablauf ab und druckt Einheiten mehrfach. Vergleichbar ist nur der letzte Block je Einheit plus die Schlusszeilen. Im Wortlaut „is not written“ gegenüber „which waits“ kann er vom neuen Lauf abweichen; das ist kein Fehler des neuen Ablaufs.
- Wird im SCC erstmals umgewandelt, sehen die Partner noch keine Tabellen. Jede Einheit im Zyklus wird deshalb mindestens zweimal umgewandelt: Ihre eigene Tabelle ist Teil ihrer Eingabe (need enthält u), so wollte es job_of schon vorher.
- Wandzeit ≈ Summe über die Runden der jeweils langsamsten Einheit. Dauert ein Game-Teil allein lange (Baum per clang bei jeder Umwandlung neu, bis zu 400 g++-Runden im Test), bleibt das die Grenze. Mögliche spätere Verbesserung: den Baum je Einheit über die Runden halten (ein Prozess je Einheit). Das ist innerhalb eines Laufs erlaubt, vorher aber den Speicher auf mag prüfen.
- PYTHONHASHSEED ist je Lauf zufällig. Die Worker werden geforkt und teilen ihn. Hängt Unit/TestWriter irgendwo an der Reihenfolge eines Mengen-Durchlaufs, unterscheiden sich zwei Läufe unabhängig vom Ablauf. Weicht der 8/40-Vergleich ab, zuerst dort suchen; das wäre dann ein Befund für die anderen Pakete.
- Nicht in den Ablauf gehören (Reihenfolge-Lauf der Teststücke): „Teststücke einzeln parallel übersetzen“ geht nicht. Ein Test ist eine Übersetzungseinheit, und die g++-Runden in TestWriter.write hängen voneinander ab.
