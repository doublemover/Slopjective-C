# M227-B005 Type-System Completeness for ObjC3 Forms Edge Compatibility Packet

Packet: `M227-B005`  
Milestone: `M227`  
Lane: `B`

## Scope

Harden canonical type-form scaffold edge compatibility by enforcing scalar/reference disjointness and explicit exclusion of `SEL` from bridge-top reference forms.

## Anchors

- Contract: `docs/contracts/m227_type_system_objc3_forms_edge_compat_b005_expectations.md`
- Checker: `scripts/check_m227_b005_type_system_objc3_forms_edge_compat_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_b005_type_system_objc3_forms_edge_compat_contract.py`
- Scaffold module:
  - `native/objc3c/src/sema/objc3_type_form_scaffold.h`
  - `native/objc3c/src/sema/objc3_type_form_scaffold.cpp`
- Sema readiness consumer: `native/objc3c/src/sema/objc3_semantic_passes.cpp`

## Required Evidence

- `tmp/reports/m227/M227-B005/type_system_objc3_forms_edge_compat_contract_summary.json`

## Determinism Criteria

- Message scalar canonical forms remain disjoint from canonical reference forms.
- Bridge-top canonical forms remain a subset of canonical reference forms and explicitly exclude `SEL`.
- Scaffold readiness requires all edge-compat invariants.
