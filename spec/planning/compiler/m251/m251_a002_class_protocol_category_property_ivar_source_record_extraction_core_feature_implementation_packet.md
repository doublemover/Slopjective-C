# M251-A002 Class Protocol Category Property Ivar Source-Record Extraction Packet

Packet: `M251-A002`
Milestone: `M251`
Lane: `A`
Implementation date: `2026-03-07`
Dependencies: `M251-A001`

## Purpose

Implement canonical source-record extraction for runtime-owned Objective-C
frontend declarations so class, protocol, category, property, method, and ivar
metadata can be handed to later runtime work without reparsing declaration
containers.

## Scope Anchors

- Contract:
  `docs/contracts/m251_class_protocol_category_property_ivar_source_record_extraction_core_feature_implementation_a002_expectations.md`
- Checker:
  `scripts/check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py`
- Reference fixtures:
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract`
  - `test:tooling:m251-a002-class-protocol-category-property-ivar-source-record-extraction-contract`
  - `check:objc3c:m251-a002-lane-a-readiness`
- Code anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ast/objc3_ast.h`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `docs/objc3c-native.md`

## Canonical Packet

- Canonical contract id:
  `objc3c-runtime-metadata-source-record-extraction/m251-a002-v1`
- Canonical packet type:
  `Objc3RuntimeMetadataSourceRecordSet`
- Canonical class record split:
  `record_kind == interface | implementation`
- Canonical category owner spelling:
  `Class(Category)`
- Canonical ivar source model:
  `property-synthesis-ivar-binding-symbols`

## Dynamic Probe Cases

The checker runs the frontend C API runner against two declaration fixtures:

1. `class-protocol-property-ivar`
   - validates class, protocol, property, method, and ivar source-record
     extraction surfaces.
2. `category-protocol-property`
   - validates category, protocol, property, method, and ivar source-record
     extraction surfaces without depending on later duplicate-identity policy.

Both probes must remain lex/parse/sema clean. Current emit-only `O3L300`
fail-closed behavior is allowed because `M251-A003` is responsible for
manifest-oriented projection through the downstream lowering boundary.

## Gate Commands

- `python scripts/check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py`
- `python -m pytest tests/tooling/test_check_m251_a002_class_protocol_category_property_ivar_source_record_extraction_contract.py -q`
- `npm run check:objc3c:m251-a002-lane-a-readiness`

## Evidence Output

- `tmp/reports/m251/M251-A002/runtime_metadata_source_record_extraction_contract_summary.json`
