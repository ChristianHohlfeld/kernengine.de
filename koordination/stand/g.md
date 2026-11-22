# Paket g – Stand (Regeln des Konverters für die Game-Dateien), Zweig work-g1

## BEREIT ZUM MERGEN: work-g-stop 51364912 (Fix a5a157f2 + Merge work-zusammen a2e6dd1e) – Beleg work-g-stop-vergleich-100140-27958 (dazu 093350-22249 auf a5a157f2)
UAF ~Game (t1, converted_Game__game_cpp) behoben: Game::stop() setzt life_ vor world_ zurück; ~Game() { life_.reset(); } (statt = default). Kein Feld verschoben;
~Game ruft NICHT stop() (world_ endete sonst vor cars_/remote_car_/remote_bus_, deren Car beim Abbau detach() ruft). vergleich game, game_traffic, game_life, ragdoll, car:
convert/bau/alle rc=0, 50/50 ok beide Durchgänge, 0 Abweichungen, converted_Game__game_cpp ok ohne Abbruch.
VORBEHALTE (Entscheidung Koordinator/Chris):
1. Rückfall: Game::stop war im vollen Lauf 045421 „written“, wartet jetzt: „the end of a WorldLife: its field trucks is a list … of what ends with it“; ebenso das neue
   Game::~Game. Allgemeine Fähigkeit des Konverters (Ende einer Liste von Strukturen mit besessenem Car) – Paket g/k.
2. Differenztest fehlt: source_diff.py bildet die Paare (Game::stop, Game::~Game), der erzeugte Test übersetzt nicht (Game nicht kopierbar – TowTruck; Aufzählungen
   der Engine, const char*, Mat4) = Werkzeug „zwei Bauten“ (UEBERGABE 4.2). Nach Regel nur „vorbereitet“. Zudem lässt stop() jetzt life_ leer zurück (Objektvergleich
   direkt nach stop() unterscheidet sich; danach folgen im Programm nur start() – ersetzt life_ – oder der Abbau).
BEFUND (nicht geändert): dasselbe Muster bei cars_ (TrafficCar::car), remote_car_, remote_bus_ (nach stop() ist world_ weg, ihr ~Car → detach am Programmende, wenn
angehängt) und swarm_cruiser_ (game.h:341, vor world_ erklärt → nach ihm abgebaut, falls je angehängt).

## BEREIT ZUM MERGEN: c4d4544b – Beleg work-g1-vergleich-232903-26675
net_session.cpp auf c4d4544b, beide Durchgänge: Session_to_all/send_rules/send_scores/send_seats/send_proj/send_toss/send_world/
town_to_all/destroy_to_all/finish_ballot/set_origin/set_world_rules weichen nicht mehr ab (vorher ~100/300: max_clients einmal statt je Runde).
Übrig, nicht von g: Session_push 206/300 und ASan (real) = Länge eines Zeiger-Eingangs (BEFUND FÜR d); real „0 calls compared“ für
Session_connected/join/stop wie vorher (Engine weist ab). Keine neue Abweichung.

Basis: work-g1 = origin/work-zusammen 5a445af8 (+ eigene Commits). Arbeitsliste aus work-t2-vergleich-223555-30482
(ausgepackt: $S/mag/work-t2-vergleich-223555-30482/einzeln/einzeln{,-real}).

| Abweichung | Ursache | Commit | Beleg | Stand |
|---|---|---|---|---|
| Session_send_rules/_to_all/_send_* … (~100/300, net_session.cpp) | for-Grenze `c < server_->max_clients()` als Zählschleife: Blueprint fragt die Engine einmal, C++ jede Runde | c4d4544b | work-g1-vergleich-232903-26675: keine Abweichung mehr in beiden Durchgängen | belegt |
| Session_connected/status/roster/… 300/300 | im Beleg-Lauf (Testrahmen von work-zusammen, t1) nicht mehr da; Ursache war Test: die Quell-Kopie der Session besitzt client_ (unique_ptr, `owned`) und gibt ihn am Ende frei (client.release(4)), der Blueprint nicht | – | – | BEFUND FÜR t1 |
| ABBRUCH converted_net_session (real, ASan heap-buffer-overflow) + Session_push 206/300 (Stellvertreter: push_incoming(…, [35], 4) Quelle, [35,16,33,20] Blueprint – Blueprint richtig) | Stellvertreter HostTransport::push_incoming(from, const u8* data, int bytes): der Zeiger geht als Liste kürzerer Länge an die Engine, die `bytes` liest (Länge eines Zeiger-Eingangs) | – | – | BEFUND FÜR d |
| matrix_of 88/88 (game.cpp) | Stellvertreter antwortet für Mat4 (`f32 m[16]`) mit Form `{[n]}` = 0–2 Einträgen statt 16 (engine_api.gen.cpp, Api.shape); Quelle behält den Rest (Einheitsmatrix), Blueprint setzt ihn 0. Real: jeder Aufruf abgewiesen (kein game_world) → 0 verglichen | – | – | BEFUND FÜR d (Form fester Länge) / t1 (real) |
| eject 65/65, state_of 91/91 (game.h:170) | `read_body(world_->physics(), body(a))`: Reihenfolge der Argumente (E9) – GCC (Test) ruft actor_body zuerst, clang/Blueprint physics zuerst | – | – | E13: wartet auf t1s clang-Umstellung, dann vergleich |
| look_apply_tier (game_look.cpp:234) | Test: Stellvertreter von `kern::LightPool(n, 96)` als Temporär ruft kein light_pool.make (EngineObject(const A&…)-Konstruktor), der Blueprint schon | – | – | BEFUND FÜR t1 (zu prüfen am Testtext) |
| ABBRUCH Game__game_look_cpp (UBSan nullptr→string) | Verdacht: Test zieht Governor::tier_ außerhalb 0..kTiers-1 (Quelle hält es dort: perf.cpp:35/46/65/72) → kTier[t] außerhalb → q.name Müll/null | – | – | BEFUND FÜR t1 (Verdacht) |

