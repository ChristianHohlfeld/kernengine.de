# original

url https://spawncity.de/drone_simulator.html · kern false
started after 31.9 s

## foot

frames 18 in 10 s · frame ms p50 633.4 · p95 1150.0 · p99 1150.0 · max 1199.9 · draws/frame 296 · GL calls/frame 1535
GL time per call name (ms in 10 s, calls): getProgramParameter 310 (4) · bindVertexArray 5 (4796) · uniformMatrix4fv 5 (5104) · bufferSubData 5 (1845) · uniform1f 2 (2544) · drawElements 2 (2338) · bindTexture 2 (108) · bindBuffer 2 (1872) · uniformMatrix3fv 2 (2084) · drawArrays 2 (1180) · uniform3f 2 (1752) · useProgram 1 (661)
long frames (> 150 ms) and their slowest GL calls: 417 ms [bindVertexArray 0, bufferSubData 0, uniform1f 0, uniformMatrix3fv 0, uniform3f 0] | 650 ms [getProgramParameter 20, bufferData 1, drawArrays 1, useProgram 1, uniformMatrix4fv 1] | 283 ms [bindVertexArray 1, uniformMatrix4fv 0, bindBuffer 0, bindFramebuffer 0, useProgram 0] | 467 ms [bindTexture 2, bufferSubData 0, bindVertexArray 0, useProgram 0, uniformMatrix3fv 0] | 400 ms [getProgramParameter 289, shaderSource 0, uniformMatrix4fv 0, bufferSubData 0, uniform1f 0] | 783 ms [bufferSubData 0, bindVertexArray 0, useProgram 0, uniform3f 0, uniform1f 0] | 1150 ms [uniform3f 0, bindVertexArray 0, bindBuffer 0, bufferSubData 0, uniform1f 0] | 750 ms [bindVertexArray 0, uniform1f 0, bindBuffer 0, uniformMatrix3fv 0, uniformMatrix4fv 0] | 617 ms [bufferSubData 1, bindVertexArray 1, uniformMatrix4fv 0, scissor 0, clear 0] | 633 ms [bufferSubData 0, drawElements 0, uniformMatrix4fv 0, useProgram 0, uniform3f 0] | 1200 ms [bufferSubData 1, bindVertexArray 0, uniformMatrix4fv 0, uniformMatrix3fv 0, drawElements 0] | 667 ms [bufferSubData 0, uniformMatrix4fv 0, bindBuffer 0, bindVertexArray 0, uniformMatrix3fv 0]

| self % | ms/s | function |
|---:|---:|---|
| 46.6 | 505.6 | (idle) ·  |
| 8.2 | 88.9 | (program) ·  |
| 3.3 | 35.3 | wasm-function[449] · 00387afa |
| 2.9 | 30.9 | getProgramParameter ·  |
| 1.8 | 19.0 | wasm-function[95] · 00387afa |
| 1.5 | 16.2 | wasm-function[75] · 00387afa |
| 1.2 | 13.3 | (garbage collector) ·  |
| 1.2 | 13.0 | toWireType · drone_simulator.html |
| 1.0 | 11.2 | (anonymous) · drone_simulator.html |
| 0.8 | 9.1 | wasm-function[706] · 00387afa |
| 0.8 | 8.9 | cE · drone_simulator.html |
| 0.8 | 8.2 | wasm-function[178] · 00387afa |
| 0.7 | 7.9 | sTe · drone_simulator.html |
| 0.7 | 7.5 | applyForces · drone_simulator.html |
| 0.7 | 7.1 | loadGeometry · drone_simulator.html |
| 0.6 | 6.9 | updateMatrixWorld · drone_simulator.html |
| 0.6 | 6.5 | wasm-function[789] · 00387afa |
| 0.5 | 5.7 | wasm-function[262] · 00387afa |
| 0.5 | 5.3 | now ·  |
| 0.5 | 5.1 | $le · drone_simulator.html |
| 0.5 | 5.0 | RFe · drone_simulator.html |
| 0.4 | 4.9 | aTe · drone_simulator.html |
| 0.4 | 4.8 | readSVarint · drone_simulator.html |
| 0.4 | 4.7 | (anonymous) ·  |
| 0.4 | 4.6 | get geometry · drone_simulator.html |
| 0.4 | 4.5 | DC · drone_simulator.html |
| 0.4 | 4.2 | wasm-function[424] · 00387afa |
| 0.4 | 4.2 | wasm-function[74] · 00387afa |
| 0.4 | 4.1 | drawMinimap · drone_simulator.html |
| 0.4 | 3.9 | fre · drone_simulator.html |
| 0.4 | 3.9 | proto.<computed> ·  |
| 0.3 | 3.8 | wasm-function[350] · 00387afa |
| 0.3 | 3.7 | wasm-function[502] · 00387afa |
| 0.3 | 3.3 | oG · drone_simulator.html |
| 0.3 | 3.2 | wasm-function[711] · 00387afa |
| 0.3 | 3.1 | postMessage ·  |
| 0.3 | 3.0 | traverse · drone_simulator.html |
| 0.3 | 3.0 | Ag · drone_simulator.html |
| 0.3 | 2.8 | setTimeout ·  |
| 0.3 | 2.8 | querySelector ·  |
| 0.2 | 2.7 | step · drone_simulator.html |
| 0.2 | 2.7 | pg · drone_simulator.html |
| 0.2 | 2.6 | queryIntersectsFeature · drone_simulator.html |
| 0.2 | 2.6 | oUe · drone_simulator.html |
| 0.2 | 2.6 | pose · drone_simulator.html |

