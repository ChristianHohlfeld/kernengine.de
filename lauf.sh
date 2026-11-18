#!/usr/bin/env bash
# SpawnCity-Schlusslauf auf mag, gesteuert über GitHub (Branch mag/auftrag, Datei "ref").
#   einmal starten:  nohup bash ~/spawncity-auftrag/lauf.sh > ~/spawncity-auftrag.log 2>&1 &
#   beenden:         touch ~/spawncity-auftrag/STOP      (oder den Prozess beenden)
# Die Schleife schaut jede Minute nach. Steht in "ref" eine neue Zeile "<nummer> <git-ref>", rechnet sie
# diesen Stand (Konvertierung, Spiel-Suite, Engine-Suite) in ~/spawncity-lauf und pusht das Ergebnis auf
# den Branch mag/schlusslauf-<nummer>: Commit 1 = Level und Tests, Commit 2 = Protokolle (mag-log/).
# Sie pusht nie auf main oder work, erzwingt nichts, installiert nichts und räumt ~/spawncity-lauf danach weg.
# Kerne: alle (nproc), oder CORES=42-55 JOBS=14 in der Umgebung, um sie zu begrenzen.
set -u
A="$(cd "$(dirname "$0")" && pwd)"; W=~/spawncity-lauf; REPO=https://github.com/ChristianHohlfeld/kernengine.de
JOBS="${JOBS:-$(nproc)}"; R=""; [ -n "${CORES:-}" ] && R="taskset -c $CORES"
last="$(cat "$A/.done" 2>/dev/null || true)"
while [ ! -e "$A/STOP" ]; do
  git -C "$A" pull -q --ff-only origin mag/auftrag 2>/dev/null || true
  line="$(head -1 "$A/ref" 2>/dev/null || true)"; nr="${line%% *}"; ref="${line#* }"
  if [ -z "$nr" ] || [ "$nr" = "$last" ]; then sleep 60; continue; fi
  echo "$(date '+%F %T') Lauf $nr auf $ref, $JOBS Kerne"
  rm -rf "$W"; git clone -q "$REPO" "$W" && cd "$W" && git checkout -q --detach "$ref" || { echo "Stand $ref nicht gefunden"; last="$nr"; echo "$nr" > "$A/.done"; continue; }
  SHA="$(git rev-parse --short HEAD)"
  export KERN_JOBS="$JOBS" TMPDIR="$W/tmp" SC_TEST_OUT="$W/tmp/sc-tests"; mkdir -p "$TMPDIR" "$W/mag-log"
  echo "Lauf $nr, Stand $SHA ($ref), $JOBS Kerne, Start $(date '+%F %T')" > mag-log/when
  cd kern/games/spawncity && rm -rf tests/converted
  ( time FUNCTIONS=1 $R python3 ../../../tools/cpp_to_blueprint.py --convert sc --program firmware=fc,hwcontract \
      --level level/spawncity.klevel --test tests/converted -I../../include -I../../third_party/box3d/include -Isrc -- \
      $(ls src/*.cpp | grep -v "level_data.cpp\|level_blueprints.cpp\|main.cpp") ) > "$W/mag-log/convert.log" 2>&1
  echo "convert rc=$? $(date '+%T')" >> "$W/mag-log/when"; cd "$W"
  ( time $R kern/games/spawncity/tests/run_tests.sh ) > mag-log/suite.log 2>&1;  echo "suite rc=$? $(date '+%T')" >> mag-log/when
  ( time $R kern/tests/run.sh ) > mag-log/engine.log 2>&1;                       echo "engine rc=$? $(date '+%T')" >> mag-log/when
  for f in convert suite engine; do tail -60 mag-log/$f.log > mag-log/$f.tail.txt; done
  git checkout -q -B "mag/schlusslauf-$nr"
  git add kern/games/spawncity/level/spawncity.klevel kern/games/spawncity/tests/converted
  git -c user.name=mag -c user.email=mag@localhost commit -q -m "mag: Schlusslauf $nr auf $SHA – Level und Tests der Blueprints" || true
  xz -9 -T0 mag-log/*.log; git add -f mag-log
  git -c user.name=mag -c user.email=mag@localhost commit -q -m "mag: Protokolle des Schlusslaufs $nr auf $SHA"
  git push -q origin "mag/schlusslauf-$nr" && echo "$(date '+%F %T') Lauf $nr gepusht" || echo "$(date '+%F %T') Push von Lauf $nr fehlgeschlagen"
  cd "$A"; rm -rf "$W"; last="$nr"; echo "$nr" > "$A/.done"
done
echo "$(date '+%F %T') beendet (STOP)"
