# M227-B002 Type-System Completeness for ObjC3 Forms Modular Split Packet

Packet: `M227-B002`  
Milestone: `M227`  
Lane: `B`

## Scope

Extract canonical ObjC form verification into a dedicated sema scaffold module and require sema + build manifests to consume that module as the single modular split anchor.

## Anchors

- Contract: `docs/contracts/m227_type_system_objc3_forms_modular_split_expectations.md`
- Checker: `scripts/check_m227_b002_type_system_objc3_forms_modular_split_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_b002_type_system_objc3_forms_modular_split_contract.py`
- Scaffold header/source:
  - `native/objc3c/src/sema/objc3_type_form_scaffold.h`
  - `native/objc3c/src/sema/objc3_type_form_scaffold.cpp`
- Sema consumer:
  - `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- Build anchors:
  - `native/objc3c/CMakeLists.txt`
  - `scripts/build_objc3c_native.ps1`

## Required Evidence

- `tmp/reports/m227/M227-B002/type_system_objc3_forms_modular_split_contract_summary.json`

## Determinism Criteria

- Scaffold summary remains ready (non-empty, unique canonical sets, bridge-top subset).
- Sema checks gate canonical type-form compatibility through scaffold readiness.
- Both CMake and native build script include scaffold source as sema module input.
