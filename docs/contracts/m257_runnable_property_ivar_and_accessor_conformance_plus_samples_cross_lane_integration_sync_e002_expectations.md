# M257 Runnable Property, Ivar, And Accessor Conformance Plus Samples Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-runnable-property-ivar-accessor-execution-matrix/m257-e002-v1`

Issue: `#7157`

## Objective

Broaden the frozen `M257-E001` gate into one live executable matrix proving
runtime-backed property storage, synthesized accessors, and property metadata
all work together on the same instance.

## Required implementation

1. Add a canonical expectations document for the runnable property/ivar matrix.
2. Add this packet, a deterministic checker, tooling tests, and a direct lane-E
   readiness runner:
   - `scripts/check_m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m257_e002_runnable_property_ivar_and_accessor_conformance_plus_samples_cross_lane_integration_sync.py`
   - `scripts/run_m257_e002_lane_e_readiness.py`
3. Add the canonical live proof assets:
   - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
   - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
4. Add `M257-E002` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/ast/objc3_ast.h`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
5. Fail closed unless the upstream `A002/B003/C003/D003/E001` summaries remain
   present and valid.
6. The live linked probe must compile and run successfully over the emitted
   object produced from `m257_property_ivar_execution_matrix_positive.objc3`.
7. The live payload must prove:
   - `count` stores and reloads `37`
   - `enabled` stores and reloads `1`
   - `currentValue` stores and reloads `55`
   - `tokenValue` remains the default `0`
   - the realized `Widget` class preserves base identity `1024` and instance size `24`
   - property reflection reports the expected selectors, slot layout, and owner identities
8. `package.json` must wire:
   - `check:objc3c:m257-e002-runnable-property-ivar-and-accessor-conformance-plus-samples`
   - `test:tooling:m257-e002-runnable-property-ivar-and-accessor-conformance-plus-samples`
   - `check:objc3c:m257-e002-lane-e-readiness`
9. The matrix must explicitly hand off to `M258-A001`.

## Canonical models

- Evidence model:
  `a002-b003-c003-d003-e001-summary-chain-plus-live-property-runtime-execution`
- Execution matrix model:
  `runnable-property-ivar-matrix-composes-upstream-summaries-with-live-storage-accessor-and-reflection-proof`
- Failure model:
  `fail-closed-on-runnable-property-ivar-matrix-drift-or-missing-live-runtime-proof`

## Non-goals

- No new property or ivar semantics beyond the live proof surface.
- No public reflection ABI.
- No import/module work; that begins at `M258-A001`.

## Evidence

- `tmp/reports/m257/M257-E002/runnable_property_ivar_execution_matrix_summary.json`
