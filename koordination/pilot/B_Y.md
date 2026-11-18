# Vergleichsweg B (Paket Y „values"), city_jobs

01:47 – Stand, ehrlich:
- Mein Weg für city_jobs war bisher **kein Konverter-Baustein**, sondern – wie Paket R mit wire.h – ein
  Umschreiben der Quelle (Commit 97d2c05 auf work-values): Vorlagen put<T>/Reader::get<T>/array<T> durch
  je Zahlentyp eine Funktion ersetzt (wire::put_u8/i32/u32/f32/f64, neu I32/put_i32/get_i32 in wire.h;
  put_f32s/u32s/f64s; Reader über kern::Span mit get_u8 … get_f64, f32s/u32s/f64s). Patch:
  /tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad/values/city_jobs_B.patch.
- Dazu im Konverter (allgemein, nicht nur city_jobs): ein Parameter kern::Span<const T> ist eine
  const-Referenz auf die Liste (vorher eine Kopie je Aufruf), eine Sicht auf eine ganze Liste
  (Span(vector)) ist deren Platz (Commit 94ee77c).
- Engine: nichts für city_jobs.
- Auf die Nachricht des Koordinators hin (city_jobs.cpp gehört P) nehme ich die Änderung an
  city_jobs.cpp auf meinem Zweig zurück (wire.h-Ergänzung I32 bleibt, schadet nichts); Zahlen der
  geschriebenen/verglichenen Funktionen folgen unten, sobald der Probelauf durch ist.

02:48 – Probelauf (Konverter ohne Test, ohne Level, city_jobs.cpp + destruction.cpp + mp_rules.cpp, Stand
work-values 31e466b ff., mit der umgeschriebenen Quelle aus dem Patch):
- city_jobs_cpp: **21 von 24 Funktionen geschrieben** (154 von 233 Zeilen) – alle put_*, alle Reader::get_*,
  f32s/u32s/f64s, points, spots, read_one_chunk, tile_input, read_features, read_chunks, chunk_count, read_chunk.
- Es warten: run_tile (call: init() of a GeoProjector – die Engine-Funktion ruft das variadische fail(), kein
  Knoten), run_buildings (call: begin() of a Int – BuildingBuilder als Engine-Objekt), buildings_input (a place in
  a list of another shape – std::vector<f64>(footprints.data(), … + size()) aus einem FootprintStream).
- Verglichen (Bit für Bit): in diesem Lauf nicht gemessen (Testlauf ohne --test; der Test kommt im Schlusslauf).
- Berührte Dateien: kern/games/spawncity/src/city_jobs.cpp, src/wire.h (I32, put_i32, get_i32);
  tools/cpp_to_blueprint.py (Span<const T> als const-Referenz, Sicht auf ganze Liste = deren Platz). Engine: nichts.
- Die Änderung an city_jobs.cpp ist jetzt auf work-values zurückgenommen (gehört P); der Weg liegt als Patch in
  /tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad/values/city_jobs_B.patch.
