# M252 Executable Metadata Source Graph Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-executable-metadata-source-graph-freeze/m252-a001-v1`
Status: Accepted
Issue: `#7071`
Scope: M252 lane-A contract and architecture freeze for the canonical
executable metadata source graph.

## Objective

Freeze one deterministic source graph for class, metaclass, protocol, category,
property, ivar, and method metadata so later semantic-closure and lowering work
preserves one canonical owner/edge model and fails closed on drift.

## Required Invariants

1. `parse/objc3_parser.cpp` remains the canonical owner-identity anchor for
   executable metadata nodes via:
   - `BuildObjcContainerScopeOwner`
   - `BuildObjcCategorySemanticLinkSymbol`
   - `BuildScopePathLexicographic`
   - `AssignObjcPropertySynthesisIvarBindingSymbols`
2. `sema/objc3_sema_contract.h` remains the canonical semantic graph-summary
   declaration point for:
   - `Objc3InterfaceImplementationSummary`
   - `Objc3ProtocolCategoryCompositionSummary`
   - `Objc3ClassProtocolCategoryLinkingSummary`
   - `Objc3PropertySynthesisIvarBindingSummary`
   - `Objc3SemanticIntegrationSurface`
3. `sema/objc3_semantic_passes.cpp` continues to recompute and validate the
   class/protocol/category linking and protocol/category composition summaries
   deterministically before artifact emission.
4. `pipeline/objc3_frontend_artifacts.cpp` publishes one manifest evidence node:
   - `objc_executable_metadata_source_graph`
5. The frozen graph keeps:
   - owner identity model
     `semantic-link-symbol-lexicographic-owner-identity`
   - metaclass node policy
     `metaclass-nodes-derived-from-resolved-interface-symbols`
   - `ready_for_semantic_closure = false`
   - `ready_for_lowering = false`

## Non-Goals and Fail-Closed Rules

- `M252-A001` does not complete metadata semantic closure.
- `M252-A001` does not add new semantic diagnostics for graph ambiguity.
- `M252-A001` does not make the graph lowering-ready.
- `M252-A001` does not perform runtime ingest packaging or startup
  registration.
- The frozen graph must therefore remain a source-graph contract and evidence
  surface only.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m252-a001-executable-metadata-source-graph-contract`.
- `package.json` includes
  `test:tooling:m252-a001-executable-metadata-source-graph-contract`.
- `package.json` includes `check:objc3c:m252-a001-lane-a-readiness`.

## Validation

- `python scripts/check_m252_a001_executable_metadata_source_graph_contract.py`
- `python -m pytest tests/tooling/test_check_m252_a001_executable_metadata_source_graph_contract.py -q`
- `npm run check:objc3c:m252-a001-lane-a-readiness`

## Evidence Path

- `tmp/reports/m252/M252-A001/executable_metadata_source_graph_contract_summary.json`
