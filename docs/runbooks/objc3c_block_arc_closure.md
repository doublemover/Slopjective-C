# Objective-C 3 Block And ARC Closure

This runbook freezes the block/byref/ARC closure boundary.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/block_arc_closure/boundary_inventory.json`
- `tests/tooling/fixtures/block_arc_closure/escaping_block_byref_ownership_semantic_model.json`
- `tests/tooling/fixtures/block_arc_closure/arc_automation_lifetime_insertion_policy.json`
- `tests/tooling/fixtures/block_arc_closure/byref_promotion_copy_dispose_forwarding_contract.json`
- `tests/tooling/fixtures/block_arc_closure/block_arc_lowering_runtime_abi_contract.json`
- `tests/tooling/fixtures/block_arc_closure/executable_proof_abi_contract.json`
- `tests/tooling/fixtures/block_arc_closure/issue_8033_escaping_owned_object_copy_dispose_evidence.json`

Replayable public workflow actions:

- `npm run objc3c -- test-runtime-acceptance-block-arc`
- `npm run objc3c -- validate-block-arc-conformance`
- `npm run objc3c -- validate-runnable-block-arc`

Helper implementations are action-registry anchors and
milestone evidence builders, not a separate public command surface.

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
- property interaction stays within the runtime-owned helper surface already covered by proof, while error and concurrency interaction claims stay deferred to the dedicated error-runtime and concurrency-closure tracks

Byref promotion, copy/dispose, and forwarding implementation:

- byref forwarding remains supported only through the runtime-owned promotion, invoke, and final-release path
- private runtime byref cells forward stack captures to shared heap cells across promoted blocks; compiler-emitted caller-frame forwarding remains a later lowering bridge
- dispose is deferred until final release and invoke-after-release stays fail-closed
- byref closure claims are grounded in the live runtime probes and packaged block/ARC execution path, not in evidence-only sidecars
- issue #8033 escaping owned-object block copy/dispose proof is anchored by the live owned-capture lifetime runtime probe, which retains the captured object during block promotion, keeps it live after the original owner is released, releases it on final block dispose, and rejects stale invocation

Lowering and runtime ABI contract:

- the block/ARC compile-manifest and runtime-registration surface is the shared acceptance output published by `npm run objc3c -- test-runtime-acceptance-block-arc`
- the four required block/ARC surfaces are `runtime_block_arc_unified_source_surface`, `runtime_ownership_transfer_capture_family_source_surface`, `runtime_block_arc_lowering_helper_surface`, and `runtime_block_arc_runtime_abi_surface`
- release-scope checks must consume those emitted surfaces instead of recreating parallel manifest truth

Executable proof and ABI contract:

- the block/ARC public command surface is `npm run objc3c -- validate-block-arc-conformance` and `npm run objc3c -- validate-runnable-block-arc`
- the public workflow surface remains `validate-block-arc-conformance` and `validate-runnable-block-arc`
- block and ARC closure still relies on private runtime-owned helper and snapshot surfaces; the public runtime header is not widened by this milestone

Explicit non-goals:

- public runtime ABI widening for blocks, ARC helpers, or reflection
- claims that ARC automation is complete outside the emitted helper/lifetime surfaces already covered by compile-coupled proof
- release-scope block/ARC scaffolding parallel to the shared runtime acceptance and runnable package path
- claims about error or concurrency interaction beyond the existing runtime-owned helper and cleanup surfaces

Follow-on tracks:

- throws, cleanup, bridged errors, and executable propagation closure
- async/task/actor runtime execution, scheduling, and isolation closure
- metaprogramming, property-behavior runtime materialization, and interop closure
- full-envelope conformance, stability, and production claimability

Authoritative live surfaces:

- runtime:
  - `native/objc3c/src/runtime/blocks/`
  - `native/objc3c/src/runtime/dispatch/`
  - `native/objc3c/src/runtime/state/`
  - `native/objc3c/src/runtime/public/objc3_runtime_api.h`
- sema and lowering:
  - `native/objc3c/src/sema/`
  - `native/objc3c/src/lower/contracts/`
  - `native/objc3c/src/ir/`
  - `native/objc3c/src/artifacts/`
- acceptance and public workflow:
  - `npm run objc3c -- test-runtime-acceptance-block-arc`
  - `npm run objc3c -- validate-block-arc-conformance`
  - `npm run objc3c -- validate-runnable-block-arc`
  - package bridge: `npm run objc3c -- <action>`
- public claims:
  - `README.md`
  - `docs/objc3c-native.md`
  - `tests/tooling/runtime/README.md`
  - `docs/runbooks/objc3c_public_command_surface.md`

Reproducible generated evidence:

- `tmp/reports/block-arc-closure/boundary-inventory/block_arc_closure_boundary_inventory_summary.json`
- `tmp/reports/block-arc-closure/boundary-inventory/block_arc_closure_boundary_inventory_summary.md`
- `tmp/reports/block-arc-closure/byref-promotion-forwarding/byref_promotion_copy_dispose_forwarding_summary.json`
- `tmp/reports/block-arc-closure/byref-promotion-forwarding/byref_promotion_copy_dispose_forwarding_summary.md`
