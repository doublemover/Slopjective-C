# M228-B007 Ownership-Aware Lowering Behavior Diagnostics Hardening Packet

Packet: `M228-B007`
Milestone: `M228`
Lane: `B`
Freeze date: `2026-03-02`
Dependencies: `M228-B006`

## Purpose

Freeze lane-B diagnostics hardening closure for ownership-aware lowering so
diagnostics consistency/readiness and diagnostics-key continuity stay
deterministic and fail-closed before IR emission.

## Scope Anchors

- Contract:
  `docs/contracts/m228_ownership_aware_lowering_behavior_diagnostics_hardening_b007_expectations.md`
- Checker:
  `scripts/check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
- Dependency anchors (`M228-B006`):
  - `docs/contracts/m228_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_b006_expectations.md`
  - `scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py`
  - `spec/planning/compiler/m228/m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_packet.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m228-b007-ownership-aware-lowering-behavior-diagnostics-hardening-contract`
  - `test:tooling:m228-b007-ownership-aware-lowering-behavior-diagnostics-hardening-contract`
  - `check:objc3c:m228-b007-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Gate Commands

- `python scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py`
- `python scripts/check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m228-b007-lane-b-readiness`

## Evidence Output

- `tmp/reports/m228/M228-B007/ownership_aware_lowering_behavior_diagnostics_hardening_contract_summary.json`
