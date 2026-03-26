# M313 Legacy Validation And Namespace Policy Core Feature Implementation Expectations (B004)

Contract ID: `objc3c-cleanup-legacy-validation-quarantine-namespace-policy/m313-b004-v1`

## Purpose

Freeze the namespace quarantine policy that separates active cleanup-era validation surfaces and retained non-milestone policy guards from historical milestone-local wrappers.

## Quarantine requirements

- Treat milestone-coded validation wrappers below the active cleanup window as quarantined migration-only surfaces.
- Treat non-milestone validation surfaces as retained program-level policy guards or operator surfaces unless a later issue retires them explicitly.
- Count only non-quarantined namespaces against the `M313-A002` namespace-quarantine ratchet stage.
- Require exhaustive classification so no checker, readiness runner, or pytest wrapper remains in an ambiguous bucket.
- Keep the active milestone window narrow: only `M313-M318` may remain active in the migration-only namespace after this issue.

## Required machine-readable outputs

- active milestone window
- ratchet stage maximums imported from `M313-A002`
- active and quarantined namespace classification rules
- measured non-quarantined and quarantined counts written under `tmp/reports/`
- next-issue handoff to `M313-B005`
