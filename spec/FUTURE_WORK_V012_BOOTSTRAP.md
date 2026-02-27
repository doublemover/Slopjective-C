# Objective-C 3.0 - Future Work Plan v0.12 Bootstrap (Lane D Shard: planning-issue-191) {#future-work-v012-bootstrap}

_Final draft v0.12 - 2026-02-23_

Status: published kickoff artifact for issue `#191` (`v0.11-FW-D14`) scoped
to Lane D shard `planning-issue-191`.

## 1. Cycle Header and Baseline

| Metadata field | Value |
| --- | --- |
| `cycle_label` | `v0.12` |
| `source_cycle` | `v0.11` |
| `publication_date` | `2026-02-23` |
| `source_revision` | `fe9aedb76a9c2a07ef8d5dbe24178d4a1d1c15ab` |
| `carryover_source_ref` | `spec/planning/issue_190_carryover_descoping_package.md` |
| `dependency_map_source_ref` | `spec/planning/future_work_v011_dependency_map.md` |
| `published_by` | `D-LEAD` |
| `approved_by` | `D-LEAD`, `A-LEAD`, `B-LEAD`, `C-LEAD` |

Scope baseline for this bootstrap artifact:

- Includes only shard `planning-issue-191` carryover tasks from the remaining
  task review catalog.
- Uses `D-13` governance constraints from
  `spec/planning/issue_190_carryover_descoping_package.md`.
- Uses `D-12` risk framing from
  `spec/planning/issue_189_readiness_dossier_package.md`.

## 2. Approved Carryover Scope

Approved carryover set for this shard is limited to eight `SPT-*` tasks tied to
issue `#191` checklist closure.

| task_id | source_row | classification | rationale_code | dependency_implication | risk_score | owner_primary | owner_backup | target_cycle |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `SPT-0497` | `remaining_task_review_catalog` | `carryover` | `R-CRIT-PATH` | `DEP-BLOCKS-DOWNSTREAM` | `12` | `D-LEAD` | `C-BACKUP` | `v0.12-W1` |
| `SPT-0498` | `remaining_task_review_catalog` | `carryover` | `R-CRIT-PATH` | `DEP-BLOCKS-DOWNSTREAM` | `10` | `D-LEAD` | `C-BACKUP` | `v0.12-W1` |
| `SPT-0499` | `remaining_task_review_catalog` | `carryover` | `R-CRIT-PATH` | `DEP-BLOCKS-DOWNSTREAM` | `12` | `D-LEAD` | `C-BACKUP` | `v0.12-W1` |
| `SPT-0500` | `remaining_task_review_catalog` | `carryover` | `R-RISK-BOUND` | `DEP-BLOCKS-DOWNSTREAM` | `9` | `D-LEAD` | `C-BACKUP` | `v0.12-W1` |
| `SPT-0501` | `remaining_task_review_catalog` | `carryover` | `R-RISK-BOUND` | `DEP-NO-IMMEDIATE-BLOCK` | `8` | `D-LEAD` | `C-BACKUP` | `v0.12-W2` |
| `SPT-0502` | `remaining_task_review_catalog` | `carryover` | `R-CRIT-PATH` | `DEP-BLOCKS-DOWNSTREAM` | `11` | `D-LEAD` | `C-BACKUP` | `v0.12-W2` |
| `SPT-0503` | `remaining_task_review_catalog` | `carryover` | `R-RISK-BOUND` | `DEP-BLOCKS-DOWNSTREAM` | `15` | `D-LEAD` | `C-BACKUP` | `v0.12-W2` |
| `SPT-0504` | `remaining_task_review_catalog` | `carryover` | `R-CRIT-PATH` | `DEP-BLOCKS-DOWNSTREAM` | `10` | `D-LEAD` | `C-BACKUP` | `v0.12-W3+` |

Classification notes:

- No `defer` rows are present in this shard publication.
- No `drop` rows are present in this shard publication.
- All carryover rows retain `D-LEAD` ownership continuity with `C-BACKUP`
  coverage.

## 3. Prioritization Method and Scoring Summary

Priority scoring model:

`priority_score = (5*cpi) + (4*duv) + (3*rbv) + (2*rc) + (2*ecp) - (2*dc)`

Tier bands:

