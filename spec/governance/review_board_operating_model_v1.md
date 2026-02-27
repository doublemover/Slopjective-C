# Review Board Operating Model v1 (`V013-GOV-03`) {#review-board-operating-model-v1}

_Active governance baseline - 2026-02-23 (issue `#791`, batch `BATCH-20260223-11S`)._

This artifact defines deterministic review-board cadence, quorum publication,
and escalation windows for v0.13 governance operations.

## 0. Reseed metadata binding

| Field | Value |
| --- | --- |
| `issue` | `#791` |
| `seed_id` | `V013-GOV-03` |
| `wave_id` | `W1` |
| `batch_id` | `BATCH-20260223-11S` |
| `milestone_id` | `#32` |
| `acceptance_gate_id` | `AC-V013-GOV-03` |
| `package_artifact` | `spec/planning/v013_review_board_cadence_quorum_package.md` |
| `calendar_artifact` | `reports/reviews/v013_review_board_calendar.md` |
| `lane_evidence_artifact` | `spec/planning/evidence/lane_c/v013_seed_gov03_validation_20260223.md` |

## 1. Scope and authority

This operating model governs:

1. Session scheduling and calendar generation for review-board operations.
2. Quorum checks for binding formal decisions.
3. Publication windows for quorum and decision artifacts.
4. Escalation behavior when quorum or publication windows are missed.

This model is an operational refresh and does not replace charter authority
boundaries defined by governance baseline artifacts.

## 2. Deterministic schedule model

### 2.1 Session types and recurrence rules

| Session type | Rule ID | Deterministic recurrence |
| --- | --- | --- |
| Weekly triage | `SCHED-02` | Every Tuesday at `16:00:00Z`. |
| Formal decision session | `SCHED-03` | Every 14 days on Thursday at `16:00:00Z`, anchored at `2026-02-26T16:00:00Z`. |
| Monthly governance snapshot | `SCHED-04` | First business day of each month at `18:00:00Z`. |
| Quarterly governance audit | `SCHED-05` | Second Wednesday of quarter-start month at `17:00:00Z`. |

All schedule calculations are performed in `UTC`.

### 2.2 Deterministic identifier rules

| Identifier | Format | Example |
| --- | --- | --- |
| `session_id` | `RB-SES-{YYYYMMDD}-{TRIAGE/FORMAL/AUDIT}` | `RB-SES-20260312-FORMAL` |
| `snapshot_id` | `RB-SNAPSHOT-<YYYYMM>` | `RB-SNAPSHOT-202604` |
| `decision_id` | `RB-DEC-<YYYY>-<NNN>` | `RB-DEC-2026-014` |
| `quorum_attestation_id` | `RB-QUORUM-<YYYYMMDD>-<NN>` | `RB-QUORUM-20260312-01` |

### 2.3 Collision and variance policy

1. If a formal session conflicts with a release-freeze gate, shift the session
   by `+24h`.
2. A maximum of three shifts (`+72h`) is allowed without steering review.
3. If conflict remains after `+72h`, invoke escalation window `EW-03`.
4. Every shift requires a variance row in the calendar report with original and
   replacement timestamps.

## 3. Quorum policy for binding decisions

A formal decision is binding only if all predicates pass:

| Predicate ID | Predicate | Pass rule |
| --- | --- | --- |
| `Q-01` | Voting attendance floor | `present_voters >= 5` |
| `Q-02` | Independent vendor floor | `vendor_count >= 3` |
| `Q-03` | Mandatory role attendance | `spec_editor_present = true` and `tooling_owner_present = true` |
| `Q-04` | Recusal cap | `recusal_ratio <= 0.40` |

Deterministic calculations:

- `recusal_ratio = recused_voters / seated_voters`
- `quorum_result = PASS` only when `Q-01`..`Q-04` are all `PASS`

If quorum fails:

