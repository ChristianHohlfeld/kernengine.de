// Kern jobs on the web (kern/include/kern/core/jobs.h): the module's queued jobs run in Web
// Workers, each on its own instance of the same WebAssembly module (no renderer, no game started:
// only the job functions the module registered at static initialisation). Inputs leave with
// kern_jobs_take, results come back with kern_job_done - as transferred ArrayBuffers.
//
//   const jobs = await startKernJobs(kern, wasmModule);   // a compiled WebAssembly.Module
//   ...every frame: jobs.pump();
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).

// The worker: loads the module, runs one job per message.
const KERN_JOB_WORKER = `
let kern = null;
self.onmessage = async ev => {
  const m = ev.data;
  if (m.init) {
    try {
      kern = await __loadKern({module: m.module});
      self.postMessage({ready: true});
    } catch (error) { self.postMessage({ready: false, error: String(error?.message || error)}); }
    return;
  }
  try {
    const e = kern.exports, input = new Uint8Array(m.input);
    const ptr = e.kern_alloc(Math.max(1, input.length));
    new Uint8Array(kern.memory.buffer, ptr, input.length).set(input);
    const out = e.kern_job_run(m.kind, ptr, input.length), len = e.kern_job_result_length();
    const result = new Uint8Array(kern.memory.buffer, out, len).slice();
    e.kern_free(ptr);
    self.postMessage({id: m.id, output: result.buffer}, [result.buffer]);
  } catch (error) {
    // a trap in this instance: the page runs the job itself (and this worker is replaced)
    self.postMessage({id: m.id, error: String(error?.message || error)});
  }
};
`;

/**
 * Starts `count` workers for the module (default: cores - 1, at most 3). Resolves to null when the
 * browser has no workers; the module then runs its jobs itself in frame slices.
 * `kernSource`: the text of kern.mjs (single-file pages); otherwise the workers import it by URL.
 */
export async function startKernJobs(kern, wasmModule, {count = 0, kernSource = null, kernUrl = new URL("./kern.mjs", import.meta.url).href} = {}) {
  const e = kern.exports;
  if (typeof Worker !== "function" || !e.kern_jobs_take || !(wasmModule instanceof WebAssembly.Module)) return null;
  const n = count || Math.max(1, Math.min(3, (navigator.hardwareConcurrency || 2) - 1));
  const head = kernSource
    ? kernSource.replace(/^export\s+/gm, "") + "\nconst __loadKern = loadKern;\n"
    : `import {loadKern as __loadKern} from ${JSON.stringify(kernUrl)};\n`;
  const url = URL.createObjectURL(new Blob([head + KERN_JOB_WORKER], {type: "text/javascript"}));
  const workers = [];
  try {
    for (let i = 0; i < n; i++) {
      const w = new Worker(url, {type: "module"});
      const ready = await new Promise(resolve => {
        w.onmessage = ev => resolve(ev.data);
        w.onerror = ev => resolve({ready: false, error: ev.message});
        w.postMessage({init: true, module: wasmModule});
      });
      if (!ready.ready) { w.terminate(); console.warn("[kern] job worker failed:", ready.error); continue; }
      workers.push({w, busy: 0});
    }
  } finally { URL.revokeObjectURL(url); }
  if (!workers.length) return null;
  const done = (id, output) => {
    const bytes = new Uint8Array(output), ptr = e.kern_alloc(Math.max(1, bytes.length));
    new Uint8Array(kern.memory.buffer, ptr, bytes.length).set(bytes);
    e.kern_job_done(id, ptr, bytes.length);
    e.kern_free(ptr);
  };
  // Every job handed out is kept until its answer arrives: a worker that fails (a trap, an error
  // event) or stays silent for 20 s does not leave the game waiting - the page runs the job on its
  // own module (the same code), and a failed worker is not given more work.
  const out = new Map();  // id -> {kind, input, at, s}
  const local = (id, job) => {
    try {
      const ptr = e.kern_alloc(Math.max(1, job.input.length));
      new Uint8Array(kern.memory.buffer, ptr, job.input.length).set(job.input);
      const res = e.kern_job_run(job.kind, ptr, job.input.length), len = e.kern_job_result_length();
      const bytes = new Uint8Array(kern.memory.buffer, res, len).slice();
      e.kern_free(ptr);
      done(id, bytes.buffer);
    } catch (error) { console.warn("[kern] job", id, "failed on the page too:", error?.message || error); done(id, new ArrayBuffer(0)); }
  };
  const fail = (s, why) => {
    if (s.dead) return;
    s.dead = true;
    console.warn("[kern] job worker failed, its jobs run on the page:", why);
    for (const [id, job] of out) if (job.s === s) { out.delete(id); local(id, job); }
    try { s.w.terminate(); } catch { /* gone */ }
  };
  for (const s of workers) {
    s.w.onmessage = ev => {
      s.busy--;
      const id = ev.data.id, job = out.get(id);
      out.delete(id);
      if (ev.data.error) { console.warn("[kern] job", id, "trapped in a worker:", ev.data.error); if (job) local(id, job); fail(s, ev.data.error); return; }
      done(id, ev.data.output);
    };
    s.w.onerror = ev => { ev.preventDefault?.(); fail(s, ev.message || "error event"); };
  }
  e.kern_jobs_external(1);
  return {
    workers: workers.length,
    /** Hands the module's queued jobs to the least busy workers (call once per frame). */
    pump() {
      const ptr = e.kern_jobs_take(), len = e.kern_jobs_take_length();
      if (len < 4) return;
      const view = new DataView(kern.memory.buffer, ptr, len);
      let at = 4;
      for (let i = view.getUint32(0, true); i > 0; i--) {
        const id = view.getUint32(at, true), kind = view.getUint32(at + 4, true), n = view.getUint32(at + 8, true);
        at += 12;
        const input = new Uint8Array(kern.memory.buffer, ptr + at, n).slice();
        at += n;
        const live = workers.filter(w => !w.dead);
        if (!live.length) { local(id, {kind, input}); continue; }
        const s = live.reduce((a, b) => (b.busy < a.busy ? b : a));
        s.busy++;
        out.set(id, {kind, input: input.slice(), at: performance.now(), s});
        s.w.postMessage({id, kind, input: input.buffer}, [input.buffer]);
      }
      // a job silent for 20 s: its worker is stuck
      const now = performance.now();
      for (const [id, job] of out) if (now - job.at > 20000) { fail(job.s, `job ${id} silent for 20 s`); break; }
    },
    stop() { for (const s of workers) s.w.terminate(); e.kern_jobs_external(0); },
  };
}
