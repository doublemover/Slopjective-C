# M228 Ownership-Aware Lowering Behavior Core Feature Implementation Expectations (B003)

Contract ID: `objc3c-ownership-aware-lowering-behavior-core-feature-implementation/m228-b003-v1`
Status: Accepted
Scope: ownership-aware lowering core-feature implementation closure on top of B001 freeze and B002 modular split scaffolding.

## Objective

Implement lane-B core feature closure so ownership-aware lowering behavior
remains deterministic and fail-closed for direct LLVM IR emission hardening.
Code/spec anchors and milestone optimization improvements are mandatory scope
inputs for this closure.

## Dependency Scope

- Dependencies: `M228-B002`
- M228-B002 modular split/scaffolding anchors remain mandatory prerequisites:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_modular_split_scaffolding_b002_expectations.md`
  - `scripts/check_m228_b002_ownership_aware_lowering_behavior_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m228_b002_ownership_aware_lowering_behavior_modular_split_scaffolding_contract.py`
- Packet/checker/test assets for B003 remain mandatory:
  - `spec/planning/compiler/m228/m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_packet.md`
  - `scripts/check_m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_contract.py`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` remains the canonical
   ownership-aware lowering core-feature implementation surface for:
   - ownership qualifier lowering contract readiness
   - retain/release lowering contract readiness
   - autoreleasepool scope lowering contract readiness
   - ARC diagnostics/fix-it lowering contract readiness
   - lane-contract replay-key determinism
2. `BuildObjc3OwnershipAwareLoweringBehaviorScaffold(...)` remains the only
   canonical core-feature builder for ownership-aware lowering closure.
3. `BuildObjc3FrontendArtifacts(...)` remains fail-closed on ownership core
   feature implementation drift through
   `IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady(...)` with diagnostic
   code `O3L305`.
4. B001 and B002 remain mandatory prerequisites:
   - `docs/contracts/m228_ownership_aware_lowering_behavior_contract_freeze_b001_expectations.md`
   - `docs/contracts/m228_ownership_aware_lowering_behavior_modular_split_scaffolding_b002_expectations.md`
   - `scripts/check_m228_b001_ownership_aware_lowering_behavior_contract.py`
   - `scripts/check_m228_b002_ownership_aware_lowering_behavior_modular_split_scaffolding_contract.py`
5. Architecture/spec anchors explicitly mention M228 lane-B B003 core feature
   implementation closure.

## Validation

- `python scripts/check_m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m228-b003-lane-b-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-B003/ownership_aware_lowering_behavior_core_feature_implementation_contract_summary.json`
- `tmp/reports/m228/M228-B003/closeout_validation_report.md`
