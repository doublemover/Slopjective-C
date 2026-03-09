# M263 Registration Manifest and Descriptor Frontend Closure Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
Status: Accepted
Issue: `#7221`
Scope: M263 lane-A core feature implementation for the registration-manifest and registration-descriptor frontend closure.

## Objective

Turn the frozen `M263-A001` bootstrap naming surface into a real emitted frontend-owned artifact path so later lowering and runtime work consume one deterministic registration-descriptor payload.

Canonical surface path:
`frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_frontend_closure`

Canonical emitted artifact:
`module.runtime-registration-descriptor.json`

## Required Invariants

1. `pipeline/objc3_frontend_types.h` remains the canonical declaration point for `Objc3RuntimeRegistrationDescriptorFrontendClosureSummary`.
2. `pipeline/objc3_frontend_artifacts.cpp` remains the canonical publication point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_frontend_closure`
   - flattened `runtime_registration_descriptor_frontend_closure_*` summary keys
3. `io/objc3_manifest_artifacts.h` and `io/objc3_manifest_artifacts.cpp` remain the canonical artifact-path and artifact-write surface for `module.runtime-registration-descriptor.json`.
4. `io/objc3_process.h` and `io/objc3_process.cpp` remain the canonical descriptor-artifact build surface for:
   - payload model `runtime-registration-descriptor-json-v1`
   - authority model `registration-descriptor-artifact-derived-from-source-surface-and-registration-manifest`
   - payload ownership model `compiler-emits-registration-descriptor-artifact-runtime-consumes-bootstrap-identity`
5. `driver/objc3_objc3_path.cpp` and `libobjc3c_frontend/frontend_anchor.cpp` must fail closed when the registration-descriptor frontend closure is not ready.
6. The emitted descriptor artifact must preserve the resolved registration descriptor and image-root identifiers together with their identity-source classification.
7. The emitted descriptor artifact must preserve the already-frozen bootstrap-visible ownership model from `M263-A001`.
8. The emitted descriptor artifact must preserve the emitted runtime registration entrypoint, runtime-support archive path, constructor-root symbol, derived init-stub symbol, derived registration-table symbol, and derived image-local-init symbol.

## Happy-Path Coverage

The checker must prove two real compile paths:

1. Explicit pragma path
   - `module.runtime-registration-descriptor.json` is emitted
   - semantic-surface closure reports `source-pragma`
   - emitted descriptor artifact preserves the explicit names
2. Module-derived default path
   - `module.runtime-registration-descriptor.json` is emitted
   - semantic-surface closure reports `module-derived-default`
   - emitted descriptor artifact derives `<Module>_registration_descriptor` and `<Module>_image_root`

## Non-Goals and Fail-Closed Rules

- `M263-A002` does not lower bootstrap descriptors into final bootstrap tables yet.
- `M263-A002` does not realize multi-image replay/registration behavior yet.
- `M263-A002` does not add runtime bootstrap execution yet.
- `M263-B001` must preserve this emitted frontend closure while freezing the legality and failure contract above it.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m263-a002-registration-manifest-and-descriptor-frontend-closure`.
- `package.json` includes `test:tooling:m263-a002-registration-manifest-and-descriptor-frontend-closure`.
- `package.json` includes `check:objc3c:m263-a002-lane-a-readiness`.

## Validation

- `python scripts/check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py -q`
- `python scripts/run_m263_a002_lane_a_readiness.py`

## Evidence Path

- `tmp/reports/m263/M263-A002/registration_manifest_and_descriptor_frontend_closure_summary.json`
