# M244 Interop Lowering and ABI Conformance Recovery and Determinism Hardening Expectations (C008)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-recovery-and-determinism-hardening/m244-c008-v1`
Status: Accepted
Dependencies: `M244-C007`
Scope: lane-C interop lowering/ABI recovery and determinism hardening governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-C recovery and determinism hardening governance for interop lowering and ABI
conformance on top of C007 diagnostics hardening assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6557` defines canonical lane-C recovery and determinism hardening scope.
- `M244-C007` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_lowering_and_abi_conformance_diagnostics_hardening_c007_expectations.md`
  - `spec/planning/compiler/m244/m244_c007_interop_lowering_and_abi_conformance_diagnostics_hardening_packet.md`
  - `scripts/check_m244_c007_interop_lowering_and_abi_conformance_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m244_c007_interop_lowering_and_abi_conformance_diagnostics_hardening_contract.py`

## Deterministic Invariants

1. lane-C recovery and determinism hardening dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-C007` before `M244-C008`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-c008-interop-lowering-abi-conformance-recovery-determinism-hardening-contract`.
- `package.json` includes
  `test:tooling:m244-c008-interop-lowering-abi-conformance-recovery-determinism-hardening-contract`.
- `package.json` includes `check:objc3c:m244-c008-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-c007-lane-c-readiness`
  - `check:objc3c:m244-c008-lane-c-readiness`

## Validation

- `python scripts/check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py`
- `python scripts/check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c008_interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m244-c008-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/M244-C008/interop_lowering_and_abi_conformance_recovery_and_determinism_hardening_contract_summary.json`

