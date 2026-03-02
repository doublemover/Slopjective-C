# M228 Ownership-Aware Lowering Behavior Edge-Case Expansion and Robustness Expectations (B006)

Contract ID: `objc3c-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness/m228-b006-v1`
Status: Accepted
Scope: ownership-aware lowering edge-case expansion and robustness guardrails on top of B005 compatibility completion.

## Objective

Expand lane-B ownership-aware lowering closure with explicit edge-case expansion
consistency and robustness readiness/key signals so edge drift remains
deterministic and fail-closed before LLVM IR emission.

## Dependency Scope

- Dependencies: `M228-B005`
- M228-B005 compatibility completion anchors remain mandatory prerequisites:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_b005_expectations.md`
  - `scripts/check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py`
- Packet/checker/test assets for B006 remain mandatory:
  - `spec/planning/compiler/m228/m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` carries explicit edge-case
   expansion and robustness fields:
   - `edge_case_expansion_consistent`
   - `edge_case_robustness_ready`
   - `edge_case_robustness_key`
2. `BuildObjc3OwnershipAwareLoweringBehaviorEdgeCaseRobustnessKey(...)`
   remains deterministic and keyed by B005 compatibility closure plus parse edge
   robustness/replay evidence.
3. `BuildObjc3OwnershipAwareLoweringBehaviorScaffold(...)` computes
   edge-case expansion and robustness fail-closed from B005 compatibility closure
   plus deterministic parse robustness handoff keys.
4. `IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(...)`
   remains fail-closed and now requires non-empty robustness-key evidence so
   existing ownership gate diagnostics (`O3L312`) fail closed on robustness drift.
5. Failure reasons remain explicit for ownership-aware lowering edge-case
   expansion and robustness drift.
6. B005 remains a mandatory prerequisite for B006 readiness.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m228-b006-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness-contract`
  - `test:tooling:m228-b006-ownership-aware-lowering-behavior-edge-case-expansion-and-robustness-contract`
  - `check:objc3c:m228-b006-lane-b-readiness`
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m228-b005-lane-b-readiness`
  - `check:objc3c:m228-b006-lane-b-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes M228 lane-B B006 robustness
  anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes M228 lane-B B006
  robustness fail-closed wiring text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B B006
  robustness metadata anchors.

## Validation

- `python scripts/check_m228_b005_ownership_aware_lowering_behavior_edge_case_and_compatibility_completion_contract.py`
- `python scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m228-b006-lane-b-readiness`

## Evidence Path

- `tmp/reports/m228/M228-B006/ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract_summary.json`
- `tmp/reports/m228/M228-B006/closeout_validation_report.md`
