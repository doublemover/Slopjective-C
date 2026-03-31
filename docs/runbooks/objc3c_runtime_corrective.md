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

Replayable summary generator:

- `python scripts/build_runtime_corrective_boundary_inventory_summary.py`
- `python scripts/build_runtime_corrective_dispatch_summary.py`
- `python scripts/build_runtime_corrective_synthesized_accessor_summary.py`
- `python scripts/build_runtime_corrective_native_output_truth_summary.py`
- `python scripts/build_runtime_corrective_acceptance_workload_summary.py`
- `python scripts/build_runtime_corrective_lowering_provenance_summary.py`
- `python scripts/build_runtime_corrective_executable_proof_summary.py`
- `python scripts/check_runtime_corrective_dispatch_lowering.py`
- `python scripts/check_runtime_corrective_synthesized_accessor_lowering.py`
- `python scripts/check_runtime_corrective_live_dispatch_runtime.py`
- `python scripts/check_runtime_corrective_synthesized_accessor_runtime.py`
- `python scripts/check_runtime_corrective_closeout_gate.py`

Current corrective scope:

- realized dispatch over the live object graph through `objc3_runtime_dispatch_i32`
- synthesized accessor code generation and execution through the current-property helper path
- native-output truth tied to real compiler invocation, emitted object output, provenance, registration manifest, and linked runtime probe

Current corrective gaps:

- unresolved dispatch still retains one deterministic fallback path after slow-path miss
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
- throws, cleanup, bridged errors, and executable propagation closure
- async/task/actor runtime execution, scheduling, and isolation closure
- metaprogramming, property-behavior runtime materialization, and interop closure
- full-envelope conformance, stability, and production claimability

Authoritative live surfaces:

- runtime:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
- lowering and IR:
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- proof and provenance:
  - `scripts/objc3c_native_compile.ps1`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/shared_compiler_runtime_acceptance_harness.py`
- claims:
  - `docs/objc3c-native.md`
  - `tests/tooling/runtime/README.md`

Generated evidence:

- `tmp/reports/runtime-corrective/boundary-inventory/runtime_corrective_boundary_inventory_summary.json`
- `tmp/reports/runtime-corrective/boundary-inventory/runtime_corrective_boundary_inventory_summary.md`
