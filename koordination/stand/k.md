# Paket k – Stand (neuer Auftrag 22:25: Regeln des Konverters, die die Spiel-Suite rot machen)

| Abweichung | Ursache | Commit | Beleg | Stand |
|---|---|---|---|---|
| G4 flak_cell 201/255 | snprintf %lld eines i64 ging über int64_to_int (2^53) | 2a6ef5b3 | work-k-vergleich-222911-25281 | läuft |
| G7 Drone_step (u16 20268/85804) | `x++`/`x op= y` eines u16/u8 ohne Rückwandlung in den Typ (wrap32 statt band 65535) | 2a6ef5b3 | work-k-vergleich-222911-25281 | läuft |
| G5 Car_new 300/300 | kein Konverterfehler: `Applied::on` unbestimmt (C++ lässt offen) | – | Quelle car.h:110 | BEFUND FÜR t1 |
| E3-C (p) fc_Filters `List<fc_PT1>` in FirmwareRuntime_new | kein Konverterfehler: Felder sind Fixed<fc_PT1, 3>; ein Wertliteral einer Struktur schreibt ihre festen Felder als List (Sprache, notation.cpp literal()) – die Klasse legt dafür einen std::vector an | – | Kleinstfall /tmp/k_mini, lesen.py | BEFUND FÜR d/p |
| G6 CityStream feed_terrain/upload | – | – | wartet auf work-zusammen-vergleich-222045-20677 | offen |

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