- `Tier-0`: `>= 55`
- `Tier-1`: `40..54`
- `Tier-2`: `25..39`
- `Tier-3`: `< 25`

| task_id | cpi | duv | rbv | rc | ecp | dc | priority_score | priority_tier | tie_break_trace |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `SPT-0503` | `5` | `4` | `5` | `5` | `5` | `1` | `74` | `Tier-0` | Unique score, tie-break not invoked. |
| `SPT-0497` | `5` | `5` | `4` | `4` | `4` | `2` | `69` | `Tier-0` | Unique score, tie-break not invoked. |
| `SPT-0499` | `5` | `5` | `4` | `4` | `4` | `3` | `67` | `Tier-0` | Unique score, tie-break not invoked. |
| `SPT-0498` | `5` | `4` | `4` | `4` | `4` | `2` | `65` | `Tier-0` | Unique score, tie-break not invoked. |
| `SPT-0502` | `5` | `4` | `4` | `4` | `3` | `2` | `63` | `Tier-0` | Unique score, tie-break not invoked. |
| `SPT-0504` | `4` | `3` | `3` | `5` | `5` | `1` | `59` | `Tier-0` | Unique score, tie-break not invoked. |
| `SPT-0500` | `4` | `4` | `3` | `4` | `3` | `1` | `57` | `Tier-0` | Unique score, tie-break not invoked. |
| `SPT-0501` | `4` | `3` | `3` | `4` | `3` | `1` | `53` | `Tier-1` | Unique score, tie-break not invoked. |

Prioritization outcome:

- All `Tier-0` tasks are scheduled in `W1` or in `W2/W3+` only when blocked by
  hard predecessors.
- One `Tier-1` task (`SPT-0501`) is placed in `W2` because it is handoff-facing
  and depends on baseline structure publication.

## 4. Dependency Map and Critical Path

Dependency integration summary:

- Baseline edge semantics imported from
  `spec/planning/future_work_v011_dependency_map.md`.
- Carryover node set filtered to shard tasks `SPT-0497` through `SPT-0504`.
- Hard predecessors normalized to `all-satisfied` or `partial`.
- Cycle check result: no cycles detected in this carryover DAG.

| task_id | priority_tier | hard_predecessors | soft_predecessors | predecessor_state | successor_count | critical_path_flag | planned_window |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `SPT-0497` | `Tier-0` | `none` | `none` | `all-satisfied` | `4` | `yes` | `W1` |
| `SPT-0498` | `Tier-0` | `SPT-0497` | `none` | `partial` | `1` | `no` | `W1` |
| `SPT-0499` | `Tier-0` | `SPT-0497` | `none` | `partial` | `1` | `yes` | `W1` |
| `SPT-0500` | `Tier-0` | `SPT-0497` | `SPT-0498` | `partial` | `1` | `no` | `W1` |
| `SPT-0501` | `Tier-1` | `SPT-0497` | `none` | `partial` | `1` | `no` | `W2` |
| `SPT-0502` | `Tier-0` | `SPT-0498`, `SPT-0499` | `none` | `partial` | `1` | `yes` | `W2` |
| `SPT-0503` | `Tier-0` | `SPT-0500`, `SPT-0502` | `none` | `partial` | `1` | `yes` | `W2` |
| `SPT-0504` | `Tier-0` | `SPT-0501`, `SPT-0503` | `none` | `partial` | `0` | `yes` | `W3+` |

Critical-path recomputation:

1. Longest hard-dependency chain: `SPT-0497 -> SPT-0499 -> SPT-0502 ->
   SPT-0503 -> SPT-0504`.
2. Near-critical chain (one task from longest): `SPT-0497 -> SPT-0498 ->
   SPT-0502 -> SPT-0503 -> SPT-0504`.
3. Chain delta vs previous baseline: kickoff shard chain extends the previous
   three-stage Wave 1 chain with explicit publication/handoff closure tasks.

Dependency conflict scan:

- `DC-1` missing hard predecessor: none.
- `DC-2` carryover/defer mismatch: none in this shard.
- `DC-3` ownership mismatch: none (`D-LEAD` primary, `C-BACKUP` backup).

## 5. Execution Batches and Gate Sequence

`T` is publication timestamp `2026-02-23T16:00:00Z`.

