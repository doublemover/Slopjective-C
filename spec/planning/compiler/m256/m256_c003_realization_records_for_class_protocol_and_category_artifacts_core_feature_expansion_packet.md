# M256-C003 Realization Records for Class, Protocol, and Category Artifacts Core Feature Expansion Packet

Packet: `M256-C003`
Milestone: `M256`
Wave: `W48`
Lane: `C`
Issue: `#7138`
Contract ID: `objc3c-executable-realization-records/m256-c003-v1`
Dependencies: `M256-C002`

## Objective

Emit realization-ready class/protocol/category records that preserve the owner and graph edges needed by the runtime realization tranche, without breaking the already-live executable method-body path.

## Canonical Realization Surface

- contract id `objc3c-executable-realization-records/m256-c003-v1`
- class record model `class-and-metaclass-records-carry-bundle-object-and-super-owner-identities-plus-method-list-refs`
- protocol record model `protocol-records-carry-owner-inherited-protocol-edges-and-split-instance-class-method-counts`
- category record model `category-records-carry-explicit-class-and-category-owner-identities-plus-attachment-and-adopted-protocol-edges`
- fail-closed model `no-identity-edge-elision-no-out-of-band-graph-reconstruction`
- emitted IR comment `; executable_realization_records = ...`

## Acceptance Criteria

- Add explicit C003 constants in `native/objc3c/src/lower/objc3_lowering_contract.h`.
- Add one deterministic summary helper in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Keep `native/objc3c/src/parse/objc3_parser.cpp` explicit that parser still stops at canonical bundle/object/attachment identities.
- Keep `native/objc3c/src/sema/objc3_semantic_passes.cpp` explicit that sema still owns legality and canonical superclass/protocol/category identities.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp`:
  - publish `; executable_realization_records = ...`
  - expand class/metaclass records with bundle-owner, object-owner, and super-owner identities
  - expand protocol records with split instance/class method counts
  - expand category records with explicit class/category owner identities while retaining attachment/adopted-protocol aggregates
- Update `native/objc3c/src/runtime/objc3_runtime.cpp` to consume the expanded record layouts without regressing the current dispatch path.
- Add deterministic docs/spec/package/checker/test evidence.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m256_inheritance_override_realization_positive.objc3` proving emitted class bundles carry:
   - `; executable_realization_records = ...`
   - `@__objc3_meta_class_class_object_identity_*`
   - `@__objc3_meta_class_metaclass_object_identity_*`
   - `@__objc3_meta_class_super_class_object_identity_*`
   - `@__objc3_meta_class_super_metaclass_object_identity_*`
2. Native compile probe over `tests/tooling/fixtures/native/m256_protocol_conformance_positive.objc3` proving emitted protocol descriptors carry:
   - inherited protocol refs
   - split instance/class method counts
3. Native compile probe over `tests/tooling/fixtures/native/m256_category_merge_positive.objc3` proving emitted category descriptors carry:
   - explicit class owner identity
   - explicit category owner identity
   - retained attachment and adopted-protocol aggregates
4. Lane-C readiness replays `M256-C002` before the C003 checker so current live runtime dispatch stays covered.

## Non-Goals

- `M256-C003` does not add new metadata section families.
- `M256-C003` does not add new public runtime entrypoints.
- `M256-C003` does not reinterpret parser/sema legality inside lowering.

## Validation

- `python scripts/check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m256_c003_realization_records_for_class_protocol_and_category_artifacts_core_feature_expansion.py -q`
- `npm run check:objc3c:m256-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m256/M256-C003/realization_records_summary.json`
