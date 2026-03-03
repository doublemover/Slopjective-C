# M246 IR Optimization Pass Wiring and Validation Modular Split/Scaffolding Expectations (C002)

Contract ID: `objc3c-ir-optimization-pass-wiring-validation-modular-split-scaffolding/m246-c002-v1`
Status: Accepted
Scope: M246 lane-C IR optimization pass wiring and validation modular split/scaffolding continuity for optimizer pipeline integration and invariants.

## Objective

Fail closed unless lane-C IR optimization pass wiring and validation modular split/scaffolding dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5078` defines canonical lane-C modular split/scaffolding scope.
- Dependencies: `M246-C001`
- Prerequisite assets from `M246-C001` remain mandatory:
  - `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m246/m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py`
  - `tests/tooling/test_check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M246 lane-C C002 IR optimization pass wiring and validation modular split/scaffolding fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C IR optimization pass wiring and validation modular split/scaffolding fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C IR optimization pass wiring modular split metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m246-c002-ir-optimization-pass-wiring-validation-modular-split-scaffolding-contract`.
- `package.json` includes `test:tooling:m246-c002-ir-optimization-pass-wiring-validation-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m246-c002-lane-c-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m246-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m246/M246-C002/ir_optimization_pass_wiring_validation_modular_split_scaffolding_summary.json`
