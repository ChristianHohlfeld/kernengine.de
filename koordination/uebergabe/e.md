# Übergabe Paket e (Rekursion) – 19.11., 08:45

Entscheidung E16 (Koordinator, stand/KOORDINATOR.md 07:35): Rekursion als Schritt (func.call); ein Kreis über eine Funktion, die als Wert
gelesen wird (pure), bleibt ein Fehler. Grenze 65.536 geschachtelte Funktionsaufrufe in Interpreter und JIT, am selben Aufruf, mit festgelegtem
Fehler (Konstante in der Doku; Events und call_on bleiben bei 64). Interpreter und JIT werden über ein Sprungbrett vom Maschinenstapel unabhängig.
Die Klasse ist ein gewöhnlicher Methodenaufruf ohne Zähler. Endlos = „C++ leaves open“: der Aufruf wird aufgegeben und neu gezogen.
Reihenfolge 1–5, jeder Schritt mit Beleg.

## Schritt 1 – neue Form der Klasse: FERTIG, Beleg läuft (work-e @ a020e227)
- notation.cpp:
  - `Shape::locals()` (nach der Bestimmung der Mitglieder in build): Ein Schritt einer Funktion (Ort kCallable mit func.entry), ohne Gedächtnis
    oder eine Schleife (def->loop), wird lokal (`Info::local_in` = Entry, member=false), wenn zwei Dinge gelten: Alle Schritte, die ihn lesen
    (über pure Werte verfolgt), stehen am selben Ort. Und keiner ist vom Entry aus über Exec-Kanten erreichbar, ohne an dem Schritt vorbeizugehen
    (Dominanz). `Info::kept()` = member || local.
  - Writer::body schreibt die Deklarationen (member_text) als erste Zeilen des Rumpfs, nach den lokalen Variablen, nach Id geordnet.
  - Leser, erster Durchgang bei Item::function nach den lokalen Variablen: `XResult n<id>;`, `struct {…} n<id>;`, `struct : kern::Node<id> {…} n<id>;`,
    `kern::Node<id> n<id>;` mit `MemberDecl::local`. Die Schleife der lokalen Variablen nimmt dafür keine Namen n<digits>.
  - checked(): Ein lokaler Knoten darf auch als Mitglied der Klasse deklariert sein. Das ist die Form der Konverter-Texte und alter Texte, also
    derselbe Blueprint. Eine Deklaration im falschen Rumpf weist der Leser ab („… it is a member of the class …“).
- Engine-Fall test_main.cpp `logic_notation_values_of_a_call`; Doku blueprint_cpp.md: eine Tabellenzeile und der Absatz „Werte eines Aufrufs“.
- Lokal belegt: Rundreise aller 43 logic_cases und des Fib-Graphen sind Fixpunkte; Varianten richtig, alt und falsch.
- Das Level hält Graphen (JSON), keine Texte: Der Bench schrieb mit diesem Stand alle Klassen aus dem alten Level. Kein konvert nötig.
- Zeit (stand/e.md): geometrisches Mittel 2,100 → 2,086. lamps_near 2,4 → 2,85 ist im Mittel schlechter, die Ursache ist nicht eindeutig
  (Codelage, siehe Gegenproben).
- Noch offen: mag `work-e-engine-080904-22468` (Engine-Suite), danach `./mag.sh vergleich origin/work-e src/car.cpp src/drone.cpp src/city_stream.cpp`.
- Achtung für Paket p: Ein `struct { List<…> } n<id>` als lokaler Wert wird jetzt je Aufruf neu angelegt, wie im C++ der Quelle. Vorher war es
  ein Mitglied, das den Puffer behielt.

## Schritt 2 – Interpreter: GEBAUT, NICHT BELEGT (Zweig work-e-tramp @ b482049a, auf a020e227; nicht in work-e)
- logic.h Instance:
  - `Kept` (Parameter, Locals, Ausgänge und NodeState der Schritte, ihre 8 Slots je Knoten, Entry zuerst).
  - Frame bekommt trampoline, call_node, base, outer_refs und kept.
  - `kAfterCall = -1000`; `enter_call`/`leave_call`/`begin_call`/`end_call`; `fn_running_`, `fn_nodes_` (die Schritte, die Exec vom Entry erreicht,
    ohne Gedächtnis außer Schleifen) und `calls_fn_`.
  - Runtime: `calls_`, `static constexpr int kCallsDeep = 65536`.
