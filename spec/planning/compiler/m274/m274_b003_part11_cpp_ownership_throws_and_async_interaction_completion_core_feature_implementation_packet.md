# M274-B003 Packet: Part 11 C++ Ownership, Throws, And Async Interaction Completion - Core Feature Implementation

Packet: `M274-B003`
Milestone: `M274`
Lane: `B`
Dependencies: `M274-B002`, `M274-A002`, `M267-E002`, `M268-E002`, `M271-E002`
Next issue: `M274-B004`

## Objective

Complete the remaining Part 11 lane-B legality slice for C++-facing callable annotations by making ownership-managed callable surfaces, `throws`, and `async` / `objc_executor(...)` combinations fail closed before lowering and runtime work.

## Required outputs

- semantic surface `frontend.pipeline.semantic_surface.objc_part11_cpp_ownership_throws_and_async_interactions`
- issue-local checker `scripts/check_m274_b003_part11_cpp_ownership_throws_and_async_interaction_completion_core_feature_implementation.py`
- issue-local readiness runner `scripts/run_m274_b003_lane_b_readiness.py`
- issue-local pytest `tests/tooling/test_check_m274_b003_part11_cpp_ownership_throws_and_async_interaction_completion_core_feature_implementation.py`

## Canonical payload expectations

The emitted manifest must preserve a deterministic Part 11 packet with the following field families:

- callable classification:
  - `cpp_interop_callable_sites`
  - `cpp_named_callable_sites`
  - `header_named_callable_sites`
- deferred interaction counts:
  - `ownership_interaction_sites`
  - `throws_interaction_sites`
  - `async_interaction_sites`
- rejection counts:
  - `ownership_rejection_sites`
  - `throws_rejection_sites`
  - `async_rejection_sites`
- contract state:
  - `cpp_annotation_profile_reused`
  - `ownership_interactions_fail_closed`
  - `throws_interactions_fail_closed`
  - `async_interactions_fail_closed`
  - `ffi_abi_lowering_deferred`
  - `runtime_bridge_generation_deferred`
  - `deterministic`
  - `ready_for_lowering_and_runtime`

## Required diagnostics

- `O3S334`
- `O3S335`
- `O3S336`

## Non-goals

- foreign ABI lowering
- runnable cross-language bridge execution
- Swift-facing metadata or isolation completion
