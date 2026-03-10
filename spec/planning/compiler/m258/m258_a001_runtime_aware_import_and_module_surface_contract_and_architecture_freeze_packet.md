# M258-A001 Runtime-Aware Import and Module Surface Contract and Architecture Freeze Packet

Packet: `M258-A001`
Milestone: `M258`
Lane: `A`
Issue: `#7158`
Freeze date: `2026-03-10`
Dependencies: none
Next issue: `M258-A002`

## Purpose

Freeze the runtime-aware import/module source surface above the current local module-import graph lowering facts and below any real imported runtime-owned node or metadata-reference realization.

## Scope Anchors

- Contract:
  `docs/contracts/m258_runtime_aware_import_and_module_surface_contract_and_architecture_freeze_a001_expectations.md`
- Checker:
  `scripts/check_m258_a001_runtime_aware_import_module_surface_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m258_a001_runtime_aware_import_module_surface_contract.py`
- Readiness runner:
  `scripts/run_m258_a001_lane_a_readiness.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m258-a001-runtime-aware-import-and-module-surface-contract`
  - `test:tooling:m258-a001-runtime-aware-import-and-module-surface-contract`
  - `check:objc3c:m258-a001-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Code anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/libobjc3c_frontend/api.h`

## Canonical Surface

- Contract id:
  `objc3c-runtime-aware-import-module-surface/m258-a001-v1`
- Semantic-surface path:
  `frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_surface_contract`
- Source model:
  `runtime-aware-import-module-surface-freezes-frontend-owned-runtime-declaration-and-metadata-reference-boundaries-before-cross-translation-unit-realization`
- Non-goal model:
  `no-imported-module-artifact-reader-no-imported-runtime-declaration-materialization-no-imported-runtime-metadata-reference-lowering`
- Failure model:
  `fail-closed-on-runtime-aware-import-module-surface-drift-or-premature-capability-claims`

## Happy-Path Fixture

- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`

## Gate Commands

- `python scripts/check_m258_a001_runtime_aware_import_module_surface_contract.py`
- `python -m pytest tests/tooling/test_check_m258_a001_runtime_aware_import_module_surface_contract.py -q`
- `python scripts/run_m258_a001_lane_a_readiness.py`

## Evidence Output

- `tmp/reports/m258/M258-A001/runtime_aware_import_module_surface_contract_summary.json`
