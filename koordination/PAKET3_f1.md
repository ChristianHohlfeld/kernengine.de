Paket „f1“. Lies zuerst $S/PAKET_COMMON3.md (S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad) ganz und was es nennt. Sperre: heavy3.lock. Zweig work-f1, Worktree /home/claude/w-f1.

Deine Dateien des Spiels (nur diese änderst du): src/city_stream.h/.cpp, city_jobs.h/.cpp, tiles.*, mvt.*, interiors.*, facades.*, terrain.*, water_look.*, und die Header der Engine, die als Einheiten engine_geo_* / engine_logic_logic warten (dort NUR, was allgemein ist – z. B. GeoProjector::init ruft das variadische fail(): eine Form von fail ohne variadische Argumente als allgemeine Fähigkeit der Engine). Größte Brocken: Interiors::build (306 Zeilen, „a vector constant written as CallExpr“), CityStream::work (ruft sich selbst), Terrain::build_render_tile.

Deine Liste: grep -E "^(city_stream|city_jobs|tiles|mvt|interiors|facades|terrain|water_look|engine_geo|engine_logic)" $S/f9_roots.txt (Grundursachen) und $S/f9_waiting.txt (alles Wartende, auch Folgefehler).
Ziel: in deinen Dateien wartet nichts mehr – jede Funktion geschrieben UND im Test verglichen.
