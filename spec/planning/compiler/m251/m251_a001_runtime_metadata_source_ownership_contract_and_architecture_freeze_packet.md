# M251-A001 Runtime Metadata Source Ownership Contract and Architecture Freeze Packet

Packet: `M251-A001`
Milestone: `M251`
Lane: `A`
Freeze date: `2026-03-07`
Dependencies: none

## Purpose

Freeze the canonical frontend-owned runtime metadata source boundary so class,
protocol, category, property, method, and ivar source records have one
explicit ownership packet before extraction, lowering, object emission, and
runtime registration work begin.

## Scope Anchors

- Contract:
  `docs/contracts/m251_runtime_metadata_source_ownership_contract_and_architecture_freeze_a001_expectations.md`
- Checker:
  `scripts/check_m251_a001_runtime_metadata_source_ownership_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m251_a001_runtime_metadata_source_ownership_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m251-a001-runtime-metadata-source-ownership-contract`
  - `test:tooling:m251-a001-runtime-metadata-source-ownership-contract`
  - `check:objc3c:m251-a001-lane-a-readiness`
- Code anchors:
  - `native/objc3c/src/ast/objc3_ast.h`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `docs/objc3c-native.md`

## Fail-Closed Boundary

- Contract ID: `objc3c-runtime-metadata-source-ownership-freeze/m251-a001-v1`
- Canonical source schema: `objc3-runtime-metadata-source-boundary-v1`
- Source ownership remains frontend-owned.
- Runtime metadata source records remain not ready for lowering.
- Native runtime library remains absent.
- The deterministic `objc3_msgsend_i32` shim remains test-only evidence.
- Ivar source ownership is explicitly modeled as
  `property-synthesis-ivar-binding-symbols` until native ivar declarations are
  implemented.

## Gate Commands

- `python scripts/check_m251_a001_runtime_metadata_source_ownership_contract.py`
- `python -m pytest tests/tooling/test_check_m251_a001_runtime_metadata_source_ownership_contract.py -q`
- `npm run check:objc3c:m251-a001-lane-a-readiness`

## Evidence Output

- `tmp/reports/m251/M251-A001/runtime_metadata_source_ownership_contract_summary.json`
