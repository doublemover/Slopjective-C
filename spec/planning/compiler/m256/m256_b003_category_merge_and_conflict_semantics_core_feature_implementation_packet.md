# M256-B003 Category Merge and Conflict Semantics Core Feature Implementation Packet

Packet: `M256-B003`
Milestone: `M256`
Lane: `B`
Issue: `#7134`
Contract ID: `objc3c-category-merge-conflict-semantics/m256-b003-v1`
Dependencies: `M256-B002`, `M256-A003`
Next issue: `M256-B004`

## Summary

Implement live lane-B category merge legality for realized classes so attached category members form a deterministic semantic surface used by concrete resolution and protocol conformance while incompatible attachments fail closed.

## Acceptance criteria

- Realized-class category attachments build a deterministic merged category surface in source declaration order.
- Concrete known-owner message resolution can resolve attached category members through the merged surface.
- Declared protocol conformance can satisfy requirements through the merged surface.
- Missing realized-class category interface/implementation pairs fail closed with `O3S219`.
- Incompatible attached category methods fail closed with `O3S219`.
- Incompatible attached category properties fail closed with `O3S219`.
- Unrealized metadata-only category fixtures remain non-blocking.
- Positive proof compiles through `artifacts/bin/objc3c-native.exe` on the `llvm-direct` backend.
- Evidence lands at `tmp/reports/m256/M256-B003/category_merge_and_conflict_semantics_summary.json`.

## Ownership boundary

- Parser owns category identity and declaration order.
- Sema owns realized-class merge construction, conflict handling, concrete resolution, and conformance use of the merged surface.
- IR owns proof publication only and must not reinterpret category legality.

## Required anchors

- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Evidence

- Canonical summary path:
  `tmp/reports/m256/M256-B003/category_merge_and_conflict_semantics_summary.json`

## Validation

- `python scripts/check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py -q`
- `npm run check:objc3c:m256-b003-lane-b-readiness`
