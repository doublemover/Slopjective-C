# M314-A001 Planning Packet

## Summary

Freeze the current command-surface inventory and a script-budget policy before `M314` begins collapsing package aliases and workflow entrypoints.

## Current measured surface

- `package.json` scripts: `7815`
- milestone-local script aliases: `7496`
- package-script namespace leaders:
  - `check`: `5235`
  - `test`: `2498`
  - `plan`: `25`
  - `refresh`: `24`
  - `run`: `10`
  - `build`: `6`
- workflow files under `.github/workflows/`: `9`
- non-`__pycache__` prototype compiler source files under `compiler/`: `1`

## Frozen budget target

- public package entrypoints: `<=25`
- public command families allowed after cleanup:
  - `build`
  - `compile`
  - `lint`
  - `test`
  - `package`
  - `tool`
  - `proof`
- milestone-local `check:*` and `test:tooling:*` script growth is explicitly not a long-term accepted operator model.

## Non-goals

- Do not collapse scripts yet.
- Do not retire compatibility aliases yet.
- Do not rewrite README operator workflows yet.
- Do not move or retire the dead prototype compiler path yet.

## Replacement direction

- Package scripts remain the public operator layer.
- Python runners become the internal parameterized orchestration layer.
- PowerShell becomes an implementation detail behind documented package or Python entrypoints.
- Workflow YAML remains CI-only.

## Evidence

- `spec/planning/compiler/m314/m314_a001_command_surface_inventory_and_script_budget_policy_contract_and_architecture_freeze_inventory.json`

Next issue: `M314-A002`.
