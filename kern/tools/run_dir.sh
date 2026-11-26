# The folder of one run - sourced by every script that builds (kern/build.sh, kern/tools/build_native.sh,
# tools/blueprint_cpp.sh, tools/converter_cases/compare.sh, the games' test scripts).
#
# Nothing a build makes is kept from one run to the next: no cache. What a run builds once and needs again -
# the engine's objects for a second module, the tool of tools/blueprint_cpp.sh for its next call - it takes again
# only from this folder, which the run itself made new and empty:
#   - KERN_RUN_DIR not set: the script that sources this first makes it (mktemp) and removes it when it ends; the
#     scripts it calls get it through the environment, so within this one call nothing is built twice.
#   - KERN_RUN_DIR set: the caller's folder of its run (a CI job sets a new one on its fresh runner; it ends with
#     the job). Never a folder that outlives its run.
# A key inside it (a hash of the source, the switches, the headers) only tells apart the builds of the same run.
if [ -z "${KERN_RUN_DIR:-}" ]; then
  KERN_RUN_DIR="$(mktemp -d "${TMPDIR:-/tmp}/kern-run-XXXXXX")"
  export KERN_RUN_DIR
  # (removed when the script ends - after what its own trap on EXIT does, if it set one before; a script that sets
  # one after sourcing this keeps the line: trap '... ; rm -rf "$KERN_RUN_DIR"' EXIT)
  kern_run_dir_before="$(trap -p EXIT | sed -n "s/^trap -- '\(.*\)' EXIT\$/\1/p")"
  trap -- "${kern_run_dir_before:+$kern_run_dir_before; }rm -rf \"\$KERN_RUN_DIR\"" EXIT
fi
mkdir -p "$KERN_RUN_DIR"
