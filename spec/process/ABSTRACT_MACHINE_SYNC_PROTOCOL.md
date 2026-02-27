# Abstract Machine Sync Protocol (`v0.11-A03`) {#am-sync-protocol}

_Working draft v0.11 - last updated 2026-02-23_

This protocol closes issue `#117` and defines the merge gate that keeps
`spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md` synchronized with Parts 3/6/7/8.

## Scope and applicability

This protocol applies to every pull request that changes normative behavior in
any of these files:

- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
  - `AM.2.2` (`#am-2-2`), `AM.4.4` (`#am-4-4`), `AM.5` (`#am-5`),
    `AM.6.1`-`AM.6.7` (`#am-6-1`..`#am-6-7`), `AM.7` (`#am-7`)
- `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`
  - `3.4.1` (`#part-3-4-1`), `3.4.2` (`#part-3-4-2`),
    `3.4.5` (`#part-3-4-5`)
- `spec/PART_6_ERRORS_RESULTS_THROWS.md`
  - `6.3` (`#part-6-3`), `6.4` (`#part-6-4`), `6.5` (`#part-6-5`),
    `6.6` (`#part-6-6`)
- `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md`
  - `7.3.3` (`#part-7-3-3`), `7.6` (`#part-7-6`),
    `7.9.1`-`7.9.4` (`#part-7-9-1`..`#part-7-9-4`)
- `spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md`
  - `8.1` (`#part-8-1`), `8.2.3` (`#part-8-2-3`), `8.3` (`#part-8-3`),
    `8.6` (`#part-8-6`)

For this protocol, a "normative behavior change" means any edit that changes:

- required sequencing/evaluation order,
- `shall`/`must`/`ill-formed` conditions,
- unwind/cleanup/lifetime guarantees,
- required diagnostics or conformance matrix expectations.

Pure editorial edits (typos, wording clarity, formatting) do not require full
sync review if they do not change the four items above.

## Trigger matrix (what changes require sync review)

| Trigger | Typical source sections | Required sync actions | Required approvals |
| --- | --- | --- | --- |
| Optional short-circuit semantics change (`?.`, optional send, nil-path argument suppression, scalar/struct restrictions) | Part 3 `3.4.1`, `3.4.2`, `3.4.5` | Update AM sequencing text in `AM.2.2`, composed-order text in `AM.6.6`/`AM.6.7`, and AM matrix rows `AM-T09`, `AM-T10`, `AM-T11`, `AM-T15`, `AM-T18`, `AM-T19`. | AM owner + Part 3 owner |
| `throws` / `try` / postfix propagation `?` behavior change | Part 6 `6.3`, `6.4`, `6.5`, `6.6` | Update AM error/exit rules in `AM.4.2`, `AM.6.4`, `AM.6.5`, `AM.6.7`; keep carrier restrictions aligned; update AM rows `AM-T04`, `AM-T05`, `AM-T06`, `AM-T07`, `AM-T08`, `AM-T16`, `AM-T17`. | AM owner + Part 6 owner |
| `await` suspension, cancellation unwind, actor/executor hop, or autorelease slicing change | Part 7 `7.3.3`, `7.6`, `7.9.1`-`7.9.4` | Update AM suspension/lifetime rules in `AM.2.3`, `AM.3.3`, `AM.6.1`-`AM.6.4`, `AM.6.7`; update AM rows `AM-T04`, `AM-T12`, `AM-T14`, `AM-T15`-`AM-T19`. | AM owner + Part 7 owner |
| Scope-exit/cleanup/resource/lifetime-control ordering change | Part 8 `8.1`, `8.2.3`, `8.3`, `8.6` | Update AM cleanup algorithm in `AM.1.2`, `AM.4`, `AM.5`, `AM.6.1`-`AM.6.3`; keep ARC-after-cleanup rule consistent; update AM rows `AM-T01`-`AM-T04`, `AM-T12`, `AM-T13`, `AM-T15`-`AM-T19`. | AM owner + Part 8 owner |
| AM core algorithm or AM matrix row change | AM `AM.2`-`AM.7` | Push equivalent updates to every impacted Part section above and add/adjust cross-links in the edited Part file(s). | AM owner + owner of each impacted Part |
| Canonical-home move for a cross-cutting rule | Any of the rows above | Update `spec/CROSS_CUTTING_RULE_INDEX.md` in the same PR so canonical and non-canonical homes match. | AM owner + owner of moved canonical section |

No PR touching a triggered row merges until required sync actions are complete in
the same PR.

## Owners and review roles

Roles are assigned per PR with deterministic commands:

