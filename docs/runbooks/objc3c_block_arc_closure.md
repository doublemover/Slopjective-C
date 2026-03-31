# Objective-C 3 Block And ARC Closure

This runbook freezes the `M320` block/byref/ARC closure boundary.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/block_arc_closure/boundary_inventory.json`
- `tests/tooling/fixtures/block_arc_closure/escaping_block_byref_ownership_semantic_model.json`
- `tests/tooling/fixtures/block_arc_closure/arc_automation_lifetime_insertion_policy.json`
- `tests/tooling/fixtures/block_arc_closure/byref_promotion_copy_dispose_forwarding_contract.json`
- `tests/tooling/fixtures/block_arc_closure/block_arc_lowering_runtime_abi_contract.json`
- `tests/tooling/fixtures/block_arc_closure/executable_proof_abi_contract.json`

Replayable generators and validators:

- `python scripts/build_block_arc_closure_boundary_inventory_summary.py`
- `python scripts/build_block_arc_closure_semantic_summary.py`
- `python scripts/build_block_arc_closure_arc_policy_summary.py`
- `python scripts/build_block_arc_closure_byref_summary.py`
- `python scripts/build_block_arc_closure_lowering_abi_summary.py`
- `python scripts/check_block_arc_closure_escaping_block_lowering.py`
- `python scripts/check_block_arc_closure_arc_lifetime_lowering.py`
- `python scripts/build_block_arc_closure_executable_proof_summary.py`
- `python scripts/check_block_arc_closure_live_block_runtime.py`
- `python scripts/check_block_arc_closure_live_arc_runtime.py`
- `python scripts/check_m320_block_arc_closure_closeout_gate.py`

Current closure scope:

- escaping block promotion, invocation, and owned capture preservation over the runtime-owned helper cluster
- byref forwarding, copy/dispose execution, and heap-promotion behavior over the live runtime path
- ARC retain/release/autorelease/autoreleasepool and weak/current-property helper traffic as part of one executable ownership story

Current closure constraints:

- the public runtime ABI remains registration, selector lookup, dispatch, and reset; block/ARC behavior continues to live on private runtime-owned helper and snapshot surfaces
- packaged and conformance proof already exist, but the milestone still needs one truthful closure boundary tying block/byref and ARC automation claims together
- interaction claims for properties, cleanup, and future error/concurrency surfaces must stay narrower than the evidence published today

Escaping block, byref, and ownership semantic model:

- capture-family truth stays sema-owned before lowering, including explicit capture modes, cleanup ownership transfer, and retainable-family legality
- escaping byref behavior is only supported through the runtime-owned promotion, forwarding, copy, and dispose helper path already targeted by lowering
- milestone claims stay narrower than the shared acceptance, runtime-probe, and packaged-e2e evidence and do not widen the public ABI

ARC automation and lifetime insertion policy:

- ARC is supported only as emitted retain/release/autorelease/autoreleasepool and weak/current-property helper traffic over the live runtime path
- cleanup scopes, implicit cleanup, and autorelease returns remain part of one coupled lowering and runtime story
- property interaction stays within the runtime-owned helper surface already covered by proof, while error and concurrency interaction claims stay deferred to `M321` and `M322`

Byref promotion, copy/dispose, and forwarding implementation:

- byref forwarding remains supported only through the runtime-owned promotion, invoke, and final-release path
- dispose is deferred until final release and invoke-after-release stays fail-closed
- byref closure claims are grounded in the live runtime probes and packaged block/ARC execution path, not in proof-only sidecars

Lowering and runtime ABI contract:

- the canonical compile-manifest and runtime-registration surface for this milestone is the shared acceptance output published by `scripts/check_objc3c_runtime_acceptance.py`
- the four required block/ARC surfaces are `runtime_block_arc_unified_source_surface`, `runtime_ownership_transfer_capture_family_source_surface`, `runtime_block_arc_lowering_helper_surface`, and `runtime_block_arc_runtime_abi_surface`
- milestone-local checks must consume those emitted surfaces instead of recreating parallel manifest truth

Executable proof and ABI contract:

- the public command surface for this milestone remains `test:objc3c:block-arc-conformance` and `test:objc3c:runnable-block-arc`
- the public workflow surface remains `validate-block-arc-conformance` and `validate-runnable-block-arc`
- block and ARC closure still relies on private runtime-owned helper and snapshot surfaces; the public runtime header is not widened by this milestone

Explicit non-goals:

- public runtime ABI widening for blocks, ARC helpers, or reflection
- claims that ARC automation is complete outside the emitted helper/lifetime surfaces already covered by compile-coupled proof
- milestone-local block/ARC scaffolding parallel to the shared runtime acceptance and runnable package path
- claims about error or concurrency interaction beyond the existing runtime-owned helper and cleanup surfaces

Successor milestones:

- `M321`: throws, cleanup, bridged errors, and executable propagation closure
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
- acceptance and public workflow:
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_runnable_block_arc_conformance.py`
  - `scripts/check_objc3c_runnable_block_arc_end_to_end.py`
  - `scripts/objc3c_public_workflow_runner.py`
- public claims:
  - `README.md`
  - `docs/objc3c-native.md`
  - `tests/tooling/runtime/README.md`
  - `docs/runbooks/objc3c_public_command_surface.md`

Generated evidence:

- `tmp/reports/m320/M320-A001/block_arc_closure_boundary_inventory_summary.json`
- `tmp/reports/m320/M320-A001/block_arc_closure_boundary_inventory_summary.md`
