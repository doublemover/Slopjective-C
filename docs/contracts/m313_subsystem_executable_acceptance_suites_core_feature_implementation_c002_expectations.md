# M313 Subsystem Executable Acceptance Suites Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-cleanup-subsystem-executable-acceptance-suites/m313-c002-v1`

## Purpose

Implement schema-compliant executable suite summaries for the `M313-A003` subsystem boundaries through the shared compiler/runtime acceptance harness.

## Suite requirements

- Reuse `scripts/shared_compiler_runtime_acceptance_harness.py` as the only suite-execution front door.
- Emit `M313-C001` acceptance artifact envelopes for each suite run.
- Write default suite summaries under `tmp/reports/m313/acceptance/<suite_id>/summary.json`.
- Report per-suite fixture-root and probe-root measurements instead of falling back to undifferentiated root checks.
- Keep the suite set aligned to the `M313-A003` boundary map and hand off compatibility-bridge work to `M313-C003`.

## Required machine-readable outputs

- executable suite plan keyed to the four `M313-A003` suites
- harness suite-execution mode and default report roots
- schema-compliant per-suite summaries
- next-issue handoff to `M313-C003`
