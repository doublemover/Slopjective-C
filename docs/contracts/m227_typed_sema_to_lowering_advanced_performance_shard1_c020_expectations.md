# M227 Typed Sema-to-Lowering Advanced Performance Workpack (Shard 1) Expectations (C020)

Contract ID: `objc3c-typed-sema-to-lowering-advanced-performance-shard1/m227-c020-v1`
Status: Accepted
Scope: typed sema-to-lowering advanced performance workpack (shard 1) on top of C019 advanced integration shard 1.

## Objective

Execute issue `#5140` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with advanced-performance-shard1
consistency/readiness invariants, with deterministic fail-closed alignment
checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C019`
- `M227-C019` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_advanced_integration_shard1_c019_expectations.md`
  - `scripts/check_m227_c019_typed_sema_to_lowering_advanced_integration_shard1_contract.py`
  - `tests/tooling/test_check_m227_c019_typed_sema_to_lowering_advanced_integration_shard1_contract.py`
  - `spec/planning/compiler/m227/m227_c019_typed_sema_to_lowering_advanced_integration_shard1_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed advanced-performance-shard1 fields:
   - `typed_advanced_performance_shard1_consistent`
   - `typed_advanced_performance_shard1_ready`
   - `typed_advanced_performance_shard1_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed advanced-performance-shard1 fields:
   - `typed_sema_advanced_performance_shard1_consistent`
   - `typed_sema_advanced_performance_shard1_ready`
   - `typed_sema_advanced_performance_shard1_key`
3. Parse/lowering readiness fails closed when typed advanced-performance-shard1
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c020-typed-sema-to-lowering-advanced-performance-shard1-contract`
  - `test:tooling:m227-c020-typed-sema-to-lowering-advanced-performance-shard1-contract`
  - `check:objc3c:m227-c020-lane-c-readiness`

## Validation

- `python scripts/check_m227_c019_typed_sema_to_lowering_advanced_integration_shard1_contract.py`
- `python scripts/check_m227_c020_typed_sema_to_lowering_advanced_performance_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c020_typed_sema_to_lowering_advanced_performance_shard1_contract.py -q`
- `npm run check:objc3c:m227-c020-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C020/typed_sema_to_lowering_advanced_performance_shard1_contract_summary.json`




