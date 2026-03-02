# M226 Parse-Lowering Cross-Lane Integration Sync Expectations (C012)

Contract ID: `objc3c-parse-lowering-cross-lane-integration-sync-contract/m226-c012-v1`
Status: Accepted
Scope: Parse/lowering cross-lane integration synchronization in `native/objc3c/src/pipeline/*`.

## Objective

Expand C011 performance/quality guardrail closure with explicit cross-lane
integration consistency/readiness evidence so parse lowering fails closed when
parser replay, semantic handoff, lowering boundary, or guardrail invariants
drift.

## Required Invariants

1. Readiness surface tracks cross-lane integration evidence:
   - `toolchain_runtime_ga_operations_cross_lane_integration_consistent`
   - `toolchain_runtime_ga_operations_cross_lane_integration_ready`
   - `toolchain_runtime_ga_operations_cross_lane_integration_key`
2. Readiness builder computes and persists cross-lane integration deterministically:
   - `IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsCrossLaneIntegrationKey(...)`
3. Cross-lane key evidence is folded into parse-lowering guardrail and
   long-tail conformance key surfaces.
4. Integration closeout and gate sign-off remain fail-closed and require
   cross-lane integration consistency/readiness.
5. Readiness failure reasons include:
   - `toolchain/runtime GA operations cross-lane integration is inconsistent`
   - `toolchain/runtime GA operations cross-lane integration is not ready`
6. Manifest projection includes cross-lane integration fields under
   `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c012_parse_lowering_cross_lane_integration_sync_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c012_parse_lowering_cross_lane_integration_sync_contract.py -q`
- `python scripts/check_m226_c011_parse_lowering_performance_quality_guardrails_contract.py`

## Evidence Path

- `tmp/reports/m226/m226_c012_parse_lowering_cross_lane_integration_sync_contract_summary.json`
