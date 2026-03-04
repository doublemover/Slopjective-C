# M246-C008 IR Optimization Pass Wiring and Validation Recovery and Determinism Hardening Packet

Packet: `M246-C008`
Milestone: `M246`
Lane: `C`
Issue: `#5084`
Freeze date: `2026-03-04`
Dependencies: `M246-C007`

## Purpose

Freeze lane-C IR optimization pass wiring and validation recovery and determinism hardening
continuity for M246 so dependency surfaces and optimizer validation governance
remain deterministic and fail-closed, with code/spec anchors and milestone
optimization improvements treated as mandatory scope inputs.

## Issue Anchor

- Primary issue anchor: `#5084`
- Dependency-chain integrity for C008 remains pinned to issue `#5084` across
  expectations, checker, tests, and readiness surfaces.

## Scope Anchors

- Contract:
  `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_recovery_and_determinism_hardening_c008_expectations.md`
- Checker:
  `scripts/check_m246_c008_ir_optimization_pass_wiring_and_validation_recovery_and_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m246_c008_ir_optimization_pass_wiring_and_validation_recovery_and_determinism_hardening_contract.py`
- Readiness runner:
  `scripts/run_m246_c008_lane_c_readiness.py`

## Dependency Anchors

- Primary dependency: `M246-C007`
- Predecessor anchors inherited via `M246-C007`: `M246-C001`, `M246-C002`, `M246-C003`, `M246-C004`, `M246-C005`, `M246-C006`.
- Dependency anchors (`M246-C007`):
  - `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_diagnostics_hardening_c007_expectations.md`
  - `spec/planning/compiler/m246/m246_c007_ir_optimization_pass_wiring_and_validation_diagnostics_hardening_packet.md`
  - `scripts/check_m246_c007_ir_optimization_pass_wiring_and_validation_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m246_c007_ir_optimization_pass_wiring_and_validation_diagnostics_hardening_contract.py`
  - `scripts/run_m246_c007_lane_c_readiness.py`
- Inherited predecessor anchors (`M246-C001`):
  - `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m246/m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py`
  - `tests/tooling/test_check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py`
- Inherited predecessor anchors (`M246-C002`):
  - `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_c002_expectations.md`
  - `spec/planning/compiler/m246/m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_packet.md`
  - `scripts/check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m246_c008_ir_optimization_pass_wiring_and_validation_recovery_and_determinism_hardening_contract.py --emit-json --summary-out tmp/reports/m246/M246-C008/ir_optimization_pass_wiring_validation_recovery_and_determinism_hardening_summary.json`
- `python scripts/check_m246_c008_ir_optimization_pass_wiring_and_validation_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m246_c008_ir_optimization_pass_wiring_and_validation_recovery_and_determinism_hardening_contract.py -q`
- `python scripts/run_m246_c008_lane_c_readiness.py`

## Evidence Output

- `tmp/reports/m246/M246-C008/ir_optimization_pass_wiring_validation_recovery_and_determinism_hardening_summary.json`
