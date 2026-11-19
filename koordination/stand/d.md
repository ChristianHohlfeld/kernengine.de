# Stand d (18.11., Nachfolger)
- fertig+bewiesen+gemergt: Stufe 4 (d) (CI 44337346531)

## A – rote Engine-Suite (R129): Fix gebaut, Beweis läuft
- Fix-Commit **6f62201** (Eltern: origin/work 90b5ab8 – per cherry-pick/merge ohne Konflikt übernehmbar; enthält NUR den Fix),
  in work-d gemergt als 5367e14 (Konflikte nur mit edae85f in engine_api.h/engine_api.py/cpp_to_blueprint.py, Generator neu).
- Ursache: Jobs liefen nativ auf Worker-Threads – ob der Job des Falls noch in der Schlange lag, wenn take_queued kam, hing vom Ablauf ab
  (Klasse schnell → noch da, 16 Einträge; Tür langsam → Worker hatte ihn). take_queued/done nativ zudem ohne Sperre (Datenrennen).
  R122_0 (Job-Nummer), R126 (pending), R118 (last_error) waren nur „nicht wiederholbar“, weil die drei Spiele in einem Prozess den
  Zustand der Engine weitertrugen. Echt nicht wiederholbar: R120 now_ms, R121 wall_ms (Uhren).
- Lösung: jobs::set_threads(false) (Jobs in pump auf dem Frame-Thread, wie Web ohne Worker; allgemein, zwei neue Knoten), Sperren in
  take_queued/done; jedes Spiel per fork vom selben Zustand der Engine; Uhren aus der Tabelle (KERN_CLOCK → EngineFunction::clock),
  Variablen über die Drähte bestimmt; jede andere Abweichung (auch Tür/Tür) ist jetzt ein FAIL.
- Beweis läuft: mag work-d-engine-174905-11207 und work-d-engine-174911-11261 (Stand 5367e14), CI 44351625418.

## C – Antworten
- `Mixer::bus(Bus)` → `BusSettings&`: trägt der Entwurf NICHT (Platz nur ohne weitere Eingänge, Rumpf genau `return member_;`).
  Allgemein bräuchte es: Plätze mit Eingängen als Wert – gebunden mit den Werten der Eingänge zum Zeitpunkt des Bindens (wie C++ das
  Argument einmal auswertet), @get/@set bekommen sie mit (in = [Handle, Eingänge…]); Erkennung am Baum: Rumpf `return m_[p];`
  (ArraySubscript auf ein Feld fester Länge, Index nur aus einem Parameter, keine Aufrufe). Achtung: `Bus` hat den Wert `count`
  → `buses_[count]` liegt neben dem Feld (UB in C++); dafür gälte die Regel „Platz neben einer Liste“ (eigener Default-Eintrag, im Test
  markiert) oder die Stelle wartet – keine stille Annahme.
- `TextureDesc::rgba` (`const u8*` in einer Struktur der Engine, Länge width*height*4 nur im Kommentar): trägt der Entwurf NICHT (er
  betrifft Funktionen, die T& geben, keine Zeiger-Felder). Allgemein bräuchte es eine Länge, die der Typ sagt: z. B. das Feld als
  `Span<const u8>` (Zeiger + Länge) ↔ Liste in der Struktur des Blueprints (Conv/Direct: Zeiger auf die Daten der Liste, ohne Kopie,
  wo der Elementtyp gleich ist – bei List<Int> (64 Bit) gegen u8 ist eine Kopie unvermeidlich, solange die Sprache keine Byte-Liste hat).
  Aus einem Kommentar die Länge abzuleiten wäre eine Sonderregel.
- engine_api.py: Änderungen für „Platz der Engine“ noch nicht fertig (Vermerk folgt hier, sobald gepusht – Paket c wartet).

## B – Platz der Engine (in Arbeit, Nachfolger 2)
- A erledigt: origin/work-zusammen (8874e79) eingemischt – Fast-Forward (work-zusammen enthielt work-d schon), gepusht.
- Schritt 3 (Interpreter) fertig, Syntax ✓, gepusht: 6310a9e (Root::bound, Spot::bound, Context::bind, read/write über @get/@set,
  EngineStandIn::places). In Arbeit: Schritt 4 (Klasse).
