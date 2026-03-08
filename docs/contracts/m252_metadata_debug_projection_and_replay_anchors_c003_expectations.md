# M252 Metadata Debug Projection And Replay Anchors Expectations (C003)

Contract ID: `objc3c-executable-metadata-debug-projection/m252-c003-v1`

Scope: Deterministic lane-C manifest and IR inspection matrix for executable metadata graph debug projection.

## Required outcomes

1. `Objc3ExecutableMetadataDebugProjectionSummary` becomes the canonical lane-C debug-projection packet.
2. Manifest JSON and emitted LLVM IR publish the same inspection contract through `frontend.pipeline.semantic_surface.objc_executable_metadata_debug_projection` and `!objc3.objc_executable_metadata_debug_projection`.
3. Matrix row `class-protocol-property-ivar-manifest-projection` maps to `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`.
4. Matrix row `category-protocol-property-manifest-projection` maps to `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`.
5. Matrix row `hello-ir-named-metadata-anchor` maps to `tests/tooling/fixtures/native/hello.objc3`.
6. The two metadata-rich rows inspect `module.manifest.json` and replay the active C002 typed-handoff payload through the debug-projection packet.
7. The hello row inspects `module.ll` and proves the IR named-metadata anchor remains published before runtime section emission lands.
8. Validation evidence is written to `tmp/reports/m252/M252-C003/metadata_debug_projection_and_replay_anchors_summary.json`.

## Dynamic proof points

- A native compile probe on `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3` proves manifest JSON publishes the debug-projection packet, that the matrix stays deterministic, and that the packet carries through the active C002 typed-handoff replay key.
- A native compile probe on `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3` proves the category/protocol/property variant publishes the same manifest debug-projection packet.
- A native compile probe on `tests/tooling/fixtures/native/hello.objc3` proves emitted LLVM IR publishes `!objc3.objc_executable_metadata_debug_projection` with the canonical row descriptors and replay anchors.

## Validation commands

- `python scripts/check_m252_c003_metadata_debug_projection_and_replay_anchors.py`
- `python -m pytest tests/tooling/test_check_m252_c003_metadata_debug_projection_and_replay_anchors.py -q`
- `npm run check:objc3c:m252-c003-lane-c-readiness`
