# M317 Future Milestone Dependency Rewrites For Post-M292 Work Edge Case And Compatibility Completion Expectations (B003)

Contract ID: `objc3c-cleanup-future-milestone-dependency-rewrites/m317-b003-v1`

## Purpose

Define the machine-auditable post-cleanup dependency rewrites for `M293-M308` so future work consumes the corrective cleanup program instead of recreating checker sprawl, command-surface sprawl, unstable identifiers, or unauthenticated proof artifacts.

## Required outcomes

- Every post-`M292` milestone description explicitly states which cleanup milestones it must consume after the cleanup-first sequence completes.
- Every open issue in `M293-M308` carries a `Post-cleanup dependency rewrite` section with actionable instructions.
- The rewritten future milestones require:
  - `M313` shared acceptance-suite and validation-collapse outputs
  - `M314` compact command-surface and workflow outputs
  - `M315` stable identifiers, provenance, and regeneration rules
  - `M318` anti-noise budgets and exception policy
- Runtime-sensitive future work additionally treats `M316` as the authoritative runtime-truth floor where dispatch, synthesized accessors, or native-output authenticity matter.

## Explicit scope

- Milestones covered: `M293-M308`
- Post-`M292` families covered:
  - `M293-M296` performance
  - `M297-M300` distribution
  - `M301-M304` conformance and external validation
  - `M305-M308` stdlib

## Non-goals

- Do not execute the future milestones.
- Do not create another future-planning scaffold outside the existing `M293-M308` backlog.
- Do not weaken the cleanup-first sequence; this issue sharpens how later work consumes it.

## Acceptance anchors

- The target map must be replayable and machine-auditable.
- The GitHub rewrite application must be idempotent.
- Future issue authoring after this point should have less room to recreate the current noisy patterns.
