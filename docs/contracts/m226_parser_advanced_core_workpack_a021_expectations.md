# M226 Parser Advanced Core Workpack Expectations (A021)

Contract ID: `objc3c-parser-advanced-core-workpack-contract/m226-a021-v1`
Status: Accepted
Scope: Parser advanced core workpack shard 2 for native frontend decomposition and parser completeness.

## Objective

Expand parser contract completeness with deterministic Objective-C declaration
member counters (protocol/interface/implementation property and method totals)
that survive parser->sema handoff and emit through parser-stage manifest telemetry.

## Required Invariants

1. Parser contract snapshot includes advanced ObjC member counters:
   - `protocol_property_decl_count`
   - `protocol_method_decl_count`
   - `interface_property_decl_count`
   - `interface_method_decl_count`
   - `implementation_property_decl_count`
   - `implementation_method_decl_count`
2. Snapshot fingerprint includes these advanced counters.
3. Snapshot builder computes counters as aggregate declaration totals from parser AST.
4. Legacy compatibility normalization can recover missing advanced counters from
   AST in sema handoff.
5. Snapshot consistency validation requires parser snapshot values to match
   aggregate member counts rebuilt from AST.
6. Validation entrypoints are pinned:
   - `python scripts/check_m226_a021_parser_advanced_core_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a021_parser_advanced_core_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a021_parser_advanced_core_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a021_parser_advanced_core_workpack_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A021/parser_advanced_core_workpack_summary.json`
