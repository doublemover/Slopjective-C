# M260 Ownership Surface Closure For Runtime-Backed Objects Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-runtime-backed-object-ownership-surface-freeze/m260-a001-v1`

Issue: `#7168`

## Objective

Freeze the truthful ownership surface that already exists for the current
runtime-backed object slice so later `M260` implementation issues extend one
explicit boundary instead of over-claiming ARC/runtime support early.

## Required implementation

1. Add the issue-local assets:
   - `docs/contracts/m260_ownership_surface_closure_for_runtime_backed_objects_contract_and_architecture_freeze_a001_expectations.md`
   - `spec/planning/compiler/m260/m260_a001_ownership_surface_closure_for_runtime_backed_objects_contract_and_architecture_freeze_packet.md`
   - `scripts/check_m260_a001_ownership_surface_closure_for_runtime_backed_objects_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m260_a001_ownership_surface_closure_for_runtime_backed_objects_contract_and_architecture_freeze.py`
   - `scripts/run_m260_a001_lane_a_readiness.py`
2. Add explicit anchors to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `package.json`
3. The freeze must preserve the current truthful boundary:
   - runtime-backed object property/accessor ownership profiles are real emitted evidence
   - scalar `assign` properties already lower to `unowned-unsafe` ownership profiles
   - legacy ownership lowering summaries remain authoritative for ownership qualifier, retain/release, weak/unowned, and ARC fix-it surfaces
   - executable function/method ownership qualifiers remain fail-closed outside the runnable slice
   - `@autoreleasepool` remains fail-closed outside the runnable slice
   - live ARC retain/release/autorelease runtime semantics do not land here
4. The checker must prove the boundary with one live compile of:
   - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
5. `package.json` must wire:
   - `check:objc3c:m260-a001-ownership-surface-closure-for-runtime-backed-objects`
   - `test:tooling:m260-a001-ownership-surface-closure-for-runtime-backed-objects`
   - `check:objc3c:m260-a001-lane-a-readiness`
6. The contract must explicitly hand off to `M260-A002`.

## Canonical models

- Surface model:
  `runtime-backed-object-ownership-surface-freezes-property-accessor-and-legacy-lowering-ownership-profiles-before-live-arc-runtime-semantics`
- Evidence model:
  `canonical-runnable-sample-manifest-and-ir-ownership-profile-proof`
- Failure model:
  `fail-closed-on-ownership-surface-drift-or-premature-arc-runnable-claim`

## Evidence

- `tmp/reports/m260/M260-A001/runtime_backed_object_ownership_surface_contract_summary.json`
