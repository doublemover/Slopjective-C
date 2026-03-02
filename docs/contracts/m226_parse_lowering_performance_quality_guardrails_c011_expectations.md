# M226 Parse-Lowering Performance/Quality Guardrails Expectations (C011)

Contract ID: `objc3c-parse-lowering-performance-quality-guardrails-contract/m226-c011-v1`
Status: Accepted
Scope: Parse/lowering performance and quality guardrails in `native/objc3c/src/pipeline/*`.

## Objective

Add deterministic performance/quality guardrail coverage on top of C010
conformance-corpus readiness so lowering only proceeds when parser token budget,
diagnostic-code surface, and hardening invariants all pass fail-closed checks.

## Required Invariants

1. Readiness surface tracks guardrail evidence:
   - `parse_lowering_performance_quality_guardrails_consistent`
   - `parse_lowering_performance_quality_guardrails_case_count`
   - `parse_lowering_performance_quality_guardrails_passed_case_count`
   - `parse_lowering_performance_quality_guardrails_failed_case_count`
   - `parse_lowering_performance_quality_guardrails_key`
2. Readiness builder pins and computes guardrail coverage:
   - `kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount`
   - `BuildObjc3ParseLoweringPerformanceQualityGuardrailsKey(...)`
3. Guardrail consistency requires complete pass across token-budget, diagnostic
   surface, diagnostics hardening, edge robustness, recovery hardening, and C010
   corpus consistency.
4. `ready_for_lowering` requires
   `parse_lowering_performance_quality_guardrails_ready`.
5. Readiness failure reason includes:
   - `parse-lowering performance/quality guardrails are inconsistent`
6. Manifest projection includes guardrail fields under `parse_lowering_readiness`.

## Validation

- `python scripts/check_m226_c011_parse_lowering_performance_quality_guardrails_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c011_parse_lowering_performance_quality_guardrails_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/m226_c011_parse_lowering_performance_quality_guardrails_contract_summary.json`
