# M228 Ownership-Aware Lowering Behavior Conformance Matrix Implementation Expectations (B009)

Contract ID: `objc3c-ownership-aware-lowering-behavior-conformance-matrix-implementation/m228-b009-v1`
Status: Accepted
Scope: ownership-aware lowering conformance-matrix closure on top of B008 recovery/determinism hardening.

## Objective

Extend lane-B ownership-aware lowering closure with explicit conformance-matrix
consistency/readiness and conformance-key continuity so ownership lowering
remains deterministic and fail-closed before LLVM IR emission.

## Dependency Scope

- Dependencies: `M228-B008`
- M228-B008 remains a mandatory prerequisite for B009 conformance-matrix
  implementation:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_recovery_determinism_hardening_b008_expectations.md`
  - `scripts/check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py`
  - `spec/planning/compiler/m228/m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_packet.md`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` carries conformance-matrix
   implementation fields:
   - `conformance_matrix_consistent`
   - `conformance_matrix_ready`
   - `conformance_matrix_key`
2. `BuildObjc3OwnershipAwareLoweringBehaviorConformanceMatrixKey(...)`
   remains deterministic and keyed by B008 recovery/determinism continuity plus
   parse-lowering conformance matrix evidence.
3. `BuildObjc3OwnershipAwareLoweringBehaviorScaffold(...)` computes
   conformance-matrix fail-closed from recovery/determinism readiness and parse
   conformance-matrix consistency/key continuity.
4. `IsObjc3OwnershipAwareLoweringBehaviorConformanceMatrixReady(...)` fails
   closed when conformance consistency/readiness or conformance-key continuity
   drifts.
5. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` enforces explicit
   fail-closed lane-B conformance-matrix gating with deterministic diagnostic
   code `O3L319`.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m228-b009-ownership-aware-lowering-behavior-conformance-matrix-implementation-contract`
  - `test:tooling:m228-b009-ownership-aware-lowering-behavior-conformance-matrix-implementation-contract`
  - `check:objc3c:m228-b009-lane-b-readiness`
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m228-b008-lane-b-readiness`
  - `check:objc3c:m228-b009-lane-b-readiness`

## Architecture and Spec Anchors

Shared-file deltas required for full lane-B readiness.

- `native/objc3c/src/ARCHITECTURE.md` includes M228 lane-B B009 conformance
  matrix implementation anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes M228 lane-B B009
  conformance-matrix fail-closed wiring text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B B009
  conformance-matrix metadata anchors.

## Validation

- `python scripts/check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py`
- `python scripts/check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m228-b009-lane-b-readiness`

## Evidence Path

- `tmp/reports/m228/M228-B009/ownership_aware_lowering_behavior_conformance_matrix_implementation_contract_summary.json`
