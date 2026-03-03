# M244 Interop Lowering and ABI Conformance Edge-Case and Compatibility Completion Expectations (C005)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-edge-case-and-compatibility-completion/m244-c005-v1`
Status: Accepted
Dependencies: `M244-C004`
Scope: lane-C interop lowering/ABI edge-case and compatibility completion governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-C edge-case and compatibility completion governance for interop lowering and
ABI conformance on top of C004 core-feature expansion assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.

## Dependency Scope

- Issue `#6554` defines canonical lane-C edge-case and compatibility completion scope.
- `M244-C004` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_lowering_and_abi_conformance_core_feature_expansion_c004_expectations.md`
  - `spec/planning/compiler/m244/m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_packet.md`
  - `scripts/check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m244_c004_interop_lowering_and_abi_conformance_core_feature_expansion_contract.py`

## Deterministic Invariants

1. lane-C edge-case and compatibility completion dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `M244-C004` before `M244-C005`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-c005-interop-lowering-abi-conformance-edge-case-and-compatibility-completion-contract`.
- `package.json` includes
  `test:tooling:m244-c005-interop-lowering-abi-conformance-edge-case-and-compatibility-completion-contract`.
- `package.json` includes `check:objc3c:m244-c005-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-c004-lane-c-readiness`
  - `check:objc3c:m244-c005-lane-c-readiness`

## Validation

- `python scripts/check_m244_c005_interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract.py`
- `python scripts/check_m244_c005_interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c005_interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m244-c005-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/M244-C005/interop_lowering_and_abi_conformance_edge_case_and_compatibility_completion_contract_summary.json`

