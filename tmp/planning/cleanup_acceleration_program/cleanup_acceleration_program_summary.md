# Cleanup Acceleration And Runtime Corrective Program

- milestones: 6
- issues: 58
- count snapshot: `tmp/planning/cleanup_acceleration_program/count_snapshot.json`
- prerequisite sequence: `M317 -> {M313, M314, M315 overlap} -> M316 -> M318`
- recommended sequence: `M317 -> {M313, M314, M315 overlap} -> M316 -> M318`

## Why this program exists
- Collapse validation, workflow, and provenance noise before later closure work compounds it.
- Correct the concrete runtime gaps that should not be left vague in later envelope milestones.
- Normalize publication, blocker, and anti-noise mechanics before more backlog is created.

## Notes
- `M317` is an internal-first publication-shaping milestone even though it remains in the draft corpus.
- `M313`, `M314`, and `M315` are allowed to overlap once `M317` is complete.

## Parallel milestone groups
- `M313, M314, M315`

## Publication rules
- Use milestone codes and issue codes as stable source identifiers.
- Accept GitHub-assigned milestone and issue numbers as live publication artifacts; do not predict or preserve desired numeric IDs in source manifests.
- Keep `blocked_by_issue_codes` and GitHub built-in blocker links direct-only.
- Back all count claims with generated snapshots or replayable measurement scripts.
- Canonical policy surface: `tmp/planning/backlog_publication/backlog_publication_policy.json` and `tmp/planning/backlog_publication/backlog_publication_policy.md`.

## Observed repo facts
- `package.json` scripts: 144
- `check_*.py` files under measured roots: 82
- `test_check_*.py` files under measured roots: 0
- `m2xx-*` refs inside `native/objc3c/src`: 0
- tracked `.ll` files: 76
- tracked `*stub*.ll` files: 0
- `compiler/objc3c/semantic.py` present: False

## Milestones
- M317: M317 Backlog publication realignment and supersession hygiene (6 issues)
- M313: M313 Validation architecture, acceptance suites, and checker collapse (11 issues)
- M314: M314 Command-surface reduction, dead-path removal, and workflow simplification (10 issues)
- M315: M315 Source de-scaffolding, stable identifiers, artifact authenticity, and proof hygiene (11 issues)
- M316: M316 Runtime corrective tranche: realized dispatch, synthesized accessors, and native output truth (12 issues)
- M318: M318 Governance hardening, anti-noise budgets, and sustainable progress enforcement (8 issues)

## GitHub publication readiness
- Per-issue draft bodies are generated under `tmp/planning/cleanup_acceleration_program/issues/`.
- Publish-ready JSON bodies are generated under `tmp/github-publish/cleanup_acceleration_program/issue_bodies/`.
- `program_manifest.json` carries future label and blocker metadata for each milestone and issue.
- `dependency_edges.json` is the exact direct blocker graph to translate into GitHub built-in issue dependencies when the backlog is published.
