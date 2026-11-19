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
3. [x] Interpreter (6310a9e, Syntax ✓): Graph::Root::bound + `bound_function(g, n, &binder)` (logic.h/.cpp), Spot::bound
   (Index des CNode), Instance::bound_now/read/write (@get → value_at/change_at → @set; @get scheitert: Null des Typs, Schreiben
   ins Leere, kein @set), Context::bind (Ausgang hält Value::structure(in) ohne Konversion), engine_api.cpp d.run bindet bei
   f.place; ungebunden = in {0}. EngineStandIn::places (Schlüssel engine_call_shown(F, in); @get/@set nicht aufgezeichnet;
   Anfangswert aus answer(shape) mit Saat aus "F@get"+Schlüssel). JIT: var_slot("") = -1 → Interpreter.
4. [~] Klasse (64cb433, nur Header-Syntax ✓, NIE instanziiert – erst ein nativer Fall/Engine-Test zeigt Fehler):
   EngineObjects::generation() (++ in end/clear/place). engine_direct.h: RefCore<Tag,C,B> (kern_at = C der Engine ODER
   kern_own = eigener Eintrag des Blueprint-Typs B; kern_door: Kopie durch die Tür, wrote() = @set), PlaceOps<D,…>
   (operator B, operator= gibt B wie das Set des Interpreters, +=, -=, [] mit eigenem Eintrag daneben), Ref, Field<D,…,Cm,Bm>
   (Feld per Namen, schaut beim Nennen über D::ref() nach – damit sieht jede Nennung die Generation), Place<P,B>
   (kern_bind, ref(): direkt mit Generationsprüfung, sonst object_of neu + engine_failed(P::get); ohne Objekte: @get in
   eine B-Kopie (own-Modus) + @set nach jeder Änderung), concept NamesPlace, ev::list_add/insert/remove_at/remove(B-Vergleich)/
   clear/resize/append/take. blueprint.h: Überladungen list_length/add/insert/remove_at/remove/clear/resize/append und
   var_take (die alten var_take mit requires !NamesPlace). engine_api.py: struct P_<Knoten> (Object, C, Tag, pin, get,
   set, at()), `template <class B> using <Knoten>_place = ev::Place<P_<Knoten>, B>;` in EngineNodesOf, KernNode-Fassung
   bindet; S_<Struktur>::Fields<D,C,B> (Blueprint-Feldname n, C-Feld c). Offen: list_append reserviert genau size+n
   (C++-insert wächst je nach Bibliothek anders – prüfen/klären).
5. [~] Schreibweise (64cb433, Syntax ✓, ungetestet): Writer place_text → `n<b>.result…`, member_text → `<Knoten>_place<Typ>
   result = {};`; Reader: bound_named/bound_ahead, root_here/naming → {"bound", id}; gelesen als Wert, als Anweisung
   `n12.result… = v;`/`+=`, in `n5.value = (n12.result… = v);`, als Platz-Argument der Schritte (Vorlese-Schleife ~Z. 2967 und
   variable_args). OFFEN: check()-Regeln (in logic.cpp check() vor `return out;` ~Z. 2229 einfügen: bound nur auf Knoten mit
   Pfad-Parameter und gültigem bound_function; keine Drähte vom Platz-Ausgang; kein `<input>.bound`; Dominanz: BFS über
   Exec-Drähte von allen Wurzeln ohne den Binder darf den Nutzer (bei Get: die Exec-Knoten, die ihn transitiv lesen) nicht
   erreichen; vom Binder zum Nutzer kein latenter Knoten (NodeDef::latent)), Palette (logic_edit.cpp).
   check() ✓ (2250736, Syntax ✓): Regeln am Ende von check() in logic.cpp. Palette offen.
6. [ ] Tests, nativer Fall, Doku (Punkt 8). Erst hier zeigt sich, ob Place/Ref/Field instanziierbar sind: zuerst einen
   kleinen nativen Fall (shots: binden, list_add, [i].to setzen, lesen, Länge; Release während gebunden) bauen und lokal
   mit wasi-sdk (Abschnitt C) übersetzen, dann Engine-Suite auf mag.
