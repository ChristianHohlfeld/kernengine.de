#!/usr/bin/env bash
# SpawnCity on Kern as a drop-in for spawncity.de: the files to lay over the site's own (GitHub
# Pages of ChristianHohlfeld/spawncity-de). The game answers at the same address as the original
# (/drone_simulator.html), with the original's manifest (installed apps keep their identity), and
# the original's service worker file now carries Kern's (network first), so a phone with the old
# worker gets the new game on its next start. The landing page, SEO files and CNAME stay the site's.
#
#   kern/games/spawncity/tools/dropin.sh OUT_DIR      (after kern/games/spawncity/build.sh)
#
# Licensed under the Apache License 2.0 (see LICENSE in the repository root).
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
kern="$(cd "$here/../.." && pwd)"
out="${1:?out dir}"
mkdir -p "$out/kern/web/studio"
# the page and its modules at the site root
cp -r "$here/web/." "$out/"
rm -f "$out/index.html"
[ -f "$out/spawncity.wasm" ] || { echo "spawncity.wasm missing (run build.sh first)" >&2; exit 1; }
# Kern's web modules next to it (host.mjs imports them from the repository layout)
cp "$kern/web/app.mjs" "$kern/web/kern.mjs" "$kern/web/jobs.mjs" "$out/kern/web/"
cp "$kern/web/studio/game.mjs" "$out/kern/web/studio/"
sed -i 's#"\.\./\.\./\.\./web/#"./kern/web/#g' "$out/host.mjs"
grep -q '\.\./\.\./\.\./' "$out/host.mjs" && { echo "host.mjs: a repository path is left" >&2; exit 1; }
# the game at the original's address, with the original's manifest
sed -e 's#href="\./manifest\.webmanifest"#href="./drone_simulator.webmanifest"#' "$here/web/index.html" > "$out/drone_simulator.html"
grep -q 'drone_simulator.webmanifest' "$out/drone_simulator.html" || { echo "manifest link not rewritten" >&2; exit 1; }
rm -f "$out/manifest.webmanifest"
# the original's worker file (registered for /drone_simulator.html) becomes Kern's worker
cp "$here/web/sw.js" "$out/oppenheimer-sw.js"
echo "drop-in for spawncity.de in $out ($(du -sh "$out" | cut -f1))"
