# M232-C003 Message Send Lowering and Call Emission Core Feature Implementation Packet

Packet: `M232-C003`
Milestone: `M232`
Lane: `C`
Issue: `#5613`
Dependencies: `M232-C002`

## Scope

Freeze lane-C message send lowering and call emission core feature
implementation anchors so semantic-to-lowering continuity remains deterministic
and fail-closed on top of C002 modular split/scaffolding governance.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m232_message_send_lowering_and_call_emission_core_feature_implementation_c003_expectations.md`
- Operator runbook:
  `docs/runbooks/m232_wave_execution_runbook.md`
- Checker:
  `scripts/check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py`
- Dependency anchors (`M232-C002`):
  - `docs/contracts/m232_message_send_lowering_and_call_emission_modular_split_and_scaffolding_c002_expectations.md`
  - `scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py`
  - `tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py`
  - `spec/planning/compiler/m232/m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_packet.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m232-c002-lane-c-readiness`
  - `check:objc3c:m232-c003-message-send-lowering-and-call-emission-core-feature-implementation-contract`
  - `test:tooling:m232-c003-message-send-lowering-and-call-emission-core-feature-implementation-contract`
  - `check:objc3c:m232-c003-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Required Evidence

- `tmp/reports/m232/M232-C003/message_send_lowering_and_call_emission_core_feature_implementation_summary.json`

## Gate Commands

- `python scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py -q`
- `python scripts/check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m232-c003-lane-c-readiness`