## drone

frames 13 in 10 s · frame ms p50 750.0 · p95 1483.3 · p99 1483.3 · max 2049.9 · draws/frame 258 · GL calls/frame 1382
GL time per call name (ms in 10 s, calls): bindVertexArray 4 (3131) · bufferSubData 3 (1235) · uniform3f 2 (2147) · uniformMatrix4fv 2 (2809) · bindBuffer 1 (1243) · drawElementsInstanced 1 (1155) · uniformMatrix3fv 1 (1167) · uniform1f 1 (1586) · drawArrays 1 (1033) · drawElements 1 (1040) · useProgram 0 (397) · disable 0 (128)
long frames (> 150 ms) and their slowest GL calls: 750 ms [drawElementsInstanced 0, bufferSubData 0, uniform3f 0, bindVertexArray 0, uniformMatrix4fv 0] | 833 ms [bufferSubData 0, uniform1f 0, drawElements 0, uniformMatrix4fv 0, uniformMatrix3fv 0] | 183 ms [bindVertexArray 0, bufferSubData 0, uniform3f 0, drawElements 0, uniformMatrix4fv 0] | 950 ms [uniformMatrix4fv 0, bufferSubData 0, uniformMatrix3fv 0, uniform3f 0, viewport 0] | 2050 ms [bindVertexArray 0, drawElementsInstanced 0, bufferSubData 0, drawElements 0, useProgram 0] | 167 ms [bindBuffer 0, bindVertexArray 0, clear 0, useProgram 0, uniform3f 0] | 733 ms [bufferSubData 0, bindVertexArray 0, uniform1f 0, disable 0, drawElementsInstanced 0] | 800 ms [bindVertexArray 0, uniform3f 0, scissor 0, bufferSubData 0, uniformMatrix3fv 0] | 200 ms [bindVertexArray 0, uniformMatrix4fv 0, uniform3f 0, bufferSubData 0, bindBuffer 0] | 700 ms [bindVertexArray 0, uniform3f 0, bindBuffer 0, bufferSubData 0, uniformMatrix3fv 0] | 783 ms [bindVertexArray 1, drawElementsInstanced 0, bindFramebuffer 0, useProgram 0, bindBuffer 0] | 1483 ms [uniformMatrix4fv 0, bufferSubData 0, uniform3f 0, uniform1f 0, bindVertexArray 0]