7. [ ] Stellvertreter (Punkt 6) und Konverter (Punkt 7); vorher Engine-Teil auf mag beweisen und melden.

### Stand Nachfolger 3 (18.11., 23:00)
- Merge work-zusammen: d6869e1. Schritt 4/5/6 gebaut, lokal belegt: c0d6d24 (Leser/Schreiber-Fix für `_place`-Member, Pins
  gebundener Knoten beim Lesen, blueprint.h-Lesefunktionen auf Plätzen, case_text mit Plätzen, nativer Fall EnginePlaces,
  Engine-Test engine_places_class_names_the_engines_list, Fall mit jedem Knoten), 42c86fe Palette, 62d0978 Doku, cf8786c E7.
- Lokaler Harness in $S/d/: build.sh (Logik als Objekte; `H=harness3 ./build.sh` für andere Hauptdateien), `harness <text.cpp> Läufe…`
  (lesen, check, Fixpunkt gegen .back, Interpreter/JIT gegen Stellvertreter, schreibt <text>.class.cpp mit Glue), build2.sh +
  `harness2 <text.cpp> Läufe…` (Klasse mit hineinübersetzt, geo-Engine: Interpreter-Tür gegen Klasse direkt). eplaces.cpp = Text des
  nativen Falls. wasi-sdk fehlt in dieser Sitzung → libc++ nur CI „build“.
- Beweis: mag work-d-engine-223942-31317 (holen), CI 44403776223 (c0d6d24); danach neuer CI-Lauf auf dem letzten Stand.
- E7 umgesetzt (cf8786c). Schritt 7 gebaut (e02ea24): Konverter engine(as_place)/engine_place (spot_held, `auto& r =`), self.bound
  (is_local/writable); TestWriter.stand_in-Zweig f["place"] → converted::engine_place<T, Shape>; converted.h OpenPlace/places_set/
  places_get/places_close (engine_call setzt vorher zurück, liest danach neu); rule_blueprint.h test_engine: @get/@set nicht aufgezeichnet.
  Beleg läuft: mag work-d-vergleich-231118-18156 (game_weapons.cpp + game.cpp: world_->shots().push_back).
- Befund k (Array `T name[N]`): 50832229 (auf work-zusammen e9eec02) BEREIT ZUM MERGEN, Beleg work-d-vergleich-225255-10477; in work-d als ddb54b7.
- Engine-Suite work-d-engine-223942-31317 wartete um 23:10 noch auf mag (rechnet den Stand bei Start); CI 44403776223 (c0d6d24): build ✓.
- Nächstes: Ergebnis des vergleich lesen (ob die Funktionen mit shots() jetzt „written“ und in beiden Durchgängen ok sind); Engine-Suite
  holen; danach neuen CI-Lauf auf dem letzten Stand; BEREIT ZUM MERGEN „Platz der Engine“ erst mit Suite rc=0 + CI grün.

### Stand 23:58 – für Nachfolger 4
- ZUERST: Befund-Block Zeiger/Array-Längen (stand/d.md „STAND 23:40“): set_pcm, matrix_of (`T m[N]` als Feld/Ergebnis), push_incoming
  (Count<Per> im Typ statt Namensregel). Je Fix eigener Commit auf origin/work-zusammen (Arbeitsweise wie bei 50832229: Zweig
  d-<name> von origin/work-zusammen im Worktree w-d, Patch anwenden, `python3 tools/engine_api.py`, committen, in work-d mischen,
  Merge-Nachricht deutsch + Attribution, pushen), Beleg `vergleich` (game_gaps.cpp / net_session.cpp / Datei mit actor_matrix).
- Platz der Engine: Engine-Suite grün (223942), CI e2e rot nur wegen des alten Levels (siehe stand/d.md 23:58); Schritt 7 gebaut, an keiner
  Quelle belegt, weil die Funktionen mit shots() aus anderen Gründen warten.
- Lokaler Harness: $S/d/ (siehe oben).

