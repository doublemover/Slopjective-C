# M253-B001 Layout Ordering and Visibility Policy Contract and Architecture Freeze Packet

Packet: `M253-B001`
Milestone: `M253`
Wave: `W45`
Lane: `B`
Issue: `#7087`
Contract ID: `objc3c-runtime-metadata-layout-ordering-visibility-policy-freeze/m253-b001-v1`
Dependencies: none

## Objective

Freeze the deterministic metadata layout, ordering, relocation, COMDAT,
retention, and visibility policy that the current emitted scaffold already
implements, so `M253-B002` can semantic-finalize one known policy surface
instead of mutating behavior implicitly.

## Canonical Policy Surface

- Family order:
  `image-info`, `class`, `protocol`, `category`, `property`, `ivar`
- Within-family order:
  descriptor ordinals ascend before the family aggregate
- Aggregate relocation model:
  `zero-sentinel-or-count-plus-pointer-vector`
- COMDAT policy:
  `disabled`
- Visibility spelling policy:
  `local-linkage-omits-explicit-ir-visibility`
- Retention ordering:
  `llvm.used-emission-order`
- Object-format policy model:
  `object-format-neutral-until-m253-b003`

## Acceptance Criteria

- Freeze the current metadata layout/visibility boundary with explicit
  fail-closed non-goals.
- Keep the canonical anchors explicit in:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
- Add deterministic docs/spec/package/checker/test evidence.
- Dynamic evidence proves the happy path preserves:
  - manifest matrix row ordering and relocation labels,
  - emitted IR family order,
  - `@llvm.used` retention order,
  - no COMDAT,
  - no explicit hidden visibility spelling on local-linkage metadata globals,
  - successful llvm-direct object emission.

## Dynamic Probes

1. Runner manifest-only probe over
   `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   proving the published source-to-section matrix remains in the frozen row
   order and preserves the relocation labels for emitted vs non-standalone
   rows.
2. Native compile probe over `hello.objc3`
   (`tests/tooling/fixtures/native/hello.objc3`)
   proving the emitted IR/object path preserves:
   - `@__objc3_image_info` first,
   - aggregate section family order,
   - `@llvm.used` retention order,
   - zero-sentinel aggregate payloads,
   - no COMDAT,
   - successful `module.obj` emission.

## Non-Goals

- `M253-B001` does not yet implement semantic finalization of layout decisions.
- `M253-B001` does not add per-target COFF/ELF/Mach-O policy variants.
- `M253-B001` does not add emitted method, selector, or string-pool sections.
- `M253-B001` does not alter the `M253-A001` emitted inventory or `M253-A002`
  completeness matrix.

## Next Issues Must Preserve

- `M253-B002` must preserve the frozen policy while moving the decision point
  into real normalized compiler/runtime capability.
- `M253-B003` must preserve the frozen policy while expanding it into explicit
  COFF/ELF/Mach-O variants.

## Validation Commands

- `python scripts/check_m253_b001_layout_ordering_and_visibility_policy_contract.py`
- `python -m pytest tests/tooling/test_check_m253_b001_layout_ordering_and_visibility_policy_contract.py -q`
- `npm run check:objc3c:m253-b001-lane-b-readiness`

## Evidence Path

- `tmp/reports/m253/M253-B001/layout_ordering_and_visibility_policy_contract_summary.json`