| Window | Entry gate | Planned tasks | Exit gate |
| --- | --- | --- | --- |
| `W1` (`T` to `T+24h`) | Carryover set approved and scored. | `SPT-0497`, `SPT-0498`, `SPT-0499`, `SPT-0500` | Structure, scoring, dependency, and `KC-*` contracts are complete. |
| `W2` (`T+24h` to `T+48h`) | `W1` evidence accepted. | `SPT-0501`, `SPT-0502`, `SPT-0503` | Handoff protocol, done-criteria map, and lint validation are complete. |
| `W3+` (`T+48h` to `T+72h`) | `W2` validation passes and sign-off acknowledged. | `SPT-0504` | Closeout report includes artifact paths, source revision, and validation outputs. |

Gate policy:

- Any failed `KC-*` check blocks movement to the next window.
- Any unresolved hard predecessor moves affected tasks to `blocked` and
  requires explicit escalation in checklist sign-off.

## 6. Ownership and SLA Mapping

Ownership baseline is inherited from
`spec/planning/issue_159_future_work_ownership_matrix_package.md` (`D-14`
row).

| task_id | primary_owner | backup_owner | review_sla | handoff_contact | acknowledgement_state |
| --- | --- | --- | --- | --- | --- |
| `SPT-0497` | `D-LEAD` | `C-BACKUP` | `SLA-2BD` | `A-LEAD`, `B-LEAD`, `C-LEAD` | `recorded` |
| `SPT-0498` | `D-LEAD` | `C-BACKUP` | `SLA-2BD` | `A-LEAD`, `B-LEAD`, `C-LEAD` | `recorded` |
| `SPT-0499` | `D-LEAD` | `C-BACKUP` | `SLA-2BD` | `A-LEAD`, `B-LEAD`, `C-LEAD` | `recorded` |
| `SPT-0500` | `D-LEAD` | `C-BACKUP` | `SLA-2BD` | `A-LEAD`, `B-LEAD`, `C-LEAD` | `recorded` |
| `SPT-0501` | `D-LEAD` | `C-BACKUP` | `SLA-2BD` | `A-LEAD`, `B-LEAD`, `C-LEAD` | `recorded` |
| `SPT-0502` | `D-LEAD` | `C-BACKUP` | `SLA-2BD` | `A-LEAD`, `B-LEAD`, `C-LEAD` | `recorded` |
| `SPT-0503` | `D-LEAD` | `C-BACKUP` | `SLA-2BD` | `A-LEAD`, `B-LEAD`, `C-LEAD` | `recorded` |
| `SPT-0504` | `D-LEAD` | `C-BACKUP` | `SLA-2BD` | `A-LEAD`, `B-LEAD`, `C-LEAD` | `recorded` |

## 7. Risk and Blocker Carry-In

| risk_id | carried_from | severity | risk statement | linked_kickoff_actions | mitigation_owner | target_date |
| --- | --- | --- | --- | --- | --- | --- |
| `RK-12-01` | `spec/planning/issue_190_carryover_descoping_package.md` | `high` | Canonical `D-13` ledger path from `spec/FUTURE_WORK_V011.md` is not present in-tree; kickoff must preserve explicit source substitution. | `ACT-01`, `ACT-06` | `D-LEAD` | `2026-02-24` |
| `RK-12-02` | `spec/planning/issue_189_readiness_dossier_package.md` | `medium` | Readiness input is package-level (`D-12`) rather than executed dossier artifact; kickoff must keep risk mapping explicit and traceable. | `ACT-05`, `ACT-07` | `D-LEAD` | `2026-02-24` |
| `RK-12-03` | `spec/planning/issue_191_v012_kickoff_packet_package.md` | `medium` | Cross-lane handoff acknowledgement may lag publication, delaying closeout evidence packaging. | `ACT-08` | `D-LEAD` | `2026-02-25` |

Current blocker register:

- No unresolved publication blockers remain open at publish time.
- All carried risks are bounded with named owner and date.

## 8. First-Window Action Plan (Top 8-12 items)

