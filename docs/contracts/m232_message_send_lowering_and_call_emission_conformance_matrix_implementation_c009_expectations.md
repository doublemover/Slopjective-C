# M232 Message Send Lowering and Call Emission Conformance Matrix Implementation Expectations (C009)

Contract ID: `objc3c-message-send-lowering-and-call-emission-conformance-matrix-implementation/m232-c009-v1`
Status: Accepted
Scope: lane-C message send lowering and call emission conformance matrix implementation closure on top of C008 recovery and determinism hardening governance.

## Objective

Execute issue `#5619` by locking deterministic lane-C conformance matrix implementation
continuity over canonical dependency anchors, command sequencing, and evidence
paths so readiness remains fail-closed when dependency or sequencing drift
appears.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.
Shared-file deltas required for full lane-C readiness.

## Dependency Scope

- Dependencies: `M232-C008`
- `M232-C008` remains a mandatory prerequisite:
  - `docs/contracts/m232_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_c008_expectations.md`
  - `scripts/check_m232_c008_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m232_c008_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_contract.py`
  - `spec/planning/compiler/m232/m232_c008_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_packet.md`

## Deterministic Invariants

1. Operator runbook conformance matrix implementation continuity remains explicit in:
   - `docs/runbooks/m232_wave_execution_runbook.md`
2. Runbook anchor continuity remains deterministic for:
   - `objc3c-message-send-lowering-and-call-emission-recovery-and-determinism-hardening/m232-c008-v1`
   - `objc3c-message-send-lowering-and-call-emission-conformance-matrix-implementation/m232-c009-v1`
3. Lane-C conformance matrix implementation command sequencing remains fail-closed for:
   - `python scripts/check_m232_c008_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_contract.py`
   - `python -m pytest tests/tooling/test_check_m232_c008_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_contract.py -q`
   - `python scripts/check_m232_c009_message_send_lowering_and_call_emission_conformance_matrix_implementation_contract.py`
   - `python -m pytest tests/tooling/test_check_m232_c009_message_send_lowering_and_call_emission_conformance_matrix_implementation_contract.py -q`
   - `npm run check:objc3c:m232-c009-lane-c-readiness`
4. Dependency continuity remains explicit and deterministic across `M232-C008`
   contract/checker/test/packet assets.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m232-c008-lane-c-readiness`
  - `check:objc3c:m232-c009-message-send-lowering-and-call-emission-conformance-matrix-implementation-contract`
  - `test:tooling:m232-c009-message-send-lowering-and-call-emission-conformance-matrix-implementation-contract`
  - `check:objc3c:m232-c009-lane-c-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M232 lane-C C009
  conformance matrix implementation anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C M232 C009 fail-closed
  governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C M232 C009
  metadata anchor wording.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m232_c008_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c008_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_contract.py -q`
- `python scripts/check_m232_c009_message_send_lowering_and_call_emission_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m232_c009_message_send_lowering_and_call_emission_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m232-c009-lane-c-readiness`

## Evidence Path

- `tmp/reports/m232/M232-C009/message_send_lowering_and_call_emission_conformance_matrix_implementation_summary.json`








