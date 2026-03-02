# M226 Parser Advanced Diagnostics Workpack Expectations (A023)

Contract ID: `objc3c-parser-advanced-diagnostics-workpack-contract/m226-a023-v1`
Status: Accepted
Scope: Parser advanced diagnostics workpack shard 2 for native frontend decomposition and parser completeness.

## Objective

Harden parser error-diagnostics recovery profile normalization with overflow-safe
count arithmetic so deterministic fail-closed behavior is preserved under
extreme diagnostic-site cardinalities.

## Required Invariants

1. Parser recovery profile arithmetic uses overflow-safe helper:
   - `TryAddErrorDiagnosticsRecoverySiteCounts(...)`
2. Normalization checks avoid raw unchecked addition for:
   - `normalized_sites + gate_blocked_sites`
3. Recovery profile builder uses overflow-safe addition for:
   - `error_diagnostics_recovery_sites` accumulation
   - `blocked_total` accumulation
4. Contract remains fail-closed when arithmetic overflow is detected.
5. Validation entrypoints are pinned:
   - `python scripts/check_m226_a023_parser_advanced_diagnostics_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a023_parser_advanced_diagnostics_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a023_parser_advanced_diagnostics_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a023_parser_advanced_diagnostics_workpack_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A023/parser_advanced_diagnostics_workpack_summary.json`
