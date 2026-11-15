Kostenursachen für Commit 266887fadcff44ac5bf3b27e4e965e0fd93d9901 (101 cost:-Zeilen der Spiel-Suite)

Quelldateien ohne Pfad liegen in `kern/games/spawncity/src/`; Zeilenangaben bei „Klasse/Quelltext“ beziehen sich auf `kern/games/spawncity/src/level_blueprints.cpp`. Ein Eintrag mit mehreren Gruppen ist eine Zeile, die aus mehreren Gründen teurer ist, die erste Gruppe ist die wichtigste. In den Funktionen der Art-Gruppen (`*Art_*`) wird jedes `put` mitgezählt, das sie aufrufen. Sie erben dessen Gruppe D (siehe die `*_put`-Zeilen) und kommen über ihre Mat4-Zwischenwerte zu A.

Drei Mechanismen, die mehrfach vorkommen:

- **Doppelte Vorbelegung (E):** Der Konverter legt eine lokale Struktur an ihrer Deklaration mit dem Standardwert an. Am Knoten `T x;` belegt er sie noch einmal mit einem frisch gebauten Standardwert (`s = CarSpec{…}`, `r = Mat4{…}`), und jede Liste darin fragt dann noch einmal nach Speicher.
- **beside-Eintrag (F):** `logic_and` und `logic_or` sind Funktionen und werten jeden Operanden aus, `kern::formula` ebenso alle Argumente. C++ bricht bei `&&`, `||` und `?:` früher ab. Liest der Blueprint dabei hinter dem Ende einer Liste, gibt `List::operator[]` (nodes.h:149) einen Eintrag aus `beside::entry<T>()` zurück. Dieser hält einen `static std::deque<T>`, den der erste Zugriff für den Typ T im Prozess anlegt (Map + Block = 2 Anforderungen). Bei `const` greift stattdessen `static const Item zero{}`, das beim ersten Mal gebaut wird.
- **Enum-Namenstabelle (F):** `enum_from_index<E>` baut beim ersten Aufruf `kern_enum_values<E>()` (blueprint.h:303). Das ist ein statischer `std::vector<std::string>`, der Name für Name wächst (`logic::enums::names`, nodes.h:515).

