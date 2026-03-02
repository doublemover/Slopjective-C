# M227 Type-System Completeness for ObjC3 Forms Core Feature Expectations (B003)

Contract ID: `objc3c-type-system-objc3-forms-core-feature/m227-b003-v1`
Status: Accepted
Scope: semantic type-check summary surfaces and parity comparison.

## Objective

Implement core feature wiring so canonical ObjC type-form scaffold metrics become first-class fields in `Objc3IdClassSelObjectPointerTypeCheckingSummary` and are enforced in sema parity checks.

## Deterministic Invariants

1. `Objc3IdClassSelObjectPointerTypeCheckingSummary` carries canonical scaffold counters/flags:
   - canonical reference/message-scalar/bridge-top form counts
   - uniqueness flags
   - bridge-top subset flag
   - scaffold readiness flag
2. Both integration-surface and type-metadata-handoff summary builders apply scaffold metrics via the shared scaffold helper.
   - `ApplyTypeFormScaffoldSummaryToIdClassSelObjectPointerTypeCheckingSummary(...)`
3. Summary determinism logic requires canonical scaffold metrics to match canonical form constants and be ready.
4. Sema parity checks compare canonical scaffold metrics between handoff and integration summaries and fail closed on drift.

## Validation

- `python scripts/check_m227_b003_type_system_objc3_forms_core_feature_contract.py`
- `python -m pytest tests/tooling/test_check_m227_b003_type_system_objc3_forms_core_feature_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-B003/type_system_objc3_forms_core_feature_contract_summary.json`
