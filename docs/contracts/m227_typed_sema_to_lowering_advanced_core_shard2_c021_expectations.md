# M227 Typed Sema-to-Lowering Advanced Core Workpack (Shard 2) Expectations (C021)

Contract ID: `objc3c-typed-sema-to-lowering-advanced-core-shard2/m227-c021-v1`
Status: Accepted
Scope: typed sema-to-lowering advanced core workpack (shard 2) on top of C020 advanced performance shard 1.

## Objective

Execute issue `#5141` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with advanced-core-shard2
consistency/readiness invariants, with deterministic fail-closed alignment
checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C020`
- `M227-C020` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_advanced_performance_shard1_c020_expectations.md`
  - `scripts/check_m227_c020_typed_sema_to_lowering_advanced_performance_shard1_contract.py`
  - `tests/tooling/test_check_m227_c020_typed_sema_to_lowering_advanced_performance_shard1_contract.py`
  - `spec/planning/compiler/m227/m227_c020_typed_sema_to_lowering_advanced_performance_shard1_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed advanced-core-shard2 fields:
   - `typed_advanced_core_shard2_consistent`
   - `typed_advanced_core_shard2_ready`
   - `typed_advanced_core_shard2_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed advanced-core-shard2 fields:
   - `typed_sema_advanced_core_shard2_consistent`
   - `typed_sema_advanced_core_shard2_ready`
   - `typed_sema_advanced_core_shard2_key`
3. Parse/lowering readiness fails closed when typed advanced-core-shard2
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c021-typed-sema-to-lowering-advanced-core-shard2-contract`
  - `test:tooling:m227-c021-typed-sema-to-lowering-advanced-core-shard2-contract`
  - `check:objc3c:m227-c021-lane-c-readiness`

## Validation

- `python scripts/check_m227_c020_typed_sema_to_lowering_advanced_performance_shard1_contract.py`
- `python scripts/check_m227_c021_typed_sema_to_lowering_advanced_core_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c021_typed_sema_to_lowering_advanced_core_shard2_contract.py -q`
- `npm run check:objc3c:m227-c021-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C021/typed_sema_to_lowering_advanced_core_shard2_contract_summary.json`





