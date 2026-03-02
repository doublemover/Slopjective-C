# M226 Parser Advanced Integration Workpack Expectations (A031)

Contract ID: `objc3c-parser-advanced-integration-workpack-contract/m226-a031-v1`
Status: Accepted
Scope: Parser advanced integration workpack shard 3 for native frontend decomposition and parser completeness.

## Objective

Integrate outputs from advanced parser packets A027-A030 into one fail-closed
integration summary artifact for deterministic lane-A closeout continuity.

## Required Invariants

1. Integration runner exists:
   - `scripts/run_m226_a031_parser_integration_workpack.ps1`
2. Runner consumes summary artifacts:
   - `tmp/reports/m226/M226-A027/parser_advanced_core_workpack_summary.json`
   - `tmp/reports/m226/M226-A028/parser_advanced_edge_compat_workpack_summary.json`
   - `tmp/reports/m226/M226-A029/parser_advanced_diagnostics_workpack_summary.json`
   - `tmp/reports/m226/M226-A030/parser_conformance_shard3_summary.json`
3. Runner fails closed if any upstream summary is missing or has `ok != true`.
4. Runner fails closed if any A030 fixture has `deterministic_match != true`.
5. Runner emits integrated summary:
   - `tmp/reports/m226/M226-A031/parser_integration_shard3_summary.json`
6. Validation entrypoints are pinned:
   - `python scripts/check_m226_a031_parser_advanced_integration_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a031_parser_advanced_integration_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a031_parser_advanced_integration_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a031_parser_advanced_integration_workpack_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m226_a031_parser_integration_workpack.ps1`

## Evidence Path

- `tmp/reports/m226/M226-A031/parser_integration_shard3_summary.json`

