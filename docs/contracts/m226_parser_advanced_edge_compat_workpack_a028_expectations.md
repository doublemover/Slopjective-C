# M226 Parser Advanced Edge Compatibility Workpack Expectations (A028)

Contract ID: `objc3c-parser-advanced-edge-compat-workpack-contract/m226-a028-v1`
Status: Accepted
Scope: Parser advanced edge-compatibility workpack shard 3 for native frontend decomposition and parser completeness.

## Objective

Harden legacy compatibility normalization for A027 class/instance method
counters by detecting and recovering missing/over-reported bucketed values while
keeping canonical mode fail-closed.

## Required Invariants

1. Compatibility edge-case detection covers shard-3 method buckets:
   - `protocol_class_method_decl_count`
   - `protocol_instance_method_decl_count`
   - `interface_class_method_decl_count`
   - `interface_instance_method_decl_count`
   - `implementation_class_method_decl_count`
   - `implementation_instance_method_decl_count`
2. Legacy normalization (`Objc3SemaCompatibilityMode::Legacy`) repairs the
   above counters for zero/missing and over-reported values.
3. Legacy normalization repairs method-bucket consistency when
   class + instance != total method count.
4. Canonical mode remains fail-closed and does not apply compatibility
   normalization.
5. Validation entrypoints are pinned:
   - `python scripts/check_m226_a028_parser_advanced_edge_compat_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a028_parser_advanced_edge_compat_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a028_parser_advanced_edge_compat_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a028_parser_advanced_edge_compat_workpack_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_parser_contract_sema_integration.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A028/parser_advanced_edge_compat_workpack_summary.json`
