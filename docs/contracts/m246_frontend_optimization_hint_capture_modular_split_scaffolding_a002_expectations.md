# M246 Frontend Optimization Hint Capture Modular Split/Scaffolding Expectations (A002)

Contract ID: `objc3c-frontend-optimization-hint-capture-modular-split-scaffolding/m246-a002-v1`
Status: Accepted
Scope: M246 lane-A modular split/scaffolding continuity for frontend optimization hint capture dependency wiring.

## Objective

Fail closed unless lane-A modular split/scaffolding dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Issue Anchor

- Issue: `#5049`

## Dependency Scope

- Dependencies: `M246-A001`
- M246-A001 freeze anchors remain mandatory prerequisites:
  - `docs/contracts/m246_frontend_optimization_hint_capture_contract_and_architecture_freeze_a001_expectations.md`
  - `spec/planning/compiler/m246/m246_a001_frontend_optimization_hint_capture_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m246_a001_frontend_optimization_hint_capture_contract_and_architecture_freeze_contract.py`
  - `tests/tooling/test_check_m246_a001_frontend_optimization_hint_capture_contract_and_architecture_freeze_contract.py`
- Packet/checker/test assets for A002 remain mandatory:
  - `spec/planning/compiler/m246/m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_packet.md`
  - `scripts/check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M246 lane-A A002 frontend optimization hint capture modular split/scaffolding anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A frontend optimization hint capture modular split/scaffolding fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A frontend optimization hint modular split metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m246-a002-frontend-optimization-hint-capture-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m246-a002-frontend-optimization-hint-capture-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m246-a002-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m246-a002-lane-a-readiness`

## Evidence Path

- `tmp/reports/m246/M246-A002/frontend_optimization_hint_capture_modular_split_scaffolding_summary.json`
