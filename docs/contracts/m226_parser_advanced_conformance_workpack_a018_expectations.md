# M226 Parser Advanced Conformance Workpack Expectations (A018)

Contract ID: `objc3c-parser-advanced-conformance-workpack-contract/m226-a018-v1`
Status: Accepted
Scope: Parser advanced conformance workpack shard 1 for native frontend decomposition and parser completeness.

## Objective

Run deterministic parser-stage conformance checks across representative positive
native fixtures and fail closed if required parser manifest contract keys drift.

## Required Invariants

1. Conformance runner exists:
   - `scripts/run_m226_a018_parser_conformance_workpack.ps1`
2. Runner compiles these fixtures using native wrapper:
   - `hello.objc3`
   - `return_paths_ok.objc3`
   - `typed_i32_bool.objc3`
3. Runner validates manifest parser-stage keys for every fixture:
   - `diagnostic_code_count`
   - `diagnostic_code_fingerprint`
   - `diagnostic_code_surface_deterministic`
   - `interface_categories`
   - `implementation_categories`
   - `function_prototypes`
   - `function_pure`
4. Runner writes summary evidence under:
   - `tmp/reports/m226/M226-A018/parser_conformance_summary.json`
5. Validation entrypoints are pinned:
   - `python scripts/check_m226_a018_parser_advanced_conformance_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a018_parser_advanced_conformance_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a018_parser_advanced_conformance_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a018_parser_advanced_conformance_workpack_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m226_a018_parser_conformance_workpack.ps1`

## Evidence Path

- `tmp/reports/m226/M226-A018/parser_conformance_summary.json`
