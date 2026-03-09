# M263 Registration Descriptor and Image-Root Source Surface Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-bootstrap-registration-descriptor-image-root-source-surface/m263-a001-v1`
Status: Accepted
Issue: `#7220`
Scope: M263 lane-A contract and architecture freeze for the frontend-visible
registration-descriptor and image-root source surface.

## Objective

Freeze how the frontend accepts, resolves, and publishes bootstrap-visible
registration descriptor and image-root identities so later M263 lowering and
runtime work consume one deterministic source-model packet.

Canonical surface path:
`frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_image_root_source_surface`

## Required Invariants

1. The source surface remains real compiler functionality, not a contract-only
   placeholder:
   - `#pragma objc_registration_descriptor(Name)` is accepted in the file-scope
     prelude.
   - `#pragma objc_image_root(Name)` is accepted in the file-scope prelude.
   - module-derived defaults remain available when either pragma is absent.
2. `token/objc3_token_contract.h` remains the canonical declaration point for:
   - `kObjc3BootstrapRegistrationDescriptorPragmaName`
   - `kObjc3BootstrapImageRootPragmaName`
3. `lex/objc3_lexer.cpp` remains the canonical directive ingestion point for:
   - prelude-only placement enforcement
   - duplicate detection
   - identifier capture
4. `parse/objc3_parser.cpp` remains explicit that module identity is the
   parser-owned portion of the A001 source surface while the paired bootstrap
   pragmas are lexer-owned inputs.
5. `pipeline/objc3_frontend_types.h` remains the canonical declaration point
   for `Objc3RuntimeRegistrationDescriptorImageRootSourceSurfaceSummary`.
6. `pipeline/objc3_frontend_artifacts.cpp` remains the canonical publication
   point for:
   - `frontend.bootstrap_registration_source_pragma_contract`
   - `frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_image_root_source_surface`
   - flattened `runtime_registration_descriptor_image_root_source_surface_*`
     summary keys
7. `io/objc3_process.cpp`, `driver/objc3_objc3_path.cpp`, and
   `libobjc3c_frontend/frontend_anchor.cpp` remain explicit that the emitted
   `module.runtime-registration-manifest.json` carries:
   - the A001 contract id
   - the semantic-surface path
   - pragma names
   - resolved registration descriptor identifier
   - resolved image-root identifier
   - identity-source classification
   - bootstrap-visible metadata ownership model
8. The frozen identity-source vocabulary remains:
   - module identity source `module-declaration-or-default`
   - pragma-derived identity source `source-pragma`
   - module-derived identity source `module-derived-default`
9. The frozen ownership model remains:
   - `image-root-owns-registration-descriptor-runtime-owns-bootstrap-state`

## Happy-Path Coverage

The checker must prove two real compile paths:

1. Explicit pragma path
   - source pragmas are present
   - semantic surface reports `source-pragma`
   - emitted registration manifest preserves the explicit names
2. Module-derived default path
   - no bootstrap pragmas are present
   - semantic surface reports `module-derived-default`
   - emitted registration manifest derives `Module_registration_descriptor`
     and `Module_image_root`

## Non-Goals and Fail-Closed Rules

- `M263-A001` does not lower registration descriptors into emitted bootstrap
  tables yet.
- `M263-A001` does not realize multi-image bootstrap behavior yet.
- `M263-A001` does not add runtime replay/discovery execution yet.
- `M263-A002` must preserve this frontend/source-surface freeze while turning it
  into fuller frontend closure and emitted registration-manifest behavior.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m263-a001-registration-descriptor-and-image-root-source-surface-contract`.
- `package.json` includes
  `test:tooling:m263-a001-registration-descriptor-and-image-root-source-surface-contract`.
- `package.json` includes `check:objc3c:m263-a001-lane-a-readiness`.

## Validation

- `python scripts/check_m263_a001_registration_descriptor_and_image_root_source_surface_contract.py`
- `python -m pytest tests/tooling/test_check_m263_a001_registration_descriptor_and_image_root_source_surface_contract.py -q`
- `python scripts/run_m263_a001_lane_a_readiness.py`

## Evidence Path

- `tmp/reports/m263/M263-A001/registration_descriptor_image_root_source_surface_contract_summary.json`
