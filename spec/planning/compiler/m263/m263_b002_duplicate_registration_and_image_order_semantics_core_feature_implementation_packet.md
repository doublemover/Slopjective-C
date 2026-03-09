# M263-B002 Duplicate-Registration and Image-Order Semantics Packet

Packet: `M263-B002`
Milestone: `M263`
Lane: `B`
Implementation date: `2026-03-09`
Dependencies: `M263-B001`, `M263-A002`, `M254-B002`
Next issue: `M263-B003`

## Purpose

Implement the live semantic bridge that binds duplicate-registration legality and image-order legality to the emitted translation-unit identity key and translation-unit registration-order ordinal before lowering/runtime handoff.

## Scope Anchors

- Contract:
  `docs/contracts/m263_duplicate_registration_and_image_order_semantics_core_feature_implementation_b002_expectations.md`
- Checker:
  `scripts/check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py`
- Tooling tests:
  `tests/tooling/test_check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m263-b002-duplicate-registration-and-image-order-semantics`
  - `test:tooling:m263-b002-duplicate-registration-and-image-order-semantics`
  - `check:objc3c:m263-b002-lane-b-readiness`
- Code anchors:
  - `native/objc3c/src/sema/objc3_sema_contract.h`
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Live Semantic Boundary

- contract id
  `objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1`
- semantic/front-end surface
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_semantics`
- upstream contract ids:
  - `objc3c-runtime-bootstrap-legality-duplicate-order-failure-contract/m263-b001-v1`
  - `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
  - `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
- canonical legality/diagnostic models:
  - `duplicate_registration_policy`
    `fail-closed-by-translation-unit-identity-key`
  - `image_registration_order_invariant`
    `strictly-monotonic-positive-registration-order-ordinal`
  - `cross_image_legality_model`
    `translation-unit-identity-key-and-registration-order-ordinal-govern-bootstrap-legality`
  - `semantic_diagnostic_model`
    `fail-closed-bootstrap-legality-before-runtime-handoff`
- canonical continuity fields:
  - `translation_unit_identity_model`
  - `translation_unit_identity_key`
  - `registration_descriptor_identifier`
  - `image_root_identifier`
  - `registration_descriptor_identity_source`
  - `image_root_identity_source`
  - `translation_unit_registration_order_ordinal`

## Deterministic Probe Minimums

- compiling `m263_bootstrap_legality_explicit.objc3` twice produces the same `translation_unit_identity_key`
- compiling `m263_bootstrap_legality_explicit_peer.objc3` produces a different `translation_unit_identity_key`
- both explicit fixtures preserve the same visible registration/image identifiers
- the default fixture preserves module-derived identifiers while still publishing a non-empty identity key

## Non-Goals

- no multi-image bootstrap execution yet
- no new public runtime API/header/archive surface
- no restart/recovery topology expansion beyond the already-frozen `M263-B001` boundary

## Gate Commands

- `python scripts/check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py -q`
- `npm run check:objc3c:m263-b002-lane-b-readiness`

## Evidence Output

- `tmp/reports/m263/M263-B002/duplicate_registration_and_image_order_semantics_summary.json`
