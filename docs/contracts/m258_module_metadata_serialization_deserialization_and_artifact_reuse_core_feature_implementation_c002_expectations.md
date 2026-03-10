# M258 Module Metadata Serialization, Deserialization, And Artifact Reuse Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1`
Issue: `#7163`

## Required outcomes

1. The frontend publishes a deterministic semantic-surface contract at:
   - `frontend.pipeline.semantic_surface.objc_serialized_runtime_metadata_artifact_reuse`
2. Emitted `module.runtime-import-surface.json` artifacts carry a nested:
   - `serialized_runtime_metadata_reuse_payload`
3. The nested payload serializes transitive runtime metadata so downstream modules can recover upstream object-model semantics without reparsing source.
4. Downstream imports prefer the serialized reuse payload when present.
5. Reused module names and transitive runtime metadata counts remain deterministic.
6. IR and the public embedding ABI remain explicit that runtime registration and direct imported-payload IR lowering are still not landed in lane-C.
7. Code/spec/package anchors remain explicit and deterministic.
8. Validation evidence lands at:
   - `tmp/reports/m258/M258-C002/module_metadata_artifact_reuse_summary.json`

## Proof obligations

- An upstream module emits a runtime-import-surface artifact.
- A middle module imports that upstream artifact and emits a runtime-import-surface artifact whose nested reuse payload carries both modules.
- A downstream module imports only the middle artifact and successfully recovers the upstream metadata through the nested payload reuse path.
- The downstream manifest publishes the C002 semantic surface with the reused module set and transitive metadata counts.
