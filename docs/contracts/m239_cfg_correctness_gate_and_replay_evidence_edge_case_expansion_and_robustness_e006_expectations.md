# M239 CFG correctness gate and replay evidence Edge-case Expansion and Robustness Expectations (E006)

Contract ID: `objc3c-cfg-correctness-gate-and-replay-evidence-edge-case-expansion-and-robustness/m239-e006-v1`
Status: Accepted
Issue: `#5018`
Dependencies: `M239-A001`, `M239-B001`, `M239-C001`
Scope: M239 lane-E cfg correctness gate and replay evidence edge-case expansion and robustness bound to currently-closed early lane steps.

## Objective

Fail closed unless lane-E cfg correctness gate and replay evidence dependency anchors
remain explicit, deterministic, and traceable against currently-closed early
lane steps before E002+ workpacks consume lane-E readiness.

## Prerequisite Dependency Matrix (Currently-Closed Early Lane Steps)

| Lane Task | Required Freeze State |
| --- | --- |
| `M239-A001` | Contract assets for A001 are required and must remain present/readable. |
| `M239-B001` | Contract assets for B001 are required and must remain present/readable. |
| `M239-C001` | Contract assets for C001 are required and must remain present/readable. |

## Scope Anchors

- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m239/m239_e006_cfg_correctness_gate_and_replay_evidence_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m239_e006_cfg_correctness_gate_and_replay_evidence_contract.py`
  - `tests/tooling/test_check_m239_e006_cfg_correctness_gate_and_replay_evidence_contract.py`
- Dependency anchors from currently-closed early lane steps:
  - `docs/contracts/m239_frontend_normalization_hints_for_cfg_quality_contract_and_architecture_freeze_a001_expectations.md`
  - `docs/contracts/m239_semantic_flow_analysis_and_invariants_contract_and_architecture_freeze_b001_expectations.md`
  - `docs/contracts/m239_cfg_ssa_lowering_and_phi_construction_contract_and_architecture_freeze_c001_expectations.md`

## Validation

- `python scripts/check_m239_e006_cfg_correctness_gate_and_replay_evidence_contract.py --summary-out tmp/reports/m239/M239-E006/local_check_summary.json`
- `python -m pytest tests/tooling/test_check_m239_e006_cfg_correctness_gate_and_replay_evidence_contract.py -q`

## Evidence Path

- `tmp/reports/m239/M239-E006/local_check_summary.json`













