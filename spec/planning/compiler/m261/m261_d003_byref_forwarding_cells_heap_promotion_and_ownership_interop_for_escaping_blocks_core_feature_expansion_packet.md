# M261-D003 Byref Forwarding Cells Heap Promotion And Ownership Interop For Escaping Blocks Core Feature Expansion Packet

Packet: `M261-D003`
Issue: `#7191`
Milestone: `M261`
Lane: `D`

Summary:
Implement runtime-owned forwarding cells for escaping pointer-capture blocks so byref mutation and owned-capture helper interop survive heap promotion.

Dependencies:

- `M261-D002`
- `M261-C004`
- `M260-D002`

Acceptance criteria:

1. Escaping pointer-capture promotion rewrites capture slots onto runtime-owned heap cells.
2. Escaped byref mutation persists across repeated block-handle invokes after the source frame returns.
3. Owned-capture copy/dispose helpers execute against runtime-owned capture cells.
4. Emitted IR publishes `; runtime_block_byref_forwarding_heap_promotion_ownership_interop = ...`.
5. Historical `M261-C004` validation remains truthful after this widening.
6. Validation evidence lands under `tmp/`.

Proof assets:

- fixture `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_byref_positive.objc3`
- fixture `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_owned_capture_positive.objc3`
- runtime probe `tests/tooling/runtime/m261_d003_block_runtime_byref_forwarding_probe.cpp`
- checker `python scripts/check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py`
- readiness runner `python scripts/run_m261_d003_lane_d_readiness.py`
- summary `tmp/reports/m261/M261-D003/block_runtime_byref_forwarding_heap_promotion_ownership_interop_summary.json`

Code anchors:

- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/driver/objc3_compilation_driver.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`

Next issue:

- `M261-E001` is the explicit next issue after this implementation lands.