- AM owner:
  - `git log -1 --format="%an <%ae>" -- spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- Part owners:
  - `git log -1 --format="%an <%ae>" -- spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`
  - `git log -1 --format="%an <%ae>" -- spec/PART_6_ERRORS_RESULTS_THROWS.md`
  - `git log -1 --format="%an <%ae>" -- spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md`
  - `git log -1 --format="%an <%ae>" -- spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md`
- Conformance owner (matrix/diagnostics impact gate):
  - `git log -1 --format="%an <%ae>" -- spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md`
- Governance backup owner:
  - `git log -1 --format="%an <%ae>" -- spec/IMPLEMENTATION_EXECUTION_ROADMAP.md`

Review duties:

- PR author:
  - runs the checklist below before requesting review,
  - maps each changed rule to its canonical home,
  - links every updated AM rule to matching Part section updates.
- AM owner:
  - validates abstract-machine ordering, unwind, and matrix consistency.
- Impacted Part owner(s):
  - validate that Part-local normative language matches AM behavior.
- Conformance owner:
  - required when AM matrix rows or required diagnostics change.
- Governance backup owner:
  - resolves stalled conflicts per escalation path.

Approval gate:

- Minimum approvals: AM owner plus every impacted Part owner.
- If matrix/diagnostics changed: add Conformance owner approval.
- If one person occupies multiple required roles: add one extra reviewer from
  any untouched required role so at least two people approve.

## PR-time drift checklist (runnable steps)

Run from repository root.

1. Detect whether this PR touches sync-governed files.

```sh
git fetch origin main --quiet
git diff --name-only origin/main...HEAD -- spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md spec/PART_6_ERRORS_RESULTS_THROWS.md spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md spec/CROSS_CUTTING_RULE_INDEX.md
```

1. If the command in step 1 prints no path, mark "no sync trigger" and stop.
   If it prints any path, continue.

1. Resolve owners and request the required reviewers.

```sh
git log -1 --format="AM owner: %an <%ae>" -- spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md
git log -1 --format="Part 3 owner: %an <%ae>" -- spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md
git log -1 --format="Part 6 owner: %an <%ae>" -- spec/PART_6_ERRORS_RESULTS_THROWS.md
git log -1 --format="Part 7 owner: %an <%ae>" -- spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md
git log -1 --format="Part 8 owner: %an <%ae>" -- spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md
git log -1 --format="Conformance owner: %an <%ae>" -- spec/PART_12_DIAGNOSTICS_TOOLING_TESTS.md
```

1. Inspect semantic diffs only.

```sh
git diff origin/main...HEAD -- spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md spec/PART_6_ERRORS_RESULTS_THROWS.md spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md
```

1. Verify required anchors still exist after edits.

```sh
rg -n "{#am-2-2}|{#am-4-4}|{#am-5}|{#am-6-4}|{#am-6-5}|{#am-6-6}|{#am-6-7}|{#am-7-1}" spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md
rg -n "{#part-3-4-1}|{#part-3-4-2}|{#part-3-4-5}" spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md
rg -n "{#part-6-3}|{#part-6-4}|{#part-6-5}|{#part-6-6}" spec/PART_6_ERRORS_RESULTS_THROWS.md
rg -n "{#part-7-3-3}|{#part-7-6}|{#part-7-9-1}|{#part-7-9-2}|{#part-7-9-3}|{#part-7-9-4}" spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md
rg -n "{#part-8-1}|{#part-8-2-3}|{#part-8-3}|{#part-8-6}" spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md
```

1. Verify AM still references Parts 3/6/7/8 and keeps matrix coverage.

```sh
rg -n "Part 3|Part 6|Part 7|Part 8|AM-T0[1-9]|AM-T1[0-9]" spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md
```

1. If canonical-home text moved, verify index update.

```sh
rg -n "defer|cleanup|await|optional|propagation|throws" spec/CROSS_CUTTING_RULE_INDEX.md
```

1. Add a PR note with:
   - changed rule(s),
   - canonical normative home,
   - mirrored section updates,
   - AM matrix rows added/changed,
   - conflict class (`normative conflict`, `editorial mismatch`, or
     `missing example`) if any mismatch remains.

Merge is blocked until all triggered items above are complete.

## Cadence and reporting

- Per-PR: run the checklist before review request on every triggered PR.
- Weekly cadence: every Monday by 18:00 UTC, AM owner posts a sync report in
  issue `#117` (or its successor tracking issue) containing:
  - triggered PRs merged in the last 7 days,
  - open mismatches by class (`normative conflict`, `editorial mismatch`,
    `missing example`),
  - owner and target date for each open item,
  - whether escalation was used.
- Batch cadence: whenever issue state changes are merged, keep
  `spec/IMPLEMENTATION_EXECUTION_ROADMAP.md` and
  `spec/EXECUTION_MICROTASK_BACKLOG.md` synchronized in the same batch, per
  roadmap section `IR.7`.
- Release gate cadence: run one full sync audit before any `v0.11` release-candidate
  tag and again before final release; unresolved normative conflicts are
  release blockers.

## Escalation path for semantic conflicts

1. Detect and classify.
   - Any reviewer who sees a mismatch labels it as one of:
     `normative conflict`, `editorial mismatch`, `missing example`.
   - `normative conflict` immediately blocks merge.
1. Open a conflict record in the PR description.
   - Include file + section IDs (for example `AM.6.5` vs `Part 6.6`),
     proposed canonical home, and impacted AM test rows.
1. First resolution window (`T+24h`).
   - AM owner and impacted Part owner produce one reconciled wording proposal.
1. Second resolution window (`T+48h`).
   - If unresolved, Conformance owner decides whether matrix/diagnostic coverage
     is currently invalid and marks required test/doc deltas.
1. Governance decision (`T+72h`).
   - If still unresolved, Governance backup owner makes a tie-break decision
     and blocks merges to the conflicting sections until a reconciliation PR is
     merged.
1. Closure conditions.
   - Conflict closes only when:
     - AM and impacted Part text match semantically,
     - AM matrix rows are updated (or explicitly unchanged with rationale),
     - checklist steps pass,
     - conflict classification is removed from the PR.

This escalation path is mandatory for any conflict that could alter program
behavior, diagnostics, or conformance outcomes.
