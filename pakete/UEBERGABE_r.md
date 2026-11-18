# Übergabe Paket r (rückwirkender Differenztest gegen origin/main) – Stand 18.11., abgebrochen auf Anweisung

Zweig `work-r` (von origin/work 955503b), ein Commit: tools/source_diff.py = origin/work-f2 + die Erweiterungen von
origin/work-f1 dreiwegig eingefügt (git merge-file, ohne Konflikt); kern/games/spawncity/tests/source_diff.h = origin/work-f2.
Achtung: f1 hat auf seinem Zweig source_diff.h NICHT auf dem Stand von f2 (150 Zeilen weniger) – beim Zusammenführen f2 nehmen.
Keine Spiel-Quelle geändert, keine Tests erzeugt/committet, kein Lauf auf mag.

## Umfang
`git diff --name-only origin/main origin/work -- kern/games/spawncity/src`: 117 Dateien (110 geändert, 7 neu:
game_destruction_sync.h, game_net.h, game_projsync.h, game_toss.h, game_townsync.h, shader_look.h, wire.h).
Als Einheiten des Werkzeugs (.cpp samt gleichnamigem Header; Header ohne .cpp für sich): 76.
Listen: $S/r/changed.txt, $S/r/units.txt (S = scratchpad). Aufruf: $S/r/run.sh <out-dir> <einheit>… im Verzeichnis
kern/games/spawncity (Protokoll je Einheit in $S/r/log/<einheit>.log). Ein Lauf des Werkzeugs je Datei dauert lokal ~2–10 s.

## Erster Durchlauf (--base origin/main, Ausgabe nur nach $S/r/out, nicht im Repo), 28 von 76 Einheiten, dann gestoppt
Test entstanden (noch NICHT gebaut/gelaufen): combat_art (1 Paar), destruction (3), facades (1), game_audio_web (1),
game_barricades (3), game_controls (12), game_events (4), game_foot_rounds (3), game_gaps (5), game_lamps (4).
Kein Test nötig: game_combat.h (jede alte Funktion unverändert, nur neue dazu).
Kein Test – Grund des Werkzeugs (jeweils eine allgemeine Regel, die fehlt):
1. „the actor of g_rules/g_military/g_jetpack: its name is not written where it is made“ – settings, combat_detail,
   combat_rules, controls, equipment: in origin/main steht der Name des ActorBlueprints nicht als Literal am Variablen-
   Initialisierer (actor_of sucht ein StringLiteral). Regel fehlt: Name aus Konstante/Konstruktor-Argument folgen.
2. „X has no counterpart in the new text (and others than its own file may call it)“ – blueprint_call.h number(f32),
   city_jobs tile_input(const TileKey&, f64, f64, const u8*, size_t), city_stream CityStream::tile(const TileKey&),
   common.h Rng::Rng(u32), drone Drone::~Drone(), game_destruction_sync Game::dsync(), game_equipment
   Game::ui_render_music(f32*, u32). Fehlende Paarregeln: geänderte Parametertypen (Zeiger+Länge → Sicht, f32 → f64),
   entfernte/umgezogene öffentliche Funktionen (in andere Datei/Header gewandert – Paarung über Dateigrenzen),
   Destruktor/Konstruktor, der implizit wurde.
3. „an entry by a pointer“ – car.cpp:17, combat_audio.cpp:180: Index über Zeiger im ALTEN Text (das neue hat ihn nicht mehr).
4. „the class Game has other fields in the new text“ – game.cpp: Game bekam/verlor Felder (pad, dsync_, toss_ …, in
   Teilzustände der neuen game_*.h gewandert). check_fields verlangt gleiche Felder; Regel für umgezogene Felder fehlt.
5. „- written by a macro (game_details.cpp:60)“ – Wächter für Operator, der aus einem Makro stammt.
6. „make_sounds(Mixer&, ClipId*) is gone, and nothing compared calls it“ – game_life.
7. „Game::look_view_models(f32) uses the variable g_vm_arms of the file, which the new neither has nor is handed“ –
   game_look: Dateivariable wurde Feld des Objekts (Regel bisher nur: Variable → Feld einer als Parameter übergebenen Struktur).
Die übrigen 48 Einheiten (ab game_military.cpp in units.txt) sind noch nicht gelaufen.

## Offene Grundsatzfrage (ungeprüft, vor dem ersten Bau klären)
Der Test fügt alten und neuen Text EINER Datei ein, alle übrigen Header aber einmal im HEUTIGEN Stand. Zwischen main und
work haben sich 117 Dateien zugleich geändert: der alte Text einer Datei wird gegen neue fremde Header vielfach nicht
übersetzen (geänderte Signaturen, z. B. Zeiger → Index/Referenz). Bei settings/city_jobs trat das nicht auf, weil die Basis
direkt vor der Absenkung lag. Vermutlich braucht es eine allgemeine Regel „mehrere Einheiten in einem Test“ (alte Fassung
aller Header, von denen der alte Text abhängt, in sc_old) – nicht begonnen. Ebenso offen: Methoden von Game (game_*.cpp)
brauchen ein gewürfeltes Game-Objekt; ob draw/carry das für alle Feldtypen kann, zeigt erst der Bau.

## Noch nicht getan
Bewusste Verhaltensänderungen seit main (git log -L / git log -- datei) sind noch nicht gesichtet; kein Fund gemeldet.
