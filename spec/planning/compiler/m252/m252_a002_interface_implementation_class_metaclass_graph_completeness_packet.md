# M252-A002 Interface Implementation Class Metaclass Graph Completeness Packet

Packet: `M252-A002`
Milestone: `M252`
Lane: `A`
Implementation date: `2026-03-07`
Dependencies: `M252-A001`

## Purpose

Promote the executable metadata source graph from a frozen count-level contract
into a real frontend packet with explicit interface, implementation, class, and
metaclass nodes plus deterministic owner edges.

## Scope Anchors

- Contract:
  `docs/contracts/m252_interface_implementation_class_metaclass_graph_completeness_a002_expectations.md`
- Checker:
  `scripts/check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py`
- Tooling tests:
  `tests/tooling/test_check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py`
- Reference fixture:
  - `tests/tooling/fixtures/native/m252_executable_metadata_graph_class_metaclass.objc3`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m252-a002-interface-implementation-class-metaclass-graph-completeness`
  - `test:tooling:m252-a002-interface-implementation-class-metaclass-graph-completeness`
  - `check:objc3c:m252-a002-lane-a-readiness`
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

- Canonical contract id:
  `objc3c-executable-metadata-source-graph-completeness/m252-a002-v1`
- Canonical packet type:
  `Objc3ExecutableMetadataSourceGraph`
- Canonical declaration owner identities:
  - interfaces: parser semantic-link symbols (`interface:Class`)
  - implementations: parser semantic-link symbols (`implementation:Class`)
- Canonical runtime owner identities:
  - classes: `class:Class`
  - metaclasses: `metaclass:Class`
- Canonical edge ordering:
  `lexicographic-kind-source-target`

## Happy-Path Graph Proof

The checker runs the frontend C API runner against one class/superclass fixture
that proves:

1. two interface nodes (`Base`, `Widget`),
2. two implementation nodes,
3. two class nodes,
4. two metaclass nodes,
5. superclass edge `class:Widget -> class:Base`,
6. super-metaclass edge `metaclass:Widget -> metaclass:Base`,
7. interface-to-class, implementation-to-class, implementation-to-interface,
   and class-to-metaclass owner edges.

## Gate Commands

- `python scripts/check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py`
- `python -m pytest tests/tooling/test_check_m252_a002_interface_implementation_class_metaclass_graph_completeness.py -q`
- `npm run check:objc3c:m252-a002-lane-a-readiness`

## Evidence Output

- `tmp/reports/m252/M252-A002/executable_metadata_graph_completeness_summary.json`
