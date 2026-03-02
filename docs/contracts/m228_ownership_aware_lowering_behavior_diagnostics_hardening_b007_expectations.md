# M228 Ownership-Aware Lowering Behavior Diagnostics Hardening Expectations (B007)

Contract ID: `objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1`
Status: Accepted
Scope: ownership-aware lowering diagnostics hardening closure on top of B006 edge-case robustness.

## Objective

Extend lane-B ownership-aware lowering closure with explicit diagnostics
hardening consistency/readiness and diagnostics-key integrity so ownership
diagnostic routing stays deterministic and fail-closed before LLVM IR emission.

## Dependency Scope

- Dependencies: `M228-B006`
- M228-B006 remains a mandatory prerequisite for B007 diagnostics hardening:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_b006_expectations.md`
  - `scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py`
  - `spec/planning/compiler/m228/m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_packet.md`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` carries diagnostics hardening
   fields:
   - `diagnostics_hardening_consistent`
   - `diagnostics_hardening_ready`
   - `diagnostics_hardening_key`
2. `BuildObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningKey(...)`
   remains deterministic and keyed by B006 robustness plus ARC diagnostics and
   parse artifact replay evidence.
3. `BuildObjc3OwnershipAwareLoweringBehaviorScaffold(...)` computes diagnostics
   hardening fail-closed from robustness and replay key continuity.
4. `IsObjc3OwnershipAwareLoweringBehaviorDiagnosticsHardeningReady(...)` fails
   closed when diagnostics hardening consistency/readiness or key continuity
   drifts.
5. `IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(...)`
   requires diagnostics hardening readiness/key evidence so existing ownership
   fail-closed diagnostic routing (`O3L312`) closes on diagnostics drift.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m228-b007-ownership-aware-lowering-behavior-diagnostics-hardening-contract`
  - `test:tooling:m228-b007-ownership-aware-lowering-behavior-diagnostics-hardening-contract`
  - `check:objc3c:m228-b007-lane-b-readiness`
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m228-b006-lane-b-readiness`
  - `check:objc3c:m228-b007-lane-b-readiness`

## Architecture and Spec Anchors

Shared-file deltas required for full lane-B readiness.

- `native/objc3c/src/ARCHITECTURE.md` includes M228 lane-B B007 diagnostics
  hardening anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes M228 lane-B B007
  diagnostics hardening fail-closed wiring text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B B007
  diagnostics hardening metadata anchors.

## Validation

- `python scripts/check_m228_b006_ownership_aware_lowering_behavior_edge_case_expansion_and_robustness_contract.py`
- `python scripts/check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m228-b007-lane-b-readiness`

## Evidence Path

- `tmp/reports/m228/M228-B007/ownership_aware_lowering_behavior_diagnostics_hardening_contract_summary.json`
