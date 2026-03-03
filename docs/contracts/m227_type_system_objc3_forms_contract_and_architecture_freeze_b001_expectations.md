# M227 Type-System Completeness for ObjC3 Forms Contract and Architecture Freeze Expectations (B001)

Contract ID: `objc3c-type-system-objc3-forms-contract-and-architecture-freeze/m227-b001-v1`
Status: Accepted
Scope: M227 lane-B type-system completeness freeze for canonical ObjC forms across sema boundaries and shared architecture/spec/readiness anchors.

## Objective

Fail closed unless lane-B type-system completeness anchors for canonical ObjC
forms remain explicit, deterministic, and traceable across sema code surfaces,
shared architecture/spec anchors, and package readiness wiring.

## Dependency Scope

- Issue `#4842` defines canonical lane-B contract freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m227/m227_b001_type_system_objc3_forms_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m227_b001_type_system_objc3_forms_contract.py`
  - `tests/tooling/test_check_m227_b001_type_system_objc3_forms_contract.py`

## Sema Contract Anchors

- `native/objc3c/src/sema/objc3_sema_contract.h` must preserve explicit
  canonical ObjC form sets and helpers:
  - `kObjc3CanonicalReferenceTypeForms`
  - `kObjc3CanonicalScalarMessageSendTypeForms`
  - `kObjc3CanonicalBridgeTopReferenceTypeForms`
  - `IsObjc3CanonicalReferenceTypeForm(...)`
  - `IsObjc3CanonicalMessageSendTypeForm(...)`
  - `IsObjc3CanonicalBridgeTopReferenceTypeForm(...)`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp` must route assignment and
  message compatibility through canonical helper predicates and remain fail
  closed against ad-hoc literal compatibility checks.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-B B001
  contract and architecture freeze anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B type-system
  completeness governance wording with explicit `M227-B001` dependency anchor.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B
  type-system completeness metadata anchor wording for `M227-B001`.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m227-b001-type-system-objc3-forms-contract`.
- `package.json` includes `test:tooling:m227-b001-type-system-objc3-forms-contract`.
- `package.json` includes `check:objc3c:m227-b001-lane-b-readiness`.

## Milestone Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m227_b001_type_system_objc3_forms_contract.py`
- `python -m pytest tests/tooling/test_check_m227_b001_type_system_objc3_forms_contract.py -q`
- `npm run check:objc3c:m227-b001-lane-b-readiness`

## Evidence Path

- `tmp/reports/m227/M227-B001/type_system_objc3_forms_contract_and_architecture_freeze_summary.json`
