# M262 Cleanup Emission Weak Load-Store Lowering And Lifetime Extension Hooks Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-arc-cleanup-weak-lifetime-hooks/m262-c003-v1`

## Required Proof Surface

- Lane C publishes one canonical lowering contract for the supported ARC cleanup,
  weak current-property, and block-capture lifetime slice.
- Emitted IR publishes the boundary comment
  `; arc_cleanup_weak_lifetime_hooks = ...`.
- Emitted IR publishes named metadata
  `!objc3.objc_arc_cleanup_weak_lifetime_hooks = !{...}`.
- Supported ARC cleanup lowering now materially proves:
  - scope-exit dispose-helper unwinding for supported block captures
  - implicit-exit ARC cleanup before `ret void` and supported scalar returns
  - continued weak current-property helper lowering through
    `objc3_runtime_load_weak_current_property_i32` and
    `objc3_runtime_store_weak_current_property_i32`
- Validation evidence lands at
  `tmp/reports/m262/M262-C003/arc_cleanup_weak_lifetime_hooks_summary.json`.

## Required Anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Dynamic Proof Cases

- `tests/tooling/fixtures/native/m262_arc_property_interaction_positive.objc3`
- `tests/tooling/fixtures/native/m262_arc_cleanup_scope_positive.objc3`
- `tests/tooling/fixtures/native/m262_arc_implicit_cleanup_void_positive.objc3`
