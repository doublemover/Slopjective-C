# M227-D002 Runtime-Facing Type Metadata Modular Split Packet

Packet: `M227-D002`
Milestone: `M227`
Lane: `D`
Issue: `#5148`
Freeze date: `2026-03-03`
Dependencies: `M227-D001`

## Purpose

Freeze lane-D runtime-facing type metadata modular split/scaffolding prerequisites so sema pass orchestration remains scaffold-owned, runtime metadata handoff transport stays deterministic, and artifact/runtime projection remains fail-closed before core-feature expansion workpacks.

## Scope Anchors

- Contract:
  `docs/contracts/m227_runtime_facing_type_metadata_modular_split_d002_expectations.md`
- Checker:
  `scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`
- Sema handoff scaffold:
  - `native/objc3c/src/sema/objc3_parser_sema_handoff_scaffold.h`
- Sema pass-flow scaffold:
  - `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.h`
  - `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp`
- Sema orchestration integration:
  - `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
  - `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- Pipeline/runtime-facing transport and projection:
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Build wiring:
  - `native/objc3c/CMakeLists.txt`
  - `scripts/build_objc3c_native.ps1`
- Dependency anchors from `M227-D001`:
  - `docs/contracts/m227_runtime_facing_type_metadata_semantics_expectations.md`
  - `spec/planning/compiler/m227/m227_d001_runtime_facing_type_metadata_semantics_contract_freeze.md`
  - `scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`
  - `tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract`
  - `test:tooling:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract`
  - `check:objc3c:m227-d002-lane-d-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Inputs (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py -q`
- `npm run check:objc3c:m227-d002-lane-d-readiness`

## Evidence Output

- `tmp/reports/m227/M227-D002/runtime_facing_type_metadata_modular_split_contract_summary.json`
