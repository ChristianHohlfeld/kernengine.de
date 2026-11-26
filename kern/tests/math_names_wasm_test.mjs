// std::sin, sinf, atan2 ... in WebAssembly are the engine's musl, bit for bit what kern::dm gives (math_names.h), and the
// digest of all the results is the native one (math_names_digest.txt).
//   node kern/tests/math_names_wasm_test.mjs <math_names_test.wasm>
import fs from "node:fs";
import {loadKern} from "../web/kern.mjs";

const kern = await loadKern({bytes: fs.readFileSync(process.argv[2])});
const x = kern.exports;
const text = ptr => { const m = new Uint8Array(x.memory.buffer); let e = ptr; while (m[e]) e++; return new TextDecoder().decode(m.subarray(ptr, e)); };
const differ = x.math_names_run();
const digest = (x.math_names_digest_high() >>> 0).toString(16).padStart(8, "0") + (x.math_names_digest_low() >>> 0).toString(16).padStart(8, "0");
const want = fs.readFileSync(new URL("./math_names_digest.txt", import.meta.url), "utf8").trim();
let failures = 0;
if (differ !== 0) { console.log(`FAIL ${differ} of ${x.math_names_compared()} differ from the engine's - the first: ${text(x.math_names_first())}`); failures++; }
else console.log(`ok   ${x.math_names_compared()} calls of std:: mathematics are the engine's, bit for bit`);
if (digest !== want) { console.log(`FAIL the digest ${digest} is not the native one (math_names_digest.txt: ${want})`); failures++; }
else console.log(`ok   the digest ${digest} is the native one`);
process.exit(failures ? 1 : 0);
