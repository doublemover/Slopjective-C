# M254-A001 Translation-Unit Registration Surface Contract and Architecture Freeze Packet

Packet: `M254-A001`
Milestone: `M254`
Lane: `A`
Freeze date: `2026-03-08`
Dependencies: none

## Purpose

Freeze the translation-unit preregistration surface so later startup
registration work extends one deterministic manifest boundary over the emitted
runtime metadata payload inventory, constructor-root reservation, and
runtime-owned registration entrypoint.

## Scope Anchors

- Contract:
  `docs/contracts/m254_translation_unit_registration_surface_contract_and_architecture_freeze_a001_expectations.md`
- Checker:
  `scripts/check_m254_a001_translation_unit_registration_surface_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m254_a001_translation_unit_registration_surface_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m254-a001-translation-unit-registration-surface-contract`
  - `test:tooling:m254-a001-translation-unit-registration-surface-contract`
  - `check:objc3c:m254-a001-lane-a-readiness`
- Code anchors:
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `tests/tooling/runtime/README.md`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Fail-Closed Boundary

- Contract id `objc3c-translation-unit-registration-surface-freeze/m254-a001-v1`
- Manifest surface
  `frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_contract`
- Frozen payload model
  `runtime-metadata-binary-plus-linker-retention-sidecars-v1`
- Frozen runtime-owned payload inventory:
  - `module.runtime-metadata.bin`
  - `module.runtime-metadata-linker-options.rsp`
  - `module.runtime-metadata-discovery.json`
- Frozen constructor/ownership boundary:
  - constructor root `__objc3_runtime_register_image_ctor`
  - constructor-root ownership model
    `compiler-emits-constructor-root-runtime-owns-registration-state`
  - runtime-owned registration entrypoint `objc3_runtime_register_image`
  - translation-unit identity model
    `input-path-plus-parse-and-lowering-replay`
- Upstream contract continuity:
  - `objc3c-executable-metadata-runtime-ingest-binary-boundary/m252-d002-v1`
  - `objc3c-runtime-metadata-archive-and-static-link-discovery/m253-d003-v1`
  - `objc3c-runtime-cross-lane-object-emission-closeout/m253-e002-v1`
  - `objc3c-runtime-support-library-link-wiring/m251-d003-v1`
- `M254-A002` must preserve this freeze while materializing registration
  manifests and constructor-root ownership.

## Non-Goals

- no constructor-root emission yet
- no startup registration yet
- no runtime bootstrap yet
- no additional runtime-owned payload families beyond the binary and linker
  sidecars

## Gate Commands

- `python scripts/check_m254_a001_translation_unit_registration_surface_contract.py`
- `python -m pytest tests/tooling/test_check_m254_a001_translation_unit_registration_surface_contract.py -q`
- `npm run check:objc3c:m254-a001-lane-a-readiness`

## Evidence Output

- `tmp/reports/m254/M254-A001/translation_unit_registration_surface_contract_summary.json`
