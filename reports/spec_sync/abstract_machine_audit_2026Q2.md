# Abstract Machine Consistency Audit 2026Q2

## Metadata

- Audit timestamp (UTC): `2026-02-23T18:36:02Z`
- Baseline commit SHA: `ad8bb135c7a05172199297f7f2e6d3ad6cab5ded`
- Protocol version: `v0.11-A03` (`spec/process/ABSTRACT_MACHINE_SYNC_PROTOCOL.md`)
- Audit scope: Parts `0/3/6/7/8` plus AM core synchronization surface
- Auditor: `worker-lane-A (issue #784, V013-SPEC-03)`

## Reconciled baseline import (`V013-SPEC-02`)

Imported from `spec/planning/v013_seed_source_reconciliation_package.md`:

- `contract_id = V013-SPEC-02-RECON-v1`
- `normalized_topics.part_open_issue_status = {part_0: closed, part_3: closed, part_10: closed}`
- `consumer binding`: `V013-SPEC-03` consumes
  `normalized_topics.part_open_issue_status` and `conflict_decisions`
- `REL-03 transitive path`: `V013-SPEC-02 -> V013-SPEC-03 -> V013-REL-03`

Interpretation rule for this audit refresh:

1. Drift rows here are treated as synchronization debt, not as reopening Part 0/3/10
   open-issue sections.
2. Release consumers use this report as the current AM-sync baseline for
   `EDGE-V013-017`.

## Source inventory used for 2026Q2 baseline

| Source ID | Artifact path | Signal consumed |
| --- | --- | --- |
| `SRC-V013-01` | `spec/FUTURE_WORK_V011.md` | Original AM sync task contract (`A-03`, `A-04`) and delta classification requirement. |
| `SRC-V013-02` | `spec/IMPLEMENTATION_EXECUTION_ROADMAP.md` | Active W1 execution context and active batch snapshot (`BATCH-20260223-11A`). |
| `SRC-V013-12` | `spec/planning/v013_future_work_seed_matrix.md` | Seed contract for `V013-SPEC-03`, `AC-V013-SPEC-03`, and `EDGE-V013-017` to `REL-03`. |
| `RSRC-12/13` | `spec/planning/v013_seed_source_reconciliation_package.md` | Reconciled dependency binding and conflict decisions consumed by this audit run. |
| `AM-Q1` | `reports/spec_sync/abstract_machine_audit_2026Q1.md` | Prior baseline for delta-to-delta comparison and carry-forward tracking. |

## Anchor verification snapshot

| File | Command | Result |
| --- | --- | --- |
| `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md` | `rg -N "\{#am-2-2\}|\{#am-4-4\}|\{#am-5\}|\{#am-6-1\}|\{#am-6-2\}|\{#am-6-3\}|\{#am-6-4\}|\{#am-6-5\}|\{#am-6-6\}|\{#am-6-7\}|\{#am-7\}"` | `PASS AM count=11 expected=11` |
| `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md` | `rg -N "\{#part-0-1\}|\{#part-0-2-1\}|\{#part-0-2-2\}|\{#part-0-2-4\}|\{#part-0-3-1\}|\{#part-0-3-2\}|\{#part-0-4-1\}|\{#part-0-4-6\}|\{#part-0-4-14\}|\{#part-0-4-16\}|\{#part-0-4-17\}|\{#part-0-4-18\}|\{#part-0-4-19\}|\{#part-0-4-20\}|\{#part-0-6\}"` | `PASS Part0 count=15 expected=15` |
| `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md` | `rg -N "\{#part-3-4-1\}|\{#part-3-4-2\}|\{#part-3-4-5\}"` | `PASS Part3 count=3 expected=3` |
| `spec/PART_6_ERRORS_RESULTS_THROWS.md` | `rg -N "\{#part-6-3\}|\{#part-6-4\}|\{#part-6-5\}|\{#part-6-6\}"` | `PASS Part6 count=4 expected=4` |
| `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md` | `rg -N "\{#part-7-3-3\}|\{#part-7-6\}|\{#part-7-9-1\}|\{#part-7-9-2\}|\{#part-7-9-3\}|\{#part-7-9-4\}"` | `PASS Part7 count=6 expected=6` |
| `spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md` | `rg -N "\{#part-8-1\}|\{#part-8-2-3\}|\{#part-8-3\}|\{#part-8-6\}"` | `PASS Part8 count=4 expected=4` |
| AM matrix rows | `rg -n "AM-T0[1-9]|AM-T1[0-9]|Part 3|Part 6|Part 7|Part 8" spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md` | `PASS rows AM-T01..AM-T19 and cross-part links present` |

## Drift findings table

