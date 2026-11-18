# Übergabe Paket d – Stand 18.11.2026 (Sitzungswechsel)

**Zweig:** `work-d` (Repo ChristianHohlfeld/kernengine.de), **letzter Commit:** `edae85f`
(„Zwischenstand: Plätze der Engine – der Generator erkennt sie, die Tür kann sie lesen und setzen (Schritt 1+2 von 7)“).
work-d enthält origin/work bis `90b5ab8` (Merge von work-d in work). Commit-Ende immer:
`Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>` + `Claude-Session: https://claude.ai/code/session_01K2YidUPFTJShcS1fG6cZzJ`.

---

## A. Stufe 4 (d) „Volle Geschwindigkeit im Export“ – FERTIG, BEWIESEN, in work GEMERGT (90b5ab8)

In der exportierten Klasse ruft ein Engine-Knoten die Funktion der Engine direkt und typisiert
(kern/include/kern/logic/engine_direct.h, engine_nodes.gen.h), die Tür `Host::engine` bleibt für Interpreter/JIT/Tests;
tools/engine_api.py schreibt beides aus einer Tabelle. (Vorgänger-Commits 150724d, cc6426d, eddaa95, c65428b.)
Von mir dazu: Commit `32c0c18` – der native Fall „Engine“ vergleicht jetzt auch die **freien Funktionen** (Free1–3) auf
der echten Engine: blueprint_native_test spielt sie in einem Prozess dreimal (Tür, Klasse direkt, Tür) –
kern/tests/native_show.h `native_compare_on_engine`, kern/tests/blueprint_native.h `Case::on_engine_free`. Was die
Tür zweimal gleich gibt, muss die Klasse Zeichen für Zeichen gleich geben; Nicht-Wiederholbares wird benannt.

**Beweis:** CI-Lauf 44337346531 auf 30f63e4: Job native ✓ (engine_calls_class_calls_the_engine_itself ✓,
engine_calls_class_asks_for_no_memory ✓, blueprint_native_test ✓ „51 Blueprints as their C++, 13872 calls“,
game_native_test ✓ „705 lines“), build ✓ (Export nach wasm mit libc++), determinism ✓. Nicht wiederholbar (also nicht
verglichen) genau 6 Werte: R118 engine_last_error, R120 now_ms, R121 wall_ms, R122_0 jobs_submit-Nummer, R126
jobs_pending, R129 jobs_take_queued. Lokal zusätzlich: die ganze Engine-Klasse samt Glue (über
`tools/blueprint_cpp.sh --apply` aus kEngine erzeugt, $S/d/eng_bp.cpp) übersetzt -Werror sauber mit wasi-sdk
clang++ (libc++, wasm32), g++ 13 und clang++ 18 (libstdc++). Generator-Neulauf ergab bitgleich die eingecheckten
.gen-Dateien.

---

## B. Folgeauftrag „Platz der Engine“ – IN ARBEIT (Schritt 1+2 von 7 fertig, unbewiesen)

