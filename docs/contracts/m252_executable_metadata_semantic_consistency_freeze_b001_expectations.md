# M252 Executable Metadata Semantic Consistency Freeze Expectations (B001)

Contract ID: `objc3c-executable-metadata-semantic-consistency-freeze/m252-b001-v1`
Status: Accepted
Scope: M252 lane-B contract and architecture freeze for semantic consistency of the executable metadata graph.

## Objective

Freeze the semantic boundary that decides when the executable metadata graph is complete enough, deterministic enough, and internally consistent enough to be admitted by later lowering work, while still keeping lowering admission itself pending.

## Required Invariants

1. `pipeline/objc3_frontend_types.h` defines `Objc3ExecutableMetadataSemanticConsistencyBoundary` as the canonical lane-B freeze packet.
2. `pipeline/objc3_frontend_pipeline.cpp` synthesizes the boundary from:
   - the ready executable metadata source graph,
   - deterministic protocol/category handoff,
   - deterministic class/protocol/category linking handoff,
   - deterministic selector normalization handoff,
   - deterministic property attribute handoff,
   - deterministic symbol-graph/scope-resolution handoff.
3. The boundary records explicit completeness booleans for:
   - protocol inheritance edges,
   - category attachment edges,
   - declaration/export owner split,
   - property/method/ivar owner-edge coverage.
4. The boundary remains fail-closed while later enforcement work is still pending.
5. `pipeline/objc3_frontend_artifacts.cpp` publishes the boundary into the manifest under `frontend.pipeline.semantic_surface.objc_executable_metadata_semantic_consistency_boundary`.
6. The boundary is allowed to be `ready=true` while:
   - semantic conflict diagnostics enforcement remains pending,
   - duplicate export-owner enforcement remains pending,
   - lowering admission remains pending,
   provided `lowering_admission_ready == false` and the boundary stays fail-closed.

## Non-Goals and Pending Enforcement

- `M252-B001` does not yet emit semantic conflict diagnostics.
- `M252-B001` does not yet reject duplicate export owners.
- `M252-B001` does not yet admit lowering from the semantic-consistency boundary.
- `M252-B001` does not yet wire runtime ingest or startup registration.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m252-b001-executable-metadata-semantic-consistency-freeze`.
- `package.json` includes
  `test:tooling:m252-b001-executable-metadata-semantic-consistency-freeze`.
- `package.json` includes `check:objc3c:m252-b001-lane-b-readiness`.

## Validation

- `python scripts/check_m252_b001_executable_metadata_semantic_consistency_freeze.py`
- `python -m pytest tests/tooling/test_check_m252_b001_executable_metadata_semantic_consistency_freeze.py -q`
- `npm run check:objc3c:m252-b001-lane-b-readiness`

## Evidence Path

- `tmp/reports/m252/M252-B001/executable_metadata_semantic_consistency_summary.json`
