# M256 Runnable Class, Protocol, And Category Execution Matrix Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-runnable-class-protocol-category-execution-matrix/m256-e002-v1`

## Objective

Expand the frozen `M256-E001` conformance gate into one truthful runnable
execution matrix for the current executable class/protocol/category surface.

## Required implementation

1. Add a canonical expectations document for the integrated execution matrix.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-E
   readiness runner:
   - `scripts/check_m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m256_e002_runnable_class_protocol_and_category_execution_matrix_cross_lane_integration_sync.py`
   - `scripts/run_m256_e002_lane_e_readiness.py`
3. Add `M256-E002` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
4. Keep the matrix fail closed over the canonical upstream evidence chain:
   - `tmp/reports/m256/M256-A003/protocol_category_source_surface_completion_for_executable_runtime_summary.json`
   - `tmp/reports/m256/M256-B004/inheritance_override_realization_legality_summary.json`
   - `tmp/reports/m256/M256-C003/realization_records_summary.json`
   - `tmp/reports/m256/M256-D004/canonical_runnable_object_sample_support_summary.json`
   - `tmp/reports/m256/M256-E001/class_protocol_category_conformance_gate_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops
   reporting `ok: true`, loses the contract ids defining the current runnable
   class/protocol/category boundary, or drops the dynamic proof surface required
   by this matrix.
6. The checker must add one live executable inheritance matrix case using:
   - `tests/tooling/fixtures/native/m256_inheritance_override_realization_positive.objc3`
   - `artifacts/bin/objc3c-native.exe`
   - `artifacts/lib/objc3_runtime.lib`
   The linked executable must return exit code `4`.
7. `package.json` must wire:
   - `check:objc3c:m256-e002-runnable-class-protocol-and-category-execution-matrix`
   - `test:tooling:m256-e002-runnable-class-protocol-and-category-execution-matrix`
   - `check:objc3c:m256-e002-lane-e-readiness`
8. The matrix must explicitly hand off to `M257-A001`.

## Canonical models

- Evidence model:
  `a003-b004-c003-d004-e001-summary-chain-plus-live-inheritance-execution`
- Execution matrix model:
  `runnable-class-protocol-category-matrix-composes-upstream-summaries-with-live-inheritance-and-runtime-dispatch-proof`
- Failure model:
  `fail-closed-on-runnable-object-matrix-drift-or-missing-live-runtime-proof`

## Non-goals

- No new parser, sema, lowering, or runtime feature work.
- No new metadata section families.
- No new property / ivar / storage realization work.
- No module/import expansion.
- No bootstrap/runtime-registration feature expansion.

## Evidence

- `tmp/reports/m256/M256-E002/runnable_class_protocol_category_execution_matrix_summary.json`
