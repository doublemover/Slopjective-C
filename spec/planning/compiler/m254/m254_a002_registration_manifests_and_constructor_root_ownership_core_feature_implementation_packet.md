# M254-A002 Registration Manifests and Constructor-Root Ownership Core Feature Implementation Packet

Packet: `M254-A002`
Milestone: `M254`
Lane: `A`
Implementation date: `2026-03-08`
Dependencies: `M254-A001`

## Purpose

Implement a real translation-unit registration-manifest artifact and freeze the
constructor-root ownership inputs that later lowering/bootstrap lanes will
consume directly.

## Scope Anchors

- Contract:
  `docs/contracts/m254_registration_manifests_and_constructor_root_ownership_core_feature_implementation_a002_expectations.md`
- Checker:
  `scripts/check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py`
- Tooling tests:
  `tests/tooling/test_check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m254-a002-registration-manifests-and-constructor-root-ownership-core-feature-implementation`
  - `test:tooling:m254-a002-registration-manifests-and-constructor-root-ownership-core-feature-implementation`
  - `check:objc3c:m254-a002-lane-a-readiness`
- Code anchors:
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `tests/tooling/runtime/README.md`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Implemented Boundary

- Contract id `objc3c-translation-unit-registration-manifest/m254-a002-v1`
- Semantic surface
  `frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_manifest`
- Manifest payload model `translation-unit-registration-manifest-json-v1`
- Emitted registration-manifest artifact:
  - `module.runtime-registration-manifest.json`
- Runtime-owned payload continuity:
  - `module.runtime-metadata.bin`
  - `module.runtime-metadata-linker-options.rsp`
  - `module.runtime-metadata-discovery.json`
- Constructor/ownership boundary:
  - constructor root `__objc3_runtime_register_image_ctor`
  - constructor-root ownership model
    `compiler-emits-constructor-root-runtime-owns-registration-state`
  - manifest authority model
    `registration-manifest-authoritative-for-constructor-root-shape`
  - init-stub symbol prefix `__objc3_runtime_register_image_init_stub_`
  - init-stub ownership model
    `lowering-emits-init-stub-from-registration-manifest`
  - constructor priority policy `deferred-until-m254-c001`
- Runtime linkage continuity:
  - runtime archive path `artifacts/lib/objc3_runtime.lib`
  - runtime-owned registration entrypoint `objc3_runtime_register_image`
  - translation-unit identity model `input-path-plus-parse-and-lowering-replay`
- `M254-C001` must consume this manifest while landing constructor-root/init-stub
  emission.

## Non-Goals

- no init-stub emission yet
- no constructor-root emission yet
- no automatic startup registration yet
- no runtime bootstrap execution yet

## Gate Commands

- `python scripts/check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m254_a002_registration_manifests_and_constructor_root_ownership_core_feature_implementation.py -q`
- `npm run check:objc3c:m254-a002-lane-a-readiness`

## Evidence Output

- `tmp/reports/m254/M254-A002/registration_manifests_and_constructor_root_ownership_summary.json`
