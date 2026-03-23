# M270 Runnable Actor And Isolation Matrix Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-part7-runnable-actor-isolation-matrix/m270-e002-v1`

Issue: `#7319`

## Objective

Close `M270` truthfully by consuming the already-landed actor source, semantic, lowering, runtime, cross-module, and lane-E gate proofs for the current runnable Part 7 actor/isolation slice.

## Required implementation

1. Add the planning packet, deterministic checker, tooling test, and direct lane-E readiness runner:
   - `scripts/check_m270_e002_runnable_actor_and_isolation_matrix_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m270_e002_runnable_actor_and_isolation_matrix_cross_lane_integration_sync.py`
   - `scripts/run_m270_e002_lane_e_readiness.py`
2. Add explicit `M270-E002` anchor text to:
   - `docs/objc3c-native/src/20-grammar.md`
   - `docs/objc3c-native.md`
   - `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
   - `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
   - `native/objc3c/src/driver/objc3_objc3_path.cpp`
   - `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
   - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
   - `package.json`
3. Keep the closeout fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m270/M270-A002/actor_member_isolation_source_closure_summary.json`
   - `tmp/reports/m270/M270-B003/actor_race_hazard_escape_diagnostics_summary.json`
   - `tmp/reports/m270/M270-C003/actor_replay_race_guard_summary.json`
   - `tmp/reports/m270/M270-D002/live_actor_mailbox_runtime_summary.json`
   - `tmp/reports/m270/M270-D003/cross_module_actor_isolation_metadata_summary.json`
   - `tmp/reports/m270/M270-E001/strict_concurrency_conformance_gate_summary.json`
4. The checker must reject drift if any upstream summary disappears, stops reporting successful coverage, or drops the dynamic-proof indicator that keeps the earlier `M270` chain honest.
5. The closeout summary must publish a runnable matrix with rows for:
   - actor member and isolation source closure
   - actor race-hazard and strict-concurrency diagnostics
   - actor hop/replay/race artifact integration
   - live actor mailbox and isolation runtime
   - cross-module actor isolation preservation
   - lane-E strict concurrency conformance gate
6. `package.json` must wire:
   - `check:objc3c:m270-e002-runnable-actor-and-isolation-matrix-cross-lane-integration-sync`
   - `test:tooling:m270-e002-runnable-actor-and-isolation-matrix-cross-lane-integration-sync`
   - `check:objc3c:m270-e002-lane-e-readiness`
7. The closeout must explicitly hand off from `M270-E001` to `M271-A001`.

## Canonical models

- Evidence model:
  `a002-through-e001-summary-chain-runnable-actor-isolation-closeout`
- Closeout model:
  `lane-e-closeout-replays-implemented-part7-actor-isolation-slice-without-surface-widening`
- Failure model:
  `fail-closed-on-runnable-actor-isolation-closeout-drift`

## Non-goals

- No new runtime semantics.
- No new actor helper ABI.
- No widened front-door metadata-export claim.
- No new executable probe beyond the already-landed upstream proofs.

## Evidence

- `tmp/reports/m270/M270-E002/runnable_actor_and_isolation_matrix_summary.json`
