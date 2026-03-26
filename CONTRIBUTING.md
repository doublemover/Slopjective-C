# Contributing

Keep changes small, explicit, and testable.

## Branches and Commits

- Use short descriptive branch names.
- Keep one primary goal per branch.
- Keep commits atomic.
- Include the validation you actually ran.

## Local Checks

Run these before committing:

```sh
npm run lint
npm run check:md
```

## Core Maintainer Checks

- dependency boundaries: `python scripts/check_objc3c_dependency_boundaries.py --strict`
- task hygiene: `python scripts/ci/check_task_hygiene.py`
- docs drift: `python scripts/build_objc3c_native_docs.py --check`

## PR Expectations

- describe what changed
- describe the validation you ran
- call out risks or non-obvious tradeoffs
- do not hide follow-up work
