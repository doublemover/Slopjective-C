# M226 Lane E Integration Gate Recovery Determinism Evidence Expectations (E008)

Contract ID: `objc3c-lane-e-integration-gate-recovery-determinism-evidence-contract/m226-e008-v1`
Status: Accepted
Scope: Lane-E milestone integration gate recovery/determinism evidence expansion for M226 continuity.

## Objective

Extend lane-E evidence from E007 diagnostics hardening to a dedicated
recovery/determinism gate that binds parser conformance shard-2 determinism,
parser->sema recovery hardening, parse-lowering recovery readiness, and
frontend wrapper cache-recovery determinism into one fail-closed index.

## Required Invariants

1. Packet and scaffold modules are explicit:
   - `spec/planning/compiler/m226/m226_e008_lane_e_integration_gate_recovery_determinism_evidence_packet.md`
   - `spec/planning/compiler/m226/m226_e008_lane_e_integration_gate_recovery_determinism_evidence_scaffold.md`
2. E008 evidence schema extends E007 with recovery/determinism controls:
   - Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `recovery_determinism`.
   - `recovery_determinism` required keys: `parser_conformance`, `parser_sema`, `parse_lowering`, `build_invocation`, `integration_gate`.
   - `generated_at_utc` uses RFC3339 UTC format with trailing `Z`.
   - Index path is `tmp/reports/m226/e008/evidence_index.json`.
3. Freeze registry includes E008 packet wiring:
   - `spec/planning/compiler/m226/m226_lane_e_contract_freeze_20260302.md`
     registers packet `M226-E008` and its contract/checker/test anchors.
4. E008 gate validates recovery/determinism anchors from:
   - `M226-E007`, `M226-A024`, `M226-B008`, `M226-C008`, `M226-D008`.
   - Required upstream doc pointers:
     - `docs/contracts/m226_parser_advanced_conformance_workpack_a024_expectations.md`
     - `docs/contracts/m226_parser_sema_recovery_determinism_hardening_b008_expectations.md`
     - `docs/contracts/m226_parse_lowering_recovery_determinism_hardening_c008_expectations.md`
     - `docs/contracts/m226_frontend_build_invocation_recovery_determinism_hardening_d008_expectations.md`
     - `docs/contracts/m226_lane_e_integration_gate_e007_diagnostics_hardening_evidence_expectations.md`
   - Required upstream artifact pointers:
     - `tmp/reports/m226/M226-A024/parser_conformance_shard2_summary.json`
     - `tmp/reports/m226/M226-B008/parser_sema_recovery_determinism_hardening_summary.json`
     - `tmp/reports/m226/m226_c008_parse_lowering_recovery_determinism_hardening_contract_summary.json`
     - `tmp/reports/m226/M226-D008/frontend_build_invocation_recovery_determinism_hardening_summary.json`
     - `tmp/reports/m226/m226_e007_lane_e_integration_gate_diagnostics_hardening_evidence_contract_summary.json`
5. Fail-closed validation entrypoints remain pinned:
   - `python scripts/check_m226_e008_lane_e_integration_gate_recovery_determinism_evidence_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_e008_lane_e_integration_gate_recovery_determinism_evidence_contract.py -q`

## Validation

- `python scripts/check_m226_e008_lane_e_integration_gate_recovery_determinism_evidence_contract.py`
- `python -m pytest tests/tooling/test_check_m226_e008_lane_e_integration_gate_recovery_determinism_evidence_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_e008_lane_e_integration_gate_recovery_determinism_evidence_contract_summary.json`
- `tmp/reports/m226/e008/validation/pytest_check_m226_e008_lane_e_integration_gate_recovery_determinism_evidence_contract.txt`
- `tmp/reports/m226/e008/evidence_index.json`
