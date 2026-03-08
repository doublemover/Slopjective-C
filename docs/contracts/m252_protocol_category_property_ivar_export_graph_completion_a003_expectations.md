# M252 Protocol Category Property Ivar Export Graph Completion Expectations (A003)

Status: Accepted
Scope: M252 lane-A core feature implementation for protocol/category/property/method/ivar executable metadata graph closure.

## Objective

Extend `Objc3ExecutableMetadataSourceGraph` from the A002 class/metaclass packet into a complete executable metadata export graph for protocols, categories, properties, methods, and ivars while preserving the existing fail-closed boundary and keeping `ready_for_lowering == false`.

## Required Invariants

1. `pipeline/objc3_frontend_types.h` defines first-class executable metadata graph structs for:
   - protocol nodes,
   - category nodes,
   - property nodes,
   - method nodes,
   - ivar nodes,
   - the aggregate `Objc3ExecutableMetadataSourceGraph` packet.
2. The graph packet keeps the existing canonical packet contract id:
   - `objc3c-executable-metadata-source-graph-completeness/m252-a002-v1`.
3. `pipeline/objc3_frontend_pipeline.cpp` builds deterministic protocol/category/property/method/ivar nodes and owner edges through `BuildExecutableMetadataSourceGraph(...)` without regressing A002 interface/implementation/class/metaclass coverage.
4. Protocol graph nodes preserve:
   - `protocol:` owner identities,
   - inherited protocol owner identities,
   - property/method counts,
   - forward-declaration state.
5. Category graph nodes preserve:
   - canonical `category:Class(Category)` owner identities,
   - declaration owners on `interface:` / `implementation:` surfaces,
   - runtime class attachment on the canonical `class:` surface,
   - adopted protocol owner identities,
   - interface/implementation method and property counts.
6. Property, method, and ivar graph nodes preserve explicit:
   - declaration-owner identities,
   - export-owner identities,
   - deterministic node owner identities,
   - owner-kind / owner-name source-model fields.
7. `pipeline/objc3_frontend_artifacts.cpp` publishes the expanded graph under `frontend.pipeline.semantic_surface.objc_executable_metadata_source_graph` with explicit:
   - `protocol_node_entries`,
   - `category_node_entries`,
   - `property_node_entries`,
   - `method_node_entries`,
   - `ivar_node_entries`,
   - `owner_edges`.
8. The owner-edge inventory must explicitly cover:
   - protocol inheritance,
   - category attachment to class/interface/implementation/protocol owners,
   - property declaration/export ownership,
   - method declaration/export ownership,
   - ivar/property binding ownership,
   - property getter/setter method attachment when selectors resolve.
9. Dynamic validation must prove `source_graph_complete == true`, `ready_for_semantic_closure == true`, and `ready_for_lowering == false` on both:
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`,
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`.

## Non-Goals and Fail-Closed Rules

- `M252-A003` does not make the graph lowering-ready or runtime-ingest ready.
- `M252-A003` does not add lane-B ambiguity diagnostics or export legality enforcement.
- `M252-A003` does not claim startup registration or runtime metadata section emission are complete.
- The contract fails closed when:
  - protocol/category/property/method/ivar entries regress to count-only reporting,
  - category owners stop canonicalizing to `category:Class(Category)`,
  - member nodes stop publishing declaration/export owners,
  - owner edges stop being deterministic,
  - either happy-path fixture stops proving complete graph closure.

## Reference Fixtures

- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m252-a003-protocol-category-property-ivar-export-graph-completion`.
- `package.json` includes
  `test:tooling:m252-a003-protocol-category-property-ivar-export-graph-completion`.
- `package.json` includes `check:objc3c:m252-a003-lane-a-readiness`.

## Validation

- `python scripts/check_m252_a003_protocol_category_property_ivar_export_graph_completion.py`
- `python -m pytest tests/tooling/test_check_m252_a003_protocol_category_property_ivar_export_graph_completion.py -q`
- `npm run check:objc3c:m252-a003-lane-a-readiness`

## Evidence Path

- `tmp/reports/m252/M252-A003/executable_metadata_export_graph_completion_summary.json`
