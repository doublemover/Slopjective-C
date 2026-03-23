# M269 Runnable Task And Executor Matrix Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-part7-runnable-task-executor-matrix/m269-e002-v1`

Issue: `#7306`

## Objective

Close `M269` truthfully by consuming the already-landed task source, semantic, lowering, runtime, and lane-E gate proofs for the current runnable Part 7 task/runtime slice.

## Required implementation

1. Add the planning packet, deterministic checker, tooling test, and direct lane-E readiness runner:
   - `scripts/check_m269_e002_runnable_task_and_executor_matrix_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m269_e002_runnable_task_and_executor_matrix_cross_lane_integration_sync.py`
   - `scripts/run_m269_e002_lane_e_readiness.py`
2. Add explicit `M269-E002` anchor text to:
   - `docs/objc3c-native/src/20-grammar.md`
   - `docs/objc3c-native.md`
   - `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
   - `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
   - `native/objc3c/src/driver/objc3_objc3_path.cpp`
   - `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
   - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
   - `package.json`
3. Keep the closeout fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m269/M269-A002/task_group_cancellation_source_closure_summary.json`
   - `tmp/reports/m269/M269-B003/executor_hop_affinity_compatibility_summary.json`
   - `tmp/reports/m269/M269-C003/task_runtime_abi_completion_summary.json`
   - `tmp/reports/m269/M269-D003/task_runtime_hardening_summary.json`
   - `tmp/reports/m269/M269-E001/task_executor_conformance_gate_summary.json`
4. The checker must reject drift if any upstream summary disappears, stops reporting successful coverage, or drops the dynamic-proof indicator that keeps the earlier `M269` chain honest.
5. The closeout summary must publish a runnable matrix with rows for:
   - task creation source closure
   - executor-affinity legality
   - task runtime ABI/helper publication
   - hardened cancellation/autorelease replay
   - lane-E conformance gate
6. `package.json` must wire:
   - `check:objc3c:m269-e002-runnable-task-and-executor-matrix-cross-lane-integration-sync`
   - `test:tooling:m269-e002-runnable-task-and-executor-matrix-cross-lane-integration-sync`
   - `check:objc3c:m269-e002-lane-e-readiness`
7. The closeout must explicitly hand off from `M269-E001` to `M270-A001`.

## Canonical models

- Evidence model:
  `a002-through-e001-summary-chain-runnable-task-runtime-closeout`
- Closeout model:
  `lane-e-closeout-replays-implemented-part7-task-runtime-slice-without-surface-widening`
- Failure model:
  `fail-closed-on-runnable-task-runtime-closeout-drift`

## Non-goals

- No new runtime semantics.
- No new task helper ABI.
- No widened front-door metadata-export claim.
- No new executable probe beyond the already-landed upstream proofs.

## Evidence

- `tmp/reports/m269/M269-E002/runnable_task_and_executor_matrix_summary.json`
