# M266 Cleanup And Unwind Integration Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-part5-cleanup-unwind-integration-freeze/m266-d001-v1`
Status: Draft
Issue: `#7265`
Scope: Freeze the truthful runtime/toolchain boundary that the current runnable Part 5 cleanup slice already uses.

## Objective

`M266-D001` does not invent a new cleanup runtime. It freezes the actual boundary that exists today.

- `M266-C002` already lowers runnable `defer` cleanup insertion into native IR/object output.
- The driver/toolchain already emits the linker-response sidecar and runtime-support archive path needed to turn those objects into executables.
- The runtime already owns private cleanup-adjacent hooks through:
  - `objc3_runtime_push_autoreleasepool_scope`
  - `objc3_runtime_pop_autoreleasepool_scope`
  - `objc3_runtime_copy_memory_management_state_for_testing`

## Required Invariants

1. `runtime/objc3_runtime_bootstrap_internal.h` remains the private home of the cleanup-adjacent runtime hooks and testing snapshots.
2. `runtime/objc3_runtime.h` must not expose those private helpers publicly.
3. `runtime/objc3_runtime.cpp` keeps the autoreleasepool push/pop behavior as the current runtime-owned cleanup carrier.
4. `io/objc3_process.cpp` continues to publish the runtime-support archive path and linker-response payload used by native executable probes.
5. This issue remains truthful:
   - no public cleanup-stack ABI
   - no generalized exception runtime claim
   - no broader unwind claim than the current runnable `defer` slice plus existing private hooks
6. `M266-D002` remains the explicit next issue for broader runnable cleanup/unwind execution support.

## Dynamic Coverage

The issue-local checker must prove all of the following:

1. A generated `defer` happy-path probe compiles with `artifacts/bin/objc3c-native.exe`, emitting:
   - `module.manifest.json`
   - `module.ll`
   - `module.obj`
   - `module.runtime-registration-manifest.json`
   - `module.runtime-metadata-linker-options.rsp`
2. That same probe can be linked into `module.exe` using the emitted response file plus the emitted runtime-support archive path, and the executable exits successfully with the expected code.
3. The emitted manifest still truthfully reports the existing Part 5 lowering packet with live `defer` cleanup support.
4. A small C++ runtime probe linked against `artifacts/lib/objc3_runtime.lib` proves the private cleanup-adjacent runtime path:
   - pool push increments depth
   - autorelease queues one value
   - pool pop drains that value
5. Validation evidence lands at `tmp/reports/m266/M266-D001/cleanup_unwind_integration_contract_summary.json`.

## Non-Goals

- no public cleanup/unwind runtime header widening
- no new runtime helper family for `defer`
- no generalized language exception model
- no broader execution claims beyond the currently supported native `defer` slice and the existing private runtime hooks

## Validation

- `python scripts/check_m266_d001_cleanup_and_unwind_integration_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m266_d001_cleanup_and_unwind_integration_contract_and_architecture_freeze.py -q`
- `python scripts/run_m266_d001_lane_d_readiness.py`
