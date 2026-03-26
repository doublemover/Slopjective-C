# M318-C001 Planning Packet

Issue: `#7840`  
Title: `Governance automation and CI policy contract`

## Objective

Freeze the shared automation contract that later `M318` issues will implement for
budget enforcement, regression alarms, and new-work proposal tooling.

## Contract summary

- shared runner path: `scripts/m318_governance_guard.py`
- CI workflow path: `.github/workflows/m318-governance-budget-enforcement.yml`
- stage order:
  - `budget-snapshot`
  - `exception-registry`
  - `residue-proof`
  - `topology`
- alarm classes:
  - `public_command_budget_regression`
  - `validation_budget_regression`
  - `expired_exception_record`
  - `source_hygiene_regression`
  - `artifact_authenticity_regression`

## Validation posture

Static verification is justified because this issue freezes the automation contract and
report layout before live workflow implementation lands.

Next issue: `M318-C002`.
