# M227 Typed Sema-to-Lowering Core Feature Implementation Expectations (C003)

Contract ID: `objc3c-typed-sema-to-lowering-core-feature/m227-c003-v1`
Status: Accepted
Scope: M227 lane-C typed sema-to-lowering core feature implementation continuity for deterministic case-accounted handoff readiness.

## Objective

Fail closed unless lane-C typed sema-to-lowering core feature implementation anchors remain explicit, deterministic, and traceable across pipeline code surfaces, docs, shared architecture/spec anchors, and package readiness wiring.

## Dependency Scope

- Issue `#5123` defines canonical lane-C core feature implementation scope.
- Dependencies: `M227-C001`, `M227-C002`
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m227/m227_c003_typed_sema_to_lowering_core_feature_packet.md`
  - `scripts/check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`
  - `tests/tooling/test_check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`
- Prerequisite assets from `M227-C001` remain mandatory:
  - `docs/contracts/m227_typed_sema_to_lowering_contract_expectations.md`
  - `spec/planning/compiler/m227/m227_c001_typed_sema_to_lowering_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m227_c001_typed_sema_to_lowering_contract.py`
  - `tests/tooling/test_check_m227_c001_typed_sema_to_lowering_contract.py`
- Prerequisite assets from `M227-C002` remain mandatory:
  - `docs/contracts/m227_typed_sema_to_lowering_modular_split_c002_expectations.md`
  - `spec/planning/compiler/m227/m227_c002_typed_sema_to_lowering_modular_split_packet.md`
  - `scripts/check_m227_c002_typed_sema_to_lowering_modular_split_contract.py`
  - `tests/tooling/test_check_m227_c002_typed_sema_to_lowering_modular_split_contract.py`

## Pipeline Core Feature Anchors

- `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h` preserves deterministic core feature case accounting, typed core feature key derivation, and fail-closed `ready_for_lowering` projection from core feature consistency.
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h` preserves typed core feature readiness derivation from typed surface continuity, typed handoff determinism, and non-empty typed core feature key gating.
- `native/objc3c/src/pipeline/objc3_frontend_types.h` preserves typed sema/lowering core feature transport fields (`semantic_handoff_consistent`, `semantic_handoff_deterministic`, `typed_handoff_key_deterministic`, `typed_core_feature_consistent`, core feature case counts, and `typed_core_feature_key`).

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-C C003 typed sema-to-lowering core feature implementation fail-closed anchor text with `M227-C002` dependency continuity.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C typed sema-to-lowering core feature governance wording with explicit `M227-C002` dependency anchor.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C typed sema-to-lowering core feature metadata anchor wording for `M227-C003`.

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m227-c003-typed-sema-to-lowering-core-feature-contract`.
- `package.json` includes `test:tooling:m227-c003-typed-sema-to-lowering-core-feature-contract`.
- `package.json` includes `check:objc3c:m227-c003-lane-c-readiness`.

## Milestone Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c003_typed_sema_to_lowering_core_feature_contract.py -q`
- `npm run check:objc3c:m227-c003-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C003/typed_sema_to_lowering_core_feature_contract_summary.json`
