# M246 Toolchain Integration and Optimization Controls Modular Split and Scaffolding Expectations (D002)

Contract ID: `objc3c-toolchain-integration-optimization-controls-modular-split-and-scaffolding/m246-d002-v1`
Status: Accepted
Scope: M246 lane-D toolchain integration and optimization controls modular split and scaffolding continuity for deterministic optimizer pipeline governance.

## Objective

Fail closed unless M246 lane-D toolchain integration and optimization controls
modular split and scaffolding anchors remain explicit, deterministic, and
traceable across dependency surfaces, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5107` defines canonical lane-D modular split and scaffolding scope.
- Dependencies: `M246-D001`
- Prerequisite contract freeze assets from `M246-D001` remain mandatory:
  - `docs/contracts/m246_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_d001_expectations.md`
  - `spec/planning/compiler/m246/m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_contract.py`
  - `tests/tooling/test_check_m246_d001_toolchain_integration_and_optimization_controls_contract_and_architecture_freeze_contract.py`
- Packet/checker/test/readiness assets for `M246-D002` remain mandatory:
  - `spec/planning/compiler/m246/m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_packet.md`
  - `scripts/check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py`
  - `tests/tooling/test_check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py`
  - `scripts/run_m246_d002_lane_d_readiness.py`
- Forward-compatibility anchors for the existing lane-D chain remain explicit:
  - `scripts/run_m246_d003_lane_d_readiness.py`
  - `scripts/run_m246_d004_lane_d_readiness.py`
  - `scripts/run_m246_d005_lane_d_readiness.py`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_and_scaffolding_contract.py -q`
- `python scripts/run_m246_d002_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m246/M246-D002/toolchain_integration_optimization_controls_modular_split_and_scaffolding_contract_summary.json`
