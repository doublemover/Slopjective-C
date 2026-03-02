# M226 Parser Advanced Integration Workpack Expectations (A019)

Contract ID: `objc3c-parser-advanced-integration-workpack-contract/m226-a019-v1`
Status: Accepted
Scope: Parser advanced integration workpack shard 1 for native frontend decomposition and parser completeness.

## Objective

Integrate outputs from advanced parser packets A015-A018 into one fail-closed
integration summary artifact for deterministic lane-A closeout continuity.

## Required Invariants

1. Integration runner exists:
   - `scripts/run_m226_a019_parser_integration_workpack.ps1`
2. Runner consumes summary artifacts:
   - `tmp/reports/m226/M226-A015/parser_advanced_core_workpack_summary.json`
   - `tmp/reports/m226/M226-A016/parser_advanced_edge_compat_workpack_summary.json`
   - `tmp/reports/m226/M226-A017/parser_advanced_diagnostics_workpack_summary.json`
   - `tmp/reports/m226/M226-A018/parser_conformance_summary.json`
3. Runner fails closed if any upstream summary is missing or has `ok != true`.
4. Runner emits integrated summary:
   - `tmp/reports/m226/M226-A019/parser_integration_summary.json`
5. Validation entrypoints are pinned:
   - `python scripts/check_m226_a019_parser_advanced_integration_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a019_parser_advanced_integration_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a019_parser_advanced_integration_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a019_parser_advanced_integration_workpack_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m226_a019_parser_integration_workpack.ps1`

## Evidence Path

- `tmp/reports/m226/M226-A019/parser_integration_summary.json`
