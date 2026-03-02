# M226 Lane E Integration Gate Conformance Matrix Evidence Expectations (E009)

Contract ID: `objc3c-lane-e-integration-gate-conformance-matrix-evidence-contract/m226-e009-v1`
Status: Accepted
Scope: Lane-E milestone integration gate conformance-matrix evidence expansion for M226 closeout continuity.

## Objective

Extend lane-E evidence from E008 recovery determinism to a dedicated
conformance-matrix gate that binds parser matrix determinism, parser->sema
matrix hardening, parse-lowering matrix readiness, and frontend invocation
matrix coverage into one fail-closed index.

## Required Invariants

1. Packet and scaffold modules are explicit:
   - `spec/planning/compiler/m226/m226_e009_lane_e_integration_gate_conformance_matrix_evidence_packet.md`
   - `spec/planning/compiler/m226/m226_e009_lane_e_integration_gate_conformance_matrix_evidence_scaffold.md`
2. E009 evidence schema extends E008 with conformance-matrix controls:
   - Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `conformance_matrix`.
   - `conformance_matrix` required keys: `parser`, `parser_sema`, `parse_lowering`, `build_invocation`, `integration_gate`.
   - `generated_at_utc` uses RFC3339 UTC format with trailing `Z`.
   - Index path is `tmp/reports/m226/e009/evidence_index.json`.
3. Freeze registry includes E009 packet wiring:
   - `spec/planning/compiler/m226/m226_lane_e_contract_freeze_20260302.md`
     registers packet `M226-E009` and its contract/checker/test anchors.
4. E009 gate validates conformance-matrix anchors from:
   - `M226-E008`, `M226-A009`, `M226-B009`, `M226-C009`, `M226-D009`.
   - Required upstream doc pointers:
     - `docs/contracts/m226_parser_conformance_matrix_expectations.md`
     - `docs/contracts/m226_parser_sema_conformance_matrix_b009_expectations.md`
     - `docs/contracts/m226_parse_lowering_conformance_matrix_c009_expectations.md`
     - `docs/contracts/m226_frontend_build_invocation_conformance_matrix_d009_expectations.md`
     - `docs/contracts/m226_lane_e_integration_gate_e008_recovery_determinism_evidence_expectations.md`
   - Required upstream artifact pointers:
     - `tmp/reports/m226/M226-A009/parser_conformance_matrix_summary.json`
     - `tmp/reports/m226/M226-B009/parser_sema_conformance_matrix_summary.json`
     - `tmp/reports/m226/m226_c009_parse_lowering_conformance_matrix_contract_summary.json`
     - `tmp/reports/m226/M226-D009/frontend_build_invocation_conformance_matrix_summary.json`
     - `tmp/reports/m226/m226_e008_lane_e_integration_gate_recovery_determinism_evidence_contract_summary.json`
5. Fail-closed validation entrypoints remain pinned:
   - `python scripts/check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py -q`

## Validation

- `python scripts/check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py`
- `python -m pytest tests/tooling/test_check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract_summary.json`
- `tmp/reports/m226/e009/validation/pytest_check_m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract.txt`
- `tmp/reports/m226/e009/evidence_index.json`
