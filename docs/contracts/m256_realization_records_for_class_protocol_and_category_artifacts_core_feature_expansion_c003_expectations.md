# M256 Realization Records for Class, Protocol, and Category Artifacts Core Feature Expansion Expectations (C003)

Contract ID: `objc3c-executable-realization-records/m256-c003-v1`
Status: Accepted
Issue: `#7138`
Scope: M256 lane-C core feature expansion that preserves realization-ready class/protocol/category graph edges directly inside emitted metadata records so the runtime tranche can consume them without reconstructing source relationships out-of-band.

## Objective

Extend the existing executable object surface so emitted realization records keep the runtime-meaningful owner and graph edges for classes, protocols, and categories while preserving the already-live executable method-body path from `M256-C002`.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-executable-realization-records/m256-c003-v1`
   - class record model `class-and-metaclass-records-carry-bundle-object-and-super-owner-identities-plus-method-list-refs`
   - protocol record model `protocol-records-carry-owner-inherited-protocol-edges-and-split-instance-class-method-counts`
   - category record model `category-records-carry-explicit-class-and-category-owner-identities-plus-attachment-and-adopted-protocol-edges`
   - fail-closed model `no-identity-edge-elision-no-out-of-band-graph-reconstruction`
2. `lower/objc3_lowering_contract.cpp` publishes one deterministic `Objc3ExecutableRealizationRecordsSummary()` surface.
3. `parse/objc3_parser.cpp` remains explicit that parser stops at canonical bundle/object/attachment identities only.
4. `sema/objc3_semantic_passes.cpp` remains explicit that sema owns legality and canonical superclass/protocol/category identities only.
5. `ir/objc3_ir_emitter.cpp` publishes `; executable_realization_records = ...` and emits:
   - class/metaclass records with bundle-owner, object-owner, super-owner, super-bundle, and owner-scoped method-list refs
   - protocol records with inherited protocol refs plus split instance/class method counts
   - category records with explicit class/category owner identities while retaining attachment and adopted-protocol aggregates
6. `runtime/objc3_runtime.cpp` must consume the expanded record layouts without regressing the already-live dispatch path.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m256_inheritance_override_realization_positive.objc3` proves emitted class bundles carry explicit class/metaclass object identities plus super-object identities.
2. Native compile probe over `tests/tooling/fixtures/native/m256_protocol_conformance_positive.objc3` proves emitted protocol records carry inherited protocol refs and split instance/class method counts.
3. Native compile probe over `tests/tooling/fixtures/native/m256_category_merge_positive.objc3` proves emitted category records carry explicit class/category owner identities while retaining attachment and adopted-protocol aggregates.
4. Lane-C readiness still replays `M256-C002` so live runtime dispatch remains valid after the record-layout expansion.

## Non-Goals and Fail-Closed Rules

- `M256-C003` does not introduce new metadata section families.
- `M256-C003` does not move legality ownership out of parser/sema.
- `M256-C003` does not require new public runtime entrypoints.
- If the emitted realization record surface drops one of the required owner/graph edges, the checker must fail closed instead of accepting sidecar reconstruction.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m256-c003-realization-records-for-class-protocol-and-category-artifacts`.
- `package.json` includes `test:tooling:m256-c003-realization-records-for-class-protocol-and-category-artifacts`.
- `package.json` includes `check:objc3c:m256-c003-lane-c-readiness`.

## Validation

- `python scripts/check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py -q`
- `npm run check:objc3c:m256-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m256/M256-C003/realization_records_summary.json`
