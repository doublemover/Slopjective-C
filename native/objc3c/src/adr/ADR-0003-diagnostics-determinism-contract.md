# ADR-0003: Deterministic Diagnostics Contract

- Status: Accepted
- Date: 2026-02-27
- Deciders: objc3c native maintainers
- Related surfaces: `objc3c.frontend.layeredboundaries.v1`, `objc3c.frontend.diagnosticsdeterminism.probe.v1`, `objc3c.frontend.diagnosticsdeterminism.closeout.v1`

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
