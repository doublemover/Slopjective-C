# M249 IR/Object Packaging and Symbol Policy Conformance Matrix Implementation Expectations (C009)

Contract ID: `objc3c-ir-object-packaging-symbol-policy-conformance-matrix-implementation/m249-c009-v1`
Status: Accepted
Dependencies: `M249-C008`
Scope: lane-C IR/object packaging and symbol policy conformance matrix implementation governance with fail-closed continuity from C008.

## Objective

Execute lane-C conformance matrix implementation governance for IR/object
packaging and symbol policy on top of C008 recovery and determinism hardening
assets so dependency continuity and readiness evidence remain deterministic and
fail-closed against drift.

## Dependency Scope

- Issue `#6924` defines canonical lane-C conformance matrix implementation scope.
- `M249-C008` assets remain mandatory prerequisites:
  - `docs/contracts/m249_ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_c008_expectations.md`
  - `scripts/check_m249_c008_ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m249_c008_ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_contract.py`
  - `spec/planning/compiler/m249/m249_c008_ir_object_packaging_and_symbol_policy_recovery_and_determinism_hardening_packet.md`
- C009 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m249/m249_c009_ir_object_packaging_and_symbol_policy_conformance_matrix_implementation_packet.md`
  - `scripts/check_m249_c009_ir_object_packaging_and_symbol_policy_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m249_c009_ir_object_packaging_and_symbol_policy_conformance_matrix_implementation_contract.py`

## Deterministic Invariants

1. Lane-C conformance matrix implementation dependency references remain
   explicit and fail closed when dependency tokens drift.
2. Readiness command chain enforces `M249-C008` before `M249-C009`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m249-c009-ir-object-packaging-symbol-policy-conformance-matrix-implementation-contract`.
- `package.json` includes
  `test:tooling:m249-c009-ir-object-packaging-symbol-policy-conformance-matrix-implementation-contract`.
- `package.json` includes `check:objc3c:m249-c009-lane-c-readiness`.
- Lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m249-c008-lane-c-readiness`
  - `check:objc3c:m249-c009-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m249_c009_ir_object_packaging_and_symbol_policy_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m249_c009_ir_object_packaging_and_symbol_policy_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m249-c009-lane-c-readiness`

## Evidence Path

- `tmp/reports/m249/M249-C009/ir_object_packaging_and_symbol_policy_conformance_matrix_implementation_contract_summary.json`
