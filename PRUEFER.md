# Prüfer auf mag (Branch mag/ergebnis)

- Baut und testet jeden neuen Commit von origin/work mit 14 Kernen (CPU 42-55, nice 19, idle IO), immer nur einen Lauf; bewegt sich work während eines Laufs, kommt danach der neueste Commit dran.
- Dauerlauf: tmux-Sitzung `pruefer` auf mag (`~/spawncity-pruefer-loop.sh`), Lauf: `~/spawncity-pruefer.sh <commit>`.
- Logs (ganze Ausgaben): `~/spawncity-pruefer-logs/<commit>/engine.log`, `spiel.log`; Schleife: `~/spawncity-pruefer-logs/loop.log`.
- Eigener Klon: `~/spawncity-pruef` (~/spawncity bleibt unberührt). Build-Daten: /tmp/kern-tests, /tmp/spawncity-tests.
- Stoppen: `touch ~/spawncity-pruefer.stop` (endet nach dem laufenden Lauf bzw. binnen 60 s) oder tmux-Sitzung `pruefer` beenden.
- Erster Lauf 266887f: GRÜN, Engine 22m05s, Spiel 21m14s (2026-11-15 00:45–01:28).
