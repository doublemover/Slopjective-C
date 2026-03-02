# M227-B006 Type-System Completeness for ObjC3 Forms Edge Robustness Packet

Packet: `M227-B006`  
Milestone: `M227`  
Lane: `B`

## Scope

Expand canonical ObjC3 type-form scaffold robustness with explicit edge guardrails for anchor-form presence, unknown-form exclusion, and exact bridge-top/reference-minus-SEL alignment.

## Anchors

- Contract: `docs/contracts/m227_type_system_objc3_forms_edge_robustness_b006_expectations.md`
- Checker: `scripts/check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py`
- Scaffold module:
  - `native/objc3c/src/sema/objc3_type_form_scaffold.h`
  - `native/objc3c/src/sema/objc3_type_form_scaffold.cpp`
- Sema readiness consumer: `native/objc3c/src/sema/objc3_semantic_passes.cpp`

## Required Evidence

- `tmp/reports/m227/M227-B006/type_system_objc3_forms_edge_robustness_contract_summary.json`

## Determinism Criteria

- Canonical reference forms include `SEL`.
- Canonical message-scalar forms include both `I32` and `Bool`.
- Canonical reference/message-scalar/bridge-top sets exclude `Unknown`.
- Bridge-top canonical set exactly equals canonical reference set with `SEL` removed.
- Scaffold readiness and determinism fail closed when any robustness guardrail drifts.
