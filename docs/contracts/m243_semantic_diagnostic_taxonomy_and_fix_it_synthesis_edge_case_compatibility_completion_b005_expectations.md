# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Edge-Case Compatibility Completion Expectations (B005)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-edge-case-compatibility-completion/m243-b005-v1`
Status: Accepted
Scope: M243 lane-B edge-case and compatibility completion for semantic diagnostic taxonomy and ARC fix-it synthesis closure.

## Objective

Complete lane-B edge-case compatibility closure so semantic diagnostic taxonomy
and fix-it synthesis preserve typed handoff continuity, parse/lowering
compatibility handoff, and replay-key determinism through explicit fail-closed
readiness gates.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-B004`
- M243-B004 core feature expansion anchors remain mandatory prerequisites:
  - `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_b004_expectations.md`
  - `spec/planning/compiler/m243/m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_packet.md`
  - `scripts/check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py`
- Packet/checker/test assets for B005 remain mandatory:
  - `spec/planning/compiler/m243/m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_packet.md`
  - `scripts/check_m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_contract.py`

## Deterministic Invariants

1. `Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface`
   remains the canonical lane-B edge-case compatibility surface for:
   - typed sema handoff-key continuity against B004 expansion metadata
   - parse/lowering compatibility handoff continuity
   - parse artifact edge-case robustness and replay-key determinism closure
2. `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface(...)`
   remains the canonical closure-builder for M243 lane-B B005.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface(...)`
   and stores the resulting surface in `Objc3FrontendPipelineResult`.
4. Edge-case compatibility readiness remains fail-closed through
   `IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurfaceReady(...)`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-B B005
  semantic diagnostic taxonomy/fix-it synthesis edge-case compatibility
  completion anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B semantic diagnostic
  taxonomy/fix-it synthesis edge-case compatibility completion fail-closed
  governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B
  semantic diagnostic taxonomy/fix-it synthesis edge-case compatibility
  completion metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-b005-semantic-diagnostic-taxonomy-and-fix-it-synthesis-edge-case-compatibility-completion-contract`.
- `package.json` includes
  `test:tooling:m243-b005-semantic-diagnostic-taxonomy-and-fix-it-synthesis-edge-case-compatibility-completion-contract`.
- `package.json` includes `check:objc3c:m243-b005-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b005_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m243-b005-lane-b-readiness`

## Evidence Path

- `tmp/reports/m243/M243-B005/semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_completion_summary.json`
