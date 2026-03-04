# M246-D008 Toolchain Integration and Optimization Controls Recovery and Determinism Hardening Packet

Packet: `M246-D008`
Milestone: `M246`
Lane: `D`
Issue: `#6687`
Freeze date: `2026-03-04`
Dependencies: `M246-D007`

## Purpose

Freeze lane-D toolchain integration and optimization controls recovery and
determinism hardening prerequisites for M246 so predecessor continuity remains explicit,
deterministic, and fail-closed, including code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m246_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_d008_expectations.md`
- Checker:
  `scripts/check_m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_contract.py`
- Readiness runner:
  `scripts/run_m246_d008_lane_d_readiness.py`
- Dependency anchors from `M246-D007`:
  - `docs/contracts/m246_toolchain_integration_and_optimization_controls_diagnostics_hardening_d007_expectations.md`
  - `spec/planning/compiler/m246/m246_d007_toolchain_integration_and_optimization_controls_diagnostics_hardening_packet.md`
  - `scripts/check_m246_d007_toolchain_integration_and_optimization_controls_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m246_d007_toolchain_integration_and_optimization_controls_diagnostics_hardening_contract.py`
  - `scripts/run_m246_d007_lane_d_readiness.py`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Readiness Chain

- `D007 readiness -> D008 checker -> D008 pytest`

## Gate Commands

- `python scripts/check_m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m246_d008_toolchain_integration_and_optimization_controls_recovery_and_determinism_hardening_contract.py -q`
- `python scripts/run_m246_d008_lane_d_readiness.py`

## Evidence Output

- `tmp/reports/m246/M246-D008/toolchain_integration_optimization_controls_recovery_and_determinism_hardening_contract_summary.json`

