# M252-C003 Metadata Debug Projection And Replay Anchors Packet

Packet: `M252-C003`
Milestone: `M252`
Lane: `C`
Issue: `#7080`

## Objective

Project the executable metadata graph into one deterministic manifest/IR inspection matrix so operators can inspect metadata-bearing artifacts before runtime section emission lands.

## Dependencies

- `M252-C002`

## Required contract surface

- Contract id `objc3c-executable-metadata-debug-projection/m252-c003-v1`
- Canonical packet `Objc3ExecutableMetadataDebugProjectionSummary`
- Manifest semantic surface `frontend.pipeline.semantic_surface.objc_executable_metadata_debug_projection`
- Named LLVM metadata `!objc3.objc_executable_metadata_debug_projection`
- Deterministic rows:
  - `class-protocol-property-ivar-manifest-projection`
  - `category-protocol-property-manifest-projection`
  - `hello-ir-named-metadata-anchor`

## Implementation notes

- Publish one shared matrix contract across manifest and IR-facing surfaces instead of inventing separate inspection schemas.
- Carry through the active C002 typed-handoff replay key whenever the current input materializes a typed metadata payload.
- Keep the hello IR anchor row deterministic even when the current input does not materialize the typed metadata payload.
- Preserve explicit parser/sema/code/spec anchors for the issue.

## Validation notes

- Manifest probes use the two metadata-rich fixtures and inspect `module.manifest.json`.
- IR probe uses `tests/tooling/fixtures/native/hello.objc3` and inspects `module.ll` for `!objc3.objc_executable_metadata_debug_projection`.
- Evidence lands under `tmp/reports/m252/M252-C003/metadata_debug_projection_and_replay_anchors_summary.json`.