### Auftrag des Koordinators (Wortlaut)
„d – Folgeauftrag (allgemeine Fähigkeit, du bist der Besitzer von tools/engine_api.py und der Engine-Knoten): Eine
Funktion der Engine, die eine NICHT-const Referenz auf etwas gibt, das die Engine hält (z. B.
`std::vector<ShotEvent>& GameWorld::shots()`, ebenso events(), sound_cues()), ist ein **Platz der Engine**, keine
Kopie. Heute gibt der Knoten engine.game_world.shots eine Kopie; das Spiel schreibt aber in diese Listen
(`world_->shots().push_back(e)`; `auto& shots = world_->shots(); shots[i].to = …`) – der Konverter wartet dort („an
assignment to CXXMemberCallExpr“, „a reference to something it changes“; betroffen: update_fighters_back, step_heli,
step_soldier, step_cop, update_police_drones, handle_shots, drone_attack, jet_attack, ctl_touch_shot). Baue das
allgemein: engine_api.py erkennt jede solche Funktion (Rückgabe T& ohne const auf Liste/Struktur/Zahl) und erzeugt
dafür, was die Sprache für einen Platz braucht – Lesen, Setzen am Platz (auch eines Felds eines Eintrags), bei Listen
Länge/Anhängen/die vorhandenen Listenschritte auf diesem Platz –, in jeder Stufe bitgleich (Interpreter und JIT durch
die Tür, Klasse direkt und typisiert auf der Referenz, ohne Kopie: gleiche Kosten wie das C++), mit Engine-Test,
nativem Fall, Doku in kern/docs/blueprint_cpp.md; und der Konverter schreibt `engine_fn().push_back(x)`, `auto& r =
engine_fn(); r[i].f = v;`, `engine_fn()[i]` damit (halte die Konverter-Änderung klein und lokal;
g1/g2/f1/f2/t1/t2 arbeiten in derselben Datei). Wähle die sauberste Form (z. B. der Knoten der Funktion gibt einen
Platz, auf dem die bestehenden Platz-/Listen-Knoten arbeiten, statt je Funktion fünf neue Knoten) und melde mir den
Entwurf, bevor du lange baust.“

### Auflagen zum Entwurf (Wortlaut Koordinator)
„Entwurf „Platz der Engine“: ja, so bauen. Drei Auflagen: (a) Punkt 5 gilt nur für Funktionen, die ein Nachschlagen
ohne Wirkung sind – das muss an der Funktion feststehen (z. B. ihr Rumpf ist `return feld;`/inline-Getter, vom
Generator am Baum geprüft), nicht angenommen; für jede andere T&-Funktion wird die Referenz einmal gebunden oder die
Stelle wartet mit Grund. In der Klasse wird für `auto& r = f();` die Referenz einmal gebunden (ein Aufruf, wie im C++)
– gleiche Kosten, nicht ein Aufruf je Nennung. (b) Die Regel „gehaltener Eintrag bleibt“ und die Wächter des Tests
(Eintrag außerhalb der Liste: C++ lässt offen → Aufruf markiert) gelten für Plätze der Engine wie für Variablen; im
Vergleich gegen die echte Engine (SC_ENGINE=real) und gegen den Stellvertreter muss der Stellvertreter die Liste
hinter der Funktion als Zustand halten, sonst vergleicht der Test nichts – kläre das mit dem Besitzer des
Test-Schreibers nicht ab, sondern baue den Stellvertreter-Teil selbst (er kommt aus deiner Tabelle) und sag mir, was
t1 wissen muss. (c) Editor: die Palette bietet die Platz-Knoten auf einem Platz der Engine an wie auf einer Variablen
(kern/src/studio/logic_edit.cpp), der Fall mit jedem Knoten (test_main.cpp) deckt die neue Einstellung ab. Arbeite
auf work-d weiter (vorher origin/work einmischen), Proben/Suiten auf mag; melde dich, wenn der Engine-Teil bewiesen
ist, bevor du den Konverter-Teil pushst.“

„d – bestätigt: der Platz ist der Ausgang des Engine-Knotens („bound“), so bauen. Eine Ergänzung zur Lebensdauer: ein
gebundener Platz gilt, solange im C++ die Referenz gälte – innerhalb des Laufs der Funktion, in dem der Knoten lief.
Über ein Warten/einen späteren Event hinweg (Delay, Handler, nächster Tick) ist er nicht mehr gebunden: check() macht
das zum Fehler des Graphen, wo es am Graphen feststeht, sonst gilt „ungebunden“ wie von dir beschrieben (in jeder
Stufe gleich, Host wird es gesagt). Und: endet das Objekt hinter dem Handle (Release), während ein Platz gebunden ist,
darf die Klasse nicht auf einen toten Zeiger schreiben – dieselbe Antwort wie „ungebunden“, mit Engine-Test.“

