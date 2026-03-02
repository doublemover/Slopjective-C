# M226 Parser-to-Sema Core Feature Expansion Expectations (B004)

Contract ID: `objc3c-parser-sema-core-feature-expansion-contract/m226-b004-v1`
Status: Accepted
Scope: Expansion of parser->sema handoff integrity to include top-level layout fingerprints.

## Objective

Expand B003 core handoff guarantees with deterministic top-level layout
fingerprinting so sema intake can fail closed on ordering/layout drift.

## Required Invariants

1. Parser snapshot includes:
   - `ast_top_level_layout_fingerprint`
2. Parser contract includes:
   - `BuildObjc3ParsedProgramTopLevelLayoutFingerprint(...)`
3. Sema scaffold includes expected/match flags for top-level layout fingerprint.
4. Handoff determinism requires:
   - AST-shape fingerprint match
   - top-level layout fingerprint match
   - snapshot fingerprint match

## Validation

- `python scripts/check_m226_b004_parser_sema_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m226_b004_parser_sema_core_feature_expansion_contract.py -q`
- `npm run build:objc3c-native`

## Evidence Path

- `tmp/reports/m226/M226-B004/parser_sema_core_feature_expansion_summary.json`
