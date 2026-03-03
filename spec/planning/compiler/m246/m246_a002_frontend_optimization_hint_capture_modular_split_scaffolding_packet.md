# M246-A002 Frontend Optimization Hint Capture Modular Split/Scaffolding Packet

Packet: `M246-A002`
Milestone: `M246`
Lane: `A`
Freeze date: `2026-03-03`
Dependencies: `M246-A001`

## Purpose

Freeze lane-A modular split/scaffolding prerequisites for M246 frontend optimization hint capture continuity so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m246_frontend_optimization_hint_capture_modular_split_scaffolding_a002_expectations.md`
- Checker:
  `scripts/check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py`
- Dependency anchors from `M246-A001`:
  - `docs/contracts/m246_frontend_optimization_hint_capture_contract_and_architecture_freeze_a001_expectations.md`
  - `spec/planning/compiler/m246/m246_a001_frontend_optimization_hint_capture_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m246_a001_frontend_optimization_hint_capture_contract_and_architecture_freeze_contract.py`
  - `tests/tooling/test_check_m246_a001_frontend_optimization_hint_capture_contract_and_architecture_freeze_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m246_a002_frontend_optimization_hint_capture_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m246-a002-lane-a-readiness`

## Evidence Output

- `tmp/reports/m246/M246-A002/frontend_optimization_hint_capture_modular_split_scaffolding_summary.json`
