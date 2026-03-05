# M232 Wave Execution Runbook

## Contract IDs

- `objc3c-message-send-lowering-and-call-emission-contract-and-architecture-freeze/m232-c001-v1`

## Operator Command Sequence

1. `python scripts/check_m232_c001_message_send_lowering_and_call_emission_contract_and_architecture_freeze_contract.py`
2. `python -m pytest tests/tooling/test_check_m232_c001_message_send_lowering_and_call_emission_contract_and_architecture_freeze_contract.py -q`
3. `npm run check:objc3c:m232-c001-lane-c-readiness`

## Evidence

- `tmp/reports/m232/`
