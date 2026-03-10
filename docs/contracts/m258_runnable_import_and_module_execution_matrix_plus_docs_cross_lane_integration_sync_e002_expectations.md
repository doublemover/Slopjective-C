# M258 Runnable Import And Module Execution Matrix Plus Docs Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-runnable-import-module-execution-matrix/m258-e002-v1`

Issue: `#7167`

## Objective

Broaden the frozen `M258-E001` gate into one live runnable import/module
execution matrix proving that separate compilation preserves the current
runnable object-model core across module boundaries.

## Required implementation

1. Add a canonical expectations document for the M258 runnable import/module
   execution matrix.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-E
   readiness runner:
   - `scripts/check_m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync.py`
   - `scripts/run_m258_e002_lane_e_readiness.py`
3. Add one live runtime probe that links the separately emitted provider and
   consumer objects into one executable matrix proof:
   - `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
4. Add `M258-E002` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/libobjc3c_frontend/api.h`
5. Keep the matrix fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m258/M258-A002/runtime_aware_import_module_frontend_closure_summary.json`
   - `tmp/reports/m258/M258-B002/imported_runtime_metadata_semantic_rules_summary.json`
   - `tmp/reports/m258/M258-C002/module_metadata_artifact_reuse_summary.json`
   - `tmp/reports/m258/M258-D002/cross_module_runtime_packaging_summary.json`
   - `tmp/reports/m258/M258-E001/cross_module_object_model_gate_summary.json`
6. The live matrix must compile the provider and consumer fixtures separately,
   link them through the emitted cross-module link plan, and prove all of the
   following on the integrated runtime path:
   - startup registers two images in deterministic ordinal order
   - imported and local class dispatch remain non-zero and replay-stable on the
     current integrated runtime path
   - selector lookup snapshots resolve the emitted metadata-backed selectors
   - method-cache snapshots preserve the current truthful fallback-dispatch
     model rather than claiming fully bound source method bodies
   - protocol conformance for `ImportedProvider` against `ImportedWorker`
     succeeds
   - reset clears live state while retaining the bootstrap catalog
   - replay restores the same two-image runnable state and replays the same
     dispatch results
7. `package.json` must wire:
   - `check:objc3c:m258-e002-runnable-import-and-module-execution-matrix-plus-docs`
   - `test:tooling:m258-e002-runnable-import-and-module-execution-matrix-plus-docs`
   - `check:objc3c:m258-e002-lane-e-readiness`
8. The matrix must explicitly hand off to `M259-A001`.

## Canonical models

- Evidence model:
  `a002-b002-c002-d002-e001-summary-chain-plus-live-cross-module-runtime-execution`
- Execution matrix model:
  `runnable-import-module-matrix-composes-upstream-summaries-with-live-two-image-startup-dispatch-selector-cache-and-replay-proof`
- Failure model:
  `fail-closed-on-runnable-import-module-execution-matrix-drift-or-missing-live-runtime-proof`

## Non-goals

- No new parser, sema, lowering, or runtime feature claims.
- No new import syntax surface.
- No public in-memory cross-module import/module ABI.
- No broader method-body inheritance or override semantics across module
  boundaries beyond the currently emitted runnable class-method path.

## Evidence

- `tmp/reports/m258/M258-E002/runnable_import_module_execution_matrix_summary.json`
