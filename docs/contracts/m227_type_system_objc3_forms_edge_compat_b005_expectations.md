# M227 Type-System Completeness for ObjC3 Forms Edge Compatibility Expectations (B005)

Contract ID: `objc3c-type-system-objc3-forms-edge-compat/m227-b005-v1`
Status: Accepted
Scope: canonical type-form scaffold edge-case compatibility guarantees.

## Objective

Harden edge compatibility for canonical ObjC form scaffolding so message scalar forms stay disjoint from reference forms and bridge-top forms explicitly exclude `SEL`.

## Deterministic Invariants

1. `Objc3TypeFormScaffoldSummary` captures:
   - `canonical_message_scalars_disjoint_from_reference`
   - `canonical_bridge_top_excludes_sel`
2. Scaffold builder computes both invariants from canonical form constants.
3. Scaffold readiness requires both invariants to hold.
4. Semantic pass readiness gate continues to rely on scaffold readiness.

## Validation

- `python scripts/check_m227_b005_type_system_objc3_forms_edge_compat_contract.py`
- `python -m pytest tests/tooling/test_check_m227_b005_type_system_objc3_forms_edge_compat_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-B005/type_system_objc3_forms_edge_compat_contract_summary.json`
