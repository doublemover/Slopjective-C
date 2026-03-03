# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Conformance Corpus Expansion Expectations (B010)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-conformance-corpus-expansion/m243-b010-v1`
Status: Accepted
Scope: M243 lane-B conformance corpus expansion for semantic diagnostic taxonomy and ARC fix-it synthesis closure.

## Objective

Expand lane-B conformance-matrix closure so semantic diagnostic taxonomy and
fix-it synthesis preserve deterministic conformance-corpus
consistency/readiness and conformance-corpus-key continuity in addition to
B009 conformance-matrix closure.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-B009`
- M243-B009 conformance matrix implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_b009_expectations.md`
  - `spec/planning/compiler/m243/m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_packet.md`
  - `scripts/check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`
- Packet/checker/test assets for B010 remain mandatory:
  - `spec/planning/compiler/m243/m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_packet.md`
  - `scripts/check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`

## Deterministic Invariants

1. `Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface`
   remains the canonical lane-B conformance corpus expansion surface.
2. `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionKey(...)`
   and
   `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface(...)`
   remain the canonical fail-closed B010 builders.
3. `RunObjc3FrontendPipeline(...)` wires the B010 surface builder and stores
   the resulting surface in `Objc3FrontendPipelineResult`.
4. Conformance corpus readiness remains fail-closed through
   `IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurfaceReady(...)`.
5. B010 conformance corpus closure remains keyed by B009 conformance matrix
   readiness plus parse-lowering conformance corpus consistency/case-accounting
   and replay-key continuity.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-B B010
  semantic diagnostic taxonomy/fix-it synthesis conformance corpus expansion
  anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B semantic diagnostic
  taxonomy/fix-it synthesis conformance corpus expansion fail-closed governance
  wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B
  semantic diagnostic taxonomy/fix-it synthesis conformance corpus expansion
  metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-b010-semantic-diagnostic-taxonomy-and-fix-it-synthesis-conformance-corpus-expansion-contract`.
- `package.json` includes
  `test:tooling:m243-b010-semantic-diagnostic-taxonomy-and-fix-it-synthesis-conformance-corpus-expansion-contract`.
- `package.json` includes `check:objc3c:m243-b010-lane-b-readiness`.
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m243-b009-lane-b-readiness`
  - `check:objc3c:m243-b010-lane-b-readiness`

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`
- `python scripts/check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m243-b010-lane-b-readiness`

## Evidence Path

- `tmp/reports/m243/M243-B010/semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_summary.json`
