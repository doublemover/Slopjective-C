# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Core Feature Implementation Expectations (B003)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-core-feature-implementation/m243-b003-v1`
Status: Accepted
Scope: M243 lane-B core feature implementation continuity for semantic diagnostic taxonomy and ARC fix-it synthesis closure.

## Objective

Implement lane-B core feature closure so semantic diagnostics taxonomy and
fix-it synthesis remain deterministic and fail-closed for diagnostics UX and
fix-it engine behavior. Code/spec anchors and milestone optimization
improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-B002`
- M243-B002 modular split/scaffolding anchors remain mandatory prerequisites:
  - `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_b002_expectations.md`
  - `spec/planning/compiler/m243/m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_packet.md`
  - `scripts/check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py`
- Packet/checker/test assets for B003 remain mandatory:
  - `spec/planning/compiler/m243/m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_packet.md`
  - `scripts/check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py`

## Deterministic Invariants

1. `Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface`
   remains the canonical lane-B core feature implementation surface for:
   - semantic pass-flow diagnostics case accounting closure
   - ARC diagnostics/fix-it accounting determinism
   - typed sema handoff continuity
   - replay-key completeness for lane-B readiness
2. `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface(...)`
   remains the canonical closure-builder for M243 lane-B B003.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface(...)`
   and stores the resulting surface in `Objc3FrontendPipelineResult`.
4. Core-feature readiness remains fail-closed through
   `IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurfaceReady(...)`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-B B003
  semantic diagnostic taxonomy/fix-it synthesis core feature implementation
  anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B semantic diagnostic
  taxonomy/fix-it synthesis core feature implementation fail-closed governance
  wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B
  semantic diagnostic taxonomy/fix-it synthesis core feature metadata anchor
  wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m243-b003-semantic-diagnostic-taxonomy-and-fix-it-synthesis-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m243-b003-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m243-b003-lane-b-readiness`

## Evidence Path

- `tmp/reports/m243/M243-B003/semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_summary.json`