## ENTSCHIEDEN (E13: Spiel-Suite mit clang, t1 stellt um): Reihenfolge der Argumente in den Game-Dateien (eject/state_of)
game.h:170 `BodyState state_of(ActorId a) const { return read_body(world_->physics(), body(a)); }` – zwei Engine-Aufrufe als Argumente.
Nach E9 gilt clang (= Blueprint). Weg nach E9: Absenkung (physics() vorher benennen) mit Differenztest unter clang – für Game::state_of
braucht der Differenztest ein Game-Objekt. Vorschlag: die Tests tests/converted ebenfalls mit clang bauen (allgemein, wie tests/lowered) –
dann gilt in jedem Vergleich die Bedeutung des Compilers des Spiels, ohne Absenkung je Stelle. Bis zur Antwort: nicht angefasst.

## In Arbeit (00:00): Pendeln des Ablaufs (Auftrag des Koordinators)
(2) Entwurf, gebaut, unbelegt: 082acc7d – erst wenn der Ablauf zum selben Zustand zurückkommt (heute: Abbruch), warten die
Funktionen der kreisenden Blueprints, die in den Runden des Kreises „was written and waits now“ waren, für immer (World.bars, mit
ihrem Grund; asked_of nennt es), dann weiter. Läufe, die heute konvergieren, erreichen die Stelle nie → kein Byte anders.
Grenze: kreisen nur die Stücke (exports) ohne solche Funktion – im Lauf 232247 gab es „waits now“ nur einmal (Runde 10), danach
kreisten 30 Einheiten weiter –, bleibt der Abbruch; das klärt (1).
(1) Ursache: Probe work-zusammen-probe-235554-4594 (game_events, combat_rules, game.cpp auf work-zusammen); Diagnose-Lauf des
Koordinators work-zusammen-konvert-235338-1934 („goes round … exports differ“).

## Pendeln – Stand 01:45
- In work-zusammen (2c7667bc): 082acc7d, 2f8b97fc, 480af66b, 7bd9ac5d. Im konvert 005330 griff das Festhalten (16 Funktionen, Runde 17) – Beleg: voller Lauf
  work-zusammen-voll-012215-4354 des Koordinators.
- **Wurzel des Kreises (Befund):** Game::explosion (game_town) steht in einer Rekursion ÜBER Teile hinweg: explosion → blast_cars (game_vehicle_damage)
  → … wreck_car (game_town) → explosion; explosion → combat_blast (game_military) → … → explosion (military.cpp:89/126/253/539/717), ebenso swarm,
  traffic, gaps, life … Teile rufen einander nur über den Eintrag der Vorrunde; wartet der Eintrag, wartet der Rufer. Eine Rekursion über Teile wartet
  darum nach Bauart für immer – und die Gründe wandern dabei verbatim (Unit.function: `raise Wait(f.wait if f.open …)`) mit einer Runde Verzug im Kreis:
  alte Gründe („look_blast() … is not written“ aus einer frühen Runde, als game_look noch nicht in need war) sterben nie und wechseln sich ab.
  Bis 203000 verdeckte das ein Testgrund (take(Span), von t2 behoben) – darum kreist es erst jetzt.
- **ENTSCHEIDUNG NÖTIG (nicht dringend, der Lauf endet jetzt):** sollen Funktionen in einer Rekursion über Teile geschrieben werden? Allgemeiner Weg:
  optimistisch – ein Aufruf eines Teils derselben Blueprint, dessen Eintrag nur „im Kreis mit dem Rufer“ wartet, gilt als geschrieben (Gestalt aus dem
  Eintrag: params/kinds/result), am Fixpunkt geprüft; wartet der Gerufene am Ende aus eigenem Grund, wartet der Rufer mit. Das ändert die Bedeutung des
  Ablaufs (heute: „What they would call of each other … waits for good, as a function that calls itself“) – darum erst nach deiner Entscheidung.
