# M246 Toolchain Integration and Optimization Controls Core Feature Implementation Expectations (D003)

Contract ID: `objc3c-toolchain-integration-optimization-controls-core-feature-implementation/m246-d003-v1`
Status: Accepted
Scope: M246 lane-D toolchain integration and optimization controls core feature implementation continuity for deterministic optimizer pipeline governance.

## Objective

Fail closed unless M246 lane-D toolchain integration and optimization controls
core feature implementation anchors remain explicit, deterministic, and
traceable across dependency surfaces, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.
Checker outputs must remain deterministically sorted, with optional canonical JSON emission via `--emit-json`.

## Dependency Scope

- Issue `#5108` defines canonical lane-D core feature implementation scope.
- Dependencies: `M246-D002`
- `M246-D002` is a pending seeded lane-D modular split/scaffolding dependency token and must remain explicit until seed assets land.
- Seed target paths for `M246-D002` continuity anchors:
  - `docs/contracts/m246_toolchain_integration_and_optimization_controls_modular_split_scaffolding_d002_expectations.md`
  - `spec/planning/compiler/m246/m246_d002_toolchain_integration_and_optimization_controls_modular_split_scaffolding_packet.md`
  - `scripts/check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m246_d002_toolchain_integration_and_optimization_controls_modular_split_scaffolding_contract.py`
- Packet/checker/test/readiness assets for `M246-D003` remain mandatory:
  - `spec/planning/compiler/m246/m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_packet.md`
  - `scripts/check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py`
  - `scripts/run_m246_d003_lane_d_readiness.py`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py -q`
- `python scripts/run_m246_d003_lane_d_readiness.py`

## Evidence Path

- `tmp/reports/m246/M246-D003/toolchain_integration_optimization_controls_core_feature_implementation_contract_summary.json`
