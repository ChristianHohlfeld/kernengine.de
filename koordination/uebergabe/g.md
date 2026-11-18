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
