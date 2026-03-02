# M226 Parser-Sema Performance and Quality Guardrails Expectations (B011)

Contract ID: `objc3c-parser-sema-performance-quality-guardrails-contract/m226-b011-v1`
Status: Accepted
Scope: Parser->sema performance and quality guardrails with fail-closed pass-manager gating.

## Objective

Freeze parser->sema guardrails so conformance hot-path logic stays bounded,
quality budgets stay explicit, and sema execution fails closed when guardrail
determinism drifts.

## Required Invariants

1. Guardrail budgets are explicit and versioned in the handoff scaffold:
   - `kObjc3ParserSemaConformanceMatrixBuilderMaxLines`
   - `kObjc3ParserSemaConformanceCorpusBuilderMaxLines`
   - `kObjc3ParserSemaHandoffScaffoldBuilderMaxLines`
2. Parser->sema guardrail surface is explicit in contract/handoff/manager:
   - `Objc3ParserSemaPerformanceQualityGuardrails`
   - `BuildObjc3ParserSemaPerformanceQualityGuardrails(...)`
   - `parser_sema_performance_quality_guardrails`
3. Pass-manager execution is fail-closed on guardrail drift:
   - `if (!result.deterministic_parser_sema_performance_quality_guardrails) { return result; }`
4. Parity/readiness requires guardrail determinism and complete pass accounting:
   - `required_guardrail_count == 7u`
   - `passed_guardrail_count == required_guardrail_count`
   - `failed_guardrail_count == 0u`
5. Tooling checker enforces bounded line counts for core parser->sema builder
   functions and fails closed on drift.

## Guarded Functions and Max Lines

| Function | Max Lines |
|---|---|
| `BuildObjc3ParserSemaConformanceMatrix` | `190` |
| `BuildObjc3ParserSemaConformanceCorpus` | `75` |
| `BuildObjc3ParserSemaHandoffScaffold` | `80` |

## Validation

- `python scripts/check_m226_b011_parser_sema_performance_quality_guardrails_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b011_parser_sema_performance_quality_guardrails_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-B011/parser_sema_performance_quality_guardrails_summary.json`
