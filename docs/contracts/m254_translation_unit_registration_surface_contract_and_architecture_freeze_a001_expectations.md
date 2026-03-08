# M254 Translation-Unit Registration Surface Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-translation-unit-registration-surface-freeze/m254-a001-v1`
Status: Accepted
Issue: `#7101`
Scope: M254 lane-A contract and architecture freeze for translation-unit startup
registration surface publication.

## Objective

Freeze how one compiled translation unit publishes startup registration
requirements and runtime-owned payload inventory before constructor-root
emission and bootstrap behavior land.

Frozen payload model: `runtime-metadata-binary-plus-linker-retention-sidecars-v1`

## Required Invariants

1. `driver/objc3_objc3_path.cpp` remains explicit that the native driver’s
   preregistration payload inventory is:
   - `module.runtime-metadata.bin`
   - `module.runtime-metadata-linker-options.rsp`
   - `module.runtime-metadata-discovery.json`
2. `pipeline/objc3_frontend_types.h` remains the canonical declaration point
   for `Objc3RuntimeTranslationUnitRegistrationContractSummary`.
3. `pipeline/objc3_frontend_artifacts.cpp` remains the canonical manifest
   publication point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_contract`
   - flattened `runtime_translation_unit_registration_*` summary keys
4. The frozen summary preserves one explicit upstream contract chain:
   - `objc3c-executable-metadata-runtime-ingest-binary-boundary/m252-d002-v1`
   - `objc3c-runtime-metadata-archive-and-static-link-discovery/m253-d003-v1`
   - `objc3c-runtime-cross-lane-object-emission-closeout/m253-e002-v1`
   - `objc3c-runtime-support-library-link-wiring/m251-d003-v1`
5. The frozen constructor/ownership boundary remains:
   - constructor root `__objc3_runtime_register_image_ctor`
   - constructor-root ownership model
     `compiler-emits-constructor-root-runtime-owns-registration-state`
   - runtime-owned registration entrypoint `objc3_runtime_register_image`
   - translation-unit identity model
     `input-path-plus-parse-and-lowering-replay`
6. `io/objc3_process.cpp` remains explicit that startup registration will
   consume the emitted linker-response/discovery sidecars rather than rederive
   translation-unit identity or rename the M253 public anchor/discovery model.
7. `tests/tooling/runtime/README.md` remains explicit about the frozen
   preregistration inventory and the fact that no constructor-root emission has
   landed yet.

## Non-Goals and Fail-Closed Rules

- `M254-A001` does not emit a constructor root.
- `M254-A001` does not call `objc3_runtime_register_image` automatically.
- `M254-A001` does not add runtime bootstrap execution.
- `M254-A001` does not broaden the runtime-owned payload inventory beyond the
  binary + linker-response + discovery sidecars.
- `M254-A002` must preserve this boundary while materializing registration
  manifests and constructor-root ownership.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m254-a001-translation-unit-registration-surface-contract`.
- `package.json` includes
  `test:tooling:m254-a001-translation-unit-registration-surface-contract`.
- `package.json` includes `check:objc3c:m254-a001-lane-a-readiness`.

## Validation

- `python scripts/check_m254_a001_translation_unit_registration_surface_contract.py`
- `python -m pytest tests/tooling/test_check_m254_a001_translation_unit_registration_surface_contract.py -q`
- `npm run check:objc3c:m254-a001-lane-a-readiness`

## Evidence Path

- `tmp/reports/m254/M254-A001/translation_unit_registration_surface_contract_summary.json`
