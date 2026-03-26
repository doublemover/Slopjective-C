# M314-A001 Expectations

Contract ID: `objc3c-cleanup-command-surface-inventory-budget/m314-a001-v1`

## Purpose

Freeze one truthful inventory of the current operator-facing command surface and one explicit script-budget policy for the workflow-simplification tranche.

## Required truths

- The current `package.json` script surface contains `7815` entries.
- The current script surface is dominated by milestone-local aliases and wrappers rather than a compact operator-facing interface.
- The current repo contains `9` GitHub workflow files under `.github/workflows/`.
- The only non-`__pycache__` source file left under the dead prototype compiler path is `compiler/objc3c/semantic.py`.
- The cleanup target is a public command surface of at most `25` documented package entrypoints.
- No new public command entrypoints may be introduced outside the frozen public families without an explicit future exception policy.

## Frozen policy boundary

- Public operator-facing entrypoints remain a package-script surface, not raw Python, PowerShell, or workflow YAML.
- Python remains allowed as the internal parameterized orchestration layer.
- PowerShell remains an implementation detail or platform-specific helper layer, not a competing public command namespace.
- GitHub workflow files remain CI entrypoints only and are not operator-facing commands.

## Evidence

- `spec/planning/compiler/m314/m314_a001_command_surface_inventory_and_script_budget_policy_contract_and_architecture_freeze_inventory.json`
- `tmp/reports/m314/M314-A001/command_surface_inventory_budget_summary.json`

## Next issue

- `M314-A002`
