# Contributing

This repo is spec-first. Keep contributions small, explicit, and testable.

## ObjC3C refactor workflow and PR slicing policy

Use this section for M132+ monolith decomposition work. It establishes one refactor workflow for all parallel lanes.

### Branch and commit conventions

- Branch naming: `refactor/m132-<lane>-issue-<issue>-<slug>` (example: `refactor/m132-e-issue-4236-pr-slicing`).
- Keep one primary issue per branch. If a dependency issue is required, record it in the PR body under `Dependencies`.
- Commit subject format: `[M132-<lane-id>][#<issue>] <imperative summary>`.
- Keep commits atomic: one behavior/documentation delta per commit and include matching validation evidence in the same PR.

### Issue-link requirements

- PR description must include `Primary issue: #<id>` and `Packet ID: M132-<lane><nnn>`.
- PR description must include `Parallel lane impact:` with `none` or explicit lane IDs.
- If blocked, mark `Blocked by: #<id>` and do not merge until cleared.

### PR slicing policy

`PR slicing` is mandatory for refactor lanes.

| Slice type | Maximum changed files | Maximum changed lines | Required notes |
| --- | --- | --- | --- |
| Docs-only | 3 | 500 | linked issue, validation command output |
| Compiler + tests | 10 | 800 | risk notes, fallback behavior, test evidence |
| Plumbing/chore | 6 | 400 | dependency map and follow-up issue IDs |

If a change exceeds any cap, split into multiple PRs before review.

### review checklist (required in every refactor PR)

- [ ] Scope maps to exactly one primary issue and packet ID.
- [ ] Dependencies and blocked lanes are listed in PR body.
- [ ] Validation commands and exit codes are included.
- [ ] Risks and rollback/cutover impact are stated.
- [ ] Reviewer can merge without inferring hidden follow-up work.

### Refactor references

- `docs/refactor/objc3c_refactor_workflow.md`
- `docs/refactor/objc3c_refactor_cutover_governance.md`
- `docs/refactor/objc3c_dependency_violation_playbook.md`

## Modular architecture onboarding (M134+)

Use the modular guide when working inside a specific ObjC3C module or subsystem:

- `docs/refactor/objc3c_modular_developer_guide.md`

Quick subsystem entrypoints:

- `npm run dev:objc3c:lex`
- `npm run dev:objc3c:parse`
- `npm run dev:objc3c:sema`
- `npm run dev:objc3c:lower`
- `npm run dev:objc3c:ir`

Always run `npm run check:objc3c:boundaries` before opening or updating a PR that changes module boundaries.

## Local checks

Run these before committing:

```sh
npm run lint:md:all
npm run check:md
```

`lint:md:all` runs:

- stitched spec rebuild (`npm run build:spec`)
- spec structural lint (`python scripts/spec_lint.py`)
- markdown lint (`markdownlint-cli2`)

## Task hygiene checks

Run these when working on planning/task hygiene automation:

```sh
npm run check:task-hygiene
```

`check:task-hygiene` runs:

- `python scripts/spec_lint.py`
- `python scripts/extract_open_issues.py --format json`
- `python scripts/check_issue_checkbox_drift.py`

Release-evidence gate local check:

```sh
npm run check:release-evidence
```

## Issue templates and closeout evidence

When opening execution tasks, use one of:

- `.github/ISSUE_TEMPLATE/roadmap_execution.yml`
- `.github/ISSUE_TEMPLATE/conformance_execution.yml`

When closing execution tasks, follow:

- `spec/planning/ISSUE_CLOSEOUT_EVIDENCE_TEMPLATE.md`

When refreshing roadmap/backlog metrics, follow:

- `spec/planning/ROADMAP_REFRESH_CADENCE_AND_SNAPSHOT_PROTOCOL.md`

## Spec lint rules

`scripts/spec_lint.py` enforces the following in `spec/*.md`:

- unique section numbering within each document
- unique heading anchors within each document
- monotonic top-level section ordering (for numbered `## x.y` headings)
- balanced fenced code blocks
- no dangling list conjunctions (e.g., bullet text ending in `and`/`or` before a heading boundary)
- valid internal cross references (`#anchor` and `file.md#anchor`)

Violations fail CI.

## CI

`Spec Quality` workflow runs on pull requests and pushes to `main` and fails on lint/format violations.
