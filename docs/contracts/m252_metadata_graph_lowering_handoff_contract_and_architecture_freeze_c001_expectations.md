# M252 Metadata Graph Lowering Handoff Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-executable-metadata-lowering-handoff-freeze/m252-c001-v1`

Scope: M252 lane-C contract and architecture freeze for the typed handoff from
parser/sema executable metadata graph production into later lowering work.

## Required outcome

`M252-C001` introduces one canonical lowering-handoff packet,
`Objc3ExecutableMetadataLoweringHandoffSurface`, and threads it through:

- frontend pipeline construction,
- semantic-surface manifest publication,
- typed sema-to-lowering handoff keys,
- and parse/lowering readiness projection.

The packet must stay fail-closed and `ready_for_lowering == false` while
freezing the schema that later lowering/object-emission issues will consume.

## Required anchors

1. `parse/objc3_parser.cpp` documents that category semantic-link symbols remain
   the canonical parser-owned owner identities for lowering handoff.
2. `sema/objc3_semantic_passes.cpp` documents that semantic integration plus
   typed metadata handoff remain the canonical executable metadata graph source
   for typed lowering handoff packets.
3. `sema/objc3_sema_contract.h` documents `Objc3SemanticTypeMetadataHandoff` as
   the canonical sema-to-lowering schema input for executable metadata graph
   lowering freeze packets.
4. `pipeline/objc3_frontend_types.h` defines:
   - `kObjc3ExecutableMetadataLoweringHandoffContractId`
   - `Objc3ExecutableMetadataLoweringHandoffSurface`
   - readiness checks and typed/parse projection fields.
5. `pipeline/objc3_frontend_pipeline.cpp` builds one canonical
   `Objc3ExecutableMetadataLoweringHandoffSurface` from the executable metadata
   graph, semantic consistency boundary, semantic validation surface, type
   metadata handoff, and deterministic semantic summaries.
6. `pipeline/objc3_frontend_artifacts.cpp` publishes the packet under
   `frontend.pipeline.semantic_surface.objc_executable_metadata_lowering_handoff_surface`
   and projects its readiness/key into `frontend.pipeline.parse_lowering_readiness`.
7. Dynamic validation proves the packet is ready, fail-closed, and not
   lowering-ready on both:
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`

## Non-goals

- `M252-C001` does not emit object-file metadata sections.
- `M252-C001` does not implement runtime ingest packaging.
- `M252-C001` does not flip global lowering admission on for executable
  metadata-driven programs.

## Validation and evidence

- `python scripts/check_m252_c001_metadata_graph_lowering_handoff_contract.py`
- `python -m pytest tests/tooling/test_check_m252_c001_metadata_graph_lowering_handoff_contract.py -q`
- `npm run check:objc3c:m252-c001-lane-c-readiness`
- Evidence path:
  `tmp/reports/m252/M252-C001/metadata_graph_lowering_handoff_contract_summary.json`
