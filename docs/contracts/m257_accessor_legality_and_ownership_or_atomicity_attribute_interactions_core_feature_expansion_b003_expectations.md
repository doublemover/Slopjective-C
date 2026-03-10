# M257 Accessor Legality and Ownership or Atomicity Attribute Interactions Core Feature Expansion Expectations (B003)

Contract ID: `objc3c-property-accessor-attribute-interactions/m257-b003-v1`
Status: Accepted
Issue: `#7149`
Scope: `M257` lane-B expansion of property accessor legality and ownership or atomicity attribute interactions.

## Objective

Fail closed on runtime-meaningful property accessor and attribute interactions that
still compiled after `M257-B002`: duplicate effective accessor selectors,
ownership modifiers on non-object properties, and atomic ownership-aware
properties.

## Required implementation

1. Effective getter selectors must be unique within each property container.
2. Effective setter selectors must be unique within each property container.
3. Runtime-managed ownership modifiers `copy`, `strong`, `weak`, and `unowned` must fail closed on non-object properties.
4. Atomic ownership-aware properties must fail closed until executable accessor storage semantics land.
5. Canonical proof fixtures must include:
   - `tests/tooling/fixtures/native/m257_accessor_attribute_interactions_positive.objc3`
   - `tests/tooling/fixtures/native/m257_accessor_duplicate_getter_negative.objc3`
   - `tests/tooling/fixtures/native/m257_accessor_duplicate_setter_negative.objc3`
   - `tests/tooling/fixtures/native/m257_property_scalar_ownership_negative.objc3`
   - `tests/tooling/fixtures/native/m257_property_atomic_ownership_negative.objc3`
6. `package.json` must wire:
   - `check:objc3c:m257-b003-accessor-legality-and-ownership-or-atomicity-attribute-interactions`
   - `test:tooling:m257-b003-accessor-legality-and-ownership-or-atomicity-attribute-interactions`
   - `check:objc3c:m257-b003-lane-b-readiness`
7. Validation evidence lands at `tmp/reports/m257/M257-B003/accessor_legality_attribute_interactions_summary.json`.

## Required diagnostics

- Duplicate effective getter selector => `O3S206`
- Duplicate effective setter selector => `O3S206`
- Ownership modifier on non-object property => `O3S206`
- Atomic ownership-aware property => `O3S206`

## Non-goals

- No synthesized accessor body emission yet.
- No runtime-backed atomic accessor synchronization yet.
- No property storage realization beyond sema fail-closed admission.

## Validation

- `python scripts/check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m257_b003_accessor_legality_and_ownership_or_atomicity_attribute_interactions_core_feature_expansion.py -q`
- `npm run check:objc3c:m257-b003-lane-b-readiness`

## Evidence Path

- `tmp/reports/m257/M257-B003/accessor_legality_attribute_interactions_summary.json`
