# M252 Artifact Packaging And Binary Boundary For Metadata Payloads Expectations (D002)

Contract ID: `objc3c-executable-metadata-runtime-ingest-binary-boundary/m252-d002-v1`

Scope: Deterministic lane-D implementation of a real runtime-facing binary artifact boundary for executable metadata payloads.

## Required outcomes

1. `Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary` becomes the canonical lane-D binary-boundary packet.
2. Manifest JSON publishes the same packet through `frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_binary_boundary`.
3. The binary boundary stays explicitly chained to the D001 packaging contract, the C002 typed handoff, and the C003 debug projection.
4. The emitted artifact is `module.runtime-metadata.bin`.
5. The binary envelope format remains `objc3-runtime-metadata-envelope-v1`.
6. The binary magic remains `OBJC3RM1`.
7. The deterministic chunk set remains:
   - `runtime_ingest_packaging_contract`
   - `typed_lowering_handoff`
   - `debug_projection`
8. Replay continuity stays explicit through the packaging, typed-handoff, debug-projection, and D002 replay keys.
9. The boundary is a real compiler/runtime capability, not a manifest-only summary.
10. Validation evidence is written to `tmp/reports/m252/M252-D002/artifact_packaging_and_binary_boundary_for_metadata_payloads_summary.json`.

## Dynamic proof points

- A native frontend runner probe on `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3` proves the compiler emits `module.runtime-metadata.bin`.
- The same probe proves the binary envelope header, version, and chunk table are deterministic.
- The binary chunks decode to the canonical D001/C002/C003 payloads rather than an unrelated second schema.

## Validation commands

- `python scripts/check_m252_d002_artifact_packaging_and_binary_boundary_for_metadata_payloads.py`
- `python -m pytest tests/tooling/test_check_m252_d002_artifact_packaging_and_binary_boundary_for_metadata_payloads.py -q`
- `npm run check:objc3c:m252-d002-lane-d-readiness`
