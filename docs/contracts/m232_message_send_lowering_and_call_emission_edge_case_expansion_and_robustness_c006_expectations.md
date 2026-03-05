# M232 Message Send Lowering and Call Emission Edge-case Expansion and Robustness Expectations (C006)

Contract ID: `objc3c-message-send-lowering-and-call-emission-edge-case-expansion-and-robustness/m232-c006-v1`
Status: Accepted
Scope: lane-C message send lowering and call emission edge-case expansion and robustness closure on top of C005 edge-case and compatibility completion governance.

## Objective

Execute issue `#5616` by locking deterministic lane-C edge-case expansion and
robustness continuity over canonical dependency anchors, command sequencing, and
evidence paths so readiness remains fail-closed when dependency or sequencing
drift appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-C readiness.

## Dependency Scope

- Dependencies: `M232-C005`
- `M232-C005` remains a mandatory prerequisite:
  - `docs/contracts/m232_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_c005_expectations.md`
  - `scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
  - `spec/planning/compiler/m232/m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_packet.md`

## Deterministic Invariants

1. Operator runbook edge-case expansion and robustness continuity remains explicit in:
   - `docs/runbooks/m232_wave_execution_runbook.md`
2. Runbook anchor continuity remains deterministic for:
   - `objc3c-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion/m232-c005-v1`
   - `objc3c-message-send-lowering-and-call-emission-edge-case-expansion-and-robustness/m232-c006-v1`
3. Lane-C edge-case expansion and robustness command sequencing remains fail-closed for:
   - `python scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
   - `python -m pytest tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py -q`
   - `python scripts/check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py`
   - `python -m pytest tests/tooling/test_check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py -q`
   - `npm run check:objc3c:m232-c006-lane-c-readiness`
4. Dependency continuity remains explicit and deterministic across `M232-C005`
   contract/checker/test/packet assets.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m232-c005-lane-c-readiness`
  - `check:objc3c:m232-c006-message-send-lowering-and-call-emission-edge-case-expansion-and-robustness-contract`
  - `test:tooling:m232-c006-message-send-lowering-and-call-emission-edge-case-expansion-and-robustness-contract`
  - `check:objc3c:m232-c006-lane-c-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M232 lane-C C006
  edge-case expansion and robustness anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C M232 C006 fail-closed
  governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C M232 C006
  metadata anchor wording.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py -q`
- `python scripts/check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m232-c006-lane-c-readiness`

## Evidence Path

- `tmp/reports/m232/M232-C006/message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_summary.json`
