# M267 Error-Out ABI And Propagation Lowering Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1`

This issue turns the Part 6 lane-C lowering surface into a runnable native capability.

Required behavior:
- the frontend artifact surface publishes `frontend.pipeline.semantic_surface.objc_part6_throws_abi_propagation_lowering`
- emitted IR publishes `; part6_throws_abi_propagation_lowering = ...`
- emitted IR publishes `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
- the lowering packet truthfully publishes `ready_for_runtime_execution=true`
- native lowering emits hidden error-out ABI and runnable propagation/catch control-flow rather than a contract-only placeholder

Required boundary facts:
- contract id stays `objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1`
- source model stays `part6-semantic-packets-feed-runnable-error-out-abi-propagation-and-catch-dispatch-lowering`
- abi model stays `native-lowering-emits-hidden-error-out-abi-propagation-operators-and-do-catch-control-flow-through-real-ir-and-object-artifacts`
- fail-closed model stays `result-bridge-replay-completion-and-separate-compilation-proof-remain-deferred-to-m267-c003`
- non-goal model stays `no-generalized-foreign-exception-abi-no-stable-cross-module-replay-claim-yet`
- `next_issue=M267-C003`

Required positive proof:
- native compile probe over `tests/tooling/fixtures/native/m267_c002_error_out_abi_positive.objc3`
- emitted `module.ll` retains:
  - `define i32 @recover(ptr %error_out)`
  - `define i32 @bubble(ptr %error_out)`
  - runnable `try` lowering blocks
  - runnable `do/catch` dispatch blocks
  - the Part 6 lowering comment and metadata anchor
- emitted `module.manifest.json` proves:
  - `contract_id=objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1`
  - `ready_for_runtime_execution=true`
- emitted `module.obj` links with `artifacts/lib/objc3_runtime.lib`
- linked executable exits with code `102`

Validation commands:
- `python scripts/check_m267_c002_error_out_abi_and_propagation_lowering_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m267_c002_error_out_abi_and_propagation_lowering_core_feature_implementation.py -q`
- `python scripts/run_m267_c002_lane_c_readiness.py`

Evidence path:
- `tmp/reports/m267/M267-C002/error_out_abi_and_propagation_lowering_summary.json`