- Übergabe: $S/uebergabe/d.md

## BEFUND earcut (Koordinator 21:2x) – Fix gebaut, Probe läuft
- Commit **690ea4f** (work-d): Zeiger-Eingänge zählen Count<Per>: earcut.h `Count<2> vertex_count` (triangulate, deviation),
  ev::numbers() in Tür/Klasse (engine_api.py, .gen neu), Test-Schreiber-Stellvertreter (cpp_to_blueprint.py, Zweig der
  Zeiger-Eingänge ~Z. 10956: `per * count` Werte). Allgemein, keine Namensliste. Für t1: kleine Änderung in TestWriter (eine Zeile `per`).
- Probe: mag `work-d-probe-212615-8375` auf 690ea4f: convert rc=0; CityStream::building_shape, Destruction::tear/collapse/fracture
  „written“, Test übersetzt. Bit für Bit zeigt erst der nächste `voll` (Spiel-Suite).

## BEREIT ZUM MERGEN (earcut / Count<Per> der Zeiger-Eingänge) – nur Commit 690ea4f per cherry-pick (s. o.), Beleg: Probe work-d-probe-212615-8375
- Achtung: work-d trägt zugleich den unbewiesenen Zwischenstand „Platz der Engine“ (6310a9e, 64cb433). Nur den Fix
  übernehmen: `git cherry-pick 690ea4f` (setzt nur auf earcut/engine_host/engine_api.py/cpp_to_blueprint.py auf; danach
  `python3 tools/engine_api.py`, da engine_nodes.gen.h auch die Place-Teile hat).

## engine_api.py (für Paket c)
- Änderungen für „Platz der Engine“ (P_<Knoten>, Fields je Struktur, <Knoten>_place, Binden) und Count<Per> der
  Zeiger-Eingänge sind gepusht (64cb433, 690ea4f). Weitere Änderungen an engine_api.py durch d: nur noch Fall-Text (case_text).

## Platz der Engine – Stand 21:40 (Nachfolger 2, Lauf endet: Kontingent)
- Schritt 3 ✓ (6310a9e), Schritt 4 ~ (64cb433, nur Header-Syntax, nie instanziiert), Schritt 5 ~ (Schreibweise 64cb433,
  check() 2250736; Palette offen), Schritt 6 offen (Engine-Test, nativer Fall in case_text, Fall mit jedem Knoten, Doku), C/D offen.
- Einzelheiten und nächste Schritte: $S/uebergabe/d.md, Abschnitt B „Stand der 7 Schritte“.

## Platz der Engine – Nachfolger 3 (ab 22:20)
- work-zusammen (8844531a) eingemischt: d6869e1. Commit **c0d6d24**: Klasse instanziiert (nativer Fall EnginePlaces, Engine-Test
  engine_places_class_names_the_engines_list gegen C++ BuildingBuilder::chunks, Fall mit jedem Knoten mit `bound`, case_text mit Plätzen);
  Leser/Schreiber-Fehler behoben (Member `_place` wurde abgelehnt bzw. nicht geschrieben). Lokal (Harness $S/d/): Interpreter = Klasse auf
  Stellvertreter und auf der Engine (geo), Fixpunkt, g++/clang++ -Werror. libc++ (wasi-sdk fehlt hier) prüft nur CI „build“.
- Beweis läuft: mag `work-d-engine-223942-31317`, CI 44403776223 (Stand c0d6d24).
- engine_api.py (für Paket c): case_text schreibt Plätze als `_place`-Member mit Lese/Setz-/Listenschritten und einem Lauf auf gemachten
  Objekten (Klasse mit Macher ohne Eingänge + Freigeber). Sonst unverändert seit 690ea4f.

