# Objective-C 3 Object-Model Closure

This runbook freezes the `M319` object-model closure boundary.

Canonical checked-in boundary and contract surfaces:

- `tests/tooling/fixtures/object_model_closure/boundary_inventory.json`
- `tests/tooling/fixtures/object_model_closure/realized_object_graph_reflection_semantic_model.json`
- `tests/tooling/fixtures/object_model_closure/loader_category_protocol_workload_map.json`
- `tests/tooling/fixtures/object_model_closure/realized_object_graph_runtime_implementation_contract.json`
- `tests/tooling/fixtures/object_model_closure/object_model_reflection_artifact_runtime_registration_contract.json`
- `tests/tooling/fixtures/object_model_closure/class_category_protocol_realization_lowering_contract.json`
- `tests/tooling/fixtures/object_model_closure/property_ivar_aggregate_reflection_artifact_contract.json`
- `tests/tooling/fixtures/object_model_closure/executable_proof_abi_contract.json`

Replayable generators and validators:

- `python scripts/build_object_model_closure_boundary_inventory_summary.py`
- `python scripts/build_object_model_closure_semantic_summary.py`
- `python scripts/build_object_model_closure_workload_summary.py`
- `python scripts/build_object_model_closure_runtime_implementation_summary.py`
- `python scripts/build_object_model_closure_artifact_registration_summary.py`
- `python scripts/check_object_model_closure_realization_lowering.py`
- `python scripts/check_object_model_closure_property_reflection_artifact.py`
- `python scripts/build_object_model_closure_executable_proof_summary.py`
- `python scripts/check_object_model_closure_live_runtime.py`
- `python scripts/check_object_model_closure_live_property_reflection.py`
- `python scripts/check_m319_object_model_closure_closeout_gate.py`

Current closure scope:

- realized class, metaclass, protocol, and category behavior over the runtime-owned graph
- property, ivar, and aggregate reflection coherence over runtime-owned query surfaces
- compile-coupled object-model proof and packaged validation without widening the public runtime ABI

Current closure constraints:

- unresolved sends still retain one deterministic miss fallback path, so M319 claims must stay tied to realized runtime-owned behavior instead of universal selector success
- reflection visibility remains on the private runtime-owned query surface and must not be repackaged as public ABI
- milestone proof must stay compile-coupled to the live runtime/registration path and not fork into milestone-local scaffolding

Explicit non-goals:

- public runtime ABI widening beyond registration, selector lookup, dispatch, and reset
- claims that unresolved dispatch miss fallback is part of the supported object-model closure
- probe-local or source-only reflection truth that bypasses runtime-owned query state
- milestone-local runtime or packaging scaffolds parallel to the shipped acceptance path

Successor milestones:

- `M320`: escaping block/byref execution, ownership transfer, and ARC automation
- `M321`: throws, cleanup, bridged errors, and executable propagation closure
- `M322`: async/task/actor runtime execution, scheduling, and isolation closure
- `M323`: metaprogramming, property-behavior runtime materialization, and interop closure
- `M324`: full-envelope conformance, stability, and production claimability

Authoritative live surfaces:

- runtime:
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
- lowering and manifests:
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- acceptance and public workflow:
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_runnable_object_model_conformance.py`
  - `scripts/check_objc3c_runnable_storage_reflection_conformance.py`
  - `scripts/check_objc3c_runnable_object_model_end_to_end.py`
  - `scripts/check_objc3c_runnable_storage_reflection_end_to_end.py`
  - `scripts/objc3c_public_workflow_runner.py`
- public claims:
  - `README.md`
  - `docs/objc3c-native.md`
  - `tests/tooling/runtime/README.md`
  - `docs/runbooks/objc3c_public_command_surface.md`

Generated evidence:

- `tmp/reports/m319/M319-A001/object_model_closure_boundary_inventory_summary.json`
- `tmp/reports/m319/M319-A001/object_model_closure_boundary_inventory_summary.md`
