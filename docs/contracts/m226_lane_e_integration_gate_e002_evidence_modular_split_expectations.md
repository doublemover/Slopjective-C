# M226 Lane E Integration Gate Evidence Modular Split Expectations (E002)

Contract ID: `objc3c-lane-e-integration-gate-evidence-modular-split-contract/m226-e002-v1`
Status: Accepted
Scope: Lane-E evidence packet modular split and scaffolding for the M226 integration gate.

## Objective

Split lane-E integration-gate evidence from packet policy into explicit packet and
scaffold modules so evidence capture remains deterministic, fail-closed, and
ready for milestone closeout runs.

## Required Invariants

1. Packet modularization is explicit:
   - `spec/planning/compiler/m226/m226_e002_lane_e_integration_gate_evidence_packet.md`
   - Packet captures E002 scope, commands, and upstream `M226-E001` dependency.
2. Evidence scaffolding is explicit and tmp-rooted:
   - `spec/planning/compiler/m226/m226_e002_lane_e_integration_gate_evidence_scaffold.md`
   - Scaffold pins summary artifact path
     `tmp/reports/m226/m226_e002_lane_e_integration_gate_evidence_modular_split_contract_summary.json`.
   - Scaffold pins validation transcript path
     `tmp/reports/m226/e002/validation/pytest_check_m226_e002_lane_e_integration_gate_evidence_modular_split_contract.txt`.
   - Scaffold pins evidence index path `tmp/reports/m226/e002/evidence_index.json`.
3. Freeze registry includes E002 packet wiring:
   - `spec/planning/compiler/m226/m226_lane_e_contract_freeze_20260302.md`
     registers packet `M226-E002` and its contract/checker anchors.
4. Fail-closed validation entrypoints remain pinned:
   - `python scripts/check_m226_e002_lane_e_integration_gate_evidence_modular_split_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_e002_lane_e_integration_gate_evidence_modular_split_contract.py -q`

## Validation

- `python scripts/check_m226_e002_lane_e_integration_gate_evidence_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m226_e002_lane_e_integration_gate_evidence_modular_split_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_e002_lane_e_integration_gate_evidence_modular_split_contract_summary.json`
- `tmp/reports/m226/e002/validation/pytest_check_m226_e002_lane_e_integration_gate_evidence_modular_split_contract.txt`
- `tmp/reports/m226/e002/evidence_index.json`