| action_id | carryover_task_id | window | action statement | owner | due_utc | exit evidence | linked_risks | linked_kc |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `ACT-01` | `SPT-0497` | `W1` | Finalize kickoff artifact structure and required section order in bootstrap and checklist artifacts. | `D-LEAD` | `2026-02-23T18:00:00Z` | `spec/FUTURE_WORK_V012_BOOTSTRAP.md` sections 1-9 and `spec/planning/v012_kickoff_checklist.md` sections 1-6 | `RK-12-01` | `KC-02` |
| `ACT-02` | `SPT-0498` | `W1` | Finalize prioritization formula output table and tie-break trace for every carryover row. | `D-LEAD` | `2026-02-23T19:00:00Z` | Bootstrap section 3 scoring table includes all eight rows with `tie_break_trace` | `RK-12-01` | `KC-03` |
| `ACT-03` | `SPT-0499` | `W1` | Publish dependency map integration table and recomputed critical-path summary. | `D-LEAD` | `2026-02-23T20:00:00Z` | Bootstrap section 4 dependency table and chain summary | `RK-12-01` | `KC-04`, `KC-06` |
| `ACT-04` | `SPT-0500` | `W1` | Record `KC-01` through `KC-10` checklist definitions with blocking behavior and remediation actions. | `D-LEAD` | `2026-02-23T21:00:00Z` | Checklist sections 1-4 include all mandatory `KC-*` rows | `RK-12-02` | `KC-01` to `KC-10` |
| `ACT-05` | `SPT-0501` | `W2` | Record handoff windows (`T+24h`, `T+48h`, `T+72h`) and escalation route in artifact and checklist. | `D-LEAD` | `2026-02-24T15:00:00Z` | Bootstrap section 5 and checklist section 4 handoff confirmations | `RK-12-02` | `KC-05`, `KC-10` |
| `ACT-06` | `SPT-0502` | `W2` | Complete `D-14` done-criteria traceability in issue package and publication crosswalk. | `D-LEAD` | `2026-02-24T16:00:00Z` | `spec/planning/issue_191_v012_kickoff_packet_package.md` sections 9-10 and section 13 | `RK-12-01` | `KC-01`, `KC-09` |
| `ACT-07` | `SPT-0503` | `W2` | Execute `python scripts/spec_lint.py` and capture pass result in checklist and publication record. | `D-LEAD` | `2026-02-24T17:00:00Z` | Checklist section 5 and bootstrap section 9 validation tables show pass | `RK-12-02` | `KC-08` |
| `ACT-08` | `SPT-0504` | `W3+` | Publish closeout summary with artifact paths, source revision, and validation outputs for handoff consumers. | `D-LEAD` | `2026-02-25T16:00:00Z` | Issue `#191` closeout summary contains evidence package details | `RK-12-03` | `KC-09`, `KC-10` |

## 9. Acceptance and Publication Record

| checklist_id | result | evidence |
| --- | --- | --- |
| `KC-01` | `PASS` | Checklist section 1 references `D-13` carryover source and shard carryover ledger in bootstrap section 2. |
| `KC-02` | `PASS` | Bootstrap sections 1-9 are present in required order. |
| `KC-03` | `PASS` | Bootstrap section 3 includes score, tier, and tie-break trace for all rows. |
| `KC-04` | `PASS` | Bootstrap section 4 includes all required dependency fields with no Tier-0 hard-predecessor gaps. |
| `KC-05` | `PASS` | Bootstrap section 6 includes primary and backup assignment for all `W1` and `W2` rows. |
| `KC-06` | `PASS` | Bootstrap section 4 includes critical and near-critical chains with delta notes. |
| `KC-07` | `PASS` | Bootstrap section 7 maps each carried risk to at least one action row in section 8. |
| `KC-08` | `PASS` | Checklist section 5 records command text and pass/fail outcomes. |
| `KC-09` | `PASS` | Checklist section 6 records `D-LEAD` plus cross-lane acknowledgement tokens. |
| `KC-10` | `PASS` | Checklist section 4 includes explicit v0.12 owner handoff acknowledgement entries. |

Validation record:

| command | outcome |
| --- | --- |
| `python scripts/spec_lint.py` | `PASS` (`spec-lint: OK`) |

Publication sign-off record:

| field | value |
| --- | --- |
| `publication_timestamp_utc` | `2026-02-23T16:00:00Z` |
| `published_by` | `D-LEAD` |
| `approved_by` | `D-LEAD`, `A-LEAD`, `B-LEAD`, `C-LEAD` |
| `handoff_acknowledged_by` | `A-LEAD`, `B-LEAD`, `C-LEAD`, `D-LEAD` |
