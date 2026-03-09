# M263-A001 Registration Descriptor and Image-Root Source Surface Contract and Architecture Freeze Packet

Packet: `M263-A001`
Milestone: `M263`
Lane: `A`
Freeze date: `2026-03-09`
Dependencies: `M259-E002`
Next issue: `M263-A002`

## Purpose

Freeze the frontend-visible bootstrap naming surface so later M263 frontend,
lowering, and runtime work consume one deterministic packet for registration
descriptor identity, image-root identity, and bootstrap-visible ownership.

## Scope Anchors

- Contract:
  `docs/contracts/m263_registration_descriptor_and_image_root_source_surface_contract_and_architecture_freeze_a001_expectations.md`
- Checker:
  `scripts/check_m263_a001_registration_descriptor_and_image_root_source_surface_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m263_a001_registration_descriptor_and_image_root_source_surface_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m263-a001-registration-descriptor-and-image-root-source-surface-contract`
  - `test:tooling:m263-a001-registration-descriptor-and-image-root-source-surface-contract`
  - `check:objc3c:m263-a001-lane-a-readiness`
- Code anchors:
  - `native/objc3c/src/token/objc3_token_contract.h`
  - `native/objc3c/src/lex/objc3_lexer.cpp`
  - `native/objc3c/src/parse/objc3_parser.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Fail-Closed Boundary

- Contract id
  `objc3c-bootstrap-registration-descriptor-image-root-source-surface/m263-a001-v1`
- Semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_image_root_source_surface`
- Frontend prelude contract path `frontend.bootstrap_registration_source_pragma_contract`
- Canonical pragma names:
  - `objc_registration_descriptor`
  - `objc_image_root`
- Canonical identity-source vocabulary:
  - `module-declaration-or-default`
  - `source-pragma`
  - `module-derived-default`
- Canonical ownership model:
  - `image-root-owns-registration-descriptor-runtime-owns-bootstrap-state`
- Required happy-path proof:
  - explicit pragma-driven identity resolution
  - default module-derived identity resolution
- Emitted registration-manifest carry-through:
  - A001 contract id
  - A001 surface path
  - pragma names
  - resolved registration descriptor identifier
  - resolved image-root identifier
  - identity-source classification
  - ownership model

## Non-Goals

- no bootstrap-table lowering yet
- no multi-image root emission yet
- no runtime replay/discovery execution yet
- no runtime registration realization yet

## Gate Commands

- `python scripts/check_m263_a001_registration_descriptor_and_image_root_source_surface_contract.py`
- `python -m pytest tests/tooling/test_check_m263_a001_registration_descriptor_and_image_root_source_surface_contract.py -q`
- `python scripts/run_m263_a001_lane_a_readiness.py`

## Evidence Output

- `tmp/reports/m263/M263-A001/registration_descriptor_image_root_source_surface_contract_summary.json`
