# M262 ARC Source Surface And Mode Boundary Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-arc-source-mode-boundary-freeze/m262-a001-v1`

Issue: `#7194`

## Objective

Freeze the truthful ARC-adjacent source surface and mode boundary before ARC
automation work begins.

## Required implementation

1. Add a canonical expectations document for the ARC source/mode boundary.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-A
   readiness runner:
   - `scripts/check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py`
   - `scripts/run_m262_a001_lane_a_readiness.py`
3. Add `M262-A001` anchor text to:
   - `docs/objc3c-native/src/20-grammar.md`
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
   - `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Freeze the current boundary around:
   - ownership qualifier, weak/unowned, `@autoreleasepool`, and ARC fix-it
     source surfaces remaining live and deterministic
   - executable function/method ownership qualifiers still failing closed with
     `O3S221`
   - the native driver still rejecting `-fobjc-arc`
   - `Objc3ArcSourceModeBoundarySummary()`
5. The checker must prove the boundary with focused live probes:
   - `tests/tooling/fixtures/native/hello.objc3` compiles and emits the
     boundary line and named metadata
   - `tests/tooling/fixtures/native/m260_runtime_backed_object_ownership_attribute_surface_positive.objc3`
     compiles as the current ownership/property baseline
   - `tests/tooling/fixtures/native/m259_b002_unsupported_feature_claim_arc_ownership_qualifier.objc3`
     still fails with `O3S221`
   - passing `-fobjc-arc` still fails closed with the driver `unknown arg`
     rejection
6. `package.json` must wire:
   - `check:objc3c:m262-a001-arc-source-surface-mode-boundary-contract`
   - `test:tooling:m262-a001-arc-source-surface-mode-boundary-contract`
   - `check:objc3c:m262-a001-lane-a-readiness`
7. The contract must explicitly hand off to `M262-A002`.

## Canonical models

- Source model:
  `ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode`
- Mode model:
  `native-driver-rejects-fobjc-arc-while-executable-ownership-qualified-functions-and-methods-stay-fail-closed`
- Failure model:
  `fail-closed-on-arc-source-mode-boundary-drift-before-arc-automation`
- Non-goal model:
  `no-fobjc-arc-cli-mode-no-fno-objc-arc-cli-mode-no-automatic-arc-cleanup-insertion-no-user-visible-arc-runtime-mode-split`

## Non-goals

- No runnable `-fobjc-arc` or `-fno-objc-arc` mode yet.
- No generalized ARC automation for executable functions or methods yet.
- No automatic cleanup/retain-release synthesis beyond the already-landed
  runtime-backed ownership/property baseline.
- No claim that executable ownership-qualified functions or methods are runnable
  yet.

## Evidence

- `tmp/reports/m262/M262-A001/arc_source_mode_boundary_summary.json`
