# M235 Qualified Type Lowering and ABI Representation Modular Split/Scaffolding Expectations (C002)

Contract ID: `objc3c-qualified-type-lowering-and-abi-representation-modular-split-scaffolding/m235-c002-v1`
Status: Accepted
Dependencies: `M235-C001`
Scope: M235 lane-C qualified type lowering and ABI representation modular split/scaffolding continuity for deterministic dependency-chain governance.

## Objective

Fail closed unless lane-C qualified type lowering and ABI representation
modular split/scaffolding anchors remain explicit, deterministic, and
traceable across dependency-chain surfaces. Code/spec anchors and milestone
optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#5812` defines canonical lane-C modular split and scaffolding scope.
- Dependencies: `M235-C001`
- M235-C001 freeze anchors remain mandatory prerequisites:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m235/m235_c001_qualified_type_lowering_and_abi_representation_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m235_c001_qualified_type_lowering_and_abi_representation_contract.py`
  - `tests/tooling/test_check_m235_c001_qualified_type_lowering_and_abi_representation_contract.py`
- Packet/checker/test assets for C002 remain mandatory:
  - `spec/planning/compiler/m235/m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_packet.md`
  - `scripts/check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit M235 lane-C C001
  qualified type lowering and ABI representation contract-freeze anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C qualified type
  lowering and ABI representation fail-closed dependency wording inherited by
  C002.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  qualified type lowering and ABI representation metadata wording inherited by
  C002.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m235-c001-qualified-type-lowering-and-abi-representation-contract`.
- `package.json` includes
  `test:tooling:m235-c001-qualified-type-lowering-and-abi-representation-contract`.
- `package.json` includes `check:objc3c:m235-c001-lane-c-readiness`.
- Readiness dependency chain order: `C001 readiness -> C002 checker -> C002 pytest`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `npm run check:objc3c:m235-c001-lane-c-readiness`
- `python scripts/check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m235_c002_qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract.py -q`

## Evidence Path

- `tmp/reports/m235/M235-C002/qualified_type_lowering_and_abi_representation_modular_split_scaffolding_contract_summary.json`
