# M313 Shared Compiler Runtime Integration Harness Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-cleanup-shared-compiler-runtime-harness/m313-b002-v1`

## Purpose

Implement one shared compiler/runtime acceptance harness that can enumerate the `M313-A003` subsystem suites, expose a stable CLI, and validate suite roots without creating new milestone-local validation namespaces.

## Harness requirements

- Load a machine-readable suite registry derived from `M313-A003`.
- Expose a stable CLI that can:
  - list suites
  - show suite metadata
  - validate suite roots
- Keep the harness scoped to shared suite orchestration and root validation in this issue; later `M313` issues can add migration policy, quarantine, compatibility bridges, and CI integration on top of it.
- Reuse the active acceptance roots from `M313-A001/A003` instead of inventing new roots.

## Required machine-readable outputs

- shared suite registry
- stable harness CLI modes
- root-validation summary output
- next-issue handoff to `M313-B003`
