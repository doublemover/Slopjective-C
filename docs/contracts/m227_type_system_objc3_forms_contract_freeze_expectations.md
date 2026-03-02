# M227 Type-System Completeness for ObjC3 Forms Contract Freeze Expectations (B001)

Contract ID: `objc3c-type-system-objc3-forms-contract-freeze/m227-b001-v1`
Status: Accepted
Scope: `native/objc3c/src/sema/*` and architecture contract anchors.

## Objective

Freeze canonical ObjC type-form classification rules so semantic checking remains deterministic while M227 type-system expansion shards land.

## Deterministic Invariants

1. Canonical ObjC reference forms are explicitly anchored in `sema/objc3_sema_contract.h`:
   - `kObjc3CanonicalReferenceTypeForms`
   - `IsObjc3CanonicalReferenceTypeForm(...)`
2. Canonical ObjC message-send-compatible forms are explicitly anchored:
   - `kObjc3CanonicalScalarMessageSendTypeForms`
   - `IsObjc3CanonicalMessageSendTypeForm(...)`
3. Canonical ObjC bridge-top assignment compatibility forms are explicitly anchored:
   - `kObjc3CanonicalBridgeTopReferenceTypeForms`
   - `IsObjc3CanonicalBridgeTopReferenceTypeForm(...)`
4. Semantic pass type compatibility logic consumes canonical helper predicates instead of duplicated ad-hoc literals:
   - `IsObjCReferenceValueType(...)` delegates to `IsObjc3CanonicalReferenceTypeForm(...)`.
   - `IsMessageCompatibleType(...)` delegates to `IsObjc3CanonicalMessageSendTypeForm(...)`.
   - `AreObjCReferenceTypesAssignmentCompatible(...)` delegates to `IsObjc3CanonicalBridgeTopReferenceTypeForm(...)`.
5. Architecture contract text records this boundary explicitly in `native/objc3c/src/ARCHITECTURE.md`.

## Validation

- `python scripts/check_m227_b001_type_system_objc3_forms_contract.py`
- `python -m pytest tests/tooling/test_check_m227_b001_type_system_objc3_forms_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-B001/type_system_objc3_forms_contract_summary.json`
