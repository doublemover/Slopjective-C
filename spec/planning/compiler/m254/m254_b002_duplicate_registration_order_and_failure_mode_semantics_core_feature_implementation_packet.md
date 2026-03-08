# M254-B002 Duplicate-Registration, Order, and Failure-Mode Semantics Core Feature Implementation Packet

Packet: `M254-B002`
Milestone: `M254`
Lane: `B`
Dependencies: `M254-B001`, `M254-A002`

## Objective

Turn the frozen startup/bootstrap semantics into a live runtime capability while
keeping constructor-root automation deferred until `M254-C001`.

## Required implementation

- Contract id `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
- Publish semantic surface
  `frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_semantics`
- Preserve duplicate-registration policy
  `fail-closed-by-translation-unit-identity-key`
- Preserve realization-order policy
  `constructor-root-then-registration-manifest-order`
- Preserve failure mode
  `abort-before-user-main-no-partial-registration-commit`
- Publish runtime result model `zero-success-negative-fail-closed`
- Publish runtime state snapshot symbol
  `objc3_runtime_copy_registration_state_for_testing`
- Carry the same status-code model into
  `module.runtime-registration-manifest.json`

## Code anchors

- `native/objc3c/src/runtime/objc3_runtime.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/io/objc3_process.h`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- `tests/tooling/runtime/m254_b002_bootstrap_semantics_probe.cpp`

## Acceptance criteria

- Duplicate translation-unit identity keys are rejected with status `-2`.
- Non-monotonic registration ordinals are rejected with status `-3`.
- Invalid descriptors are rejected with status `-1`.
- Failed registrations do not partially commit runtime-owned state.
- The emitted registration-manifest artifact and runtime probe agree on the
  duplicate/order/failure/status-code surface.
- Validation evidence lands at
  `tmp/reports/m254/M254-B002/bootstrap_semantics_summary.json`.
