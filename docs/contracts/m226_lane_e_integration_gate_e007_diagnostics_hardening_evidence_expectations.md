# M226 Lane E Integration Gate Diagnostics Hardening Evidence Expectations (E007)

Contract ID: `objc3c-lane-e-integration-gate-diagnostics-hardening-evidence-contract/m226-e007-v1`
Status: Accepted
Scope: Lane-E milestone integration gate diagnostics-hardening evidence expansion for M226 closeout continuity.

## Objective

Expand lane-E gate evidence from E006 edge-robustness completion into a
diagnostics-hardening closeout schema that binds parser diagnostics coverage,
sema diagnostics determinism, and invocation diagnostics guardrails into one
deterministic fail-closed index.

## Required Invariants

1. Packet and scaffold modules are explicit:
   - `spec/planning/compiler/m226/m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_packet.md`
   - `spec/planning/compiler/m226/m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_scaffold.md`
2. E007 evidence schema extends E006 with diagnostics-hardening controls:
   - Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `edge_compatibility`, `edge_robustness`, `diagnostics_hardening`.
   - `diagnostics_hardening` required keys: `parser_advanced`, `parser_sema`, `build_invocation`, `integration_gate`.
   - `generated_at_utc` uses RFC3339 UTC format with trailing `Z`.
   - Index path is `tmp/reports/m226/e007/evidence_index.json`.
3. Freeze registry includes E007 packet wiring:
   - `spec/planning/compiler/m226/m226_lane_e_contract_freeze_20260302.md`
     registers packet `M226-E007` and its contract/checker/test anchors.
4. E007 gate validates diagnostics-hardening anchors from:
   - `M226-A017`, `M226-B007`, `M226-D007`, `M226-E006`.
   - Required upstream doc pointers:
     - `docs/contracts/m226_parser_advanced_diagnostics_workpack_a017_expectations.md`
     - `docs/contracts/m226_parser_sema_diagnostics_hardening_b007_expectations.md`
     - `docs/contracts/m226_frontend_build_invocation_diagnostics_hardening_d007_expectations.md`
     - `docs/contracts/m226_lane_e_integration_gate_e006_edge_robustness_evidence_expectations.md`
   - Required upstream artifact pointers:
     - `tmp/reports/m226/M226-A017/parser_advanced_diagnostics_workpack_summary.json`
     - `tmp/reports/m226/M226-B007/parser_sema_diagnostics_hardening_summary.json`
     - `tmp/reports/m226/M226-D007/frontend_build_invocation_diagnostics_hardening_summary.json`
     - `tmp/reports/m226/m226_e006_lane_e_integration_gate_edge_robustness_evidence_contract_summary.json`
5. Fail-closed validation entrypoints remain pinned:
   - `python scripts/check_m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract.py -q`

## Validation

- `python scripts/check_m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract.py`
- `python -m pytest tests/tooling/test_check_m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract_summary.json`
- `tmp/reports/m226/e007/validation/pytest_check_m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract.txt`
- `tmp/reports/m226/e007/evidence_index.json`
