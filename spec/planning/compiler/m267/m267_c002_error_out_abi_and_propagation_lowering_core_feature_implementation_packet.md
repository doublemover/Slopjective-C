# M267-C002 Error-Out ABI And Propagation Lowering Core Feature Implementation Packet

Packet: `M267-C002`
Milestone: `M267`
Wave: `W60`
Lane: `C`
Issue: `#7275`
Contract ID: `objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1`
Dependencies: `M267-C001`, `M267-B003`

## Objective

Implement runnable hidden error-out ABI lowering, propagation operators, bridge propagation, and `do/catch` dispatch in native IR and object artifacts.

## Canonical Lowering Boundary

- contract id `objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1`
- source model `part6-semantic-packets-feed-runnable-error-out-abi-propagation-and-catch-dispatch-lowering`
- abi model `native-lowering-emits-hidden-error-out-abi-propagation-operators-and-do-catch-control-flow-through-real-ir-and-object-artifacts`
- fail-closed model `result-bridge-replay-completion-and-separate-compilation-proof-remain-deferred-to-m267-c003`
- non-goal model `no-generalized-foreign-exception-abi-no-stable-cross-module-replay-claim-yet`
- emitted IR comment `; part6_throws_abi_propagation_lowering = ...`
- emitted IR metadata `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
- frontend semantic surface path `frontend.pipeline.semantic_surface.objc_part6_throws_abi_propagation_lowering`
- runtime readiness `true`

## Acceptance Criteria

- Lower throwing functions and methods with a hidden trailing error-out parameter in native IR.
- Lower `throw` so it stores into the current function error slot and exits the current frame.
- Lower `try`, `try?`, and `try!` over both direct throwing calls and status-to-`NSError` bridge calls.
- Lower `do/catch` with typed and catch-all dispatch.
- Publish truthful Part 6 lowering artifacts in emitted IR and manifest output.
- Add deterministic issue-local docs/spec/package/checker/test/readiness coverage.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m267_c002_error_out_abi_positive.objc3` proving:
   - emitted `module.ll` carries the Part 6 lowering comment and metadata
   - emitted `module.ll` carries `define i32 @recover(ptr %error_out)` and `define i32 @bubble(ptr %error_out)`
   - emitted `module.ll` carries runnable `try` and `do/catch` control-flow blocks
   - emitted `module.obj` exists and is non-empty
   - emitted `module.object-backend.txt` stays `llvm-direct`
2. Manifest probe over the same fixture proving:
   - `contract_id=objc3c-part6-throws-abi-propagation-lowering/m267-c002-v1`
   - `ready_for_runtime_execution=true`
   - `next_issue=M267-C003`
3. Link-and-run probe proving `module.obj` links with `artifacts/lib/objc3_runtime.lib` and the executable exits with code `102`.

## Non-Goals

- `M267-C002` does not claim replay completion for separate compilation.
- `M267-C002` does not claim generalized foreign error-object ABI support.
- `M267-C002` does not claim broader cross-module preservation yet.

## Validation Commands

- `python scripts/check_m267_c002_error_out_abi_and_propagation_lowering_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m267_c002_error_out_abi_and_propagation_lowering_core_feature_implementation.py -q`
- `npm run check:objc3c:m267-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m267/M267-C002/error_out_abi_and_propagation_lowering_summary.json`
