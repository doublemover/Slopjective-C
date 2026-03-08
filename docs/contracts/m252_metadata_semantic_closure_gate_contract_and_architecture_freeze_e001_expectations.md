# M252 Metadata Semantic-Closure Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-executable-metadata-semantic-closure-gate/m252-e001-v1`
Status: Accepted
Scope: M252 lane-E aggregate gate that proves the executable metadata graph is complete, deterministic, and semantically closed enough for `M253-A001` section-emission work without claiming section emission or runtime bootstrap is already implemented.

## Objective

Fail closed unless one canonical aggregate gate keeps graph completeness, legality diagnostics, debug/replay inspection, and runtime-facing packaging synchronized across `M252-A003`, `M252-B004`, `M252-C003`, and `M252-D002`.

## Dependency Scope

- Dependencies: none
- Upstream evidence inputs remain mandatory:
  - `tmp/reports/m252/M252-A003/executable_metadata_export_graph_completion_summary.json`
  - `tmp/reports/m252/M252-B004/property_ivar_export_legality_synthesis_preconditions_summary.json`
  - `tmp/reports/m252/M252-C003/metadata_debug_projection_and_replay_anchors_summary.json`
  - `tmp/reports/m252/M252-D002/artifact_packaging_and_binary_boundary_for_metadata_payloads_summary.json`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m252/m252_e001_metadata_semantic_closure_gate_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m252_e001_metadata_semantic_closure_gate.py`
  - `tests/tooling/test_check_m252_e001_metadata_semantic_closure_gate.py`

## Required Invariants

1. Lane-E freezes one canonical aggregate gate over `M252-A003`, `M252-B004`, `M252-C003`, and `M252-D002`.
2. `M252-A003` remains the canonical proof that the executable metadata source graph is complete for classes, metaclasses, protocols, categories, properties, methods, and ivars.
3. `M252-B004` remains the canonical proof that property/ivar export legality and synthesis preconditions fail closed with deterministic `O3S206` diagnostics.
4. `M252-C003` remains the canonical proof that manifest and IR inspection anchors stay aligned with the lowering-ready metadata graph and replay keys.
5. `M252-D002` remains the canonical proof that one runtime-facing `module.runtime-metadata.bin` envelope packages the D001/C002/C003 payloads without reparsing manifest JSON.
6. The next implementation issue `M253-A001` must preserve this aggregate semantic-closure boundary while adding real metadata section emission.
7. `package.json` keeps one explicit lane-E readiness entry for this aggregate gate:
   - `check:objc3c:m252-e001-metadata-semantic-closure-gate`
   - `test:tooling:m252-e001-metadata-semantic-closure-gate`
   - `check:objc3c:m252-e001-lane-e-readiness`
8. Validation evidence is written to `tmp/reports/m252/M252-E001/metadata_semantic_closure_gate_summary.json`.

## Non-Goals And Fail-Closed Rules

- `M252-E001` does not claim object-file metadata section emission is implemented.
- `M252-E001` does not claim startup registration or runtime loader bootstrap is implemented.
- `M252-E001` does not claim executable classes, protocols, categories, properties, or ivars are live runtime entities yet.
- `M252-E001` exists to freeze the aggregate pre-section-emission evidence boundary that `M253-A001` must preserve.
- The gate therefore fails closed if any upstream summary disappears, stops reporting `ok: true`, drops its required probe coverage, or loses the required case-level invariants that make the aggregate closure meaningful.

## Architecture And Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Build And Readiness Integration

- `package.json` includes `check:objc3c:m252-e001-metadata-semantic-closure-gate`.
- `package.json` includes `test:tooling:m252-e001-metadata-semantic-closure-gate`.
- `package.json` includes `check:objc3c:m252-e001-lane-e-readiness`.
- `package.json` keeps the upstream readiness inputs explicit:
  - `check:objc3c:m252-a003-lane-a-readiness`
  - `check:objc3c:m252-b004-lane-b-readiness`
  - `check:objc3c:m252-c003-lane-c-readiness`
  - `check:objc3c:m252-d002-lane-d-readiness`

## Validation

- `python scripts/check_m252_e001_metadata_semantic_closure_gate.py`
- `python -m pytest tests/tooling/test_check_m252_e001_metadata_semantic_closure_gate.py -q`
- `npm run check:objc3c:m252-e001-lane-e-readiness`

## Evidence Path

- `tmp/reports/m252/M252-E001/metadata_semantic_closure_gate_summary.json`
