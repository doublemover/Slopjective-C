# M226 Parse-Lowering Advanced Diagnostics Workpack (Shard 1) Expectations (C017)

Contract ID: `objc3c-parse-lowering-advanced-diagnostics-workpack-shard1-contract/m226-c017-v1`
Status: Accepted
Scope: Parse/lowering advanced diagnostics workpack shard-1 synchronization in `native/objc3c/src/pipeline/*`.

## Objective

Expand C016 advanced edge-compatibility closure with explicit advanced
diagnostics consistency/readiness evidence so parse lowering fails closed when
advanced edge-compatibility continuity or diagnostics key determinism drifts.

## Required Invariants

1. Readiness surface tracks advanced diagnostics shard-1 evidence:
   - `toolchain_runtime_ga_operations_advanced_diagnostics_consistent`
   - `toolchain_runtime_ga_operations_advanced_diagnostics_ready`
   - `toolchain_runtime_ga_operations_advanced_diagnostics_key`
2. Readiness builder computes advanced diagnostics shard-1 deterministically:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedDiagnosticsKey(...)`
3. Advanced diagnostics key evidence is folded back into integration-closeout
   and parse-lowering guardrail key surfaces.
4. Integration closeout and gate sign-off remain fail-closed and require
   advanced diagnostics shard-1 consistency/readiness.
5. Readiness failure reasons include:
   - `toolchain/runtime GA operations advanced diagnostics workpack is inconsistent`
   - `toolchain/runtime GA operations advanced diagnostics workpack is not ready`
6. Manifest projection includes advanced diagnostics shard-1 fields under
   `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c017_parse_lowering_advanced_diagnostics_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c017_parse_lowering_advanced_diagnostics_workpack_shard1_contract.py -q`
- `python scripts/check_m226_c016_parse_lowering_advanced_edge_compatibility_workpack_shard1_contract.py`

## Evidence Path

- `tmp/reports/m226/m226_c017_parse_lowering_advanced_diagnostics_workpack_shard1_contract_summary.json`
