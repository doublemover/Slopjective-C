# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Diagnostics Hardening Expectations (B007)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-diagnostics-hardening/m243-b007-v1`
Status: Accepted
Scope: M243 lane-B diagnostics hardening for semantic diagnostic taxonomy and ARC fix-it synthesis closure.

## Objective

Expand lane-B diagnostics closure so semantic diagnostic taxonomy and fix-it
synthesis preserve deterministic diagnostics-hardening consistency/readiness and
hardening-key continuity in addition to B006 edge robustness closure.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-B006`
- M243-B006 edge-case expansion and robustness anchors remain mandatory prerequisites:
  - `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_b006_expectations.md`
  - `spec/planning/compiler/m243/m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m243_b006_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_contract.py`
- Packet/checker/test assets for B007 remain mandatory:
  - `spec/planning/compiler/m243/m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_packet.md`
  - `scripts/check_m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_contract.py`

## Deterministic Invariants

1. `Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface`
   remains the canonical lane-B diagnostics hardening surface.
2. `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningKey(...)`
   and
   `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface(...)`
   remain the canonical fail-closed B007 builders.
3. `RunObjc3FrontendPipeline(...)` wires the B007 surface builder and stores
   the resulting surface in `Objc3FrontendPipelineResult`.
4. Diagnostics-hardening readiness remains fail-closed through
   `IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurfaceReady(...)`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-B B007
  semantic diagnostic taxonomy/fix-it synthesis diagnostics hardening anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B semantic diagnostic
  taxonomy/fix-it synthesis diagnostics hardening fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B
  semantic diagnostic taxonomy/fix-it synthesis diagnostics hardening metadata
  anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-b007-semantic-diagnostic-taxonomy-and-fix-it-synthesis-diagnostics-hardening-contract`.
- `package.json` includes
  `test:tooling:m243-b007-semantic-diagnostic-taxonomy-and-fix-it-synthesis-diagnostics-hardening-contract`.
- `package.json` includes `check:objc3c:m243-b007-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m243-b007-lane-b-readiness`

## Evidence Path

- `tmp/reports/m243/M243-B007/semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_summary.json`
