# M232 Message Send Lowering and Call Emission Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-message-send-lowering-and-call-emission-core-feature-implementation/m232-c003-v1`
Status: Accepted
Scope: lane-C message send lowering and call emission core feature implementation closure on top of C002 modular split/scaffolding governance.

## Objective

Execute issue `#5613` by locking deterministic lane-C core feature
implementation continuity over canonical dependency anchors, command sequencing,
and evidence paths so readiness remains fail-closed when dependency or
sequencing drift appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-C readiness.

## Dependency Scope

- Dependencies: `M232-C002`
- `M232-C002` remains a mandatory prerequisite:
  - `docs/contracts/m232_message_send_lowering_and_call_emission_modular_split_and_scaffolding_c002_expectations.md`
  - `scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py`
  - `tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py`
  - `spec/planning/compiler/m232/m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_packet.md`

## Deterministic Invariants

1. Operator runbook core feature continuity remains explicit in:
   - `docs/runbooks/m232_wave_execution_runbook.md`
2. Runbook anchor continuity remains deterministic for:
   - `objc3c-message-send-lowering-and-call-emission-modular-split-and-scaffolding/m232-c002-v1`
   - `objc3c-message-send-lowering-and-call-emission-core-feature-implementation/m232-c003-v1`
3. Lane-C core feature command sequencing remains fail-closed for:
   - `python scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py`
   - `python -m pytest tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py -q`
   - `python scripts/check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py`
   - `python -m pytest tests/tooling/test_check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py -q`
   - `npm run check:objc3c:m232-c003-lane-c-readiness`
4. Dependency continuity remains explicit and deterministic across `M232-C002`
   contract/checker/test/packet assets.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m232-c002-lane-c-readiness`
  - `check:objc3c:m232-c003-message-send-lowering-and-call-emission-core-feature-implementation-contract`
  - `test:tooling:m232-c003-message-send-lowering-and-call-emission-core-feature-implementation-contract`
  - `check:objc3c:m232-c003-lane-c-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M232 lane-C C003
  core feature implementation anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C M232 C003 fail-closed
  governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C M232 C003
  metadata anchor wording.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py -q`
- `python scripts/check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m232-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m232/M232-C003/message_send_lowering_and_call_emission_core_feature_implementation_summary.json`
