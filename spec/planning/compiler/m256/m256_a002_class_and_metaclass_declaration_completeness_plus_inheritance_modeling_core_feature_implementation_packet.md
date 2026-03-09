# M256-A002 Class And Metaclass Declaration Completeness Plus Inheritance Modeling Core Feature Implementation Packet

Packet: `M256-A002`
Milestone: `M256`
Wave: `W48`
Lane: `A`
Issue: `#7130`
Contract ID: `objc3c-executable-class-metaclass-source-closure/m256-a002-v1`
Dependencies:
- `M256-A001`

## Objective

Upgrade the executable metadata source graph so class and metaclass realization consumes explicit declaration-owned class-object, parent, and method-owner identities.

## Canonical Boundary

- contract id `objc3c-executable-class-metaclass-source-closure/m256-a002-v1`
- parent identity model `declaration-owned-class-parent-plus-metaclass-parent-identities`
- method-owner identity model `declaration-owned-instance-class-method-owner-identities`
- class-object identity model `declaration-owned-class-and-metaclass-object-identities`
- emitted IR summary `; executable_class_metaclass_source_closure = ...`

## Acceptance Criteria

- Extend `Objc3ExecutableMetadataSourceGraph` with explicit M256-A002 contract/model fields and fail-closed readiness booleans.
- Extend interface and implementation declaration nodes so parent and method-owner identities are explicit on the declaration records.
- Extend class nodes with realization-ready method-owner and super-metaclass identities.
- Materialize the declaration-owned edge inventory in `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`.
- Republish the same closure in `native/objc3c/src/ir/objc3_ir_emitter.cpp`.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over `tests/tooling/fixtures/native/m252_executable_metadata_graph_class_metaclass.objc3` must keep the source graph complete while exposing the new closure surface in both manifest and IR output.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/m252_executable_metadata_graph_class_metaclass.objc3` proving:
   - `module.manifest.json` and `module.ll` exist,
   - the manifest source graph carries the M256-A002 contract and all four closure booleans,
   - `Widget` interface/implementation declarations expose `class:Widget`, `metaclass:Widget`, `class:Base`, and `metaclass:Base` in the expected fields,
   - the class node for `Widget` exposes stable method-owner identities,
   - the new declaration-owned owner edges are present,
   - the IR summary line reports the expected node and edge counts.

## Non-Goals

- `M256-A002` does not implement live runtime class realization.
- `M256-A002` does not implement root-class bootstrapping.
- `M256-A002` does not change the frozen M253 class-descriptor payload shape.
- `M256-A002` does not implement category merge or protocol conformance runtime behavior.

## Validation Commands

- `python scripts/check_m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m256_a002_class_and_metaclass_declaration_completeness_plus_inheritance_modeling_core_feature_implementation.py -q`
- `npm run check:objc3c:m256-a002-lane-a-readiness`

## Evidence Path

- `tmp/reports/m256/M256-A002/class_metaclass_declaration_completeness_and_inheritance_modeling_summary.json`
