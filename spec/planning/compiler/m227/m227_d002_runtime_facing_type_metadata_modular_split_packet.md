# M227-D002 Runtime-Facing Type Metadata Modular Split Packet

Packet: `M227-D002`
Milestone: `M227`
Lane: `D`

## Scope

Enforce runtime-facing type metadata modular split/scaffolding boundaries so sema pass orchestration remains scaffolded, runtime metadata handoff transport stays deterministic, and artifact/runtime projection remains fail-closed.

## Anchors

- Contract: `docs/contracts/m227_runtime_facing_type_metadata_modular_split_d002_expectations.md`
- Checker: `scripts/check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_d002_runtime_facing_type_metadata_modular_split_contract.py`
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

## Required Evidence

- `tmp/reports/m227/M227-D002/runtime_facing_type_metadata_modular_split_contract_summary.json`

## Determinism Criteria

- Sema handoff scaffolding remains explicit and budget-guarded (`kObjc3ParserSemaHandoffScaffoldBuilderMaxLines` and performance guardrails).
- Sema pass-flow bookkeeping remains scaffold-owned (`MarkObjc3SemaPassExecuted` + `FinalizeObjc3SemaPassFlowSummary`) and not re-inlined into `objc3_sema_pass_manager.cpp`.
- Runtime-facing type metadata handoff derivation remains ordered: parser handoff scaffold build, semantic type metadata handoff derivation, pass-flow summary finalization.
- Frontend pipeline transport still moves `sema_type_metadata_handoff` and parity surface to runtime-facing artifact projection.
- Runtime shim host-link and retain/release lowering contracts are projected into IR frontend metadata with deterministic flags and replay keys.
