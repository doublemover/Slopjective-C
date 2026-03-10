# M258 Serialized Metadata Import And Lowering Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-serialized-runtime-metadata-import-lowering/m258-c001-v1`
Issue: `#7162`

## Required outcomes

1. The frontend publishes a deterministic semantic-surface contract at:
   - `frontend.pipeline.semantic_surface.objc_serialized_runtime_metadata_import_lowering_contract`
2. The published contract truthfully states that imported runtime-surface ingest is landed.
3. The published contract truthfully states that serialized imported metadata rehydration is not landed in this lane.
4. The published contract truthfully states that incremental reuse is not landed in this lane.
5. The published contract truthfully states that imported metadata payloads are not lowered into IR in this lane.
6. The public embedding ABI remains explicit that no serialized imported-payload handles or direct lowering hooks are exposed yet.
7. Code/spec/package anchors remain explicit and deterministic.
8. Validation evidence lands at:
   - `tmp/reports/m258/M258-C001/serialized_metadata_import_and_lowering_contract_summary.json`

## Proof obligations

- Happy-path compilation with imported runtime-surface artifacts publishes the serialized import/lowering contract.
- The published contract reports:
  - imported input-path count
  - imported module count
  - imported-surface ingest landed
  - serialized metadata rehydration not landed
  - incremental reuse not landed
  - imported metadata IR lowering not landed
  - public imported-payload ABI exposure not landed
- IR remains explicit that imported runtime metadata payloads still are not rehydrated or lowered in this lane.
- The public embedding ABI remains explicit that serialized imported-payload handles and incremental reuse hooks are still absent.
