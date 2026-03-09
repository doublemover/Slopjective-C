# M256 Class And Metaclass Declaration Completeness Plus Inheritance Modeling Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-executable-class-metaclass-source-closure/m256-a002-v1`
Status: Accepted
Issue: `#7130`
Scope: M256 lane-A core feature implementation for realization-ready class/metaclass source closure.

## Objective

Complete the declaration-owned source model for classes and metaclasses so later runnable realization work consumes stable class-object, parent, and method-owner identities instead of reconstructing them from partial summaries.

## Required Invariants

1. `native/objc3c/src/pipeline/objc3_frontend_types.h` remains the canonical declaration point for:
   - `objc3c-executable-class-metaclass-source-closure/m256-a002-v1`
   - parent identity model `declaration-owned-class-parent-plus-metaclass-parent-identities`
   - method-owner identity model `declaration-owned-instance-class-method-owner-identities`
   - class-object identity model `declaration-owned-class-and-metaclass-object-identities`
2. `Objc3ExecutableMetadataSourceGraph` must now fail closed unless all four closure booleans are true:
   - `class_metaclass_declaration_closure_complete`
   - `class_metaclass_parent_identity_closure_complete`
   - `class_metaclass_method_owner_identity_closure_complete`
   - `class_metaclass_object_identity_closure_complete`
3. Interface declarations must carry explicit:
   - `class_owner_identity`
   - `metaclass_owner_identity`
   - `super_class_owner_identity`
   - `super_metaclass_owner_identity`
   - `instance_method_owner_identity`
   - `class_method_owner_identity`
   - `declaration_complete`
4. Implementation declarations must carry the same explicit runtime closure, including inherited parent identities on the declaration record itself rather than only through aggregate class nodes.
5. Class nodes must carry the same realization-owned closure, including:
   - `super_metaclass_owner_identity`
   - `instance_method_owner_identity`
   - `class_method_owner_identity`
   - `realization_identity_complete`
6. `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` must materialize and preserve declaration-owned edges for:
   - `interface-to-metaclass`
   - `interface-to-superclass`
   - `interface-to-super-metaclass`
   - `interface-to-instance-method-owner`
   - `interface-to-class-method-owner`
   - `implementation-to-metaclass`
   - `implementation-to-superclass`
   - `implementation-to-super-metaclass`
   - `implementation-to-instance-method-owner`
   - `implementation-to-class-method-owner`
7. `native/objc3c/src/ir/objc3_ir_emitter.cpp` must republish the same source closure through the explicit summary line:
   - `; executable_class_metaclass_source_closure = ...`
8. The happy-path fixture must prove the closure over `Base` and `Widget` without breaking the frozen M256-A001 source boundary.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/m252_executable_metadata_graph_class_metaclass.objc3` proves:
   - `module.manifest.json` exists
   - `module.ll` exists
   - the manifest source graph carries the M256-A002 contract and all four closure booleans as `true`
   - the `Widget` interface and implementation declaration entries carry `class:Widget`, `metaclass:Widget`, `class:Base`, and `metaclass:Base` in the expected fields
   - the `Widget` class node carries `instance_method_owner_identity = class:Widget`, `class_method_owner_identity = metaclass:Widget`, and `realization_identity_complete = true`
   - the new declaration-owned owner edges exist in the source graph
   - `module.ll` carries the `executable_class_metaclass_source_closure` summary with the expected edge counts.

## Non-Goals and Fail-Closed Rules

- `M256-A002` does not implement live runtime class realization.
- `M256-A002` does not add root-class bootstrapping.
- `M256-A002` does not add category merge or protocol conformance runtime behavior.
- `M256-A002` does not change the frozen M253 class-descriptor emission payload model.
- If declaration-owned parent or method-owner identities drift from the source graph edges, the source graph must fail closed rather than silently degrading.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m256-a002-class-and-metaclass-declaration-completeness-plus-inheritance-modeling`.
- `package.json` includes `test:tooling:m256-a002-class-and-metaclass-declaration-completeness-plus-inheritance-modeling`.
- `package.json` includes `check:objc3c:m256-a002-lane-a-readiness`.

## Validation

- `python scripts/check_m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation.py -q`
- `npm run check:objc3c:m256-a002-lane-a-readiness`

## Evidence Path

- `tmp/reports/m256/M256-A002/class_metaclass_declaration_completeness_and_inheritance_modeling_summary.json`
