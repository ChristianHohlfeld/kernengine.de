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
