# M263 Duplicate-Registration and Image-Order Semantics Expectations (B002)

Contract ID: `objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1`
Status: Accepted
Issue: `#7223`
Scope: `M263` lane-B core feature implementation for live duplicate-registration, image-order, and cross-image bootstrap legality semantics before lowering/runtime handoff.

## Objective

Land one live semantic bridge that publishes duplicate-registration legality using the emitted `translation_unit_identity_key` and the emitted registration-order ordinal instead of a contract-only freeze.

## Required Invariants

1. `native/objc3c/src/sema/objc3_sema_contract.h` remains the canonical declaration point for `Objc3BootstrapLegalitySemanticsSummary`.
2. `native/objc3c/src/sema/objc3_semantic_passes.cpp` remains the canonical builder point for the sema-owned duplicate/order legality summary and its replay key.
3. `native/objc3c/src/pipeline/objc3_frontend_types.h` remains the canonical declaration point for `Objc3RuntimeBootstrapLegalitySemanticsSummary`.
4. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` remains the canonical manifest publication point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_semantics`
   - flattened `runtime_bootstrap_legality_semantics_*` summary keys
5. The live bridge stays explicitly tied to:
   - `objc3c-runtime-bootstrap-legality-duplicate-order-failure-contract/m263-b001-v1`
   - `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
   - `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
6. The live legality model preserves:
   - duplicate-registration policy `fail-closed-by-translation-unit-identity-key`
   - image-order invariant `strictly-monotonic-positive-registration-order-ordinal`
   - cross-image legality model `translation-unit-identity-key-and-registration-order-ordinal-govern-bootstrap-legality`
   - semantic-diagnostic model `fail-closed-bootstrap-legality-before-runtime-handoff`
7. The semantic surface publishes the real `translation_unit_identity_key` together with the emitted registration/image identifiers and the emitted registration-order ordinal.
8. Deterministic probe coverage proves:
   - recompiling the same translation unit preserves the same `translation_unit_identity_key`
   - compiling a peer translation unit with identical visible bootstrap identifiers yields a different `translation_unit_identity_key`
9. `M263-B002` remains fail-closed and ready-for-lowering/runtime only when the upstream `M263-B001`, `M263-A002`, and `M254-B002` packets are all continuous.

## Non-Goals and Fail-Closed Rules

- `M263-B002` does not land multi-image bootstrap execution.
- `M263-B002` does not widen the public bootstrap runtime API.
- `M263-B003` will close the remaining failure-mode and restart edge cases.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m263-b002-duplicate-registration-and-image-order-semantics`.
- `package.json` includes `test:tooling:m263-b002-duplicate-registration-and-image-order-semantics`.
- `package.json` includes `check:objc3c:m263-b002-lane-b-readiness`.

## Validation

- `python scripts/check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m263_b002_duplicate_registration_and_image_order_semantics_core_feature_implementation.py -q`
- `npm run check:objc3c:m263-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m263/M263-B002/duplicate_registration_and_image_order_semantics_summary.json`
