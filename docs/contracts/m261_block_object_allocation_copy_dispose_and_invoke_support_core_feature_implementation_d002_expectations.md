# M261 Block Object Allocation Copy-Dispose And Invoke Support Core Feature Implementation Expectations (D002)

Issue: `#7190`
Contract ID: `objc3c-runtime-block-allocation-copy-dispose-invoke-support/m261-d002-v1`

`M261-D002` turns the frozen `M261-D001` private block helper boundary into a live runtime capability.

Required outcomes:

- promoted runtime block records copy block storage into aligned runtime-owned buffers
- pointer-capture promotion preserves copy/dispose helper pointers
- promotion runs the copy helper before the block handle is published
- final release runs the dispose helper before the runtime block record is erased
- runtime invoke support accepts promoted pointer-capture block records
- emitted IR publishes `; runtime_block_allocation_copy_dispose_invoke_support = ...`
- public runtime headers remain intentionally unchanged
- `M261-D003` remains the next issue

Required artifacts:

- `docs/contracts/m261_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation_d002_expectations.md`
- `spec/planning/compiler/m261/m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation_packet.md`
- `tests/tooling/runtime/m261_d002_block_runtime_copy_dispose_invoke_probe.cpp`
- `python scripts/check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py`
- `python scripts/run_m261_d002_lane_d_readiness.py`
- `python -m pytest tests/tooling/test_check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py -q`
- `tmp/reports/m261/M261-D002/block_runtime_copy_dispose_invoke_support_summary.json`

Targeted validation:

- `python scripts/check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py -q`
- `python scripts/run_m261_d002_lane_d_readiness.py`
