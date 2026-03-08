# M252-B004 Property Ivar Export Legality Synthesis Preconditions Packet

Packet: `M252-B004`
Milestone: `M252`
Lane: `B`

## Objective

Freeze the property and ivar export legality preconditions so property synthesis
and ivar-binding readiness are published from one canonical sema summary and
negative source-shape failures remain deterministic before lowering handoff.

## Dependencies

- `M252-B003`

## Required anchors

- `docs/contracts/m252_property_ivar_export_legality_synthesis_preconditions_b004_expectations.md`
- `tests/tooling/fixtures/native/m252_b004_class_property_synthesis_ready.objc3`
- `tests/tooling/fixtures/native/m252_b004_category_property_export_only.objc3`
- `tests/tooling/fixtures/native/m252_b004_missing_interface_property.objc3`
- `tests/tooling/fixtures/native/m252_b004_incompatible_property_signature.objc3`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json`

## Acceptance

- Class implementation properties publish canonical property-synthesis/ivar
  binding counts from sema and derive their replay key from the same counts.
- Category-only property export keeps the property-synthesis/ivar binding counts
  at zero.
- Missing interface property declarations fail with `O3S206`.
- Incompatible implementation property signatures fail with `O3S206`.
- Deterministic checker, pytest coverage, and lane-B readiness exist.
- Evidence lands under `tmp/reports/m252/M252-B004/`.

## Commands

- `python scripts/check_m252_b004_property_ivar_export_legality_synthesis_preconditions.py`
- `python -m pytest tests/tooling/test_check_m252_b004_property_ivar_export_legality_synthesis_preconditions.py -q`
- `npm run check:objc3c:m252-b004-lane-b-readiness`
