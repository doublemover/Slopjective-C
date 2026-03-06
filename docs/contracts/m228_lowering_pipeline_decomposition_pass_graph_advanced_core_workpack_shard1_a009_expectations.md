# M228 Lowering Pipeline Decomposition and Pass-Graph Advanced Core Workpack (Shard 1) Expectations (A015)

Contract ID: `objc3c-lowering-pipeline-pass-graph-advanced-core-workpack-shard1/m228-a015-v1`
Status: Accepted
Scope: lane-A advanced core workpack (shard 1) closure after A014 release-candidate replay dry-run.

## Objective

Lock deterministic shard-1 advanced-core readiness wiring for parse->lowering
handoff so toolchain/runtime GA advanced-core continuity fails closed before
advanced edge/diagnostics/conformance shards.

## Deterministic Invariants

- Dependencies: `M228-A014`

1. Parse/lowering readiness surface exports advanced-core shard1 gates:
   - `toolchain_runtime_ga_operations_advanced_core_consistent`
   - `toolchain_runtime_ga_operations_advanced_core_ready`
   - `toolchain_runtime_ga_operations_advanced_core_key`
2. Surface synthesis computes and wires advanced-core shard1 continuity via:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreKey(...)`
3. Fail-closed diagnostics include:
   - `toolchain/runtime GA operations advanced core workpack is inconsistent`
   - `toolchain/runtime GA operations advanced core workpack is not ready`
4. Manifest/readiness artifact serialization includes:
   - `toolchain_runtime_ga_operations_advanced_core_consistent`
   - `toolchain_runtime_ga_operations_advanced_core_ready`
   - `toolchain_runtime_ga_operations_advanced_core_key`
5. Lane-A readiness wiring is pinned to A014 and A015 checks/tests.

## Build and Readiness Integration

Shared-file deltas required for full lane-A readiness:

- `package.json`
  - add `check:objc3c:m228-a015-lowering-pipeline-pass-graph-advanced-core-workpack-shard1-contract`
  - add `test:tooling:m228-a015-lowering-pipeline-pass-graph-advanced-core-workpack-shard1-contract`
  - add `check:objc3c:m228-a015-lane-a-readiness`
- `docs/runbooks/m228_wave_execution_runbook.md`
  - add A015 contract/command anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-A A015 advanced core shard1 anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add fail-closed advanced-core shard1 wiring language
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic advanced-core shard1 metadata anchor language

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Validation

- `python scripts/check_m228_a015_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a015_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m228-a015-lane-a-readiness`

## Evidence Path

- `tmp/reports/m228/M228-A015/advanced_core_workpack_shard1_contract_summary.json`
