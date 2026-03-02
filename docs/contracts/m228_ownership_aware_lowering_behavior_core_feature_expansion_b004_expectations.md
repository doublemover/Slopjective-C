# M228 Ownership-Aware Lowering Behavior Core Feature Expansion Expectations (B004)

Contract ID: `objc3c-ownership-aware-lowering-behavior-core-feature-expansion/m228-b004-v1`
Status: Accepted
Scope: ownership-aware lowering core-feature expansion guardrails on top of B003 core-feature implementation.

## Objective

Expand lane-B ownership-aware lowering closure with explicit weak/unowned
integration, expansion-accounting consistency, and replay-proof expansion keys
so direct LLVM IR emission remains fail-closed when expansion drift appears.

## Dependency Scope

- Dependencies: `M228-B003`
- M228-B003 core-feature implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_core_feature_implementation_b003_expectations.md`
  - `scripts/check_m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m228_b003_ownership_aware_lowering_behavior_core_feature_implementation_contract.py`
- Packet/checker/test assets for B004 remain mandatory:
  - `spec/planning/compiler/m228/m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_packet.md`
  - `scripts/check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` carries explicit expansion
   fields:
   - `weak_unowned_semantics_contract_ready`
   - `ownership_profile_accounting_consistent`
   - `expansion_replay_keys_ready`
   - `expansion_deterministic_replay_surface`
   - `expansion_ready`
   - `expansion_key`
2. `BuildObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionKey(...)`
   remains deterministic and keyed by scaffold + weak/unowned replay anchors.
3. `BuildObjc3FrontendArtifacts(...)` remains fail-closed on ownership-aware
   lowering expansion drift through
   `IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady(...)` with
   diagnostic code `O3L310`.
4. IR frontend metadata carries ownership-aware lowering expansion readiness/key:
   - `Objc3IRFrontendMetadata::ownership_aware_lowering_core_feature_expansion_ready`
   - `Objc3IRFrontendMetadata::ownership_aware_lowering_core_feature_expansion_key`
   - IR output includes
     `; ownership_aware_lowering_core_feature_expansion = ...` and readiness.
5. Architecture/spec anchors explicitly mention M228 lane-B B004 core feature
   expansion closure.

## Validation

- `python scripts/check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b004_ownership_aware_lowering_behavior_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m228-b004-lane-b-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m228/M228-B004/ownership_aware_lowering_behavior_core_feature_expansion_contract_summary.json`
- `tmp/reports/m228/M228-B004/closeout_validation_report.md`
