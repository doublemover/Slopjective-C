# M252 Runtime Ingest Packaging For Metadata Graphs Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-executable-metadata-runtime-ingest-packaging-boundary/m252-d001-v1`

Scope: Deterministic lane-D manifest transport freeze for packaging executable metadata graphs for later runtime ingestion and startup registration.

## Required outcomes

1. `Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary` becomes the canonical lane-D runtime-ingest packaging packet.
2. Manifest JSON publishes the same contract through `frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_packaging_contract`.
3. The boundary stays explicitly chained to the C002 typed handoff and C003 debug-projection packets.
4. The frozen payload model remains `typed-handoff-plus-debug-projection-manifest-v1`.
5. The frozen transport artifact remains `module.manifest.json`.
6. Replay continuity stays explicit through `typed_lowering_handoff_replay_key`, `debug_projection_replay_key`, and `replay_key`.
7. Explicit non-goals remain published: no runtime section emission, no startup registration, and no runtime loader registration in `M252-D001`.
8. Validation evidence is written to `tmp/reports/m252/M252-D001/runtime_ingest_packaging_for_metadata_graphs_contract_summary.json`.

## Dynamic proof points

- A native frontend runner probe on `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3` proves the manifest publishes the runtime-ingest packaging packet over the real metadata-rich typed handoff and debug-projection packets.
- The same probe proves the packet is fail-closed, ready for packaging implementation, and carries forward the upstream replay keys without inventing a second schema.

## Validation commands

- `python scripts/check_m252_d001_runtime_ingest_packaging_for_metadata_graphs_contract.py`
- `python -m pytest tests/tooling/test_check_m252_d001_runtime_ingest_packaging_for_metadata_graphs_contract.py -q`
- `npm run check:objc3c:m252-d001-lane-d-readiness`
