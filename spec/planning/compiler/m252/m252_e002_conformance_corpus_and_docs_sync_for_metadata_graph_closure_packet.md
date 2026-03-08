# M252-E002 Conformance Corpus And Docs Sync For Metadata Graph Closure Packet

Packet: `M252-E002`
Milestone: `M252`
Lane: `E`
Issue: `#7084`

## Objective

Add the first real integrated metadata-closure corpus gate that fails closed unless representative class/protocol/category/property/ivar combinations stay aligned with docs and the canonical lane outputs on the same frontend runner path.

## Dependencies

- `M252-A003`
- `M252-B004`
- `M252-C003`
- `M252-D002`
- `M252-E001`

## Required implementation

1. Add a canonical lane-E expectations document for the integrated corpus gate.
2. Add this packet, a deterministic checker, tooling tests, and a direct readiness runner:
   - `scripts/check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py`
   - `tests/tooling/test_check_m252_e002_conformance_corpus_and_docs_sync_for_metadata_graph_closure.py`
   - `scripts/run_m252_e002_lane_e_readiness.py`
3. Add lane-E `M252-E002` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `native/objc3c/src/sema/objc3_sema_contract.h`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
4. Require the checker to load the canonical `M252-E001` summary and fail closed if it is missing or no longer reports `aggregate_semantic_closure_ready_for_section_emission == true`.
5. Require the checker to run representative real runner probes over:
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
   - `tests/tooling/fixtures/native/m252_b004_class_property_synthesis_ready.objc3`
   - `tests/tooling/fixtures/native/m252_b004_category_property_export_only.objc3`
   - `tests/tooling/fixtures/native/m252_b004_missing_interface_property.objc3`
   - `tests/tooling/fixtures/native/m252_b004_incompatible_property_signature.objc3`
6. Require the checker to verify on the integrated path:
   - positive graph cases preserve metadata graph publication and non-empty representative counts,
   - positive legality cases preserve deterministic property/ivar synthesis counts,
   - negative legality cases preserve deterministic `O3S206` diagnostics.
7. Require the readiness runner to execute the real integrated path directly: build once, run terminal A003/B004/C003/D002/E001 check/test steps, then run the E002 checker/test, without recursively nesting the whole lane stack.
8. Wire `package.json` so lane-E readiness points at the direct runner.

## Determinism And Fail-Closed Rules

- `M252-E002` is an integrated gate only; it does not land new language/runtime features.
- The packet therefore fails closed when any dependency drift, representative corpus regression, docs drift, or readiness-runner drift is observed.
- `M252-E002` remains pre-section-emission evidence; `M253` must still implement the actual emission path.

## Validation plan

The checker runs representative real runner probes plus dependency summary validation, and the lane-E readiness runner executes the direct integrated chain once.

## Evidence

- `tmp/reports/m252/M252-E002/conformance_corpus_and_docs_sync_for_metadata_graph_closure_summary.json`
