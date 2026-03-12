# M266-D002 Runtime Cleanup And Unwind Integration Core Feature Implementation Packet

Packet: `M266-D002`
Issue: `#7266`
Milestone: `M266`
Lane: `D`

Summary:
Implement the runtime/toolchain integration needed to prove cleanup and unwind semantics through runnable native execution.

Dependencies:

- `M266-D001`
- `M266-C003`

Acceptance criteria:

1. Ordinary lexical exit runs multiple deferred cleanups through runnable native executable proof.
2. Guard-mediated early return runs deferred cleanups through runnable native executable proof.
3. Nested-scope return unwind preserves inner-to-outer cleanup ordering through runnable native executable proof.
4. Emitted registration/link sidecars continue to provide the runtime-support archive path and linker-response payload needed to link the runnable executables.
5. The runtime cleanup carrier remains private and continues to use `objc3_runtime_push_autoreleasepool_scope`, `objc3_runtime_pop_autoreleasepool_scope`, and `objc3_runtime_copy_memory_management_state_for_testing` without widening a public cleanup/unwind ABI.
6. Emitted registration manifests publish `cleanup_unwind_runtime_link_model` deterministically.
7. Validation evidence lands under `tmp/`.

Proof assets:

- checker `python scripts/check_m266_d002_runtime_cleanup_and_unwind_integration_core_feature_implementation.py`
- readiness runner `python scripts/run_m266_d002_lane_d_readiness.py`
- runtime probe `tests/tooling/runtime/m266_d002_cleanup_unwind_runtime_probe.cpp`
- summary `tmp/reports/m266/M266-D002/runtime_cleanup_and_unwind_integration_summary.json`

Code anchors:

- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/io/objc3_process.cpp`

Next issue:

- `M266-E001` is the explicit next issue after this implementation lands.