Arbeitsweise (Nachtrag 17:10 in PAKET_COMMON3.md): Stand nach $S/stand/d.md, Nachricht an main nur für Entscheidung,
„bereit zum Mergen“ mit Beweis oder Befund für andere. Vor dem Push des Konverter-Teils: Engine-Teil bewiesen melden.

### Der bestätigte Entwurf (ausgeschrieben)
1. **Erkennung (Generator).** Ein Platz ist eine Methode eines Engine-Objekts (Handle), die T& ohne const auf etwas
   gibt, das selbst kein Objekt der Engine ist, keine weiteren Eingänge hat und deren Rumpf am Baum genau
   `CompoundStmt{ReturnStmt{MemberExpr(CXXThisExpr)}}` ist (ImplicitCast/Paren dazwischen erlaubt). Jede andere
   T&-Funktion bleibt wie heute (der Knoten gibt eine Kopie, nur lesbar); der Konverter wartet beim Schreiben dort mit
   Grund.
2. **Der Platz ist der Ausgang `result` des Engine-Knotens.** Der Knoten ist ein Schritt; wenn er läuft, bindet er
   (= `auto& r = world->shots();`, einmal). Die bestehenden Platz-Knoten (Get, Set, Add to Variable, Take, die
   Listen-Schritte Add/Insert/Remove Index/Remove Item/Clear/Set Length/Append/…) nennen als Wurzel statt einer
   Variablen diesen Ausgang: Einstellung `variable` leer, neue Einstellung **`bound` = Id des Engine-Knotens**; Weg
   (`path`, `path0…`) wie gehabt. Schreibweise:
   `engine_game_world_shots(n12, World); list_add(n12.result, e); n12.result[i].to = v;` gelesen `n12.result[i].to`,
   `list_length(n12.result)`. `world_->shots().push_back(e)` = der Schritt + list_add (auch im C++ ein Aufruf).
3. **Regeln (check).** Der Ausgang eines Platz-Knotens hat keine Drähte (nur Platz-Knoten mit `bound` lesen ihn – ein
   Draht wäre ein Schnappschuss, die Klasse liest dagegen den aktuellen Platz). Der Binde-Knoten muss jeden
   Platz-Knoten, der ihn nennt, im Ausführungsfluss dominieren (jeder Pfad vom Einstieg der Kette/Funktion zu dem
   Exec-Knoten, der den Platz liest/ändert – bei reinen Get-Knoten: jeder Exec-Knoten, der ihn transitiv liest –
   führt durch den Binde-Knoten), und zwischen beiden liegt kein latenter Knoten (Delay, Warten, Handler-Grenze):
   sonst Fehler des Graphen. Ein Platz der Engine als Referenz-Argument eines Aufrufs (`<Eingang>.bound`) zunächst
   von check() abgewiesen (die Beside-Logik von Instance::spot für weitergegebene Referenzen wäre sonst nötig).
   „Gehaltener Eintrag bleibt“ gilt wie für Variablen.
4. **Interpreter (durch die Tür).** Ausführung des Platz-Knotens (engine_api.cpp `add_engine_nodes`, d.run): die Tür
   `F` wird wie heute gerufen (Aufzeichnung + Fehler wie im C++), aber in outputs[result] steht danach
   `Value::structure(in)` – die Eingänge des Aufrufs (das Handle), nicht die Kopie. `Instance::Spot` bekommt eine
   Wurzel „bound“ (Index des CNode + dessen gespeicherte Eingänge + Funktionsname + TypeRef des Ausgangs);
   `Instance::read`: `host.engine(F+"@get", in, out)` → bei Erfolg `value_at(out[0], t, steps)`, sonst die Null des
   Typs am Weg (value_at auf zero_of); `Instance::write`: @get, `change_at(v, t, steps, change)`, dann
   `host.engine(F+"@set", in+[v], …)`; scheitert @get, wird nichts gesetzt (der Host hat es schon gesagt). In
   `Instance::spot` für diese Wurzel KEINE Beside-Erkennung durch Ablaufen (das wäre ein zweiter @get-Aufruf und
   zweimal „gesagt“); value_at/change_at behandeln Indizes neben der Liste schon richtig (Default-Eintrag, verworfen).
   Graph::root_of/path_of/place_of: neue `Root::bound` – Typ aus `engine_function(type)->outputs[0]` des Knotens mit
   der Id aus `bound` (nur ohne prefix). JIT: codegen.cpp kennt keine Wege → Graphen mit Platz-Wegen laufen im
   Interpreter; prüfen, dass jit_fits_ auch für `bound` (ohne path) nein sagt.
