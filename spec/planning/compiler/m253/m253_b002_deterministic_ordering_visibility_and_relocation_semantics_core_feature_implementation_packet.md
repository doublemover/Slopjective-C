# M253-B002 Deterministic Ordering, Visibility, and Relocation Semantics Core Feature Implementation Packet

Packet: `M253-B002`
Milestone: `M253`
Wave: `W45`
Lane: `B`
Issue: `#7088`
Contract ID: `objc3c-runtime-metadata-layout-policy/m253-b002-v1`
Dependencies: `M253-B001`, `M252-B004`

## Objective

Implement one real normalized metadata layout policy so runtime metadata section
emission is driven by shared lowering semantics rather than hardcoded emitter
order and relocation behavior.

## Canonical Normalized Policy Surface

- named metadata anchor `!objc3.objc_runtime_metadata_layout_policy`
- metadata node `!55`
- emitted replay comment `; runtime_metadata_layout_policy = ...`
- family order:
  `image-info`, `class`, `protocol`, `category`, `property`, `ivar`
- within-family order:
  descriptor ordinals ascend before the family aggregate
- aggregate relocation model:
  `zero-sentinel-or-count-plus-pointer-vector`
- COMDAT policy:
  `disabled`
- visibility spelling policy:
  `local-linkage-omits-explicit-ir-visibility`
- retention ordering:
  `llvm.used-emission-order`
- object-format policy model:
  `object-format-neutral-until-m253-b003`

## Acceptance Criteria

- Add normalized layout-policy input/output packets and fail-closed builder
  logic in `native/objc3c/src/lower/objc3_lowering_contract.h/.cpp`.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` build the normalized policy
  from the canonical ABI/scaffold inputs and consume that policy for real
  metadata emission order.
- Publish the normalized result into emitted IR as:
  - `!objc3.objc_runtime_metadata_layout_policy = !{!55}`
  - `!55 = !{...}`
  - `; runtime_metadata_layout_policy = ...`
- Keep `native/objc3c/src/io/objc3_process.cpp` explicit that llvm-direct
  object emission preserves the semantic-finalization boundary.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over `hello.objc3` must still emit `module.obj`.

## Dynamic Probes

1. Runner manifest-only probe over
   `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   proving runtime-export/scaffold inputs remain ready and non-zero for the
   descriptor families the normalized policy consumes and writes
   `module.manifest.json`.
2. Native compile probe over `tests/tooling/fixtures/native/hello.objc3`
   proving emitted IR/object output carries:
   - `!objc3.objc_runtime_metadata_layout_policy`
   - `!55`
   - `; runtime_metadata_layout_policy = ...`
   - aggregate section family order
   - zero-sentinel aggregate payloads
   - `@llvm.used` retention order
   - successful `module.obj` emission.

## Non-Goals

- `M253-B002` does not yet add COFF/ELF/Mach-O policy variants.
- `M253-B002` does not add emitted method, selector, or string-pool sections.
- `M253-B002` does not add runtime registration/bootstrap.

## Next Issue Must Preserve

- `M253-B003` must preserve the normalized policy while expanding it into
  explicit COFF/ELF/Mach-O variants.

## Validation Commands

- `python scripts/check_m253_b002_deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m253_b002_deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation.py -q`
- `npm run check:objc3c:m253-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m253/M253-B002/deterministic_ordering_visibility_and_relocation_semantics_core_feature_implementation_summary.json`
