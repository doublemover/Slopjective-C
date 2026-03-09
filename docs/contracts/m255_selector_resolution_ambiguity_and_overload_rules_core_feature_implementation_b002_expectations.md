# M255 Selector Resolution, Ambiguity, And Overload Rules Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-selector-resolution-ambiguity/m255-b002-v1`

## Objective

Implement live lane-B selector resolution for concrete Objective-C 3 dispatch
sites without adding overload recovery or a second lowering/runtime resolution
layer.

## Required implementation

1. Add this expectations document, the packet, a deterministic checker, a
   tooling test, and a lane-B readiness runner:
   - `scripts/check_m255_b002_selector_resolution_ambiguity_and_overload_rules_core_feature_implementation.py`
   - `tests/tooling/test_check_m255_b002_selector_resolution_ambiguity_and_overload_rules_core_feature_implementation.py`
   - `scripts/run_m255_b002_lane_b_readiness.py`
2. Add explicit `M255-B002` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
3. Implement one live selector-resolution rule set:
   - concrete `self` receivers resolve against the current implementation/interface owner
   - concrete `super` receivers resolve against the current superclass chain
   - known class-name receivers resolve against the named class owner
   - non-concrete receivers remain runtime-dynamic fallback sites
   - overload recovery remains a non-goal
4. Emit deterministic diagnostics:
   - missing concrete selector => `O3S216`
   - ambiguous concrete selector / incompatible concrete declarations => `O3S217`
5. Add canonical proof fixtures:
   - `tests/tooling/fixtures/native/m255_selector_resolution_positive.objc3`
   - `tests/tooling/fixtures/native/m255_selector_resolution_missing_selector.objc3`
   - `tests/tooling/fixtures/native/m255_selector_resolution_ambiguous_signature.objc3`
6. `package.json` must wire:
   - `check:objc3c:m255-b002-selector-resolution-ambiguity-and-overload-rules-core-feature-implementation`
   - `test:tooling:m255-b002-selector-resolution-ambiguity-and-overload-rules-core-feature-implementation`
   - `check:objc3c:m255-b002-lane-b-readiness`

## Non-goals

- No direct-dispatch enablement.
- No overload recovery.
- No runtime-side selector lookup changes.
- No new dispatch ABI.

## Evidence

- `tmp/reports/m255/M255-B002/selector_resolution_ambiguity_summary.json`
