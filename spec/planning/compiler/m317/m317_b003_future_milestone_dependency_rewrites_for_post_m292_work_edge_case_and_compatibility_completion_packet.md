# M317-B003 Packet: Future milestone dependency rewrites for post-M292 work - Edge case and compatibility completion

## Intent

Execute the post-`M292` backlog rewrite that makes `M293-M308` consume the cleanup-first program explicitly instead of treating cleanup as a vague prerequisite.

## Why this issue exists

The current post-`M292` milestones are blocked by the cleanup-first sequence, but that global blocker is still too weak on its own:

- it does not say which cleanup outputs each milestone must reuse
- it leaves room for new checker/readiness/npm-script sprawl after cleanup completes
- it does not force future proof, provenance, and stable-identifier rules into later issue bodies

`M317-B003` closes that gap.

## Contract

- Source of truth:
  - `docs/contracts/m317_future_milestone_dependency_rewrites_for_post_m292_work_edge_case_and_compatibility_completion_b003_expectations.md`
  - `spec/planning/compiler/m317/m317_b003_future_milestone_dependency_rewrites_for_post_m292_work_edge_case_and_compatibility_completion_targets.json`
- GitHub apply surface:
  - `tmp/github-publish/cleanup_acceleration_program/apply_m317_b003_future_dependency_rewrites.py`
- Verification:
  - `scripts/check_m317_b003_future_milestone_dependency_rewrites_for_post_m292_work.py`
  - `tests/tooling/test_check_m317_b003_future_milestone_dependency_rewrites_for_post_m292_work.py`
  - `scripts/run_m317_b003_lane_b_readiness.py`

## Implementation notes

- Rewrite all milestone descriptions for `M293-M308` with explicit `Corrective dependencies consumed:` sections.
- Rewrite all open issue bodies in `M293-M308` with explicit `## Post-cleanup dependency rewrite` sections.
- Keep the updates family-specific:
  - performance
  - distribution
  - conformance
  - stdlib
- Make the apply path idempotent.
- Preserve the existing execution-order sections and labels.

## Closeout evidence

- Apply report under `tmp/reports/m317/M317-B003/`
- Checker summary under `tmp/reports/m317/M317-B003/`
- Live GitHub probe proving all targeted milestone descriptions and issue bodies contain the required dependency-rewrite snippets

## Next issue

- Next issue: `M317-C001`.
