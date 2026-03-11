# M262 ARC Mode Handling For Methods, Properties, Returns, And Block Captures Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-arc-mode-handling/m262-a002-v1`

Issue: `#7195`

## Objective

Make explicit ARC mode a real compiler-visible execution boundary for
ownership-qualified methods, properties, returns, and block captures without
claiming general ARC automation.

## Required implementation

1. Add a canonical expectations document for ARC mode handling.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-A
   readiness runner:
   - `scripts/check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py`
   - `tests/tooling/test_check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py`
   - `scripts/run_m262_a002_lane_a_readiness.py`
3. Add `M262-A002` anchor text to:
   - `docs/objc3c-native/src/20-grammar.md`
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `native/objc3c/src/driver/objc3_cli_options.cpp`
   - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. The implementation must make the following truthful:
   - the native driver accepts `-fobjc-arc` and `-fno-objc-arc`
   - ARC mode is threaded through frontend, sema, manifests, and IR metadata
   - executable ownership-qualified method and function signatures become
     runnable under `-fobjc-arc`
   - non-ARC mode remains fail-closed for the same executable ownership
     surfaces
   - property and block-capture source surfaces compile under explicit ARC mode
5. The checker must prove the boundary with focused live probes:
   - `tests/tooling/fixtures/native/m262_arc_mode_handling_positive.objc3`
     compiles cleanly under `-fobjc-arc`
   - the same fixture fails closed without ARC mode
   - `tests/tooling/fixtures/native/hello.objc3` compiles cleanly under
     `-fno-objc-arc`
   - manifests carry `arc_mode=enabled|disabled`
   - emitted IR carries:
     - `; arc_mode_handling = ...`
     - `!objc3.objc_arc_mode_handling`
6. `package.json` must wire:
   - `check:objc3c:m262-a002-arc-mode-handling-methods-properties-returns-block-captures-contract`
   - `test:tooling:m262-a002-arc-mode-handling-methods-properties-returns-block-captures-contract`
   - `check:objc3c:m262-a002-lane-a-readiness`
7. The contract must explicitly hand off to `M262-B001`.

## Canonical models

- Source model:
  `ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode`
- Mode model:
  `driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir`
- Failure model:
  `non-arc-mode-still-rejects-executable-ownership-qualified-method-and-function-signatures`
- Non-goal model:
  `no-generalized-arc-cleanup-synthesis-no-implicit-nonarc-promotion-no-full-arc-automation-yet`

## Non-goals

- No full ARC cleanup insertion yet.
- No automatic retain/release/autorelease synthesis for arbitrary executable
  code yet.
- No claim that all ARC lifetime rules are implemented yet.
- No claim that forbidden ARC forms or diagnostics are complete yet.

## Evidence

- `tmp/reports/m262/M262-A002/arc_mode_handling_summary.json`
