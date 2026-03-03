# M244 Interop Lowering and ABI Conformance Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-core-feature-expansion/m244-c004-v1`
Status: Accepted
Dependencies: `M244-C003`
Scope: lane-C interop lowering/ABI core-feature expansion governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-C core-feature expansion governance for interop lowering and
ABI conformance on top of C003 core-feature implementation assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6553` defines canonical lane-C core-feature expansion scope.
- `M244-C003` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_lowering_and_abi_conformance_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m244/m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_packet.md`
  - `scripts/check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py`

## Deterministic Invariants

1. lane-C core-feature expansion dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-C003` before `M244-C004`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-c004-interop-lowering-abi-conformance-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m244-c004-interop-lowering-abi-conformance-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m244-c004-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-c003-lane-c-readiness`
  - `check:objc3c:m244-c004-lane-c-readiness`

## Validation

- `python scripts/check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py`
- `python scripts/check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m244-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/M244-C004/interop_lowering_and_abi_conformance_core_feature_expansion_contract_summary.json`

