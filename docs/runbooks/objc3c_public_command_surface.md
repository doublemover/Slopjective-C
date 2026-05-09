# Objective-C 3 Public Command Surface

This runbook is generated from the canonical public command contract.
It is an operator-facing appendix, not the primary onboarding or project-explanation surface.

- Current package script count: `1`
- Operator command count: `1`
- Maintainer command count: `0`
- Package bridge: `npm run objc3c -- <action>`
- Package bridge owner: `package.json`
- Action catalog owner: checked-in workflow action catalog
- Contract builder: action-catalog-owned public command contract builder
- Contract artifact: `tmp/artifacts/public-command-surface/objc3c-public-command-contract.json`

## Operator Commands

| Package script | Runner action | Tier | Guarantee owner | Extra args | Backend |
| --- | --- | --- | --- | --- | --- |
| `objc3c` | `<action>` | `repo` | `GitHub Actions and local npm users route workflow actions through one package bridge` | `pass-through` | `npm run objc3c -- <action>` |

## Maintainer Commands

| Package script | Runner action | Tier | Guarantee owner | Extra args | Backend |
| --- | --- | --- | --- | --- | --- |

## Operator Notes

- Use the operator commands above for normal public workflows.
- Treat this file as a generated machine-facing appendix for exact command mapping, not as the reader-facing project introduction.
- Maintainer commands are intentionally narrower wrappers for repo hygiene, markdown upkeep, release-evidence checks, and dependency/capability audits.
- Canonical user-facing commands use `npm run objc3c -- <action>` and route through the single `package.json` bridge.
- Action names, validation tiers, pass-through behavior, backend descriptions, and guarantee owners are action-catalog-owned.
- Canonical checked-in doc outputs are `site/index.md`, `docs/objc3c-native.md`, and `docs/runbooks/objc3c_public_command_surface.md`; edit their source roots instead of the generated files.
- `native/objc3c/`, `scripts/`, and `tests/` are the live implementation roots; `tmp/` and `artifacts/` are output roots, not naming roots.
- Composite validation entrypoints write an integrated runner summary to `tmp/reports/objc3c-public-workflow/<action>.json`.
- Those integrated summaries record the exact child-suite report paths emitted by smoke, replay, runtime-acceptance, and other live validation scripts.
- `compile-objc3c` and the fixture-backed suite actions accept pass-through arguments through the npm bridge.
- Direct `scripts/objc3c_workflow/runner.py` execution is rejected; the npm bridge is the only public route.
- No additional retired command names remain supported.
- The repository does not publish a retired package-script metadata surface.
