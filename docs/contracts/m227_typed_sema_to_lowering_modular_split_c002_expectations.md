# M227 Typed Sema-to-Lowering Modular Split Scaffolding Expectations (C002)

Contract ID: `objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1`
Status: Accepted
Scope: M227 lane-C typed sema-to-lowering modular split scaffolding continuity for deterministic semantic handoff and lowering metadata readiness.

## Objective

Fail closed unless lane-C typed sema-to-lowering modular split anchors remain explicit, deterministic, and traceable across pipeline code surfaces, docs, shared architecture/spec anchors, and package readiness wiring.

## Dependency Scope

- Issue `#5122` defines canonical lane-C modular split scaffolding scope.
- Dependencies: `M227-C001`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m227/m227_c002_typed_sema_to_lowering_modular_split_packet.md`
  - `scripts/check_m227_c002_typed_sema_to_lowering_modular_split_contract.py`
  - `tests/tooling/test_check_m227_c002_typed_sema_to_lowering_modular_split_contract.py`
- Prerequisite assets from `M227-C001` remain mandatory:
  - `docs/contracts/m227_typed_sema_to_lowering_contract_expectations.md`
  - `spec/planning/compiler/m227/m227_c001_typed_sema_to_lowering_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m227_c001_typed_sema_to_lowering_contract.py`
  - `tests/tooling/test_check_m227_c001_typed_sema_to_lowering_contract.py`

## Pipeline Modular Split Anchors

- `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h` preserves dedicated typed sema/lowering modular split scaffold helpers and fail-closed readiness derivation.
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp` preserves canonical assignment of `result.typed_sema_to_lowering_contract_surface = BuildObjc3TypedSemaToLoweringContractSurface(result, options);`.
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h` preserves typed-surface include wiring and fail-closed parse/lowering readiness derivation from typed sema handoff continuity.
- `native/objc3c/src/pipeline/objc3_frontend_types.h` preserves `Objc3TypedSemaToLoweringContractSurface` transport on `Objc3FrontendPipelineResult`.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-C C002 typed sema-to-lowering modular split scaffolding fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C typed sema-to-lowering modular split governance wording with explicit `M227-C001` dependency anchor.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C typed sema-to-lowering modular split metadata anchor wording for `M227-C002`.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m227-c002-typed-sema-to-lowering-modular-split-contract`.
- `package.json` includes `test:tooling:m227-c002-typed-sema-to-lowering-modular-split-contract`.
- `package.json` includes `check:objc3c:m227-c002-lane-c-readiness`.

## Milestone Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m227_c002_typed_sema_to_lowering_modular_split_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c002_typed_sema_to_lowering_modular_split_contract.py -q`
- `npm run check:objc3c:m227-c002-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C002/typed_sema_to_lowering_modular_split_contract_summary.json`
