# Funktionen der Engine: gerufen statt nachgerechnet – Bestand und Umbauplan (Agent projekt, 26.11.; NICHT gebaut)

Entscheidung des Koordinators (i): Eine Inline-Funktion aus einem Header der Engine ist im Blueprint ein Knoten, der die
Funktion der Engine selbst ausführt (in der Klasse der direkte Aufruf, im Interpreter der native Aufruf).
Grundsatz für den Entwurf (blueprint_cpp.md „Zahlen“, nachzutragen): Skalare der Sprache haben 64 Bit; Typen der Engine
(Vec3, Mat4, Quat …) haben die Genauigkeit der Engine; gerundet wird beim Übergang.

## Bestand (work-projekt 36bbc39b)
- Die Engine-Bibliotheken kommen aus der Engine: `tools/engine_api.py --libraries` ruft `cpp_to_blueprint.py --engine` über
  die Header und schreibt `kern/src/logic/engine_libraries.gen.cpp` (je Bibliothek der Graph als JSON-Text). 20 Bibliotheken,
  13 davon mit Funktionen, **87 Funktionen als Graph der Sprache**, 44 davon mit Float- oder Vector-Pins:
  - engine_math_math 44 (35 mit Zahlen: dot, cross, length, length_sq, distance, the_lerp, Quat_axis_angle/mul/rotate,
    Mat4_translation/mul/transform_point/transform_vector/translate/scale/trs/perspective/ortho/look_at, projection_matrix …)
  - engine_geo_mesh_data 8 (3), engine_gfx_shader 9 (0), engine_physics_physics 7 (0), engine_logic_logic 4 (0), engine_app_app 3,
    engine_net_transport 3 (1), engine_audio_audio 2 (2: db_to_gain, gain_to_db), engine_core_base 2, engine_geo_roads 2 (2),
    engine_geo_buildings 1 (1), engine_game_runtime 1, engine_gfx_mesh 1.
- Das SpawnCity-Level (alt, vor dem vollen Lauf) hält 19 Kopien engine_*, davon mit Funktionen: engine_math_math 17,
  engine_geo_mesh_data 5, engine_physics_physics 5, engine_geo_roads 2 (29 Funktionen). Mit dem neuen Level fallen sie weg.
- In SpawnCitys Quelle: 744 Aufrufe von math.h-Funktionen (length 231, normalize 128, translate 86, dot 62, trs 58, axis_angle 54,
  scale 53, cross 51, hex_color 16 …; $S/zahlen_spawncity.md).
- Heute rechnet ein solcher Graph in 64 Bit (Float), während die nativ gebaute Quelle die Engine-Funktion in f32 ruft. Mat4 ist im
  Blueprint eine Struktur aus 16 Floats (64 Bit), in der Engine aus f32. Der Vergleichstest übersetzt den Header-Rumpf ebenfalls
  „breit“ (TestWriter.widen) und vergleicht so Breit gegen Breit. Gegen die Engine vergleicht niemand.

## Plan
1. **engine_api.py --libraries schreibt Knoten statt Graph** für jede Funktion eines Engine-Headers, die die Sprache als Typen
   hat: einen Engine-Knoten wie die heutigen `engine.*`-Knoten (engine_nodes.gen.h/engine_calls.gen.cpp), Pins aus der Signatur.
   Die Bibliothek behält ihre Typen (Strukturen, Aufzählungen) und verliert ihre Funktionsgraphen. Ein Name, ein Weg: `Mat4_mul`
   wird der Knoten `engine.math.mat4_mul` (Benennung wie bei den übrigen Engine-Knoten).
2. **Übergang an einer Stelle**: Der erzeugte Knoten wandelt Pins in die Typen der Engine (Float → f32 gerundet, Struktur
   math_Mat4 → kern::Mat4 Feld für Feld gerundet) und zurück (genau). In der Klasse ist das derselbe Code (direkter Aufruf über
   denselben Wandler), im Interpreter der native Aufruf. Damit gilt: Quelle nativ == Interpreter == Klasse.
   Die Strukturen der Engine mit f32-Feldern (Mat4, Quat, Vec4) sollten im Blueprint die Genauigkeit der Engine haben: ein Feld
   „Float, gespeichert als f32“ wie `Num32` (sonst rundet jede Übergabe neu). Das ist eine kleine Spracherweiterung (Breite am
   Platz, analog U32), aber kein zweiter Zahl-Typ auf dem Draht. **Diese Frage ist vor dem Bau zu entscheiden.**
3. **Konverter**: Ein Aufruf einer Engine-Header-Funktion wird zum Knoten (wie ein Engine-Aufruf heute), nicht mehr
   `uses engine_math_math` mit Graph. Die Konverter-Fälle (meshextra, views …) bekommen neue Erwartungen.
4. **Tests/Stellvertreter**: Der Vergleichstest ruft im Quell-Teil die echte Engine-Funktion (kein `widen` für Engine-Header;
   converted.h `wide::` behält nur Num32 für Komponenten, oder entfällt mit 2.). Im Stellvertreter der Engine
   (KERN_TEST_ENGINE ≠ real) braucht jeder neue Knoten eine Implementierung, also die echte Funktion (sie ist rein, ohne
   Zustand der Engine). Engine-Test: je Bibliothek ein Fall „Knoten == Header-Funktion bitgleich“ (Interpreter, Klasse).
5. **Level/Editor**: Bibliotheksgraphen mit Funktionen verschwinden. Ein Level, das einen Funktionsgraphen einer Engine-Bibliothek
   ruft (`func_call` auf engine_math_math.Mat4_mul), ist alt: Fehler mit Ort und Grund (keine Rückwärtskompatibilität). Der nächste
   volle Konverter-Lauf schreibt das Level neu.
6. Reihenfolge: nach dem laufenden vollen Lauf; zuerst 2. entscheiden (f32-Felder der Engine-Strukturen), dann 1.+3.+4. in
   einem Zweig, Murmel als erster Beleg (nutzt heute keine math.h-Funktion: der Beleg ist ein Konverter-Fall mit Mat4/Quat).
