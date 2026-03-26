# M313 Validation Reduction Targets And Ratchet Map Source Completion Expectations (A002)

Contract ID: `objc3c-cleanup-validation-reduction-ratchet-map/m313-a002-v1`

## Purpose

Freeze concrete reduction targets and ratchet stages for the migration-only validation surfaces identified in `M313-A001`.

## Ratchet requirements

- Carry forward the `M313-A001` measured baselines for:
  - `check_scripts`
  - `readiness_runners`
  - `pytest_check_files`
- Freeze milestone-closeout maximums for migration-only surfaces.
- Freeze intermediate ratchet stages tied to concrete downstream `M313` issues.
- State that new growth on migration-only validation surfaces is blocked unless an explicit exception record exists.
- Keep executable runtime probes and native fixture inputs out of the reduction budget; they are active acceptance inputs, not cleanup targets.

## Required machine-readable outputs

- baseline values imported from `M313-A001`
- intermediate ratchet stages with owner issues
- closeout maximums for migration-only surfaces
- no-growth and exception policy markers
- next-issue handoff to `M313-A003`
