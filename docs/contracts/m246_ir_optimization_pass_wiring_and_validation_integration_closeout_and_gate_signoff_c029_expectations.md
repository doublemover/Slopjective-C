# M246 IR Optimization Pass Wiring and Validation Advanced Performance Workpack (Shard 2) Expectations (C029)

Contract ID: `objc3c-ir-optimization-pass-wiring-validation-integration-closeout-and-gate-signoff/m246-c029-v1`
Status: Accepted
Scope: M246 lane-C IR optimization pass wiring and validation integration closeout and gate sign-off continuity with explicit `M246-C028` dependency governance and predecessor anchor integrity.

## Objective

Fail closed unless lane-C IR optimization pass wiring and validation
integration closeout and gate sign-off anchors remain explicit, deterministic,
and traceable across dependency surfaces, including code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#5105` defines canonical lane-C integration closeout and gate sign-off scope.
- Dependencies: `M246-C028`
- Predecessor anchors inherited via `M246-C028`: `M246-C001`, `M246-C002`, `M246-C003`, `M246-C004`, `M246-C005`, `M246-C006`, `M246-C007`, `M246-C008`, `M246-C009`, `M246-C010`, `M246-C011`, `M246-C012`.
- Upstream predecessor assets remain mandatory prerequisites:
  - `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m246/m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py`
  - `tests/tooling/test_check_m246_c001_ir_optimization_pass_wiring_and_validation_contract_and_architecture_freeze_contract.py`
  - `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_c002_expectations.md`
  - `spec/planning/compiler/m246/m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_packet.md`
  - `scripts/check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m246_c002_ir_optimization_pass_wiring_and_validation_modular_split_scaffolding_contract.py`
- C028 packet/checker/test/readiness assets remain mandatory prerequisites:
  - `docs/contracts/m246_ir_optimization_pass_wiring_and_validation_advanced_edge_compatibility_workpack_shard_3_c028_expectations.md`
  - `spec/planning/compiler/m246/m246_c028_ir_optimization_pass_wiring_and_validation_advanced_edge_compatibility_workpack_shard_3_packet.md`
  - `scripts/check_m246_c028_ir_optimization_pass_wiring_and_validation_advanced_edge_compatibility_workpack_shard_3_contract.py`
  - `tests/tooling/test_check_m246_c028_ir_optimization_pass_wiring_and_validation_advanced_edge_compatibility_workpack_shard_3_contract.py`
  - `scripts/run_m246_c028_lane_c_readiness.py`

## Build and Readiness Integration

- `scripts/run_m246_c029_lane_c_readiness.py` must preserve fail-closed dependency chaining:
  - `scripts/run_m246_c028_lane_c_readiness.py`
  - `scripts/check_m246_c029_ir_optimization_pass_wiring_and_validation_integration_closeout_and_gate_signoff_contract.py`
  - `tests/tooling/test_check_m246_c029_ir_optimization_pass_wiring_and_validation_integration_closeout_and_gate_signoff_contract.py`
- `package.json` must retain `check:objc3c:m246-c002-lane-c-readiness` as a continuity anchor.

## Milestone Optimization Inputs (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m246_c029_ir_optimization_pass_wiring_and_validation_integration_closeout_and_gate_signoff_contract.py`
- `python -m pytest tests/tooling/test_check_m246_c029_ir_optimization_pass_wiring_and_validation_integration_closeout_and_gate_signoff_contract.py -q`
- `python scripts/run_m246_c029_lane_c_readiness.py`

## Evidence Path

- `tmp/reports/m246/M246-C029/ir_optimization_pass_wiring_validation_integration_closeout_and_gate_signoff_summary.json`




