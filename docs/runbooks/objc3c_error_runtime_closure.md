# Objective-C 3 Error Runtime Closure

This runbook freezes the error runtime closure boundary.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/error_runtime_closure/boundary_inventory.json`
- `tests/tooling/fixtures/error_runtime_closure/error_propagation_unwind_cleanup_semantic_model.json`
- `tests/tooling/fixtures/error_runtime_closure/bridged_error_cross_module_compatibility_policy.json`
- `tests/tooling/fixtures/error_runtime_closure/throws_abi_helper_semantics_contract.json`
- `tests/tooling/fixtures/error_runtime_closure/error_lowering_runtime_artifact_contract.json`
- `tests/tooling/fixtures/error_runtime_closure/executable_proof_abi_contract.json`

Replayable public workflow actions:

- `npm run objc3c -- test-runtime-acceptance-fast`
- `npm run objc3c -- validate-error-conformance`
- `npm run objc3c -- validate-runnable-error`

Helper implementations are action-registry anchors and
milestone evidence builders, not a separate public command surface.

Current closure scope:

- thrown-error storage, try/throw/catch legality, and cleanup/unwind ordering on the live compiler/runtime path
- NSError/status bridging, catch-match behavior, and runtime-owned bridge-state observation over the private helper ABI
- cross-module replay and packaged execution proof for the current bridged error path

Current closure constraints:

- the public runtime ABI remains registration, selector lookup, dispatch, and reset; error behavior stays on the private runtime-owned helper and snapshot surfaces
- the closure boundary now ties try/throw/catch semantics, ARC cleanup-preserved bridge lowering, private helper ABI behavior, and cross-module replay evidence together
- ARC claims are limited to cleanup-preservation evidence from `tests/tooling/fixtures/native/error_arc_cleanup_bridge_positive.objc3`; async and broader interop interaction claims must stay narrower than the evidence published today

Error propagation, unwind ordering, and cleanup semantic model:

- throw/catch and cleanup semantics are only supported as runtime-backed behavior when they align with the emitted lowering and live bridge-state probes
- unwind ordering, cleanup execution, and catch filtering are one coupled runtime story and must not drift into separate evidence-only contracts
- milestone claims stay narrower than the shared acceptance, runtime-probe, and packaged-e2e evidence and do not widen the public ABI

Bridged error and cross-module policy:

- bridged NSError/status behavior is supported only through the currently emitted lowering packets, private runtime helper ABI, and replayable cross-module artifact surfaces
- cross-module propagation claims are limited to the manifest/runtime-registration/replay path already exercised by the shared conformance and packaged e2e reports
- cross-module claims remain fail-closed where a wider public ABI, richer interop surface, or new transport model would be required

Throws ABI and helper semantics implementation:

- helper semantics remain supported only through runtime-owned thrown-error store/load, status bridge, NSError bridge, catch-match, and bridge-state snapshot helpers
- executable claims must stay grounded in the live runtime probes and packaged runnable error path rather than deleted milestone scripts or sidecar notes
- any broader public ABI or foreign-runtime error model is out of scope for this closure surface

Lowering and runtime artifact contract:

- the error compile-manifest and runtime-registration surface is the shared acceptance output published by `npm run objc3c -- test-runtime-acceptance-fast`
- the error owner surfaces are `runtime_error_execution_cleanup_source_surface`, `runtime_catch_filter_finalization_source_surface`, `runtime_error_propagation_cleanup_semantics_surface`, `runtime_bridging_filter_unwind_diagnostics_surface`, `runtime_error_lowering_unwind_bridge_helper_surface`, `runtime_error_runtime_abi_cleanup_surface`, and `runtime_error_propagation_catch_cleanup_runtime_implementation_surface`
- release-scope checks must consume those emitted surfaces instead of recreating parallel manifest truth

Executable proof and ABI contract:

- the error public command surface is `npm run objc3c -- validate-error-conformance` and `npm run objc3c -- validate-runnable-error`
- the public workflow surface remains `validate-error-conformance` and `validate-runnable-error`
- error closure still relies on the private runtime-owned helper ABI and snapshot surfaces; the public runtime header is not widened by this milestone

Explicit non-goals:

- public runtime ABI widening for thrown-error storage, bridge helpers, or catch matching
- claims that ARC behavior is complete beyond the cleanup-preservation bridge evidence, or that async or broader interop interaction is complete beyond the currently published error-runtime evidence
- release-scope error runtime scaffolding parallel to the shared runtime acceptance and runnable package path
- claims that cross-module propagation is complete beyond the current manifest/runtime-registration/replay proof

Follow-on tracks:

- async/task/actor runtime execution, scheduling, and isolation closure
- metaprogramming, property-behavior runtime materialization, and interop closure
- full-envelope conformance, stability, and production claimability

Authoritative live surfaces:

- runtime:
  - `native/objc3c/src/runtime/classes/`
  - `native/objc3c/src/runtime/dispatch/`
  - `native/objc3c/src/runtime/images/`
  - `native/objc3c/src/runtime/selectors/`
  - `native/objc3c/src/runtime/state/`
  - `native/objc3c/src/runtime/public/objc3_runtime_api.h`
- sema and lowering:
  - `native/objc3c/src/sema/`
  - `native/objc3c/src/lower/contracts/`
  - `native/objc3c/src/ir/`
  - `native/objc3c/src/artifacts/`
  - `native/objc3c/src/pipeline/objc3_runtime_import_surface.cpp`
- acceptance and public workflow:
  - `npm run objc3c -- test-runtime-acceptance-fast`
  - `npm run objc3c -- validate-error-conformance`
  - `npm run objc3c -- validate-runnable-error`
  - package bridge: `npm run objc3c -- <action>`
- public claims:
  - `README.md`
  - `docs/objc3c-native.md`
  - `tests/tooling/runtime/README.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
