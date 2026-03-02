# M226 Parse-Lowering Advanced Core Workpack (Shard 1) Expectations (C015)

Contract ID: `objc3c-parse-lowering-advanced-core-workpack-shard1-contract/m226-c015-v1`
Status: Accepted
Scope: Parse/lowering advanced core workpack shard-1 synchronization in `native/objc3c/src/pipeline/*`.

## Objective

Expand C014 release replay dry-run closure with explicit advanced-core shard-1
consistency/readiness evidence so parse lowering fails closed when docs/runbook
sync continuity or advanced-core key determinism drifts.

## Required Invariants

1. Readiness surface tracks advanced-core shard-1 evidence:
   - `toolchain_runtime_ga_operations_advanced_core_consistent`
   - `toolchain_runtime_ga_operations_advanced_core_ready`
   - `toolchain_runtime_ga_operations_advanced_core_key`
2. Readiness builder computes advanced-core shard-1 deterministically:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreKey(...)`
3. Advanced-core key evidence is folded back into integration-closeout and
   parse-lowering guardrail key surfaces.
4. Integration closeout and gate sign-off remain fail-closed and require
   advanced-core shard-1 consistency/readiness.
5. Readiness failure reasons include:
   - `toolchain/runtime GA operations advanced core workpack is inconsistent`
   - `toolchain/runtime GA operations advanced core workpack is not ready`
6. Manifest projection includes advanced-core shard-1 fields under
   `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c015_parse_lowering_advanced_core_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c015_parse_lowering_advanced_core_workpack_shard1_contract.py -q`
- `python scripts/check_m226_c014_parse_lowering_release_candidate_replay_dry_run_contract.py`

## Evidence Path

- `tmp/reports/m226/m226_c015_parse_lowering_advanced_core_workpack_shard1_contract_summary.json`
