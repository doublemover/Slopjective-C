# M256-C002 Bind Method Bodies to Runtime Metadata Entries Core Feature Implementation Packet

Packet: `M256-C002`
Milestone: `M256`
Wave: `W48`
Lane: `C`
Issue: `#7137`
Contract ID: `objc3c-executable-method-body-binding/m256-c002-v1`
Dependencies: `M256-C001`, `M256-A002`, `M256-B004`

## Objective

Make the executable method-body binding surface real and fail closed. The native emitter must attach implementation-owned executable method entries to concrete LLVM method bodies, and the live runtime must execute those entries through metadata-backed dispatch.

## Canonical Binding Surface

- contract id `objc3c-executable-method-body-binding/m256-c002-v1`
- source model `implementation-owned-method-entry-owner-identity-selects-one-llvm-definition-symbol`
- runtime model `emitted-method-entry-implementation-pointer-dispatches-through-objc3_runtime_dispatch_i32`
- fail-closed model `error-on-missing-or-duplicate-implementation-binding`
- emitted IR comment `; executable_method_body_binding = ...`

## Acceptance Criteria

- Add explicit executable method-body binding constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add a deterministic summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/parse/objc3_parser.cpp` explicit that parser owns canonical method owner identities only.
- Keep `native/objc3c/src/sema/objc3_semantic_passes.cpp` explicit that sema owns legality and owner identities only.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp`:
  - publish `; executable_method_body_binding = ...`
  - bind implementation-owned executable method entries to concrete `@objc3_method_*` symbols
  - fail closed if a body-bearing implementation entry has no binding or a duplicate binding appears for one canonical method owner identity
- Add deterministic docs/spec/package/checker/test evidence.
- Runnable proof must show class implementation, class method, and category method bodies execute through metadata-backed runtime dispatch.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m256_c002_method_body_binding.objc3` proving emitted IR/object output carries:
   - `; executable_method_body_binding = ...`
   - `bound_method_entry_count=3`
   - class implementation pointer `ptr @objc3_method_Widget_instance_value_extra_`
   - class implementation pointer `ptr @objc3_method_Widget_class_classValue`
   - category implementation pointer `ptr @objc3_method_Widget_instance_tracedValue`
2. Runtime probe `tests/tooling/runtime/m256_c002_method_binding_probe.cpp` proving:
   - instance dispatch returns `20`
   - class dispatch returns `44`
   - category dispatch returns `33`
   - cache entries resolve the expected owner identities

## Non-Goals

- `M256-C002` does not add protocol executable realization records.
- `M256-C002` does not change metadata section family inventory.
- `M256-C002` does not widen parser/sema responsibilities.

## Validation Commands

- `python scripts/check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m256_c002_bind_method_bodies_to_runtime_metadata_entries_core_feature_implementation.py -q`
- `npm run check:objc3c:m256-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m256/M256-C002/method_body_binding_summary.json`
