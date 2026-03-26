# M313-E001 Expectations

Contract ID: `objc3c-cleanup-validation-noise-reduction-gate/m313-e001-v1`

## Purpose
- Freeze the lane-E gate over the live `M313` validation-consolidation surface.
- Use the `M313-D002` topology summary and canonical suite/bridge outputs as the gate truth surface.

## Required truths
- Gate against the `M313-A002` closeout maximums:
  - `check_scripts <= 558`
  - `readiness_runners <= 179`
  - `pytest_check_files <= 555`
- Require zero active exception records.
- Require all four acceptance suites and all four compatibility bridges to appear in the topology summary.
- Require the bridge deprecation-owner mapping to remain truthful.

## Closeout notes
- The gate is truthful only if it replays the live topology summary.
- Next issue: `M313-E002`.
