# M227 Typed Sema-to-Lowering Advanced Diagnostics Workpack (Shard 2) Expectations (C023)

Contract ID: `objc3c-typed-sema-to-lowering-advanced-diagnostics-shard2/m227-c023-v1`
Status: Accepted
Scope: typed sema-to-lowering advanced diagnostics workpack (shard 2) on top of C022 advanced edge compatibility shard 2.

## Objective

Execute issue `#5143` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with advanced-diagnostics-shard2
consistency/readiness invariants, with deterministic fail-closed alignment
checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C022`
- `M227-C022` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_advanced_edge_compatibility_shard2_c022_expectations.md`
  - `scripts/check_m227_c022_typed_sema_to_lowering_advanced_edge_compatibility_shard2_contract.py`
  - `tests/tooling/test_check_m227_c022_typed_sema_to_lowering_advanced_edge_compatibility_shard2_contract.py`
  - `spec/planning/compiler/m227/m227_c022_typed_sema_to_lowering_advanced_edge_compatibility_shard2_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed advanced-diagnostics-shard2 fields:
   - `typed_advanced_diagnostics_shard2_consistent`
   - `typed_advanced_diagnostics_shard2_ready`
   - `typed_advanced_diagnostics_shard2_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed advanced-diagnostics-shard2 fields:
   - `typed_sema_advanced_diagnostics_shard2_consistent`
   - `typed_sema_advanced_diagnostics_shard2_ready`
   - `typed_sema_advanced_diagnostics_shard2_key`
3. Parse/lowering readiness fails closed when typed advanced-diagnostics-shard2
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c023-typed-sema-to-lowering-advanced-diagnostics-shard2-contract`
  - `test:tooling:m227-c023-typed-sema-to-lowering-advanced-diagnostics-shard2-contract`
  - `check:objc3c:m227-c023-lane-c-readiness`

## Validation

- `python scripts/check_m227_c022_typed_sema_to_lowering_advanced_edge_compatibility_shard2_contract.py`
- `python scripts/check_m227_c023_typed_sema_to_lowering_advanced_diagnostics_shard2_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c023_typed_sema_to_lowering_advanced_diagnostics_shard2_contract.py -q`
- `npm run check:objc3c:m227-c023-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C023/typed_sema_to_lowering_advanced_diagnostics_shard2_contract_summary.json`





