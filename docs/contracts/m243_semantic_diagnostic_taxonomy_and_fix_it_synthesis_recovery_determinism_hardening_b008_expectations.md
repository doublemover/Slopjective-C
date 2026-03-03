# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Recovery and Determinism Hardening Expectations (B008)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-recovery-determinism-hardening/m243-b008-v1`
Status: Accepted
Scope: M243 lane-B recovery and determinism hardening for semantic diagnostic taxonomy and ARC fix-it synthesis closure.

## Objective

Expand lane-B diagnostics-hardening closure so semantic diagnostic taxonomy and
fix-it synthesis preserve deterministic recovery consistency/readiness and
recovery-key continuity in addition to B007 diagnostics hardening closure.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-B007`
- M243-B007 diagnostics hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_b007_expectations.md`
  - `spec/planning/compiler/m243/m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_packet.md`
  - `scripts/check_m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m243_b007_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_contract.py`
- Packet/checker/test assets for B008 remain mandatory:
  - `spec/planning/compiler/m243/m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_packet.md`
  - `scripts/check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py`

## Deterministic Invariants

1. `Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface`
   remains the canonical lane-B recovery and determinism hardening surface.
2. `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningKey(...)`
   and
   `BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface(...)`
   remain the canonical fail-closed B008 builders.
3. `RunObjc3FrontendPipeline(...)` wires the B008 surface builder and stores
   the resulting surface in `Objc3FrontendPipelineResult`.
4. Recovery and determinism readiness remains fail-closed through
   `IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurfaceReady(...)`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-B B008
  semantic diagnostic taxonomy/fix-it synthesis recovery and determinism
  hardening anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B semantic diagnostic
  taxonomy/fix-it synthesis recovery and determinism hardening fail-closed
  governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B
  semantic diagnostic taxonomy/fix-it synthesis recovery and determinism
  hardening metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-b008-semantic-diagnostic-taxonomy-and-fix-it-synthesis-recovery-determinism-hardening-contract`.
- `package.json` includes
  `test:tooling:m243-b008-semantic-diagnostic-taxonomy-and-fix-it-synthesis-recovery-determinism-hardening-contract`.
- `package.json` includes `check:objc3c:m243-b008-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b008_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m243-b008-lane-b-readiness`

## Evidence Path

- `tmp/reports/m243/M243-B008/semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_summary.json`
