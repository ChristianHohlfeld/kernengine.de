#!/usr/bin/env bash
# SpawnCity auf mag, gesteuert über GitHub (Branch mag/auftrag) – Fassung 2: eine Warteschlange.
#   starten:  touch ~/spawncity-auftrag/STOP; sleep 90; rm ~/spawncity-auftrag/STOP
#             nohup bash ~/spawncity-auftrag/lauf2.sh > ~/spawncity-auftrag2.log 2>&1 &
#   beenden:  touch ~/spawncity-auftrag/STOP
# Jede Datei jobs/<name> ist ein Auftrag, erste Zeile:
#   voll <git-ref>                      Konvertierung des ganzen Spiels, Spiel-Suite, Engine-Suite
#   probe <git-ref> <quelle> [...]      Konverter nur über diese Quelltexte (relativ zu kern/games/spawncity)
#   konvert <git-ref> <kerne>          nur die Konvertierung des ganzen Spiels mit genau so vielen Kernen (Ergebnis wie voll, Commit 1)
#   engine <git-ref> [filter]           nur die Engine-Suite
#   spiel <git-ref> [filter]            nur die Spiel-Suite (ohne neue Konvertierung)
# Ergebnis: Branch mag/ergebnis-<name> (bei "voll": Commit 1 = Level und Tests, Commit 2 = Protokolle in mag-log/;
# sonst nur die Protokolle). Beim Start jedes Auftrags erscheint mag/ergebnis-<name> schon mit mag-log/when.
# Bis zu $SLOTS Aufträge laufen zugleich (ein "voll" allein mit allen Kernen). Pusht nie auf main/work, erzwingt
# nichts, installiert nichts; jeder Auftrag rechnet im Arbeitsspeicher (/dev/shm/spawncity-jobs-<nutzer>/<name>, sonst ~/spawncity-jobs/<name>) und räumt den Ordner danach weg.
# Die Schleife ersetzt sich selbst, wenn diese Datei auf dem Branch neuer ist.
# Fassung 18.11., 18:10: (1) "probe" gibt dem Konverter eine Kopie des Levels mit (--level): ohne sie wartete jede Funktion, die den Blueprint
# eines Regel-Actors ruft, mit "... which the level does not have" - ein Artefakt der Probe. (2) "voll": die Engine-Suite rechnet von Anfang an
# neben der Konvertierung (sie prüft kern/, nicht das Level), und Level und Tests werden gleich nach der Konvertierung gepusht (Commit "... Level
# und Tests der Blueprints", dazu mag-zwischen/ mit when und convert.log) - die Suiten folgen; mag-log/ kommt wie bisher am Ende.
set -u
A="$(cd "$(dirname "$0")" && pwd)"; J=~/spawncity-jobs
# Im Arbeitsspeicher rechnen, wo es geht (/dev/shm: Klon, Zwischendateien, Objekte - nichts davon geht über die Platte).
[ -d /dev/shm ] && [ -w /dev/shm ] && J="/dev/shm/spawncity-jobs-$(id -un)"; REPO=https://github.com/ChristianHohlfeld/kernengine.de
N="$(nproc)"; SLOTS="${SLOTS:-14}"   # (Aufträge zugleich; jeder rechnet mit N/6 Kernen - die meisten nutzen wenige); mkdir -p "$J" "$A/.done"
me="$(sha256sum "$0" | cut -c1-16)"
# Eine Schleife zur Zeit: wer die Sperre nicht bekommt, hört auf (eine zweite, die gestartet wird, während die erste
# in einem alten vollen Lauf steckt, übernimmt; die erste endet, sobald sie sich ersetzen will).
exec 8> "$A/.schleife.lock"; flock -n 8 || { echo "$(date '+%F %T') eine andere Schleife läuft schon – diese endet"; exit 0; }
G="git -c user.name=mag -c user.email=mag@localhost"
# clang++: der Konverter liest die Quelltexte mit clang (CLANGXX). Gesucht wird, was da ist - nichts wird installiert.
find_clang() {
  local c
  for c in "${CLANGXX:-}" clang++ clang++-20 clang++-19 clang++-18 clang++-17 clang++-16 /usr/lib/llvm-*/bin/clang++ "$HOME"/.local/bin/clang++ \
           "$HOME"/*/bin/clang++ "$HOME"/*/*/bin/clang++ /opt/*/bin/clang++ /usr/local/*/bin/clang++; do
    [ -n "$c" ] && command -v "$c" >/dev/null 2>&1 && { command -v "$c"; return; }
  done
}
CLANGXX="$(find_clang)"; export CLANGXX
run_job() {  # name
  local name="$1" kind ref rest W jobs
  read -r kind ref rest < "$A/jobs/$name"
  W="$J/$name"; rm -rf "$W"
  # Ein Spiegel des Repos im Arbeitsspeicher: jeder Auftrag holt nur das Neue und klont lokal (Sekunden statt Minuten).
  ( flock 7; [ -d "$J/.spiegel.git" ] || git clone -q --mirror "$REPO" "$J/.spiegel.git"; git -C "$J/.spiegel.git" fetch -q --prune origin ) 7> "$J/.spiegel.lock"
  git clone -q "$J/.spiegel.git" "$W" 2>/dev/null && cd "$W" && git remote set-url origin "$REPO" && git checkout -q --detach "$ref" 2>/dev/null || { echo "$name: Stand $ref nicht gefunden"; return; }
  local SHA; SHA="$(git rev-parse --short HEAD)"
  jobs=$(( N / 6 )); [ "$kind" = voll ] && jobs=$(( N * 2 / 3 ))
  export KERN_JOBS="$jobs" TMPDIR="$W/tmp" SC_TEST_OUT="$W/tmp/sc-tests"; mkdir -p "$TMPDIR" mag-log
  echo "$name: $kind auf $SHA ($ref), $jobs Kerne, Start $(date '+%F %T')" > mag-log/when
  echo "werkzeuge: clang=${CLANGXX:-FEHLT} ($("${CLANGXX:-false}" --version 2>/dev/null | head -1)) c++=$(command -v c++ || echo FEHLT) ($(c++ --version 2>/dev/null | head -1)) python=$(python3 --version 2>&1) PATH=$PATH" >> mag-log/when
  git checkout -q -B "mag/ergebnis-$name"; git add -f mag-log; $G commit -q -m "mag: $name gestartet ($kind auf $SHA)"; git push -q origin "mag/ergebnis-$name" 2>/dev/null
  local conv="python3 ../../../tools/cpp_to_blueprint.py --convert sc --program firmware=fc,hwcontract -I../../include -I../../third_party/box3d/include -Isrc"
  case "$kind" in
    voll)
      # (die Engine-Suite hängt nicht an der Konvertierung: sie rechnet von Anfang an daneben, mit einem Drittel der Kerne, in ihrem eigenen Ordner $TMPDIR/kern-tests)
      ( ( time KERN_JOBS=$(( N / 3 )) kern/tests/run.sh ) > mag-log/engine.log 2>&1; echo "engine rc=$? $(date '+%T')" > "$W/tmp/engine.when" ) &
      local engine_pid=$!
      # (die Texte der Blueprints gehen mit nach mag-zwischen/texts: scheitert das Einlesen ins Level, lässt sich jeder Text einzeln nachlesen)
      ( cd kern/games/spawncity && rm -rf tests/converted && time FUNCTIONS=1 $conv --level level/spawncity.klevel --test tests/converted --texts "$W/mag-zwischen/texts" -- \
          $(cd kern/games/spawncity 2>/dev/null; ls src/*.cpp | grep -v "level_data.cpp\|level_blueprints.cpp\|main.cpp") ) > mag-log/convert.log 2>&1
      echo "convert rc=$? $(date '+%T')" >> mag-log/when
      # (Level und Tests sofort, mit dem Protokoll der Konvertierung in mag-zwischen/ - nicht in mag-log/: ein Auftrag gilt als fertig, sobald mag-log zweimal geschrieben ist)
      git add kern/games/spawncity/level/spawncity.klevel kern/games/spawncity/tests/converted
      $G commit -q -m "mag: $name auf $SHA – Level und Tests der Blueprints" || true
      mkdir -p mag-zwischen && cp mag-log/when mag-zwischen/when && tail -80 mag-log/convert.log > mag-zwischen/convert.tail.txt && xz -9 -T0 -c mag-log/convert.log > mag-zwischen/convert.log.xz
      git add -f mag-zwischen && $G commit -q -m "mag: $name auf $SHA – Zwischenstand nach der Konvertierung (die Suiten rechnen noch)" && git push -q origin "mag/ergebnis-$name" 2>/dev/null
      ( time kern/games/spawncity/tests/run_tests.sh ) > mag-log/suite.log 2>&1; echo "suite rc=$? $(date '+%T')" >> mag-log/when
      wait "$engine_pid"; cat "$W/tmp/engine.when" >> mag-log/when 2>/dev/null || echo "engine rc=? (kein Ergebnis) $(date '+%T')" >> mag-log/when ;;
    konvert)   # konvert <git-ref> <kerne>: nur die Konvertierung des ganzen Spiels, mit genau so vielen Kernen
      export KERN_JOBS="${rest%% *}"; echo "kerne: $KERN_JOBS" >> mag-log/when
      ( cd kern/games/spawncity && rm -rf tests/converted && time FUNCTIONS=1 $conv --level level/spawncity.klevel --test tests/converted -- \
          $(ls src/*.cpp | grep -v "level_data.cpp\|level_blueprints.cpp\|main.cpp") ) > mag-log/convert.log 2>&1
      echo "convert rc=$? $(date '+%T')" >> mag-log/when
      git add kern/games/spawncity/level/spawncity.klevel kern/games/spawncity/tests/converted
      $G commit -q -m "mag: $name auf $SHA – Level und Tests der Blueprints ($KERN_JOBS Kerne)" || true ;;
    probe)
      mkdir -p "$W/pt/converted" && cp kern/games/spawncity/tests/*.h "$W/pt/"   # (der Test-Schreiber sucht <test>/../converted.h)
      cp kern/games/spawncity/level/spawncity.klevel "$W/pt/l.klevel"            # (eine Kopie des Levels: die Regel-Blueprints, die das Programm ruft, stehen dort)
      ( cd kern/games/spawncity && time FUNCTIONS=1 TEST_ERRORS=1 $conv --level "$W/pt/l.klevel" --test "$W/pt/converted" --texts "$W/mag-log/texts" -- $rest ) > mag-log/convert.log 2>&1
      echo "convert rc=$? $(date '+%T')" >> mag-log/when ;;
    engine) ( time kern/tests/run.sh $rest ) > mag-log/engine.log 2>&1; echo "engine rc=$? $(date '+%T')" >> mag-log/when ;;
    spiel)  ( time kern/games/spawncity/tests/run_tests.sh $rest ) > mag-log/suite.log 2>&1; echo "suite rc=$? $(date '+%T')" >> mag-log/when ;;
    *) echo "unbekannte Art $kind" >> mag-log/when ;;
  esac
  for f in mag-log/*.log; do [ -f "$f" ] && tail -80 "$f" > "${f%.log}.tail.txt"; done
  xz -9 -T0 mag-log/*.log 2>/dev/null; git add -f mag-log
  $G commit -q -m "mag: Protokolle von $name auf $SHA"; git push -q origin "mag/ergebnis-$name" || echo "$name: Push fehlgeschlagen"
  cd "$A"; rm -rf "$W"; echo "$(date '+%F %T') $name fertig"
}
while [ ! -e "$A/STOP" ]; do
  git -C "$A" pull -q --ff-only origin mag/auftrag 2>/dev/null || true
  if [ "$(sha256sum "$A/lauf2.sh" | cut -c1-16)" != "$me" ]; then   # (neue Fassung: nichts Neues annehmen, auslaufen lassen, ersetzen)
    echo "$(date '+%F %T') neue Fassung ($(jobs -rp | wc -l) Aufträge laufen weiter)"; exec bash "$A/lauf2.sh"; fi   # (die laufenden halten die Sperre nicht)
  for f in "$A"/jobs/*; do
    [ -f "$f" ] || continue; name="$(basename "$f")"; [ -e "$A/.done/$name" ] && continue
    read -r kind _ < "$f"
    running="$(jobs -rp | wc -l)"
    [ "$running" -lt "$SLOTS" ] || break
    touch "$A/.done/$name"; echo "$(date '+%F %T') $name: $(head -1 "$f")"
    run_job "$name" 8>&- &   # (auch ein voller Lauf hält die Warteschlange nicht auf: die Proben laufen daneben)
  done
  sleep 20
done
wait; echo "$(date '+%F %T') beendet (STOP)"