## ENTSCHEIDUNG NÖTIG (d, 22:55): Wachstum von `list_append` auf einem Platz der Engine
- Befund: Werte sieht der Vergleich nie anders (dieselben Einträge). Die `cost:`-Zeile zählt Anforderungen: heute reserviert
  `engine_values::list_append` (engine_direct.h) genau `size+n` und hängt einzeln an. Die erste Anforderung stimmt mit C++
  `v.insert(v.end(), a, b)` überein (beide 1, wenn die Kapazität nicht reicht), die Kapazität danach aber nicht: Beispiel size 4,
  cap 4, +1 → hier cap 5, libstdc++ cap 8 (size+max(size,n)), libc++ max(2·cap, size+n). Ein folgendes push_back fordert hier
  Speicher an, in C++ nicht → `cost:` weicht um 1 ab. Blueprint-Listen (logic::list::insert_items) nutzen schon vector::insert und
  wachsen wie C++.
- Vorschlag: auf einem Platz `list.insert(list.end(), n, blank<C>())` (Füll-Einfügen: dieselbe Wachstumsregel der Bibliothek wie
  das Bereichs-Einfügen, in libstdc++ und libc++), danach jeden neuen Eintrag mit Direct::from setzen – eine Anforderung wie C++,
  dieselbe Kapazität danach, innere Listen je Eintrag wie beim Kopieren. Konform (kein Proxy-Iterator). Nicht umgesetzt – wartet auf Antwort.
- E7 (Antwort Koordinator: JA) umgesetzt: **cf8786c** – list_append am Platz = insert(end, n, blank) + Direct::from; andere
  Listenschritte am Platz rufen schon dieselben logic::list-Funktionen (add/insert/remove_at/resize) – keine weitere Angleichung nötig.
  Nativer Fall EnginePlaces hat Lauf „Append“ (zweiter Platz auf dieselbe Liste).
- Weitere Commits: 42c86fe Palette („Places of the Engine“, Editor-Test), 62d0978 Doku. mag `work-d-engine-223942-31317` wartet noch
  (rechnet den Stand bei Start, also ≥ cf8786c); CI 44403776223 auf c0d6d24 läuft.
- Offen: Schritt 7 (Stellvertreter im Quelltext-Test, Konverter-Regel).

## Befund k (Array-Parameter fester Länge) – Fix gebaut, Beleg läuft
- **50832229** (Eltern: work-zusammen e9eec02, nur dieser Fix; in work-d gemergt als 54ae4f7): `T name[N]` liest N Werte –
  c2b.declared_length (aus der Quelle der ParmVarDecl), TestWriter zeichnet N Werte auf, Tür/Klasse weisen eine kürzere Liste ab.
  Generator neu (physics/game engine_calls.gen.cpp, engine_nodes.gen.h). Beleg: mag `work-d-vergleich-225255-10477` (src/drone.cpp →
  converted_rigid_spec), läuft. engine_api.py (für c): Prüfung der festen Länge bei Zeiger-Eingängen.
- Schritt 7 (Konverter + Stellvertreter der Quelle) gebaut: e02ea24, unbewiesen (Probe/vergleich über game_events.cpp o. ä. folgt).

## BEREIT ZUM MERGEN: 50832229 (Array-Parameter fester Länge, Befund k) – Beleg work-d-vergleich-225255-10477
- Eltern: work-zusammen e9eec02, enthält nur den Fix (cherry-pick oder merge sauber). mag `vergleich origin/work-d src/drone.cpp` auf ddb54b72:
  convert rc=0, bau rc=0; **converted_rigid_spec ok in beiden Durchgängen** (Stellvertreter und SC_ENGINE=real; vorher 300 of 300 calls differ).
  Übrig: converted_drone_cpp „Drone_step: 1 of 5 calls differ“ in beiden – das ist G7 von k (u16-Rückwandlung, 2a6ef5b3, nur auf work-k), nicht dieser Fix.

