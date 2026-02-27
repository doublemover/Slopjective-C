# ObjC3C Refactor Workflow (`M132-E001`)

## 1. Purpose

This refactor workflow defines how parallel lanes decompose `native/objc3c/src/main.cpp` work into reviewable slices without merge ambiguity.

## 2. Applicability

- Applies to all M132 refactor issues that touch compiler decomposition, docs, or governance surfaces.
- Applies to lane owners, reviewers, and integration maintainers.

## 3. Parallel-lane operating model

| Role | Required responsibility | Artifact |
| --- | --- | --- |
| Lane owner | Deliver issue-scoped slices and keep dependency state current | Issue updates + PR body |
| Reviewer | Enforce scope, risk, and evidence gates | review checklist in PR |
| Integrator | Sequence dependent merges and resolve lane ordering conflicts | merge notes + follow-up issue |

Parallel-lane rule set:

1. One issue per PR branch.
2. One lane owner per issue.
3. Cross-lane dependencies must be explicit in the PR body before review starts.

## 4. Branch naming and commit conventions

- Branch: `refactor/m132-<lane>-issue-<issue>-<slug>`.
- Commit subject: `[M132-<lane-id>][#<issue>] <imperative summary>`.
- Commits must remain slice-atomic and contain only one behavioral/documentation objective.

## 5. PR slicing policy

PR slicing is required for every refactor lane.

| Slice class | Max files | Max changed lines | Must include |
| --- | --- | --- | --- |
| `docs` | 3 | 500 | issue linkage + validation commands |
| `code+tests` | 10 | 800 | tests, risk notes, rollback impact |
| `plumbing` | 6 | 400 | dependency map + follow-up issues |

If any cap is exceeded, split before requesting review.

## 6. Issue-link requirements

Each PR must contain these fields in the description:

- `Primary issue: #<id>`
- `Packet ID: M132-<lane><nnn>`
- `Dependencies: <none|#id,#id>`
- `Parallel lane impact: <none|lane ids>`
- `Validation: <commands and exit codes>`

## 7. Execution steps (`WF-E001-01`..`WF-E001-08`)

1. `WF-E001-01` Intake: confirm issue scope, dependency IDs, and lane ownership.
2. `WF-E001-02` Branch setup: create branch using the naming contract.
3. `WF-E001-03` Slice planning: split work by objective and ensure each slice respects PR caps.
4. `WF-E001-04` Implementation: keep each commit atomic and issue-linked.
5. `WF-E001-05` Validation: run declared commands and capture exit codes.
6. `WF-E001-06` Review prep: populate PR body required fields and checklist.
7. `WF-E001-07` Review/merge: reviewer verifies checklist; integrator confirms dependency order.
8. `WF-E001-08` Handoff: publish follow-up issues for deferred scope before closeout.

## 8. Reviewer checklist template

Copy into every refactor PR:

- [ ] Primary issue and packet ID are present and match commit subjects.
- [ ] Scope is within PR slicing caps for the declared slice class.
- [ ] Dependencies/parallel lane impact are explicit and current.
- [ ] Validation commands include command text and exit codes.
- [ ] Risk and rollback impact are documented.
- [ ] No hidden follow-up work is required to make the slice safe to merge.

## 9. Merge and hand-off rules

- Merge only when all checklist items pass.
- If dependent lane work is outstanding, label PR as blocked and defer merge.
- Record deferred work as a new issue with owner and target milestone before closeout.

## 10. Validation command

```sh
rg -n "refactor workflow|PR slicing|review checklist" CONTRIBUTING.md docs/refactor/objc3c_refactor_workflow.md
```
