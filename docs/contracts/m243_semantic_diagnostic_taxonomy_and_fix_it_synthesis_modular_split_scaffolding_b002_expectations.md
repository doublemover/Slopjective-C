# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Modular Split Scaffolding Expectations (B002)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-modular-split-scaffolding/m243-b002-v1`
Status: Accepted
Scope: `native/objc3c/src/pipeline/*` semantic diagnostic taxonomy/fix-it modular split scaffold continuity.

## Objective

Freeze the B002 modular split/scaffolding boundary so semantic diagnostics
taxonomy and ARC fix-it synthesis remain deterministic and fail-closed between
sema pass-flow surfaces and typed sema-to-lowering handoff surfaces.

## Deterministic Invariants

1. `Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold` remains the
   canonical lane-B modular split scaffold surface:
   - `sema_pass_flow_summary_present`
   - `sema_pass_flow_summary_ready`
   - `sema_parity_surface_present`
   - `sema_parity_surface_ready`
   - `diagnostics_taxonomy_consistent`
   - `arc_diagnostics_fixit_summary_deterministic`
   - `arc_diagnostics_fixit_handoff_deterministic`
   - `typed_sema_handoff_deterministic`
   - `modular_split_ready`
2. `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold(...)`
   remains the canonical closure-builder for B002 modular split continuity.
3. `RunObjc3FrontendPipeline(...)` wires
   `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold(...)` and
   stores the resulting scaffold in `Objc3FrontendPipelineResult`.
4. `native/objc3c/src/ARCHITECTURE.md` remains authoritative for the M243
   lane-B B002 modular split scaffold anchor.

## Validation

- `python scripts/check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m243-b002-lane-b-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-B002/semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract_summary.json`