## STAND 23:40 (Nachfolger 3 endet – Kontingent): Befund-Block Zeiger/Array-Längen OFFEN, an Nachfolger 4
- Auftrag Koordinator (23:3x): VOR weiterem Platz-Beweis als ein Block, je Fix eigener Commit auf work-zusammen (5a445af8+), Beleg `vergleich`:
  (1) set_pcm (t2: gaps_guided_sounds 12/38) – `set_pcm(id, const f32* samples, u32 frames, u32 channels, u32 rate)` liest frames·channels;
      Stellvertreter der Quelle zeichnet 1 Zahl auf (Name „frames“ trifft die Namensregel nicht). Beleg: vergleich src/game_gaps.cpp.
  (2) matrix_of 88/88 (g): Form `{[n]}` für Mat4 (`f32 m[16]`) – ein Feld/Ergebnis `T m[N]` hat genau N Werte (Antwort des Stellvertreters,
      Beschreibung/Form). Beleg: vergleich über eine Datei, die actor_matrix ruft.
  (3) net_session ABBRUCH (g): `HostTransport::push_incoming(from, const u8* data, int bytes)` – Zahl neben Zeiger per TYP sagen
      (kern::Count<Per> in den Erklärungen der Engine), Namensregel `count|size|len|num|^n$` (engine_api.py Calls.body/direct ~Z. 457/593,
      TestWriter.stand_in Zweig Zeiger-Eingänge) abschaffen; alle Erklärungen mit Zeiger + Zahl umstellen; was danach ohne Count neben
      einem Zeiger steht, meldet der Erzeuger. Beleg: vergleich src/net_session.cpp.
  Hinweis zu (1): frames·channels ist kein festes Per – Count<Per> trägt es nicht. Möglichkeit: Zahl der Samples als Count<1> (frames = n/channels
  in sound.cpp; alle Spiel-Aufrufer geben channels=1, studio.cpp:82 gibt frames·channels) – das ändert die Bedeutung eines Engine-Parameters:
  mit dem Koordinator klären, falls nicht offensichtlich.
- Vorbild: 690ea4f (Count<Per> earcut), 50832229 (declared_length, Array-Parameter) – beides gemergt.

## Platz der Engine – Belege (23:58)
- **Engine-Suite mag `work-d-engine-223942-31317` auf ddb54b72: rc=0** – u. a. ok logic_notation_every_node_type, engine_calls_every_node,
  engine_places_class_names_the_engines_list, studio_logic_places_of_the_engine, blueprint_native_test (55 Blueprints, 14107 Aufrufe, mit
  EnginePlaces und Engine-Fall mit Plätzen), game_native_test.
- CI 44403776223 (c0d6d24): native ✓, build (libc++/wasm) ✓, determinism ✓, input ✓, **e2e ✗**: „Game: its Blueprint compiles in the editor …
  Sound Cues: its output "result" is a place of the engine - read it with a node that names the place, not through a wire“ (+ Folgefehler
  „Play runs them as machine code“). Ursache: das Level auf dem Zweig ist noch von einem alten Lauf – dort liest ein Draht den Ausgang von
  engine.game_world.sound_cues (früher eine Kopie). Der Konverter schreibt jetzt `tmp = nX.result` (Get mit bound). Erst ein neuer voller
  Lauf, der das Level neu schreibt, macht e2e grün. Deshalb ist „Platz der Engine“ NOCH NICHT „bereit zum Mergen“ (CI nicht grün): der
  Koordinator entscheidet, ob es mit dem nächsten `voll` gemergt wird. CI auf dem letzten Stand: 44414094027 (läuft).
- Schritt 7 (e02ea24): vergleich `work-d-vergleich-231118-18156` (game_weapons.cpp + game.cpp, ddb54b72): convert rc=0, bau rc=0; in keinem
  geschriebenen Text steht ein Platz – die Funktionen mit shots() (drone_attack, jet_attack) warten aus anderen Gründen (aim_ray, ctl_screen_ray).
  Schritt 7 ist also gebaut und bricht nichts, aber noch nicht an einer Quelle belegt. FAIL dort: eject, state_of (auch in früheren Läufen),
  matrix_of + net_session (Befund-Block oben) – nicht von Schritt 7.

