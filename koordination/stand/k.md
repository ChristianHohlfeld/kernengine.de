# Paket k – Stand (neuer Auftrag 22:25: Regeln des Konverters, die die Spiel-Suite rot machen)

## BEREIT ZUM MERGEN: 20cb5259 (enthält 2a6ef5b3, 12e3da24) – Beleg work-k-vergleich-224717-8097
- combat_rules.cpp, drone.cpp, city_stream.cpp auf 20cb5259, beide Durchgänge: converted_combat_rules_cpp ok (flak_cell, G4),
  converted_drone_cpp ok (Drone_step, G7), CityStream_upload keine Abweichung mehr (G6); 17 ok von 19.
- Übrig dort, nicht von diesen Commits: CityStream_buildings_near 5/300 (u64-Schlüssel – Fix fa3dc97d, Beleg läuft) und
  rigid_spec/create_body (BEFUND FÜR d, inertia[6]); LECK bei city_stream(_cpp) wie im Lauf 222045 (vor t1s Testrahmen).
- 12e3da24 ist ein Ausweichen vor dem Fehler der Klasse (siehe unten) – zurücknehmen, sobald ps Fix in work-zusammen ist.

| Abweichung | Ursache | Commit | Beleg | Stand |
|---|---|---|---|---|
| G4 flak_cell 201/255 | snprintf %lld eines i64 ging über int64_to_int (2^53); danach ASan use-after-free (Text-Temporär in text_format) → Text in eigener Variable | 2a6ef5b3 + 12e3da24 | work-k-vergleich-224717-8097 | belegt |
| G7 Drone_step (u16 20268/85804) | `x++`/`x op= y` eines u16/u8 ohne Rückwandlung in den Typ (wrap32 statt band 65535) | 2a6ef5b3 | work-k-vergleich-222911-25281: converted_drone_cpp ok in beiden Durchgängen | belegt (auch 224717-8097) |
| G5 Car_new 300/300 | kein Konverterfehler: `Applied::on` unbestimmt (C++ lässt offen) | – | Quelle car.h:110 | BEFUND FÜR t1 |
| E3-C (p) fc_Filters `List<fc_PT1>` in FirmwareRuntime_new | kein Konverterfehler: Felder sind Fixed<fc_PT1, 3>; ein Wertliteral einer Struktur schreibt ihre festen Felder als List (Sprache, notation.cpp literal()) – die Klasse legt dafür einen std::vector an | – | Kleinstfall /tmp/k_mini, lesen.py | BEFUND FÜR d/p |
| G6 CityStream_upload 29/243 | `m[key].push_back(x)`: variable_of hob `put = t_2` (Zurückschreiben) vor `t_2 = m[key]` – zurück ging der alte Inhalt | 20cb5259 | work-k-vergleich-224717-8097 | belegt |
| CityStream_buildings_near 5/300 (t1) | Schlüssel u64 eines Map-Eintrags (`[k, t]`) ging als Int (Compiler nennt `unsigned long`) | fa3dc97d | work-k-vergleich-230203-14801: converted_city_stream_cpp ok in beiden Durchgängen | belegt |
| G6 CityStream_feed_terrain | im Lauf 222045-20677 nicht geschrieben (wartet) – nichts zu vergleichen | – | – | offen |

## BEFUND FÜR t1
- **Car_new 300/300 (converted_car):** `struct Applied { Vec3 p, f; i32 on; }; Applied applied_[6];` (car.h:110f.) hat für `on`
  keinen Initialisierer; `explicit Car(const CarSpec& s) : s_(s) {}` lässt `applied_[i].on` unbestimmt (default-initialisiert,
  C++ [dcl.init]/[basic.indet]). Der Quelltext gibt einen Speicherrest (8), der Blueprint 0. Der Test vergleicht einen Wert, den
  C++ offen lässt – „C++ leaves open“: nicht vergleichen (oder die Funktion so begründet warten lassen), keine Konverter-Regel.

## BEFUND FÜR d/p (E3-C, von p an k gegeben)
- Der Konverter typt die Felder richtig: `KERN_STRUCT(fc_Filters) { Fixed<fc_PT1, 3> gyro = List<fc_PT1>{…}; … }`. Die Liste in
  `fc_FirmwareRuntime{fc_StateRuntime{…fc_Filters{List<fc_PT1>{…}, …}}}` (FirmwareRuntime_new, Result-Vorgabe, `self = …`) ist das
  Wertliteral der Sprache: ein Strukturwert gibt jedes Feld, eine Liste darin ist `List<T>{…}` (blueprint_cpp.md Tabelle der Werte;
  notation.cpp `literal()` schreibt es so). Versuch (Kleinstfall /tmp/k_mini, lesen.py): `Fixed<PT, 3>{…}` im Wertliteral weist der
  Leser ab („a list of a fixed length is what a place is declared as - a value written out is List<PT>{...}“). Die Anforderung
  entsteht also in der Klasse: das Literal wird ein std::vector und dann in Fixed kopiert. Allgemeiner Weg (Klasse, nicht Konverter):
  ein Listenliteral an der Stelle eines festen Felds als `{…}` (Fixed hat einen initializer_list-Konstruktor) bzw. wie Make List
  in place schreiben. Nicht mein Gebiet – nicht umgebaut.

## BEFUND FÜR d
- **converted_rigid_spec / create_body 300/300** (work-k-vergleich-222911-25281, beide Durchgänge): call 6 der Engine
  `world.set_mass_data(b, mass, center, inertia)` – der Stellvertreter des Quelltexts zeichnet von `const f32 inertia[6]`
  (kern/include/kern/physics/physics.h:256) 1 Zahl auf, der Blueprint gibt 6. Länge eines Zeiger-Eingangs ohne Count (wie Gruppe 3).
