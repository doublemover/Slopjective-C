# Backlog Publication Policy

Policy id: `backlog-publication-policy-v1`
Effective from: `M317`

## Purpose
Define one canonical policy for supersession, direct blockers, publication scope, live GitHub identifier handling, and measured count claims across the draft backlog programs.

## Canonical surfaces
- Draft generation: `tmp/planning/regenerate_draft_programs.py`
- Master snapshot: `tmp/planning/draft_backlog_master_snapshot.json`
- Program manifests: `tmp/github-publish/*/program_manifest.json`
- Blocker graph: `tmp/github-publish/*/dependency_edges.json`

## Stable identifiers vs live GitHub identifiers
- Stable planning identifiers are milestone codes like `M317` and issue codes like `M317-B001`.
- GitHub milestone numbers and issue numbers are live publication artifacts assigned by GitHub.
- Source manifests and draft issue bodies must not depend on predicted or preferred GitHub numbers.
- When publication creates or updates live objects, regenerate the live mapping report and let downstream references follow those assigned numbers.

## Direct blocker policy
- `blocked_by_issue_codes` contains direct blockers only.
- `direct_unblock_issue_codes` contains only the next directly unblocked issue codes.
- `dependency_edges.json` is the canonical publish-time blocker graph.
- Milestone descriptions may carry phase ordering.
- Individual issue bodies must list only direct blockers in `Dependencies` and `Execution Order`.

## Publication scope policy
- `internal-first`
  - backlog-shaping or administrative tranche
  - may exist on GitHub
  - should not be described as end-user feature delivery work
- `public-roadmap`
  - normal externally legible roadmap work
- Current rule: `M317` is internal-first; the rest of the published tranche is public-roadmap unless explicitly changed later.

## Supersession policy
- Every overlap family must name one active owner surface.
- Superseded surfaces may remain in history or archives, but not as canonical inputs.
- Replacement path and reason must be recorded in a generated inventory or policy-backed report.
- Historical artifacts are reference-only and must not be reopened as live planning truth.

## Live GitHub publication policy
- Reuse existing live milestones/issues when the stable code already exists.
- Create new live objects only when no matching stable-code object exists.
- Accept the identifiers GitHub returns.
- Do not attempt to reserve or preserve a desired milestone number or issue number.

## Count and evidence policy
- All count claims must come from generated snapshots or replayable measurement scripts.
- Refresh count snapshots and any derived human summaries in the same rewrite pass.
- Do not publish stale literal counts in summaries, milestone packets, or manifests.

## Immediate follow-on work
- `M317-B002`: rewrite local backlog artifacts to consume this policy.
- `M317-C001`: encode the manifest and artifact contract that makes the policy machine-checkable.
- `M317-D001`: audit generated issue markdown, publish JSON, and blocker graphs against the policy.
