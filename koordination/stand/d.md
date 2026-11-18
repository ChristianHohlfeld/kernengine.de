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
