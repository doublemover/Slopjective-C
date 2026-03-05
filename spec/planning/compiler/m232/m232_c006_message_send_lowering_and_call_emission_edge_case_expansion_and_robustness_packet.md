# M232-C006 Message Send Lowering and Call Emission Edge-case Expansion and Robustness Packet

Packet: `M232-C006`
Milestone: `M232`
Lane: `C`
Issue: `#5616`
Dependencies: `M232-C005`

## Scope

Freeze lane-C message send lowering and call emission edge-case expansion and
robustness anchors so semantic-to-lowering continuity remains deterministic and
fail-closed on top of C005 edge-case and compatibility completion governance.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Anchors

- Contract:
  `docs/contracts/m232_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_c006_expectations.md`
- Operator runbook:
  `docs/runbooks/m232_wave_execution_runbook.md`
- Checker:
  `scripts/check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py`
- Dependency anchors (`M232-C005`):
  - `docs/contracts/m232_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_c005_expectations.md`
  - `scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
  - `spec/planning/compiler/m232/m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_packet.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m232-c005-lane-c-readiness`
  - `check:objc3c:m232-c006-message-send-lowering-and-call-emission-edge-case-expansion-and-robustness-contract`
  - `test:tooling:m232-c006-message-send-lowering-and-call-emission-edge-case-expansion-and-robustness-contract`
  - `check:objc3c:m232-c006-lane-c-readiness`
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

- `tmp/reports/m232/M232-C006/message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_summary.json`

## Gate Commands

- `python scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py -q`
- `python scripts/check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m232-c006-lane-c-readiness`
