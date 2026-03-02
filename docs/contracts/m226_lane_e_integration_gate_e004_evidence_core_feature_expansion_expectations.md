# M226 Lane E Integration Gate Evidence Core Feature Expansion Expectations (E004)

Contract ID: `objc3c-lane-e-integration-gate-evidence-core-feature-expansion-contract/m226-e004-v1`
Status: Accepted
Scope: Lane-E milestone integration gate evidence core feature expansion for M226 closeout evidence.

## Objective

Expand lane-E integration gate evidence from E003 core indexing into explicit
milestone gate feature wiring so closeout packets remain deterministic,
fail-closed, and uniformly discoverable from one schema.

## Required Invariants

1. Packet and scaffold modules are explicit:
   - `spec/planning/compiler/m226/m226_e004_lane_e_integration_gate_evidence_core_feature_expansion_packet.md`
   - `spec/planning/compiler/m226/m226_e004_lane_e_integration_gate_evidence_core_feature_expansion_scaffold.md`
2. Expanded evidence index schema is pinned:
   - Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`.
   - `milestone_gate` required keys: `milestone_id`, `gate_status`, `validated_by`.
   - `feature_matrix` required entries: `core_index`, `milestone_gate`, `validation_transcripts`.
   - `generated_at_utc` uses RFC3339 UTC format with trailing `Z`.
   - Index path remains `tmp/reports/m226/e004/evidence_index.json`.
3. Freeze registry includes E004 packet wiring:
   - `spec/planning/compiler/m226/m226_lane_e_contract_freeze_20260302.md`
     registers packet `M226-E004` and its contract/checker/test anchors.
4. Fail-closed validation entrypoints remain pinned:
   - `python scripts/check_m226_e004_lane_e_integration_gate_evidence_core_feature_expansion_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_e004_lane_e_integration_gate_evidence_core_feature_expansion_contract.py -q`

## Validation

- `python scripts/check_m226_e004_lane_e_integration_gate_evidence_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m226_e004_lane_e_integration_gate_evidence_core_feature_expansion_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_e004_lane_e_integration_gate_evidence_core_feature_expansion_contract_summary.json`
- `tmp/reports/m226/e004/validation/pytest_check_m226_e004_lane_e_integration_gate_evidence_core_feature_expansion_contract.txt`
- `tmp/reports/m226/e004/evidence_index.json`