| self % | ms/s | function |
|---:|---:|---|
| 50.6 | 527.5 | (idle) ·  |
| 5.8 | 61.0 | (program) ·  |
| 5.7 | 59.7 | wasm-function[449] · 00387afa |
| 2.4 | 24.8 | wasm-function[75] · 00387afa |
| 1.9 | 19.4 | wasm-function[95] · 00387afa |
| 1.7 | 17.5 | wasm-function[178] · 00387afa |
| 1.1 | 11.8 | wasm-function[191] · 00387afa |
| 0.9 | 9.9 | wasm-function[706] · 00387afa |
| 0.9 | 9.7 | cE · drone_simulator.html |
| 0.8 | 7.9 | wasm-function[789] · 00387afa |
| 0.8 | 7.8 | (garbage collector) ·  |
| 0.7 | 7.1 | wasm-function[693] · 00387afa |
| 0.7 | 6.9 | sTe · drone_simulator.html |
| 0.7 | 6.9 | (anonymous) ·  |
| 0.6 | 6.4 | toWireType · drone_simulator.html |
| 0.6 | 6.2 | applyForces · drone_simulator.html |
| 0.6 | 5.7 | wasm-function[502] · 00387afa |
| 0.5 | 5.4 | setTimeout ·  |
| 0.5 | 5.3 | querySelector ·  |
| 0.5 | 4.8 | wasm-function[711] · 00387afa |
| 0.5 | 4.8 | aTe · drone_simulator.html |
| 0.5 | 4.8 | now ·  |
| 0.4 | 4.6 | wasm-function[262] · 00387afa |
| 0.4 | 4.5 | wasm-function[74] · 00387afa |
| 0.4 | 4.4 | loadGeometry · drone_simulator.html |
| 0.4 | 4.2 | wasm-function[424] · 00387afa |
| 0.4 | 4.1 | postMessage ·  |
| 0.4 | 4.1 | DC · drone_simulator.html |
| 0.4 | 3.8 | get geometry · drone_simulator.html |
| 0.3 | 3.5 | wasm-function[350] · 00387afa |
| 0.3 | 3.5 | PKe · drone_simulator.html |
| 0.3 | 3.4 | step · drone_simulator.html |
| 0.3 | 3.4 | _Ht · drone_simulator.html |
| 0.3 | 3.3 | updateMatrixWorld · drone_simulator.html |
| 0.3 | 3.2 | wasm-function[691] · 00387afa |
| 0.3 | 3.2 | RFe · drone_simulator.html |
| 0.3 | 3.1 | Ag · drone_simulator.html |
| 0.3 | 3.1 | (anonymous) · drone_simulator.html |
| 0.3 | 3.0 | drawMinimap · drone_simulator.html |
| 0.3 | 2.8 | oUe · drone_simulator.html |
| 0.3 | 2.8 | $le · drone_simulator.html |
| 0.3 | 2.7 | fre · drone_simulator.html |
| 0.2 | 2.5 | renderReal · drone_simulator.html |
| 0.2 | 2.5 | worldVector · drone_simulator.html |
| 0.2 | 2.5 | pg · drone_simulator.html |

## car

frames 14 in 10 s · frame ms p50 783.3 · p95 866.6 · p99 866.6 · max 1100.0 · draws/frame 259 · GL calls/frame 1368
GL time per call name (ms in 10 s, calls): uniformMatrix4fv 6 (3066) · bufferSubData 6 (1444) · bindVertexArray 5 (3334) · uniform3f 3 (2235) · uniformMatrix3fv 2 (1294) · bindFramebuffer 2 (56) · bindBuffer 2 (1444) · drawElementsInstanced 2 (1246) · uniform1f 2 (1400) · drawArrays 2 (1112) · drawElements 1 (1121) · useProgram 1 (408)
long frames (> 150 ms) and their slowest GL calls: 667 ms [getProgramParameter 514, bindVertexArray 1, bufferSubData 0, blendFuncSeparate 0, drawElementsInstanced 0] | 817 ms [bufferSubData 3, bindFramebuffer 2, uniformMatrix4fv 1, uniformMatrix3fv 0, drawElementsInstanced 0] | 1100 ms [uniformMatrix4fv 3, bufferSubData 0, drawArrays 0, bindFramebuffer 0, bindVertexArray 0] | 850 ms [uniform3f 0, uniform1f 0, useProgram 0, bufferSubData 0, drawArrays 0] | 867 ms [uniformMatrix3fv 0, bindBuffer 0, bindVertexArray 0, drawElements 0, bufferSubData 0] | 850 ms [bufferSubData 0, uniform3f 0, enable 0, bindBuffer 0, uniformMatrix4fv 0] | 783 ms [bindVertexArray 1, bufferSubData 0, useProgram 0, uniform1f 0, bindBuffer 0] | 767 ms [uniform3f 0, uniformMatrix4fv 0, bindVertexArray 0, bufferSubData 0, uniformMatrix3fv 0] | 800 ms [bindVertexArray 1, uniformMatrix4fv 1, drawElements 1, drawArrays 1, bufferSubData 1] | 783 ms [bindVertexArray 0, uniformMatrix4fv 0, uniform3f 0, drawElementsInstanced 0, clear 0] | 783 ms [bindVertexArray 0, uniformMatrix4fv 0, uniform1f 0, bindBuffer 0, bufferSubData 0] | 783 ms [bindVertexArray 1, uniformMatrix4fv 0, bufferSubData 0, uniform3f 0, bindBuffer 0]