| drift_id | class | canonical_home | impacted_sections | am_matrix_rows | rel03_impact | summary | owner | target_date | status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `AM-AUDIT-2026Q2-01` | `normative conflict` | Part 6 (`#part-6-6`) | `#am-6-5`, `#part-6-6-1`, `#part-6-6-4` | `AM-T07`, `AM-T08`, `AM-T13` | `blocking` | Part 6 still states `T?` propagation may "early-return nil (or throw)" while AM + Part 6.6.4 require optional propagation to be optional-return-only and ill-formed in `throws`/`Result` contexts. | `doublemover <153689082+doublemover@users.noreply.github.com>` | `2026-03-15` | `open` |
| `AM-AUDIT-2026Q2-02` | `missing example` | Part 7 (`#part-7-6-5`) | `#part-7-6-5`, `#part-8-3`, `#part-8-6` | `AM-T12`, `AM-T14` | `advisory` | Normative cancellation unwind guarantees exist, but no worked cross-part example combines cancellation-driven unwind with `@cleanup` or `@resource` and explicit lifetime controls (`withLifetime` or `keepAlive`). | `doublemover <153689082+doublemover@users.noreply.github.com>` | `2026-03-22` | `tracked` |
| `AM-AUDIT-2026Q2-03` | `editorial mismatch` | Part 0 (`#part-0-4-1`) | `#part-0-4-1`, `#part-3-4-5` | `AM-T09` | `advisory` | Part 0 requires exact phrase `ill-formed; diagnostic required`; Part 3 restriction bullets still use shortened `ill-formed` wording. | `doublemover <153689082+doublemover@users.noreply.github.com>` | `2026-03-29` | `tracked` |

## Detailed drift records

### AM-AUDIT-2026Q2-01

- Class: `normative conflict`
- Canonical home: `spec/PART_6_ERRORS_RESULTS_THROWS.md#part-6-6`
- Evidence:
  - `spec/PART_6_ERRORS_RESULTS_THROWS.md:294`:
    `T?`: unwrap or early-return `nil` (or throw) depending on surrounding function's declared return/effects.
  - `spec/PART_6_ERRORS_RESULTS_THROWS.md:337`:
    If the enclosing function is `throws` or returns `Result<…>`, use of `e?` is
    `ill-formed` in v1.
  - `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md:242`:
    using optional propagation to implicitly map into `throws` or `Result` contexts is ill-formed in v1.
- Reconciliation direction:
  - Keep AM + Part 6.6.4 carrier rule as canonical.
  - Normalize Part 6.6.1 wording to remove "or throw" ambiguity for optional
    propagation.

### AM-AUDIT-2026Q2-02

- Class: `missing example`
- Canonical home: `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md#part-7-6-5`
- Evidence:
  - `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md:402`:
    `defer` handlers shall execute exactly once, in LIFO order, when scope exit
    occurs due to cancellation-driven unwind.
  - `spec/PART_7_CONCURRENCY_ASYNC_AWAIT_ACTORS.md:404`:
    async-frame captured objects retained across suspension are released when the
    frame is destroyed, including cancellation unwind.
  - `spec/PART_8_SYSTEM_PROGRAMMING_EXTENSIONS.md:38` and `:291`:
    cleanup-scope and `keepAlive` controls are defined, but not shown in a shared
    cancellation-path worked example.
- Reconciliation direction:
  - Add one worked example combining `try await` cancellation path with
    `defer` + `@cleanup` or `@resource` + `keepAlive`/`withLifetime`.
  - Map example expectations explicitly to `AM-T12` and `AM-T14`.

### AM-AUDIT-2026Q2-03

- Class: `editorial mismatch`
- Canonical home: `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md#part-0-4-1`
- Evidence:
  - `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md:96-97`:
    preferred exact phrase is `ill-formed; diagnostic required`.
  - `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md:391` and `:444`:
    prohibitions use `ill-formed` without exact phrase.
- Reconciliation direction:
  - Normalize Part 3 prohibition bullets to include exact phrase while preserving
    strict/permissive profile distinctions.

## REL-03 impact snapshot (`EDGE-V013-017`)

| rel_dependency | requirement | impacted_drift_ids | current risk | release-note requirement |
| --- | --- | --- | --- | --- |
| `EDGE-V013-017` (`V013-SPEC-03 -> V013-REL-03`) | Kickoff packet must cite current abstract-machine sync status. | `AM-AUDIT-2026Q2-01`, `AM-AUDIT-2026Q2-02`, `AM-AUDIT-2026Q2-03` | `medium` (`1` blocking + `2` advisory findings) | `V013-REL-03` must cite this report and include unresolved drift IDs/statuses in kickoff handoff. |

## Deterministic remediation priority order

Priority scoring model:

- `class_weight`: `normative conflict=3`, `missing example=2`, `editorial mismatch=1`
- `rel03_weight`: `blocking=2`, `advisory=1`
- `am_row_count`: number of AM matrix rows directly touched by the finding
- `priority_score = (100 * class_weight) + (10 * rel03_weight) + am_row_count`

| priority_rank | drift_id | class_weight | rel03_weight | am_row_count | priority_score | deterministic_action |
| --- | --- | ---: | ---: | ---: | ---: | --- |
| `1` | `AM-AUDIT-2026Q2-01` | 3 | 2 | 3 | 323 | Patch Part 6 wording in `6.6.1` to remove optional-propagation throw ambiguity. |
| `2` | `AM-AUDIT-2026Q2-02` | 2 | 1 | 2 | 212 | Add one cross-part cancellation + cleanup + lifetime control worked example and AM mapping notes. |
| `3` | `AM-AUDIT-2026Q2-03` | 1 | 1 | 1 | 111 | Normalize Part 3 prohibition wording to exact Part 0 phrase standard. |

Tie-break rule: if scores are equal, sort by lexical `drift_id`.

## Validation transcript

| Command | Output | Result |
| --- | --- | --- |
| `python scripts/spec_lint.py` | `spec-lint: OK` | `PASS` |

## Open blockers

- `unresolved normative conflict count = 1` (`AM-AUDIT-2026Q2-01`)
- Escalation target: resolve before `V013-REL-03` kickoff packet publication.
