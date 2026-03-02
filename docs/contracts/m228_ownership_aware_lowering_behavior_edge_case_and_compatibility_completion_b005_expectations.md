# M228 Ownership-Aware Lowering Behavior Edge-Case and Compatibility Completion Expectations (B005)

Contract ID: `objc3c-ownership-aware-lowering-behavior-edge-case-and-compatibility-completion/m228-b005-v1`
Status: Accepted
Scope: ownership-aware lowering edge-case and compatibility completion guardrails on top of B004 core-feature expansion.

## Objective

Complete lane-B ownership-aware lowering closure by threading parse/lowering
compatibility and edge-case robustness handoff signals into the ownership-aware
lowering scaffold so direct LLVM IR emission remains fail-closed on drift.

## Dependency Scope

- Dependencies: `M228-B004`
- M228-B004 core-feature expansion anchors remain mandatory prerequisites:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_core_feature_expansion_b004_expectations.md`
  - `scripts/check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py`
- Packet/checker/test assets for B005 remain mandatory:
  - `spec/planning/compiler/m228/m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` carries explicit edge-case and compatibility completion fields:
   - `compatibility_handoff_consistent`
   - `language_version_pragma_coordinate_order_consistent`
   - `parse_artifact_edge_case_robustness_consistent`
   - `parse_artifact_replay_key_deterministic`
   - `edge_case_compatibility_ready`
   - `compatibility_handoff_key`
   - `parse_artifact_edge_robustness_key`
   - `edge_case_compatibility_key`
2. `BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityKey(...)` remains deterministic and keyed by scaffold expansion readiness plus parse/lowering compatibility handoff anchors.
3. `BuildObjc3FrontendArtifacts(...)` remains fail-closed on edge-case compatibility drift through
   `IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(...)` with deterministic diagnostic code `O3L312`.
4. Failure reasons remain explicit for ownership-aware lowering edge-case compatibility drift.
5. B004 remains a mandatory prerequisite for this completion.

## Validation

- `python scripts/check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m228-b004-lane-b-readiness && python scripts/check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py && python -m pytest tests/tooling/test_check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-B005/ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract_summary.json`
- `tmp/reports/m228/M228-B005/closeout_validation_report.md`
