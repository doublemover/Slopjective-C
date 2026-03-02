# M250 Frontend Stability and Long-Tail Grammar Closure Freeze Expectations (A001)

Contract ID: `objc3c-frontend-stability-long-tail-grammar-closure-freeze/m250-a001-v1`
Status: Accepted
Scope: parser contract snapshots, parser-to-pipeline transport, and parse/lowering readiness replay gates.

## Objective

Freeze frontend stability boundaries for long-tail grammar closure so GA hardening can expand without regressing parser determinism, parser recovery replay readiness, or parser-to-lowering contract transport.

## Required Invariants

1. Parser contract snapshot remains explicit and deterministic:
   - `Objc3ParserContractSnapshot` in `parse/objc3_parser_contract.h`.
   - Snapshot carries `deterministic_handoff` and `parser_recovery_replay_ready`.
   - Snapshot fingerprint mixes both booleans in `BuildObjc3ParserContractSnapshotFingerprint(...)`.
2. Parser execution emits contract snapshots through parser result:
   - `parse/objc3_parser.cpp` derives `deterministic_handoff`.
   - Parser result assigns `result.contract_snapshot = BuildObjc3ParserContractSnapshot(...)`.
3. Frontend pipeline transports parser snapshot without bypass:
   - `pipeline/objc3_frontend_pipeline.cpp` assigns `result.parser_contract_snapshot = parse_result.contract_snapshot`.
4. Parse/lowering readiness enforces parser replay gate:
   - `pipeline/objc3_parse_lowering_readiness_surface.h` consumes `parser_contract_snapshot`.
   - Readiness requires `parser_contract_deterministic` and `parser_recovery_replay_ready`.
5. Architecture anchor remains authoritative:
   - `native/objc3c/src/ARCHITECTURE.md` documents M250 frontend stability freeze boundary and replay-gate anchor.

## Validation

- `python scripts/check_m250_a001_frontend_stability_long_tail_grammar_closure_contract.py`
- `python -m pytest tests/tooling/test_check_m250_a001_frontend_stability_long_tail_grammar_closure_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m250/M250-A001/frontend_stability_long_tail_grammar_closure_contract_summary.json`
