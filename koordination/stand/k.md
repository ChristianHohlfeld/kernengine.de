# Paket k – Stand (neuer Auftrag 22:25: Regeln des Konverters, die die Spiel-Suite rot machen)

| Abweichung | Ursache | Commit | Beleg | Stand |
|---|---|---|---|---|
| G4 flak_cell 201/255 | snprintf %lld eines i64 ging über int64_to_int (2^53); danach ASan use-after-free (Text-Temporär in text_format) → Text in eigener Variable | 2a6ef5b3 + 12e3da24 | 222911-25281: ABBRUCH (use-after-free); neu work-k-vergleich-224717-8097 | läuft |
| G7 Drone_step (u16 20268/85804) | `x++`/`x op= y` eines u16/u8 ohne Rückwandlung in den Typ (wrap32 statt band 65535) | 2a6ef5b3 | work-k-vergleich-222911-25281: converted_drone_cpp ok in beiden Durchgängen | belegt (Bündel-Beleg 224717 folgt) |
| G5 Car_new 300/300 | kein Konverterfehler: `Applied::on` unbestimmt (C++ lässt offen) | – | Quelle car.h:110 | BEFUND FÜR t1 |
| E3-C (p) fc_Filters `List<fc_PT1>` in FirmwareRuntime_new | kein Konverterfehler: Felder sind Fixed<fc_PT1, 3>; ein Wertliteral einer Struktur schreibt ihre festen Felder als List (Sprache, notation.cpp literal()) – die Klasse legt dafür einen std::vector an | – | Kleinstfall /tmp/k_mini, lesen.py | BEFUND FÜR d/p |
| G6 CityStream_upload 29/243 | `m[key].push_back(x)`: variable_of hob `put = t_2` (Zurückschreiben) vor `t_2 = m[key]` – zurück ging der alte Inhalt | 20cb5259 | work-k-vergleich-224717-8097 | läuft |
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