5. **Klasse (direkt, typisiert, ohne Kopie).** `n12.result` ist ein generierter Stellvertreter-Platz
   `ev::Place<Tag, C>` (engine_direct.h + engine_nodes.gen.h) mit: Handle, Zeiger auf das C der Engine
   (`std::vector<kern::game::ShotEvent>`), Generation. Binden = ein Nachschlagen. Zugriff: Generation gleich und Zeiger
   da → direkt; sonst `object_of(handle)` neu (setzt o.error genau wie die Tür) – gefunden: Zeiger/Generation
   erneuern; nicht gefunden: `kern_engine_failed(F+"@get")`, Lesen = Default des Blueprint-Typs, Schreiben ins Leere.
   **EngineObjects bekommt einen Zähler der Freigaben (Generation)**, der bei jedem release steigt: so schreibt die
   Klasse nie auf einen toten Zeiger und tut genau, was die Tür tut (die schlägt jedes Mal nach). Listen: `size`,
   `[]` (neben der Liste ein eigener Eintrag mit Defaults wie kern::List), die Listen-Schritte (Überladungen von
   list_add/list_insert/list_remove/list_clear/list_resize/list_append … aus nodes.h für Place); Struktur-Einträge:
   generierte Proxy-Strukturen je Engine-Struktur mit Feld-Plätzen (Lesen per Konversion → Blueprint-Typ, `=`,
   `+=`, Direct<Tag> je Feld – f32 erst beim Zugriff nach f64 und zurück, genau was das C++ auf dem f32-Feld tut).
   Achtung: Vorlagen, die ihren Typ aus dem Argument ableiten, sehen den Proxy – Lesestellen in der Schreibweise
   brauchen ggf. einen expliziten Lesezugriff. Ohne Objekte der Engine (Test mit Stellvertreter: kern_world.engine
   == null) arbeitet derselbe Proxy durch die Tür: @get → Conv in eine eigene Kopie C, Operation, bei Schreiben @set.
   Gesagt wird bei Zugriffsfehlern immer `F@get` (der Interpreter liest vor jedem Schreiben).
6. **Stellvertreter (EngineStandIn, kern/src/logic/engine_api.cpp; und converted::engine_call im Konverter-Test):**
   muss je (F, Handle) den Wert hinter dem Platz als Zustand halten: @get gibt ihn (anfangs aus der Form der Ausgabe
   gewürfelt wie bisher F), @set setzt ihn; @get/@set werden NICHT als Aufrufe aufgezeichnet (der Quelltext ruft
   `shots()` einmal und schreibt dann in den Speicher – nur F wird aufgezeichnet). Für t1 (Test-Schreiber
   tools/cpp_to_blueprint.py TestWriter.stand_ins): die Stellvertreter-Klasse im Quelltext-Test muss für eine
   Platz-Funktion eine Referenz auf ein Mitglied geben, das mit dem Zustand des Stellvertreters (F, Handle)
   übereinstimmt – also vor dem Aufruf aus @get gefüllt und danach (am Ende des Aufrufs der Funktion des Spiels oder
   vor jedem weiteren Engine-Aufruf) per @set zurückgeschrieben; das muss mit t1 konkret abgestimmt bzw. selbst gebaut
   werden (Koordinator: „baue den Stellvertreter-Teil selbst und sag mir, was t1 wissen muss“).