## Nachfolger 4 (ab 00:05, 19.11.) – Befund-Block Zeiger/Array-Längen
- Zweig **work-d-count** (auf work-zusammen e4aa2e8d), zwei Commits (E14 und Count hängen zusammen: ohne Count im Typ prüft niemand
  `samples`, und mit der Typ-Regel meldet der Erzeuger set_pcm/create_clip, solange dort `u32 frames` steht):
  - **0c6840cc** E14: `set_pcm(id, const f32* data, Count<1> samples, channels, rate)`, ebenso `Mixer::create_clip`; frames = samples/channels,
    Rest ≠ 0 abgewiesen; Studio::sound_pcm gibt frames·channels; Spiel unverändert (channels = 1). Engine-Test + Doku.
  - **f384bdfd** Count im Typ: Namensregel weg (engine_api.py ×2, TestWriter), count_per nur Count<Per>; Api.unsaid_count meldet jede ganze Zahl
    ohne Count neben einem Daten-Zeiger, engine_api.py hält an. 30 Erklärungen umgestellt (net/geo/scene/gfx/audio/game). Generator reproduziert.
  - Syntax lokal: g++ -Werror über 41 betroffene Dateien + test_main.cpp ✓; py_compile ✓.
- Beleg läuft: mag `work-d-count-vergleich-000943-8780` (net_session.cpp + game_gaps.cpp); Engine-Suite (Filter) folgt.
- **Beleg Punkt 1 (00:27):** `work-d-count-vergleich-000943-8780` (f384bdfd; net_session.cpp + game_gaps.cpp): convert rc=0, bau rc=0;
  Stellvertreter 45/45 ok, real 44 ok / 1 FAIL. **Session_push weicht in keinem Durchgang mehr ab, kein ABBRUCH converted_net_session (real ok).**
  Einziges FAIL (real, converted_net_session_cpp): Session_connected/join/stop „only 0 calls could be compared“ – wie vorher (g: Engine weist ab), nicht neu.
  Punkt 3 hier NICHT belegt: ohne game.cpp warten die Game-Funktionen („the struct Game has more than 96 fields“) – gaps_guided_sounds nicht verglichen.
- Punkt 2 gebaut: Zweig **work-d-array** = **83733dad** (auf work-zusammen e4aa2e8d): Api.shape schreibt `[N item]` für Felder fester Länge
  (Mat4 `{[16n]}`), answer gibt genau N; Engine-Test engine_stand_in_fixed_length. Syntax ✓, Generator reproduziert.
- Beleg-Zweig **work-d-belege** = work-zusammen 310c6e10 + work-d-count + work-d-array (Generator reproduziert). Läuft:
  `work-d-belege-vergleich-002858-13659` (game.cpp + game_gaps.cpp: matrix_of, gaps_guided_sounds). Danach Engine-Suite auf work-d-belege.
- **BEFUND (real-Durchgang, für t1/Koordinator):** actor_matrix (und jede Funktion auf GameWorld: eject, hide_soldier, step_parachute …) wird im Durchgang
  gegen die Engine abgewiesen, weil `GameWorld` keinen Default-Macher hat (Konstruktor `GameWorld(Renderer&, NightWindows* = nullptr)`;
  engine_make_default kennt nur Klassen mit Konstruktor ohne Eingänge) → on_missing macht kein Objekt → call_engine false → „refused“.
  Das ist keine Frage der Form. Vorschlag (allgemein, im Generator): ein Default-Macher auch für Klassen, deren Konstruktor nur Objekte der Engine
  nimmt, die selbst einen Default-Macher haben (und Parameter mit Default) – die mitgemachten Objekte unter eigenen Handles im selben EngineObjects,
  freigegeben mit ihm. Nicht umgesetzt (über Punkt 2 hinaus; Testrahmen real = t1) – Entscheidung des Koordinators.
- **Beleg Punkt 2+3 (00:57):** `work-d-belege-vergleich-002858-13659` (work-d-belege 07bd7074: work-zusammen 310c6e10 + work-d-count + work-d-array;
  game.cpp + game_gaps.cpp): convert rc=0, bau rc=0; **Stellvertreter 47/47 ok** – matrix_of weicht nicht mehr ab (Basis t2 223555: 88/88 und
  90/90), gaps_guided_sounds weicht nicht mehr ab (Basis 11/37, 12/38). **Real:** gaps_guided_sounds 39 verglichen, keine Abweichung.
  Real-FAIL nur „only 0 calls could be compared“ bei body, eject, hide_soldier, matrix_of, state_of, step_parachute – alles Aufrufe auf GameWorld,
  die die Engine abweist (kein Default-Macher, BEFUND oben); in der Basis genauso (matrix_of real 0 verglichen). Nichts Neues.
  cost-Zeile gaps_guided_sounds (Klasse fordert bis 3× Speicher, Quelle 0) – in der Basis genauso.