1. The motion cannot produce a binding disposition.
2. Allowed session outcomes are `DEFER` or `NON-BINDING-POLL`.
3. Escalation window `EW-02` starts immediately.

## 4. Publication contract and deadlines

### 4.1 Required publication artifacts

| Artifact | Content minimum | Deadline |
| --- | --- | --- |
| Agenda packet | Session ID, motion list, expected quorum roster. | `T0 - 48h` |
| Quorum attestation | `Q-01`..`Q-04` inputs, computed `recusal_ratio`, quorum result. | `T0 + 4h` |
| Minutes summary | Attendance, vote tally, quorum result, recusal rationale IDs. | `T0 + 24h` |
| Final decision packet | Canonical motion text, threshold evaluation, quorum proof, outcome, actions. | `T0 + 72h` |
| Decision index update | Links to agenda, attestation, minutes, packet, and escalation records. | `T0 + 120h` |

`T0` is the scheduled formal-session timestamp.

### 4.2 Monthly and quarterly publication outputs

| Output | Content minimum | Deadline |
| --- | --- | --- |
| Monthly snapshot | Open motions, active veto/hold states, SLA compliance summary, escalation counters. | First business day `18:00:00Z`. |
| Quarterly audit report | Cadence adherence metrics, quorum pass-rate trend, escalation trend, corrective actions. | Second Wednesday of quarter-start month `17:00:00Z` plus `48h` for minutes publication. |

## 5. Escalation windows

| Window ID | Trigger | Escalation owner | Required response window | Closure evidence |
| --- | --- | --- | --- | --- |
| `EW-01` | `T0 - 24h` precheck predicts quorum failure. | Board chair | Publish corrected roster by `T0 - 4h`. | Updated roster row with all quorum predicates passing. |
| `EW-02` | Quorum fails at `T0`. | Board chair + recorder | Publish miss incident by `T0 + 2h`; reschedule in `<=3` business days. | Replacement `session_id` and timestamp published in calendar report. |
| `EW-03` | Two quorum misses in rolling `28` days or unresolved collision after `+72h` shift budget. | Steering owner delegate | Hold steering escalation session in `<=2` business days. | Steering disposition note with explicit operating decision. |
| `EW-04` | Any deadline miss in Section `4.1`. | Board chair delegate | Open SLA incident within `4h` of miss; publish recovery plan within `24h`. | Recovery record with new absolute deadlines and owner assignment. |
| `EW-05` | `hold` or veto state unresolved for `>10` business days. | Security owner + steering owner | Convene emergency governance session in `<=24h`. | Clearance note or explicit go/no-go decision record. |

## 6. Baseline calendar window for v0.13

The active deterministic baseline for this refresh is:

- start: `2026-02-24T16:00:00Z`
- end: `2026-05-31T23:59:59Z`

Canonical detailed rows are published in:

- `reports/reviews/v013_review_board_calendar.md`

## 7. Required audit fields

Every formal session must publish:

1. `session_id`
2. `scheduled_utc`
3. `present_voters`
4. `vendor_count`
5. `spec_editor_present`
6. `tooling_owner_present`
7. `recused_voters`
8. `seated_voters`
9. `recusal_ratio`
10. `quorum_result`
11. `vote_tally`
12. `outcome`
13. `escalation_windows_triggered`
14. `publication_deadlines_met`

## 8. Conformance anchors for `AC-V013-GOV-03`

| Anchor ID | Evidence |
| --- | --- |
| `ANCHOR-GOV03-01` | Section `0` binds this operating model to issue `#791` and tranche `BATCH-20260223-11S`. |
| `ANCHOR-GOV03-02` | Section `2` provides deterministic cadence and recurrence anchors. |
| `ANCHOR-GOV03-03` | Section `3` defines mechanical quorum evaluation rules. |
| `ANCHOR-GOV03-04` | Section `4` defines publication artifacts and deterministic deadlines. |
| `ANCHOR-GOV03-05` | Section `5` defines escalation windows with fixed response timings. |
| `ANCHOR-GOV03-06` | Section `6` binds this model to the published deterministic calendar report. |