### Stand Nachfolger 4 (19.11., ab 00:05)
- Punkt 1+3 (Count im Typ, E14): Zweig `work-d-count` = 0c6840cc (E14) + f384bdfd (Count). Beleg: vergleich `work-d-count-vergleich-000943-8780`
  (net_session.cpp, game_gaps.cpp), danach `mag.sh engine origin/work-d-count sound` (bzw. ohne Filter). Danach BEREIT ZUM MERGEN + in work-d mischen.
- Arbeitsweise: Kurzzweig im Worktree w-d (`git checkout -b work-d-<kurz> origin/work-zusammen`), danach `git checkout work-d` und mischen.
  Syntax-Skript: /tmp/d_syn.sh <dateien> (g++ -Werror -fsyntax-only, unter flock heavy3.lock) – bei Verlust neu schreiben.
- Punkt 1 belegt (net_session): `work-d-count-vergleich-000943-8780` – Session_push ok in beiden Durchgängen, kein ABBRUCH.
- Punkt 2 gebaut: `work-d-array` = 83733dad (Api.shape `[N item]`, answer genau N, Engine-Test engine_stand_in_fixed_length).
- Beleg-Zweig `work-d-belege` (work-zusammen 310c6e10 + beide Kurzzweige): vergleich `work-d-belege-vergleich-002858-13659` (game.cpp + game_gaps.cpp:
  matrix_of, gaps_guided_sounds – Game-Funktionen brauchen game.cpp, sonst „struct Game has more than 96 fields“), danach `mag.sh engine origin/work-d-belege`.
- work-d: work-zusammen 310c6e10 eingemischt (e00222c9; Konflikt TestWriter: deren Zeroed-Fassung + meine places_close-Zeile).
- BEFUND real-Durchgang: GameWorld ohne Default-Macher → alle GameWorld-Aufrufe „refused“ (stand/d.md).
- 01:35: Punkte 1–3 BEREIT ZUM MERGEN (stand/d.md), Engine-Suite work-d-belege-engine-005831-17971 rc=0. work-d = c581d498 (beide Kurzzweige eingemischt).
- Punkt 4 (Macher, Auftrag 01:0x): work-d-maker = b317557a (auf work-zusammen 2dacbbb5), Beleg: vergleich work-d-maker-vergleich-013431-7648
  (net_session.cpp + game.cpp; real: Session_connected/join/stop und GameWorld-Funktionen müssen verglichen werden), danach `mag.sh engine origin/work-d-maker`.
  Danach BEREIT + in work-d mischen. SendMessage an main ging nicht mehr (Grenze) – nur Stand-Datei.

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

## Nachfolger 5 (19.11., ab 04:40) – Arbeitsliste D aus KOORDINATOR 04:40
- D1 Lecks: Ursache EngineObjects::place überschrieb einen Eintrag, wenn on_missing für ein Handle eine andere Klasse machte (Test-Handles sind
  Nummern je Klasse, kept[h % 8]). Fix work-d-leck 16d72b56 (je Handle Liste von Einträgen). Beleg work-d-leck-vergleich-044258-11385 (0 LECK).
- D2 weld: der Test zieht MeshData mit unstimmigen Listenlängen; weld las ungeprüft. Fix work-d-weld f31ea939 (Längenprüfung wie upsert).
- D3 „0 compared“: Komponente app nicht im Testprogramm gelinkt („this build has no function engine.input.down/engine.engine.aspect“).
  Fix work-d-app d11cb0cf (run_tests.sh + kern_create_app in tests/test_main.cpp).
- Beleg-Zweig work-d-belege2 f5030c78 = alle drei; vergleich work-d-belege2-vergleich-050059-20415, danach `mag.sh engine origin/work-d-belege2`.
- Danach: work-d aktuell halten (git merge origin/work-zusammen), Platz der Engine erst nach erstem grünen Stand.
- 07:05: alle fünf Fixe belegt und als BEREIT ZUM MERGEN gemeldet (stand/d.md): leck 16d72b56, weld f31ea939, app d11cb0cf+3cb92099, key 2562e856,
  release 7b1a05ed; Belege 044258-11385, 055045-6922, Engine-Suite 062159-12230 rc=0. work-d = 9949fd00 (work-zusammen eingemischt, Konflikt in place:
  Generation + owner). Beim späteren Mischen von leck in work-d: ++generation_ in place/end/clear behalten.
