# M256 Bind Method Bodies to Runtime Metadata Entries Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-executable-method-body-binding/m256-c002-v1`
Status: Accepted
Issue: `#7137`
Scope: M256 lane-C implementation of the executable binding surface that attaches implementation-owned method metadata entries to real callable LLVM definition symbols and proves that runtime dispatch consumes those pointers directly.

## Objective

Turn the frozen `M256-C001` executable-object boundary into a fail-closed capability. Implementation-owned method entries with `has_body = true` must now carry concrete implementation pointers, and runtime dispatch must prove that those pointers execute real class and category method bodies.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-executable-method-body-binding/m256-c002-v1`
   - source model `implementation-owned-method-entry-owner-identity-selects-one-llvm-definition-symbol`
   - runtime model `emitted-method-entry-implementation-pointer-dispatches-through-objc3_runtime_dispatch_i32`
   - fail-closed model `error-on-missing-or-duplicate-implementation-binding`
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic summary for the executable method-body binding surface.
3. `parse/objc3_parser.cpp` remains explicit that parser owns canonical method owner identities only.
4. `sema/objc3_semantic_passes.cpp` remains explicit that sema owns legality and canonical owner identities only.
5. `ir/objc3_ir_emitter.cpp` publishes `; executable_method_body_binding = ...`, binds implementation-owned executable method entries to concrete `@objc3_method_*` symbols, and fails closed on missing or duplicate bindings.
6. Happy-path native emission over `tests/tooling/fixtures/native/m256_c002_method_body_binding.objc3` must keep producing:
   - non-empty `module.ll`
   - non-empty `module.obj`
   - bound class implementation pointers for `value:extra:` and `classValue`
   - a bound category implementation pointer for `tracedValue`

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m256_c002_method_body_binding.objc3` proves the emitted IR carries:
   - `; executable_method_body_binding = ...`
   - `bound_method_entry_count=3`
   - `ptr @objc3_method_Widget_instance_value_extra_`
   - `ptr @objc3_method_Widget_class_classValue`
   - `ptr @objc3_method_Widget_instance_tracedValue`
2. Linked runtime probe `tests/tooling/runtime/m256_c002_method_binding_probe.cpp` proves:
   - instance dispatch resolves `implementation:Widget::instance_method:value:extra:`
   - class dispatch resolves `implementation:Widget::class_method:classValue`
   - category dispatch resolves `implementation:Widget(Tracing)::instance_method:tracedValue`
   - repeat instance/class dispatches reuse cached live entries

## Non-Goals and Fail-Closed Rules

- `M256-C002` does not add new metadata section families.
- `M256-C002` does not add protocol executable realization records.
- `M256-C002` does not change parser or sema ownership of legality.
- If an implementation-owned executable method entry cannot bind to exactly one concrete LLVM definition symbol, IR/object emission must fail closed instead of publishing a null implementation slot.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m256-c002-bind-method-bodies-to-runtime-metadata-entries`.
- `package.json` includes `test:tooling:m256-c002-bind-method-bodies-to-runtime-metadata-entries`.
- `package.json` includes `check:objc3c:m256-c002-lane-c-readiness`.

## Validation

- `python scripts/check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py -q`
- `npm run check:objc3c:m256-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m256/M256-C002/method_body_binding_summary.json`
