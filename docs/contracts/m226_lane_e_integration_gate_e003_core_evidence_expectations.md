# M226 Lane E Integration Gate Core Evidence Expectations (E003)

Contract ID: `objc3c-lane-e-integration-gate-core-evidence-contract/m226-e003-v1`
Status: Accepted
Scope: Lane-E integration gate core evidence indexing for M226 closeout evidence.

## Objective

Define fail-closed, deterministic core evidence indexing for lane-E integration
gate artifacts so E001/E002/E003 contract summaries and validation transcripts
are discoverable via a single pinned evidence index schema.

## Required Invariants

1. Packet and scaffold modules are explicit:
   - `spec/planning/compiler/m226/m226_e003_lane_e_integration_gate_core_evidence_packet.md`
   - `spec/planning/compiler/m226/m226_e003_lane_e_integration_gate_core_evidence_scaffold.md`
2. Core evidence index schema is pinned:
   - Required top-level keys: `packet`, `generated_at_utc`, `upstream_packets`, `artifacts`.
   - `generated_at_utc` uses RFC3339 UTC format with trailing `Z`.
   - Index path remains `tmp/reports/m226/e003/evidence_index.json`.
3. Freeze registry includes E003 packet wiring:
   - `spec/planning/compiler/m226/m226_lane_e_contract_freeze_20260302.md`
     registers packet `M226-E003` and its contract/checker/test anchors.
4. Fail-closed validation entrypoints remain pinned:
   - `python scripts/check_m226_e003_lane_e_integration_gate_core_evidence_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_e003_lane_e_integration_gate_core_evidence_contract.py -q`

## Validation

- `python scripts/check_m226_e003_lane_e_integration_gate_core_evidence_contract.py`
- `python -m pytest tests/tooling/test_check_m226_e003_lane_e_integration_gate_core_evidence_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_e003_lane_e_integration_gate_core_evidence_contract_summary.json`
- `tmp/reports/m226/e003/validation/pytest_check_m226_e003_lane_e_integration_gate_core_evidence_contract.txt`
- `tmp/reports/m226/e003/evidence_index.json`