- Offen: im nächsten vollen Lauf prüfen, dass interiors/game_gaps/game_town (gleiche Leck-Ursache) ohne LECK sind; Platz der Engine erst nach erstem grünen Stand.

## Nachfolger 6 (19.11., ab 07:45)
- **Uhren gegen die Engine (gemergt, work-zusammen cf71d466):** work-d-uhr 4b7074b1 – sc::Clocks in tests/rule_blueprint.h: Antworten einer Uhr (EngineFunction::clock)
  an die Quelle je Stelle der Aufzeichnung; ein Blueprint-Aufruf an derselben Stelle mit demselben Aufruftext bekommt sie. Watch setzt g_clocks. Beleg 075109-9150.
- **Einzel-Lauf 070328-…-alle-080335-20196 ausgewertet:** nur gegen die Engine rot waren allein Uhren (city_stream, clock_ms) – erledigt; für d nichts weiter.
- **work-d = 0c4c551f** (work-zusammen cf71d466 eingemischt; Konflikt EngineObjects::end = leck-Fassung + ++generation_; Wartestelle „node that binds it is not written
  yet“ aus 1007cb7e entfernt – Emit.engine() bindet den Platz). Wichtig: vor 1007cb7e las engine_table() Platz-Einträge gar nicht, Schritt 7 lief nie.
  Beleg 082602-26333 (8 Game-Dateien): 58/58 beide Durchgänge, aber die Platz-Funktionen bleiben in Teil-Proben ungeschrieben („defined in another file“) →
  nächster Schritt: `./mag.sh konvert origin/work-d 9` (zeigt, ob die 8 Funktionen mit shots()/sound_cues() geschrieben werden) und vergleich über ALLE src/game*.cpp.
  Danach Engine-Suite auf work-d. NICHT einmischen ohne Wort des Koordinators.
- **Befunde 10:05:** work-d-opt 2226dbe9 (optional = `[?item]`, höchstens ein Eintrag im Stellvertreter), work-d-jobs a12da91a (fresh_engine: jobs::reset + set_threads(false);
  neues kern::jobs::reset). Beleg-Zweig work-d-belege3 20eeaaf0; vergleich `work-d-belege3-vergleich-101159-615` (terrain, city_stream, game.cpp), danach Engine-Suite.
  Danach beide in work-d mischen (Generator neu laufen lassen: engine_api.gen.cpp, engine_case.gen.h).
- **13:55:** belege3 gemergt (work-zusammen dc4229b5). **work-d-make 49e5c319** (auf dc4229b5): Ergebnis jedes Machers hat die Form `h` (1–7, nie 0; Api.maker
  „shapes“, engine_api.py, answer) – Befund k audio_init 1/20 (Stellvertreter gab sound_bank.make Handle 0 → bank_ leer). Engine-Test engine_stand_in_makes_an_object.
  Beleg läuft `work-d-make-vergleich-131505-2655` (game, game_equipment, sound_fx, game_details, equipment, combat_audio) → dann `./mag.sh engine origin/work-d-make`
  → BEREIT ZUM MERGEN. Zwei eigene Aufträge zugleich sind vom Koordinator erlaubt.
- Platz: `work-d-konvert-125538-27289` (konvert origin/work-d 9 auf 275d3ddf) läuft: zeigen, ob die 8 Funktionen mit shots()/sound_cues() geschrieben werden
  (xzgrep in convert.log.xz: foot_rounds_update, gaps_lamps_update, handle_shots, look_lighting, step_cop, step_soldier, ui_mix_cues, update_fighters_back,
  update_sound). Danach vergleich über alle src/game*.cpp + game.cpp und Engine-Suite auf work-d; work-d vorher mit origin/work-zusammen (fc8752ab+) mischen.
- Danach aus ks Liste (Koordinator 13:45): GeoProjector::init (fail variadisch → Engine-Knoten fail(Text)), look_* (`kern::Mesh*`), handle_shots/run_buildings
  (Platz), „a member of Int is set“ (TextureDesc als Engine-Objekt mit Handle) – Entwurf selbst entscheiden.
