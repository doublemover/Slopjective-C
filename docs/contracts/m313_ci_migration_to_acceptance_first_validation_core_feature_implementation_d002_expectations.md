# M313-D002 Expectations

Contract ID: `objc3c-cleanup-ci-acceptance-first-migration/m313-d002-v1`

## Purpose
- Implement the acceptance-first CI runner and dedicated workflow promised by `M313-D001`.
- Emit canonical stage summaries for retained static guards, acceptance suites, compatibility bridges, and the integrated topology run.

## Required truths
- `scripts/m313_acceptance_first_ci_runner.py` supports the frozen stage set.
- `.github/workflows/m313-validation-acceptance-first.yml` runs the stages in the frozen dependency order.
- Canonical summaries are written to:
  - `tmp/reports/m313/ci/static_policy_guards/summary.json`
  - `tmp/reports/m313/ci/acceptance_suites/summary.json`
  - `tmp/reports/m313/ci/compatibility_bridges/summary.json`
  - `tmp/reports/m313/ci/acceptance_first/summary.json`
- The topology summary includes non-quarantined validation counts and active-exception counts so lane `E` can gate without inventing another counting path.

## Closeout notes
- This issue implements the live CI/operator path.
- Next issue: `M313-E001`.
