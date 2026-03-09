# M256 Protocol And Category Source Surface Completion For Executable Runtime Core Feature Expansion Expectations (A003)

Contract ID: `objc3c-executable-protocol-category-source-closure/m256-a003-v1`
Status: Accepted
Issue: `#7131`
Scope: `M256` lane-A core feature expansion for executable protocol/category source closure.

## Objective

Expand the executable metadata source graph so runtime-attached protocols and
categories carry one explicit, fail-closed source closure for protocol
inheritance, category attachment, and adopted-protocol conformance before later
`M256` object-model semantic issues consume that graph.

## Required Invariants

1. `parse/objc3_parser.cpp` remains the canonical parser-owned source surface
   for:
   - adopted protocol lists in lexicographic semantic-link order
   - canonical category attachment identities via `BuildObjcContainerScopeOwner(...)`
   - stable interface/implementation owner identities for attached categories
2. `sema/objc3_semantic_passes.cpp` remains the canonical semantic owner for:
   - `protocol_category_composition_summary`
   - `class_protocol_category_linking_summary`
3. `objc3_frontend_types.h` and `objc3_frontend_pipeline.cpp` must publish one
   fail-closed executable source-closure contract for:
   - `protocol_inheritance_identity_closure_complete`
   - `category_attachment_identity_closure_complete`
   - adopted-protocol conformance identities
4. `objc3_frontend_artifacts.cpp` and `ir/objc3_ir_emitter.cpp` must publish the
   same closure through:
   - manifest/source-graph fields
   - `; executable_protocol_category_source_closure = ...`
   - category node flags including `category_attachment_identity_complete`
5. Validation evidence lands under
   `tmp/reports/m256/M256-A003/protocol_category_source_surface_completion_for_executable_runtime_summary.json`.

## Non-Goals and Fail-Closed Rules

- `M256-A003` does not implement live runtime protocol conformance checks.
- `M256-A003` does not implement category merge semantics.
- `M256-A003` does not implement object-model semantic enforcement.
- `M256-A003` must fail closed on drift before `M256-B001` begins object-model
  semantic rule freeze work.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m256-a003-protocol-and-category-source-surface-completion-for-executable-runtime`.
- `package.json` includes
  `test:tooling:m256-a003-protocol-and-category-source-surface-completion-for-executable-runtime`.
- `package.json` includes `check:objc3c:m256-a003-lane-a-readiness`.

## Validation

- `python scripts/check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m256_a003_protocol_and_category_source_surface_completion_for_executable_runtime_core_feature_expansion.py -q`
- `npm run check:objc3c:m256-a003-lane-a-readiness`

## Evidence Path

- `tmp/reports/m256/M256-A003/protocol_category_source_surface_completion_for_executable_runtime_summary.json`
