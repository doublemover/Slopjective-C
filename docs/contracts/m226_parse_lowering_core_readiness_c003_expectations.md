# M226 Parse-Lowering Core Readiness Expectations (C003)

Contract ID: `objc3c-parse-lowering-core-readiness-contract/m226-c003-v1`
Status: Accepted
Scope: Parse artifact readiness gating for parse-to-lowering handoff in `native/objc3c/src/pipeline/*`.

## Objective

Make parse artifacts a first-class readiness gate before lowering by enforcing
snapshot-to-AST consistency and deterministic handoff projection in the
parse/lowering readiness surface.

## Required Invariants

1. Readiness surface carries parse artifact handoff fields:
   - `parse_artifact_handoff_consistent`
   - `parse_artifact_handoff_deterministic`
   - `parse_artifact_handoff_key`
2. `BuildObjc3ParseLoweringReadinessSurface(...)` computes parse artifact readiness by:
   - deriving parser snapshot declaration breakdown totals,
   - deriving parsed AST top-level declaration totals,
   - requiring snapshot/AST consistency for handoff consistency,
   - requiring parser diagnostic count alignment and parser deterministic flag for handoff determinism.
3. Parse snapshot readiness fails closed on parse artifact drift:
   - `ready_for_lowering` requires `parse_artifact_handoff_deterministic`.
   - deterministic failure reasons include parse artifact inconsistency and non-determinism conditions.
4. Artifact manifest projects parse artifact readiness evidence under
   `parse_lowering_readiness`:
   - `parse_artifact_handoff_consistent`
   - `parse_artifact_handoff_deterministic`
   - `parse_artifact_handoff_key`
5. Checker and tests remain fail-closed and emit summaries under `tmp/reports/m226/`.

## Validation

- `python scripts/check_m226_c003_parse_lowering_core_readiness_contract.py`
- `python -m pytest tests/tooling/test_check_m226_c003_parse_lowering_core_readiness_contract.py -q`

## Evidence Path

- `tmp/reports/m226/m226_c003_parse_lowering_core_readiness_contract_summary.json`
