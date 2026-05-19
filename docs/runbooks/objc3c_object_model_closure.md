# Objective-C 3 Object-Model Closure

This runbook freezes the object-model closure boundary.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/object_model_closure/boundary_inventory.json`
- `tests/tooling/fixtures/object_model_closure/realized_object_graph_reflection_semantic_model.json`
- `tests/tooling/fixtures/object_model_closure/loader_category_protocol_workload_map.json`
- `tests/tooling/fixtures/object_model_closure/realized_object_graph_runtime_implementation_contract.json`
- `tests/tooling/fixtures/object_model_closure/object_model_reflection_artifact_runtime_registration_contract.json`
- `tests/tooling/fixtures/object_model_closure/class_category_protocol_realization_lowering_contract.json`
- `tests/tooling/fixtures/object_model_closure/property_ivar_aggregate_reflection_artifact_contract.json`
- `tests/tooling/fixtures/object_model_closure/executable_proof_abi_contract.json`

Replayable public workflow actions:

- `npm run objc3c -- test-runtime-acceptance-fast`
- `npm run objc3c -- validate-object-model-conformance`
- `npm run objc3c -- validate-storage-reflection-conformance`
- `npm run objc3c -- validate-runnable-object-model`
- `npm run objc3c -- validate-runnable-storage-reflection`

Helper implementations are action-registry anchors and
milestone evidence builders, not a separate public command surface.

Current closure scope:

- realized class, metaclass, protocol, and category behavior over the runtime-owned graph
- property, ivar, and aggregate reflection coherence over runtime-owned query surfaces
- compile-coupled object-model proof and packaged validation without widening the public runtime ABI

Current closure constraints:

- unresolved sends still retain one deterministic miss strict error path, so object-model closure claims must stay tied to realized runtime-owned behavior instead of universal selector success
- reflection visibility remains on the private runtime-owned query surface and must not be repackaged as public ABI
- milestone proof must stay compile-coupled to the live runtime/registration path and not fork into release-scope scaffolding

Explicit non-goals:

- public runtime ABI widening beyond registration, selector lookup, dispatch, and reset
- claims that unresolved dispatch miss recovery is part of the supported object-model closure
- probe-local or source-only reflection truth that bypasses runtime-owned query state
- release-scope runtime or packaging scaffolds parallel to the shipped acceptance path

Follow-on tracks:

- escaping block/byref execution, ownership transfer, and ARC automation
- throws, cleanup, bridged errors, and executable propagation closure
- async/task/actor runtime execution, scheduling, and isolation closure
- metaprogramming, property-behavior runtime materialization, and interop closure
- full-envelope conformance, stability, and production claimability

Authoritative live surfaces:

- runtime:
  - `native/objc3c/src/runtime/classes/`
  - `native/objc3c/src/runtime/dispatch/`
  - `native/objc3c/src/runtime/selectors/`
  - `native/objc3c/src/runtime/state/`
  - `native/objc3c/src/runtime/public/objc3_runtime_api.h`
- lowering and manifests:
  - `native/objc3c/src/ir/`
  - `native/objc3c/src/lower/contracts/`
  - `native/objc3c/src/artifacts/`
- acceptance and public workflow:
  - `npm run objc3c -- test-runtime-acceptance-fast`
  - `npm run objc3c -- validate-object-model-conformance`
  - `npm run objc3c -- validate-storage-reflection-conformance`
  - `npm run objc3c -- validate-runnable-object-model`
  - `npm run objc3c -- validate-runnable-storage-reflection`
  - package bridge: `npm run objc3c -- <action>`
- public claims:
  - `README.md`
  - `docs/objc3c-native.md`
  - `tests/tooling/runtime/README.md`
  - `docs/runbooks/objc3c_public_command_surface.md`

Generated evidence:

- `tmp/reports/object-model-closure/boundary-inventory/object_model_closure_boundary_inventory_summary.json`
- `tmp/reports/object-model-closure/boundary-inventory/object_model_closure_boundary_inventory_summary.md`
