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
  # One wrapper folder per checkout (parallel builds of several worktrees must not share it).
  wrap="${TMPDIR:-/tmp}/spawncity-wrap-$(printf '%s' "$here" | sha256sum | cut -c1-12)"
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
    "mp_host": (here / "web/mp_host.mjs").read_text(),
    "ui": (here / "web/ui.mjs").read_text(),
    "gaps_hud": (here / "web/gaps_hud.mjs").read_text(),
    "life": (here / "web/life.mjs").read_text(),
    "jobs": (web / "jobs.mjs").read_text(),
    "view": (here / "web/view.mjs").read_text(),
    "view_modes": (here / "web/view_modes.mjs").read_text(),
    "view_menu": (here / "web/view_menu.mjs").read_text(),
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
# The original's view layer (web/view, view.mjs): markup, CSS with the fonts as data: URLs, and its
# layout modules as one function (their top-level names stay their own).
view_dir = here / "web/view"
css = (view_dir / "original.css").read_text()
for font in ("InterVariable.woff2", "InterVariable-Italic.woff2"):
    css = css.replace(f'url("fonts/{font}")', 'url("data:font/woff2;base64,' + base64.b64encode((view_dir / "fonts" / font).read_bytes()).decode() + '")')
import json
layouts = "\n".join("{\n" + strip((view_dir / f).read_text()) + "\n}" for f in ("edge_hud_layout.mjs", "thumb_layout.mjs", "start_location.mjs"))
# world_options_menu.mjs imports world_options.mjs: one block for both (its names stay visible to it)
layouts += "\n{\n" + strip((view_dir / "world_options.mjs").read_text()) + "\n" + strip((view_dir / "world_options_menu.mjs").read_text()) + "\n}\n{\n" + strip((view_dir / "changelog_menu.mjs").read_text()) + "\n}"
view_inline = ("globalThis.__scViewHtml = " + json.dumps((view_dir / "original.html").read_text()) + ";\n" +
               "globalThis.__scViewCss = " + json.dumps(css) + ";\n" +
               "globalThis.__scViewLayouts = () => {\n" + layouts + "\n};\n" +
               "globalThis.__scViewFlyover = () => {\n" + (view_dir / "menu_flyover.js").read_text() + "\n" + (view_dir / "install_app.js").read_text() + "\n};\n").replace("</script", "<\\/script")
# the job workers load kern.mjs from this text (no module URL in a single file)
view_inline += "globalThis.__kernSource = " + json.dumps(sources["kern"]).replace("</script", "<\\/script") + ";\n"
bundle = view_inline + "\n".join(f"// ---- {n} ----\n" + strip(sources[n]) for n in ("kern", "app", "game", "ui", "gaps_hud", "mp_host", "life", "jobs", "view", "view_modes", "view_menu", "host"))
boot = ('<script type="text/plain" id="sc-wasm">' + b64 + '</script>\n<script type="module">\n' + bundle +
        '\nstartSpawnCity({base64: document.getElementById("sc-wasm").textContent.trim()});\n</script>')
start = html.index('<script type="module"')
end = html.index("</script>", start) + len("</script>")
single = html[:start] + boot + html[end:]
out.write_text(single)
print(f"{out}: {len(single):,} bytes (wasm {wasm.stat().st_size:,} bytes)")
PY
