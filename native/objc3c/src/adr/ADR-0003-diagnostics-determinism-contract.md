# ADR-0003: Deterministic Diagnostics Contract

- Status: Accepted
- Date: 2026-02-27
- Deciders: objc3c native maintainers
- Related issues: M132-A001 (#4228), M132-D001 (#4234), M132-D002 (#4235)

## Context

Refactoring the monolith into modules can accidentally change diagnostic order
or wording, which would break baseline parity and downstream tooling.

## Decision

- Preserve stable diagnostic ordering and deterministic formatting across
  extraction milestones.
- Treat diagnostics as stage-owned outputs normalized at pipeline boundaries.
- Require parity and determinism gates before milestone closeout.

## Consequences

- Module extraction work must include parity coverage.
- CI guardrails can reject non-deterministic changes even when compilation
  succeeds.
- Developers can trust drift failures as contract violations, not flaky noise.
