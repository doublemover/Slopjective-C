# M227-D001 Runtime-Facing Type Metadata Semantics Contract Freeze Packet

Packet: `M227-D001`
Milestone: `M227`
Lane: `D`
Freeze date: `2026-03-03`
Dependencies: none

## Scope

Freeze runtime-facing type metadata semantics contract and architecture anchors
before modular/runtime validation workpacks in M227-D002+ so runtime-facing
handoff continuity remains deterministic and fail-closed.

## Anchors

- Contract: `docs/contracts/m227_runtime_facing_type_metadata_semantics_expectations.md`
- Checker: `scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`
- Sema runtime metadata contract: `native/objc3c/src/sema/objc3_sema_contract.h`
- Sema parity readiness contract: `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- Pipeline contract defaults: `native/objc3c/src/pipeline/frontend_pipeline_contract.h`
- Pipeline runtime metadata transport: `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Pipeline orchestration wiring: `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- Artifact/runtime projection wiring: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-d001-runtime-facing-type-metadata-semantics-contract`
  - `test:tooling:m227-d001-runtime-facing-type-metadata-semantics-contract`
  - `check:objc3c:m227-d001-lane-d-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Required Evidence

- `tmp/reports/m227/M227-D001/runtime_type_metadata_semantics_contract_summary.json`

## Determinism Criteria

- Canonical runtime message-send and reference type forms are pinned by sema boundary contract helpers.
- Runtime dispatch symbol defaults are cross-layer consistent (`objc3_msgsend_i32`) between sema and pipeline contracts.
- Sema parity readiness includes runtime-shim and retain/release deterministic handoff gates.
- Pipeline transports sema type metadata + parity surfaces to runtime-facing artifact metadata without bypass paths.
- Manifest/IR metadata projection preserves deterministic runtime-facing handoff flags and replay keys.
- Architecture/spec metadata anchors and package lane-D readiness wiring remain
  explicit and fail closed on drift.

## Gate Commands

- `python scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py -q`
- `npm run check:objc3c:m227-d001-lane-d-readiness`
