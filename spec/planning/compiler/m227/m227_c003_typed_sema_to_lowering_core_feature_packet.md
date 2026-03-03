# M227-C003 Typed Sema-to-Lowering Core Feature Packet

Packet: `M227-C003`
Milestone: `M227`
Lane: `C`
Issue: `#5123`
Freeze date: `2026-03-03`
Dependencies: `M227-C001`, `M227-C002`

## Purpose

Freeze lane-C typed sema-to-lowering core feature implementation prerequisites so typed handoff case accounting, typed core feature key continuity, and parse/lowering readiness gating remain deterministic and fail-closed as core-feature expansion workpacks build on this baseline.

## Scope Anchors

- Contract:
  `docs/contracts/m227_typed_sema_to_lowering_core_feature_c003_expectations.md`
- Checker:
  `scripts/check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`
- Pipeline/code anchors:
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Dependency anchors from `M227-C001`:
  - `docs/contracts/m227_typed_sema_to_lowering_contract_expectations.md`
  - `spec/planning/compiler/m227/m227_c001_typed_sema_to_lowering_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m227_c001_typed_sema_to_lowering_contract.py`
  - `tests/tooling/test_check_m227_c001_typed_sema_to_lowering_contract.py`
- Dependency anchors from `M227-C002`:
  - `docs/contracts/m227_typed_sema_to_lowering_modular_split_c002_expectations.md`
  - `spec/planning/compiler/m227/m227_c002_typed_sema_to_lowering_modular_split_packet.md`
  - `scripts/check_m227_c002_typed_sema_to_lowering_modular_split_contract.py`
  - `tests/tooling/test_check_m227_c002_typed_sema_to_lowering_modular_split_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m227-c003-typed-sema-to-lowering-core-feature-contract`
  - `test:tooling:m227-c003-typed-sema-to-lowering-core-feature-contract`
  - `check:objc3c:m227-c003-lane-c-readiness`
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

- `python scripts/check_m227_c003_typed_sema_to_lowering_core_feature_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c003_typed_sema_to_lowering_core_feature_contract.py -q`
- `npm run check:objc3c:m227-c003-lane-c-readiness`

## Evidence Output

- `tmp/reports/m227/M227-C003/typed_sema_to_lowering_core_feature_contract_summary.json`
