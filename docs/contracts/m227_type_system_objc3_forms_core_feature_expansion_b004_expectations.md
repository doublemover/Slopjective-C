# M227 Type-System Completeness for ObjC3 Forms Core Feature Expansion Expectations (B004)

Contract ID: `objc3c-type-system-objc3-forms-core-feature-expansion/m227-b004-v1`
Status: Accepted
Scope: `Objc3IdClassSelObjectPointerTypeCheckingSummary` SEL form accounting across integration + handoff builders.

## Objective

Expand core type-form accounting so `SEL` form usage is counted deterministically in parameter/return/property sites instead of remaining implicitly zeroed.

## Deterministic Invariants

1. Integration-surface summary builder increments SEL counters:
   - `param_sel_spelling_sites`
   - `return_sel_spelling_sites`
   - `property_sel_spelling_sites`
2. Type-metadata-handoff summary builder increments SEL counters for the same site classes.
3. Existing parity and budget checks continue to enforce that SEL counts are bounded by corresponding total site counts.
4. Checker remains fail-closed for dropped SEL accounting in either builder path.

## Validation

- `python scripts/check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_b004_type_system_objc3_forms_core_feature_expansion_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-B004/type_system_objc3_forms_core_feature_expansion_contract_summary.json`
