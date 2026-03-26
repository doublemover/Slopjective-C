# M317 Planning Packet And Issue Template Simplification Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-cleanup-planning-packet-issue-template-contract/m317-c001-v1`

## Purpose

Freeze a leaner, reusable contract for future roadmap issue bodies and planning packets so later issue seeding stops defaulting to the current verbose boilerplate shape.

## Required outcomes

- Define a canonical simplified roadmap issue-body contract with explicit required and optional sections.
- Define a canonical simplified planning-packet contract with explicit required and optional sections.
- Freeze validation-posture classes so future issue seeds stop assuming every issue needs a dedicated checker, readiness runner, and pytest bundle.
- Define generator-input fields and prohibited defaults for future seeding work.
- Provide concrete simplified template examples that `M317-C002` can implement into generators and GitHub issue templates.

## Core simplification rules

- `## Required deliverables` is no longer universal boilerplate.
- `checker + readiness + pytest` is not the default issue shape.
- Shared acceptance harnesses and consolidated validation surfaces are the default where they already exist.
- Optional issue sections must be explicit and bounded rather than accreting ad hoc.
- Milestone-level context should not be duplicated mechanically into every issue body.

## Execution relationship

- `M317-C001` freezes the contract.
- `M317-C002` implements the contract in generators/templates.
