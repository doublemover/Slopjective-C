# Fixture: Dispatch Reopen Guardrails W1 Dispatch Review

## 1. Scope

Reviewed sources:

- `gh issue view 904 --json number,title,body,milestone,url`
- `gh issue view 905 --json number,title,body,milestone,url`
- `gh issue view 906 --json number,title,body,milestone,url`
- `gh issue view 907 --json number,title,body,milestone,url`
- `gh issue view 908 --json number,title,body,milestone,url`

## 3. M17 Task Set

| Issue | Lane | Title |
| --- | --- | --- |
| `#904` | A | Define deterministic contract and normative baseline |
| `#905` | B | Implement tooling and deterministic test coverage |
| `#906` | C | Integrate CI/workflow/reporting and operator entrypoints |
| `#907` | D | Publish governance, risk, and runbook controls |
| `#908` | INT | Orchestrate milestone control-plane, intake, and closeout |

Mandatory source inputs for all five issues:

- `spec/planning/v013_activation_reopen_playbook_20260223.md`
- `spec/planning/v013_activation_preflight_runner_package_20260223.md`
- `spec/planning/v013_next_dispatch_candidate_batch_20260223.md`

## 4. Parallelization Decision

1. Execute lanes `A/B/C/D` in parallel with strict file ownership and no overlapping write paths.
2. Execute integrator preflight publication in parallel with lane execution.
3. Serialize GH closeout at terminal stage as `A -> B -> C -> D -> INT`, then close milestone.

## 5. Dispatch Decision

Assigned workers:

1. Lane `A` (`#904`): `019c90a2-bf71-70f0-9915-0850bedee4b8`
2. Lane `B` (`#905`): `019c90a2-bfc8-7801-ac60-2137d732fd46`
3. Lane `C` (`#906`): `019c90a2-bfd9-7df1-9eac-8c06ceb7bd05`
4. Lane `D` (`#907`): `019c90a2-c004-7b71-acbe-74574227ca9a`
