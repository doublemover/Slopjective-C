# M227 Semantic Pass Advanced Conformance Workpack (Shard 1) Expectations (A018)

Contract ID: `objc3c-semantic-pass-advanced-conformance-workpack-shard1/m227-a018-v1`
Status: Accepted
Scope: lane-A advanced conformance workpack (shard 1) closure after A015 advanced-core shard 1.

## Objective

Lock deterministic shard-1 advanced conformance readiness wiring for
parse to lowering handoff so semantic-pass edge compatibility continuity fails
closed before advanced conformance/conformance shards.

## Deterministic Invariants

- Dependencies: `M227-A017`

1. Parse/lowering readiness surface exports advanced conformance shard1 gates:
   - `toolchain_runtime_ga_operations_advanced_conformance_consistent`
   - `toolchain_runtime_ga_operations_advanced_conformance_ready`
   - `toolchain_runtime_ga_operations_advanced_conformance_key`
2. Surface synthesis computes and wires advanced conformance shard1 continuity via:
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceConsistent(...)`
   - `IsObjc3ToolchainRuntimeGaOperationsAdvancedConformanceReady(...)`
   - `BuildObjc3ToolchainRuntimeGaOperationsAdvancedConformanceKey(...)`
3. Fail-closed conformance include:
   - `toolchain/runtime GA operations advanced conformance workpack is inconsistent`
   - `toolchain/runtime GA operations advanced conformance workpack is not ready`
4. Manifest/readiness artifact serialization includes:
   - `toolchain_runtime_ga_operations_advanced_conformance_consistent`
   - `toolchain_runtime_ga_operations_advanced_conformance_ready`
   - `toolchain_runtime_ga_operations_advanced_conformance_key`
5. Lane-A readiness wiring is pinned to A017 and A018 checks/tests.

## Build and Readiness Integration

Shared-file deltas required for full lane-A readiness:

- `package.json`
  - add `check:objc3c:m227-a018-semantic-pass-advanced-conformance-workpack-shard1-contract`
  - add `test:tooling:m227-a018-semantic-pass-advanced-conformance-workpack-shard1-contract`
  - add `check:objc3c:m227-a018-lane-a-readiness`
- `docs/runbooks/m227_wave_execution_runbook.md`
  - add A018 contract/command anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M227 lane-A A018 advanced conformance shard1 anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add fail-closed advanced conformance shard1 wiring language
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic advanced conformance shard1 metadata anchor language

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `check:objc3c:m227-a014-milestone-optimization-replay-proof`

## Validation

- `python scripts/check_m227_a018_semantic_pass_advanced_conformance_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a018_semantic_pass_advanced_conformance_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m227-a018-lane-a-readiness`

## Evidence Path

- `tmp/reports/m227/M227-A018/advanced_conformance_workpack_shard1_contract_summary.json`


