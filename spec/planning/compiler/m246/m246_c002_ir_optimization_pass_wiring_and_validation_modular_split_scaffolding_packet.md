# M246-C002 IR Optimization Pass Wiring and Validation Modular Split/Scaffolding Packet

Packet: `M246-C002`
Milestone: `M246`
Lane: `C`
Issue: `#5078`
Freeze date: `2026-03-03`
Dependencies: `M246-C001`

## Purpose

Freeze lane-C IR optimization pass wiring and validation modular split/scaffolding prerequisites for M246 so optimizer pipeline integration and invariants governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_c002_expectations.md`
- Checker:
  `scripts/check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py`
- Dependency anchors from `M246-C001`:
  - `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m246/m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py`
  - `tests/tooling/test_check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m246-c002-ir-optimization-pass-wiring-validation-modular-split-scaffolding-contract`
  - `test:tooling:m246-c002-ir-optimization-pass-wiring-validation-modular-split-scaffolding-contract`
  - `check:objc3c:m246-c002-lane-c-readiness`
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

- `python scripts/check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m246-c002-lane-c-readiness`

## Evidence Output

- `tmp/reports/m246/M246-C002/ir_optimization_pass_wiring_validation_modular_split_scaffolding_summary.json`
