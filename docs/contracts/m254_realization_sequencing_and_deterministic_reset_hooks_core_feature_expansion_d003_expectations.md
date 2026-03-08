# M254 Realization Sequencing and Deterministic Reset Hooks Core Feature Expansion Expectations (D003)

Contract ID: `objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`

## Goal

Expand the private runtime bootstrap boundary so same-process smoke harnesses can
clear live runtime state, zero retained image-local init-state cells, and
replay retained bootstrap images in original registration order without widening
the frozen D001 public runtime API.

## Canonical Surface

- Semantic surface path:
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_reset_contract`
- Private bootstrap header:
  `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- Private replay hook symbol:
  `objc3_runtime_replay_registered_images_for_testing`
- Private reset/replay snapshot symbol:
  `objc3_runtime_copy_reset_replay_state_for_testing`
- Reset lifecycle model:
  `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
- Replay order model:
  `replay-re-registers-retained-images-in-original-registration-order`
- Image-local init-state reset model:
  `retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay`
- Bootstrap catalog retention model:
  `bootstrap-catalog-retained-across-reset-for-deterministic-replay`

## Acceptance

- `objc3_runtime_reset_for_testing` clears live runtime state while preserving a
  retained bootstrap catalog.
- Reset zeroes retained emitted image-local init-state cells so same-process
  replay starts from a clean bootstrap boundary.
- `objc3_runtime_replay_registered_images_for_testing` re-registers retained
  startup images in original registration order using the staged registration-
  table path rather than an ad hoc fallback.
- `module.runtime-registration-manifest.json` publishes the deterministic
  reset/replay handoff fields.
- Deterministic evidence lands at
  `tmp/reports/m254/M254-D003/deterministic_reset_replay_summary.json`.
