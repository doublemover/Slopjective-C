# M232 Message Send Lowering and Call Emission Edge-case and Compatibility Completion Expectations (C005)

Contract ID: `objc3c-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion/m232-c005-v1`
Status: Accepted
Scope: lane-C message send lowering and call emission edge-case and compatibility completion closure on top of C004 core feature expansion governance.

## Objective

Execute issue `#5615` by locking deterministic lane-C edge-case/compatibility
continuity over canonical dependency anchors, command sequencing, and evidence
paths so readiness remains fail-closed when dependency or sequencing drift
appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-C readiness.

## Dependency Scope

- Dependencies: `M232-C004`
- `M232-C004` remains a mandatory prerequisite:
  - `docs/contracts/m232_message_send_lowering_and_call_emission_core_feature_expansion_c004_expectations.md`
  - `scripts/check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py`
  - `spec/planning/compiler/m232/m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_packet.md`

## Deterministic Invariants

1. Operator runbook edge-case/compatibility completion continuity remains explicit in:
   - `docs/runbooks/m232_wave_execution_runbook.md`
2. Runbook anchor continuity remains deterministic for:
   - `objc3c-message-send-lowering-and-call-emission-core-feature-expansion/m232-c004-v1`
   - `objc3c-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion/m232-c005-v1`
3. Lane-C edge-case/compatibility command sequencing remains fail-closed for:
   - `python scripts/check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py`
   - `python -m pytest tests/tooling/test_check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py -q`
   - `python scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
   - `python -m pytest tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py -q`
   - `npm run check:objc3c:m232-c005-lane-c-readiness`
4. Dependency continuity remains explicit and deterministic across `M232-C004`
   contract/checker/test/packet assets.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m232-c004-lane-c-readiness`
  - `check:objc3c:m232-c005-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion-contract`
  - `test:tooling:m232-c005-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion-contract`
  - `check:objc3c:m232-c005-lane-c-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M232 lane-C C005
  edge-case and compatibility completion anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C M232 C005 fail-closed
  governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C M232 C005
  metadata anchor wording.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py -q`
- `python scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m232-c005-lane-c-readiness`

## Evidence Path

- `tmp/reports/m232/M232-C005/message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_summary.json`
