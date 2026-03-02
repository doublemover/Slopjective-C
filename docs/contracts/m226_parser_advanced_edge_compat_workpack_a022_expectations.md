# M226 Parser Advanced Edge Compatibility Workpack Expectations (A022)

Contract ID: `objc3c-parser-advanced-edge-compat-workpack-contract/m226-a022-v1`
Status: Accepted
Scope: Parser advanced edge-compatibility workpack shard 2 for native frontend decomposition and parser completeness.

## Objective

Harden legacy compatibility normalization for A021 advanced ObjC member counters
by detecting and recovering both missing and over-reported values in
parser->sema handoff snapshots.

## Required Invariants

1. Compatibility edge-case detection covers advanced ObjC member counters when
   snapshot values are missing or over-reported versus AST-derived counts:
   - `protocol_property_decl_count`
   - `protocol_method_decl_count`
   - `interface_property_decl_count`
   - `interface_method_decl_count`
   - `implementation_property_decl_count`
   - `implementation_method_decl_count`
2. Legacy normalization (`Objc3SemaCompatibilityMode::Legacy`) repairs the
   above counters both when values are `0` and when values exceed AST-derived
   totals.
3. Canonical mode remains fail-closed and does not apply compatibility
   normalization.
4. Validation entrypoints are pinned:
   - `python scripts/check_m226_a022_parser_advanced_edge_compat_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a022_parser_advanced_edge_compat_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a022_parser_advanced_edge_compat_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a022_parser_advanced_edge_compat_workpack_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A022/parser_advanced_edge_compat_workpack_summary.json`
