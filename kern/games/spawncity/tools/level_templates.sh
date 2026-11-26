#!/usr/bin/env bash
# Builds tools/level_templates.cpp (the player and the vehicle templates as the level file has actors) and puts what it
# writes into level/spawncity.klevel (tools/level_templates.py: nothing else of the level changes).
#
#   kern/games/spawncity/tools/level_templates.sh [<level.klevel>]      (CXX / CC: the compilers, default c++ / cc)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
game="$(dirname "$here")"
kern="$(cd "$game/../.." && pwd)"
level="${1:-$game/level/spawncity.klevel}"
cxx="${CXX:-c++}" cc="${CC:-cc}"
out="${TMPDIR:-/tmp}/sc-level-templates"
mkdir -p "$out/obj"
flags=(-std=c++20 -O1 -ffp-contract=off -fno-exceptions -fno-rtti -w -ffunction-sections -fdata-sections -I"$kern/include" -I"$kern/third_party/box3d/include" -I"$game/src")
sources=("$here/level_templates.cpp" "$game/src/car.cpp" "$game/src/drone.cpp" "$game/src/jet.cpp" "$kern/src/game/level.cpp" "$kern/src/audio/audio.cpp"
         "$kern/src/game/logic_node_shapes.cpp" "$kern/src/game/world_export.cpp")
while IFS= read -r f; do sources+=("$f"); done < <(find "$kern/src/logic" "$kern/src/core" -name '*.cpp' | sort)
export cxx out
printf '%s\n' "${sources[@]}" | xargs -P "${KERN_JOBS:-$(nproc)}" -I{} bash -c 'o="$out/obj/$(echo "$0" | sed "s|/|_|g").o"; "$cxx" "$@" -c "$0" -o "$o"' {} "${flags[@]}"
for c in "$kern"/third_party/musl_math/src/*.c; do "$cc" -std=c17 -O2 -ffp-contract=off -fno-math-errno -w -c "$c" -o "$out/obj/$(basename "$c").o"; done
"$cxx" -Wl,--gc-sections "$out"/obj/*.o -lm -o "$out/level_templates"
"$out/level_templates" > "$out/actors.json"
python3 "$here/level_templates.py" "$out/actors.json" "$level"
