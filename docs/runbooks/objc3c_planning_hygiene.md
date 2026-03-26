# Objective-C 3 Planning Hygiene

## Purpose

Use this runbook when creating, amending, or retiring roadmap and conformance issues.
The goal is to keep future work inside the anti-noise budgets instead of recreating
checker sprawl, command sprawl, milestone-coded product strings, or fake-looking proof
surfaces.

## Required declarations

Every new roadmap or conformance issue must state:
- one validation posture;
- one budget-impact class;
- dependencies or `None`;
- bounded implementation surfaces.

Budget-impact classes:
- `no_budget_growth`: the issue only consumes existing surfaces.
- `within_existing_budget`: the issue adds work inside an already-budgeted family.
- `requires_exception_record`: the issue exceeds a budget or adds a normally-prohibited
  surface and must reference an approved exception record.

## Authoring rules

- Prefer shared acceptance suites, shared runners, and durable docs over new
  milestone-local scaffolds.
- Do not add broad `Required deliverables` boilerplate to new issues.
- Do not add new public command entrypoints or milestone-local validation surfaces
  without checking the anti-noise budget policy first.
- If an exception is required, cite the exception record directly in the issue seed or
  amendment.

## Amendment and retirement hygiene

- Amend old issues rather than layering parallel scopes when the correction is a scope
  clarification.
- Supersede only when ownership truly changes.
- Retire temporary surfaces by pointing to the replacement target named in the
  exception record or migration policy.
