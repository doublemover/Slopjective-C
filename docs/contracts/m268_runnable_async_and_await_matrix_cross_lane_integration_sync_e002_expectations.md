# M268 Runnable Async And Await Matrix Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-part7-runnable-async-and-await-matrix/m268-e002-v1`

Issue: `#7293`

## Objective

Close `M268` truthfully by consuming the already-landed parser, semantic,
lowering, runtime, and lane-E gate proofs for the current runnable Part 7
async/await slice.

## Required implementation

1. Add the planning packet, deterministic checker, tooling test, and direct
   lane-E readiness runner:
   - `scripts/check_m268_e002_runnable_async_and_await_matrix_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m268_e002_runnable_async_and_await_matrix_cross_lane_integration_sync.py`
   - `scripts/run_m268_e002_lane_e_readiness.py`
2. Add explicit `M268-E002` anchor text to:
   - `docs/objc3c-native/src/20-grammar.md`
   - `docs/objc3c-native.md`
   - `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
   - `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
   - `native/objc3c/src/driver/objc3_objc3_path.cpp`
   - `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
   - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
   - `package.json`
3. Keep the closeout fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m268/M268-A002/async_semantic_packet_summary.json`
   - `tmp/reports/m268/M268-B003/async_diagnostics_compatibility_completion_summary.json`
   - `tmp/reports/m268/M268-C003/async_cleanup_integration_summary.json`
   - `tmp/reports/m268/M268-D002/live_continuation_runtime_integration_summary.json`
   - `tmp/reports/m268/M268-E001/async_executable_conformance_gate_summary.json`
4. The checker must reject drift if any upstream summary disappears, stops
   reporting successful coverage, or drops the dynamic-proof indicator that
   keeps the earlier `M268` chain honest.
5. The closeout summary must publish a runnable matrix with rows for:
   - async function entry
   - async Objective-C method entry
   - direct-call await lowering
   - cleanup integration
   - live continuation helper execution
6. `package.json` must wire:
   - `check:objc3c:m268-e002-runnable-async-and-await-matrix-cross-lane-integration-sync`
   - `test:tooling:m268-e002-runnable-async-and-await-matrix-cross-lane-integration-sync`
   - `check:objc3c:m268-e002-lane-e-readiness`
7. The closeout must explicitly hand off from `M268-E001` to `M269-A001`.

## Canonical models

- Evidence model:
  `a002-through-e001-summary-chain-runnable-part7-closeout`
- Closeout model:
  `lane-e-closeout-replays-implemented-part7-slice-without-surface-widening`
- Failure model:
  `fail-closed-on-runnable-part7-closeout-drift`

## Non-goals

- No new runtime semantics.
- No new continuation helper ABI.
- No suspension-frame or state-machine claim.
- No new executable probe beyond the already-landed upstream proofs.

## Evidence

- `tmp/reports/m268/M268-E002/runnable_async_and_await_matrix_summary.json`
