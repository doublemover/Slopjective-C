# M232-C005 Message Send Lowering and Call Emission Edge-case and Compatibility Completion Packet

Packet: `M232-C005`
Milestone: `M232`
Lane: `C`
Issue: `#5615`
Dependencies: `M232-C004`

## Scope

Freeze lane-C message send lowering and call emission edge-case/compatibility
completion anchors so semantic-to-lowering continuity remains deterministic and
fail-closed on top of C004 core feature expansion governance.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m232_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_c005_expectations.md`
- Operator runbook:
  `docs/runbooks/m232_wave_execution_runbook.md`
- Checker:
  `scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
- Dependency anchors (`M232-C004`):
  - `docs/contracts/m232_message_send_lowering_and_call_emission_core_feature_expansion_c004_expectations.md`
  - `scripts/check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py`
  - `spec/planning/compiler/m232/m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_packet.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m232-c004-lane-c-readiness`
  - `check:objc3c:m232-c005-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion-contract`
  - `test:tooling:m232-c005-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion-contract`
  - `check:objc3c:m232-c005-lane-c-readiness`
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

- `tmp/reports/m232/M232-C005/message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_summary.json`

## Gate Commands

- `python scripts/check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py -q`
- `python scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m232-c005-lane-c-readiness`
