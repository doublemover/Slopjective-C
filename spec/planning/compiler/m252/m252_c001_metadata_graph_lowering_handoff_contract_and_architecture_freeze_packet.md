# M252-C001 Metadata Graph Lowering Handoff Contract And Architecture Freeze Packet

Packet: `M252-C001`
Milestone: `M252`
Lane: `C`

## Objective

Freeze the typed lowering handoff for executable metadata graphs so later
lowering/object-emission work consumes one canonical, replayable, fail-closed
schema.

## Dependencies

- `M252-B004`

## Required anchors

- `docs/contracts/m252_metadata_graph_lowering_handoff_contract_and_architecture_freeze_c001_expectations.md`
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

- A canonical `Objc3ExecutableMetadataLoweringHandoffSurface` exists.
- The packet is published in the semantic-surface manifest.
- Parse/lowering readiness projects packet readiness, determinism, and key.
- The packet remains fail-closed and `ready_for_lowering == false`.
- Deterministic checker, pytest coverage, and lane-C readiness exist.
- Evidence lands under `tmp/reports/m252/M252-C001/`.

## Commands

- `python scripts/check_m252_c001_metadata_graph_lowering_handoff_contract.py`
- `python -m pytest tests/tooling/test_check_m252_c001_metadata_graph_lowering_handoff_contract.py -q`
- `npm run check:objc3c:m252-c001-lane-c-readiness`