- Hinweis: zwei Merge-Commits (3c10e298 auf work-d-belege3, 275d3ddf auf work-d) tragen den Standardtext ohne Attribution (gepusht, kein Force-Push).
- **15:00 (Ende Nachfolger 6):** work-d-make 49e5c319 BELEGT und gemeldet (vergleich 131505-2655 52/52 beide Durchgänge; Engine-Suite 141127-4668 rc=0).
  Offen für den Nachfolger: (1) Ergebnis `work-d-konvert-125538-27289` (`./mag.sh holen …`; in convert.log.xz die 9 Platz-Funktionen suchen; rc und „written“-Zahl
  mit dem vollen Lauf vergleichen – kein Rückfall „was written and waits now“). (2) work-d mit origin/work-zusammen (≥ fc8752ab, samt make) mischen, Generator neu,
  dann `vergleich origin/work-d` über game.cpp + alle Game-Dateien mit Plätzen (game_events, game_lamps, game_look, game_foot_rounds, game_military, game_town, game_ui
  und was die Kette sonst braucht – besser alle src/game*.cpp) und Engine-Suite auf work-d → „BEREIT ZUM MERGEN“ erst auf Wort des Koordinators einmischen.
  (3) ks Liste (Koordinator 13:45): GeoProjector::init (fail variadisch → Engine-Knoten fail(Text)), look_* (`kern::Mesh*`), handle_shots/run_buildings (Platz),
  „a member of Int is set“ (TextureDesc als Engine-Objekt mit Handle; stand/k.md „BEFUND FÜR d (08:55)“).
- **15:45 (Nachfolger 7):** work-d e2f1dc6f (work-zusammen 4ac2e4f9 eingemischt + Fix Feld eines Platzes als Wert). Laufen: `work-d-vergleich-154229-25045`
  (game*.cpp + people.cpp), `work-d-konvert-125538-27289` (275d3ddf). Danach: Engine-Suite `./mag.sh engine origin/work-d` → BEREIT ZUM MERGEN. Gebaut, unbelegt:
  work-d-fail 5500074b (printf-Knoten; Beleg: vergleich src/city_stream.cpp + Engine-Suite). Entwurf Felder von Engine-Objekten: stand/d.md „Nachfolger 7“.
- **18:10 (Nachfolger 7, Ende):** work-d 739ca173: Teil-Beleg grün (154229: 99/99; Engine-Suite 163715 rc=0), aber Rückfall im ganzen Spiel (konvert 125538: 1633 statt 1725,
  370× „was written and waits now“, „with a Blueprint's numbers“, run_buildings/b.chunks()). Nächstes: `./mag.sh holen work-d-probe-173035-30178` → Fehler mit TEST_ERRORS
  lesen (convert.log.xz, „error:“), Stellvertreter des Platzes (TestWriter.stand_in, f.get("place") / converted::engine_place) für die Quelle mit Blueprint-Zahlen richten;
  Gegenprobe work-zusammen-voll-143319 (zeigt sie den Rückfall auch, ist es nicht d). Dann konvert 9 neu → BEREIT. work-d-fail: Engine-Suite 170043 holen → BEREIT.
  Nicht gebaut: Felder von Engine-Objekten (Entwurf stand/d.md „Nachfolger 7“). Offener alter Auftrag work-d-belege2-vergleich-055022-6799 (veraltet) wartet noch –
  abbrechen durfte ich nicht (Rechte).
- **19:00:** work-d e9ed65ca (Feld-Plätze + run_buildings-Fix, unbelegt). Offen: Engine-Suite `work-d-engine-185841-1710` holen; dann
  `./mag.sh vergleich origin/work-d src/game*.cpp src/people.cpp src/city_jobs.cpp`; work-d-fail: Engine-Suite `work-d-fail-engine-170043-6435` holen →
  BEREIT (vorher work-zusammen einmischen, c änderte engine_api.py). Platz als Ganzes BEREIT erst mit konvert ohne den fremden Rückfall (t1).
