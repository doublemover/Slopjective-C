# M228 Lowering Pipeline Decomposition and Pass-Graph Integration Closeout and Gate Sign-off Expectations (A016)

Contract ID: `objc3c-lowering-pipeline-pass-graph-integration-closeout-gate-signoff/m228-a016-v1`
Status: Accepted
Scope: lane-A integration closeout and gate sign-off closure after A015 advanced-core shard1 readiness.

## Objective

Finalize lane-A pass-graph integration closeout by requiring deterministic
integration-closeout/sign-off consistency before reporting ready-for-lowering.

## Deterministic Invariants

- Dependencies: `M228-A015`

1. Parse/lowering readiness surface includes deterministic integration closeout
   sign-off guards:
   - `toolchain_runtime_ga_operations_integration_closeout_signoff_consistent`
   - `toolchain_runtime_ga_operations_integration_closeout_signoff_ready`
   - `toolchain_runtime_ga_operations_integration_closeout_signoff_key`
2. Parse/lowering readiness requires integration closeout sign-off and
   long-tail grammar gate sign-off before `ready_for_lowering`.
3. Frontend artifact manifest projects integration closeout sign-off booleans
   and key.
4. Failure reasons remain explicit and fail closed for integration closeout
   drift.

## Build and Readiness Integration

Shared-file deltas required for full lane-A readiness:

- `package.json`
  - add `check:objc3c:m228-a016-lowering-pipeline-pass-graph-integration-closeout-gate-signoff-contract`
  - add `test:tooling:m228-a016-lowering-pipeline-pass-graph-integration-closeout-gate-signoff-contract`
  - add `check:objc3c:m228-a016-lane-a-readiness`
- `docs/runbooks/m228_wave_execution_runbook.md`
  - add A016 contract/command anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-A A016 integration closeout and gate sign-off anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add fail-closed integration closeout/gate sign-off wiring language
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic integration closeout/gate sign-off metadata anchor language

## Validation

- `python scripts/check_m228_a016_lowering_pipeline_decomposition_pass_graph_integration_closeout_and_gate_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a016_lowering_pipeline_decomposition_pass_graph_integration_closeout_and_gate_signoff_contract.py -q`
- `npm run check:objc3c:m228-a016-lane-a-readiness`

## Evidence Path

- `tmp/reports/m228/M228-A016/integration_closeout_and_gate_signoff_contract_summary.json`
