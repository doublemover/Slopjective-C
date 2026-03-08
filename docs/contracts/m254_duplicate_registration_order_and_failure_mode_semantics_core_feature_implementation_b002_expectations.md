# M254 Duplicate-Registration, Order, and Failure-Mode Semantics Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`

Scope: `M254` lane-B live startup/bootstrap semantics over the emitted
registration-manifest path and native runtime library.

Semantic surface:
`frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_semantics`

Expected implementation:

1. The native runtime library lands live duplicate-registration, order, and
   failure-mode enforcement instead of a manifest-only summary.
2. `objc3_runtime_register_image` rejects duplicate translation-unit identity
   keys with status `-2`.
3. `objc3_runtime_register_image` rejects non-monotonic registration ordinals
   with status `-3`.
4. Invalid descriptors fail closed with status `-1`.
5. `objc3_runtime_copy_registration_state_for_testing` exposes committed state
   so probes can prove failed registrations do not partially commit.
6. The emitted `module.runtime-registration-manifest.json` payload carries the
   same runtime-facing policy and status-code surface consumed by the probe.
7. The canonical runtime probe source is
   `tests/tooling/runtime/m254_b002_bootstrap_semantics_probe.cpp`.

Required anchors:

- duplicate-registration policy
  `fail-closed-by-translation-unit-identity-key`
- realization-order policy
  `constructor-root-then-registration-manifest-order`
- failure mode
  `abort-before-user-main-no-partial-registration-commit`
- runtime result model `zero-success-negative-fail-closed`
- runtime state snapshot symbol
  `objc3_runtime_copy_registration_state_for_testing`

Validation commands:

- `python scripts/check_m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m254_b002_duplicate_registration_order_and_failure_mode_semantics_core_feature_implementation.py -q`
- `npm run check:objc3c:m254-b002-lane-b-readiness`

Evidence path:

- `tmp/reports/m254/M254-B002/bootstrap_semantics_summary.json`
