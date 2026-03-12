# M266-C001 Packet

## Summary

Freeze the current lowering contract for admitted Part 5 control-flow constructs without overstating runnable support.

## Why this issue exists

The frontend/sema pipeline now admits:

- `guard`
- statement-form `match`
- source-only `defer { ... }`

That does not mean the native LLVM path can execute them yet. `M266-C001` is the point where lowering must become explicit about that truth.

## Required implementation

1. Add one lowering contract in `native/objc3c/src/lower/objc3_lowering_contract.h` and `.cpp`.
2. Build and validate that contract from the Part 5 semantic model in `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`.
3. Emit the packet into the frontend semantic surface under `frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract`.
4. Keep the native IR path fail-closed for admitted-but-unlowered control-flow constructs.
5. Add explicit IR anchor commentary in `native/objc3c/src/ir/objc3_ir_emitter.cpp` so the fail-closed path is discoverable from source.
6. Update docs/spec/package anchors so the published implementation status matches the actual lowering state.

## Packet fields

The packet must include at minimum:

- `contract_id`
- `surface_path`
- `source_semantic_contract_id`
- `guard_model`
- `match_model`
- `defer_model`
- `authority_model`
- `fail_closed_model`
- `guard_statement_sites`
- `guard_clause_sites`
- `match_statement_sites`
- `defer_statement_sites`
- `live_guard_short_circuit_sites`
- `live_match_dispatch_sites`
- `live_defer_cleanup_sites`
- `fail_closed_guard_short_circuit_sites`
- `fail_closed_match_dispatch_sites`
- `fail_closed_defer_cleanup_sites`
- `deterministic_fail_closed_sites`
- `contract_violation_sites`
- `deterministic`
- `fail_closed`
- `source_semantic_model_ready`
- `ready_for_native_guard_lowering`
- `ready_for_native_match_lowering`
- `ready_for_native_defer_lowering`
- `semantic_summary_replay_key`
- `replay_key`

## Acceptance criteria

- The packet exists in emitted semantic-surface manifests for source-only control-flow probes.
- The packet stays truthful: all live lowering counters remain `0`.
- The fail-closed counters exactly cover the admitted guard/match/defer sites in the source-only probes.
- Guard and match runnable probes fail with deterministic `O3L300` diagnostics.
- Defer runnable probe fails with deterministic `O3S221` diagnostics.
- The issue-local checker, pytest wrapper, and lane-C readiness runner pass.
- Evidence is written under `tmp/reports/m266/M266-C001/`.
