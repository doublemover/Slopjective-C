# M267 Runnable Throws, Result, And Bridge Matrix Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-part6-runnable-throws-result-and-bridge-matrix/m267-e002-v1`

Issue: `#7281`

## Objective

Close `M267` truthfully by consuming the already-landed source, semantic,
lowering, runtime, cross-module, and lane-E gate proofs for the current Part 6
throws/Result/NSError bridge slice.

## Required implementation

1. Add the planning packet, deterministic checker, tooling test, and direct
   lane-E readiness runner:
   - `scripts/check_m267_e002_runnable_throws_result_and_bridge_matrix_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m267_e002_runnable_throws_result_and_bridge_matrix_cross_lane_integration_sync.py`
   - `scripts/run_m267_e002_lane_e_readiness.py`
2. Add explicit `M267-E002` anchor text to:
   - `docs/objc3c-native/src/30-semantics.md`
   - `docs/objc3c-native.md`
   - `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
   - `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `spec/PART_6_ERRORS_RESULTS_THROWS.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `package.json`
3. Keep the closeout fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m267/M267-A001/error_source_closure_summary.json`
   - `tmp/reports/m267/M267-A002/error_bridge_marker_surface_summary.json`
   - `tmp/reports/m267/M267-B001/error_semantic_model_summary.json`
   - `tmp/reports/m267/M267-B002/try_do_catch_semantics_summary.json`
   - `tmp/reports/m267/M267-B003/bridging_legality_summary.json`
   - `tmp/reports/m267/M267-C001/throws_abi_and_propagation_lowering_summary.json`
   - `tmp/reports/m267/M267-C002/error_out_abi_and_propagation_lowering_summary.json`
   - `tmp/reports/m267/M267-C003/result_and_bridging_artifact_replay_completion_summary.json`
   - `tmp/reports/m267/M267-D001/error_runtime_bridge_helper_contract_summary.json`
   - `tmp/reports/m267/M267-D002/live_error_runtime_integration_summary.json`
   - `tmp/reports/m267/M267-D003/cross_module_error_surface_preservation_summary.json`
   - `tmp/reports/m267/M267-E001/error_model_conformance_gate_summary.json`
4. The checker must reject drift if any upstream summary disappears, stops
   reporting successful coverage, or drops the dynamic-proof indicator that
   keeps the earlier M267 chain honest.
5. `package.json` must wire:
   - `check:objc3c:m267-e002-runnable-throws-result-and-bridge-matrix-cross-lane-integration-sync`
   - `test:tooling:m267-e002-runnable-throws-result-and-bridge-matrix-cross-lane-integration-sync`
   - `check:objc3c:m267-e002-lane-e-readiness`
6. The closeout must explicitly hand off from the `M267-E001` gate marker to
   `M268-A001`.

## Canonical models

- Evidence model:
  `a001-through-e001-summary-chain-runnable-part6-closeout`
- Closeout model:
  `lane-e-closeout-replays-implemented-part6-slice-without-surface-widening`
- Failure model:
  `fail-closed-on-runnable-part6-closeout-drift`

## Non-goals

- No new runtime semantics.
- No new private helper ABI.
- No generalized foreign exception transport.
- No additional runtime probe beyond the already-landed upstream proofs.

## Evidence

- `tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json`
