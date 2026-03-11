# M261 Block Runtime API And Object Layout Contract And Architecture Freeze Expectations (D001)

Contract ID: `objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1`
Issue: `#7189`
Milestone: `M261`
Lane: `D`

## Required Outcome

`M261-D001` freezes the truthful private runtime API and object-layout boundary
after `M261-C004`.

The frozen boundary must now prove all of the following:

- public runtime headers still do not publish block helper entrypoints
- private helper entrypoints remain
  `objc3_runtime_promote_block_i32` and
  `objc3_runtime_invoke_block_i32`
- emitted IR publishes the canonical
  `runtime_block_api_object_layout` summary
- private runtime block records remain opaque runtime-owned state rather than a
  public block-object ABI
- no generalized block allocation/copy/dispose runtime API lands here

## Required Artifacts

- `tests/tooling/fixtures/native/m261_escaping_block_runtime_hook_argument_positive.objc3`
- `tmp/artifacts/compilation/objc3c-native/m261/d001/positive/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m261/d001/positive/module.obj`
- `tmp/reports/m261/M261-D001/block_runtime_api_object_layout_contract_summary.json`

## Required Runtime Surface

The frozen boundary must publish:

- `; runtime_block_api_object_layout = contract=objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1`
- `objc3_runtime_promote_block_i32`
- `objc3_runtime_invoke_block_i32`
- `handle_type=i32`
- `promotion_abi=ptr, i64, i32 -> i32`
- `invoke_abi=i32, i32, i32, i32, i32 -> i32`

The positive probe must prove:

- the private helper symbols still lower into emitted IR
- the positive escaping-block fixture still links against the current runtime
  library and exits `14`
- object emission remains `llvm-direct`

## Validation Commands

- `python scripts/check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py -q`
- `python scripts/run_m261_d001_lane_d_readiness.py`
