// Kern jobs: work off the frame thread. A game defines pure job functions (bytes in, bytes out,
// no engine state) and submits inputs; results are taken without blocking.
//
//   * native: a small thread pool runs the jobs (std::thread) - or, set_threads(false), pump does as on the web
//   * web: the host page runs them in Web Workers on a second instance of the same module
//     (kern/web/jobs.mjs: kern_jobs_take hands out the queued inputs, kern_job_run runs one in the
//     worker, kern_job_done brings the result back). Without workers (or before the host takes
//     over) the queue runs the jobs itself in time slices of the frame (pump), one job at a time -
//     the result is the same.
//
// Job functions are registered at static initialisation (KERN_JOB), so the worker instance knows
// them without starting the game.
#pragma once

#include <string>
#include <vector>

#include "kern/core/base.h"

namespace kern::jobs {

using Fn = void (*)(Span<const u8> input, std::vector<u8>& output);

/// Registers job kind `kind` (any small number, unique per module). Returns true (for statics).
bool define(u32 kind, Fn fn);
/// Queues a job; returns its id (> 0).
u32 submit(u32 kind, std::vector<u8> input);
/// True once job `id` finished; moves its result into `out` (the job is then forgotten).
bool take(u32 id, std::vector<u8>& out);
/// Forgets a job (its result is dropped when it arrives).
void cancel(u32 id);
/// Runs queued jobs on this thread for about `budget_ms` when nobody else does (no threads, no
/// web workers) - at least one, if one waits. Call once per frame.
void pump(f64 budget_ms);
/// Jobs waiting or running.
u32 pending();
/// The host has workers (web): jobs leave through kern_jobs_take instead of running in pump.
void set_external(bool on);
bool external();
/// Native: the jobs run on worker threads (on, the default) or here, in pump, one after another in the
/// order they came - as on a page without workers. The results are the same; off, when a job runs is
/// fixed by the program's calls alone (a test that plays a program twice sees the same twice). Off
/// waits for the jobs the threads are running. (On the web there are no threads: always off.)
void set_threads(bool on);
bool threads();
/// Forgets every job - waiting, running (it waits for those the threads run) or done - and numbers the next one 1
/// again, as when the program starts; what is defined and the switches stay. (A test that plays calls of a program
/// on several sides from the same beginning: each side begins with the jobs as the others did.)
void reset();

// ---- host interface (kern/web/jobs.mjs) ----
/// Queued jobs not yet handed out, as one buffer: u32 count, then per job u32 id, u32 kind,
/// u32 length, bytes. The jobs count as running until kern_job_done.
const std::vector<u8>& take_queued();
/// Runs one job here (in the worker) and returns its output.
void run(u32 kind, Span<const u8> input, std::vector<u8>& output);
/// The result of a handed-out job.
void done(u32 id, Span<const u8> output);

}  // namespace kern::jobs

#define KERN_JOB_CAT2(a, b) a##b
#define KERN_JOB_CAT(a, b) KERN_JOB_CAT2(a, b)
/// KERN_JOB(kind, fn): registers a job function at static initialisation.
#define KERN_JOB(kind, fn) static const bool KERN_JOB_CAT(kern_job_registered_, __LINE__) = ::kern::jobs::define(kind, fn)