- combat_hud („Fun has no member multiplier – g_fun not a member of combat_rules_cpp_local“): stand schon in 203000 so; kleine Probe 235554 schreibt es.
  Verdacht: das Stück der Anweisung `rules::Blueprint g_military{…}, …, g_fun{…}, g_race{…};` (combat_rules.cpp:81) kommt je nach Runde von einem anderen
  Weiterreicher (take_const/merge, cec973a-Familie). Prüfe ich am Zwischenstand von 012215.

## BEREIT ZUM MERGEN: 96d52bef – Beleg lokal (lesen.py), der volle Lauf muss es zeigen
work-zusammen-voll-012215-4354 (2c7667bc): **das Festhalten trägt** – Fixpunkt nach Runde 18 (16 Funktionen „waits for good … (it went round with …)“),
dann convert rc=1 „the Blueprints were not read“: Game.cpp 8106/8128 `id = kern::math::wrap32s(int64_low(text_parse_int(…).value));` (game_toss.cpp:245/2xx
`std::atoi`): narrow() gibt Formeltext, parsed() setzte ihn ohne Formel. 96d52bef: cut_low (als kern::formula, wie value_whole64). Beleg: lesen.py an den
Texten von 012215 mit beiden Zeilen so gesetzt → „GELESEN: das Level nimmt alle Texte an“ (156 Blueprints). 012215 ist ungültig → 96d52bef einmischen,
voll neu ablegen. (SendMessage an main war nicht mehr möglich – Kontingent der Nachrichten erschöpft.)

## Rekursion (für den Entwurfspunkt Stufe 6)
(1) Endstand 012215: direkt („calls itself (a Blueprint's function does not)“): **0** Funktionen. Über Teile, festgehalten („went round“): **16** –
Game::explosion, wreck_car (town); combat_blast, combat_shot, step_heli (military); hit_scan (weapons); update_combat (events); update_details (details);
gaps_update (gaps); life_command (life); swarm_apply_hit (swarm); update_traffic_world (traffic); web_audio_command (audio_web); foot_input, ctl_grenade,
ctl_touch_shot (controls). Dazu 6 Folgefälle „it calls Game::explosion/wreck_car/hit_scan/combat_blast(), which waits“. Echte Rekursion ist davon nur der
Kern explosion → blast_cars/wreck_car → explosion und explosion → combat_blast/… → explosion; die übrigen rufen explosion und warten mit.
(2) Doku: blueprint_cpp.md/BLUEPRINTS.md sagen nichts zur Rekursion. Die SPRACHE verbietet sie: der Leser (kern/src/logic/notation.cpp:686ff.) weist
„function X calls itself (through Y): a function does not run again while it runs - repeat steps with a loop“ ab – direkt und über andere. Erlaubt ist
nur Wiedereintritt über ein Event (logic_cases.h `function_reentry`: jeder Aufruf mit eigenen Parametern, Events höchstens 64 tief). Der Konverter
folgt dem (Unit.function: „calls itself (a Blueprint's function does not)“; über Teile: der Ablauf wartet). Die Klasse (C++-Methoden) könnte es, der
Interpreter je Aufruf auch (Rahmen je Aufruf, s. function_reentry) – die Grenze ist eine Regel des Lesers, keine Fähigkeit, die fehlt.
(3) Vorschlag: Rekursion in der Sprache zulassen wie Wiedereintritt über Events – mit derselben Tiefengrenze (64) und einem Abbruch, der in jeder Stufe
beim selben Aufruf greift (Engine-Fall in logic_cases.h, Interpreter = JIT = Klasse); dann schreibt der Konverter eine Funktion, die sich (auch über Teile)
ruft, als Aufruf ihres Namens. Im Ablauf heißt das: ein Aufruf eines Teils derselben Blueprint, der nur im Kreis mit dem Rufer wartet, gilt mit der
Gestalt seines Eintrags als geschrieben, geprüft am Fixpunkt. Bis dahin warten die 16 ehrlich („went round“).

## Offen
- combat_hud: auch im Endstand 012215 „no member named 'multiplier' in …combat::Fun – g_fun not in …combat_rules_cpp_local“ (wie 203000; die kleine Probe
  235554 über game_events+combat_rules+game.cpp schreibt es). Verdacht: das Stück der Anweisung `rules::Blueprint g_military{…}, …, g_fun{…}, g_race{…};`
  (combat_rules.cpp:81) kommt im ganzen Programm von einem Weiterreicher ohne g_fun (take_const/merge, cec973a-Familie) – nicht geklärt.
- Die Familie „X was not declared … did you mean …net_session_cpp_local::kReliable/kUnreliable“ (Session::send_fx/town_to_all/town_to_host in Tests anderer
  Game-Teile) und „CombatArt has no member heli – kOlive not in combat_art_cpp_local“: dieselbe Art – ein Stück einer Datei, das ein Test über einen dritten
  Teil bekommt, ohne die Stücke des `<datei>_local`, auf die es zeigt. Nicht geklärt.
- eject/state_of: nach E13 (Suite mit clang) neu vergleichen. Befunde für t1/d siehe Tabelle oben.
