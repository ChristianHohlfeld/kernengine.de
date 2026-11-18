# Paket g – Stand (Regeln des Konverters für die Game-Dateien), Zweig work-g1

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
