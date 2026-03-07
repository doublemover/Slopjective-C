# M239-E004 CFG correctness gate and replay evidence Core Feature Expansion Packet

Packet: `M239-E004`
Milestone: `M239`
Lane: `E`
Issue: `#5016`
Freeze date: `2026-03-05`
Dependencies: `M239-A001`, `M239-B001`, `M239-C001`

## Purpose

Freeze lane-E cfg correctness gate and replay evidence contract prerequisites for
M239 so lane-E readiness stays deterministic and fail-closed against
currently-closed early lane steps.

## Scope Anchors

- Contract:
  `docs/contracts/m239_cfg_correctness_gate_and_replay_evidence_core_feature_expansion_e004_expectations.md`
- Checker:
  `scripts/check_m239_e004_cfg_correctness_gate_and_replay_evidence_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m239_e004_cfg_correctness_gate_and_replay_evidence_contract.py`
- Dependency anchors from currently-closed early lane steps:
  - `docs/contracts/m239_frontend_normalization_hints_for_cfg_quality_contract_and_architecture_freeze_a001_expectations.md`
  - `spec/planning/compiler/m239/m239_a001_frontend_normalization_hints_for_cfg_quality_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m239_a001_frontend_normalization_hints_for_cfg_quality_contract.py`
  - `tests/tooling/test_check_m239_a001_frontend_normalization_hints_for_cfg_quality_contract.py`
  - `docs/contracts/m239_semantic_flow_analysis_and_invariants_contract_and_architecture_freeze_b001_expectations.md`
  - `spec/planning/compiler/m239/m239_b001_semantic_flow_analysis_and_invariants_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m239_b001_semantic_flow_analysis_and_invariants_contract.py`
  - `tests/tooling/test_check_m239_b001_semantic_flow_analysis_and_invariants_contract.py`
  - `docs/contracts/m239_cfg_ssa_lowering_and_phi_construction_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m239/m239_c001_cfg_ssa_lowering_and_phi_construction_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m239_c001_cfg_ssa_lowering_and_phi_construction_contract.py`
  - `tests/tooling/test_check_m239_c001_cfg_ssa_lowering_and_phi_construction_contract.py`

## Gate Commands

- `python scripts/check_m239_e004_cfg_correctness_gate_and_replay_evidence_contract.py --summary-out tmp/reports/m239/M239-E004/local_check_summary.json`
- `python -m pytest tests/tooling/test_check_m239_e004_cfg_correctness_gate_and_replay_evidence_contract.py -q`

## Evidence Output

- `tmp/reports/m239/M239-E004/local_check_summary.json`









