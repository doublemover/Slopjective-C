# M226 Parser Advanced Core Workpack Expectations (A027)

Contract ID: `objc3c-parser-advanced-core-workpack-contract/m226-a027-v1`
Status: Accepted
Scope: Parser advanced core workpack shard 3 for native frontend decomposition and parser completeness.

## Objective

Expand parser contract completeness with deterministic ObjC class/instance
method counters (protocol/interface/implementation) that survive parser->sema
handoff and participate in fail-closed conformance matrix gating.

## Required Invariants

1. Parser contract snapshot includes shard-3 method counters:
   - `protocol_class_method_decl_count`
   - `protocol_instance_method_decl_count`
   - `interface_class_method_decl_count`
   - `interface_instance_method_decl_count`
   - `implementation_class_method_decl_count`
   - `implementation_instance_method_decl_count`
2. Snapshot fingerprint includes these shard-3 counters.
3. Snapshot builder computes these counters from parser AST method declarations.
4. Legacy compatibility normalization can recover missing shard-3 counters from
   AST in sema handoff.
5. Snapshot consistency validation requires parser snapshot values to match
   aggregate class/instance method counts rebuilt from AST.
6. Sema parity readiness remains fail-closed and requires class/instance method
   match flags.
7. Validation entrypoints are pinned:
   - `python scripts/check_m226_a027_parser_advanced_core_workpack_contract.py`
   - `python -m pytest tests/tooling/test_check_m226_a027_parser_advanced_core_workpack_contract.py -q`

## Validation

- `python scripts/check_m226_a027_parser_advanced_core_workpack_contract.py`
- `python -m pytest tests/tooling/test_check_m226_a027_parser_advanced_core_workpack_contract.py -q`

## Evidence Path

- `tmp/reports/m226/M226-A027/parser_advanced_core_workpack_summary.json`
