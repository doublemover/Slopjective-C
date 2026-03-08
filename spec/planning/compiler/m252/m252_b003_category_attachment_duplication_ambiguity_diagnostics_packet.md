# M252-B003 Category Attachment Duplication Ambiguity Diagnostics Packet

Packet: `M252-B003`
Milestone: `M252`
Lane: `B`

## Objective

Expand lane-B semantic diagnostics so executable metadata graph conflicts fail
closed with precise runtime-metadata messages instead of collapsing into generic
class-duplicate or shape-drift failures.

## Dependencies

- `M252-B002`

## Required anchors

- `docs/contracts/m252_category_attachment_duplication_ambiguity_diagnostics_b003_expectations.md`
- `tests/tooling/fixtures/native/m252_b003_valid_category_attachment.objc3`
- `tests/tooling/fixtures/native/m252_b003_category_attachment_collision.objc3`
- `tests/tooling/fixtures/native/m252_b003_duplicate_runtime_member_ambiguity.objc3`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json`

## Acceptance

- Valid class-plus-category input no longer fails as a duplicate class
  interface/implementation.
- Category attachment collisions emit `O3S261` and `O3S263`.
- Duplicate runtime members emit `O3S262` and preserve the ambiguous graph
  blocker with `O3S263` when attachment candidates are no longer unique.
- Existing incomplete-declaration diagnostics remain stable under `O3S260`.
- Deterministic checker, pytest coverage, and lane-B readiness exist.
- Evidence lands under `tmp/reports/m252/M252-B003/`.

## Commands

- `python scripts/check_m252_b003_category_attachment_duplication_ambiguity_diagnostics.py`
- `python -m pytest tests/tooling/test_check_m252_b003_category_attachment_duplication_ambiguity_diagnostics.py -q`
- `npm run check:objc3c:m252-b003-lane-b-readiness`
