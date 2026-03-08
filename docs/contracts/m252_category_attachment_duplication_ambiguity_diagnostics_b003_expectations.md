# M252 Category Attachment Duplication Ambiguity Diagnostics Expectations (B003)

Contract ID: `objc3c-runtime-export-diagnostics/m252-b003-v1`

Scope: M252 lane-B core feature expansion for deterministic fail-closed
runtime-metadata diagnostics covering category attachment collisions, duplicate
runtime members, and ambiguous graph-resolution outcomes.

## Required outcome

`M252-B003` does not add a new lowering packet. It hardens the existing runtime
metadata export blocker so that valid class-plus-category programs pass while
invalid category collision and duplicate-member programs fail with precise,
source-anchored diagnostics.

## Required anchors

1. `parse/objc3_parser.cpp` preserves canonical category owner identities for
   `class(category)` containers so diagnostics can report stable owners.
2. `sema/objc3_semantic_passes.cpp` stops treating category containers as class
   interface/implementation map entries, preventing false duplicate-class
   diagnostics on valid class-plus-category input.
3. `sema/objc3_sema_contract.h` documents that class interface/implementation
   maps are class-only and categories are diagnosed through runtime metadata
   conflict analysis.
4. `pipeline/objc3_frontend_pipeline.cpp` extends
   `BuildRuntimeExportBlockingDiagnostics(...)` so the blocker emits:
   - `O3S261` for category attachment collisions,
   - `O3S262` for duplicate runtime members,
   - `O3S263` for ambiguous runtime metadata graph resolution,
   while keeping `O3S260` for incomplete declarations.
5. `pipeline/objc3_frontend_artifacts.cpp` keeps the runtime-export counters
   explicitly serialized so the new diagnostics remain replayable from manifest
   evidence.
6. The happy-path fixture
   `tests/tooling/fixtures/native/m252_b003_valid_category_attachment.objc3`
   succeeds manifest-only with a real class-plus-category pair.
7. Negative fixtures prove:
   - category attachment collisions fail with `O3S261` and `O3S263`,
   - duplicate runtime members fail with `O3S262` and `O3S263`.

## Non-goals

- `M252-B003` does not make metadata lowering-ready.
- `M252-B003` does not add runtime ingest or object-file metadata payloads.
- `M252-B003` does not implement property/ivar export legality beyond the
  diagnostics already exposed by the runtime export blocker.

## Validation and evidence

- `python scripts/check_m252_b003_category_attachment_duplication_ambiguity_diagnostics.py`
- `python -m pytest tests/tooling/test_check_m252_b003_category_attachment_duplication_ambiguity_diagnostics.py -q`
- `npm run check:objc3c:m252-b003-lane-b-readiness`
- Evidence path:
  `tmp/reports/m252/M252-B003/category_attachment_duplication_ambiguity_diagnostics_summary.json`
