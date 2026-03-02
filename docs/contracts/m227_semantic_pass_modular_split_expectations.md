# M227 Semantic Pass Modular Split and Scaffolding Expectations (A002)

Contract ID: `objc3c-sema-pass-modular-split-scaffold/m227-a002-v1`
Status: Accepted
Scope: semantic pass-flow scaffolding module and sema pass manager integration points.

## Objective

Split semantic pass-flow bookkeeping into a dedicated scaffold module so future semantic/type-system work can evolve pass logic without re-monolithizing `objc3_sema_pass_manager.cpp`.

## Required Invariants

1. Dedicated scaffold module exists:
   - `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.h`
   - `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp`
2. Sema pass manager consumes scaffold API:
   - Includes `sema/objc3_sema_pass_flow_scaffold.h`.
   - Uses `MarkObjc3SemaPassExecuted(...)`.
   - Uses `FinalizeObjc3SemaPassFlowSummary(...)`.
3. Sema target graph wires scaffold compilation unit:
   - `native/objc3c/CMakeLists.txt` includes `src/sema/objc3_sema_pass_flow_scaffold.cpp` in `objc3c_sema`.
4. Contract and architecture freeze surfaces remain authoritative:
   - `docs/contracts/m227_semantic_pass_decomposition_expectations.md` still references `Objc3SemaPassFlowSummary`.
   - `native/objc3c/src/ARCHITECTURE.md` retains M227 sema boundary language.

## Validation

- `python scripts/check_m227_a002_semantic_pass_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a002_semantic_pass_modular_split_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-A002/semantic_pass_modular_split_contract_summary.json`
