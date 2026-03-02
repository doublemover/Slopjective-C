# Frontend Stability and Long-Tail Grammar Closure Core Feature Expansion Expectations (M250-A004)

Contract ID: `objc3c-frontend-stability-long-tail-grammar-core-feature-expansion/m250-a004-v1`
Status: Accepted
Scope: lane-A parse/lowering readiness expansion guardrails for long-tail grammar accounting and replay-key closure.

## Objective

Expand A003 long-tail grammar closure so parse/lowering readiness exposes explicit expansion-accounting and replay-key gates, with deterministic expansion key projection and fail-closed readiness behavior.

## Deterministic Invariants

1. `Objc3ParseLoweringReadinessSurface` carries expansion guardrail fields:
   - `long_tail_grammar_expansion_accounting_consistent`
   - `long_tail_grammar_replay_keys_ready`
   - `long_tail_grammar_expansion_ready`
   - `long_tail_grammar_expansion_key`
2. `BuildObjc3ParseLoweringReadinessSurface(...)` computes long-tail expansion accounting and replay-key readiness deterministically from parser snapshot + parse artifact keys.
3. Parse recovery/determinism hardening and matrix readiness require long-tail expansion readiness and key presence.
4. Failure reasons remain explicit for long-tail expansion-accounting/replay/expansion drift.
5. Frontend artifact JSON projects long-tail expansion fields from parse/lowering readiness.

## Validation

- `python scripts/check_m250_a004_frontend_stability_long_tail_grammar_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m250_a004_frontend_stability_long_tail_grammar_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m250-a004-lane-a-readiness`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m250/M250-A004/frontend_stability_long_tail_grammar_core_feature_expansion_contract_summary.json`