| self % | ms/s | function |
|---:|---:|---|
| 45.6 | 484.6 | (idle) ·  |
| 7.6 | 81.3 | (program) ·  |
| 6.3 | 66.6 | wasm-function[449] · 00387afa |
| 2.8 | 29.7 | wasm-function[75] · 00387afa |
| 2.2 | 22.9 | wasm-function[95] · 00387afa |
| 1.7 | 18.4 | wasm-function[178] · 00387afa |
| 1.3 | 14.1 | wasm-function[191] · 00387afa |
| 1.2 | 12.3 | wasm-function[706] · 00387afa |
| 1.0 | 10.2 | cE · drone_simulator.html |
| 0.9 | 9.7 | (garbage collector) ·  |
| 0.9 | 9.3 | toWireType · drone_simulator.html |
| 0.8 | 8.4 | querySelector ·  |
| 0.7 | 7.1 | wasm-function[693] · 00387afa |
| 0.6 | 6.9 | wasm-function[789] · 00387afa |
| 0.6 | 6.7 | (anonymous) ·  |
| 0.6 | 6.4 | wasm-function[502] · 00387afa |
| 0.6 | 6.3 | applyForces · drone_simulator.html |
| 0.5 | 5.7 | DC · drone_simulator.html |
| 0.5 | 5.6 | setTimeout ·  |
| 0.5 | 5.5 | updateMatrixWorld · drone_simulator.html |
| 0.5 | 5.3 | PKe · drone_simulator.html |
| 0.5 | 5.3 | now ·  |
| 0.5 | 5.2 | wasm-function[262] · 00387afa |
| 0.5 | 5.1 | wasm-function[424] · 00387afa |
| 0.5 | 5.0 | wasm-function[74] · 00387afa |
| 0.5 | 4.8 | wasm-function[350] · 00387afa |
| 0.4 | 4.3 | wasm-function[711] · 00387afa |
| 0.4 | 4.2 | (anonymous) · drone_simulator.html |
| 0.4 | 4.2 | step · drone_simulator.html |
| 0.4 | 3.9 | fre · drone_simulator.html |
| 0.4 | 3.9 | renderReal · drone_simulator.html |
| 0.4 | 3.8 | wasm-function[691] · 00387afa |
| 0.3 | 3.6 | pg · drone_simulator.html |
| 0.3 | 3.4 | RFe · drone_simulator.html |
| 0.3 | 3.4 | sTe · drone_simulator.html |
| 0.3 | 3.3 | Ag · drone_simulator.html |
| 0.3 | 3.3 | postMessage ·  |
| 0.3 | 3.0 | rG · drone_simulator.html |
| 0.3 | 3.0 | oG · drone_simulator.html |
| 0.3 | 2.9 | worldVector · drone_simulator.html |
| 0.3 | 2.9 | aTe · drone_simulator.html |
| 0.3 | 2.8 | L0 · drone_simulator.html |
| 0.3 | 2.8 | _Ht · drone_simulator.html |
| 0.2 | 2.6 | traverse · drone_simulator.html |
| 0.2 | 2.5 | pose · drone_simulator.html |
