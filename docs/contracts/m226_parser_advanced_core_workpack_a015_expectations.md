# M226 Parser Advanced Core Workpack Expectations (A015)

Contract ID: `objc3c-parser-advanced-core-workpack-contract/m226-a015-v1`
Status: Accepted
Scope: Parser advanced core workpack shard 1 for native frontend decomposition and parser completeness.

## Objective

Expand parser contract coverage with deterministic category/prototype/pure
function counters that survive parser->sema handoff and emit through manifest
parser stage telemetry.

## Required Invariants

1. Parser contract snapshot includes advanced counters:
   - `interface_category_decl_count`
   - `implementation_category_decl_count`
   - `function_prototype_count`
   - `function_pure_count`
2. Snapshot fingerprint includes these advanced counters.
3. Snapshot builder computes these counters from parser AST.
4. Legacy compatibility normalization can recover missing advanced counters
   from AST in sema handoff.
5. Manifest parser stage includes these counters:
   - `interface_categories`
   - `implementation_categories`
   - `function_prototypes`
   - `function_pure`
6. Validation entrypoints are pinned:
   - `python scripts/check_m226_a015_parser_advanced_core_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a015_parser_advanced_core_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a015_parser_advanced_core_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a015_parser_advanced_core_workpack_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/reports/m226/M226-A015/smoke_out --emit-prefix module`

## Evidence Path

- `tmp/reports/m226/M226-A015/parser_advanced_core_workpack_summary.json`