- logic.cpp:
  - call_function ist in enter_call und leave_call geteilt. Der Merkstand entsteht, wenn `fn_running_[f] > 0`.
  - run_from (Interpreter-Zweig): Ein Knoten mit calls_fn_ ≥ 0 geht über begin_call (Marke (n, kAfterCall) auf den Stapel, weiter am Entry).
    Beim Abheben der Marke folgt end_call (Ausgänge setzen, weiter nach dem Knoten). `own_returned` kürzt den Stapel bis zur Marke.
  - Beim Ende der Kette werden übrige Trampolin-Rahmen abgebaut. Der Trace des Aufrufknotens steht am Ende, wie zuvor.
- Gemessen lokal, die Leser-Sperre nur für die Messung aus ($S/e/mess_rekursion.cpp, Graph Deep/Fib/Loop):
  - 0 B Maschinenstapel je Ebene, vorher 1.504 B.
  - Fib(15) = 610 (vorher −195), Loop(3) Count = 30 wie C++.
  - Tiefe 60.000 geht, bei 70.000 stoppt der 65.537. Aufruf mit Fehler, der Blueprint ist gestoppt.
  - Alle 43 logic_cases: die Ausgabe des Interpreters (play) ist byte-gleich mit 821208ac.
  - Aufruf ohne Wiedereintritt: valgrind 338,3 → 331,4 Mio. Befehle (20.000 Aufrufe; Mikrobench /tmp/e/b/bcall.cpp, nicht gesichert).
- Fehlt als Beleg: die Engine-Suite und die Spiel-Suite auf diesem Zweig. Diese Maschine darf kein wasi-sdk nutzen; den JIT nur über die CI belegen.
- Wichtig: Solange der JIT nicht umgestellt ist, ruft der JIT func.call weiter verschachtelt (call_node → Lambda `call` → call_function).
  Der Merkstand greift dort auch, denn er sitzt in enter_call und sichert auch Slots.

## Schritt 3 – JIT (Entwurf, nicht begonnen) – codegen.cpp (533 Zeilen), logic.cpp call_node/run_compiled
1. Stapel wachsen lassen: Der erzeugte Code backt heute `stack_` als feste Adresse ein (push/pop in codegen.cpp `push()` und `body()`).
   - Vorschlag: eine Zelle in Instance (fester Ort, z. B. `i32 stack_at_[2]` = {Adresse, Kapazität}). Der Code lädt die Basis bei jedem
     push/pop aus der Zelle.
   - Ist der Stapel voll, ruft der Code `call_node(id, -2, 0)`: Die C++-Seite vergrößert stack_ und schreibt die Zelle neu. So braucht es keinen
     neuen Import und kern.mjs bleibt unverändert.
   - Auch Instance::call_node (die Fortsetzungen der C++-Knoten) muss wachsen statt still abzuschneiden.
   - Die feste Schranke „66 × offen“ reicht mit Rekursion nicht. Eine Schranke kCallsDeep × offen wäre im Kreis command/ui_* (21 Funktionen)
     hunderte MB, deshalb wachsen.
2. func.call im erzeugten Code: `r = call_node(id, pc, entry)`. C++ call_node prüft vor `def->run` auf `calls_fn_[node] >= 0`:
   - Ist entry == kAfterCall: end_call, zurück 0.
   - Sonst wie begin_call, aber die Marke kommt auf stack_ und frame.stack_base = frame.base = Tiefe nach der Marke. Zurück kEnter (z. B. 1000).
   - Der Code springt bei r == kEnter an den pc von fn_entry_[f] (zur Übersetzungszeit bekannt), entry 0; sonst wie bei jedem C++-Knoten.
   - Return: return_values setzt stack_[0] = stack_base. Dann ist die Marke das Nächste.
3. run_from, Zweig compiled: nach run_compiled übrige Trampolin-Rahmen abbauen wie im Interpreter.
4. Beleg: `gh workflow run ci.yml --ref work-e…`, Job mit „Logic JIT“ (ci.yml:113–117), Ergebnis mit `gh run list/view` (REST).
   Die Fälle unter 5 müssen dort laufen: Interpreter = JIT.

