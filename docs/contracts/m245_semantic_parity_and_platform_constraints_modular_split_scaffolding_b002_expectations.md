# M245 Semantic Parity and Platform Constraints Modular Split/Scaffolding Expectations (B002)

Contract ID: `objc3c-semantic-parity-platform-constraints-modular-split-scaffolding/m245-b002-v1`
Status: Accepted
Scope: M245 lane-B semantic parity/platform constraints modular split/scaffolding continuity across typed sema-to-lowering and parse/lowering readiness surfaces.

## Objective

Fail closed unless M245 lane-B semantic parity and platform constraints modular
split/scaffolding anchors remain explicit, deterministic, and traceable across
dependency surfaces, including pipeline code anchors and milestone
optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M245-B001`
- Prerequisite frozen assets from `M245-B001` remain mandatory:
  - `docs/contracts/m245_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_b001_expectations.md`
  - `spec/planning/compiler/m245/m245_b001_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py`
  - `tests/tooling/test_check_m245_b001_semantic_parity_and_platform_constraints_contract.py`
- Packet/checker/test assets for `M245-B002` remain mandatory:
  - `spec/planning/compiler/m245/m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_packet.md`
  - `scripts/check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py`

## Semantic and Platform Anchors

- `native/objc3c/src/pipeline/objc3_frontend_types.h` preserves explicit typed
  sema parity and parse/lowering compatibility handoff readiness flags.
- `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  preserves runtime dispatch bounds and fail-closed typed readiness gating.
- `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
  preserves deterministic compatibility handoff key synthesis and fail-closed
  long-tail compatibility readiness gating.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m245-b002-lane-b-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m245-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m245/M245-B002/semantic_parity_and_platform_constraints_modular_split_scaffolding_contract_summary.json`
