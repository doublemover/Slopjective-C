# M252-A003 Protocol Category Property Ivar Export Graph Completion Packet

Packet: `M252-A003`
Milestone: `M252`
Lane: `A`
Implementation date: `2026-03-07`
Dependencies: `M252-A002`

## Purpose

Complete the executable metadata source graph for protocols, categories, properties, methods, and ivars so the frontend emits one deterministic export graph packet instead of relying on count-only summaries outside the class/metaclass surface.

## Scope Anchors

- Contract:
  `docs/contracts/m252_protocol_category_property_ivar_export_graph_completion_a003_expectations.md`
- Checker:
  `scripts/check_m252_a003_protocol_category_property_ivar_export_graph_completion.py`
- Tooling tests:
  `tests/tooling/test_check_m252_a003_protocol_category_property_ivar_export_graph_completion.py`
- Reference fixtures:
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m252-a003-protocol-category-property-ivar-export-graph-completion`
  - `test:tooling:m252-a003-protocol-category-property-ivar-export-graph-completion`
  - `check:objc3c:m252-a003-lane-a-readiness`
- Code anchors:
  - `native/objc3c/src/parse/objc3_parser.cpp`
  - `native/objc3c/src/sema/objc3_sema_contract.h`
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `docs/objc3c-native.md`

## Canonical Packet

- Canonical packet type:
  `Objc3ExecutableMetadataSourceGraph`
- Canonical packet contract id (unchanged from A002):
  `objc3c-executable-metadata-source-graph-completeness/m252-a002-v1`
- Canonical declaration owner identities:
  - protocols: `protocol:Protocol`
  - class interfaces: `interface:Class`
  - class implementations: `implementation:Class`
  - category interfaces: `interface:Class(Category)`
  - category implementations: `implementation:Class(Category)`
- Canonical runtime/export owner identities:
  - classes: `class:Class`
  - metaclasses: `metaclass:Class`
  - categories: `category:Class(Category)`
  - protocols: `protocol:Protocol`
- Canonical edge ordering:
  `lexicographic-kind-source-target`

## Happy-Path Graph Proofs

The checker runs the frontend C API runner against two fixtures and proves:

1. class/protocol/property/ivar closure on the class fixture:
   - protocol inheritance,
   - class and metaclass export owners,
   - property declaration/export owner edges,
   - method declaration/export owner edges,
   - ivar/property binding edges.
2. category/protocol/property closure on the category fixture:
   - canonical `category:Widget(Debug)` owner identity,
   - category-to-class / interface / implementation / protocol edges,
   - protocol property getter attachment,
   - property-to-ivar binding edges.

## Gate Commands

- `python scripts/check_m252_a003_protocol_category_property_ivar_export_graph_completion.py`
- `python -m pytest tests/tooling/test_check_m252_a003_protocol_category_property_ivar_export_graph_completion.py -q`
- `npm run check:objc3c:m252-a003-lane-a-readiness`

## Evidence Output

- `tmp/reports/m252/M252-A003/executable_metadata_export_graph_completion_summary.json`