## 9. M07 Governance Annualization W1 Addendum (`RBOM-DEP-M07-*`)

### 9.1 Contract binding for lane A issue `#854`

| Field | Value |
| --- | --- |
| `issue` | `#854` |
| `milestone_slice` | `[v0.14][M07][lane:A]` |
| `objective` | Preserve operating-model normative enforcement as annualization W1 baseline authority. |
| `acceptance_matrix_artifact` | `spec/planning/v014_governance_annualization_w1_acceptance_matrix_20260224.md` |
| `lane_validation_artifact` | `spec/planning/evidence/lane_a/v014_governance_annualization_w1_lane_a_validation_20260224.md` |

### 9.2 Dependency rows

| Dependency ID | Type | Deterministic semantic rule | Fail criteria | Escalation owner | Unblock condition | Linked acceptance row |
| --- | --- | --- | --- | --- | --- | --- |
| `RBOM-DEP-M07-01` | `Hard` | Schedule model (`Section 2`) remains normative and deterministic for annualization, including recurrence anchors and collision policy. | Annualization changes remove or weaken deterministic recurrence/collision semantics. | `Lane A M07 owner (#854)` | Reinstate deterministic schedule clauses and rerun operating-model command anchors. | `AC-V014-M07-03` |
| `RBOM-DEP-M07-02` | `Hard` | Quorum policy (`Section 3`) remains mechanically evaluable with explicit predicates `Q-01`..`Q-04`. | Predicate math/threshold semantics are missing, ambiguous, or replaced by discretionary language. | `Lane A M07 owner (#854)` | Restore mechanical quorum predicate rules and revalidate acceptance commands. | `AC-V014-M07-03` |
| `RBOM-DEP-M07-03` | `Hard` | Publication deadlines (`Section 4`) remain fail-closed and tied to `T0` windows. | Deadline windows are removed/ambiguous or converted from required to optional behavior. | `Lane A M07 owner (#854)` | Reintroduce deterministic `T0`-relative deadlines and rerun verification commands. | `AC-V014-M07-03` |
| `RBOM-DEP-M07-04` | `Hard` | Escalation windows (`Section 5`) remain explicit with trigger, owner, response window, and closure evidence. | Any escalation row lacks required fields or response windows become undefined. | `Lane A M07 owner (#854)` | Restore full escalation schema and revalidate matrix acceptance rows. | `AC-V014-M07-04` |
| `RBOM-DEP-M07-05` | `Soft` | Baseline calendar window extension (`Section 6`) is advisory `HOLD` until synchronized with annual package and cadence package snapshots. | Calendar-window drift is untracked or used as a hard-go/no-go decision input without synchronization evidence. | `Lane A M07 owner (#854)` | Publish synchronized snapshot timestamp across all three baseline artifacts or escalate drift to hard-failure investigation. | `AC-V014-M07-04` |
| `RBOM-DEP-M07-06` | `Hard` | Matrix/evidence schema and repository lint are blocking annualization release gates. | `DEP/CMD/EVID/AC` mapping is incomplete or `python scripts/spec_lint.py` fails. | `Lane A M07 owner (#854)` | Repair schema mapping, resolve lint findings, and capture clean validation transcript. | `AC-V014-M07-05`, `AC-V014-M07-06` |

### 9.3 Deterministic disposition rule

1. `PASS`: hard rows `RBOM-DEP-M07-01`, `RBOM-DEP-M07-02`, `RBOM-DEP-M07-03`, `RBOM-DEP-M07-04`, and `RBOM-DEP-M07-06` pass with linked evidence.
2. `HOLD`: only soft row `RBOM-DEP-M07-05` remains open with explicit owner, ETA, and synchronized-risk note.
3. `FAIL`: any hard row fails, or a soft-row calendar drift is used to bypass hard quorum/publication/escalation controls.
