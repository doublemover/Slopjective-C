# M226 Parser Advanced Performance Workpack Expectations (A020)

Contract ID: `objc3c-parser-advanced-performance-workpack-contract/m226-a020-v1`
Status: Accepted
Scope: Parser advanced performance workpack shard 1 for native frontend decomposition and parser completeness.

## Objective

Capture deterministic non-gating parser compile-time baselines for representative
positive fixtures and publish them as packet evidence for future regression
tracking.

## Required Invariants

1. Performance runner exists:
   - `scripts/run_m226_a020_parser_performance_workpack.ps1`
2. Runner compiles these fixtures through native wrapper:
   - `hello.objc3`
   - `return_paths_ok.objc3`
   - `typed_i32_bool.objc3`
3. Runner records per-fixture elapsed milliseconds and emits summary:
   - `tmp/reports/m226/M226-A020/parser_performance_summary.json`
4. Packet is explicitly non-gating:
   - no hard performance thresholds enforced
5. Validation entrypoints are pinned:
   - `python scripts/check_m226_a020_parser_advanced_performance_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a020_parser_advanced_performance_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a020_parser_advanced_performance_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a020_parser_advanced_performance_workpack_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m226_a020_parser_performance_workpack.ps1`

## Evidence Path

- `tmp/reports/m226/M226-A020/parser_performance_summary.json`
