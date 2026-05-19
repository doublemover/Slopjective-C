# Contributing

Keep changes small, explicit, and testable.

## Contributor Surface

Use these files as the live contributor-facing surface:

- `README.md`
- `CONTRIBUTING.md`
- `docs/tutorials/README.md`
- `docs/tutorials/getting_started.md`
- `docs/tutorials/build_run_verify.md`
- `docs/tutorials/guided_walkthrough.md`
- `docs/tutorials/objc2_to_objc3_migration.md`
- `docs/tutorials/objc2_swift_cpp_comparison.md`
- `docs/runbooks/objc3c_public_command_surface.md`

Use `docs/runbooks/objc3c_maintainer_workflows.md` only when you need the
maintainer-only workflow map.

## Repo Boundary

Treat these as the owner roots for normal contribution work:

- implementation roots:
  - `native/objc3c/`
  - `scripts/`
  - `tests/`
- doc owner inputs:
  - `README.md`
  - `CONTRIBUTING.md`
  - `docs/tutorials/README.md`
  - `docs/tutorials/getting_started.md`
  - `docs/tutorials/build_run_verify.md`
  - `docs/tutorials/guided_walkthrough.md`
  - `docs/tutorials/objc2_to_objc3_migration.md`
  - `docs/tutorials/objc2_swift_cpp_comparison.md`
  - `showcase/`
  - `site/src/`
  - `docs/objc3c-native/src/`
  - `package.json`
- generated checked-in outputs:
  - `site/index.md`
  - `docs/objc3c-native.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
- machine-owned outputs:
  - `tmp/`
  - `artifacts/`

Refresh generated checked-in outputs from their owner inputs. Do not treat
`tmp/`, `artifacts/`, or archived redirect material as primary contributor
guidance.

## Branches and Commits

- Use short descriptive branch names.
- Keep one primary goal per branch.
- Keep commits atomic.
- Include the validation you actually ran.

## Local Checks

Run these before committing:

```sh
npm run objc3c -- build-site
npm run objc3c -- lint
npm run objc3c -- check-markdown
npm run objc3c -- test-smoke
```

## Core Maintainer Checks

- dependency boundaries: `npm run objc3c -- check-dependency-boundaries`
- task hygiene: `npm run objc3c -- check-task-hygiene`
- docs drift: `npm run objc3c -- check-native-docs`
- repo superclean surface: `npm run objc3c -- check-repo-superclean-surface`

When a change widens package scripts, runbooks, schemas, checker surfaces, or
publication helpers, also refresh the governance summaries:

- governance validation: `npm run objc3c -- validate-governance-sustainability`
- governance publication metadata: `npm run objc3c -- publish-governance-sustainability`

Direct helper paths under `scripts/` are implementation anchors for the action
catalog at `scripts/objc3c_workflow/action_catalog.py`. Contributor-facing command examples should route through
`npm run objc3c -- <action>`.

## PR Expectations

- describe what changed
- describe the validation you ran
- call out risks or non-obvious tradeoffs
- do not hide follow-up work