7. **Konverter (klein, lokal):** `world_->shots().push_back(x)` → Schritt + list_add auf `bound`;
   `auto& r = world_->shots(); r[i].f = v;` → ein Schritt (gebunden einmal), jede Nennung von r = Platz auf `bound`;
   `world_->shots()[i]` → Schritt + Get am Platz. Nur für Funktionen mit place=true; sonst warten mit Grund.
8. **Beweis:** Engine-Test (Klasse direkt vs. Tür vs. C++ auf GameWorld::shots/hud_mut: lesen, Feld eines Eintrags
   setzen, anhängen, Länge; Speicher gezählt: direkt 0 Anforderungen außer dem push_back selbst; Release während
   gebunden → beide Stufen „ungebunden“, gleicher Text an den Host); nativer Fall (engine_case.gen.h aus
   case_text: je Platz-Funktion binden + lesen/setzen/anhängen, auch auf der echten Engine); Fall mit jedem Knoten
   (test_main.cpp) mit `bound`; Palette (logic_edit.cpp); Doku kern/docs/blueprint_cpp.md neuer Abschnitt „Plätze der
   Engine“ (nach „Knoten der Engine“, Zeile ~1154) und Kopfkommentare (engine_api.h, engine_direct.h).

### Stand der 7 Schritte
1. [x] **Generator-Erkennung** – `Api.looks_up` + `place` in `Api.function1` (tools/cpp_to_blueprint.py, nach
   `ret = qtype(d)...`), `EngineFunction::place` (engine_api.h, Default false, nur für Plätze `, true` in der
   Tabelle), `engine_api.py --list` zeigt „place“. Ergebnis (Lauf $S/d/list.txt): 6 Plätze –
   engine.game_world.hud_mut → game_Hud, .events → List<Text>, .sound_cues → List<game_SoundCue>, .shots →
   List<game_ShotEvent>, engine.building_builder.chunks / road_builder.chunks → List<GeoChunk>. (Die AST-Form von
   shots wurde geprüft: CompoundStmt→ReturnStmt→MemberExpr(shots_)→CXXThisExpr.)
2. [x] **Tür-Operationen** – engine_api.py `Calls.texts`: je Platz `c<k>_set` (`ev::take<Tag>(o, in, 1,
   self->shots())`) und Einträge `F@get` (= c<k>) / `F@set` in kCalls. Erzeugt in src/game/engine_calls.gen.cpp,
   src/geo/engine_calls.gen.cpp; Syntax g++ -Werror ✓. Noch von niemandem gerufen. engine_nodes.gen.h und
   engine_case.gen.h sind unverändert (Platz-Knoten dort noch wie bisher: Kopie).
3. [ ] Interpreter: Root::bound, Spot-Wurzel, read/write über @get/@set, d.run speichert die Eingänge (Punkt 4).
4. [ ] Klasse: EngineObjects-Generation, ev::Place + generierte Proxys, engine_nodes.gen.h (Punkt 5).
5. [ ] Schreibweise (kern/src/logic/notation.cpp: Lesen von `n12.result…` als Platz, Schreiben), check()
   (Punkt 3), Palette.
6. [ ] Tests, nativer Fall, Doku (Punkt 8).
7. [ ] Stellvertreter (Punkt 6) und Konverter (Punkt 7); vorher Engine-Teil auf mag beweisen und melden.

