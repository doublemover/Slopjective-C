# M318-B002 Planning Packet

Issue: `#7838`  
Title: `Sustainable planning hygiene policy`

## Objective

Implement the long-lived planning-hygiene policy so future issue seeding and
amendments have to declare budget impact, exception linkage, and validation posture
up front.

## Implementation surfaces

- `spec/governance/objc3c_planning_hygiene_policy.json`
- `docs/runbooks/objc3c_planning_hygiene.md`
- `.github/ISSUE_TEMPLATE/roadmap_execution.yml`
- `.github/ISSUE_TEMPLATE/conformance_execution.yml`
- `package.json#objc3cGovernance`

## Implemented rules

- Every new roadmap/conformance issue must declare one validation posture.
- Every new roadmap/conformance issue must declare one budget-impact class:
  - `no_budget_growth`
  - `within_existing_budget`
  - `requires_exception_record`
- If budget impact is `requires_exception_record`, the issue must carry an exception
  record reference.
- The templates continue to prefer shared acceptance and runner surfaces over new
  milestone-local checkers and scripts.

## Validation posture

Static verification is justified because this issue wires policy into templates and a
runbook rather than landing live CI alarms.

Next issue: `M318-B003`.
