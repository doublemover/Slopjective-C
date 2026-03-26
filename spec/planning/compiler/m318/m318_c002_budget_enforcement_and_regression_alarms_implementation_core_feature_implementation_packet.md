# M318-C002 Planning Packet

Issue: `#7841`  
Title: `Budget enforcement and regression alarms implementation`

## Objective

Implement the shared governance guard and workflow defined by `M318-C001`.

## Implemented surfaces

- `scripts/m318_governance_guard.py`
- `.github/workflows/m318-governance-budget-enforcement.yml`
- topology stage and summary emission under `tmp/reports/m318/governance/`

## Implemented stages

- `budget-snapshot`
- `exception-registry`
- `residue-proof`
- `topology`

## Implemented alarm classes

- `public_command_budget_regression`
- `validation_budget_regression`
- `expired_exception_record`
- `source_hygiene_regression`
- `artifact_authenticity_regression`

## Validation posture

Executable integration is justified here because the shared governance runner and CI
workflow are live implementation surfaces.

Next issue: `M318-C003`.
