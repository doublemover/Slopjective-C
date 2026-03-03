# M227 Typed Sema-to-Lowering Advanced Diagnostics Workpack (Shard 1) Expectations (C017)

Contract ID: `objc3c-typed-sema-to-lowering-advanced-diagnostics-shard1/m227-c017-v1`
Status: Accepted
Scope: typed sema-to-lowering advanced diagnostics workpack (shard 1) on top of C016 advanced edge compatibility shard 1.

## Objective

Execute issue `#5137` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with advanced-diagnostics-shard1
consistency/readiness invariants, with deterministic fail-closed alignment
checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C016`
- `M227-C016` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_advanced_edge_compatibility_shard1_c016_expectations.md`
  - `scripts/check_m227_c016_typed_sema_to_lowering_advanced_edge_compatibility_shard1_contract.py`
  - `tests/tooling/test_check_m227_c016_typed_sema_to_lowering_advanced_edge_compatibility_shard1_contract.py`
  - `spec/planning/compiler/m227/m227_c016_typed_sema_to_lowering_advanced_edge_compatibility_shard1_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed advanced-diagnostics-shard1 fields:
   - `typed_advanced_diagnostics_shard1_consistent`
   - `typed_advanced_diagnostics_shard1_ready`
   - `typed_advanced_diagnostics_shard1_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed advanced-diagnostics-shard1 fields:
   - `typed_sema_advanced_diagnostics_shard1_consistent`
   - `typed_sema_advanced_diagnostics_shard1_ready`
   - `typed_sema_advanced_diagnostics_shard1_key`
3. Parse/lowering readiness fails closed when typed advanced-diagnostics-shard1
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c017-typed-sema-to-lowering-advanced-diagnostics-shard1-contract`
  - `test:tooling:m227-c017-typed-sema-to-lowering-advanced-diagnostics-shard1-contract`
  - `check:objc3c:m227-c017-lane-c-readiness`

## Validation

- `python scripts/check_m227_c016_typed_sema_to_lowering_advanced_edge_compatibility_shard1_contract.py`
- `python scripts/check_m227_c017_typed_sema_to_lowering_advanced_diagnostics_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c017_typed_sema_to_lowering_advanced_diagnostics_shard1_contract.py -q`
- `npm run check:objc3c:m227-c017-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C017/typed_sema_to_lowering_advanced_diagnostics_shard1_contract_summary.json`

