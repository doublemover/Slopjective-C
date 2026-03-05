# M232 Wave Execution Runbook

## Contract IDs

- `objc3c-message-send-lowering-and-call-emission-contract-and-architecture-freeze/m232-c001-v1`
- `objc3c-message-send-lowering-and-call-emission-modular-split-and-scaffolding/m232-c002-v1`
- `objc3c-message-send-lowering-and-call-emission-core-feature-implementation/m232-c003-v1`
- `objc3c-message-send-lowering-and-call-emission-core-feature-expansion/m232-c004-v1`
- `objc3c-message-send-lowering-and-call-emission-edge-case-and-compatibility-completion/m232-c005-v1`
- `objc3c-message-send-lowering-and-call-emission-edge-case-expansion-and-robustness/m232-c006-v1`
- `objc3c-message-send-lowering-and-call-emission-diagnostics-hardening/m232-c007-v1`
- `objc3c-message-send-lowering-and-call-emission-recovery-and-determinism-hardening/m232-c008-v1`
- `objc3c-message-send-lowering-and-call-emission-conformance-matrix-implementation/m232-c009-v1`
- `objc3c-message-send-lowering-and-call-emission-conformance-corpus-expansion/m232-c010-v1`
- `objc3c-message-send-lowering-and-call-emission-performance-and-quality-guardrails/m232-c011-v1`
- `objc3c-message-send-lowering-and-call-emission-cross-lane-integration-sync/m232-c012-v1`
- `objc3c-message-send-lowering-and-call-emission-docs-and-operator-runbook-synchronization/m232-c013-v1`

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
19. `python scripts/check_m232_c007_message_send_lowering_and_call_emission_diagnostics_hardening_contract.py`
20. `python -m pytest tests/tooling/test_check_m232_c007_message_send_lowering_and_call_emission_diagnostics_hardening_contract.py -q`
21. `npm run check:objc3c:m232-c007-lane-c-readiness`
22. `python scripts/check_m232_c008_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_contract.py`
23. `python -m pytest tests/tooling/test_check_m232_c008_message_send_lowering_and_call_emission_recovery_and_determinism_hardening_contract.py -q`
24. `npm run check:objc3c:m232-c008-lane-c-readiness`
25. `python scripts/check_m232_c009_message_send_lowering_and_call_emission_conformance_matrix_implementation_contract.py`
26. `python -m pytest tests/tooling/test_check_m232_c009_message_send_lowering_and_call_emission_conformance_matrix_implementation_contract.py -q`
27. `npm run check:objc3c:m232-c009-lane-c-readiness`
28. `python scripts/check_m232_c010_message_send_lowering_and_call_emission_conformance_corpus_expansion_contract.py`
29. `python -m pytest tests/tooling/test_check_m232_c010_message_send_lowering_and_call_emission_conformance_corpus_expansion_contract.py -q`
30. `npm run check:objc3c:m232-c010-lane-c-readiness`
31. `python scripts/check_m232_c011_message_send_lowering_and_call_emission_performance_and_quality_guardrails_contract.py`
32. `python -m pytest tests/tooling/test_check_m232_c011_message_send_lowering_and_call_emission_performance_and_quality_guardrails_contract.py -q`
33. `npm run check:objc3c:m232-c011-lane-c-readiness`
34. `python scripts/check_m232_c012_message_send_lowering_and_call_emission_cross_lane_integration_sync_contract.py`
35. `python -m pytest tests/tooling/test_check_m232_c012_message_send_lowering_and_call_emission_cross_lane_integration_sync_contract.py -q`
36. `npm run check:objc3c:m232-c012-lane-c-readiness`
37. `python scripts/check_m232_c013_message_send_lowering_and_call_emission_docs_and_operator_runbook_synchronization_contract.py`
38. `python -m pytest tests/tooling/test_check_m232_c013_message_send_lowering_and_call_emission_docs_and_operator_runbook_synchronization_contract.py -q`
39. `npm run check:objc3c:m232-c013-lane-c-readiness`

## Evidence

- `tmp/reports/m232/`
