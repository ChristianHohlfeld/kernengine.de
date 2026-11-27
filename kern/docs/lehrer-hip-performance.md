# Native HIP throughput on MAG

Only the optional HIP build uses concurrent streams. The existing arithmetic
order, eight deterministic PPO gradient chunks, seeds, environment count,
network sizes, CPU simulation, WASM and ONNX format are unchanged. GPU
allocation/copy/kernel errors are still fatal, not CPU fallback. Each visible GPU
has four independent workspaces by default (device buffers, pinned host staging
and nonblocking HIP stream); the CPU waits only for the stream producing its
result. Workspaces grow lazily and are reused until process exit. Device
selection remains round-robin across all visible GPUs.

`KERN_HIP_STREAMS=1..8` selects the stream count per GPU at process start.
Use 1, 2, 4 and 8 for a controlled sweep; 4 is an initial engineering default,
not a measured optimum. More streams consume additional pinned RAM and VRAM.
The one-stream setting is a diagnostic configuration of the new code, not the
old implementation (it also uses pinned staging). Do not change the GPU
power cap, driver, GPU locks or host services for this comparison.

## Acceptance on MAG

1. Preserve the existing job's logs and checkpoint. End training normally
   before updating the service. Never run a separate benchmark alongside the
   MAG GPU lease or bypass the exclusive hook.
2. Build the new commit in its own build directory with the existing ROCm and
   gfx906 flags. The Studio SHA and remote service SHA must match.
3. Through the existing remote job path, run the same task, seed, iterations,
   ducks per worker, PPO options and number of visible GPUs as the baseline.
   Compare at least five steady-state iterations, omitting startup and
   imitation from PPO timings. Test imitation separately with the same task.
4. The service runs the native `--hip-check` under the GPU lease. Each GPU and
   each stream must pass three concurrent forward/gradient comparisons against
   the CPU, with distinct input parameters per stream. Check the training
   log for the stream count. Fail closed if HIP fails.
5. The existing training log contains `KERN_PROGRESS` records with
   `sps`, `collectSeconds` and `learnSeconds`. Use median steps/s and
   median learning/collection time; record GPU activity with rocm-smi.
   Compare output/checkpoints for an identical deterministic run as well.
   The small self-test alone does not establish whole-run bit identity.
6. Verify ONNX import and that MAG services return to their prior state
   after stop/completion. Keep the old binary and restore the old matched
   Studio/server release if acceptance fails.

If necessary, configure the stream count with a systemd service drop-in
(`Environment=KERN_HIP_STREAMS=2`, for example) only between completed jobs,
then reload/restart the service. Remove the override to restore the default.
Use the normal job path for every measured run.

The objective is higher end-to-end simulation steps/s at unchanged learning
semantics, not 100% GPU utilization or higher VRAM consumption. Physics,
single-row inference, optimizer and PPO control still run on CPU. The native
HIP compile, numeric checks and speedup require actual ROCm hardware; ordinary
CPU/WASM CI does not prove them.