## Schritt 4 – Leser/check, Fälle, Doku
- `functions_ok` (notation.cpp, Abschnitt „a function that calls itself“): Der Zyklus mit func.call ist erlaubt. Ein Kreis, der über eine Kante
  aus func.pure läuft oder eine Funktion enthält, die als Wert gelesen wird (value[f]), bleibt ein Fehler mit neuer Meldung. Eine Funktion in
  einem Kreis, deren Schrittwerte Mitglieder sind (Info::member eines Schritts im Rumpf, also außerhalb ihres Wegs gelesen), weist check() ab.
- `places_ok`: Die Reihenfolge „Gerufene zuerst“ gilt nicht mehr. writes/raises bis zum Fixpunkt iterieren.
- Kommentar in call_function und Doku Z. „Eine Funktion läuft nicht noch einmal …“ (Regeln) und „Wie tief es geht“ neu: 65.536, Fehlertext.
- logic_cases.h: `recursion_direct` (Fib(15)=610, Local, Referenz, For über den inneren Aufruf), `recursion_through` (A→B→A, Sequence offen),
  `recursion_deep` (5.000 Ebenen), `recursion_endless` (Fehler am selben Aufruf, Blueprint gestoppt).
  - `Case` bekommt ein Feld „C++ lässt offen“, damit blueprint_native_test diesen Fall nicht in der Klasse rechnet.
  - Dazu die Prüfungen in test_main.cpp und ein Fall „Pure-Rekursion abgewiesen“. Dafür muss der Fall mit jedem Knoten und die Palette nicht
    geändert werden (keine neuen Knoten).
- Grafen für die Fälle: siehe $S/e/mess_rekursion.cpp (Deep, Fib, Loop; JSON wie logic_cases).

## Schritt 5 – Konverter
- `cpp_to_blueprint.py:2332`: `raise Wait("… calls itself (a Blueprint's function does not)")` fällt weg.
- Ablauf (Kommentar ab 13826, `pinned`/„went round“ ab 13882): Ruft eine Funktion einen Teil desselben Blueprints, der nur im Kreis mit ihr
  wartet, gilt der Aufruf mit der Gestalt seines Eintrags (params/kinds/result) als geschrieben. Am Fixpunkt wird geprüft. Wartet der Gerufene aus
  eigenem Grund, wartet der Rufer mit.
- Eine Funktion in einem Kreis wird nie KERN_PURE geschrieben.
- Kreise im Spiel ($S/e/zyklen.py):
  - {explosion, combat_blast}: game_town, game_military.
  - {command, ui_command, ctl_discrete, foot_input, hud_json, place_world, read_input, rebase_origin, ui_apply_settings, ui_gps_update,
    ui_move_world, ui_reset, ui_update, update, update_net, update_town, vr_foot, vr_menu_input, web_audio_command, web_audio_state,
    world_rules_json} (21).
  - wreck_car/blast_cars sind kein Kreis.
- Beleg: `./mag.sh konvert origin/work-e 9` (rc=0, die „went round“-Funktionen geschrieben), dann vergleich über game.cpp, game_town,
  game_military, game_controls, game_ui, game_audio_web.

## BEFUND FÜR t1 (nicht selbst ändern)
- Meldet der Interpreter „function calls nest more than 65536 deep“, markiert der Vergleichstest den Aufruf als offen (g_open/g_bail,
  converted.h:1610ff.) und zieht neu, nie still gleich.
- Eine Quelle, die endlos rekursiert, lässt sich nicht abfangen. Bei gezogenen Eingaben endet sie, weil jede Ebene ein Flag verbraucht
  (k.wreck vor explosion).

## Zahlen des Entwurfs
- Interpreter vorher 1.504 B je Ebene (nativ -O2), Absturz bei 8 MiB nach rund 5.500 Ebenen.
- C++: explosion 248 B + combat_blast 152 B; WASM-Stapel 1 MiB (kern/build.sh:153).
- Tiefe von explosion ≤ 1 + Panzer, die nicht wreck sind (keine feste Schranke).
