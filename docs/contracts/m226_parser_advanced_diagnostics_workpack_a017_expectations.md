# M226 Parser Advanced Diagnostics Workpack Expectations (A017)

Contract ID: `objc3c-parser-advanced-diagnostics-workpack-contract/m226-a017-v1`
Status: Accepted
Scope: Parser advanced diagnostics workpack shard 1 for native frontend decomposition and parser completeness.

## Objective

Expand parser diagnostics telemetry by emitting deterministic parser diagnostic
code coverage metrics in the manifest parser stage.

## Required Invariants

1. Frontend artifacts define parser diagnostic coverage surface:
   - `Objc3ParserDiagnosticCodeCoverage`
   - `BuildObjc3ParserDiagnosticCodeCoverage`
   - `TryExtractDiagnosticCode`
2. Manifest parser stage emits:
   - `diagnostic_code_count`
   - `diagnostic_code_fingerprint`
   - `diagnostic_code_surface_deterministic`
3. Diagnostic-code fingerprint mixes sorted unique diagnostic codes.
4. Validation entrypoints are pinned:
   - `python scripts/check_m226_a017_parser_advanced_diagnostics_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a017_parser_advanced_diagnostics_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a017_parser_advanced_diagnostics_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a017_parser_advanced_diagnostics_workpack_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/reports/m226/M226-A017/smoke_out --emit-prefix module`

## Evidence Path

- `tmp/reports/m226/M226-A017/parser_advanced_diagnostics_workpack_summary.json`
