# M251-B001 Object-Model ABI Invariants and Legality Packet

Packet: `M251-B001`
Milestone: `M251`
Lane: `B`
Implementation date: `2026-03-07`
Dependencies: none

## Purpose

Freeze the lane-B semantic boundary that future runtime metadata export
validation must preserve before native metadata section emission and runtime
registration are implemented.

## Scope Anchors

- Contract:
  `docs/contracts/m251_object_model_abi_invariants_and_legality_contract_and_architecture_freeze_b001_expectations.md`
- Checker:
  `scripts/check_m251_b001_object_model_abi_invariants_and_legality_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m251_b001_object_model_abi_invariants_and_legality_contract.py`
- Reference fixtures:
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
  - `tests/tooling/fixtures/native/hello.objc3`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m251-b001-object-model-abi-invariants-and-legality-contract`
  - `test:tooling:m251-b001-object-model-abi-invariants-and-legality-contract`
  - `check:objc3c:m251-b001-lane-b-readiness`
- Code anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `docs/objc3c-native.md`

## Canonical Behavior

- Frontend/sema owns a single runtime-export legality packet:
  `Objc3RuntimeExportLegalityBoundary`.
- The packet is emitted into the manifest and the LLVM IR frontend metadata.
- The packet is allowed to be `ready=true` while export enforcement remains
  pending, as long as the packet is explicitly fail-closed and marks those
  enforcement lanes pending.
- Duplicate-identity blocking, incomplete-declaration blocking, and illegal
  redeclaration blocking remain deferred to `M251-B002`.

## Dynamic Probe Cases

The checker runs three deterministic probes:

1. `c_api_manifest_only_class`
   - `objc3c-frontend-c-api-runner.exe`
   - class/protocol/property/ivar fixture
   - expects success and a legality packet with `frozen=true`,
     `fail_closed=true`, `ready=true`.
2. `c_api_manifest_only_category`
   - `objc3c-frontend-c-api-runner.exe`
   - category/protocol/property fixture
   - expects success and a legality packet with non-zero category record count.
3. `cli_ir_metadata_surface`
   - `objc3c-native.exe`
   - `hello.objc3`
   - expects successful IR emission containing the legality contract marker and
     named metadata handle.

## Gate Commands

- `python scripts/check_m251_b001_object_model_abi_invariants_and_legality_contract.py`
- `python -m pytest tests/tooling/test_check_m251_b001_object_model_abi_invariants_and_legality_contract.py -q`
- `npm run check:objc3c:m251-b001-lane-b-readiness`

## Evidence Output

- `tmp/reports/m251/M251-B001/object_model_abi_invariants_and_legality_contract_summary.json`
