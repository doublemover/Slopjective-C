# M227 Type-System Completeness for ObjC3 Forms Modular Split Expectations (B002)

Contract ID: `objc3c-type-system-objc3-forms-modular-split/m227-b002-v1`
Status: Accepted
Scope: `native/objc3c/src/sema/*` type-form scaffolding boundaries.

## Objective

Extract canonical ObjC form validation scaffolding into a dedicated sema module so future type-system shards can consume stable helpers without re-embedding local uniqueness/subset checks.

## Deterministic Invariants

1. Dedicated module exists:
   - `sema/objc3_type_form_scaffold.h`
   - `sema/objc3_type_form_scaffold.cpp`
2. Module exports explicit scaffold summary contract:
   - `Objc3TypeFormScaffoldSummary`
   - `BuildObjc3TypeFormScaffoldSummary(...)`
   - `IsReadyObjc3TypeFormScaffoldSummary(...)`
3. Scaffold summary checks canonical-set shape:
   - reference/message/bridge-top counts are non-zero
   - each canonical set is unique
   - bridge-top forms are a subset of canonical reference forms
4. Semantic pass type checks consume scaffold readiness gate before canonical compatibility checks.
5. Native build wiring includes scaffold source in sema source manifests (`native/objc3c/CMakeLists.txt`, `scripts/build_objc3c_native.ps1`).

## Validation

- `python scripts/check_m227_b002_type_system_objc3_forms_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m227_b002_type_system_objc3_forms_modular_split_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-B002/type_system_objc3_forms_modular_split_contract_summary.json`
