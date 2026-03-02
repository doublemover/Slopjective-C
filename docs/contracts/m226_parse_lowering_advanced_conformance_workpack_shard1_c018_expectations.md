# M226 Parse-Lowering Advanced Conformance Workpack (Shard 1) Expectations (C018)

Contract ID: `objc3c-parse-lowering-advanced-conformance-workpack-shard1-contract/m226-c018-v1`
Status: Accepted
Scope: Parse/lowering advanced conformance workpack shard-1 synchronization in `native/objc3c/src/pipeline/*`.

## Objective

Expand C017 advanced diagnostics closure with explicit advanced conformance
consistency/readiness evidence so parse lowering fails closed when advanced
diagnostics continuity or conformance key determinism drifts.

## Required Invariants

1. Readiness surface tracks advanced conformance shard-1 evidence:
   - `toolchain_runtime_ga_operations_advanced_conformance_consistent`
   - `toolchain_runtime_ga_operations_advanced_conformance_ready`
   - `toolchain_runtime_ga_operations_advanced_conformance_key`
2. Readiness builder computes advanced conformance shard-1 deterministically:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedConformanceKey(...)`
3. Advanced conformance key evidence is folded back into integration-closeout
   and parse-lowering guardrail key surfaces.
4. Integration closeout and gate sign-off remain fail-closed and require
   advanced conformance shard-1 consistency/readiness.
5. Readiness failure reasons include:
   - `toolchain/runtime GA operations advanced conformance workpack is inconsistent`
   - `toolchain/runtime GA operations advanced conformance workpack is not ready`
6. Manifest projection includes advanced conformance shard-1 fields under
   `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c018_parse_lowering_advanced_conformance_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c018_parse_lowering_advanced_conformance_workpack_shard1_contract.py -q`
- `python scripts/check_m226_c017_parse_lowering_advanced_diagnostics_workpack_shard1_contract.py`

## Evidence Path

- `tmp/reports/m226/m226_c018_parse_lowering_advanced_conformance_workpack_shard1_contract_summary.json`
