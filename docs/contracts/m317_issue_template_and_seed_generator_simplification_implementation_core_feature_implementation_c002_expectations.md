# M317 Issue Template And Seed Generator Simplification Implementation Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-cleanup-issue-template-seed-generator-simplification/m317-c002-v1`

## Purpose

Implement the `M317-C001` simplified authoring contract in the actual GitHub issue templates and the local cleanup-program seed generator.

## Required outcomes

- `.github/ISSUE_TEMPLATE/roadmap_execution.yml` matches the lean issue-body contract.
- `.github/ISSUE_TEMPLATE/conformance_execution.yml` matches the lean issue-body contract.
- `tmp/planning/cleanup_acceleration_program/generate_cleanup_acceleration_program.py` emits issue bodies with `## Validation posture` and no universal `## Required deliverables` section.
- Regenerated cleanup-program seed output demonstrates the new shape.

## Non-goals

- Do not rewrite historical GitHub issues here.
- Do not add a new parallel generator stack.
- Do not bring the old boilerplate back through template-specific exceptions.
