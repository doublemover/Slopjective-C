# M227 Typed Sema-to-Lowering Advanced Conformance Workpack (Shard 1) Expectations (C018)

Contract ID: `objc3c-typed-sema-to-lowering-advanced-conformance-shard1/m227-c018-v1`
Status: Accepted
Scope: typed sema-to-lowering advanced conformance workpack (shard 1) on top of C017 advanced diagnostics shard 1.

## Objective

Execute issue `#5138` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with advanced-conformance-shard1
consistency/readiness invariants, with deterministic fail-closed alignment
checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C017`
- `M227-C017` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_advanced_diagnostics_shard1_c017_expectations.md`
  - `scripts/check_m227_c017_typed_sema_to_lowering_advanced_diagnostics_shard1_contract.py`
  - `tests/tooling/test_check_m227_c017_typed_sema_to_lowering_advanced_diagnostics_shard1_contract.py`
  - `spec/planning/compiler/m227/m227_c017_typed_sema_to_lowering_advanced_diagnostics_shard1_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed advanced-conformance-shard1 fields:
   - `typed_advanced_conformance_shard1_consistent`
   - `typed_advanced_conformance_shard1_ready`
   - `typed_advanced_conformance_shard1_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed advanced-conformance-shard1 fields:
   - `typed_sema_advanced_conformance_shard1_consistent`
   - `typed_sema_advanced_conformance_shard1_ready`
   - `typed_sema_advanced_conformance_shard1_key`
3. Parse/lowering readiness fails closed when typed advanced-conformance-shard1
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c018-typed-sema-to-lowering-advanced-conformance-shard1-contract`
  - `test:tooling:m227-c018-typed-sema-to-lowering-advanced-conformance-shard1-contract`
  - `check:objc3c:m227-c018-lane-c-readiness`

## Validation

- `python scripts/check_m227_c017_typed_sema_to_lowering_advanced_diagnostics_shard1_contract.py`
- `python scripts/check_m227_c018_typed_sema_to_lowering_advanced_conformance_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c018_typed_sema_to_lowering_advanced_conformance_shard1_contract.py -q`
- `npm run check:objc3c:m227-c018-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C018/typed_sema_to_lowering_advanced_conformance_shard1_contract_summary.json`


