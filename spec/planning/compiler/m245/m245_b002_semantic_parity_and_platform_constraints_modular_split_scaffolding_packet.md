# M245-B002 Semantic Parity and Platform Constraints Modular Split/Scaffolding Packet

Packet: `M245-B002`
Milestone: `M245`
Lane: `B`
Freeze date: `2026-03-03`
Dependencies: `M245-B001`

## Purpose

Freeze lane-B semantic parity and platform constraints modular
split/scaffolding prerequisites for M245 so typed sema handoff and
parse/lowering compatibility gating remain deterministic and fail-closed across
dependency wiring.

## Scope Anchors

- Contract:
  `docs/contracts/m245_semantic_parity_and_platform_constraints_modular_split_scaffolding_b002_expectations.md`
- Checker:
  `scripts/check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py`
- Prerequisite B001 assets:
  - `docs/contracts/m245_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_b001_expectations.md`
  - `spec/planning/compiler/m245/m245_b001_semantic_parity_and_platform_constraints_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m245_b001_semantic_parity_and_platform_constraints_contract.py`
  - `tests/tooling/test_check_m245_b001_semantic_parity_and_platform_constraints_contract.py`
- Pipeline semantic/platform anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract`
  - `test:tooling:m245-b002-semantic-parity-platform-constraints-modular-split-scaffolding-contract`
  - `check:objc3c:m245-b002-lane-b-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m245_b002_semantic_parity_and_platform_constraints_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m245-b002-lane-b-readiness`

## Evidence Output

- `tmp/reports/m245/M245-B002/semantic_parity_and_platform_constraints_modular_split_scaffolding_contract_summary.json`
