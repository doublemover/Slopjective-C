# M263-B003 Bootstrap Failure-Mode and Restart Semantics Packet

Packet: `M263-B003`
Milestone: `M263`
Lane: `B`
Implementation date: `2026-03-09`
Dependencies: `M263-B002`, `M254-B002`, `M254-D003`
Next issue: `M263-C001`

## Purpose

Close the remaining bootstrap failure-mode, restart, and unsupported-topology gaps by publishing one live semantic/runtime bridge over the already-landed reset/replay runtime path.

## Scope Anchors

- Contract:
  `docs/contracts/m263_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion_b003_expectations.md`
- Checker:
  `scripts/check_m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion.py`
- Tooling tests:
  `tests/tooling/test_check_m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion.py`
- Runtime probe:
  `tests/tooling/runtime/m263_b003_bootstrap_failure_restart_probe.cpp`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m263-b003-bootstrap-failure-mode-and-restart-semantics`
  - `test:tooling:m263-b003-bootstrap-failure-mode-and-restart-semantics`
  - `check:objc3c:m263-b003-lane-b-readiness`
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
  `objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1`
- semantic/front-end surface
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_failure_restart_semantics`
- upstream contract ids:
  - `objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1`
  - `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
  - `objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`
- canonical restart/recovery models:
  - `failure_mode`
    `abort-before-user-main-no-partial-registration-commit`
  - `restart_lifecycle_model`
    `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
  - `replay_order_model`
    `replay-re-registers-retained-images-in-original-registration-order`
  - `image_local_init_reset_model`
    `retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay`
  - `catalog_retention_model`
    `bootstrap-catalog-retained-across-reset-for-deterministic-replay`
  - `unsupported_topology_model`
    `replay-requires-empty-live-runtime-state-and-retained-bootstrap-catalog`
- canonical continuity fields:
  - `translation_unit_identity_model`
  - `translation_unit_identity_key`
  - `translation_unit_registration_order_ordinal`
  - `runtime_state_snapshot_symbol`
  - `replay_registered_images_symbol`
    `objc3_runtime_replay_registered_images_for_testing`
  - `reset_replay_state_snapshot_symbol`
    `objc3_runtime_copy_reset_replay_state_for_testing`
  - `invalid_descriptor_status_code`

## Deterministic Probe Minimums

- startup registers exactly one image for the supported single-image fixture
- replay while live runtime state is still populated fails closed with the invalid-descriptor status
- reset clears live state, resets the next expected ordinal to `1`, and preserves one retained bootstrap image
- reset plus replay restores the same retained translation-unit identity key in canonical order
- a second unsupported replay attempt still fails closed
- a second reset plus replay still succeeds, proving deterministic restart semantics

## Non-Goals

- no multi-image bootstrap execution yet
- no public runtime ABI widening
- no partial recovery from unsupported restart topologies

## Gate Commands

- `python scripts/check_m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion.py`
- `python -m pytest tests/tooling/test_check_m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion.py -q`
- `npm run check:objc3c:m263-b003-lane-b-readiness`

## Evidence Output

- `tmp/reports/m263/M263-B003/bootstrap_failure_restart_semantics_summary.json`
