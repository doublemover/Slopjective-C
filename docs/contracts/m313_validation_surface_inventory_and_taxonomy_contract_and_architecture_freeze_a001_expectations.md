# M313 Validation Surface Inventory And Taxonomy Contract And Architecture Freeze Expectations (A001)

Contract ID: `objc3c-cleanup-validation-surface-inventory-taxonomy/m313-a001-v1`

## Purpose

Freeze the measured validation surface and the taxonomy used to collapse checker-heavy milestone-local validation into a smaller acceptance-first architecture.

## Inventory requirements

- Publish the current measured counts for the primary validation surfaces:
  - `scripts/check_*.py`
  - `scripts/run_*_readiness.py`
  - `tests/tooling/test_check_*.py`
  - `tests/tooling/runtime/*.cpp`
  - `tests/tooling/fixtures/native/**/*.objc3`
- Freeze a taxonomy that distinguishes:
  - retained static policy guards
  - readiness runners
  - issue-local pytest wrappers
  - runtime probes
  - native fixture inputs
  - acceptance-suite roots
  - migration-only validation surfaces
- Freeze namespace intent for:
  - active executable validation surfaces
  - migration-only compatibility surfaces
  - archival or historical surfaces

## Required machine-readable outputs

- measured validation-surface counts
- taxonomy entries with path globs and replacement targets
- namespace policy entries
- next-issue handoff to `M313-A002`
