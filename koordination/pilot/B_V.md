# Vergleichsweg B (Paket V, Zweig work-gamestate) – settings

01:48 Stand und Hinweis:
- Weg B für settings war bei mir KEINE Konverter-Fähigkeit, sondern eine Umschreibung der Quelle (Commit c13d62b,
  00:55 gepusht, vor der Entscheidung 01:46): Settings hält die Tabelle selbst (rows_, specs_ als Felder;
  Settings::take_table, spec, find statt bind_settings mit Tabelle / opt_spec / find_opt; bind_settings bindet
  nur noch den Regel-Blueprint). Berührte Dateien: src/settings.h, src/settings.cpp, src/game.cpp (Aufruf
  take_table nach bind_settings), src/game_net.cpp (ui_.settings.spec), src/game_ui.cpp (st.find),
  tests/test_settings.cpp (s.take_table()). Konverter/Engine: nichts dafür nötig.
- Genauso für world_traffic (Commit 2065841: TrafficRules::laws_ statt g_laws; take_laws, gap_speed const).
- Seit 01:46 ändere ich an der settings-Quelle nichts mehr. Konflikt mit P: mein Zweig enthält diese Umschreibung –
  beim Zusammenführen entweder P oder c13d62b (settings-Teil) nehmen, nicht beide.
- Zahlen (geschrieben/verglichen für settings_cpp und Game/settings_cpp): folgen unten, sobald der Probelauf
  (game.cpp, settings.cpp, game_ui.cpp) durch ist.

02:10 Ergebnis Probelauf (Stand work-gamestate b6a0c3a; Quellen game.cpp, settings.cpp, game_ui.cpp; KERN_JOBS=1, 1334 s):
- settings_cpp: 10 von 11 Funktionen geschrieben (72 von 89 Zeilen), alle 10 im Test verglichen (escape_into, num,
  bind_settings, Settings::take_table, spec, find, set(Opt, f32), reset_section, values_json, schema_json).
  Es wartet nur Settings::set(string_view, string_view) (17 Zeilen): „a pointer“ (std::strtod(text.c_str(), &end) –
  Texte zerlegen, Paket U).
- Game/settings_cpp: entfällt (0 Funktionen) – keine Variable der Datei mehr, die Tabelle ist ein Feld des Objekts.
- settings (Header): 3 von 3 (get, on, version).
- Nötig in Konverter/Engine: nichts. Nur die Quelle (s. oben).
- (Bitgleichheit selbst prüft erst der Schlusslauf; der Probelauf zeigt, dass die Tests geschrieben und übersetzbar sind.)
