# Contributing

This repo is spec-first. Keep contributions small, explicit, and testable.

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
