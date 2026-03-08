# M252 Interface Implementation Class Metaclass Graph Completeness Expectations (A002)

Contract ID: `objc3c-executable-metadata-source-graph-completeness/m252-a002-v1`
Status: Accepted
Scope: M252 lane-A core feature implementation for first-class interface, implementation, class, and metaclass graph completeness.

## Objective

Materialize one deterministic executable metadata source graph for class-shaped
Objective-C declarations so the frontend emits explicit interface,
implementation, class, and metaclass nodes plus owner edges instead of only
count-level graph summaries.

## Required Invariants

1. `pipeline/objc3_frontend_types.h` defines first-class executable metadata
   graph structs for:
   - interface nodes,
   - implementation nodes,
   - class nodes,
   - metaclass nodes,
   - owner edges,
   - the aggregate `Objc3ExecutableMetadataSourceGraph` packet.
2. `pipeline/objc3_frontend_pipeline.cpp` builds the graph through
   `BuildExecutableMetadataSourceGraph(...)` using parser semantic-link owner
   identities for interface/implementation declarations and runtime owner
   identities on the canonical `class:` / `metaclass:` surface.
3. Interface graph nodes preserve:
   - interface owner identity,
   - class owner identity,
   - metaclass owner identity,
   - superclass owner identity,
   - method split (`class_method_count`, `instance_method_count`).
4. Implementation graph nodes preserve:
   - implementation owner identity,
   - matching interface owner identity,
   - class owner identity,
   - metaclass owner identity,
   - method split (`class_method_count`, `instance_method_count`).
5. Class graph nodes aggregate the canonical runtime owner surface per class and
   explicitly publish:
   - interface/implementation owner identities,
   - metaclass owner identity,
   - superclass owner identity,
   - interface/implementation method and property counts.
6. Metaclass graph nodes are first-class entries, not implied counts. They must
   preserve:
   - metaclass owner identity,
   - class owner identity,
   - interface/implementation owner identities,
   - superclass metaclass owner identity,
   - class-method ownership counts.
7. `pipeline/objc3_frontend_artifacts.cpp` publishes the graph under
   `frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph`
   with explicit:
   - `interface_node_entries`,
   - `implementation_node_entries`,
   - `class_node_entries`,
   - `metaclass_node_entries`,
   - `owner_edges`.
8. Dynamic validation uses `objc3c-frontend-c-api-runner.exe` with the A002
   reference fixture and requires zero lex/parse/sema/lower diagnostics,
   `source_graph_complete == true`, `ready_for_semantic_closure == true`, and
   `ready_for_lowering == false`.

## Non-Goals and Fail-Closed Rules

- `M252-A002` does not complete protocol/category/property/ivar graph closure;
  that is `M252-A003`.
- `M252-A002` does not add ambiguity, inheritance legality, or duplicate-owner
  diagnostics; that is lane-B work.
- `M252-A002` does not make the graph lowering-ready or runtime-ingest ready.
- The contract fails closed when:
  - the graph regresses to count-only reporting,
  - metaclass nodes stop being explicit entries,
  - owner edges stop being deterministic,
  - the happy-path fixture no longer proves superclass and metaclass edges.

## Reference Fixture

- `tests/tooling/fixtures/native/m252_executable_metadata_graph_class_metaclass.objc3`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m252-a002-interface-implementation-class-metaclass-graph-completeness`.
- `package.json` includes
  `test:tooling:m252-a002-interface-implementation-class-metaclass-graph-completeness`.
- `package.json` includes `check:objc3c:m252-a002-lane-a-readiness`.

## Validation

- `python scripts/check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py`
- `python -m pytest tests/tooling/test_check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py -q`
- `npm run check:objc3c:m252-a002-lane-a-readiness`

## Evidence Path

- `tmp/reports/m252/M252-A002/executable_metadata_graph_completeness_summary.json`
