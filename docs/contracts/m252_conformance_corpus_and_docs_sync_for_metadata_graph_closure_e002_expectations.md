# M252 Conformance Corpus And Docs Sync For Metadata Graph Closure Expectations (E002)

Contract ID: `objc3c-metadata-graph-closure-conformance-corpus-doc-sync/m252-e002-v1`
Status: Accepted
Scope: M252 lane-E cross-lane integration sync that proves representative metadata graph closure cases stay aligned with docs and the real frontend runner path before `M253` section-emission work begins.

## Objective

Fail closed unless representative class/protocol/category/property/ivar corpus coverage stays synchronized with `M252-A003`, `M252-B004`, `M252-C003`, `M252-D002`, and `M252-E001` on the real integrated runner path.

## Dependency Scope

- Dependencies:
  - `M252-A003`
  - `M252-B004`
  - `M252-C003`
  - `M252-D002`
  - `M252-E001`
- Upstream evidence inputs remain mandatory:
  - `tmp/reports/m252/M252-A003/executable_metadata_export_graph_completion_summary.json`
  - `tmp/reports/m252/M252-B004/property_ivar_export_legality_synthesis_preconditions_summary.json`
  - `tmp/reports/m252/M252-C003/metadata_debug_projection_and_replay_anchors_summary.json`
  - `tmp/reports/m252/M252-D002/artifact_packaging_and_binary_boundary_for_metadata_payloads_summary.json`
  - `tmp/reports/m252/M252-E001/metadata_semantic_closure_gate_summary.json`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m252/m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure_packet.md`
  - `scripts/check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py`
  - `tests/tooling/test_check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py`
  - `scripts/run_m252_e002_lane_e_readiness.py`

## Required Invariants

1. Lane-E runs one real integrated corpus over representative closure cases:
   - `class-protocol-property-ivar-runtime-graph`
   - `category-protocol-property-runtime-graph`
   - `class-property-synthesis-ready`
   - `category-property-export-only`
   - `missing-interface-property-diagnostic`
   - `incompatible-property-signature-diagnostic`
2. The gate uses `artifacts/bin/objc3c-frontend-c-api-runner.exe` on real fixtures under `tests/tooling/fixtures/native/`; it does not substitute mock payloads.
3. The positive graph cases keep the executable metadata source graph complete and preserve manifest semantic-surface publication for the real metadata graph path.
4. The positive legality cases keep property/ivar synthesis and export counts deterministic on the same integrated runner path.
5. The negative legality cases keep deterministic `O3S206` diagnostics.
6. `M252-E001` remains the aggregate prerequisite gate and must report `aggregate_semantic_closure_ready_for_section_emission == true`.
7. `package.json` keeps one explicit lane-E readiness entry for this cross-lane sync:
   - `check:objc3c:m252-e002-conformance-corpus-and-docs-sync-for-metadata-graph-closure`
   - `test:tooling:m252-e002-conformance-corpus-and-docs-sync-for-metadata-graph-closure`
   - `check:objc3c:m252-e002-lane-e-readiness`
8. Validation evidence is written to `tmp/reports/m252/M252-E002/conformance_corpus_and_docs_sync_for_metadata_graph_closure_summary.json`.

## Non-Goals And Fail-Closed Rules

- `M252-E002` does not add new parser, sema, lowering, or runtime features.
- `M252-E002` does not claim object-file section emission or startup registration is implemented.
- `M252-E002` exists to prove the representative corpus and docs remain synchronized before `M253` work begins.
- The gate therefore fails closed if any upstream summary drifts, if any representative corpus case regresses, or if the direct readiness runner stops executing the real integrated path.

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

- `package.json` includes `check:objc3c:m252-e002-conformance-corpus-and-docs-sync-for-metadata-graph-closure`.
- `package.json` includes `test:tooling:m252-e002-conformance-corpus-and-docs-sync-for-metadata-graph-closure`.
- `package.json` includes `check:objc3c:m252-e002-lane-e-readiness`.
- `scripts/run_m252_e002_lane_e_readiness.py` executes the real integrated path directly instead of recursively nesting the entire lane stack.

## Validation

- `python scripts/check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py`
- `python -m pytest tests/tooling/test_check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py -q`
- `python scripts/run_m252_e002_lane_e_readiness.py`

## Evidence Path

- `tmp/reports/m252/M252-E002/conformance_corpus_and_docs_sync_for_metadata_graph_closure_summary.json`
