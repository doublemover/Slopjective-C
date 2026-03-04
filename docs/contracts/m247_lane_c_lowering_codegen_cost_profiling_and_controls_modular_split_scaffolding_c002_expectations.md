# M247 Lane C Lowering/Codegen Cost Profiling and Controls Modular Split/Scaffolding Expectations (C002)

Contract ID: `objc3c-lane-c-lowering-codegen-cost-profiling-controls-modular-split-scaffolding-contract/m247-c002-v1`
Status: Accepted
Dependencies: `M247-C001`
Scope: M247 lane-C lowering/codegen cost profiling and controls modular split/scaffolding continuity for deterministic dependency-chain governance.

## Objective

Fail closed unless M247 lane-C lowering/codegen cost profiling and controls
modular split/scaffolding anchors remain explicit, deterministic, and traceable
across dependency-chain surfaces. Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6743` defines canonical lane-C modular split/scaffolding scope.
- Prerequisite contract-freeze assets from `M247-C001` remain mandatory:
  - `docs/contracts/m247_lane_c_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m247/m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py`
  - `tests/tooling/test_check_m247_c001_lowering_codegen_cost_profiling_and_controls_contract_and_architecture_freeze_contract.py`
- Packet/checker/test/readiness assets for `M247-C002` remain mandatory:
  - `spec/planning/compiler/m247/m247_c002_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_packet.md`
  - `scripts/check_m247_c002_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m247_c002_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract.py`
  - `scripts/run_m247_c002_lane_c_readiness.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit M247 lane-C C001
  lowering/codegen cost profiling and controls contract-freeze anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C lowering/codegen
  modular split/scaffolding fail-closed dependency wording inherited by C002.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  lowering/codegen modular split/scaffolding metadata wording inherited by
  C002.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m247-c002-lowering-codegen-cost-profiling-controls-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m247-c002-lowering-codegen-cost-profiling-controls-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m247-c002-lane-c-readiness`.
- `scripts/run_m247_c002_lane_c_readiness.py` chains predecessor readiness
  using:
  - `check:objc3c:m247-c001-lane-c-readiness`
  - `check:objc3c:m247-c002-lane-c-readiness`
- Readiness chain order: `C001 readiness -> C002 checker -> C002 pytest`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m247_c002_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m247_c002_lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract.py -q`
- `python scripts/run_m247_c002_lane_c_readiness.py`
- `npm run check:objc3c:m247-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m247/M247-C002/lowering_codegen_cost_profiling_and_controls_modular_split_scaffolding_contract_summary.json`
