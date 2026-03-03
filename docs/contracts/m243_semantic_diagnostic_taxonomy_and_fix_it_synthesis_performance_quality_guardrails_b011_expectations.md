# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Performance Quality Guardrails Expectations (B011)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-performance-quality-guardrails/m243-b011-v1`
Status: Accepted
Scope: M243 lane-B performance and quality guardrails for semantic diagnostic taxonomy and ARC fix-it synthesis closure.

## Objective

Expand lane-B conformance-corpus closure so semantic diagnostic taxonomy and
fix-it synthesis preserve deterministic performance/quality-guardrail
consistency/readiness and performance-quality-guardrails-key continuity in
addition to B010 conformance-corpus closure.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-B010`
- M243-B010 conformance corpus expansion anchors remain mandatory prerequisites:
  - `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_b010_expectations.md`
  - `spec/planning/compiler/m243/m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_packet.md`
  - `scripts/check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`
- Packet/checker/test assets for B011 remain mandatory:
  - `spec/planning/compiler/m243/m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_packet.md`
  - `scripts/check_m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_contract.py`
  - `tests/tooling/test_check_m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_contract.py`

## Deterministic Invariants

1. `Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface`
   remains the canonical lane-B performance/quality-guardrails surface.
2. `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsKey(...)`
   and
   `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface(...)`
   remain the canonical fail-closed B011 builders.
3. `RunObjc3FrontendPipeline(...)` wires the B011 surface builder and stores
   the resulting surface in `Objc3FrontendPipelineResult`.
4. Performance/quality guardrails readiness remains fail-closed through
   `IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurfaceReady(...)`.
5. B011 performance/quality guardrails closure remains keyed by B010
   conformance-corpus readiness plus parse-lowering performance/quality
   guardrails consistency/case-accounting and replay-key determinism continuity.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-B B011
  semantic diagnostic taxonomy/fix-it synthesis performance and quality
  guardrails anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B semantic diagnostic
  taxonomy/fix-it synthesis performance and quality guardrails fail-closed
  governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B
  semantic diagnostic taxonomy/fix-it synthesis performance and quality
  guardrails metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-b011-semantic-diagnostic-taxonomy-and-fix-it-synthesis-performance-quality-guardrails-contract`.
- `package.json` includes
  `test:tooling:m243-b011-semantic-diagnostic-taxonomy-and-fix-it-synthesis-performance-quality-guardrails-contract`.
- `package.json` includes `check:objc3c:m243-b011-lane-b-readiness`.
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m243-b010-lane-b-readiness`
  - `check:objc3c:m243-b011-lane-b-readiness`

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m243_b010_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_contract.py`
- `python scripts/check_m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b011_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_contract.py -q`
- `npm run check:objc3c:m243-b011-lane-b-readiness`

## Evidence Path

- `tmp/reports/m243/M243-B011/semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_summary.json`
