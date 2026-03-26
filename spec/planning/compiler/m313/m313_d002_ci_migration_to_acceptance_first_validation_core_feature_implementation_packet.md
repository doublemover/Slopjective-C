# M313-D002 Planning Packet

## Goal
- Implement the acceptance-first CI runner and dedicated workflow defined by `M313-D001`.

## Scope
- Land `scripts/m313_acceptance_first_ci_runner.py`.
- Land `.github/workflows/m313-validation-acceptance-first.yml`.
- Emit canonical CI summaries for static guards, acceptance suites, compatibility bridges, and the integrated topology run.

## Focus
- Acceptance-first CI execution.
- Canonical topology summary that lane `E` can gate directly.
- No new milestone-local validation namespace outside `tmp/reports/m313/ci/**`.

## Next issue
- Next issue: `M313-E001`.
