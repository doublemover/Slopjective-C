# M274-B004 Packet: Part 11 Swift Metadata And Isolation Mapping Completion - Edge-case And Compatibility Completion

Packet: `M274-B004`
Milestone: `M274`
Lane: `B`
Dependencies: `M274-B003`, `M274-A002`, `M270-E002`
Next issue: `M274-C001`

## Objective

Close the remaining Swift-facing Part 11 lane-B legality gap by making metadata pairing, actor-owned surfaces, `objc_nonisolated` surfaces, and implementation-surface Swift metadata fail closed before lowering and runtime work.

## Required outputs

- semantic surface `frontend.pipeline.semantic_surface.objc_part11_swift_metadata_and_isolation_mapping`
- issue-local checker `scripts/check_m274_b004_part11_swift_metadata_and_isolation_mapping_completion_edge_case_and_compatibility_completion.py`
- issue-local readiness runner `scripts/run_m274_b004_lane_b_readiness.py`
- issue-local pytest `tests/tooling/test_check_m274_b004_part11_swift_metadata_and_isolation_mapping_completion_edge_case_and_compatibility_completion.py`

## Canonical payload expectations

The emitted manifest must preserve a deterministic Part 11 packet with the following field families:

- callable classification:
  - `swift_interop_callable_sites`
  - `swift_named_callable_sites`
  - `swift_private_callable_sites`
- unsupported surface counts:
  - `swift_private_without_name_sites`
  - `actor_owned_swift_callable_sites`
  - `nonisolated_swift_callable_sites`
  - `implementation_swift_callable_sites`
- rejection counts:
  - `swift_private_without_name_rejection_sites`
  - `actor_isolation_mapping_rejection_sites`
  - `nonisolated_mapping_rejection_sites`
  - `implementation_surface_rejection_sites`
- contract state:
  - `swift_metadata_profile_reused`
  - `swift_private_requires_name_enforced`
  - `actor_isolation_mapping_fail_closed`
  - `nonisolated_mapping_fail_closed`
  - `implementation_surface_fail_closed`
  - `ffi_abi_lowering_deferred`
  - `runtime_bridge_generation_deferred`
  - `deterministic`
  - `ready_for_lowering_and_runtime`

## Required diagnostics

- `O3S337`
- `O3S338`
- `O3S339`
- `O3S340`

## Non-goals

- Swift ABI lowering
- runnable isolation export
- cross-language bridge execution