- **Klasse text_format** (kern/include/kern/logic/blueprint.h:571ff.): `format_arg(const Text&)` macht eine Sicht; ein Wert, der kein
  Text ist (der std::string von PureNodes::int64_text*), wird dafür in einen vorläufigen Text umgewandelt, der vor `format::write`
  vergeht (ASan heap-use-after-free, 222911-25281 combat_rules_cpp). Der Konverter umgeht es jetzt (Text in eigener Variable,
  12e3da24); allgemein gehört ein `format_arg(const std::string&)` (Sicht auf den Wert selbst) in die Klasse.

## Zu 12e3da24 (Frage des Koordinators 22:5x)
- Was es ändert: der Text eines Int64 in einem Format wird vorher in eine eigene Text-Variable gesetzt (`digits = int64_text_signed(cx);
  key = text_format<"flak:l%s:%s">(63, digits, digits_2);`) statt direkt als Wert übergeben. Bedeutung gleich, eine Variable mehr.
- Urteil: **nur Ausweichen** vor dem Fehler der Klasse. Ich nehme es zurück, sobald ps Fix in work-zusammen ist (dann wieder
  `text_format<…>(63, int64_text_signed(cx), …)`), und belege das mit einem Vergleich über combat_rules.cpp.
- Kleinstfall für den Engine-Fall (Klasse, ASan): `kern::BlueprintCore::text_format<"%s">(63, kern::logic::PureNodes::int64_text_signed(
  kern::logic::Int64{0x69d6b3a6c1e2f7ee}))` – genauer: jeder Wert für `%s`, der kein `Text` ist (hier std::string, lang genug für den
  Heap, z. B. 19 Ziffern), wird in `format_arg(const Text&)` in einen vorläufigen Text gewandelt; `args[]` hält eine Sicht darauf, der
  Text vergeht am Ende der Initialisierung von args → heap-use-after-free in format::Sink::put (nodes.h:894), gesehen in
  work-k-vergleich-222911-25281, converted_combat_rules_cpp (flak_cell). Allgemeine Behebung: `format_arg(const std::string& v)`
  → `arg(std::string_view(v))` (der Temporär-String lebt als Argument von text_format bis zu dessen Ende).

## (entschieden: E9 – (b) ja, (a) später) Destruction_collapse 7/25, Destruction_fracture 11/20 / 12/28 (Lauf 222045-20677)
- Befund: Interpreter = Klasse ≠ Quelltext, erste Stelle: `pieces_[k].size` (x und z), p und v gleich. Ursache ist **keine Regel**:
  destruction.cpp:191/194 `launch({…}, {s.len * rng.range(0.85f, 1), 0.45f, band * rng.range(0.8f, 1)}, s.yaw, tint(0.18f));`
  – zwei Argumente ziehen aus demselben `rng` (size über rng.range, tint über rng.unit). In welcher Reihenfolge C++ die Argumente
  eines Aufrufs ausrechnet, lässt es offen ([expr.call]/8: indeterminately sequenced). GCC (der Test) rechnet hier von rechts
  (tint zuerst), der Blueprint – wie clang und wie das Original in JS – von links. Darum stimmen size.x/size.z nicht.
- Vorschlag: (a) **Konverter, allgemein:** ein Aufruf, bei dem mehr als ein Argument etwas ändert, das ein anderes Argument liest
  oder ändert (Aufrufe mit Wirkung, ++, Zuweisung), wartet mit „C++ leaves open in which order the arguments of a call are worked
  out“ (wie clear() bei 4068). Dafür braucht es eine Wirkungsanalyse der Argumente (Lambda mit Referenz-Fang wie `tint` zählt).
  (b) Die Quelle (destruction.cpp, Paket f2) wird gesenkt: die Werte in der Reihenfolge des Originals (links nach rechts) benannt,
  vor dem Aufruf. Das ändert ihr Verhalten unter GCC – der Differenztest (tools/source_diff.py) kann dann nicht grün sein,
  denn die heutige Quelle hat unter GCC ein anderes Ergebnis. Die Entscheidung, welches Verhalten gilt, liegt beim Koordinator/Chris.
  Ich schlage (a) + (b) vor: (a) macht jede solche Stelle sichtbar, (b) legt die Reihenfolge des Originals fest (Differenztest mit
  clang als Bezug oder als dokumentierte Ausnahme – das braucht eure Entscheidung).

## E9 umgesetzt: 90ecf01f – Absenkung destruction.cpp (Argumentreihenfolge, clang = von links)
- fracture: size/Farbe bzw. at/size/yaw/Farbe vor launch benannt; in launch vx, vy, up. Differenztest tests/lowered/test_destruction_lowered.cpp
  (1 Paar: Destruction::fracture). Kleinstfall /tmp/k_order/order.cpp: clang++ alt = neu, g++ alt ≠ neu.
- WARTET AUF: t1s Umstellung (tests/lowered mit clang) – dann Differenztest grün belegen, erst dann BEREIT ZUM MERGEN für 90ecf01f.
  Danach `vergleich` über destruction.cpp (unter GCC sollten collapse/fracture dann gleich sein).

## BEREIT ZUM MERGEN: fa3dc97d (+ Merge ad36eee9) – Beleg work-k-vergleich-230203-14801
- city_stream.cpp, destruction.cpp auf ad36eee9 (mit t1s Testrahmen): converted_city_stream_cpp ok in beiden Durchgängen (buildings_near,
  upload), 11 ok von 12, keine LECK. Übrig nur converted_destruction_cpp (collapse/fracture/blast: pieces_[i].size/.p – die
  Argumentreihenfolge, E9; die Absenkung 90ecf01f kam nach diesem Stand). Beleg dafür: work-k-vergleich auf 90ecf01f (destruction.cpp).
