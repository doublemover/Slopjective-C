# M252 Typed Metadata Graph Handoff And Manifest Schema Expectations (C002)

Contract ID: `objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1`
Status: Accepted
Scope: M252 lane-C core feature implementation for the lowering-ready typed metadata graph handoff packet.

## Objective

Materialize one deterministic lowering-ready metadata graph packet so the
frontend publishes the ordered executable metadata payload itself for later
lowering work instead of only count-level handoff summaries.

## Required Invariants

1. `pipeline/objc3_frontend_types.h` defines the concrete lowering packet:
   - `Objc3ExecutableMetadataTypedLoweringHandoff`
   - `IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(...)`
   - packet/projected fields on the typed and parse/lowering surfaces.
2. `pipeline/objc3_frontend_pipeline.cpp` builds the packet through
   `BuildExecutableMetadataTypedLoweringHandoff(...)` from:
   - the ready A003 executable metadata source graph,
   - the B001 semantic-consistency boundary,
   - the B002 semantic-validation surface,
   - the C001 freeze packet.
3. The packet keeps the explicit contract chain:
   - `objc3c-executable-metadata-source-graph-completeness/m252-a002-v1`
   - `objc3c-executable-metadata-semantic-consistency-freeze/m252-b001-v1`
   - `objc3c-executable-metadata-semantic-validation/m252-b002-v1`
   - `objc3c-executable-metadata-lowering-handoff-freeze/m252-c001-v1`
   - `objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1`
4. The packet preserves one explicit manifest schema ordering model:
   - `contract-header-then-source-graph-payload-v1`
5. The packet publishes the ordered metadata graph payload itself under
   `source_graph` instead of regressing to a count-only summary.
6. `pipeline/objc3_frontend_artifacts.cpp` publishes the packet under
   `frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff`.
7. `pipeline/objc3_frontend_artifacts.cpp` projects into
   `frontend.pipeline.parse_lowering_readiness`:
   - `executable_metadata_typed_lowering_handoff_ready`
   - `executable_metadata_typed_lowering_handoff_deterministic`
   - `executable_metadata_typed_lowering_handoff_key`
8. The typed packet must become lowering-ready while preserving the older graph
   evidence packet as pre-lowering input:
   - `ready_for_lowering == true` on the typed handoff packet,
   - `ready_for_lowering == false` on the nested A003 `source_graph` payload.
9. Dynamic validation proves the happy path on both:
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`

## Non-Goals and Fail-Closed Rules

- `M252-C002` does not emit object-file metadata sections yet; that is M253.
- `M252-C002` does not package runtime-ingest binary payloads yet; that is D-lane work.
- `M252-C002` does not add the debug/replay matrix projection; that is `M252-C003`.
- The contract fails closed when:
  - the typed packet disappears or regresses to counts only,
  - the manifest schema ordering model drifts,
  - the C001 freeze counters no longer match the typed payload sizes,
  - the typed packet stops being lowering-ready on the happy path,
  - the parse/lowering projection key no longer matches the packet replay key.

## Reference Fixtures

- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m252-c002-typed-metadata-graph-handoff-and-manifest-schema`.
- `package.json` includes
  `test:tooling:m252-c002-typed-metadata-graph-handoff-and-manifest-schema`.
- `package.json` includes `check:objc3c:m252-c002-lane-c-readiness`.

## Validation

- `python scripts/check_m252_c002_typed_metadata_graph_handoff_and_manifest_schema.py`
- `python -m pytest tests/tooling/test_check_m252_c002_typed_metadata_graph_handoff_and_manifest_schema.py -q`
- `npm run check:objc3c:m252-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m252/M252-C002/typed_metadata_graph_handoff_and_manifest_schema_summary.json`
