# M314-B001 Expectations

Contract ID: `objc3c-cleanup-public-internal-command-model/m314-b001-v1`

## Purpose

Freeze the command-model semantics for `M314`: what counts as a public operator command, what counts as an internal or implementation detail, what has been explicitly retired, and what is prohibited from growing further.

## Required truths

- Public operator commands are a compact subset of `package.json` scripts only.
- Internal parameterized runners under `scripts/` remain allowed but are non-public.
- PowerShell scripts remain implementation details and may only surface publicly through package-script wrappers.
- GitHub workflows remain CI-only and must not become operator documentation.
- Legacy milestone-local `check:*` and `test:tooling:*` aliases are removed from `package.json` and may survive only as archival references.
- New public command families outside the frozen allow-list require explicit exception handling later in the cleanup program.

## Allowed public surface

- `build`
- `compile`
- `lint`
- `test`
- `package`
- `tool`
- `proof`

## Retired or transitional surface

- legacy milestone-local package aliases removed from `package.json`
- existing direct Python README entries pending workflow-doc cleanup
- existing direct PowerShell README smoke entry pending wrapper cleanup

## Prohibited patterns

- new public `plan:*`, `refresh:*`, `run:*`, or `dev:*` operator commands
- new README operator instructions that bypass package scripts when a public entrypoint exists
- new CI workflow commands documented as normal operator paths
- any reintroduction of `compiler/objc3c/` as a public compiler interface

## Evidence

- `spec/planning/compiler/m314/m314_b001_public_versus_internal_command_model_contract_and_architecture_freeze_model.json`
- `tmp/reports/m314/M314-B001/public_internal_command_model_summary.json`

## Next issue

- `M314-B002`
