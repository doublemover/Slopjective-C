# M244 Interop Lowering and ABI Conformance Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-core-feature-implementation/m244-c003-v1`
Status: Accepted
Dependencies: `M244-C002`
Scope: lane-C interop lowering/ABI core-feature implementation governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-C core-feature implementation governance for interop lowering and
ABI conformance on top of C002 modular split/scaffolding assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6552` defines canonical lane-C core-feature implementation scope.
- `M244-C002` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_lowering_and_abi_conformance_modular_split_scaffolding_c002_expectations.md`
  - `spec/planning/compiler/m244/m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_packet.md`
  - `scripts/check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m244_c002_interop_lowering_and_abi_conformance_modular_split_scaffolding_contract.py`

## Deterministic Invariants

1. lane-C core-feature implementation dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-C002` before `M244-C003`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-c003-interop-lowering-abi-conformance-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m244-c003-interop-lowering-abi-conformance-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m244-c003-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-c002-lane-c-readiness`
  - `check:objc3c:m244-c003-lane-c-readiness`

## Validation

- `python scripts/check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py`
- `python scripts/check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c003_interop_lowering_and_abi_conformance_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m244-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/M244-C003/interop_lowering_and_abi_conformance_core_feature_implementation_contract_summary.json`

