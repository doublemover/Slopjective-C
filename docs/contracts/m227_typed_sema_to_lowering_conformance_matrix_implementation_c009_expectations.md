# M227 Typed Sema-to-Lowering Conformance Matrix Implementation Expectations (C009)

Contract ID: `objc3c-typed-sema-to-lowering-conformance-matrix-implementation/m227-c009-v1`
Status: Accepted
Scope: typed sema-to-lowering conformance matrix implementation on top of C008 recovery/determinism hardening.

## Objective

Execute issue `#5129` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with conformance-matrix consistency and readiness
invariants, with deterministic fail-closed alignment checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C008`
- `M227-C008` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_recovery_determinism_hardening_c008_expectations.md`
  - `scripts/check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
  - `spec/planning/compiler/m227/m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries conformance matrix fields:
   - `typed_conformance_matrix_consistent`
   - `typed_conformance_matrix_ready`
   - `typed_conformance_matrix_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed conformance fields:
   - `typed_sema_conformance_matrix_consistent`
   - `typed_sema_conformance_matrix_ready`
   - `typed_sema_conformance_matrix_key`
3. Parse/lowering readiness fails closed when typed conformance-matrix
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c009-typed-sema-to-lowering-conformance-matrix-implementation-contract`
  - `test:tooling:m227-c009-typed-sema-to-lowering-conformance-matrix-implementation-contract`
  - `check:objc3c:m227-c009-lane-c-readiness`
- lane-C readiness chaining preserves C008 continuity:
  - `scripts/check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
  - `check:objc3c:m227-c009-lane-c-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-C C009
  conformance-matrix implementation anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C C009 fail-closed
  conformance matrix governance text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C C009
  conformance matrix metadata anchors.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
- `python scripts/check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c009_typed_sema_to_lowering_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m227-c009-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C009/typed_sema_to_lowering_conformance_matrix_implementation_contract_summary.json`
