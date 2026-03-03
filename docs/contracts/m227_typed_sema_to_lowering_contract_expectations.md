# M227 Typed Sema-to-Lowering Contracts Contract and Architecture Freeze Expectations (C001)

Contract ID: `objc3c-typed-sema-to-lowering-contract-and-architecture-freeze/m227-c001-v1`
Status: Accepted
Scope: M227 lane-C typed sema-to-lowering contracts contract and architecture freeze for deterministic semantic handoff and lowering metadata continuity.

## Objective

Fail closed unless lane-C typed sema-to-lowering anchors remain explicit,
deterministic, and traceable across pipeline code surfaces, docs, shared
architecture/spec anchors, and package readiness wiring.

## Dependency Scope

- Issue `#5121` defines canonical lane-C contract freeze scope.
- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m227/m227_c001_typed_sema_to_lowering_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m227_c001_typed_sema_to_lowering_contract.py`
  - `tests/tooling/test_check_m227_c001_typed_sema_to_lowering_contract.py`

## Pipeline Contract Anchors

- `native/objc3c/src/pipeline/frontend_pipeline_contract.h` preserves typed
  stage sema/lowering carrier structures (`FunctionSignatureSurface`,
  `SemaStageOutput`, lowering dispatch boundaries, and fail-closed error-model
  defaults).
- `native/objc3c/src/pipeline/objc3_frontend_types.h` preserves typed sema-to-
  lowering handoff surfaces on `Objc3FrontendPipelineResult`
  (`integration_surface`, `sema_type_metadata_handoff`, object-
  pointer/nullability/generics summary, symbol-graph/scope-resolution summary,
  and `sema_parity_surface`).
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` preserves canonical
  sema orchestration through `RunObjc3SemaPassManager(...)`, deterministic
  summary derivation, and parse/lowering readiness projection.
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` preserves typed
  lowering contract construction from sema/type carriers and deterministic
  projection into manifest and IR frontend metadata.
- `docs/objc3c-native/src/30-semantics.md` and
  `docs/objc3c-native/src/50-artifacts.md` remain synchronized with the typed
  sema transport and lowering metadata anchors consumed by this freeze gate.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-C C001 typed
  sema-to-lowering contracts contract and architecture freeze fail-closed anchor
  text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C typed sema-to-
  lowering governance wording with explicit `M227-C001` dependency anchor.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C typed
  sema-to-lowering metadata anchor wording for `M227-C001`.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m227-c001-typed-sema-to-lowering-contract`.
- `package.json` includes `test:tooling:m227-c001-typed-sema-to-lowering-contract`.
- `package.json` includes `check:objc3c:m227-c001-lane-c-readiness`.

## Milestone Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m227_c001_typed_sema_to_lowering_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c001_typed_sema_to_lowering_contract.py -q`
- `npm run check:objc3c:m227-c001-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C001/typed_sema_to_lowering_contract_and_architecture_freeze_summary.json`
