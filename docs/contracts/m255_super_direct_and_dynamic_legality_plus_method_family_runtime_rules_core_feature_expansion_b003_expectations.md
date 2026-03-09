# M255 Super, Direct, And Dynamic Legality Plus Method-Family Runtime Rules Core Feature Expansion Expectations (B003)

Contract ID: `objc3c-super-dynamic-method-family/m255-b003-v1`

## Objective

Close the remaining lane-B legality and runtime-rule edges for `super`,
dynamic receivers, and runtime-visible method-family accounting without
introducing direct-dispatch syntax or a second lowering/runtime dispatch path.

## Required implementation

1. Add this expectations document, the packet, a deterministic checker, a
   tooling test, and a lane-B readiness runner:
   - `scripts/check_m255_b003_super_direct_and_dynamic_legality_plus_method_family_runtime_rules_core_feature_expansion.py`
   - `tests/tooling/test_check_m255_b003_super_direct_and_dynamic_legality_plus_method_family_runtime_rules_core_feature_expansion.py`
   - `scripts/run_m255_b003_lane_b_readiness.py`
2. Add explicit `M255-B003` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
3. Expand lane-B legality so:
   - `super` outside an implementation method fails closed with `O3S216`
   - `super` in a root implementation with no superclass fails closed with `O3S216`
   - direct dispatch remains reserved/non-goal
   - admitted dynamic receiver sites stay on the runtime dispatch path
4. Keep runtime-visible method-family accounting intact for admitted `super`
   and dynamic sites. The positive corpus must prove:
   - `super` dispatch sites `4`
   - dynamic dispatch sites `3`
   - direct dispatch sites `0`
   - method-family totals `init=1 copy=2 mutableCopy=1 new=2 none=1`
5. Add canonical proof fixtures:
   - `tests/tooling/fixtures/native/m255_super_dynamic_method_family_edges.objc3`
   - `tests/tooling/fixtures/native/m255_super_outside_method.objc3`
   - `tests/tooling/fixtures/native/m255_super_root_dispatch.objc3`
6. `package.json` must wire:
   - `check:objc3c:m255-b003-super-direct-and-dynamic-legality-plus-method-family-runtime-rules-core-feature-expansion`
   - `test:tooling:m255-b003-super-direct-and-dynamic-legality-plus-method-family-runtime-rules-core-feature-expansion`
   - `check:objc3c:m255-b003-lane-b-readiness`

## Non-goals

- No direct-dispatch enablement.
- No new dispatch ABI.
- No second runtime dispatch symbol or overload-recovery layer.
- No runtime-side selector lookup changes in this issue.

## Evidence

- `tmp/reports/m255/M255-B003/super_direct_dynamic_method_family_summary.json`
