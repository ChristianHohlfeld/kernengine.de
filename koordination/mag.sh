#!/usr/bin/env bash
# Ein Auftrag an mag (56 Kerne), über GitHub:   mag.sh <art> <git-ref> [argumente...]
#   mag.sh probe origin/work-g1 src/game.cpp src/game_look.cpp     Konverter über diese Quelltexte (Pfade relativ zu kern/games/spawncity)
#   mag.sh engine origin/work-d [filter]                            Engine-Suite (kern/tests/run.sh)
#   mag.sh spiel origin/work-f1 [filter]                            Spiel-Suite (run_tests.sh, ohne neue Konvertierung)
#   mag.sh konvert origin/work-k 8                                  nur die Konvertierung des ganzen Spiels mit genau so vielen Kernen
#   mag.sh voll origin/work                                         Konvertierung + Spiel-Suite + Engine-Suite (nur der Koordinator)
#   mag.sh vergleich origin/work-<paket> src/a.cpp src/b.cpp        Probe UND Vergleich Bit für Bit der Tests dieser Quelltexte (Stellvertreter und Engine selbst)
#   mag.sh abbrechen <name> [tests]                                 den EIGENEN Auftrag beenden (er schreibt sein Protokoll und räumt auf); "tests": nur Hänger
#   mag.sh alle origin/mag/ergebnis-<voller-lauf>                   jeder Vergleichstest einzeln (Stellvertreter und Engine selbst): ALLE Abweichungen in einem Lauf
# Der Stand muss gepusht sein. Wartet, bis mag fertig ist, und legt die Protokolle nach $S/mag/<name>/ (when, *.tail.txt, *.log.xz, texts/).
# Gibt when und das Ende jedes Protokolls aus. Ein Auftrag dauert: Probe wenige Minuten, Suiten 10-30 Minuten.
#   MAG_NOWAIT=1 mag.sh ...     nur ablegen, den Namen ausgeben, nicht warten
#   mag.sh holen <name>         die Protokolle eines (auch fremden/früheren) Auftrags nach $S/mag/<name>/ holen, falls er fertig ist
#   mag.sh warten <name> [min]  bis zu min Minuten (Vorgabe 9) auf diesen Auftrag warten; 0 = fertig und geholt, 1 = läuft noch (dann noch einmal rufen)
#   mag.sh stand                welche Aufträge laufen, warten, fertig sind (eine Zeile je Auftrag)
# Fassung 18.11., 17:40 (neue Sitzung): das Ergebnis wird unter seinem eigenen Namen geholt (nicht über FETCH_HEAD – mehrere
# Aufrufe zugleich im selben Klon konnten sich das Ergebnis gegenseitig vertauschen); Ablegen mit Wiederholung (der Git-Zugang
# der Sitzung lässt nur 2 Übertragungen zugleich zu: HTTP 429 heißt 5-10 s warten und noch einmal, nicht aufgeben).
S=/home/claude/kernengine.de/koordination
K=/home/claude/kw
holen() {  # name -> 0, wenn die Protokolle da sind
  local name="$1" ref="refs/remotes/origin/mag/ergebnis-$1"
  git -C "$K" fetch -q origin "+refs/heads/mag/ergebnis-$name:$ref" 2>/dev/null || return 1
  # (fertig: mag-log ist zweimal geschrieben, und when gehört diesem Auftrag und nennt ein Ergebnis - ein Auftrag, dessen Stand selbst ein
  # Ergebnis-Zweig ist ("alle"), bringt dessen mag-log mit)
  local w; w="$(git -C "$K" show "$ref:mag-log/when" 2>/dev/null)"
  if [ "$(git -C "$K" rev-list --count "$ref" -- mag-log 2>/dev/null)" -lt 2 ] || ! echo "$w" | head -1 | grep -q "^$name:" || ! echo "$w" | grep -q ' rc='; then
    # (ein voller Lauf pusht Level und Tests gleich nach der Konvertierung: mag-zwischen/when, convert.tail.txt, convert.log.xz - die Suiten rechnen dann noch)
    if git -C "$K" cat-file -e "$ref:mag-zwischen/when" 2>/dev/null; then
      mkdir -p "$S/mag/$name/zwischen"; git -C "$K" archive "$ref" mag-zwischen | tar -x -C "$S/mag/$name/zwischen" --strip-components=1
      echo "Zwischenstand von $name (Konvertierung fertig, Suiten rechnen noch) in $S/mag/$name/zwischen:"; cat "$S/mag/$name/zwischen/when"
    fi
    return 1
  fi
  mkdir -p "$S/mag/$name"; git -C "$K" archive "$ref" mag-log | tar -x -C "$S/mag/$name" --strip-components=1
  echo "fertig $(date +%H:%M:%S): $S/mag/$name   (Zweig origin/mag/ergebnis-$name)"; cat "$S/mag/$name/when"
  for t in "$S/mag/$name"/*.tail.txt; do [ -f "$t" ] && { echo "--- $(basename "$t")"; tail -25 "$t"; }; done
  return 0
}
if [ "$1" = holen ]; then holen "$2" && exit 0; echo "$2: noch nicht fertig (oder kein solcher Auftrag)"; exit 1; fi
if [ "$1" = warten ]; then
  end=$(( $(date +%s) + ${3:-9} * 60 ))
  while :; do holen "$2" && exit 0; [ "$(date +%s)" -ge "$end" ] && break; sleep $(( 25 + RANDOM % 11 )); done
  echo "$2: läuft noch ($(date +%H:%M:%S)) – noch einmal: $0 warten $2"; exit 1
fi
if [ "$1" = stand ]; then
  git -C "$K" fetch -q origin '+refs/heads/mag/*:refs/remotes/origin/mag/*' 2>/dev/null
  for n in $(git -C "$K" ls-tree --name-only origin/mag/auftrag jobs/ | sed 's#jobs/##' | grep -v gitkeep); do
    r="refs/remotes/origin/mag/ergebnis-$n"
    if git -C "$K" rev-parse -q --verify "$r" >/dev/null; then
      w="$(git -C "$K" show "$r:mag-log/when" 2>/dev/null)"
      if echo "$w" | grep -q ' rc='; then echo "fertig  $n  $(echo "$w" | grep -oE '[a-z]+ rc=[0-9]+ [0-9:]+' | tr '\n' ' ')"
      else echo "läuft   $n  $(echo "$w" | head -1 | sed -E 's/^[^:]*: //' | cut -c1-90)"; fi
    else echo "wartet  $n  ($(git -C "$K" show "origin/mag/auftrag:jobs/$n" | head -1 | cut -c1-70))"; fi
  done; exit 0
fi
if [ "${1:-}" = abbrechen ] && [ $# -ge 2 ]; then   # abbrechen <name> [tests]: den eigenen Auftrag beenden ("tests": nur Testläufe über 20 Minuten)
  ( flock 9; cd "$S/magjob" || exit 1
    for try in 1 2 3 4 5 6; do
      git pull -q --rebase origin mag/auftrag 2>/dev/null
      mkdir -p abbruch; echo "${3:-alles}" > "abbruch/$2"; git add "abbruch/$2"; git commit -q -m "Abbruch $2 (${3:-alles})" 2>/dev/null
      git push -q origin mag/auftrag 2>/dev/null && exit 0; sleep $(( 5 + RANDOM % 6 ))
    done; exit 1 ) 9> "$S/magjob.lock" && echo "Abbruch von $2 abgelegt (${3:-alles}) – der Auftrag schreibt sein Protokoll und endet" || echo "Abbruch nicht abgelegt"
  exit 0
fi
[ $# -ge 2 ] || { sed -n '2,14p' "$0"; exit 2; }
name="$(echo "$2" | sed 's|.*/||; s/[^A-Za-z0-9-]/-/g')-$1-$(date +%H%M%S)-$$"
(
  flock 9
  cd "$S/magjob" || exit 1
  for try in 1 2 3 4 5 6; do
    git pull -q --rebase origin mag/auftrag 2>/dev/null
    if [ ! -e "jobs/$name" ]; then echo "$*" > "jobs/$name" && git add "jobs/$name" && git commit -q -m "Auftrag $name" || exit 1; fi
    git push -q origin mag/auftrag 2>/dev/null && exit 0
    sleep $(( 5 + RANDOM % 6 ))
  done
  exit 1
) 9> "$S/magjob.lock" || { echo "Auftrag nicht abgelegt (Push auf mag/auftrag sechsmal gescheitert)"; exit 1; }
echo "Auftrag $name abgelegt $(date +%H:%M:%S)"
[ -n "${MAG_NOWAIT:-}" ] && exit 0
for i in $(seq 1 240); do
  sleep $(( 25 + RANDOM % 11 ))
  holen "$name" && exit 0
done
echo "kein Ergebnis nach 2 Stunden – später: $0 holen $name"; exit 1
