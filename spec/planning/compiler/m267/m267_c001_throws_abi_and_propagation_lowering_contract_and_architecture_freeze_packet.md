# M267-C001 Throws ABI And Propagation Lowering Contract And Architecture Freeze Packet

Packet: `M267-C001`
Milestone: `M267`
Wave: `W60`
Lane: `C`
Issue: `#7274`
Contract ID: `objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1`
Dependencies: `M267-B002`, `M267-B003`

## Objective

Freeze the truthful current native lowering boundary for Part 6 before runnable propagation transfer lands.

## Canonical Lowering Boundary

- contract id `objc3c-part6-throws-abi-propagation-lowering/m267-c001-v1`
- source model `part6-semantic-packets-feed-throws-result-like-nserror-and-unwind-lowering-replay-boundary`
- abi model `native-lowering-currently-publishes-propagation-carrier-and-out-error-boundaries-through-deterministic-replay-contracts`
- fail-closed model `native-throw-try-do-catch-execution-and-general-error-object-abi-remain-fail-closed-until-m267-c002`
- non-goal model `no-runnable-throw-transfer-no-catch-dispatch-no-generalized-error-object-runtime-abi-claim-yet`
- emitted IR comment `; part6_throws_abi_propagation_lowering = ...`
- emitted IR metadata `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`

## Acceptance Criteria

- Add explicit Part 6 lowering-boundary constants and summary helpers in `native/objc3c/src/lower/objc3_lowering_contract.h` and `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
- Materialize deterministic result-like, NSError-bridging, and unwind-cleanup lowering contracts in `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` instead of publishing only throws lowering.
- Publish one combined semantic surface at `frontend.pipeline.semantic_surface.objc_part6_throws_abi_propagation_lowering`.
- Publish one combined replay boundary into emitted IR and named metadata.
- Keep the runtime gap explicit: runnable `throw`, `try`, and `do/catch` transfer remain deferred to `M267-C002`.
- Add deterministic docs/spec/package/checker/test evidence.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/hello.objc3` proving emitted IR/object output carries:
   - `; part6_throws_abi_propagation_lowering = ...`
   - `throws_replay_key=throws_propagation_sites=`
   - `result_like_replay_key=result_like_sites=`
   - `ns_error_replay_key=ns_error_bridging_sites=`
   - `unwind_replay_key=unwind_cleanup_sites=`
   - `ready_for_runtime_execution=false`
   - `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
   - successful `module.obj` emission.
2. Source-only manifest probe over `tests/tooling/fixtures/native/m267_bridge_legality_positive.objc3` proving the combined lowering packet is present, linked to the constituent replay keys, and remains fail-closed for runtime execution.
3. Native fail-closed probe over `tests/tooling/fixtures/native/m267_try_do_catch_native_fail_closed.objc3` proving `do/catch` still rejects runnable native mode.

## Non-Goals

- `M267-C001` does not add runnable `throw` transfer.
- `M267-C001` does not add runnable `try` / `do/catch` execution.
- `M267-C001` does not add generalized native thrown-error object ABI.

## Validation Commands

- `python scripts/check_m267_c001_throws_abi_and_propagation_lowering_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m267_c001_throws_abi_and_propagation_lowering_contract_and_architecture_freeze.py -q`
- `npm run check:objc3c:m267-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m267/M267-C001/throws_abi_and_propagation_lowering_summary.json`
