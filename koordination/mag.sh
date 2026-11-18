#!/usr/bin/env bash
# Ein Auftrag an mag (56 Kerne), über GitHub:   mag.sh <art> <git-ref> [argumente...]
#   mag.sh probe origin/work-g1 src/game.cpp src/game_look.cpp     Konverter über diese Quelltexte (Pfade relativ zu kern/games/spawncity)
#   mag.sh engine origin/work-d [filter]                            Engine-Suite (kern/tests/run.sh)
#   mag.sh spiel origin/work-f1 [filter]                            Spiel-Suite (run_tests.sh, ohne neue Konvertierung)
# Der Stand muss gepusht sein. Wartet, bis mag fertig ist, und legt die Protokolle nach $S/mag/<name>/ (when, *.tail.txt, *.log.xz, texts/).
# Gibt when und das Ende jedes Protokolls aus. Ein Auftrag dauert: Probe wenige Minuten, Suiten 10-30 Minuten.
S=/tmp/claude-0/-home-claude/87277680-b1fd-5493-af97-2573ffd436c2/scratchpad
name="$(echo "$2" | sed 's|.*/||; s/[^A-Za-z0-9-]/-/g')-$1-$(date +%H%M%S)-$$"
(
  flock 9
  cd "$S/magjob" && git pull -q --rebase origin mag/auftrag 2>/dev/null
  echo "$*" > "jobs/$name" && git add "jobs/$name" && git commit -q -m "Auftrag $name" && git push -q origin mag/auftrag 2>/dev/null
) 9> "$S/magjob.lock" || { echo "Auftrag nicht abgelegt"; exit 1; }
echo "Auftrag $name abgelegt $(date +%H:%M:%S)"
cd /home/claude/kw
for i in $(seq 1 360); do
  sleep 20
  git fetch -q origin "mag/ergebnis-$name" 2>/dev/null || continue
  [ "$(git rev-list --count FETCH_HEAD -- mag-log 2>/dev/null)" -ge 2 ] || continue
  mkdir -p "$S/mag/$name"; git archive FETCH_HEAD mag-log | tar -x -C "$S/mag/$name" --strip-components=1
  echo "fertig $(date +%H:%M:%S): $S/mag/$name"; cat "$S/mag/$name/when"
  for t in "$S/mag/$name"/*.tail.txt; do echo "--- $(basename "$t")"; tail -25 "$t"; done
  exit 0
done
echo "kein Ergebnis nach 2 Stunden"; exit 1
