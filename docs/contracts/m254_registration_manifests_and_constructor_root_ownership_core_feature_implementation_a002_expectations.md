# M254 Registration Manifests and Constructor-Root Ownership Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-translation-unit-registration-manifest/m254-a002-v1`
Status: Accepted
Issue: `#7102`
Scope: M254 lane-A core feature implementation for translation-unit registration
manifests and constructor-root ownership.

## Objective

Materialize one real translation-unit registration manifest artifact and one
canonical constructor-root ownership model so later lowering work can emit init
stubs from deterministic manifest inputs instead of reconstructing startup
registration data ad hoc from sidecars.

Manifest payload model:
`translation-unit-registration-manifest-json-v1`

## Required Invariants

1. `driver/objc3_objc3_path.cpp` emits one real
   `module.runtime-registration-manifest.json` artifact on the successful native
   object-emission path.
2. `libobjc3c_frontend/frontend_anchor.cpp` preserves the same manifest
   emission path so frontend-c-api consumers do not drift from the native
   driver.
3. `pipeline/objc3_frontend_types.h` remains the canonical declaration point
   for `Objc3RuntimeTranslationUnitRegistrationManifestSummary`.
4. `pipeline/objc3_frontend_artifacts.cpp` remains the canonical manifest
   publication point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_manifest`
   - flattened `runtime_translation_unit_registration_manifest_*` summary keys
5. `io/objc3_process.cpp` remains the canonical builder for the emitted
   registration-manifest JSON payload, including:
   - translation-unit identity continuity with linker-retention discovery data
   - init-stub symbol derivation from the identity key
   - constructor-root ownership and manifest authority fields
   - runtime-support archive and linker-flag carry-through
6. `io/objc3_manifest_artifacts.*` remains explicit about the canonical emitted
   artifact path `module.runtime-registration-manifest.json`.
7. The emitted registration-manifest artifact preserves these canonical fields:
   - `translation_unit_registration_contract_id`
     `objc3c-translation-unit-registration-surface-freeze/m254-a001-v1`
   - `runtime_support_library_link_wiring_contract_id`
     `objc3c-runtime-support-library-link-wiring/m251-d003-v1`
   - authority model
     `registration-manifest-authoritative-for-constructor-root-shape`
   - init-stub ownership model
     `lowering-emits-init-stub-from-registration-manifest`
   - constructor priority policy `deferred-until-m254-c001`
   - runtime archive path `artifacts/lib/objc3_runtime.lib`
8. `tests/tooling/runtime/README.md` remains explicit that `M254-A002`
   publishes a real registration manifest while still deferring init-stub
   emission and runtime bootstrap execution.

## Non-Goals and Fail-Closed Rules

- `M254-A002` does not emit the init stub yet.
- `M254-A002` does not emit the constructor root yet.
- `M254-A002` does not execute startup registration automatically.
- `M254-A002` does not land runtime bootstrap behavior.
- `M254-C001` must consume this manifest authority model when constructor-root
  and init-stub emission land.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m254-a002-registration-manifests-and-constructor-root-ownership-core-feature-implementation`.
- `package.json` includes
  `test:tooling:m254-a002-registration-manifests-and-constructor-root-ownership-core-feature-implementation`.
- `package.json` includes `check:objc3c:m254-a002-lane-a-readiness`.

## Validation

- `python scripts/check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py -q`
- `npm run check:objc3c:m254-a002-lane-a-readiness`

## Evidence Path

- `tmp/reports/m254/M254-A002/registration_manifests_and_constructor_root_ownership_summary.json`
