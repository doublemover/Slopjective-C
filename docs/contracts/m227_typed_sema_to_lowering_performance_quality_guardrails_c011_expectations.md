# M227 Typed Sema-to-Lowering Performance/Quality Guardrails Expectations (C011)

Contract ID: `objc3c-typed-sema-to-lowering-performance-quality-guardrails/m227-c011-v1`
Status: Accepted
Scope: typed sema-to-lowering performance/quality guardrails on top of C010 conformance-corpus expansion.

## Objective

Execute issue `#5131` by extending typed sema-to-lowering and parse/lowering
readiness surfaces with performance/quality guardrail accounting, consistency,
and readiness invariants, with deterministic fail-closed alignment checks.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C010`
- `M227-C010` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_conformance_corpus_expansion_c010_expectations.md`
  - `scripts/check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py`
  - `tests/tooling/test_check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py`
  - `spec/planning/compiler/m227/m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries typed performance/quality guardrail fields:
   - `typed_performance_quality_guardrails_consistent`
   - `typed_performance_quality_guardrails_ready`
   - `typed_performance_quality_guardrails_case_count`
   - `typed_performance_quality_guardrails_passed_case_count`
   - `typed_performance_quality_guardrails_failed_case_count`
   - `typed_performance_quality_guardrails_key`
2. `Objc3ParseLoweringReadinessSurface` carries mapped typed performance/quality guardrail fields:
   - `typed_sema_performance_quality_guardrails_consistent`
   - `typed_sema_performance_quality_guardrails_ready`
   - `typed_sema_performance_quality_guardrails_case_count`
   - `typed_sema_performance_quality_guardrails_passed_case_count`
   - `typed_sema_performance_quality_guardrails_failed_case_count`
   - `typed_sema_performance_quality_guardrails_key`
3. Parse/lowering readiness fails closed when typed performance/quality
   guardrail alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c011-typed-sema-to-lowering-performance-quality-guardrails-contract`
  - `test:tooling:m227-c011-typed-sema-to-lowering-performance-quality-guardrails-contract`
  - `check:objc3c:m227-c011-lane-c-readiness`

## Validation

- `python scripts/check_m227_c010_typed_sema_to_lowering_conformance_corpus_expansion_contract.py`
- `python scripts/check_m227_c011_typed_sema_to_lowering_performance_quality_guardrails_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c011_typed_sema_to_lowering_performance_quality_guardrails_contract.py -q`
- `npm run check:objc3c:m227-c011-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C011/typed_sema_to_lowering_performance_quality_guardrails_contract_summary.json`
