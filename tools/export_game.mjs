// Writes a standalone HTML game from a level and an engine module (tools/export_game.sh).
//
//   node tools/export_game.mjs <level.klevel> <game.wasm> <out.html> [title]
//
// The page is the one Kern Studio's "Package Game" writes (buildGameHtml in
// kern/web/studio/game.mjs): the engine module, kern.mjs and game.mjs as one inline module, and
// the level. Here the module is one that was built for this level - with its Blueprints compiled
// in as C++ classes.
import {readFileSync, writeFileSync} from "node:fs";
import {dirname, resolve} from "node:path";
import {fileURLToPath} from "node:url";

const [levelPath, wasmPath, outPath, givenTitle] = process.argv.slice(2);
if (!levelPath || !wasmPath || !outPath) {
  console.error("usage: node tools/export_game.mjs <level.klevel> <game.wasm> <out.html> [title]");
  process.exit(2);
}
const web = resolve(dirname(fileURLToPath(import.meta.url)), "../kern/web");
const {buildGameHtml} = await import(resolve(web, "studio/game.mjs"));
const levelText = readFileSync(levelPath, "utf8");
let level;
try {
  level = JSON.parse(levelText);
} catch (error) {
  console.error(`${levelPath}: ${error.message}`);
  process.exit(1);
}
const title = givenTitle || level.world?.title || level.name || "Kern game";
const html = buildGameHtml({
  sources: {kern: readFileSync(resolve(web, "kern.mjs"), "utf8"), game: readFileSync(resolve(web, "studio/game.mjs"), "utf8")},
  wasmBase64: readFileSync(wasmPath).toString("base64"),
  levelText,
  title,
});
writeFileSync(outPath, html);
console.log(`${outPath}: "${title}" (${(html.length / 1048576).toFixed(1)} MB)`);
