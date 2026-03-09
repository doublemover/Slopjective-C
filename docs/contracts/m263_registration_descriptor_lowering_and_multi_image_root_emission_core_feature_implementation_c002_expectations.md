# M263 Registration-Descriptor Lowering And Multi-Image Root Emission Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1`
Status: Accepted
Issue: `#7226`
Scope: M263 lane-C implementation of first-class emitted registration-descriptor and image-root globals/sections above the frozen `M263-C001` bootstrap table path.

## Objective

Turn the `M263-A002` descriptor/image-root identities into real emitted LLVM/object artifacts. The compiler must now materialize dedicated registration-descriptor and image-root globals, attach them to canonical runtime sections, and retain them through object emission so later multi-image runtime work consumes binary evidence instead of only JSON sidecars.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1`
   - lowering model `frontend-identifiers-drive-emitted-registration-descriptor-and-image-root-globals`
   - logical sections `objc3.runtime.registration_descriptor` and `objc3.runtime.image_root`
   - symbol prefixes `__objc3_runtime_registration_descriptor_` and `__objc3_runtime_image_root_`
2. `lower/objc3_lowering_contract.cpp` publishes one replay-stable lowering summary for the live descriptor/image-root emission surface.
3. `ir/objc3_ir_emitter.cpp` must emit:
   - one `runtime_registration_descriptor_image_root_lowering` boundary line
   - one retained image-root global in `objc3.runtime.image_root`
   - one retained registration-descriptor global in `objc3.runtime.registration_descriptor`
4. The emitted image-root payload must point at:
   - the image-root identifier string
   - the module name string
   - the live image descriptor global
   - the live registration table global
   - the live discovery-root global
5. The emitted registration-descriptor payload must point at:
   - the registration-descriptor identifier string
   - the emitted image-root global
   - the live image descriptor global
   - the live registration table global
   - the live linker-anchor global
   - the live image-local-init-state global
6. `pipeline/objc3_frontend_artifacts.cpp` must thread the authoritative descriptor/image-root identifiers from the `M263-A002` frontend closure into the IR metadata handoff.

## Dynamic Coverage

1. Native compile probes over:
   - `tests/tooling/fixtures/native/m263_registration_descriptor_image_root_default.objc3`
   - `tests/tooling/fixtures/native/m263_registration_descriptor_image_root_explicit.objc3`
   must each emit:
   - `module.runtime-registration-descriptor.json`
   - `module.ll`
   - `module.obj`
   - `module.object-backend.txt`
2. The default probe must preserve:
   - `AutoBootstrap_registration_descriptor`
   - `AutoBootstrap_image_root`
3. The explicit probe must preserve:
   - `DemoRegistrationDescriptor`
   - `DemoImageRoot`
4. `module.ll` must contain:
   - the `runtime_registration_descriptor_image_root_lowering` line with identifiers, sections, and symbol names
   - `@__objc3_runtime_image_root_...`
   - `@__objc3_runtime_registration_descriptor_...`
   - `section "objc3.runtime.image_root"`
   - `section "objc3.runtime.registration_descriptor"`
   - retention of both globals through `@llvm.used`
5. `llvm-readobj --sections module.obj` must report non-empty `objc3.runtime.image_root` and `objc3.runtime.registration_descriptor` sections.
6. `llvm-objdump --syms module.obj` must report the emitted image-root and registration-descriptor symbols.
7. `module.object-backend.txt` must remain `llvm-direct`.

## Non-Goals and Fail-Closed Rules

- `M263-C002` does not implement cross-translation-unit image-root merging.
- `M263-C002` does not change the frozen `M263-C001` registration-table ABI.
- `M263-C002` does not implement runtime fanout across multiple loaded images yet.
- Later runtime work must consume these emitted globals rather than reconstructing identifier payloads from sidecar JSON at registration time.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m263-c002-registration-descriptor-and-image-root-lowering`.
- `package.json` includes `test:tooling:m263-c002-registration-descriptor-and-image-root-lowering`.
- `package.json` includes `check:objc3c:m263-c002-lane-c-readiness`.

## Validation

- `python scripts/check_m263_c002_registration_descriptor_lowering_and_multi_image_root_emission_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m263_c002_registration_descriptor_lowering_and_multi_image_root_emission_core_feature_implementation.py -q`
- `python scripts/run_m263_c002_lane_c_readiness.py`

## Evidence Path

- `tmp/reports/m263/M263-C002/registration_descriptor_lowering_and_multi_image_root_emission_summary.json`
