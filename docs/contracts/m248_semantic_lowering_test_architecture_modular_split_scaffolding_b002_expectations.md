# M248 Semantic/Lowering Test Architecture Modular Split Scaffolding Expectations (B002)

Contract ID: `objc3c-semantic-lowering-test-architecture-modular-split-scaffolding/m248-b002-v1`
Status: Accepted
Scope: M248 lane-B semantic/lowering test architecture modular split/scaffolding continuity for deterministic CI sharding and replay governance.

## Objective

Fail closed unless M248 lane-B semantic/lowering test architecture modular
split/scaffolding anchors remain explicit, deterministic, and traceable across
dependency surfaces, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M248-B001`
- Prerequisite frozen assets from `M248-B001` remain mandatory:
  - `docs/contracts/m248_semantic_lowering_test_architecture_contract_freeze_b001_expectations.md`
  - `spec/planning/compiler/m248/m248_b001_semantic_lowering_test_architecture_contract_freeze_packet.md`
  - `scripts/check_m248_b001_semantic_lowering_test_architecture_contract.py`
  - `tests/tooling/test_check_m248_b001_semantic_lowering_test_architecture_contract.py`
- Packet/checker/test assets for `M248-B002` remain mandatory:
  - `spec/planning/compiler/m248/m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_packet.md`
  - `scripts/check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves the explicit lane-B `M248-B001`
  semantic/lowering architecture dependency anchor.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-B semantic/lowering
  fail-closed governance wording required by `M248-B001`.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-B
  semantic/lowering metadata anchor wording for `M248-B001`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-b002-semantic-lowering-test-architecture-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m248-b002-semantic-lowering-test-architecture-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m248-b002-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m248-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m248/M248-B002/semantic_lowering_test_architecture_modular_split_scaffolding_contract_summary.json`
