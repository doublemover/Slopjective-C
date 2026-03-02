# M226 Parser Advanced Performance Workpack Expectations (A026)

Contract ID: `objc3c-parser-advanced-performance-workpack-contract/m226-a026-v1`
Status: Accepted
Scope: Parser advanced performance workpack shard 2 for native frontend decomposition and parser completeness.

## Objective

Capture deterministic non-gating parser compile-time baselines for expanded
positive fixtures with two-pass drift accounting and publish them as packet
evidence for regression tracking.

## Required Invariants

1. Performance runner exists:
   - `scripts/run_m226_a026_parser_performance_workpack.ps1`
2. Runner compiles these fixtures through native wrapper:
   - `hello.objc3`
   - `return_paths_ok.objc3`
   - `typed_i32_bool.objc3`
   - `comparison_logic.objc3`
3. Runner executes two passes per fixture and records:
   - per-pass elapsed milliseconds
   - fixture average elapsed milliseconds
   - fixture pass drift milliseconds
4. Runner emits summary:
   - `tmp/reports/m226/M226-A026/parser_performance_shard2_summary.json`
5. Packet is explicitly non-gating:
   - no hard performance thresholds enforced
6. Validation entrypoints are pinned:
   - `python scripts/check_m226_a026_parser_advanced_performance_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a026_parser_advanced_performance_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a026_parser_advanced_performance_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a026_parser_advanced_performance_workpack_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m226_a026_parser_performance_workpack.ps1`

## Evidence Path

- `tmp/reports/m226/M226-A026/parser_performance_shard2_summary.json`
