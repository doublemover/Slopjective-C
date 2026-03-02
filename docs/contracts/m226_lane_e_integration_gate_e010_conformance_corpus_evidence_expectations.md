# M226 Lane E Integration Gate Conformance Corpus Evidence Expectations (E010)

Contract ID: `objc3c-lane-e-integration-gate-conformance-corpus-evidence-contract/m226-e010-v1`
Status: Accepted
Scope: Lane-E milestone integration gate conformance-corpus evidence expansion for M226 closeout continuity.

## Objective

Extend lane-E evidence from E009 conformance-matrix coverage to a dedicated
conformance-corpus gate that binds parser corpus determinism, parser->sema
corpus hardening, parse-lowering corpus readiness, and frontend invocation
corpus coverage into one fail-closed index.

## Required Invariants

1. Packet and scaffold modules are explicit:
   - `spec/planning/compiler/m226/m226_e010_lane_e_integration_gate_conformance_corpus_evidence_packet.md`
   - `spec/planning/compiler/m226/m226_e010_lane_e_integration_gate_conformance_corpus_evidence_scaffold.md`
2. E010 evidence schema extends E009 with conformance-corpus controls:
   - Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `conformance_corpus`.
   - `conformance_corpus` required keys: `parser`, `parser_sema`, `parse_lowering`, `build_invocation`, `integration_gate`.
   - `generated_at_utc` uses RFC3339 UTC format with trailing `Z`.
   - Index path is `tmp/reports/m226/e010/evidence_index.json`.
3. Freeze registry includes E010 packet wiring:
   - `spec/planning/compiler/m226/m226_lane_e_contract_freeze_20260302.md`
4. E010 gate validates conformance-corpus anchors from:
   - `M226-E009`, `M226-A010`, `M226-B010`, `M226-C010`, `M226-D010`.
   - Required upstream doc pointers:
     - `docs/contracts/m226_parser_conformance_corpus_expansion_expectations.md`
     - `docs/contracts/m226_parser_sema_conformance_corpus_b010_expectations.md`
     - `docs/contracts/m226_parse_lowering_conformance_corpus_c010_expectations.md`
     - `docs/contracts/m226_frontend_build_invocation_conformance_corpus_d010_expectations.md`
     - `docs/contracts/m226_lane_e_integration_gate_e009_conformance_matrix_evidence_expectations.md`
   - Required upstream artifact pointers:
     - `tmp/reports/m226/M226-A010/parser_conformance_corpus_summary.json`
     - `tmp/reports/m226/M226-B010/parser_sema_conformance_corpus_summary.json`
     - `tmp/reports/m226/m226_c010_parse_lowering_conformance_corpus_contract_summary.json`
     - `tmp/reports/m226/M226-D010/frontend_build_invocation_conformance_corpus_summary.json`
     - `tmp/reports/m226/m226_e009_lane_e_integration_gate_conformance_matrix_evidence_contract_summary.json`
5. Fail-closed validation entrypoints remain pinned:
   - `python scripts/check_m226_e010_lane_e_integration_gate_conformance_corpus_evidence_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_e010_lane_e_integration_gate_conformance_corpus_evidence_contract.py -q`

## Validation

- `python scripts/check_m226_e010_lane_e_integration_gate_conformance_corpus_evidence_contract.py`
- `python -m pytest tests/tooling/test_check_m226_e010_lane_e_integration_gate_conformance_corpus_evidence_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_e010_lane_e_integration_gate_conformance_corpus_evidence_contract_summary.json`
- `tmp/reports/m226/e010/validation/pytest_check_m226_e010_lane_e_integration_gate_conformance_corpus_evidence_contract.txt`
- `tmp/reports/m226/e010/evidence_index.json`
