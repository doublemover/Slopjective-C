# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Core Feature Expansion Expectations (B004)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-core-feature-expansion/m243-b004-v1`
Status: Accepted
Scope: M243 lane-B core feature expansion continuity for semantic diagnostic taxonomy and ARC fix-it synthesis closure.

## Objective

Implement lane-B core feature expansion closure so semantic diagnostics taxonomy
and fix-it synthesis preserve typed-handoff continuity and replay-key/payload
accounting through explicit fail-closed gates.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-B003`
- M243-B003 core feature implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_b003_expectations.md`
  - `spec/planning/compiler/m243/m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_packet.md`
  - `scripts/check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py`
- Packet/checker/test assets for B004 remain mandatory:
  - `spec/planning/compiler/m243/m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_packet.md`
  - `scripts/check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py`

## Deterministic Invariants

1. `Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface`
   remains the canonical lane-B core feature expansion surface for:
   - typed sema handoff-key continuity against B003 replay metadata
   - ARC diagnostics/fix-it payload accounting continuity from sema parity
   - replay-key closure over core feature and expansion keys
2. `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface(...)`
   remains the canonical closure-builder for M243 lane-B B004.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface(...)`
   and stores the resulting surface in `Objc3FrontendPipelineResult`.
4. Core-feature expansion readiness remains fail-closed through
   `IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurfaceReady(...)`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-B B004
  semantic diagnostic taxonomy/fix-it synthesis core feature expansion
  anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B semantic diagnostic
  taxonomy/fix-it synthesis core feature expansion fail-closed governance
  wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B
  semantic diagnostic taxonomy/fix-it synthesis core feature expansion metadata
  anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-b004-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m243-b004-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m243-b004-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m243-b004-lane-b-readiness`

## Evidence Path

- `tmp/reports/m243/M243-B004/semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_summary.json`
