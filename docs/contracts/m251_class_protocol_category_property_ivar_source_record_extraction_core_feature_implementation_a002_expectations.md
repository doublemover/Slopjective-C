# M251 Class Protocol Category Property Ivar Source-Record Extraction Expectations (A002)

Contract ID: `objc3c-runtime-metadata-source-record-extraction/m251-a002-v1`
Status: Accepted
Scope: M251 lane-A core feature implementation for canonical runtime metadata source-record extraction.

## Objective

Promote class, protocol, category, property, method, and ivar source records into
one deterministic frontend-owned packet so later runtime metadata work consumes a
canonical handoff instead of count-only summaries.

## Required Invariants

1. `pipeline/objc3_frontend_types.h` defines `Objc3RuntimeMetadataSourceRecordSet`
   plus dedicated source-record structs for:
   - class records,
   - protocol records,
   - category records,
   - property records,
   - method records,
   - ivar records.
2. `pipeline/objc3_frontend_pipeline.cpp` synthesizes the record set through
   `BuildRuntimeMetadataSourceRecordSet(...)` and marks it deterministic only
   when every record vector is lexicographically sorted.
3. Category owner names are normalized through `BuildCategoryOwnerName(...)`
   as `Class(Category)`.
4. Property source records preserve getter, setter, type-name, and
   `ivar_binding_symbol` fields.
5. Method source records preserve selector, class-method bit, body bit,
   parameter count, and return-type name.
6. `pipeline/objc3_frontend_artifacts.cpp` projects runtime metadata source
   records into manifest payloads for:
   - `interfaces`,
   - `implementations`,
   - `protocols`,
   - `categories`,
   - `runtime_metadata_source_records.properties`,
   - `runtime_metadata_source_records.methods`,
   - `runtime_metadata_source_records.ivars`.
7. Dynamic validation uses `objc3c-frontend-c-api-runner.exe` with the A002
   reference fixtures and requires lex/parse/sema acceptance with zero
   frontend-stage diagnostics. Current emit-only `O3L300` failure is acceptable
   until `M251-A003` decouples manifest projection from downstream lowering
   readiness.

## Non-Goals and Fail-Closed Rules

- `M251-A002` does not emit native runtime metadata sections or runtime
  registration payloads.
- `M251-A002` does not remove the existing downstream parse-to-lowering
  readiness gate.
- `M251-A002` does not resolve duplicate-identity semantic policy for combined
  class-plus-category integration; that remains later lane-B work.
- The contract therefore fails closed when:
  - source-record structs drift,
  - deterministic sorting is removed,
  - manifest projection regresses back to count-only summaries,
  - frontend-stage fixture acceptance regresses before the downstream emit gate.

## Reference Fixtures

- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract`.
- `package.json` includes
  `test:tooling:m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract`.
- `package.json` includes `check:objc3c:m251-a002-lane-a-readiness`.

## Validation

- `python scripts/check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py`
- `python -m pytest tests/tooling/test_check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py -q`
- `npm run check:objc3c:m251-a002-lane-a-readiness`

## Evidence Path

- `tmp/reports/m251/M251-A002/runtime_metadata_source_record_extraction_contract_summary.json`
