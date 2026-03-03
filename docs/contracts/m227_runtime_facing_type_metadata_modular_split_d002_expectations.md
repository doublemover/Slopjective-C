# Runtime-Facing Type Metadata Modular Split and Scaffolding Expectations (M227-D002)

Contract ID: `objc3c-runtime-facing-type-metadata-modular-split-scaffold/m227-d002-v1`
Status: Accepted
Scope: M227 lane-D runtime-facing type metadata modular split/scaffolding continuity for deterministic sema pass orchestration and runtime metadata projection.

## Objective

Fail closed unless lane-D runtime-facing type metadata modular split/scaffolding anchors remain explicit, deterministic, and traceable across sema scaffolds, pipeline/artifact projection, docs/planning assets, shared architecture/spec anchors, and package lane-D readiness wiring.

## Dependency Scope

- Issue `#5148` defines canonical lane-D modular split/scaffolding scope.
- Dependencies: `M227-D001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m227/m227_d002_runtime_facing_type_metadata_modular_split_packet.md`
  - `scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`
  - `tests/tooling/test_check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`
- Prerequisite assets from `M227-D001` remain mandatory:
  - `docs/contracts/m227_runtime_facing_type_metadata_semantics_expectations.md`
  - `spec/planning/compiler/m227/m227_d001_runtime_facing_type_metadata_semantics_contract_freeze.md`
  - `scripts/check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`
  - `tests/tooling/test_check_m227_d001_runtime_facing_type_metadata_semantics_contract.py`

## Runtime Metadata Scaffold Anchors

- `native/objc3c/src/sema/objc3_parser_sema_handoff_scaffold.h` preserves parser/sema handoff scaffold continuity (`BuildObjc3ParserSemaHandoffScaffold(...)`) and pinned scaffold builder budget (`kObjc3ParserSemaHandoffScaffoldBuilderMaxLines`).
- `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.h` and `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp` preserve pass-flow bookkeeping ownership (`MarkObjc3SemaPassExecuted(...)`, `FinalizeObjc3SemaPassFlowSummary(...)`) instead of pass-manager inlining.
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp` preserves deterministic scaffold ordering: parser handoff scaffold build, semantic type metadata handoff derivation, and pass-flow summary finalization.
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` preserves deterministic transport of `sema_type_metadata_handoff` and `sema_parity_surface`.
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` preserves deterministic runtime-shim host-link and retain/release lowering contract projection into frontend metadata and replay keys.
- Build manifests preserve modular split wiring for `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.cpp`:
  - `native/objc3c/CMakeLists.txt`
  - `scripts/build_objc3c_native.ps1`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-D D002 runtime-facing type metadata modular split/scaffolding fail-closed anchor text with `M227-D001` dependency continuity.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-D runtime-facing type metadata modular split/scaffolding governance wording with explicit `M227-D001` dependency anchor.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-D runtime-facing type metadata modular split/scaffolding metadata anchor wording for `M227-D002`.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract`.
- `package.json` includes `test:tooling:m227-d002-runtime-facing-type-metadata-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m227-d002-lane-d-readiness`.

## Milestone Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py -q`
- `npm run check:objc3c:m227-d002-lane-d-readiness`

## Evidence Path

- `tmp/reports/m227/M227-D002/runtime_facing_type_metadata_modular_split_contract_summary.json`
