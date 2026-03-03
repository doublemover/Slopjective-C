# M227 Semantic Pass Advanced Core Workpack (Shard 1) Expectations (A015)

Contract ID: `objc3c-semantic-pass-advanced-core-workpack-shard1/m227-a015-v1`
Status: Accepted
Scope: lane-A advanced core workpack (shard 1) closure after A014 release-candidate replay dry-run.

## Objective

Lock deterministic shard-1 advanced-core readiness wiring for parse to lowering
handoff so semantic-pass advanced-core continuity fails closed before advanced
edge/diagnostics/conformance shards.

## Deterministic Invariants

- Dependencies: `M227-A014`

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
  - add `check:objc3c:m227-a015-semantic-pass-advanced-core-workpack-shard1-contract`
  - add `test:tooling:m227-a015-semantic-pass-advanced-core-workpack-shard1-contract`
  - add `check:objc3c:m227-a015-lane-a-readiness`
- `docs/runbooks/m227_wave_execution_runbook.md`
  - add A015 contract/command anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M227 lane-A A015 advanced core shard1 anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add fail-closed advanced-core shard1 wiring language
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic advanced-core shard1 metadata anchor language

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `check:objc3c:m227-a014-milestone-optimization-replay-proof`

## Validation

- `python scripts/check_m227_a015_semantic_pass_advanced_core_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a015_semantic_pass_advanced_core_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m227-a015-lane-a-readiness`

## Evidence Path

- `tmp/reports/m227/M227-A015/advanced_core_workpack_shard1_contract_summary.json`
