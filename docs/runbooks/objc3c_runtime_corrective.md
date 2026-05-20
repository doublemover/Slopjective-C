# Objective-C 3 Runtime Corrective Tranche

This runbook freezes the narrow runtime-corrective scope.

Canonical checked-in boundary contract:

- `tests/tooling/fixtures/runtime_corrective/boundary_inventory.json`
- `tests/tooling/fixtures/runtime_corrective/realized_dispatch_semantic_model.json`
- `tests/tooling/fixtures/runtime_corrective/synthesized_accessor_semantic_model.json`
- `tests/tooling/fixtures/runtime_corrective/native_output_truth_policy.json`
- `tests/tooling/fixtures/runtime_corrective/acceptance_workload_map.json`
- `tests/tooling/fixtures/runtime_corrective/lowering_provenance_artifact_contract.json`
- `tests/tooling/fixtures/runtime_corrective/dispatch_lowering_implementation_contract.json`
- `tests/tooling/fixtures/runtime_corrective/synthesized_accessor_lowering_implementation_contract.json`
- `tests/tooling/fixtures/runtime_corrective/executable_proof_abi_contract.json`

Replayable public workflow actions:

- `npm run objc3c -- compile-objc3c`
- `npm run objc3c -- test-runtime-acceptance-fast`
- `npm run objc3c -- test-execution-replay`

Helper implementations remain milestone evidence anchors. They
are not a separate public command surface.

Current corrective scope:

- realized dispatch over the live object graph through `objc3_runtime_dispatch_i32`
- synthesized accessor code generation and execution through the current-property helper path
- native-output truth tied to real compiler invocation, emitted object output, provenance, registration manifest, and linked runtime probe

Current corrective gaps:

- unresolved dispatch still retains one strict dispatch error path after slow-path miss
- live synthesized accessor execution and reflection coherence still need one integrated proof surface
- native-output proof is only trustworthy when the emitted object and linked probe stay coupled to compile provenance and the runtime registration manifest

Explicit non-goals:

- full object-model closure
- public runtime ABI widening
- milestone-specific proof scaffolding
- claims based on hand-authored `.ll` or sidecar-only artifacts

Follow-on tracks:

- governance ratchet after the corrective tranche lands
- full object-model realization, property closure, and aggregate runtime reflection
- escaping block/byref execution, ownership transfer, and ARC automation
- error runtime closure remains owned by `docs/runbooks/objc3c_error_runtime_closure.md`
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
- lowering and IR:
  - `native/objc3c/src/lower/contracts/`
  - `native/objc3c/src/ir/`
- proof and provenance:
  - `npm run objc3c -- compile-objc3c`
  - `npm run objc3c -- test-execution-replay`
  - package bridge: `npm run objc3c -- <action>`
- claims:
  - `docs/objc3c-native.md`
  - `tests/tooling/runtime/README.md`

Generated evidence:

- `tmp/reports/runtime-corrective/boundary-inventory/runtime_corrective_boundary_inventory_summary.json`
- `tmp/reports/runtime-corrective/boundary-inventory/runtime_corrective_boundary_inventory_summary.md`
