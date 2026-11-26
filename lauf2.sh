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
#   vergleich <git-ref> <quellen…>      wie probe, und danach der Vergleich Bit für Bit: die Tests dieser Quelltexte werden gebaut und laufen jeder für sich,
#                                       gegen den Stellvertreter und gegen die Engine selbst (Level: das des letzten gültigen vollen Laufs, Datei "grundlage").
#                                       Ergebnis wie bei "alle" (mag-log/alle.txt …) und convert.log, texts/. Beleg für diese Einheiten - der Beweis bleibt "voll".
#   alle <git-ref>                      die Vergleichstests der Spiel-Suite einzeln: jeder Test tests/converted/test_<einheit>.cpp in seinem eigenen
#                                       Lauf, einmal gegen den Stellvertreter und einmal gegen die Engine selbst (SC_ENGINE=real) - ein Abbruch oder
#                                       Fehlschlag hält die anderen nicht auf. <git-ref> hat Level und Tests (z. B. origin/mag/ergebnis-<voller lauf>).
#                                       Ergebnis: mag-log/alle.txt (je Test ok/FAIL/ABBRUCH), alle-real.txt, einzeln.tar.xz (jede Ausgabe)
# Ergebnis: Branch mag/ergebnis-<name> (bei "voll": Commit 1 = Level und Tests, Commit 2 = Protokolle in mag-log/;
# sonst nur die Protokolle). Beim Start jedes Auftrags erscheint mag/ergebnis-<name> schon mit mag-log/when.
# Aufträge laufen zugleich, solange sie zusammen ins Budget an Kernen passen (CAP; volle Läufe zuerst, dann vergleich/alle, dann der Rest).
# Abbrechen: eine Datei abbruch/<name> auf dem Zweig (mag.sh abbrechen <name>) - was der Auftrag rechnen lässt, endet; er schreibt sein Protokoll und räumt auf. Pusht nie auf main/work, erzwingt
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
# >>> warteschlange (diese Funktionen prüft /tmp-Probe des Koordinators einzeln, ehe eine neue Fassung auf den Zweig geht)
mkdir -p "$J" "$A/.done" "$A/.lauf"
CAP=$(( N * 5 / 4 ))   # (Kerne, die Aufträge zusammen belegen dürfen: ein voller Lauf und ein Vergleich daneben - mehr macht alle langsam)
need() { case "$1" in voll) echo "$N" ;; alle) echo $(( N * 2 / 3 )) ;; vergleich) echo $(( N / 4 )) ;; *) echo $(( N / 6 )) ;; esac; }
# Kerne, die laufende Aufträge belegen. Jeder angenommene Auftrag steht in .lauf/<name> mit seinem Bedarf und, sobald er läuft, der Nummer seiner Schale
# (ersetzt sich die Schleife, laufen ihre Aufträge weiter - die Liste der Schale kennt sie dann nicht mehr, diese Dateien schon). Ein Auftrag einer älteren
# Fassung steht dort nicht: er zählt, solange ein Prozess in seinem Ordner arbeitet.
used() {
  local s=0 f name k pid d kind cw
  cw="$(ls -l /proc/[0-9]*/cwd 2>/dev/null | sed 's/^.* -> //')"
  for f in "$A"/.lauf/*; do
    [ -f "$f" ] || continue
    k=0; pid=""; read -r k pid < "$f" || true
    if [ -n "$pid" ] && ! kill -0 "$pid" 2>/dev/null; then rm -f "$f"; continue; fi                    # (zu Ende, ohne aufzuräumen)
    if [ -z "$pid" ] && [ -n "$(find "$f" -mmin +5 2>/dev/null)" ]; then rm -f "$f"; continue; fi     # (angenommen und nie gestartet)
    s=$(( s + k ))
  done
  for d in "$J"/*/; do
    [ -d "$d" ] || continue; d="${d%/}"; name="$(basename "$d")"; [ -e "$A/.lauf/$name" ] && continue
    printf '%s\n' "$cw" | awk -v d="$d" '$0 == d || index($0, d "/") == 1 { f = 1 } END { exit !f }' || continue
    kind=probe; [ -f "$A/jobs/$name" ] && { read -r kind _ < "$A/jobs/$name" || true; }
    s=$(( s + $(need "$kind") ))
  done
  echo "$s"
}
# Abbruch (Datei abbruch/<name> auf dem Zweig): beendet, was der Auftrag rechnen lässt - Konverter, Compiler, Tests: jeden Prozess, der in seinem Ordner
# arbeitet. Die Schale des Auftrags selbst bleibt (und git, xz, tar): sie schreibt ihr Protokoll, pusht es und räumt den Ordner weg. Solange der Ordner steht,
# wiederholt die Schleife das alle 20 Sekunden - so endet auch, was der Auftrag danach noch beginnt.
cancel_job() {
  local name="$1" d pid ppid comm main="" x="" et mode
  case "$name" in ""|.*|*[!A-Za-z0-9._-]*) return 0 ;; esac
  d="$J/$name"; [ -n "$J" ] && [ -d "$d" ] || return 0
  # (steht in der Datei "tests": nur die Hänger - Testläufe dieses Auftrags, die länger als ${HANG:-1200} Sekunden laufen; der Auftrag selbst rechnet weiter)
  mode="$(head -1 "$A/abbruch/$name" 2>/dev/null)"
  if [ "$mode" = tests ]; then
    ls -l /proc/[0-9]*/cwd 2>/dev/null | awk -v d="$d" '{ i = index($0, " -> "); c = substr($0, i + 4); if (c == d || index(c, d "/") == 1) { n = split(substr($0, 1, i - 1), a, "/"); print a[n - 1] } }' | while read -r pid; do
      et=""; comm=""; read -r et comm < <(ps -o etimes=,comm= -p "$pid" 2>/dev/null) || true
      [ "$comm" = spawncity_tests ] && [ -n "$et" ] && [ "$et" -ge "${HANG:-1200}" ] && { kill "$pid" 2>/dev/null; echo "$(date '+%F %T') $name: Testlauf $pid nach $et s beendet"; }
    done
    return 0
  fi
  if [ ! -e "$d/.abbruch" ]; then touch "$d/.abbruch"; echo "abgebrochen $(date '+%T') (abbruch/$name)" >> "$d/mag-log/when" 2>/dev/null; echo "$(date '+%F %T') $name: Abbruch"; fi
  [ -f "$A/.lauf/$name" ] && { read -r x main < "$A/.lauf/$name" || true; }
  ls -l /proc/[0-9]*/cwd 2>/dev/null | awk -v d="$d" '{ i = index($0, " -> "); c = substr($0, i + 4); if (c == d || index(c, d "/") == 1) { n = split(substr($0, 1, i - 1), a, "/"); print a[n - 1] } }' | while read -r pid; do
    [ "$pid" = "$main" ] && continue
    ppid=""; comm=""; read -r ppid comm < <(ps -o ppid=,comm= -p "$pid" 2>/dev/null) || true
    [ -n "$ppid" ] || continue
    [ "$ppid" = "$$" ] && continue      # (die Schale des Auftrags, auch eines Auftrags einer älteren Fassung)
    case "$comm" in git|git-remote-http*|xz|tar|tail|ssh|rm|cp) continue ;; esac
    kill "$pid" 2>/dev/null
  done
  return 0
}
# Annahme: volle Läufe zuerst, dann "vergleich" und "alle", dann der Rest - je in der Reihenfolge des Eintreffens. Angenommen wird nur, was ins Budget an
# Kernen passt; der erste, der nicht passt, hält die nach ihm auf - so verhungert kein großer Auftrag hinter vielen kleinen.
annehmen() {
  local f name kind k u prio cap
  for f in "$A"/abbruch/*; do [ -f "$f" ] && cancel_job "$(basename "$f")"; done
  # (das Budget lässt sich über die Datei "cap" auf dem Zweig stellen, ohne die Schleife zu ersetzen - eine Zahl; fehlt sie, gilt CAP)
  cap=""; [ -f "$A/cap" ] && cap="$(head -1 "$A/cap" | tr -cd '0-9')"; [ -n "$cap" ] || cap="$CAP"
  u="$(used)"
  for prio in voll "vergleich alle" rest; do
    for name in $(ls -tr "$A/jobs" 2>/dev/null); do
      f="$A/jobs/$name"; [ -f "$f" ] || continue; [ -e "$A/.done/$name" ] && continue
      [ -e "$A/abbruch/$name" ] && { touch "$A/.done/$name"; continue; }      # (abgebrochen, ehe er begann)
      kind=""; read -r kind _ < "$f" || true; [ -n "$kind" ] || continue
      case "$prio" in
        rest) case " voll vergleich alle " in *" $kind "*) continue ;; esac ;;
        *) case " $prio " in *" $kind "*) ;; *) continue ;; esac ;;
      esac
      k="$(need "$kind")"
      if [ "$u" -gt 0 ] && [ $(( u + k )) -gt "$cap" ]; then return 0; fi
      touch "$A/.done/$name"; echo "$k" > "$A/.lauf/$name"; u=$(( u + k ))
      echo "$(date '+%F %T') $name: $(head -1 "$f") ($u von $cap Kernen belegt)"
      run_job "$name" 8>&- &
    done
  done
  return 0
}
# <<< warteschlange
run_job() {  # name
  local name="$1" kind ref rest W jobs
  read -r kind ref rest < "$A/jobs/$name"
  echo "$(need "$kind") $BASHPID" > "$A/.lauf/$name"
  W="$J/$name"; rm -rf "$W"
  # Ein Spiegel des Repos im Arbeitsspeicher: jeder Auftrag holt nur das Neue und klont lokal (Sekunden statt Minuten).
  ( flock 7; [ -d "$J/.spiegel.git" ] || git clone -q --mirror "$REPO" "$J/.spiegel.git"; git -C "$J/.spiegel.git" fetch -q --prune origin ) 7> "$J/.spiegel.lock"
  git clone -q "$J/.spiegel.git" "$W" 2>/dev/null && cd "$W" && git remote set-url origin "$REPO" && git checkout -q --detach "$ref" 2>/dev/null || { echo "$name: Stand $ref nicht gefunden"; cd "$A"; rm -rf "$W"; rm -f "$A/.lauf/$name"; return; }
  local SHA; SHA="$(git rev-parse --short HEAD)"
  jobs=$(( N / 6 )); [ "$kind" = voll ] && jobs=$(( N * 2 / 3 )); [ "$kind" = alle ] && jobs=$(( N * 2 / 3 )); [ "$kind" = vergleich ] && jobs=$(( N / 4 ))
  export KERN_JOBS="$jobs" TMPDIR="$W/tmp" SC_TEST_OUT="$W/tmp/sc-tests"; mkdir -p "$TMPDIR" mag-log
  echo "$name: $kind auf $SHA ($ref), $jobs Kerne, Start $(date '+%F %T')" > mag-log/when
  echo "werkzeuge: clang=${CLANGXX:-FEHLT} ($("${CLANGXX:-false}" --version 2>/dev/null | head -1)) c++=$(command -v c++ || echo FEHLT) ($(c++ --version 2>/dev/null | head -1)) python=$(python3 --version 2>&1) PATH=$PATH" >> mag-log/when
  git checkout -q -B "mag/ergebnis-$name"; git add -f mag-log; $G commit -q -m "mag: $name gestartet ($kind auf $SHA)"; git push -q origin "mag/ergebnis-$name" 2>/dev/null
  local conv="python3 ../../../tools/cpp_to_blueprint.py --convert sc --program firmware=fc,hwcontract -I../../include -I../../third_party/box3d/include -Isrc"
  # (ab dem Projekt über den Levels, 26.11.: der Konverter schreibt Programm-Blueprints ins Projekt und hat keine Vorgaben eines Spiels mehr)
  local proj=""
  if [ -f kern/games/spawncity/level/spawncity.kproject ]; then
    conv="$conv --project level/spawncity.kproject --folder Source --classes level_blueprints.h --whole-program"; proj="kern/games/spawncity/level/spawncity.kproject"
  fi
  case "$kind" in
    voll)
      # (die Engine-Suite hängt nicht an der Konvertierung: sie rechnet von Anfang an daneben, mit einem Drittel der Kerne, in ihrem eigenen Ordner $TMPDIR/kern-tests)
      ( ( time KERN_JOBS=$(( N / 3 )) kern/tests/run.sh ) > mag-log/engine.log 2>&1; echo "engine rc=$? $(date '+%T')" > "$W/tmp/engine.when" ) &
      local engine_pid=$!
      # (die Texte der Blueprints gehen mit nach mag-zwischen/texts: scheitert das Einlesen ins Level, lässt sich jeder Text einzeln nachlesen)
      ( cd kern/games/spawncity && rm -rf tests/converted && time FUNCTIONS=1 TEST_ERRORS=1 $conv --level level/spawncity.klevel --test tests/converted --texts "$W/mag-zwischen/texts" -- \
          $(cd kern/games/spawncity 2>/dev/null; ls src/*.cpp | grep -v "level_data.cpp\|level_blueprints.cpp") ) > mag-log/convert.log 2>&1
      echo "convert rc=$? $(date '+%T')" >> mag-log/when
      # (Level und Tests sofort, mit dem Protokoll der Konvertierung in mag-zwischen/ - nicht in mag-log/: ein Auftrag gilt als fertig, sobald mag-log zweimal geschrieben ist)
      git add kern/games/spawncity/level/spawncity.klevel kern/games/spawncity/tests/converted $proj
      $G commit -q -m "mag: $name auf $SHA – Level und Tests der Blueprints" || true
      mkdir -p mag-zwischen && cp mag-log/when mag-zwischen/when && tail -80 mag-log/convert.log > mag-zwischen/convert.tail.txt && xz -9 -T0 -c mag-log/convert.log > mag-zwischen/convert.log.xz
      git add -f mag-zwischen && $G commit -q -m "mag: $name auf $SHA – Zwischenstand nach der Konvertierung (die Suiten rechnen noch)" && git push -q origin "mag/ergebnis-$name" 2>/dev/null
      ( time kern/games/spawncity/tests/run_tests.sh ) > mag-log/suite.log 2>&1; echo "suite rc=$? $(date '+%T')" >> mag-log/when
      wait "$engine_pid"; cat "$W/tmp/engine.when" >> mag-log/when 2>/dev/null || echo "engine rc=? (kein Ergebnis) $(date '+%T')" >> mag-log/when ;;
    konvert)   # konvert <git-ref> <kerne>: nur die Konvertierung des ganzen Spiels, mit genau so vielen Kernen
      export KERN_JOBS="${rest%% *}"; echo "kerne: $KERN_JOBS" >> mag-log/when
      ( cd kern/games/spawncity && rm -rf tests/converted && time FUNCTIONS=1 $conv --level level/spawncity.klevel --test tests/converted -- \
          $(ls src/*.cpp | grep -v "level_data.cpp\|level_blueprints.cpp") ) > mag-log/convert.log 2>&1
      echo "convert rc=$? $(date '+%T')" >> mag-log/when
      git add kern/games/spawncity/level/spawncity.klevel kern/games/spawncity/tests/converted $proj
      $G commit -q -m "mag: $name auf $SHA – Level und Tests der Blueprints ($KERN_JOBS Kerne)" || true ;;
    probe)
      mkdir -p "$W/pt/converted" && cp kern/games/spawncity/tests/*.h "$W/pt/"   # (der Test-Schreiber sucht <test>/../converted.h)
      cp kern/games/spawncity/level/spawncity.klevel "$W/pt/l.klevel"            # (eine Kopie des Levels: die Regel-Blueprints, die das Programm ruft, stehen dort)
      ( cd kern/games/spawncity && time FUNCTIONS=1 TEST_ERRORS=1 $conv --level "$W/pt/l.klevel" --test "$W/pt/converted" --texts "$W/mag-log/texts" -- $rest ) > mag-log/convert.log 2>&1
      echo "convert rc=$? $(date '+%T')" >> mag-log/when ;;
    engine) ( time kern/tests/run.sh $rest ) > mag-log/engine.log 2>&1; echo "engine rc=$? $(date '+%T')" >> mag-log/when ;;
    alle)
      find mag-log -type f ! -name when -delete 2>/dev/null; rm -rf mag-zwischen   # (was der Stand selbst an Protokollen mitbringt, gehört nicht zu diesem Auftrag)
      einzeln ;;
    vergleich)   # vergleich <git-ref> <quellen…>: der Konverter über diese Quelltexte UND der Vergleich Bit für Bit ihrer Tests, jeder Test in seinem eigenen Lauf
      find mag-log -type f ! -name when -delete 2>/dev/null; rm -rf mag-zwischen
      # (Grundlage: das Level des letzten gültigen vollen Laufs - die Datei "grundlage" im Auftragszweig nennt seinen Zweig; dort stehen die Blueprints der
      # anderen Einheiten, wie der Konverter sie zuletzt schrieb. Die Tests dieses Auftrags sind allein die der genannten Quelltexte.)
      local base; base="$(cat "$A/grundlage" 2>/dev/null)"
      if [ -n "$base" ] && git fetch -q origin "$base" 2>/dev/null && git checkout -q FETCH_HEAD -- kern/games/spawncity/level/spawncity.klevel; then
        echo "grundlage: $base $(git rev-parse --short=8 FETCH_HEAD)" >> mag-log/when; else echo "grundlage: keine (das Level des Stands)" >> mag-log/when; fi
      ( cd kern/games/spawncity && rm -rf tests/converted && time FUNCTIONS=1 TEST_ERRORS=1 $conv --level level/spawncity.klevel --test tests/converted --texts "$W/mag-log/texts" -- $rest ) > mag-log/convert.log 2>&1
      echo "convert rc=$? $(date '+%T')" >> mag-log/when
      if ls kern/games/spawncity/tests/converted/test_*.cpp > /dev/null 2>&1; then einzeln; else echo "alle rc=2 $(date '+%T') (die Konvertierung schrieb keinen Test)" >> mag-log/when; fi ;;
    spiel)  ( time kern/games/spawncity/tests/run_tests.sh $rest ) > mag-log/suite.log 2>&1; echo "suite rc=$? $(date '+%T')" >> mag-log/when ;;
    *) echo "unbekannte Art $kind" >> mag-log/when ;;
  esac
  for f in mag-log/*.log; do [ -f "$f" ] && tail -80 "$f" > "${f%.log}.tail.txt"; done
  xz -9 -T0 mag-log/*.log 2>/dev/null; git add -f mag-log
  $G commit -q -m "mag: Protokolle von $name auf $SHA"; git push -q origin "mag/ergebnis-$name" || echo "$name: Push fehlgeschlagen"
  cd "$A"; rm -rf "$W"; rm -f "$A/.lauf/$name"; echo "$(date '+%F %T') $name fertig"
}
# (jeder Vergleichstest aus tests/converted in seinem eigenen Lauf - gegen den Stellvertreter und gegen die Engine selbst, höchstens 20 Minuten je Lauf; ein
# Abbruch, ein Fehlschlag oder ein Hänger hält die anderen nicht auf. Schreibt mag-log/alle.txt, alle-real.txt, *-abweichungen.txt, *-cost.txt, einzeln.tar.xz
# und die Ergebniszeile in mag-log/when. Liest $W, $SC_TEST_OUT, $KERN_JOBS des Auftrags.)
einzeln() {
  # (bauen: run_tests.sh mit einem Filter, auf den kein Test passt - das Programm steht danach in $SC_TEST_OUT)
  ( time kern/games/spawncity/tests/run_tests.sh __kein_test_heisst_so__ ) > mag-log/bau.log 2>&1; echo "bau rc=$? $(date '+%T')" >> mag-log/when
  if [ ! -x "$SC_TEST_OUT/spawncity_tests" ]; then echo "alle rc=2 $(date '+%T') (das Testprogramm ist nicht gebaut: bau.log)" >> mag-log/when; return; fi
  mkdir -p "$W/tmp/einzeln" "$W/tmp/einzeln-real"
  ( cd kern/games/spawncity/tests/converted && ls test_*.cpp | sed 's/^test_/converted_/; s/\.cpp$//' ) > "$W/tmp/namen.txt"
  export T="$SC_TEST_OUT/spawncity_tests" E="$W/tmp"
  # (lauf.sh <name>: ein Test in seinem eigenen Lauf, gegen den Stellvertreter und gegen die Engine selbst, je höchstens 20 Minuten. Die Ausgabe geht über ein
  # Terminal (script): so schreibt das Programm zeilenweise - endet es hart (LeakSanitizer endet mit _exit; ein Abbruch), fehlt am Ende nichts. Ohne das ging
  # bei jedem solchen Ende der letzte Puffer verloren: die FAIL-Zeile, die Summenzeile, Abweichungen.)
  cat > "$W/tmp/lauf.sh" <<'LAUF'
n="$1"
one() {  # one <ordner> [SC_ENGINE=real]
  local rc
  if command -v script > /dev/null 2>&1; then
    env $2 script -qec "timeout -k 10 ${HANG:-1200} '$T' --name '$n'" /dev/null < /dev/null 2>&1 | tr -d '\r' > "$E/$1/$n.txt"; rc=${PIPESTATUS[0]}
  else
    env $2 timeout -k 10 "${HANG:-1200}" "$T" --name "$n" > "$E/$1/$n.txt" 2>&1; rc=$?
  fi
  echo "$n rc=$rc" >> "$E/$1.rc"
}
one einzeln ""; one einzeln-real SC_ENGINE=real
LAUF
  # (der Filter des Programms ist ein Teil des Namens: "converted_car" trifft auch converted_car_cpp - der läuft dann zweimal, das Ergebnis ist dasselbe)
  xargs -P "$KERN_JOBS" -I{} bash "$W/tmp/lauf.sh" {} < "$W/tmp/namen.txt"
  local m o n rc bad=0
  for m in einzeln einzeln-real; do
    o=mag-log/alle.txt; [ "$m" = einzeln-real ] && o=mag-log/alle-real.txt
    : > "$W/tmp/$m.ab"
    # (zu Ende gelaufen ist ein Lauf, der seine Summenzeile "N tests, M checks" schreibt - ASan endet wie ein Fehlschlag mit 1)
    sort "$W/tmp/$m.rc" | while read -r n rc; do
      case "$rc" in
        rc=124|rc=137) echo "ZEIT $n: läuft länger als 20 Minuten" >> "$W/tmp/$m.ab" ;;
        *) if ! grep -qE '[0-9]+ tests?, [0-9]+ checks?' "$W/tmp/$m/$n.txt"; then
             echo "ABBRUCH $n ($rc): $(grep -hE 'ERROR: AddressSanitizer|runtime error|Assertion|terminate|Segmentation|SUMMARY' "$W/tmp/$m/$n.txt" | head -2 | tr '\n' ' ' | cut -c1-260)" >> "$W/tmp/$m.ab"
           elif grep -q 'ERROR: LeakSanitizer' "$W/tmp/$m/$n.txt"; then   # (zu Ende gelaufen, aber Speicher blieb liegen: der Lauf endet mit 1, die Suite wäre rot)
             echo "LECK $n: $(grep -h 'SUMMARY' "$W/tmp/$m/$n.txt" | head -1 | cut -c1-160)" >> "$W/tmp/$m.ab"
           fi ;;
      esac
    done
    { grep -hE '^(ok|FAIL) ' "$W/tmp/$m"/*.txt | awk '{print $1, $2}' | sort -u; cat "$W/tmp/$m.ab"
      echo "--- $(grep -hE '^ok ' "$W/tmp/$m"/*.txt | awk '{print $1, $2}' | sort -u | wc -l) ok, $(grep -hE '^FAIL ' "$W/tmp/$m"/*.txt | awk '{print $1, $2}' | sort -u | wc -l) FAIL, $(grep -c '^ABBRUCH' "$W/tmp/$m.ab") ABBRUCH, $(grep -c '^LECK' "$W/tmp/$m.ab") LECK, $(grep -c '^ZEIT' "$W/tmp/$m.ab") ZEIT, von $(wc -l < "$W/tmp/namen.txt") Tests; cost: $(grep -hE '^  cost:' "$W/tmp/$m"/*.txt | sort -u | wc -l), note: $(grep -hE '^  note:' "$W/tmp/$m"/*.txt | sort -u | wc -l)"
    } > "$o"
    grep -hE 'check failed|calls differ' "$W/tmp/$m"/*.txt | sed -E 's#^.*converted\.h:[0-9]+: ##' | sort -u > "${o%.txt}-abweichungen.txt"
    grep -hE '^  cost:' "$W/tmp/$m"/*.txt | sort -u > "${o%.txt}-cost.txt"
    bad=$(( bad + $(grep -cvE ' rc=0$' "$W/tmp/$m.rc") + $(wc -l < "$W/tmp/$m.ab") ))
  done
  tar -C "$W/tmp" -cf - einzeln einzeln-real | xz -6 -T0 > mag-log/einzeln.tar.xz
  echo "alle rc=$([ "$bad" = 0 ] && echo 0 || echo 1) $(date '+%T') ($(tail -1 mag-log/alle.txt); real: $(tail -1 mag-log/alle-real.txt))" >> mag-log/when
}
while [ ! -e "$A/STOP" ]; do
  git -C "$A" pull -q --ff-only origin mag/auftrag 2>/dev/null || true
  if [ "$(sha256sum "$A/lauf2.sh" | cut -c1-16)" != "$me" ]; then   # (neue Fassung: nichts Neues annehmen, auslaufen lassen, ersetzen)
    echo "$(date '+%F %T') neue Fassung ($(jobs -rp | wc -l) Aufträge laufen weiter)"; exec bash "$A/lauf2.sh"; fi   # (die laufenden halten die Sperre nicht)
  annehmen
  sleep 20
done
wait; echo "$(date '+%F %T') beendet (STOP)"
