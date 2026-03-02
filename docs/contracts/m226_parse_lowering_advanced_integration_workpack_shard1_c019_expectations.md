# M226 Parse-Lowering Advanced Integration Workpack (Shard 1) Expectations (C019)

Contract ID: `objc3c-parse-lowering-advanced-integration-workpack-shard1-contract/m226-c019-v1`
Status: Accepted
Scope: Parse/lowering advanced integration workpack shard-1 synchronization in `native/objc3c/src/pipeline/*`.

## Objective

Expand C018 advanced conformance closure with explicit advanced integration
consistency/readiness evidence so parse lowering fails closed when advanced
conformance continuity or integration key determinism drifts.

## Required Invariants

1. Readiness surface tracks advanced integration shard-1 evidence:
   - `toolchain_runtime_ga_operations_advanced_integration_consistent`
   - `toolchain_runtime_ga_operations_advanced_integration_ready`
   - `toolchain_runtime_ga_operations_advanced_integration_key`
2. Readiness builder computes advanced integration shard-1 deterministically:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedIntegrationKey(...)`
3. Advanced integration key evidence is folded back into integration-closeout
   and parse-lowering guardrail key surfaces.
4. Integration closeout and gate sign-off remain fail-closed and require
   advanced integration shard-1 consistency/readiness.
5. Readiness failure reasons include:
   - `toolchain/runtime GA operations advanced integration workpack is inconsistent`
   - `toolchain/runtime GA operations advanced integration workpack is not ready`
6. Manifest projection includes advanced integration shard-1 fields under
   `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c019_parse_lowering_advanced_integration_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c019_parse_lowering_advanced_integration_workpack_shard1_contract.py -q`
- `python scripts/check_m226_c018_parse_lowering_advanced_conformance_workpack_shard1_contract.py`

## Evidence Path

- `tmp/reports/m226/m226_c019_parse_lowering_advanced_integration_workpack_shard1_contract_summary.json`
