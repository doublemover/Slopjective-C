# M226 Parse-Lowering Readiness Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-parse-lowering-readiness-core-feature-expansion-contract/m226-c004-v1`
Status: Accepted
Scope: Parse artifact determinism expansion in `native/objc3c/src/pipeline/*`.

## Objective

Expand parse-to-lowering readiness so parse artifact replay evidence is derived
from parser snapshot fingerprints and AST shape fingerprints, then enforced
fail-closed before lowering proceeds.

## Required Invariants

1. Readiness surface tracks parse artifact fingerprint evidence:
   - `parser_contract_snapshot_fingerprint`
   - `parser_ast_shape_fingerprint`
   - `ast_shape_fingerprint`
   - `parse_artifact_fingerprint_consistent`
   - `parse_artifact_replay_key_deterministic`
   - `parse_artifact_replay_key`
2. `BuildObjc3ParseLoweringReadinessSurface(...)` computes parse artifact replay
   readiness by:
   - deriving parser snapshot fingerprint,
   - deriving parsed AST shape fingerprint,
   - requiring parser/AST fingerprint consistency,
   - requiring replay-key determinism from handoff determinism + fingerprint consistency.
3. Parse snapshot readiness fails closed on replay-key drift:
   - readiness requires `parse_artifact_replay_key_deterministic`.
   - deterministic failure reasons include fingerprint inconsistency and replay-key non-determinism.
4. Artifact manifest projects parse artifact replay evidence under
   `parse_lowering_readiness`:
   - `parse_artifact_fingerprint_consistent`
   - `parse_artifact_replay_key_deterministic`
   - `parser_contract_snapshot_fingerprint`
   - `parser_ast_shape_fingerprint`
   - `ast_shape_fingerprint`
   - `parse_artifact_replay_key`
5. Checker and tests remain fail-closed and emit summaries under `tmp/reports/m226/`.

## Validation

- `python scripts/check_m226_c004_parse_lowering_readiness_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c004_parse_lowering_readiness_core_feature_expansion_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_c004_parse_lowering_readiness_core_feature_expansion_contract_summary.json`
