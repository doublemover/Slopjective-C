# M227 Typed Sema-to-Lowering Advanced Edge Compatibility Workpack (Shard 1) Expectations (C016)

Contract ID: `objc3c-typed-sema-to-lowering-advanced-edge-compatibility-shard1/m227-c016-v1`
Status: Accepted
Scope: typed sema-to-lowering advanced edge compatibility workpack (shard 1) on top of C015 advanced core shard 1.

## Objective

Execute issue `#5136` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with advanced-edge-compatibility-shard1
consistency/readiness invariants, with deterministic fail-closed alignment
checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C015`
- `M227-C015` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_advanced_core_shard1_c015_expectations.md`
  - `scripts/check_m227_c015_typed_sema_to_lowering_advanced_core_shard1_contract.py`
  - `tests/tooling/test_check_m227_c015_typed_sema_to_lowering_advanced_core_shard1_contract.py`
  - `spec/planning/compiler/m227/m227_c015_typed_sema_to_lowering_advanced_core_shard1_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed advanced-edge-compatibility-shard1 fields:
   - `typed_advanced_edge_compatibility_shard1_consistent`
   - `typed_advanced_edge_compatibility_shard1_ready`
   - `typed_advanced_edge_compatibility_shard1_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed advanced-edge-compatibility-shard1 fields:
   - `typed_sema_advanced_edge_compatibility_shard1_consistent`
   - `typed_sema_advanced_edge_compatibility_shard1_ready`
   - `typed_sema_advanced_edge_compatibility_shard1_key`
3. Parse/lowering readiness fails closed when typed advanced-edge-compatibility-shard1
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c016-typed-sema-to-lowering-advanced-edge-compatibility-shard1-contract`
  - `test:tooling:m227-c016-typed-sema-to-lowering-advanced-edge-compatibility-shard1-contract`
  - `check:objc3c:m227-c016-lane-c-readiness`

## Validation

- `python scripts/check_m227_c015_typed_sema_to_lowering_advanced_core_shard1_contract.py`
- `python scripts/check_m227_c016_typed_sema_to_lowering_advanced_edge_compatibility_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c016_typed_sema_to_lowering_advanced_edge_compatibility_shard1_contract.py -q`
- `npm run check:objc3c:m227-c016-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C016/typed_sema_to_lowering_advanced_edge_compatibility_shard1_contract_summary.json`
