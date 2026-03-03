# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Conformance Matrix Implementation Expectations (B009)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-conformance-matrix-implementation/m243-b009-v1`
Status: Accepted
Scope: M243 lane-B conformance matrix implementation for semantic diagnostic taxonomy and ARC fix-it synthesis closure.

## Objective

Extend lane-B recovery/determinism closure so semantic diagnostic taxonomy and
fix-it synthesis preserve deterministic conformance matrix consistency/readiness
and conformance-matrix-key continuity in addition to B008 recovery/determinism
closure.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-B008`
- M243-B008 recovery/determinism hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_b008_expectations.md`
  - `spec/planning/compiler/m243/m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_packet.md`
  - `scripts/check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py`
- Packet/checker/test assets for B009 remain mandatory:
  - `spec/planning/compiler/m243/m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_packet.md`
  - `scripts/check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`

## Deterministic Invariants

1. `Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface`
   remains the canonical lane-B conformance matrix implementation surface.
2. `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationKey(...)`
   and
   `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface(...)`
   remain the canonical fail-closed B009 builders.
3. `RunObjc3FrontendPipeline(...)` wires the B009 surface builder and stores
   the resulting surface in `Objc3FrontendPipelineResult`.
4. Conformance matrix readiness remains fail-closed through
   `IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurfaceReady(...)`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-b009-semantic-diagnostic-taxonomy-and-fix-it-synthesis-conformance-matrix-implementation-contract`.
- `package.json` includes
  `test:tooling:m243-b009-semantic-diagnostic-taxonomy-and-fix-it-synthesis-conformance-matrix-implementation-contract`.
- `package.json` includes `check:objc3c:m243-b009-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b009_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m243-b009-lane-b-readiness`

## Evidence Path

- `tmp/reports/m243/M243-B009/semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_summary.json`
