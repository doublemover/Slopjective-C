# M253 Layout Ordering and Visibility Policy Contract and Architecture Freeze Expectations (B001)

Contract ID: `objc3c-runtime-metadata-layout-ordering-visibility-policy-freeze/m253-b001-v1`
Status: Accepted
Issue: `#7087`
Scope: M253 lane-B freeze for the deterministic layout, ordering, relocation,
COMDAT, retention, and visibility policy that the current emitted runtime
metadata scaffold already follows.

## Objective

Freeze one deterministic metadata layout/visibility policy surface before
`M253-B002` turns that policy into a real semantic finalization capability and
before `M253-B003` expands the object-format surface.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point
   for:
   - contract id
     `objc3c-runtime-metadata-layout-ordering-visibility-policy-freeze/m253-b001-v1`
   - family ordering model
     `image-info-then-class-protocol-category-property-ivar`
   - within-family ordering model
     `ascending-descriptor-ordinal-then-family-aggregate`
   - relocation model
     `zero-sentinel-or-count-plus-pointer-vector`
   - COMDAT policy `disabled`
   - object-format policy model
     `object-format-neutral-until-m253-b003`
   - visibility spelling policy
     `local-linkage-omits-explicit-ir-visibility`
   - retention ordering model
     `llvm.used-emission-order`
2. `lower/objc3_lowering_contract.cpp` remains explicit that lowering replay
   keys do not infer, normalize, or rewrite metadata family ordering,
   within-family ordering, relocation, COMDAT, visibility spelling, or
   retention order.
3. `ir/objc3_ir_emitter.cpp` continues to bind the real emitted scaffold to
   the frozen policy by emitting:
   - `@__objc3_image_info` first,
   - section families in `class`, `protocol`, `category`, `property`, `ivar`
     order,
   - descriptor globals before the family aggregate,
   - zero-sentinel aggregate payloads when descriptor counts are zero,
   - retention through `@llvm.used` in emission order,
   - no COMDAT on metadata globals,
   - no explicit `hidden` visibility spelling on local-linkage metadata
     globals.
4. `io/objc3_process.cpp` remains explicit that llvm-direct object emission
   preserves the IR-defined metadata order/policy and may not silently insert
   COMDAT, exported visibility, or object-format-specific rewrites.
5. The current policy remains one object-format-neutral boundary. Per-target
   COFF/ELF/Mach-O policy split remains deferred to `M253-B003`.

## Non-Goals and Fail-Closed Rules

- `M253-B001` does not yet implement semantic finalization of metadata layout
  decisions; that lands in `M253-B002`.
- `M253-B001` does not yet add object-format-specific policy variants; that
  lands in `M253-B003`.
- `M253-B001` does not add new emitted method, selector, or string-pool
  sections.
- `M253-B001` does not relax the existing emitted inventory or matrix boundary
  frozen by `M253-A001` and `M253-A002`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m253-b001-layout-ordering-and-visibility-policy-contract`.
- `package.json` includes
  `test:tooling:m253-b001-layout-ordering-and-visibility-policy-contract`.
- `package.json` includes `check:objc3c:m253-b001-lane-b-readiness`.

## Validation

- `python scripts/check_m253_b001_layout_ordering_and_visibility_policy_contract.py`
- `python -m pytest tests/tooling/test_check_m253_b001_layout_ordering_and_visibility_policy_contract.py -q`
- `npm run check:objc3c:m253-b001-lane-b-readiness`

## Evidence Path

- `tmp/reports/m253/M253-B001/layout_ordering_and_visibility_policy_contract_summary.json`
