# M235 Qualified Type Lowering and ABI Representation Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-qualified-type-lowering-and-abi-representation-core-feature-implementation/m235-c003-v1`
Status: Accepted
Dependencies: `M235-C002`
Scope: M235 lane-C qualified type lowering and ABI representation core feature implementation continuity for deterministic dependency-chain governance.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
core feature implementation anchors remain explicit, deterministic, and
traceable across dependency-chain surfaces. Code/spec anchors and milestone
optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5813` defines canonical lane-C core feature implementation scope.
- Dependencies: `M235-C002`
- M235-C002 modular split/scaffolding anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_c002_expectations.md`
  - `spec/planning/compiler/m235/m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_packet.md`
  - `scripts/check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py`
- Packet/checker/test assets for C003 remain mandatory:
  - `spec/planning/compiler/m235/m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_packet.md`
  - `scripts/check_m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit M235 lane-C C002
  qualified type lowering and ABI representation modular split/scaffolding anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C qualified type
  lowering and ABI representation modular split/scaffolding fail-closed dependency wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  qualified type lowering and ABI representation modular split/scaffolding metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m235-c002-qualified-type-lowering-and-abi-representation-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m235-c002-qualified-type-lowering-and-abi-representation-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m235-c002-lane-c-readiness`.
- Readiness dependency chain order: `C002 readiness -> C003 checker -> C003 pytest`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `npm run check:objc3c:m235-c002-lane-c-readiness`
- `python scripts/check_m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m235_c003_qualified_type_lowering_and_abi_representation_core_feature_implementation_contract.py -q`

## Evidence Path

- `tmp/reports/m235/M235-C003/qualified_type_lowering_and_abi_representation_core_feature_implementation_contract_summary.json`
