# M235 Qualified Type Lowering and ABI Representation Integration Closeout and Gate Sign-off Expectations (C020)

Contract ID: `objc3c-qualified-type-lowering-and-abi-representation-integration-closeout-and-gate-sign-off/m235-c020-v1`
Status: Accepted
Dependencies: `M235-C019`
Scope: M235 lane-C qualified type lowering and ABI representation integration closeout and gate sign-off continuity for deterministic dependency-chain governance.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
integration closeout and gate sign-off anchors remain explicit, deterministic, and traceable
across dependency-chain surfaces. Code/spec anchors and milestone
optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5830` defines canonical lane-C integration closeout and gate sign-off scope.
- Dependencies: `M235-C019`
- M235-C019 advanced integration workpack (shard 1) anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_advanced_integration_workpack_shard_1_c019_expectations.md`
  - `spec/planning/compiler/m235/m235_c019_qualified_type_lowering_and_abi_representation_advanced_integration_workpack_shard_1_packet.md`
  - `scripts/check_m235_c019_qualified_type_lowering_and_abi_representation_advanced_integration_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m235_c019_qualified_type_lowering_and_abi_representation_advanced_integration_workpack_shard_1_contract.py`
- Packet/checker/test assets for C020 remain mandatory:
  - `spec/planning/compiler/m235/m235_c020_qualified_type_lowering_and_abi_representation_integration_closeout_and_gate_sign_off_packet.md`
  - `scripts/check_m235_c020_qualified_type_lowering_and_abi_representation_integration_closeout_and_gate_sign_off_contract.py`
  - `tests/tooling/test_check_m235_c020_qualified_type_lowering_and_abi_representation_integration_closeout_and_gate_sign_off_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit M235 lane-C C019
  qualified type lowering and ABI representation integration closeout and gate sign-off anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C qualified type
  lowering and ABI representation integration closeout and gate sign-off fail-closed dependency wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  qualified type lowering and ABI representation integration closeout and gate sign-off metadata wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m235-c019-lane-c-readiness`.
- `package.json` includes
  `check:objc3c:m235-c020-qualified-type-lowering-and-abi-representation-integration-closeout-and-gate-sign-off-contract`.
- `package.json` includes
  `test:tooling:m235-c020-qualified-type-lowering-and-abi-representation-integration-closeout-and-gate-sign-off-contract`.
- `package.json` includes `check:objc3c:m235-c020-lane-c-readiness`.
- Readiness dependency chain order: `C019 readiness -> C020 checker -> C020 pytest`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m235_c020_qualified_type_lowering_and_abi_representation_integration_closeout_and_gate_sign_off_contract.py`
- `python -m pytest tests/tooling/test_check_m235_c020_qualified_type_lowering_and_abi_representation_integration_closeout_and_gate_sign_off_contract.py -q`
- `npm run check:objc3c:m235-c020-lane-c-readiness`

## Evidence Path

- `tmp/reports/m235/M235-C020/qualified_type_lowering_and_abi_representation_integration_closeout_and_gate_sign_off_contract_summary.json`
















