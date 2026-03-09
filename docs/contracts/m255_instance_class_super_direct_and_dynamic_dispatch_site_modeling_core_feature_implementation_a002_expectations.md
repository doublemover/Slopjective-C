# M255 Instance, Class, Super, Direct, and Dynamic Dispatch-Site Modeling Core Feature Implementation Expectations (A002)

Implementation Contract ID: `objc3c-dispatch-site-modeling/m255-a002-v1`

Live lowering handoff contract: `objc3c-dispatch-surface-classification/m255-a001-v1`

## Objective

Turn the frozen `M255-A001` dispatch taxonomy into one real frontend/sema/
lowering implementation so native LLVM emission no longer fails on method-body
message sends that reference implicit `self`, implicit `super`, or known class
identifiers.

## Required implementation

1. Add a canonical expectations document for `M255-A002`.
2. Add this packet, a deterministic checker, tooling tests, and a lane-A
   readiness runner:
   - `scripts/check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py`
   - `tests/tooling/test_check_m255_a002_instance_class_super_direct_and_dynamic_dispatch_site_modeling_core_feature_implementation.py`
   - `scripts/run_m255_a002_lane_a_readiness.py`
3. Add `M255-A002` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.h`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
4. Publish explicit AST-side dispatch-site modeling fields for message sends:
   - dispatch surface kind
   - dispatch family symbol
   - dispatch entrypoint family symbol
   - normalization completion flag
5. Normalize dispatch-site kinds before semantic validation using:
   - implicit method-context `self`
   - implicit method-context `super`
   - known class symbols
   - fallback dynamic classification for non-identifier receivers
6. Seed semantic method-body environments so `self`, `super`, and known class
   names stop failing as unresolved identifiers before lowering.
7. Publish the live semantic surface at
   `frontend.pipeline.semantic_surface.objc_dispatch_surface_classification_surface`.
8. Publish the lowering handoff at
   `lowering_dispatch_surface_classification`.
9. Native IR emission must carry the handoff into both:
   - the textual profile line `frontend_objc_dispatch_surface_classification_profile`
   - named metadata `!objc3.objc_dispatch_surface_classification`
10. Add one canonical fixture
    `tests/tooling/fixtures/native/m255_dispatch_surface_modeling.objc3` proving
    these counts:
    - instance dispatch sites `2`
    - class dispatch sites `2`
    - super dispatch sites `1`
    - direct dispatch sites `0`
    - dynamic dispatch sites `1`
11. `objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object` must
    succeed on that fixture and emit the manifest surface/handoff values above.
12. `objc3c-native.exe` must compile that same fixture through `llvm-direct`
    without the old unresolved-`self` lowering failure.
13. `package.json` must wire:
    - `check:objc3c:m255-a002-instance-class-super-direct-and-dynamic-dispatch-site-modeling-core-feature-implementation`
    - `test:tooling:m255-a002-instance-class-super-direct-and-dynamic-dispatch-site-modeling-core-feature-implementation`
    - `check:objc3c:m255-a002-lane-a-readiness`

## Non-goals

- No new direct-dispatch syntax.
- No new runtime entrypoint family beyond the existing live compatibility path.
- No selector ABI widening.
- No full class/super runtime semantic split yet.

## Required live values

- instance/class/super/dynamic entrypoint family:
  `objc3_runtime_dispatch_i32-objc3_msgsend_i32-compat`
- direct entrypoint family:
  `reserved-non-goal`
- lowering lane contract:
  `objc3c-dispatch-surface-classification/m255-a001-v1`

## Evidence

- `tmp/reports/m255/M255-A002/dispatch_site_modeling_summary.json`
