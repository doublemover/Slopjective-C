# Objective-C 3 Public Command Surface

This runbook is generated from the canonical public command contract.
It is an operator-facing appendix, not the primary onboarding or project-explanation surface.

- Current package script count: `1`
- Operator command count: `1`
- Maintainer command count: `0`
- Runner path: `scripts.objc3c_workflow`
- Contract builder: `scripts/build_objc3c_public_command_contract.py`
- Contract artifact: `tmp/artifacts/public-command-surface/objc3c-public-command-contract.json`

## Operator Commands

| Package script | Runner action | Tier | Guarantee owner | Extra args | Backend |
| --- | --- | --- | --- | --- | --- |
| `objc3c` | `<action>` | `repo` | `GitHub Actions and local npm users route workflow actions through one package bridge` | `pass-through` | `python -m scripts.objc3c_workflow` |

## Maintainer Commands

| Package script | Runner action | Tier | Guarantee owner | Extra args | Backend |
| --- | --- | --- | --- | --- | --- |

## Operator Notes

- Use the operator commands above for normal public workflows.
- Treat this file as a generated machine-facing appendix for exact command mapping, not as the reader-facing project introduction.
- Maintainer commands are intentionally narrower wrappers for repo hygiene, markdown upkeep, release-evidence checks, and dependency/capability audits.
- Canonical user-facing command names come from `package.json` and map directly to `python -m scripts.objc3c_workflow` action names.
- Canonical checked-in doc outputs are `site/index.md`, `docs/objc3c-native.md`, and `docs/runbooks/objc3c_public_command_surface.md`; edit their source roots instead of the generated files.
- `native/objc3c/`, `scripts/`, and `tests/` are the live implementation roots; `tmp/` and `artifacts/` are output roots, not naming roots.
- Composite validation entrypoints write an integrated runner summary to `tmp/reports/objc3c-public-workflow/<action>.json`.
- Those integrated summaries record the exact child-suite report paths emitted by smoke, replay, runtime-acceptance, and other live validation scripts.
- `compile:objc3c` and the fixture-backed suite commands accept pass-through arguments for bounded selectors.
- No additional package-script compatibility aliases remain supported.
