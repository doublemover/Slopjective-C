# M254-D003 Realization Sequencing and Deterministic Reset Hooks Core Feature Expansion Packet

Packet: `M254-D003`
Milestone: `M254`
Lane: `D`
Dependencies: `M254-D002`

## Purpose

Implement deterministic startup replay and explicit reset hooks so the runtime
can be reinitialized safely inside one process without widening the frozen D001
public bootstrap API.

## Scope Anchors

- Contract:
  `docs/contracts/m254_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion_d003_expectations.md`
- Checker:
  `scripts/check_m254_d003_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion.py`
- Tooling tests:
  `tests/tooling/test_check_m254_d003_realization_sequencing_and_deterministic_reset_hooks_core_feature_expansion.py`
- Runtime probe:
  `tests/tooling/runtime/m254_d003_deterministic_reset_replay_probe.cpp`
- Lane readiness:
  `scripts/run_m254_d003_lane_d_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m254-d003-realization-sequencing-and-deterministic-reset-hooks-core-feature-expansion`
  - `test:tooling:m254-d003-realization-sequencing-and-deterministic-reset-hooks-core-feature-expansion`
  - `check:objc3c:m254-d003-lane-d-readiness`

## Runtime Boundary

- Contract id `objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`
- Semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_reset_contract`
- Private bootstrap header
  `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- Private bridge symbols:
  - `objc3_runtime_replay_registered_images_for_testing`
  - `objc3_runtime_copy_reset_replay_state_for_testing`
- Canonical policies:
  - reset lifecycle model
    `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
  - replay order model
    `replay-re-registers-retained-images-in-original-registration-order`
  - image-local init-state reset model
    `retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay`
  - bootstrap catalog retention model
    `bootstrap-catalog-retained-across-reset-for-deterministic-replay`

## Acceptance Checklist

- live runtime state can be reset without deleting the retained bootstrap
  catalog
- retained image-local init-state cells are zeroed during reset
- replay re-registers retained images in original registration order through the
  validated registration-table ingestion path
- the runtime exposes deterministic reset/replay snapshot state for probes
- `module.runtime-registration-manifest.json` publishes the reset/replay fields
- evidence is written to
  `tmp/reports/m254/M254-D003/deterministic_reset_replay_summary.json`
