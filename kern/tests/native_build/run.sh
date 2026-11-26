#!/usr/bin/env bash
# The engine's native build, proven on a minimal project of its own (kern/docs/app.md 7.): kern/tools/build_native.sh
# with --project, then with --classes (the level's and the program's Blueprints as their classes) - each program runs
# kern/tests/native_build/mini.cpp.
#   kern/tests/native_build/run.sh        (CXX / CC, KERN_NATIVE_CACHE as build_native.sh)
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
kern="$(cd "$here/../.." && pwd)"
out="${TMPDIR:-/tmp}/kern-native-build"
"$kern/tools/build_native.sh" --project "$here/mini.kproject" -o "$out/mini" -DMINI_TIER='"interpreter"' "$here/mini.cpp"
"$out/mini"
"$kern/tools/build_native.sh" --project "$here/mini.kproject" --classes -o "$out/mini_classes" -DMINI_TIER='"native C++"' "$here/mini.cpp"
"$out/mini_classes"
# (an error says where and why: a source that is not there)
if "$kern/tools/build_native.sh" -o "$out/none" "$here/missing.cpp" 2>"$out/err.txt"; then echo "FAIL a missing source builds"; exit 1; fi
grep -q "missing.cpp: no such source" "$out/err.txt" && echo "ok   a missing source is an error with its name"
