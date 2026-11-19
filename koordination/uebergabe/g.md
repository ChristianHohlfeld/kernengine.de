# Übergabe Paket g (Konverter-Regeln für Game-Dateien)
Worktree /home/claude/w-g1, Zweig work-g1 (auf work-zusammen 5a445af8 gebracht, gepusht).
Auftrag: $S/PAKET3_g.md. Stand-Tabelle: $S/stand/g.md.
Rohdaten: $S/mag/work-t2-vergleich-223555-30482/einzeln (je Test ganze Ausgabe), einzeln-real (Engine selbst).
## Erkenntnisse
(noch keine)
- c4d4544b: for_loop (cpp_to_blueprint.py) – Zählschleife nur, wenn die Grenze die Engine nicht fragt (asks_engine). Beleg: work-g1-vergleich-232903-26675.
- Alle anderen Abweichungen der Liste sind keine Konverter-Regel (Befunde für t1/d, E9-Frage) – siehe stand/g.md-Tabelle. Nachricht an main 23:30 gesendet.
- Nächstes: Ergebnis des Belegs holen; danach volle Liste work-zusammen-voll-232247-24031 (converted_Game__*), stand/t1.md „SUITE 232247“.
- c4d4544b BELEGT (work-g1-vergleich-232903-26675), BEREIT ZUM MERGEN eingetragen. E13 entschieden: eject/state_of warten auf t1s clang-Umstellung.
- 00:00 Neuer Auftrag: Pendeln. (2) 082acc7d gebaut (held/fell im main-Ablauf, World.bars). (1) wartet auf Probe work-zusammen-probe-235554-4594
  und Diagnose work-zusammen-konvert-235338-1934 (Koordinator). Verdacht: Stücke (G.foreign / w.pieces.setdefault über sorted(need)) wandern
  zwischen Game-Teilen und tauschen Fassungen (Fun mit/ohne multiplier-Member, g_fun) Runde um Runde.
- 00:23 2f8b97fc (Diagnose mit Werten im Kreis) + 082acc7d; Beleg/Diagnose: konvert work-g1-konvert-002324-11083 (37 Kerne). Kleine Probe
  work-zusammen-probe-235554-4594 konvergiert (combat_hud geschrieben) → Pendeln nur im ganzen Programm. Wiederkehrende Testfehler im Kreis:
  „kReliable/kUnreliable/g_fun/kOlive not declared … did you mean …<datei>_cpp_local::X“ (Text einer Methode aus net_session.cpp/combat_rules.cpp
  unqualifiziert, obwohl das Stück im <datei>_local steht – qualify() in TestWriter.items).
- 00:53 konvert 002324 rc=1: Kreis = wechselnde Wartegründe (explosion: look_blast/explosion „not written“). 480af66b: Regel verallgemeinert
  (wechselt im Kreis + wartet → wartet für immer, Grund + „(it went round with <BP>)“). Merge work-zusammen 9f10aa88 → 2a5d6f77.
  Beleg: work-g1-konvert-005330-16535. Bei rc=0: sofort main melden (er mischt ein und legt voll ab). Danach (1): Wurzel der gepinnten.
- 01:21 konvert 005330: Pinnen griff (16 Funktionen), dann ValueError (Variable `more` überschrieb --program-Liste) → 7bd9ac5d. Neu: work-g1-konvert-012113-4171.
  Wurzel offen: explosion/wreck_car (game_town) „look_blast()/explosion() of the Blueprint Game is not written“ = Unit.foreign: Schlüssel fehlt in world.table
  (w.table aus tables der need-Einheiten). Idee: WHY=explosion (Env, druckt die Stelle) in einem ganzen Lauf.
- 02:05 Festhalten belegt im Zwischenstand von work-zusammen-voll-012215-4354 (Fixpunkt Runde 18). Danach Lesefehler (atoi → wrap32s ohne Formel) →
  96d52bef (cut_low), lokal mit lesen.py an den Texten von 012215 belegt. Rekursion: stand/g.md „Rekursion“. Offen: combat_hud/g_fun und die Familie
  „<datei>_local“-Stücke über dritte Teile (stand/g.md „Offen“). Kopie der Texte: $S/shared-tmp/g-lesen (zum Löschen frei).
  Nachrichten an main gingen ab ~02:00 nicht mehr (Kontingent) – Bericht über SubagentHandback.
