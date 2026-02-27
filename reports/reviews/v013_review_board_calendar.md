# v0.13 Review Board Calendar (`V013-GOV-03`) {#v013-review-board-calendar}

_Published on 2026-02-23 for issue `#791` in batch `BATCH-20260223-11S`._

Timezone baseline: `UTC`

## 0. Reseed metadata binding

| Field | Value |
| --- | --- |
| `issue` | `#791` |
| `seed_id` | `V013-GOV-03` |
| `wave_id` | `W1` |
| `batch_id` | `BATCH-20260223-11S` |
| `milestone_id` | `#32` |
| `acceptance_gate_id` | `AC-V013-GOV-03` |
| `source_package` | `spec/planning/v013_review_board_cadence_quorum_package.md` |
| `operating_model` | `spec/governance/review_board_operating_model_v1.md` |
| `lane_evidence` | `spec/planning/evidence/lane_c/v013_seed_gov03_validation_20260223.md` |

## 1. Deterministic Session Calendar

| Session ID | Session type | Scheduled UTC | Quorum precheck (`T0-24h`) | Quorum attestation due (`T0+4h`) | Minutes due (`T0+24h`) | Decision packet due (`T0+72h`) |
| --- | --- | --- | --- | --- | --- | --- |
| `RB-SES-20260226-FORMAL` | Formal decision | `2026-02-26T16:00:00Z` | `2026-02-25T16:00:00Z` | `2026-02-26T20:00:00Z` | `2026-02-27T16:00:00Z` | `2026-03-01T16:00:00Z` |
| `RB-SES-20260312-FORMAL` | Formal decision | `2026-03-12T16:00:00Z` | `2026-03-11T16:00:00Z` | `2026-03-12T20:00:00Z` | `2026-03-13T16:00:00Z` | `2026-03-15T16:00:00Z` |
| `RB-SES-20260326-FORMAL` | Formal decision | `2026-03-26T16:00:00Z` | `2026-03-25T16:00:00Z` | `2026-03-26T20:00:00Z` | `2026-03-27T16:00:00Z` | `2026-03-29T16:00:00Z` |
| `RB-SES-20260409-FORMAL` | Formal decision | `2026-04-09T16:00:00Z` | `2026-04-08T16:00:00Z` | `2026-04-09T20:00:00Z` | `2026-04-10T16:00:00Z` | `2026-04-12T16:00:00Z` |

## 2. Recurring Governance Cadence

| Cadence class | Recurrence | Next 3 UTC anchors |
| --- | --- | --- |
| Weekly triage (`SCHED-02`) | Every Tuesday `16:00:00Z` | `2026-02-24T16:00:00Z`, `2026-03-03T16:00:00Z`, `2026-03-10T16:00:00Z` |
| Monthly snapshot (`SCHED-04`) | First business day `18:00:00Z` | `2026-03-02T18:00:00Z`, `2026-04-01T18:00:00Z`, `2026-05-01T18:00:00Z` |
| Quarterly audit (`SCHED-05`) | Second Wednesday `17:00:00Z` of quarter-start month | `2026-04-08T17:00:00Z`, `2026-07-08T17:00:00Z`, `2026-10-14T17:00:00Z` |

## 3. Escalation Window Anchors

| Escalation window | Trigger anchor | Response SLA |
| --- | --- | --- |
| `EW-01` | `T0-24h` quorum precheck predicts failure | Alternate roster by `T0-4h` |
| `EW-02` | Quorum fail at `T0` | Incident publish `T0+2h`; reschedule `<=3` business days |
| `EW-03` | Two misses in rolling `28` days | Steering review `<=2` business days |
| `EW-04` | Publication deadline miss (`QPUB-02`..`QPUB-04`) | SLA incident `<=4h` from miss |
| `EW-05` | Hold/veto unresolved `>10` business days | Emergency session `<=24h` |

## 4. `AC-V013-GOV-03` Evidence Anchors

| Acceptance row | Calendar evidence | Status |
| --- | --- | --- |
| `AC-V013-GOV-03-01` | Section `0` binds issue/tranche metadata and Section `1` preserves deterministic formal-session anchors. | `PASS` |
| `AC-V013-GOV-03-02` | Section `1` publishes quorum-attestation, minutes, and decision-packet deadlines per session. | `PASS` |
| `AC-V013-GOV-03-03` | Section `3` publishes deterministic escalation trigger and SLA anchors. | `PASS` |
| `AC-V013-GOV-03-04` | Section `1` includes precomputed formal-session deadlines for all listed sessions. | `PASS` |
| `AC-V013-GOV-03-05` | Section `2` includes recurring triage, monthly snapshot, and quarterly audit anchors. | `PASS` |

## 5. Validation Transcript

| Command | Exact output | Exit code | Status |
| --- | --- | --- | --- |
| `python scripts/spec_lint.py` | `spec-lint: OK` | `0` | `PASS` |
