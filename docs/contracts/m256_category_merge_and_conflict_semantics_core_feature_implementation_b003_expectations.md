# M256 Category Merge and Conflict Semantics Core Feature Implementation Expectations (B003)

Contract ID: `objc3c-category-merge-conflict-semantics/m256-b003-v1`
Status: Accepted
Issue: `#7134`
Scope: `M256` lane-B implementation of realized-class category merge legality and deterministic conflict handling.

## Objective

Implement live sema enforcement for realized-class category attachment so attached category members form a deterministic merged surface, participate in concrete resolution and protocol conformance, and fail closed on incompatible conflicts.

## Required implementation

1. Parser category clause handling remains authoritative for category identity and source declaration order.
2. `sema/objc3c/src/sema/objc3_semantic_passes.cpp` must:
   - build one deterministic merged category surface per realized class
   - only enforce merge pairing/conflict rules for realized classes
   - reject realized-class category interface/implementation pairs when one side is missing
   - reject incompatible attached category methods with `O3S219`
   - reject incompatible attached category properties with `O3S219`
   - consult the merged category surface during concrete known-owner message resolution
   - consult the merged category surface during declared protocol conformance checks
3. `ir/objc3_ir_emitter.cpp` remains downstream proof/consumer only for this issue and must not reinterpret category legality.
4. Canonical proof fixtures must include:
   - `tests/tooling/fixtures/native/m256_category_merge_positive.objc3`
   - `tests/tooling/fixtures/native/m256_category_merge_conflicting_method.objc3`
   - `tests/tooling/fixtures/native/m256_category_merge_conflicting_property.objc3`
   - `tests/tooling/fixtures/native/m256_category_merge_missing_pair.objc3`
5. `package.json` must wire:
   - `check:objc3c:m256-b003-category-merge-and-conflict-semantics`
   - `test:tooling:m256-b003-category-merge-and-conflict-semantics`
   - `check:objc3c:m256-b003-lane-b-readiness`
6. Validation evidence lands at `tmp/reports/m256/M256-B003/category_merge_and_conflict_semantics_summary.json`.

## Required diagnostics

- Missing realized-class category implementation => `O3S219`
- Incompatible attached category method => `O3S219`
- Incompatible attached category property => `O3S219`

## Required behavior boundaries

- Merge legality only applies to realized classes.
- Metadata-only orphan categories on unrealized classes remain non-blocking for earlier metadata issues.
- Deterministic conflict reporting must preserve the previously attached category owner in the diagnostic text.

## Non-goals

- No runtime-side category realization yet.
- No emitted metadata ABI change for this issue.
- No generic runtime dispatch rewrite.
- No merge legality for unrealized metadata-only category packets.

## Validation

- `python scripts/check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m256_b003_category_merge_and_conflict_semantics_core_feature_implementation.py -q`
- `npm run check:objc3c:m256-b003-lane-b-readiness`

## Evidence Path

- `tmp/reports/m256/M256-B003/category_merge_and_conflict_semantics_summary.json`
