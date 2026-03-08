# M254-D002 Registrar Implementation and Image Walk Core Feature Implementation Packet

Packet: `M254-D002`
Milestone: `M254`
Lane: `D`
Dependencies: `M254-D001`, `M254-C002`, `M254-C003`

## Purpose

Implement the live runtime registrar/image-walk bridge so emitted startup code
can stage the emitted registration table before calling the frozen D001 public
runtime API, and so the runtime can validate and retain the discovered metadata
roots for later realization.

## Scope Anchors

- Contract:
  `docs/contracts/m254_registrar_implementation_and_image_walk_core_feature_implementation_d002_expectations.md`
- Checker:
  `scripts/check_m254_d002_registrar_implementation_and_image_walk_core_feature_implementation.py`
- Tooling tests:
  `tests/tooling/test_check_m254_d002_registrar_implementation_and_image_walk_core_feature_implementation.py`
- Runtime probe:
  `tests/tooling/runtime/m254_d002_runtime_registrar_image_walk_probe.cpp`
- Lane readiness:
  `scripts/run_m254_d002_lane_d_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m254-d002-registrar-implementation-and-image-walk-core-feature-implementation`
  - `test:tooling:m254-d002-registrar-implementation-and-image-walk-core-feature-implementation`
  - `check:objc3c:m254-d002-lane-d-readiness`

## Runtime Boundary

- Contract id `objc3c-runtime-bootstrap-registrar-image-walk/m254-d002-v1`
- Semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_registrar_contract`
- Private bootstrap header
  `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- Private bridge symbols:
  - `objc3_runtime_stage_registration_table_for_bootstrap`
  - `objc3_runtime_copy_image_walk_state_for_testing`
- Canonical policies:
  - image-walk model
    `registration-table-roots-validated-and-staged-before-realization`
  - discovery-root validation model
    `linker-anchor-must-point-at-discovery-root`
  - selector-pool interning model
    `canonical-selector-pool-preinterned-during-startup-image-walk`
  - realization staging model
    `registration-table-roots-retained-for-later-realization`

## Acceptance Checklist

- the emitted init stub stages the registration table before the frozen D001
  register-image call
- the runtime validates the staged registration table and discovery/linker-root
  relationship
- the runtime retains the walked roots for later realization work
- selector pool entries are preinterned during the image walk
- `module.runtime-registration-manifest.json` publishes the registrar fields
- evidence is written to
  `tmp/reports/m254/M254-D002/registrar_image_walk_summary.json`
