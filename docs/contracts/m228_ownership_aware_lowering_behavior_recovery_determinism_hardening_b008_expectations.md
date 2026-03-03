# M228 Ownership-Aware Lowering Behavior Recovery and Determinism Hardening Expectations (B008)

Contract ID: `objc3c-ownership-aware-lowering-behavior-recovery-determinism-hardening/m228-b008-v1`
Status: Accepted
Scope: ownership-aware lowering recovery/determinism hardening closure on top of B007 diagnostics hardening.

## Objective

Extend lane-B ownership-aware lowering closure with explicit recovery and
determinism consistency/readiness and recovery-key continuity so ownership
lowering remains deterministic and fail-closed before LLVM IR emission.

## Dependency Scope

- Dependencies: `M228-B007`
- M228-B007 remains a mandatory prerequisite for B008 recovery/determinism
  hardening:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_diagnostics_hardening_b007_expectations.md`
  - `scripts/check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
  - `spec/planning/compiler/m228/m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_packet.md`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` carries recovery/determinism
   hardening fields:
   - `recovery_determinism_consistent`
   - `recovery_determinism_ready`
   - `recovery_determinism_key`
2. `BuildObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismKey(...)`
   remains deterministic and keyed by B007 diagnostics hardening plus
   parse-lowering recovery hardening evidence.
3. `BuildObjc3OwnershipAwareLoweringBehaviorScaffold(...)` computes
   recovery/determinism fail-closed from diagnostics hardening continuity and
   parser recovery determinism handoff consistency/key continuity.
4. `IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady(...)` fails
   closed when recovery consistency/readiness or recovery-key continuity drifts.
5. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` enforces explicit
   fail-closed lane-B recovery/determinism gating with deterministic diagnostic
   code `O3L318`.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m228-b008-ownership-aware-lowering-behavior-recovery-determinism-hardening-contract`
  - `test:tooling:m228-b008-ownership-aware-lowering-behavior-recovery-determinism-hardening-contract`
  - `check:objc3c:m228-b008-lane-b-readiness`
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m228-b007-lane-b-readiness`
  - `check:objc3c:m228-b008-lane-b-readiness`

## Architecture and Spec Anchors

Shared-file deltas required for full lane-B readiness.

- `native/objc3c/src/ARCHITECTURE.md` includes M228 lane-B B008 recovery and
  determinism hardening anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes M228 lane-B B008
  recovery/determinism fail-closed wiring text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B B008
  recovery/determinism metadata anchors.

## Validation

- `python scripts/check_m228_b007_ownership_aware_lowering_behavior_diagnostics_hardening_contract.py`
- `python scripts/check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b008_ownership_aware_lowering_behavior_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m228-b008-lane-b-readiness`

## Evidence Path

- `tmp/reports/m228/M228-B008/ownership_aware_lowering_behavior_recovery_determinism_hardening_contract_summary.json`
