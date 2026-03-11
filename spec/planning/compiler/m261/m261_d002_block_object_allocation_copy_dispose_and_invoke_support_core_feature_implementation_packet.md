# M261-D002 Block Object Allocation Copy-Dispose And Invoke Support Core Feature Implementation Packet

Packet: `M261-D002`
Issue: `#7190`
Milestone: `M261`
Lane: `D`

Summary:
Implement runtime support for promoted block record allocation, helper-mediated copy/dispose behavior, and invoke support.

Dependencies:

- `M261-D001`
- `M261-C004`

Acceptance criteria:

1. Promoted block records copy storage into aligned runtime-owned buffers.
2. Pointer-capture promotion preserves copy/dispose helper pointers.
3. Promotion runs the copy helper before the runtime block handle is published.
4. Final runtime release runs the dispose helper before the block record is erased.
5. `objc3_runtime_invoke_block_i32` accepts promoted pointer-capture block records.
6. Emitted IR publishes `; runtime_block_allocation_copy_dispose_invoke_support = ...`.
7. Public runtime headers remain intentionally unchanged.
8. Validation evidence lands under `tmp/`.

Proof assets:

- fixture `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_argument_positive.objc3`
- runtime probe `tests/tooling/runtime/m261_d002_block_runtime_copy_dispose_invoke_probe.cpp`
- checker `python scripts/check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py`
- readiness runner `python scripts/run_m261_d002_lane_d_readiness.py`
- summary `tmp/reports/m261/M261-D002/block_runtime_copy_dispose_invoke_support_summary.json`

Code anchors:

- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/driver/objc3_compilation_driver.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`

Next issue:

- `M261-D003` is the explicit next issue after this implementation lands.
