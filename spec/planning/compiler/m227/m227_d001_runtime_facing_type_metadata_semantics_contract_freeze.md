# M227-D001 Runtime-Facing Type Metadata Semantics Contract Freeze Packet

Packet: `M227-D001`
Milestone: `M227`
Lane: `D`

## Scope

Freeze runtime-facing type metadata semantics contract and architecture anchors before modular/runtime validation workpacks in M227-D002+.

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

## Required Evidence

- `tmp/reports/m227/M227-D001/runtime_type_metadata_semantics_contract_summary.json`

## Determinism Criteria

- Canonical runtime message-send and reference type forms are pinned by sema boundary contract helpers.
- Runtime dispatch symbol defaults are cross-layer consistent (`objc3_msgsend_i32`) between sema and pipeline contracts.
- Sema parity readiness includes runtime-shim and retain/release deterministic handoff gates.
- Pipeline transports sema type metadata + parity surfaces to runtime-facing artifact metadata without bypass paths.
- Manifest/IR metadata projection preserves deterministic runtime-facing handoff flags and replay keys.
