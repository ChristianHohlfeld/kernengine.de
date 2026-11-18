# Übergabe Paket d – „Platz der Engine“ (Folgeauftrag nach Stufe 4 (d))

Zweig work-d, Worktree /home/claude/w-d. Stufe 4 (d) ist fertig, belegt (CI 44337346531) und in work gemergt (90b5ab8).
Dieser Folgeauftrag läuft auf demselben Zweig (origin/work ist eingemischt).

## Auftrag (Koordinator, 18.11. ~17:00, bestätigt)
Eine Funktion der Engine, die eine NICHT-const Referenz auf etwas gibt, das die Engine hält
(`std::vector<ShotEvent>& GameWorld::shots()`, `events()`, `sound_cues()` – kern/include/kern/game/runtime.h:277-281),
ist ein Platz der Engine, keine Kopie. Betroffen im Spiel: update_fighters_back, step_heli, step_soldier, step_cop,
update_police_drones, handle_shots, drone_attack, jet_attack, ctl_touch_shot (Konverter wartet dort: „an assignment to
CXXMemberCallExpr“, „a reference to something it changes“).

## Entwurf (bestätigt)
- **Der Platz ist der Ausgang des Engine-Knotens.** engine.game_world.shots ist ein Schritt, sein Ausgang `result` ist eine
  Referenz, gebunden, wenn der Knoten läuft (= `auto& r = world->shots();`, einmal gebunden).
- **Die bestehenden Platz-Knoten** (Get, Set, Add to Variable, Take, Listen-Schritte Add/Insert/Remove/Clear/Set Length/
  Append …) nennen als Wurzel statt einer Variablen diesen Ausgang: Einstellung `variable` leer, neue Einstellung
  `bound` = Id des Engine-Knotens. Text: `engine_game_world_shots(n12, World); list_add(n12.result, e);
  n12.result[i].to = v;` gelesen `n12.result[i].to`.
- **Nur Nachschlagen ohne Wirkung ist ein Platz:** der Generator prüft am Baum: Rumpf genau
  CompoundStmt{ReturnStmt{MemberExpr(CXXThisExpr)}} (ggf. durch ImplicitCast/Paren), Rückgabe T& ohne const, T kein Objekt
  der Engine (kein Handle). Jede andere T&-Funktion bleibt wie heute (Kopie); der Konverter wartet beim Schreiben mit Grund.
  (AST von shots geprüft: genau diese Form.)
- **Klasse:** n12.result ist ein generierter Stellvertreter-Platz `ev::Place<Tag, C>` mit Zeiger auf das C der Engine
  (std::vector<kern::game::ShotEvent>), ein Nachschlagen beim Binden, danach direkt; Zahlen werden erst beim Zugriff
  gewandelt (Direct<Tag>, f32↔f64). Struktur-Einträge: generierte Proxy-Strukturen mit Feld-Plätzen (Lesen per
  Konversion, =, +=). Index neben der Liste: eigener Eintrag mit Defaults (wie kern::List). Ungebunden / Objekt
  freigegeben: Lesen = Null, Schreiben ins Leere, Host::engine_failed wird gesagt.
  **Tod des Objekts:** EngineObjects bekommt einen Zähler der Freigaben (Generation); der Platz merkt sich beim Binden
  die Generation und prüft sie je Zugriff (ein Vergleich) – kein toter Zeiger. Engine-Test dafür.
- **Interpreter:** der Ausgang hält die Eingänge des Aufrufs (Handle) als Wert; jede Operation am Platz geht durch die
  Tür: eine allgemeine Platz-Operation je Platz-Funktion aus derselben Tabelle (engine_calls.gen.cpp): Referenz holen,
  Weg ablaufen, lesen/ändern (Conv<Tag>). JIT: Graphen mit Platz-Wegen laufen heute schon im Interpreter (codegen.cpp
  kennt keine Wege) – prüfen, dass jit_fits_ für `bound` ebenfalls nein sagt.
- **Lebensdauer (Koordinator):** gebunden nur innerhalb des Laufs der Funktion/Kette, in der der Knoten lief. Über
  Delay/Handler/nächsten Tick hinweg ungebunden; check() meldet es als Fehler, wo es am Graphen feststeht.
- **check():** Typ entlang des Wegs wie bei Variablen; „gehaltener Eintrag bleibt“ gilt auch für Plätze der Engine.
- **Editor:** Palette bietet die Platz-Knoten auf einem Platz der Engine an (kern/src/studio/logic_edit.cpp); der Fall
  mit jedem Knoten (kern/tests/test_main.cpp) deckt `bound` ab.
- **Tests:** Engine-Test (Klasse direkt vs. Tür vs. C++ auf GameWorld::shots, Speicher gezählt, Freigabe während
  gebunden); nativer Fall (engine_case.gen.h: je Platz-Funktion lesen/setzen/anhängen); Doku blueprint_cpp.md „Plätze
  der Engine“.
- **Stellvertreter im Test (Konverter-Test, SC_ENGINE=stand-in und real):** der Stellvertreter muss die Liste hinter
  der Platz-Funktion als Zustand halten (aus der Tabelle erzeugt) – selbst bauen, t1 sagen, was er wissen muss.
- **Konverter (klein, lokal; g1/g2/f1/f2/t1/t2 arbeiten in derselben Datei):** `world_->shots().push_back(x)`,
  `auto& r = world_->shots(); r[i].f = v;`, `world_->shots()[i]` → Schritt + Platz-Knoten auf `bound`.
- Melden an main, wenn der Engine-Teil bewiesen ist, BEVOR der Konverter-Teil gepusht wird.

## Wo der Code steht (gelesen)
- Platz im Graphen: Graph::root_of / path_of / place_of (kern/src/logic/logic.cpp ~865-1000), Instance::Spot
  (logic.h ~1067; Wurzel: key | entry/output | beside), Instance::spot/read/write (logic.cpp ~2742-2870),
  value_at/change_at (~2692). Context::spot (~3386). Einstellungen path/scope: ParamDef::Kind (logic.h ~736),
  core_nodes.cpp path_of()/place_title.
- Generator: tools/engine_api.py (main, Calls.body = Tür, Calls.direct = Klasse, case_text = Fall);
  Api.function1 in tools/cpp_to_blueprint.py ~9042 (Rückgabe: `ret = qtype(d).split("(")[0]`, handed(ret)).

## Stand
- [ ] 1 Generator: place-Erkennung (Nachschlagen am Baum), Feld in der Tabelle
- [ ] 2 Tür-Platz-Operation (engine_host.h, engine_calls.gen.cpp)
- [ ] 3 Interpreter: Spot-Wurzel `bound`
- [ ] 4 Klasse: ev::Place + Proxys, Generation in EngineObjects
- [ ] 5 Schreibweise (notation.cpp lesen/schreiben), check(), Palette
- [ ] 6 Tests, nativer Fall, Doku
- [ ] 7 Stellvertreter im Konverter-Test, Konverter
