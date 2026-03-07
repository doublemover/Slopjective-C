# M251 Metadata Completeness and Duplicate Suppression Expectations (B002)

Contract ID: `objc3c-runtime-export-enforcement/m251-b002-v1`
Status: Accepted
Scope: M251 lane-B core feature implementation for fail-closed runtime export enforcement semantics.

## Objective

Turn the B001 runtime export legality freeze into a real fail-closed semantic
barrier so runnable metadata export only proceeds when exported Objective-C
runtime declarations are complete, non-duplicated, redeclaration-compatible,
and shape-stable.

## Required Invariants

1. `pipeline/objc3_frontend_types.h` defines
   `Objc3RuntimeExportEnforcementSummary` as the canonical lane-B enforcement
   packet.
2. `pipeline/objc3_frontend_pipeline.cpp` synthesizes the enforcement packet
   from runtime metadata source records plus the B001 legality boundary.
3. Duplicate runtime identities, incomplete runtime-owned declarations, illegal
   redeclaration mixes, and metadata-shape drift all fail closed before
   lowering begins.
4. Forward `@protocol` declarations remain legal dependency hints rather than
   incomplete export candidates.
5. `pipeline/objc3_frontend_artifacts.cpp` publishes enforcement evidence into
   the manifest and forwards it into IR frontend metadata.
6. `ir/objc3_ir_emitter.h` / `ir/objc3_ir_emitter.cpp` preserve the
   enforcement metadata node in emitted LLVM IR.
7. `driver/objc3_objc3_path.cpp` continues to preserve manifest emission before
   fail-closed exits.
8. `tests/tooling/runtime/objc3_msgsend_i32_shim.c` remains explicitly test-only
   evidence and not the native runtime implementation.

## Negative Cases That Must Fail Closed

- Duplicate class/interface identities.
- Interface/category declarations that remain incomplete for export.
- Interface/implementation or category interface/implementation property or
  method redeclarations whose runtime-visible signatures drift.
- Metadata-shape drift between runtime source ownership records and the B001
  legality boundary.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m251-b002-metadata-completeness-and-duplicate-suppression-semantics`.
- `package.json` includes
  `test:tooling:m251-b002-metadata-completeness-and-duplicate-suppression-semantics`.
- `package.json` includes `check:objc3c:m251-b002-lane-b-readiness`.

## Validation

- `python scripts/check_m251_b002_metadata_completeness_and_duplicate_suppression_semantics.py`
- `python -m pytest tests/tooling/test_check_m251_b002_metadata_completeness_and_duplicate_suppression_semantics.py -q`
- `npm run check:objc3c:m251-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m251/M251-B002/metadata_completeness_and_duplicate_suppression_semantics_summary.json`