- Engine-Suite läuft: `work-d-belege-engine-005831-17971` (ganze Suite: E14-Test, engine_stand_in_fixed_length, geänderte Signaturen).
- **Punkt 4 (Auftrag Koordinator 01:0x: Macher für Klassen ohne Standard-Konstruktor)** gebaut: Zweig **work-d-maker** = **b317557a**
  (auf work-zusammen 2dacbbb5). Calls.default_makers: Werte → Null (ev::blank), Zeiger auf Objekt → null, Referenz auf Objekt der Engine →
  mitgemacht ohne Handle (ev::MadeWith, EngineObjects::put_made, Entry::owner); abstrakte Referenz → die eine abgeleitete machbare Klasse.
  Neu gemacht: Engine, GameAudio, GameWorld, SoundBank, net::Client, net::Server. `--list`: nicht gemacht nur kern::net::Transport (abstrakt).
  Hinweis: GameAudio(Mixer&, SoundBank&) bekommt zwei unabhängige Mixer (jedes Objekt für sich gemacht – keine Annahme über Teilen).
  Engine-Test engine_calls_default_objects_made_with, Doku. Syntax ✓, Generator reproduziert. Beleg folgt (vergleich net_session real; Engine-Suite).

## BEREIT ZUM MERGEN (d, 01:35): work-d-count = 0c6840cc (E14 set_pcm/create_clip) + f384bdfd (Count im Typ) – Belege work-d-count-vergleich-000943-8780, work-d-belege-vergleich-002858-13659, work-d-belege-engine-005831-17971
- Auf work-zusammen e4aa2e8d, nur diese zwei Commits (E14 zuerst: mit der Typ-Regel meldet der Erzeuger sonst set_pcm/create_clip). Session_push ok,
  kein ABBRUCH converted_net_session (real); gaps_guided_sounds ok in beiden Durchgängen. **Engine-Suite rc=0** (54469 checks, 0 failures) auf work-d-belege 07bd7074.

## BEREIT ZUM MERGEN (d, 01:35): work-d-array = 83733dad (Form `[N item]`, Feld T m[N]) – Belege work-d-belege-vergleich-002858-13659, work-d-belege-engine-005831-17971
- Auf work-zusammen e4aa2e8d. matrix_of: Stellvertreter ok (Basis 88/88). Real weiter „0 compared“ – Ursache GameWorld ohne Macher → Punkt 4 (work-d-maker).
  engine_stand_in_fixed_length ok in der Suite.
- Punkt 4 (work-d-maker b317557a): Beleg läuft – vergleich net_session.cpp + game.cpp (real: Session_connected/join/stop, matrix_of …).
- **Beleg Punkt 4 (01:54):** `work-d-maker-vergleich-013431-7648` (b317557a, ohne die Fixe 1–3; net_session.cpp + game.cpp): convert rc=0, bau rc=0.
  **Real:** converted_Game__game_cpp ok – matrix_of, eject, state_of, hide_soldier, step_parachute, body werden jetzt verglichen, keine Abweichung
  (vorher „only 0 calls could be compared“). Session_connected/join/stop: 300 verglichen, keine Abweichung (vorher 0). Übrig real: ABBRUCH-artiger
  ASan heap-buffer-overflow in converted_net_session = push_incoming (Punkt 1, auf diesem Zweig nicht enthalten); note Session_join/stop:
  „engine.client.release(n) (the handle names no Client that was made)“ in 52–62 von 300 Aufrufen neu gezogen (die Quelle gibt einen Client frei,
  den die Engine nie gesehen hat – Testrahmen, BEFUND FÜR t1). Stellvertreter: matrix_of / Session_push weichen ab = Punkte 2/1 (nicht auf diesem Zweig).
  alle-real.txt nennt converted_net_session_cpp FAIL, seine Einzelausgabe sagt „0 failures“ (wie bei game_gaps in 002858) – ungeklärt, Bitte an t1/Koordinator.
- Engine-Suite läuft: `work-d-maker-engine-015441-9451`.
