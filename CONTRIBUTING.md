# Contributing

Keep changes small, explicit, and testable.

## Contributor Surface

Use these files as the live contributor-facing surface:

- `README.md`
- `CONTRIBUTING.md`
- `docs/runbooks/objc3c_public_command_surface.md`

Use `docs/runbooks/objc3c_maintainer_workflows.md` only when you need the
maintainer-only workflow map.

## Repo Boundary

Treat these as the canonical roots for normal contribution work:

- implementation roots:
  - `native/objc3c/`
  - `scripts/`
  - `tests/`
- canonical doc inputs:
  - `README.md`
  - `CONTRIBUTING.md`
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

Do not hand-edit generated outputs. Do not treat `tmp/`, `artifacts/`, or
archived redirect material as primary contributor guidance.

## Branches and Commits

- Use short descriptive branch names.
- Keep one primary goal per branch.
- Keep commits atomic.
- Include the validation you actually ran.

## Local Checks

Run these before committing:

```sh
npm run build:site
npm run lint
npm run check:md
npm run test:fast
```

## Core Maintainer Checks

- dependency boundaries: `python scripts/check_objc3c_dependency_boundaries.py --strict`
- task hygiene: `python scripts/ci/check_task_hygiene.py`
- docs drift: `python scripts/build_objc3c_native_docs.py --check`
- repo superclean surface: `npm run check:repo:surface`

## PR Expectations

- describe what changed
- describe the validation you ran
- call out risks or non-obvious tradeoffs
- do not hide follow-up work
