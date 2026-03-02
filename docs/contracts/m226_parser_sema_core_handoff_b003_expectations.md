# M226 Parser-to-Sema Core Handoff Expectations (B003)

Contract ID: `objc3c-parser-sema-core-handoff-contract/m226-b003-v1`
Status: Accepted
Scope: Core parser-to-sema handoff integrity for AST-shape/fingerprint consistency.

## Objective

Promote parser->sema handoff from structural-count-only checks to core integrity
checks that include AST-shape fingerprinting and snapshot fingerprint matching.

## Required Invariants

1. Parser contract snapshot includes `ast_shape_fingerprint`.
2. Parser contract helper APIs exist:
   - `BuildObjc3ParsedProgramAstShapeFingerprint(...)`
   - `BuildObjc3ParserContractSnapshotFingerprint(...)`
3. Sema handoff scaffold validates:
   - AST-shape fingerprint equality
   - Snapshot fingerprint equality
4. Handoff determinism remains fail-closed:
   - `scaffold.deterministic = scaffold.parser_contract_snapshot_matches_program;`

## Validation

- `python scripts/check_m226_b003_parser_sema_core_handoff_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b003_parser_sema_core_handoff_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-B003/parser_sema_core_handoff_summary.json`
