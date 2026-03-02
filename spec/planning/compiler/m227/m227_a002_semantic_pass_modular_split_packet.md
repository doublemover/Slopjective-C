# M227-A002 Semantic Pass Modular Split Packet

Packet: `M227-A002`
Milestone: `M227`
Lane: `A`

## Scope

Extract semantic pass-flow bookkeeping from `objc3_sema_pass_manager.cpp` into a dedicated scaffold module to preserve architecture boundaries as semantic/type-system behavior expands.

## Anchors

- Contract: `docs/contracts/m227_semantic_pass_modular_split_expectations.md`
- Checker: `scripts/check_m227_a002_semantic_pass_modular_split_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_a002_semantic_pass_modular_split_contract.py`
- New scaffold header: `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.h`
- New scaffold implementation: `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp`
- Sema manager integration: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- Sema target wiring: `native/objc3c/CMakeLists.txt`

## Required Evidence

- `tmp/reports/m227/M227-A002/semantic_pass_modular_split_contract_summary.json`

## Determinism Criteria

- Pass execution remains recorded through scaffold entry points.
- Final pass-flow summary remains deterministic and count-consistent.
- Split logic does not regress A001 architecture freeze invariants.
