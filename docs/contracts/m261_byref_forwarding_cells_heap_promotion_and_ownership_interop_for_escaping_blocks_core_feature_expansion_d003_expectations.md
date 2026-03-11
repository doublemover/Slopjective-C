# M261 Byref Forwarding Cells Heap Promotion And Ownership Interop For Escaping Blocks Core Feature Expansion Expectations (D003)

Issue: `#7191`
Contract ID: `objc3c-runtime-block-byref-forwarding-heap-promotion-interop/m261-d003-v1`

`M261-D003` upgrades the D002 runtime block model so escaping pointer-capture blocks stop borrowing stack-cell addresses after promotion.

Required outcomes:

- promotion rewrites pointer-capture slots onto runtime-owned heap cells before helper execution
- escaped byref mutation persists across repeated runtime block-handle invokes after the source frame returns
- owned-capture copy/dispose helpers run against runtime-owned capture cells
- emitted IR publishes `; runtime_block_byref_forwarding_heap_promotion_ownership_interop = ...`
- the private runtime helper ABI remains private
- `M261-E001` is the next issue

Required artifacts:

- `docs/contracts/m261_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion_d003_expectations.md`
- `spec/planning/compiler/m261/m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion_packet.md`
- `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_byref_positive.objc3`
- `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_owned_capture_positive.objc3`
- `tests/tooling/runtime/m261_d003_block_runtime_byref_forwarding_probe.cpp`
- `python scripts/check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py`
- `python scripts/run_m261_d003_lane_d_readiness.py`
- `python -m pytest tests/tooling/test_check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py -q`
- `tmp/reports/m261/M261-D003/block_runtime_byref_forwarding_heap_promotion_ownership_interop_summary.json`

Targeted validation:

- `python scripts/check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py -q`
- `python scripts/run_m261_d003_lane_d_readiness.py`