| Funktion | Quelldatei:Zeile | Klasse/Quelltext | Ursache in einem Satz | Gruppe |
|---|---|---|---|---|
| CarSpec_bus | car.cpp:26 | car_cpp::CarSpec_bus, Z. 37268 | `wheels[4]` ist eine List. Dazu kommt `s` doppelt vorbelegt (Z. 37269 und 37270): wheels und upper_profile werden zweimal gebaut, C++ baut sie einmal. | A, E |
| CarSpec_car | car.h:50 | car::CarSpec_car, Z. 16625 | – | A |
| Car_new | car.h:70 | car::Car_new, Z. 16630 | Die Fixlängen-Felder (wheels, w_, applied_, rig_) sind Listen. Außerdem baut die Klasse `self` erst mit einem Standard-CarSpec samt upper_profile-Liste und überschreibt es dann mit `s` (Z. 16631/16632), C++ kopiert `s` direkt (`s_(s)`). | A, E |
| Car_spec | car.h:96 | car::Car_spec, Z. 16666 | `wheels[4]` wird als List mitkopiert. B ist hier nicht die gemessene Ursache, denn auch die Quelle kopiert: der Test-Lambda `[&]{ return a0.spec(); }` gibt einen Wert zurück. | A |
| CombatArt_beam | combat_art.cpp:76 | combat_art_cpp::CombatArt_beam, Z. 22565 | – (Mat4-Zwischenwerte als Listen, dazu `put`) | A, D |
| CombatArt_fighter | combat_art.cpp:161 | combat_art_cpp::CombatArt_fighter, Z. 22994 | – (Mat4-Zwischenwerte, die Schleifenliste `{-1, 1}` als `list_make` Z. 23060, dazu `put`) | A, D |
| CombatArt_flak | combat_art.cpp:134 | combat_art_cpp::CombatArt_flak, Z. 22841 | – (Mat4-Zwischenwerte, die vier Läufe `{Vec2…}` als `list_make` Z. 22962, dazu `put`) | A, D |
| CombatArt_gate | combat_art.cpp:177 | combat_art_cpp::CombatArt_gate, Z. 23097 | – (Mat4-Zwischenwerte, dazu `put`) | A, D |
| CombatArt_heli | combat_art.cpp:95 | combat_art_cpp::CombatArt_heli, Z. 22592 | – (Mat4-Zwischenwerte, die Schleifenliste `{-1, 1}` als `list_make` Z. 22697, dazu `put`) | A, D |
| CombatArt_put | combat_art.cpp:64 | combat_art_cpp::CombatArt_put, Z. 22509 | Die 16 Matrixwerte und die 4 Farbwerte gehen Wert für Wert per `list_insert` in transforms/tints (Z. 22524, 22531–22534), sodass die Listen mehrmals wachsen. C++ fügt sie mit je einem `insert(end, first, last)` ein. Zusätzlich kopiert `from = m.m` (Z. 22520) die Matrixliste. | D, E |
| CombatArt_put_2 | combat_art.cpp:70 | combat_art_cpp::CombatArt_put_2, Z. 22537 | Wie CombatArt_put: Einfügen Wert für Wert (Z. 22552, 22559–22562) statt `insert` in einem Zug, dazu die Kopie `from = m.m` (Z. 22548). | D, E |
| CombatArt_tank | combat_art.cpp:121 | combat_art_cpp::CombatArt_tank, Z. 22755 | – (Mat4-Zwischenwerte, dazu `put`) | A, D |
| Destruction_free_torn | destruction.cpp:210 | destruction_cpp::Destruction_free_torn, Z. 38023 | unklar: Der Ablauf ist gleich, das Mehr steckt in einem einzigen Aufruf im Engine-Aufruf `engine_renderer_remove` und lässt sich aus den Texten nicht ablesen. | unklar |
| Facades_fragment_declarations | facades.cpp:18 | facades_cpp::Facades_fragment_declarations, Z. 20978 | Fester Shader-Text als `const char*`; die Klasse gibt ihn als `Text` (std::string) zurück, länger als 15 Zeichen. | C |
| Facades_fragment_surface | facades.cpp:27 | facades_cpp::Facades_fragment_surface, Z. 20983 | Fester Shader-Text als `const char*`; die Klasse gibt ihn als `Text` zurück, länger als 15 Zeichen. | C |
| Facades_name | facades.h:18 | facades::Facades_name, Z. 9051 | `"spawncity-facades"` (17 Zeichen) als `Text`; zum Vergleich: `"spawncity-water"` (15 Zeichen) passt in den SSO-Puffer, WaterLook_name fehlt deshalb in der Liste. | C |
| Facades_vertex | facades.cpp:16 | facades_cpp::Facades_vertex, Z. 20973 | `"  vWall = aWall;\n"` (17 Zeichen) als `Text`. | C |
| Facades_vertex_declarations | facades.cpp:15 | facades_cpp::Facades_vertex_declarations, Z. 20968 | `"flat out vec4 vWall;\n"` als `Text`. | C |
| FootprintStream_begin_2 | kern/include/kern/geo/buildings.h:100 | engine_geo_buildings::FootprintStream_begin_2, Z. 10688 | Die 7 Werte gehen einzeln per `list_insert` in `data_` (Z. 10706–10712). C++ hängt sie mit einem `insert(end, {…})` in einem Zug an. Teurer ist der Aufruf nur, wenn data_ wachsen muss (215 von 300). | D |
| Interiors_clear | interiors.cpp:539 | interiors_cpp::Interiors_clear, Z. 38589 | unklar: Der Ablauf ist gleich, das Mehr (3) entsteht in einem einzigen Aufruf und lässt sich in den Engine-Aufrufen nicht ablesen. | unklar |
| Jet_new | jet.h:58 | jet_cpp::Jet_new, Z. 39389 | `gear[3]` (im Standard-JetSpec des vorbelegten `self`, Z. 39390) ist eine List. | A |
| Jet_spec | jet.h:80 | jet::Jet_spec, Z. 17546 | `gear[3]` wird als List mitkopiert. B ist nicht ursächlich, auch der Test kopiert die Quelle. | A |
| LifeArt_put | life_art.cpp:50 | life_art_cpp::LifeArt_put, Z. 29234 | Wie CombatArt_put: Einfügen Wert für Wert (Z. 29249, 29256–29259) statt `insert` in einem Zug, dazu die Kopie `from = m.m` (Z. 29245). | D, E |
| Life_provoke_cat | life.cpp:836 | life_cpp::Life_provoke_cat, Z. 45689 | Bei weniger als zwei Haustieren liest `logic_or` trotzdem `self.pets_[1].shown` (Z. 45690), C++ bricht bei `pets_.size() < 2` ab. Der erste beside-Eintrag für Life_Pet legt dann einmal die deque an (2). | F |
| Life_ragdolls_2 | life.h:146 | life::Life_ragdolls_2, Z. 19715 | In jeder Doll sind bodies, joints, pose, from und stand (C++ `[kBones]`) Listen: 5 Anforderungen je Puppe beim Kopieren (2 + 4×5 = 22). Teurer ist der Aufruf nur bei mindestens einer Puppe (240 von 300). B ist nicht ursächlich, der Test kopiert auch. | A |
| Life_set_remote_animals | life.cpp:1208 | life_cpp::Life_set_remote_animals, Z. 45733 | unklar: Der Ablauf ist gleich, das Mehr (4 in 2 Aufrufen) steckt in den Engine-Aufrufen. | unklar |
| Mat4_mul | kern/include/kern/math/math.h:111 | engine_math_math::Mat4_mul, Z. 11358 | – | A |
| Mat4_scale | kern/include/kern/math/math.h:134 | engine_math_math::Mat4_scale, Z. 11403 | – | A |
| Mat4_translate | kern/include/kern/math/math.h:133 | engine_math_math::Mat4_translate, Z. 11393 | – | A |
| Mat4_trs | kern/include/kern/math/math.h:137 | engine_math_math::Mat4_trs, Z. 11414 | – | A |
| MeshData_push | kern/include/kern/geo/mesh_data.h:23 | engine_geo_mesh_data::MeshData_push, Z. 10920 | Position, Normale, Farbe und UV gehen Wert für Wert per `list_insert` in ihre Listen (Z. 10940–10961). C++ hängt je Liste mit einem `insert(end, {…})` an. Teurer ist der Aufruf, wenn eine Liste wachsen muss (276 von 300). | D |
| MpArt_drone_accent | mp_art.cpp:91 | mp_art_cpp::MpArt_drone_accent, Z. 40999 | – (Mat4-Zwischenwerte, dazu `put`) | A, D |
| MpArt_flash | mp_art.cpp:86 | mp_art_cpp::MpArt_flash, Z. 40991 | – (Mat4-Zwischenwert, dazu `put`) | A, D |
| MpArt_ping | mp_art.cpp:66 | mp_art_cpp::MpArt_ping, Z. 40952 | – (Mat4-Zwischenwerte, dazu `put`) | A, D |
| MpArt_put | mp_art.cpp:61 | mp_art_cpp::MpArt_put, Z. 40924 | Wie CombatArt_put: Einfügen Wert für Wert (ab Z. 40938) statt `insert` in einem Zug, dazu die Kopie `from = m.m` (Z. 40935). | D, E |
| MpArt_shield | mp_art.cpp:96 | mp_art_cpp::MpArt_shield, Z. 41012 | – (Mat4-Zwischenwert, dazu `put`) | A, D |
| MpArt_tracer | mp_art.cpp:75 | mp_art_cpp::MpArt_tracer, Z. 40970 | – (Mat4-Zwischenwert, dazu `put`) | A, D |
| NavTimeline_push | mp_rules.cpp:131 | mp_rules_cpp::NavTimeline_push, Z. 41340 | Bei leerer `snaps_` liest `logic_and` trotzdem `self.snaps_[len-1]` (Z. 41341/41344), C++ prüft vorher `!snaps_.empty()`. Der erste beside-Eintrag legt einmal die deque an (+2). | F |
| PeopleArt_beacon | people_art.cpp:349 | people_art_cpp::PeopleArt_beacon, Z. 29988 | – (Mat4-Zwischenwerte, dazu `put`) | A, D |
| PeopleArt_fire | people_art.cpp:365 | people_art_cpp::PeopleArt_fire, Z. 30023 | – (Mat4-Zwischenwerte, dazu `put`) | A, D |
| PeopleArt_light_bar | people_art.cpp:354 | people_art_cpp::PeopleArt_light_bar, Z. 29998 | – (Mat4-Zwischenwerte, dazu `put`) | A, D |
| PeopleArt_police_drone | people_art.cpp:374 | people_art_cpp::PeopleArt_police_drone, Z. 30039 | – (Mat4-Zwischenwerte, dazu `put`) | A, D |
| PeopleArt_put | people_art.cpp:226 | people_art_cpp::PeopleArt_put, Z. 29894 | Wie CombatArt_put: Einfügen Wert für Wert (ab Z. 29908) statt `insert` in einem Zug, dazu die Kopie `from = m.m` (Z. 29905). | D, E |
| PeopleArt_street_lamp | people_art.cpp:212 | people_art_cpp::PeopleArt_street_lamp, Z. 29924 | – (Mat4-Zwischenwerte, dazu `put`) | A, D |
| People_route_point | people.cpp:35 | people_cpp::People_route_point, Z. 35079 | `L = lanes[p.lane]` (Z. 35092) kopiert die Spur samt ihrer `points`-Liste, C++ nimmt `const Lane& L`. Das kostet nur, wenn der Index gültig ist und die Spur Punkte hat (26 von 300). | E |
| Ragdolls_bone_body | ragdoll.h:44 | ragdoll_cpp::Ragdolls_bone_body, Z. 43422 | Ist der Schlüssel nicht da (i = -1), liest `logic_and` trotzdem `self.dolls_[i]` (Z. 43425), C++ prüft vorher `i >= 0`. Weil `self` const ist, wird beim ersten Mal das statische Null-Doll gebaut, mit seinen 5 Listen (bodies, joints, pose, from, stand), also 5. | F, A |
| Ragdolls_release | ragdoll.cpp:69 | ragdoll_cpp::Ragdolls_release, Z. 43458 | unklar: Der Ablauf ist gleich, das Mehr (6) entsteht in einem von 108 Aufrufen in den Engine-Aufrufen. | unklar |
| RoadGraph_node_at | road_graph.cpp:25 | road_graph_cpp::RoadGraph_node_at, Z. 35848 | Ist eine Nachbarzelle nicht da (`it == Länge`), liest `logic_and` trotzdem `self.nodes_[self.cells_[it].second]` (Z. 35862), C++ prüft vorher `it != end()`. Der erste beside-Eintrag je Typ legt einmal eine deque an (+2, in 2 Aufrufen). | F |
| RoadGraph_rebuild | road_graph.cpp:38 | road_graph_cpp::RoadGraph_rebuild, Z. 35892 | unklar: Das Mehr (bis 3, in 8 von 123 Aufrufen) lässt sich keiner Stelle sicher zuordnen. node_at (siehe oben) erklärt nur je 2 beim ersten Mal. | unklar |
| SoundFx_drive | sound_fx.cpp:130 | sound_fx_cpp::SoundFx_drive, Z. 31310 | unklar: Der Ablauf ist gleich, das Mehr (1, in 3 Aufrufen) steckt in den Mixer-Aufrufen der Engine. | unklar |
| SoundFx_update | sound_fx.cpp:156 | sound_fx_cpp::SoundFx_update, Z. 31355 | unklar: Die Funktion ruft nur siebenmal SoundFx_drive, das Mehr (3, in einem Aufruf) lässt sich nicht ablesen. | unklar |
| VehicleArt_car | vehicle_art.cpp:173 | vehicle_art_cpp::VehicleArt_car, Z. 32662 | `c.spec()` gibt in C++ `const CarSpec&` zurück; `car::Car_spec(c).Result` kopiert dagegen jedes Mal den ganzen CarSpec samt wheels- und upper_profile-Liste (Z. 32678, 32693, 32694, in der Schleife Z. 32697). Dazu kommen die Mat4-Zwischenwerte, die vorbelegte Liste `CarSpec s` und `put`. | B, A, D |
| VehicleArt_jet | vehicle_art.cpp:200 | vehicle_art_cpp::VehicleArt_jet, Z. 32712 | – (Mat4-Zwischenwerte, `JetSpec s` mit gear-Liste (C++ `gear[3]`, Z. 32717/32736), dazu `put`) | A, D |
| VehicleArt_parachute | vehicle_art.cpp:227 | vehicle_art_cpp::VehicleArt_parachute, Z. 32754 | – (Mat4-Zwischenwerte, die vier Ecken `{Vec2…}` als `list_make` Z. 32790, dazu `put`) | A, D |
| VehicleArt_put | vehicle_art.cpp:168 | vehicle_art_cpp::VehicleArt_put, Z. 32632 | Wie CombatArt_put: Einfügen Wert für Wert (ab Z. 32646) statt `insert` in einem Zug, dazu die Kopie `from = m.m` (Z. 32643). | D, E |
| WaterLook_fragment_declarations | water_look.cpp:17 | water_look_cpp::WaterLook_fragment_declarations, Z. 21771 | Fester Shader-Text als `const char*`; die Klasse gibt ihn als `Text` zurück. | C |
| WaterLook_fragment_surface | water_look.cpp:27 | water_look_cpp::WaterLook_fragment_surface, Z. 21776 | Fester Shader-Text als `const char*`; die Klasse gibt ihn als `Text` zurück. | C |
| WaterLook_vertex | water_look.cpp:15 | water_look_cpp::WaterLook_vertex, Z. 21766 | `"  vWater = aWater;\n"` (19 Zeichen) als `Text`. | C |
| WaterLook_vertex_declarations | water_look.cpp:14 | water_look_cpp::WaterLook_vertex_declarations, Z. 21761 | `"out vec3 vWater;\n"` (17 Zeichen) als `Text`. | C |
| WorldArt_put | world_art.cpp:48 | world_art_cpp::WorldArt_put, Z. 33199 | Wie CombatArt_put: Einfügen Wert für Wert (Z. 33217, 33225–33228) statt `insert` in einem Zug, dazu die Kopie `from = m.m` (Z. 33213). Das kostet nur, wenn `id` gültig ist; sonst kehrt die Funktion vorher zurück (12 von 300). | D, E, F |
| addm | nostr.cpp:96 | nostr_cpp::addm, Z. 29478 | – (`U256.w[8]` als List) | A |
| at (1 von 4) | combat_art.cpp:14 | combat_art_cpp::at, Z. 22493 | – | A |
| at (2 von 4) | life_art.cpp:15 | life_art_cpp::at, Z. 29220 | – (die vier at-Zeilen sind gleich lautend und lassen sich keiner Datei zuordnen) | A |
| at (3 von 4) | people_art.cpp:17 | people_art_cpp::at, Z. 29885 | – | A |
| at (4 von 4) | vehicle_art.cpp:15 | vehicle_art_cpp::at, Z. 32628 | – | A |
| billboard | game_gaps.cpp:90 | game_gaps_cpp::billboard, Z. 27069 | – | A |
| box | game_look.cpp:30 | game_look_cpp::box, Z. 27997 | unklar: Die Mat4-Listen (A) sind beteiligt, doch das Mehr (9 bei 171) verteilt sich auf die Umwandlungen in `engine_primitives_box/append` und lässt sich nicht sicher aufteilen (242 von 247). | unklar |
| box_entry | person_hit.h:25 | person_hit::box_entry, Z. 12755 | – (`po[3]`, `pd[3]`, `h` als `list_make`, Z. 12767–12769) | A |
| building_options | city_stream.cpp:61 | city_stream_cpp::building_options, Z. 22224 | `bo` doppelt vorbelegt (Z. 22225/22226, je 2 Listen im BuildingStyle), dazu die doppelte Vorbelegung in konstanz_style (unten). | E |
| camera_frame | game_look.cpp:156 | game_look_cpp::camera_frame, Z. 28081 | – | A |
| clang_pcm | game_lamps.cpp:50 | game_lamps_cpp::clang_pcm, Z. 27757 | – (die Schleifenliste `{180, 412, 733}` als `list_make`, Z. 27769) | A |
| clean | interiors.cpp:32 | interiors_cpp::clean, Z. 38420 | Bei leerem `r` liest `logic_and` trotzdem `r[len-1]` und `r[0]` (Z. 38432/38440), C++ prüft vorher `!r.empty()` bzw. `size() > 2`. Der erste beside-Eintrag für Vector legt einmal die deque an (2). | F |
| clock_ms | game_look.cpp:25 | game_look_cpp::clock_ms, Z. 27985 | unklar: Die Funktion ist nur ein Engine-Aufruf `engine_now_ms`; das Mehr (1, in einem Aufruf) lässt sich nicht ablesen. | unklar |
| config | net_session.cpp:24 | net_session_cpp::config, Z. 42828 | `c` doppelt vorbelegt (Z. 42829/42832, je eine channels-Liste). Danach ersetzt `list_make` (Z. 42840) die channels durch eine neue Liste, C++ weist `{reliable, unreliable}` in die schon vorhandene Kapazität zu. | E |
| cyl | game_look.cpp:32 | game_look_cpp::cyl, Z. 28007 | unklar: wie box, das Mehr (9 bei 1147) lässt sich in den Engine-Umwandlungen nicht sicher zuordnen (230 von 233). | unklar |
| drive_loop | game_equipment.cpp:150 | game_equipment_cpp::drive_loop, Z. 26759 | unklar: Der Ablauf ist gleich, das Mehr (2, in 2 Aufrufen) steckt in den Mixer-Aufrufen der Engine. | unklar |
| euler | game_look.cpp:168 | game_look_cpp::euler, Z. 28110 | – | A |
| glow_box | game_look.cpp:35 | game_look_cpp::glow_box, Z. 28016 | unklar: Die Mat4-Liste aus Mat4_translate ist beteiligt, doch das Mehr (6 bei 1331, nur 223 von 300) lässt sich in den Engine-Umwandlungen nicht sicher zuordnen. | unklar |
| glow_cyl | game_look.cpp:36 | game_look_cpp::glow_cyl, Z. 28023 | unklar: wie glow_box (6 bei 587, 217 von 300). | unklar |
| hint_weapon | game_controls.cpp:41 | game_controls_cpp::hint_weapon, Z. 26537 | Fester Text als `const char*`; als `Text` passt nur `"GRENADE LAUNCHER"` (16 Zeichen) nicht in den SSO-Puffer. Teurer ist der Aufruf also nur außerhalb des Drohnenmodus mit der Waffe grenade_launcher (14 von 300). | C, F |
| hit (1/300, 4 statt 2) | combat_audio.cpp:112 | combat_audio_cpp::hit, Z. 24429 | Bei `i < delay` liest `kern::formula` trotzdem `data[i - delay]` (Z. 24462), C++ liest es nur im Zweig `i >= delay ? … : 0`. Der erste beside-Eintrag für Float legt einmal die deque an (+2). | F |
| hit (300/300, 6 statt 0) | person_hit.h:57 | person_hit::hit, Z. 12832 | – (zwei box_entry-Aufrufe mit je drei Fixlängen-Arrays als `list_make`) | A |
| jet_roar_pcm | game_gaps.cpp:647 | game_gaps_cpp::jet_roar_pcm, Z. 27164 | unklar: Der Ablauf ist gleich, das Mehr (1, in 27 von 39 Aufrufen) lässt sich keiner Stelle sicher zuordnen (am ehesten `engine_audio_normalize`, Z. 27208). | unklar |
| kind_of | town_sync.cpp:37 | town_sync_cpp::kind_of, Z. 36509 | Der erste Aufruf mit gültiger Art baut über `enum_from_index<town_Kind>` (Z. 36513) einmal die Namenstabelle: 8 Namen, also 4 Wachstumsschritte. | F |
| konstanz_style | city_stream.cpp:17 | city_stream_cpp::konstanz_style, Z. 22166 | `s` doppelt vorbelegt (Z. 22167/22168): walls und roofs werden einmal mehr gebaut als in C++ (`kern::BuildingStyle s;`). | E |
| mulm | nostr.cpp:107 | nostr_cpp::mulm, Z. 29502 | – (`U256.w[8]` als List, in jedem addm der 256 Schritte) | A |
| powm | nostr.cpp:115 | nostr_cpp::powm, Z. 29520 | – (`U256.w[8]` als List, in jedem mulm/addm) | A |
| prog_of | equipment.cpp:157 | equipment_cpp::prog_of, Z. 26263 | Der erste Aufruf außerhalb des Menüs baut über `enum_from_index<Genre>` (Z. 26270) einmal die Namenstabelle: 5 Namen, also 4 Wachstumsschritte. | F |
| read_body | common.h:72 | common::read_body, Z. 14767 | unklar: Die Funktion besteht nur aus drei Engine-Aufrufen; das Mehr (3, in einem Aufruf) lässt sich nicht ablesen. | unklar |
| reduce | nostr.cpp:124 | nostr_cpp::reduce, Z. 29539 | – | A |
| rot (1 von 4) | combat_art.cpp:13 | combat_art_cpp::rot, Z. 22488 | – | A |
| rot (2 von 4) | life_art.cpp:14 | life_art_cpp::rot, Z. 29215 | – (die vier rot-Zeilen sind gleich lautend) | A |
| rot (3 von 4) | people_art.cpp:16 | people_art_cpp::rot, Z. 29880 | – | A |
| rot (4 von 4) | vehicle_art.cpp:14 | vehicle_art_cpp::rot, Z. 32623 | – | A |
| sc3 (1 von 2) | combat_art.cpp:15 | combat_art_cpp::sc3, Z. 22498 | – (die beiden sc3-Zeilen sind gleich lautend) | A |
| sc3 (2 von 2) | life_art.cpp:16 | life_art_cpp::sc3, Z. 29225 | – | A |
| shot | combat_audio.cpp:37 | combat_audio_cpp::shot, Z. 24342 | – (`taps[3][2]` als `list_make` von 3 Zeilen mit je einer 2er-Liste, Z. 24402: 4 Anforderungen) | A |
| smoke_disc | game_gaps.cpp:103 | game_gaps_cpp::smoke_disc, Z. 27097 | Die Ecken gehen über MeshData_push Wert für Wert hinein, die Indizes einzeln per `list_insert` (Z. 27142–27157). C++ hängt mit `insert(end, {…})` in einem Zug an. | D |
| spec | weapons.h:41 | weapons::spec, Z. 14053 | `list_get` kopiert den Spec samt `name` als `Text`; C++ hat dort `const char*`. Über den SSO-Puffer geht nur `"Granatwerfer 40 mm"` (18 Zeichen), also kostet es nur beim grenade_launcher (16 von 300). | C, F |
| subm | nostr.cpp:102 | nostr_cpp::subm, Z. 29491 | – | A |
| yaw_at | game_lamps.cpp:26 | game_lamps_cpp::yaw_at, Z. 27705 | – | A |

## Gruppen

Gezählt wird jede Zeile in jeder Gruppe, die sie nennt; Zeilen mit zwei Gruppen zählen also doppelt.

| Gruppe | Zeilen | davon allein |
|---|---|---|
| A festes Array als List | 54 | 32 |
| B Referenz in C++, Kopie im Blueprint | 1 | 0 |
| C Text aus festem Wortlaut | 11 | 9 |
| D Liste wächst in Schritten | 29 | 3 |
| E Kopie oder doppelte Vorbelegung, die C++ nicht macht | 13 | 4 |
| F nur in wenigen Aufrufen (Kurzschluss/beside, Enum-Tabelle, seltene Fälle) | 11 | 7 |
| unklar | 15 | 15 |

Kombinationen: A, D 18 · D, E 6 · A, E 2 · C, F 2 · B, A, D 1 · F, A 1 · D, E, F 1.

Zeilen mit „unklar“: 15.
