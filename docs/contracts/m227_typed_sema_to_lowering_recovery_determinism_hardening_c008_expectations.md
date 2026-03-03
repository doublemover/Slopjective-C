# M227 Typed Sema-to-Lowering Recovery and Determinism Hardening Expectations (C008)

Contract ID: `objc3c-typed-sema-to-lowering-recovery-determinism-hardening/m227-c008-v1`
Status: Accepted
Scope: typed sema-to-lowering recovery and determinism hardening on top of C007 diagnostics hardening.

## Objective

Execute issue `#5128` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with recovery/determinism consistency and readiness
invariants, plus deterministic fail-closed alignment checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C007`
- `M227-C007` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_diagnostics_hardening_c007_expectations.md`
  - `scripts/check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
  - `spec/planning/compiler/m227/m227_c007_typed_sema_to_lowering_diagnostics_hardening_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries recovery/determinism fields:
   - `typed_recovery_determinism_consistent`
   - `typed_recovery_determinism_ready`
   - `typed_recovery_determinism_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed recovery fields:
   - `typed_sema_recovery_determinism_consistent`
   - `typed_sema_recovery_determinism_ready`
   - `typed_sema_recovery_determinism_key`
3. Parse/lowering readiness fails closed when typed recovery/determinism
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c008-typed-sema-to-lowering-recovery-determinism-hardening-contract`
  - `test:tooling:m227-c008-typed-sema-to-lowering-recovery-determinism-hardening-contract`
  - `check:objc3c:m227-c008-lane-c-readiness`
- lane-C readiness chaining preserves C007 continuity:
  - `scripts/check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
  - `check:objc3c:m227-c008-lane-c-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-C C008
  recovery/determinism hardening anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C C008 fail-closed
  recovery/determinism governance text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C C008
  recovery/determinism metadata anchors.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m227_c007_typed_sema_to_lowering_diagnostics_hardening_contract.py`
- `python scripts/check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c008_typed_sema_to_lowering_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m227-c008-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C008/typed_sema_to_lowering_recovery_determinism_hardening_contract_summary.json`
