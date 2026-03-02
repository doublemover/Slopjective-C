# M227 Type-System Completeness for ObjC3 Forms Edge Robustness Expectations (B006)

Contract ID: `objc3c-type-system-objc3-forms-edge-robustness/m227-b006-v1`
Status: Accepted
Scope: canonical ObjC3 type-form scaffold edge-robustness guardrails.

## Objective

Expand edge robustness for canonical ObjC3 form scaffolding so malformed canonical sets fail closed when critical anchor forms drift, unknown forms appear, or bridge-top/reference alignment diverges.

## Deterministic Invariants

1. `Objc3TypeFormScaffoldSummary` captures robustness fields:
   - `canonical_reference_includes_sel`
   - `canonical_message_scalars_include_i32`
   - `canonical_message_scalars_include_bool`
   - `canonical_forms_exclude_unknown`
   - `canonical_bridge_top_matches_reference_without_sel`
2. Scaffold builder computes robustness fields directly from canonical constants:
   - `ObjCSel` must remain present in `kObjc3CanonicalReferenceTypeForms`.
   - `I32` and `Bool` must remain present in `kObjc3CanonicalScalarMessageSendTypeForms`.
   - `ValueType::Unknown` must be excluded from canonical reference/message-scalar/bridge-top sets.
   - Bridge-top forms must exactly match reference forms with `ObjCSel` removed.
3. Scaffold determinism and readiness require every robustness invariant in addition to prior B005 checks.
4. Semantic pass readiness gate continues to rely on `IsCanonicalObjc3TypeFormScaffoldReady(...)`.

## Validation

- `python scripts/check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-B006/type_system_objc3_forms_edge_robustness_contract_summary.json`
