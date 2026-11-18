Paket „g“ (Regeln des Konverters für die Game-Dateien). Lies zuerst $S/PAKET_COMMON3.md (S=/home/claude/kernengine.de/koordination) ganz, mit allen Nachträgen
(v. a. 17:45, 18:10, 21:10, 22:25). Zweig work-g1, Worktree /home/claude/w-g1 (der Zweig trägt alte, in work-zusammen zurückgenommene Absenkungen: zuerst
`git merge origin/work-zusammen`, danach ist er auf dessen Stand).

Stand (18.11., 23:25): Der Test-Schreiber (t2, jetzt in work-zusammen 5a445af8) gibt rund 100 Funktionen der Game-Dateien erstmals einen Test, der übersetzt. Ihr
erster Vergleich Bit für Bit (`work-t2-vergleich-223555-30482`: game, game_events, combat_rules, game_look, net_session, game_gaps, game_toss) zeigt 50 ok,
7 FAIL, 2 ABBRUCH (gegen die Engine selbst 6 FAIL, 3 ABBRUCH). Der volle Lauf `work-zusammen-voll-232247-24031` zeigt die ganze Liste aller Game-Teile.

Auftrag: Jede Abweichung in den Game-Dateien (src/game*.cpp, net_session.cpp, mp_rules.cpp, lamps und was Game sonst trägt), bei der Interpreter = Klasse ≠
Quelltext ist – eine Regel des Konverters tools/cpp_to_blueprint.py (oder eine Absenkung) übersetzt falsch –, finden, ALLGEMEIN beheben, Bit für Bit belegen.
Die anderen Dateien (city_stream, destruction, combat_*, drone, car, Firmware-Einheiten …) hat Paket k: dieselbe Datei tools/cpp_to_blueprint.py – kleine Commits,
vor jedem Push `git merge origin/work-zusammen`; berührt ein Fix dieselbe Regel wie einer von k (stand/k.md lesen), stimmt euch über stand/-Dateien ab.

Arbeitsliste zu Beginn (Rohausgaben: `$S/mag.sh holen work-t2-vergleich-223555-30482` → $S/mag/<name>/, `einzeln.tar.xz` auspacken; stand/t2.md „Befunde“):
1. game.cpp: `matrix_of` 88/88 (der Blueprint gibt Nullen statt der Einheitsmatrix), `eject` 65/65 und `state_of` 91/91 (die Quelle ruft actor_body, der
   Blueprint physics).
2. `look_apply_tier`: die Quelle ruft light_pool.release, der Blueprint light_pool.make.
3. net_session_cpp: 25 Session-Funktionen weichen ab; converted_net_session bricht gegen die Engine selbst mit ASan heap-buffer-overflow ab.
4. ABBRUCH converted_Game__game_look_cpp (und damit converted_Game__game): UBSan „null pointer passed as argument 1“ – ein std::string aus nullptr; Verdacht
   game_look.cpp:247 `q.name` mit `kTier[t]`. Klären, ob Blueprint, Test oder Quelle.
5. Danach: alle `converted_Game__*`-Tests des vollen Laufs (t1 wertet ihn aus: stand/t1.md „SUITE 232247“).
Nicht dein Gebiet – als „BEFUND FÜR …“ in stand/g.md und Nachricht an main: Fehler des Tests/Test-Schreibers (t1), Stellvertreter/Engine-Knoten (d; bekannt:
gaps_guided_sounds – set_pcm bekommt im Blueprint die ganze Liste statt einer Zahl), Fehler in der Klasse (p).

Methode und Belege wie bei k (PAKET3_k.md, „Nachtrag 22:25“, Punkte 3 und 4): Kleinstfall, allgemeine Regel, `$S/mag.sh vergleich origin/work-g1 src/<betroffene>.cpp`
in beiden Durchgängen ohne die Abweichung und ohne neue; höchstens EIN eigener Auftrag zugleich, wenige Quelltexte (ein Vergleich über game.cpp dauert lang:
nimm die kleinste Datei, die die Stelle zeigt). Entscheidung E9 gilt (Reihenfolge, die C++ offen lässt: Bedeutung des Compilers des Spiels, Absenkung mit
Differenztest unter clang). Nach jedem belegten Fix: Commit, Push, stand/g.md („BEREIT ZUM MERGEN: <commit> – Beleg <auftrag>“), uebergabe/g.md fortschreiben,
Nachricht an main.
