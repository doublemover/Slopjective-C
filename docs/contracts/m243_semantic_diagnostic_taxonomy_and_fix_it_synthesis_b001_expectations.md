# M243 Semantic Diagnostic Taxonomy and Fix-it Synthesis Expectations (B001)

Contract ID: `objc3c-semantic-diagnostic-taxonomy-and-fixit-synthesis-freeze/m243-b001-v1`
Status: Accepted
Scope: semantic pass-flow diagnostics taxonomy, diagnostics bus publication contract, and ARC fix-it synthesis determinism anchors.

## Objective

Freeze lane-B semantic diagnostics taxonomy and fix-it synthesis anchors so
later M243 expansions cannot regress deterministic diagnostics accounting,
pass-flow monotonicity, or ARC fix-it handoff determinism.

## Required Invariants

1. Semantic pass-flow summary remains fail-closed and deterministic:
   - `Objc3SemaPassFlowSummary` tracks pass order and diagnostics accounting.
   - `IsReadyObjc3SemaPassFlowSummary(...)` requires diagnostics monotonicity,
     diagnostics bus consistency, and diagnostics hardening satisfaction.
2. Diagnostics publication contract remains explicit:
   - `Objc3SemaDiagnosticsBus` `Publish(...)` and `PublishBatch(...)` behaviors
     remain canonical and size-accounted.
3. Fix-it synthesis taxonomy remains explicit and deterministic:
   - ARC diagnostics/fix-it summary counters and deterministic handoff fields
     remain present in `sema/objc3_sema_pass_manager_contract.h`.
4. Pass-flow scaffold interface remains stable:
   - `FinalizeObjc3SemaPassFlowSummary(...)` preserves deterministic diagnostics
     and deterministic type metadata handoff parameters.
5. Pipeline handoff remains anchored:
   - `pipeline/objc3_typed_sema_to_lowering_contract_surface.h` continues to
     project semantic diagnostics determinism into typed-sema/lowering readiness.
6. Architecture anchor remains authoritative in
   `native/objc3c/src/ARCHITECTURE.md`.

## Validation

- `python scripts/check_m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract.py -q`
- `npm run check:objc3c:m243-b001-lane-b-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m243/M243-B001/semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract_summary.json`
