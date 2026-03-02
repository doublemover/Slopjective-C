# M226 Lane E Integration Gate Edge Compatibility Evidence Expectations (E005)

Contract ID: `objc3c-lane-e-integration-gate-edge-compat-evidence-contract/m226-e005-v1`
Status: Accepted
Scope: Lane-E milestone integration gate edge-compatibility evidence completion for M226 closeout.

## Objective

Extend lane-E evidence beyond E004 core-feature indexing by pinning edge
compatibility closeout anchors across parser/sema, lowering, and invocation
routing so packet gating remains deterministic and fail closed.

## Required Invariants

1. Packet and scaffold modules are explicit:
   - `spec/planning/compiler/m226/m226_e005_lane_e_integration_gate_edge_compatibility_evidence_packet.md`
   - `spec/planning/compiler/m226/m226_e005_lane_e_integration_gate_edge_compatibility_evidence_scaffold.md`
2. E005 evidence schema extends E004 with edge compatibility coverage:
   - Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`, `milestone_gate`, `feature_matrix`, `edge_compatibility`.
   - `edge_compatibility` required keys: `parser_sema`, `parse_lowering`, `build_invocation`, `replay_dry_run`.
   - `generated_at_utc` uses RFC3339 UTC format with trailing `Z`.
   - Index path is `tmp/reports/m226/e005/evidence_index.json`.
3. Freeze registry includes E005 packet wiring:
   - `spec/planning/compiler/m226/m226_lane_e_contract_freeze_20260302.md`
     registers packet `M226-E005` and its contract/checker/test anchors.
4. E005 gate validates upstream edge-compat and replay anchors from:
   - `M226-A014`, `M226-B005`, `M226-C005`, `M226-D005`, `M226-E004`.
5. Fail-closed validation entrypoints remain pinned:
   - `python scripts/check_m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract.py -q`

## Validation

- `python scripts/check_m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract.py`
- `python -m pytest tests/tooling/test_check_m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract_summary.json`
- `tmp/reports/m226/e005/validation/pytest_check_m226_e005_lane_e_integration_gate_edge_compatibility_evidence_contract.txt`
- `tmp/reports/m226/e005/evidence_index.json`
