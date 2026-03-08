# M252-C002 Typed Metadata Graph Handoff And Manifest Schema Packet

Packet: `M252-C002`
Milestone: `M252`
Lane: `C`

## Objective

Implement the lowering-ready typed metadata graph handoff packet and manifest
schema so downstream lowering consumes one deterministic payload instead of a
count-only boundary.

## Dependencies

- `M252-C001`
- `M252-A003`
- `M252-B004`

## Required anchors

- `docs/contracts/m252_typed_metadata_graph_handoff_and_manifest_schema_c002_expectations.md`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json`

## Acceptance

- A canonical `Objc3ExecutableMetadataTypedLoweringHandoff` packet exists.
- The packet is published under
  `frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff`.
- The packet keeps the ordered metadata graph payload under `source_graph`.
- The packet becomes `ready_for_lowering == true` on the happy path.
- Parse/lowering readiness projects packet readiness, determinism, and key.
- Deterministic checker, pytest coverage, and lane-C readiness exist.
- Evidence lands under `tmp/reports/m252/M252-C002/`.

## Commands

- `python scripts/check_m252_c002_typed_metadata_graph_handoff_and_manifest_schema.py`
- `python -m pytest tests/tooling/test_check_m252_c002_typed_metadata_graph_handoff_and_manifest_schema.py -q`
- `npm run check:objc3c:m252-c002-lane-c-readiness`
