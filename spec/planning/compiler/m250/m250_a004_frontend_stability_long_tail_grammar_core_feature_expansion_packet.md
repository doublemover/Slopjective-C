# M250-A004 Frontend Stability and Long-Tail Grammar Closure Core Feature Expansion Packet

Packet: `M250-A004`
Milestone: `M250`
Lane: `A`
Dependencies: `M250-A003`

## Scope

Expand lane-A long-tail grammar closure by externalizing expansion-accounting and replay-key guardrails in parse/lowering readiness surfaces.

## Anchors

- Contract: `docs/contracts/m250_frontend_stability_long_tail_grammar_core_feature_expansion_a004_expectations.md`
- Checker: `scripts/check_m250_a004_frontend_stability_long_tail_grammar_core_feature_expansion_contract.py`
- Tooling tests: `tests/tooling/test_check_m250_a004_frontend_stability_long_tail_grammar_core_feature_expansion_contract.py`
- Parse/lowering readiness surface: `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Frontend artifact projection: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Surface type projection: `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`

## Required Evidence

- `tmp/reports/m250/M250-A004/frontend_stability_long_tail_grammar_core_feature_expansion_contract_summary.json`

## Determinism Criteria

- Expansion-accounting and replay-key guardrails are first-class readiness fields.
- Parse/lowering readiness fails closed when long-tail expansion identity drifts.
- Artifact projection carries long-tail expansion identity without bypass paths.