## C. Wo der Code steht (gelesen, spart Suche)
- Plätze: `Graph::root_of` (kern/src/logic/logic.cpp ~864), `path_of` (~909), `place_of` (~958); `Graph::Root`
  (logic.h ~579, `writable()`); `Instance::Spot` (logic.h ~1067: key | entry/output | beside, steps);
  `Instance::spot` (logic.cpp ~2742), `read`/`write` (~2840/2851), `value_at`/`change_at` (~2692/2708),
  `Instance::place`/`change` (~2868) – alle Platz-Knoten in core_nodes.cpp laufen darüber (Zeilen 458, 480, 504,
  536, 741, 861, 891, 1016, 1238); `Context::spot` (~3386) für Referenz-Argumente. check()-Regeln zu Plätzen
  ~2100-2125. notation.cpp: Root-Nutzer ~758-830, 1047, 1296, 1341, 2865-2879, 3019-3022, 3742, 3861.
- Engine-Knoten im Interpreter: kern/src/logic/engine_api.cpp `add_engine_nodes` (d.run ruft host.engine, outputs ab
  Index 1 bei Schritten). Tür: engine_host.cpp `call_engine` (begin_call, Tabelle nach Name),
  engine_host.h Conv/take/give/object_of. EngineObjects in engine_host.h.
- Klasse: engine_direct.h (Direct<Tag>, Given, taken, Chars/String), engine_nodes.gen.h (je Knoten zwei Wege:
  `kern_world.engine` gesetzt → direkt, sonst Tür).
- Generator: tools/engine_api.py (table_text, case_text, Calls.body = Tür, Calls.direct = Klasse, Calls.texts);
  Laufzeit ~1 Min. Rechnen, aber unter heavy3.lock oft lange Wartezeit. Aufruf: `TMPDIR=$S/shared-tmp flock
  $S/heavy3.lock nice python3 tools/engine_api.py` (ohne Argument schreibt er; `--list` nur Liste).
- Die Engine-Klasse mit Glue lokal erzeugen/prüfen (libc++): Text aus engine_case.gen.h ziehen, `tools/blueprint_cpp.sh
  --apply <level> EngineCase <text.cpp>` dann `tools/blueprint_cpp.sh <level> out.cpp`, dann
  `/opt/wasi-sdk/bin/clang++ --target=wasm32-wasip1 --sysroot=/opt/wasi-sdk/share/wasi-sysroot -std=c++20
  -fno-exceptions -fno-rtti -Wall -Wextra -Wno-unused-parameter -Werror -fsyntax-only -Ikern/include
  -Ikern/third_party/box3d/include out.cpp` (der rohe Text ohne Glue übersetzt NICHT – die Held<>-Spezialisierungen
  der Strukturen stehen im Glue). Host-clang hat kein libc++ installiert; wasi-sdk liegt in /opt/wasi-sdk.

## D. mag / CI
- mag-Aufträge `work-d-engine-161602-25272` (Stand c65428b) und `work-d-engine-163138-9400` (Stand 30f63e4) – beide
  durch CI 44337346531 überholt, Ergebnisse kämen nach $S/mag/<name>/; nicht mehr nötig. mag hing bis zum Ende eines
  vollen Laufs in der alten Schleife (Nachtrag 16:50).
- Die native Stufe (blueprint_native_test, game_native_test) läuft in CI-Job „native“ (`CXX=g++ kern/tests/run.sh`,
  ASan/UBSan) und in `$S/mag.sh engine origin/work-d`; libc++ prüft nur der CI-Job „build“ (tools/export_game.sh nach
  wasm) bzw. lokal wie oben.

## E. Fallen
- `flock -n` gibt ohne Meldung 1 zurück, wenn die Sperre belegt ist – `-E 99` setzen oder ohne -n warten.
- Generator-Ausgabe hängt am Stand der Header: nach Merges von work neu laufen lassen und prüfen, dass git diff leer ist.
- `f["place"]` gibt es nur bei Funktionen aus Api.function1; maker/releaser haben das Feld nicht → `f.get("place")`.
- Im nativen Test heißt die Funktion `differs(...)` im erzeugten main; lokale Variablen dort nicht so nennen.
- Text-Ausgaben „told“ müssen in allen Stufen Zeichen für Zeichen gleich sein (Funktionsname + o.error).
