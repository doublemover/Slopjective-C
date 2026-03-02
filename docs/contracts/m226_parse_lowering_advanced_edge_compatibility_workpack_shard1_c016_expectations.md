# M226 Parse-Lowering Advanced Edge Compatibility Workpack (Shard 1) Expectations (C016)

Contract ID: `objc3c-parse-lowering-advanced-edge-compatibility-workpack-shard1-contract/m226-c016-v1`
Status: Accepted
Scope: Parse/lowering advanced edge-compatibility workpack shard-1 synchronization in `native/objc3c/src/pipeline/*`.

## Objective

Expand C015 advanced-core closure with explicit advanced edge-compatibility
consistency/readiness evidence so parse lowering fails closed when advanced-core
handoff continuity or edge-compatibility key determinism drifts.

## Required Invariants

1. Readiness surface tracks advanced edge-compatibility shard-1 evidence:
   - `toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent`
   - `toolchain_runtime_ga_operations_advanced_edge_compatibility_ready`
   - `toolchain_runtime_ga_operations_advanced_edge_compatibility_key`
2. Readiness builder computes advanced edge-compatibility shard-1 deterministically:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedEdgeCompatibilityKey(...)`
3. Edge-compatibility key evidence is folded back into integration-closeout and
   parse-lowering guardrail key surfaces.
4. Integration closeout and gate sign-off remain fail-closed and require
   advanced edge-compatibility shard-1 consistency/readiness.
5. Readiness failure reasons include:
   - `toolchain/runtime GA operations advanced edge compatibility workpack is inconsistent`
   - `toolchain/runtime GA operations advanced edge compatibility workpack is not ready`
6. Manifest projection includes advanced edge-compatibility shard-1 fields under
   `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c016_parse_lowering_advanced_edge_compatibility_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c016_parse_lowering_advanced_edge_compatibility_workpack_shard1_contract.py -q`
- `python scripts/check_m226_c015_parse_lowering_advanced_core_workpack_shard1_contract.py`

## Evidence Path

- `tmp/reports/m226/m226_c016_parse_lowering_advanced_edge_compatibility_workpack_shard1_contract_summary.json`
