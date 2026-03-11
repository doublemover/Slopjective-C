# M260 Ownership Attribute Surface For Runtime-Backed Objects And Members Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-runtime-backed-object-ownership-attribute-surface/m260-a002-v1`

Issue: `#7169`

## Objective

Turn the frozen runtime-backed ownership surface into emitted runtime-facing
property/member metadata that later runtime lanes can consume directly without
reconstructing ownership out of manifests or source text.

## Required implementation

1. Add the issue-local assets:
   - `docs/contracts/m260_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation_a002_expectations.md`
   - `spec/planning/compiler/m260/m260_a002_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation_packet.md`
   - `scripts/check_m260_a002_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation.py`
   - `tests/tooling/test_check_m260_a002_ownership_attribute_surface_for_runtime_backed_objects_and_members_core_feature_implementation.py`
   - `scripts/run_m260_a002_lane_a_readiness.py`
2. Add explicit anchors to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `package.json`
3. Complete the emitted ownership surface around:
   - `property_attribute_profile`
   - `ownership_lifetime_profile`
   - `ownership_runtime_hook_profile`
   - `accessor_ownership_profile`
   - `Objc3IRRuntimeMetadataPropertyBundle`
   - `EmittedPropertyDescriptor`
4. The checker must prove the capability with one live compile of:
   - `tests/tooling/fixtures/native/m260_runtime_backed_object_ownership_attribute_surface_positive.objc3`
   It must fail closed unless the manifest property records, emitted IR property
   descriptors, and runtime-facing descriptor structures remain synchronized.
5. `package.json` must wire:
   - `check:objc3c:m260-a002-ownership-attribute-surface-for-runtime-backed-objects-and-members`
   - `test:tooling:m260-a002-ownership-attribute-surface-for-runtime-backed-objects-and-members`
   - `check:objc3c:m260-a002-lane-a-readiness`
6. The contract must explicitly hand off to `M260-B001`.

## Canonical models

- Source model:
  `runtime-backed-property-source-surface-publishes-attribute-lifetime-hook-and-accessor-ownership-profiles`
- Descriptor model:
  `emitted-property-descriptor-records-carry-attribute-lifetime-hook-and-accessor-ownership-strings`
- Runtime model:
  `runtime-backed-property-metadata-consumes-emitted-ownership-strings-without-source-rediscovery`
- Failure model:
  `no-manifest-only-ownership-proof-no-source-recovery-no-live-arc-hook-emission-yet`

## Non-goals

- No live ARC retain/release/autorelease hook emission yet.
- No executable function/method ownership qualifier support yet.
- No `@autoreleasepool` runnable support yet.

## Evidence

- `tmp/reports/m260/M260-A002/runtime_backed_object_ownership_attribute_surface_summary.json`
