#!/usr/bin/env bash
# Builds SpawnCity on Kern: the WebAssembly module and a single-file HTML page.
#
#   kern/games/spawncity/build.sh [--skip-wasm] [out.html]   (default: generated/spawncity.html)
#
# Output:
#   kern/games/spawncity/web/spawncity.wasm  the game (Kern engine + Box3D + game runtime +
#                          SpawnCity), built with kern/build.sh --app --game --commercial (no
#                          non-commercial assets). web/index.html loads it next to itself and the
#                          Kern web modules from kern/web (serve the repository root).
#   generated/spawncity.html  everything inlined (modules + wasm as base64): runs from file:// or
#                          any static host
#
# kern/build.sh keys its object cache on Kern's own headers only; the game's headers (src/*.h,
# firmware/*.hpp) go into the flags as a hash, so a header change rebuilds the game's objects.
set -euo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "$here/../../.." && pwd)"
skip_wasm=0
if [ "${1:-}" = "--skip-wasm" ]; then skip_wasm=1; shift; fi
out="${1:-$root/generated/spawncity.html}"
mkdir -p "$(dirname "$out")"
wasm="$here/web/spawncity.wasm"
if [ "$skip_wasm" = 0 ]; then
  # Each game source compiles through a generated wrapper that names the hashes of the game's
  # headers and of the source: kern/build.sh hashes the wrapper's text, so a change recompiles the
  # game's objects only (a -D flag would change the flags key and rebuild the whole engine).
  header_hash="$(find "$here/src" "$here/firmware" -name '*.h' -o -name '*.hpp' | sort | xargs cat | sha256sum | cut -c1-16)"
  wrap="${TMPDIR:-/tmp}/spawncity-wrap"
  rm -rf "$wrap" && mkdir -p "$wrap"
  wrapped=()
  for f in "$here"/src/*.cpp; do
    w="$wrap/sc_$(basename "$f")"
    printf '// SpawnCity game headers %s, source %s\n#include "%s"\n' "$header_hash" "$(sha256sum "$f" | cut -c1-16)" "$f" > "$w"
    wrapped+=("$w")
  done
  "$root/kern/build.sh" --app --game --commercial "$wasm" "${wrapped[@]}" -I"$here/src"
fi
[ -s "$wasm" ] || { echo "missing $wasm (run without --skip-wasm)" >&2; exit 1; }

python3 - "$root" "$here" "$wasm" "$out" <<'PY'
import base64, re, sys, pathlib
root, here, wasm, out = map(pathlib.Path, sys.argv[1:5])
web = root / "kern/web"
html = (here / "web/index.html").read_text()
sources = {
    "kern": (web / "kern.mjs").read_text(),
    "app": (web / "app.mjs").read_text(),
    "game": (web / "studio/game.mjs").read_text(),
    "host": (here / "web/host.mjs").read_text(),
}
for name, text in sources.items():
    if re.search(r"</script", text, re.I):
        sys.exit(f"{name} source contains '</script'; cannot inline it")
# One inline module: the sources concatenated without their imports and export keywords (no
# blob: imports, so strict content security policies still work), the wasm as base64.
def strip(text):
    text = re.sub(r'^import \{[^}]*\} from "[^"]+";\n', "", text, flags=re.M)
    return re.sub(r"^export (?=(const|let|function|async function|class) )", "", text, flags=re.M)
b64 = base64.b64encode(wasm.read_bytes()).decode()
bundle = "\n".join(f"// ---- {n} ----\n" + strip(sources[n]) for n in ("kern", "app", "game", "host"))
boot = ('<script type="text/plain" id="sc-wasm">' + b64 + '</script>\n<script type="module">\n' + bundle +
        '\nstartSpawnCity({base64: document.getElementById("sc-wasm").textContent.trim()});\n</script>')
start = html.index('<script type="module"')
end = html.index("</script>", start) + len("</script>")
single = html[:start] + boot + html[end:]
out.write_text(single)
print(f"{out}: {len(single):,} bytes (wasm {wasm.stat().st_size:,} bytes)")
PY
