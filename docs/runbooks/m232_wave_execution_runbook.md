# M232 Wave Execution Runbook

## Contract IDs

- `objc3c-message-send-lowering-and-call-emission-contract-and-architecture-freeze/m232-c001-v1`
- `objc3c-message-send-lowering-and-call-emission-modular-split-and-scaffolding/m232-c002-v1`
- `objc3c-message-send-lowering-and-call-emission-core-feature-implementation/m232-c003-v1`
- `objc3c-message-send-lowering-and-call-emission-core-feature-expansion/m232-c004-v1`
- `objc3c-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion/m232-c005-v1`
- `objc3c-message-send-lowering-and-call-emission-edge-case-expansion-and-robustness/m232-c006-v1`

## Operator Command Sequence

1. `python scripts/check_m232_c001_message_send_lowering_and_call_emission_contract_and_architecture_freeze_contract.py`
2. `python -m pytest tests/tooling/test_check_m232_c001_message_send_lowering_and_call_emission_contract_and_architecture_freeze_contract.py -q`
3. `npm run check:objc3c:m232-c001-lane-c-readiness`
4. `python scripts/check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py`
5. `python -m pytest tests/tooling/test_check_m232_c002_message_send_lowering_and_call_emission_modular_split_and_scaffolding_contract.py -q`
6. `npm run check:objc3c:m232-c002-lane-c-readiness`
7. `python scripts/check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py`
8. `python -m pytest tests/tooling/test_check_m232_c003_message_send_lowering_and_call_emission_core_feature_implementation_contract.py -q`
9. `npm run check:objc3c:m232-c003-lane-c-readiness`
10. `python scripts/check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py`
11. `python -m pytest tests/tooling/test_check_m232_c004_message_send_lowering_and_call_emission_core_feature_expansion_contract.py -q`
12. `npm run check:objc3c:m232-c004-lane-c-readiness`
13. `python scripts/check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py`
14. `python -m pytest tests/tooling/test_check_m232_c005_message_send_lowering_and_call_emission_edge_case_and_compatibility_completion_contract.py -q`
15. `npm run check:objc3c:m232-c005-lane-c-readiness`
16. `python scripts/check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py`
17. `python -m pytest tests/tooling/test_check_m232_c006_message_send_lowering_and_call_emission_edge_case_expansion_and_robustness_contract.py -q`
18. `npm run check:objc3c:m232-c006-lane-c-readiness`

## Evidence

- `tmp/reports/m232/`
