# M313 Checker Consolidation And Migration Policy Core Feature Implementation Expectations (B003)

Contract ID: `objc3c-cleanup-checker-consolidation-migration-policy/m313-b003-v1`

## Purpose

Freeze the consolidation policy that routes milestone-local checker, readiness, and pytest-wrapper families onto the shared acceptance harness and the executable suite boundaries defined earlier in `M313`.

## Consolidation requirements

- Keep executable acceptance suites and the shared compiler/runtime harness as the preferred source of validation truth.
- Preserve only the retained static-guard classes already frozen in `M313-B001`.
- Route the major migration-only families onto concrete suite targets instead of leaving them as a generic future cleanup promise.
- Require new readiness behavior to delegate through the shared harness instead of minting one-off flows.
- Keep compatibility bridges time-bounded and owned by later `M313` issues instead of treating legacy wrappers as indefinite first-class surfaces.

## Required machine-readable outputs

- consolidation routes keyed to the `M313-A003` suite boundaries
- migration classes and retained static-guard classes
- shared-harness-first migration defaults
- immediate prohibited patterns
- next-issue handoff to `M313-B004`
