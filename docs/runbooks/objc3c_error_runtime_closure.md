# Objective-C 3 Error Runtime Closure

This runbook freezes the `M321` error runtime closure boundary.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/error_runtime_closure/boundary_inventory.json`
- `tests/tooling/fixtures/error_runtime_closure/error_propagation_unwind_cleanup_semantic_model.json`
- `tests/tooling/fixtures/error_runtime_closure/bridged_error_cross_module_compatibility_policy.json`
- `tests/tooling/fixtures/error_runtime_closure/throws_abi_helper_semantics_contract.json`
- `tests/tooling/fixtures/error_runtime_closure/error_lowering_runtime_artifact_contract.json`
- `tests/tooling/fixtures/error_runtime_closure/executable_proof_abi_contract.json`

Replayable generators and validators:

- `python scripts/build_error_runtime_closure_boundary_inventory_summary.py`
- `python scripts/build_error_runtime_closure_semantic_summary.py`
- `python scripts/build_error_runtime_closure_bridge_policy_summary.py`
- `python scripts/build_error_runtime_closure_throws_abi_summary.py`
- `python scripts/build_error_runtime_closure_artifact_summary.py`
- `python scripts/check_error_runtime_closure_throw_catch_cleanup_lowering.py`
- `python scripts/check_error_runtime_closure_bridge_artifact.py`
- `python scripts/build_error_runtime_closure_executable_proof_summary.py`
- `python scripts/check_error_runtime_closure_live_throw_cleanup_runtime.py`
- `python scripts/check_error_runtime_closure_live_bridged_error_runtime.py`
- `python scripts/check_m321_error_runtime_closeout_gate.py`

Current closure scope:

- thrown-error storage, try/throw/catch legality, and cleanup/unwind ordering on the live compiler/runtime path
- NSError/status bridging, catch-match behavior, and runtime-owned bridge-state observation over the private helper ABI
- cross-module replay and packaged execution proof for the current bridged error path

Current closure constraints:

- the public runtime ABI remains registration, selector lookup, dispatch, and reset; error behavior stays on the private runtime-owned helper and snapshot surfaces
- shared conformance and packaged e2e proof already exist, but the milestone still needs one truthful closure boundary tying throw/catch, cleanup, bridging, and cross-module propagation together
- ARC, async, and broader interop interaction claims must stay narrower than the evidence published today

Error propagation, unwind ordering, and cleanup semantic model:

- throw/catch and cleanup semantics are only supported as runtime-backed behavior when they align with the emitted lowering and live bridge-state probes
- unwind ordering, cleanup execution, and catch filtering are one coupled runtime story and must not drift into separate proof-only contracts
- milestone claims stay narrower than the shared acceptance, runtime-probe, and packaged-e2e evidence and do not widen the public ABI

Bridged error and cross-module compatibility policy:

- bridged NSError/status behavior is supported only through the currently emitted lowering packets, private runtime helper ABI, and replayable cross-module artifact surfaces
- cross-module propagation claims are limited to the manifest/runtime-registration/replay path already exercised by the shared conformance and packaged e2e reports
- compatibility claims remain fail-closed where a wider public ABI, richer interop surface, or new transport model would be required

Throws ABI and helper semantics implementation:

- helper semantics remain supported only through runtime-owned thrown-error store/load, status bridge, NSError bridge, catch-match, and bridge-state snapshot helpers
- executable claims must stay grounded in the live runtime probes and packaged runnable error path rather than deleted milestone scripts or sidecar notes
- any broader public ABI or foreign-runtime error model is out of scope for this milestone

Lowering and runtime artifact contract:

- the canonical compile-manifest and runtime-registration surface for this milestone is the shared acceptance output published by `scripts/check_objc3c_runtime_acceptance.py`
- the canonical error surfaces for this milestone are `runtime_error_execution_cleanup_source_surface`, `runtime_catch_filter_finalization_source_surface`, `runtime_error_propagation_cleanup_semantics_surface`, `runtime_bridging_filter_unwind_diagnostics_surface`, `runtime_error_lowering_unwind_bridge_helper_surface`, `runtime_error_runtime_abi_cleanup_surface`, and `runtime_error_propagation_catch_cleanup_runtime_implementation_surface`
- milestone-local checks must consume those emitted surfaces instead of recreating parallel manifest truth

Executable proof and ABI contract:

- the public command surface for this milestone remains `test:objc3c:error-conformance` and `test:objc3c:runnable-error`
- the public workflow surface remains `validate-error-conformance` and `validate-runnable-error`
- error closure still relies on private runtime-owned helper and snapshot surfaces; the public runtime header is not widened by this milestone

Explicit non-goals:

- public runtime ABI widening for thrown-error storage, bridge helpers, or catch matching
- claims that ARC, async, or broader interop interaction is complete beyond the currently published error-runtime evidence
- milestone-local error runtime scaffolding parallel to the shared runtime acceptance and runnable package path
- claims that cross-module propagation is complete beyond the current manifest/runtime-registration/replay proof

Successor milestones:

- `M322`: async/task/actor runtime execution, scheduling, and isolation closure
- `M323`: metaprogramming, property-behavior runtime materialization, and interop closure
- `M324`: full-envelope conformance, stability, and production claimability

Authoritative live surfaces:

- runtime:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- sema and lowering:
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`
- acceptance and public workflow:
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_runnable_error_conformance.py`
  - `scripts/check_objc3c_runnable_error_end_to_end.py`
  - `scripts/objc3c_public_workflow_runner.py`
- public claims:
  - `README.md`
  - `docs/objc3c-native.md`
  - `tests/tooling/runtime/README.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
