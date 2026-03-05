# M234 Property and Ivar Syntax Surface Completion Modular Split/Scaffolding Expectations (A002)

Contract ID: `objc3c-property-and-ivar-syntax-surface-completion-modular-split-scaffolding/m234-a002-v1`
Status: Accepted
Scope: M234 lane-A modular split/scaffolding continuity for property and ivar syntax surface completion dependency wiring.

## Objective

Fail closed unless lane-A modular split/scaffolding dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M234-A001`
- M234-A001 freeze anchors remain mandatory prerequisites:
  - `docs/contracts/m234_property_and_ivar_syntax_surface_completion_contract_and_architecture_freeze_a001_expectations.md`
  - `spec/planning/compiler/m234/m234_a001_property_and_ivar_syntax_surface_completion_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m234_a001_property_and_ivar_syntax_surface_completion_contract.py`
  - `tests/tooling/test_check_m234_a001_property_and_ivar_syntax_surface_completion_contract.py`
- Packet/checker/test assets for A002 remain mandatory:
  - `spec/planning/compiler/m234/m234_a002_property_and_ivar_syntax_surface_completion_modular_split_scaffolding_packet.md`
  - `scripts/check_m234_a002_property_and_ivar_syntax_surface_completion_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m234_a002_property_and_ivar_syntax_surface_completion_modular_split_scaffolding_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M234 lane-A A002 property and ivar syntax surface completion modular split/scaffolding anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-A property and ivar syntax surface completion modular split/scaffolding fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-A property and ivar syntax surface completion modular split metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m234-a002-property-and-ivar-syntax-surface-completion-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m234-a002-property-and-ivar-syntax-surface-completion-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m234-a002-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m234_a002_property_and_ivar_syntax_surface_completion_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m234_a002_property_and_ivar_syntax_surface_completion_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m234-a002-lane-a-readiness`

## Evidence Path

- `tmp/reports/m234/M234-A002/property_and_ivar_syntax_surface_completion_modular_split_scaffolding_summary.json`

