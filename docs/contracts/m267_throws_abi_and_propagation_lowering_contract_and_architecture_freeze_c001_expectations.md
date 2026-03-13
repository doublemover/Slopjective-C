# M267 Throws ABI And Propagation Lowering Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1`

This issue freezes one truthful native Part 6 lowering boundary at `frontend.pipeline.semantic_surface.objc_part6_throws_abi_propagation_lowering`.

Required behavior:
- the frontend artifact surface publishes one combined Part 6 lowering packet over throws propagation, result-like carriers, NSError bridging, and unwind cleanup
- emitted IR publishes `; part6_throws_abi_propagation_lowering = ...`
- emitted IR publishes `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
- the frozen boundary remains explicit that runnable `throw`, `try`, and `do/catch` transfer are still deferred to `M267-C002`

Required combined boundary facts:
- contract id stays `objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1`
- source model stays `part6-semantic-packets-feed-throws-result-like-nserror-and-unwind-lowering-replay-boundary`
- abi model stays `native-lowering-currently-publishes-propagation-carrier-and-out-error-boundaries-through-deterministic-replay-contracts`
- fail-closed model stays `native-throw-try-do-catch-execution-and-general-error-object-abi-remain-fail-closed-until-m267-c002`
- non-goal model stays `no-runnable-throw-transfer-no-catch-dispatch-no-generalized-error-object-runtime-abi-claim-yet`
- `next_issue=M267-C002`

Required positive proof:
- native compile probe over `tests/tooling/fixtures/native/hello.objc3`
- emitted `module.ll` retains:
  - `; part6_throws_abi_propagation_lowering = ...`
  - `throws_replay_key=throws_propagation_sites=`
  - `result_like_replay_key=result_like_sites=`
  - `ns_error_replay_key=ns_error_bridging_sites=`
  - `unwind_replay_key=unwind_cleanup_sites=`
  - `ready_for_runtime_execution=false`
  - `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
- source-only manifest probe over `tests/tooling/fixtures/native/m267_bridge_legality_positive.objc3` proves the combined lowering packet is present and deterministic
- source-only manifest probe over `tests/tooling/fixtures/native/m267_bridge_legality_positive.objc3` proves the combined lowering packet is present, linked to the constituent replay keys, and remains fail-closed for runtime execution

Required negative proof:
- native probe over `tests/tooling/fixtures/native/m267_try_do_catch_native_fail_closed.objc3` still fails closed in native mode

Validation commands:
- `python scripts/check_m267_c001_throws_abi_and_propagation_lowering_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m267_c001_throws_abi_and_propagation_lowering_contract_and_architecture_freeze.py -q`
- `python scripts/run_m267_c001_lane_c_readiness.py`

Evidence path:
- `tmp/reports/m267/M267-C001/throws_abi_and_propagation_lowering_summary.json`
