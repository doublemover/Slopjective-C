# M246-D004 Toolchain Integration and Optimization Controls Core Feature Expansion Packet

Packet: `M246-D004`
Milestone: `M246`
Lane: `D`
Issue: `#5109`
Freeze date: `2026-03-04`
Dependencies: `M246-D003`

## Purpose

Freeze lane-D toolchain integration and optimization controls core feature
expansion prerequisites for M246 so predecessor continuity remains explicit,
deterministic, and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.
Contract checker failure reporting must remain deterministic/sorted, fail closed on read errors, and support canonical JSON emission via `--emit-json`.

## Scope Anchors

- Contract:
  `docs/contracts/m246_toolchain_integration_and_optimization_controls_core_feature_expansion_d004_expectations.md`
- Checker:
  `scripts/check_m246_d004_toolchain_integration_and_optimization_controls_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m246_d004_toolchain_integration_and_optimization_controls_core_feature_expansion_contract.py`
- Readiness runner:
  `scripts/run_m246_d004_lane_d_readiness.py`
- Dependency anchors from `M246-D003`:
  - `docs/contracts/m246_toolchain_integration_and_optimization_controls_core_feature_implementation_d003_expectations.md`
  - `spec/planning/compiler/m246/m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_packet.md`
  - `scripts/check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m246_d003_toolchain_integration_and_optimization_controls_core_feature_implementation_contract.py`
  - `scripts/run_m246_d003_lane_d_readiness.py`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m246_d004_toolchain_integration_and_optimization_controls_core_feature_expansion_contract.py --emit-json --summary-out tmp/reports/m246/M246-D004/toolchain_integration_optimization_controls_core_feature_expansion_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m246_d004_toolchain_integration_and_optimization_controls_core_feature_expansion_contract.py -q`
- `python scripts/run_m246_d004_lane_d_readiness.py`

## Evidence Output

- `tmp/reports/m246/M246-D004/toolchain_integration_optimization_controls_core_feature_expansion_contract_summary.json`
