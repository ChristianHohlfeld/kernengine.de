// Writes a standalone HTML page of a game whose App is built with its project (tools/export_game.sh <game.kproject>,
// kern/docs/app.md 7.): the module (base64), kern/web/kern.mjs, audio.mjs and app.mjs as one inline module, and a
// canvas the App fills - its widgets, sound and input are the app runtime's. Title: the project's name.
//
//   node tools/export_app.mjs <game.kproject> <game.wasm> <out.html> [title]
import {readFileSync, writeFileSync} from "node:fs";
import {dirname, resolve} from "node:path";
import {fileURLToPath} from "node:url";

const [projectPath, wasmPath, outPath, givenTitle] = process.argv.slice(2);
if (!projectPath || !wasmPath || !outPath) {
  console.error("usage: node tools/export_app.mjs <game.kproject> <game.wasm> <out.html> [title]");
  process.exit(2);
}
const web = resolve(dirname(fileURLToPath(import.meta.url)), "../kern/web");
const {inlineModules, escHtml} = await import(resolve(web, "studio/game.mjs"));
let project;
try {
  project = JSON.parse(readFileSync(projectPath, "utf8"));
} catch (error) {
  console.error(`${projectPath}: ${error.message}`);
  process.exit(1);
}
if (project.format !== "kern-project") { console.error(`${projectPath}: not a Kern project (format must be "kern-project")`); process.exit(1); }
const title = givenTitle || project.name || "Kern game";
const source = name => readFileSync(resolve(web, name), "utf8");
const code = inlineModules(source("kern.mjs"), source("audio.mjs"), source("app.mjs")).replace(/<\/script/gi, "<\\/script");
const wasm = readFileSync(wasmPath).toString("base64");
const html = `<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="generator" content="Kern"><title>${escHtml(title)}</title>
<style>html,body{margin:0;height:100%;background:#0b1018;color:#e8eef6;font:15px system-ui,sans-serif;overflow:hidden}
canvas{display:block;width:100%;height:100%}#error{position:fixed;left:12px;top:12px;padding:6px 10px;background:rgba(120,0,0,.75);border-radius:6px;white-space:pre}#error:empty{display:none}</style>
</head><body><canvas id="game"></canvas><div id="error" role="alert"></div>
<script type="text/plain" id="kern-wasm">${wasm}</script>
<script type="module">
${code}
runKernApp({canvas: document.getElementById("game"), base64: document.getElementById("kern-wasm").textContent.trim()})
  .then(app => (window.__kernApp = app))
  .catch(error => { document.getElementById("error").textContent = String(error); window.__kernError = String(error); });
</script></body></html>
`;
writeFileSync(outPath, html);
console.log(`export_app: ${outPath} (${title}, ${html.length} bytes)`);
