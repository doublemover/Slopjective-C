# M244 Interop Lowering and ABI Conformance Conformance Matrix Implementation Expectations (C009)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-conformance-matrix-implementation/m244-c009-v1`
Status: Accepted
Dependencies: `M244-C008`
Scope: lane-C interop lowering/ABI conformance matrix implementation governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-C conformance matrix implementation governance for interop lowering and ABI
conformance on top of C008 recovery and determinism hardening assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6558` defines canonical lane-C conformance matrix implementation scope.
- `M244-C008` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_c008_expectations.md`
  - `spec/planning/compiler/m244/m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py`

## Deterministic Invariants

1. lane-C conformance matrix implementation dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-C008` before `M244-C009`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-c009-interop-lowering-abi-conformance-conformance-matrix-implementation-contract`.
- `package.json` includes
  `test:tooling:m244-c009-interop-lowering-abi-conformance-conformance-matrix-implementation-contract`.
- `package.json` includes `check:objc3c:m244-c009-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-c008-lane-c-readiness`
  - `check:objc3c:m244-c009-lane-c-readiness`

## Validation

- `python scripts/check_m244_c009_interop_lowering_and_abi_conformance_conformance_matrix_implementation_contract.py`
- `python scripts/check_m244_c009_interop_lowering_and_abi_conformance_conformance_matrix_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c009_interop_lowering_and_abi_conformance_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m244-c009-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/M244-C009/interop_lowering_and_abi_conformance_conformance_matrix_implementation_contract_summary.json`


