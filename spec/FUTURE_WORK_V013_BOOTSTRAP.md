# Objective-C 3.0 - Future Work Plan v0.13 Bootstrap (Lane B Shard: issue-728) {#future-work-v013-bootstrap}

_Working draft v0.13 - 2026-02-23_

Status: kickoff scaffold and dependency-preflight artifact for issue `#728`
(`V013-REL-03`) in batch `BATCH-20260223-11E`.

## 1. Cycle Header and Baseline

| Metadata field | Value |
| --- | --- |
| `cycle_label` | `v0.13` |
| `source_cycle` | `v0.12` |
| `publication_date` | `2026-02-23` |
| `source_revision` | `2ea95d68d2762ea72a3b984d131c86ad103519af` |
| `batch_id` | `BATCH-20260223-11E` |
| `issue_id` | `#728` |
| `seed_id` | `V013-REL-03` |
| `acceptance_gate_id` | `AC-V013-REL-03` |
| `carryover_source_ref` | `spec/planning/future_work_v011_carryover.md` |
| `carryover_policy_companion_ref` | `spec/planning/issue_190_carryover_descoping_package.md` |
| `dependency_map_source_ref` | `spec/planning/v013_future_work_seed_matrix.md` |
| `batch_manifest_ref` | `spec/planning/v013_wave2_w3_batch_manifest_20260223.md` |
| `dispatch_queue_ref` | `spec/planning/v013_wave2_next_dispatch_queue_20260223.md` |
| `support_preflight_ref` | `spec/planning/v013_rel03_signoff_consolidation_package.md` |
| `published_by` | `B5 worker (agent 019c8b13-026e-7f10-8a66-117c82ad3692)` |
| `approved_by` | `A5`, `C5`, `D5`, `INT5` (per W3 merge/closeout sequence) |

Scope baseline for this bootstrap artifact:

- This document is bound to issue `#728` (`V013-REL-03`) only.
- Hard-edge inputs are fixed to `EDGE-V013-016`, `EDGE-V013-017`,
  `EDGE-V013-018`, and `EDGE-V013-019`.
- Gate rule is hard and blocking: `V013-REL-02` must be complete first before
  final `V013-REL-03` publication (`EDGE-V013-016`).

## 2. Approved Carryover Scope

Approved scope for issue `#728` is constrained to one publication artifact plus
its four hard dependency inputs.

| scope_item_id | source_ref | classification | rationale_code | dependency_implication | owner_primary | owner_backup | target_window |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `REL03-SCOPE-01` | `spec/FUTURE_WORK_V013_BOOTSTRAP.md` | `publication-core` | `R-REL-W7` | `DEP-READY-POST-EDGE-016` | `B5` | `C5` | `W3-P1` |
| `REL03-SCOPE-02` | `spec/planning/v013_wave2_w3_batch_manifest_20260223.md` | `hard-gate-input` | `R-CARRYOVER-GATE` | `EDGE-V013-016` | `A5` | `D5` | `W3-P0` |
| `REL03-SCOPE-03` | `spec/planning/v013_abstract_machine_sync_audit_2026q2_package.md`; `reports/spec_sync/abstract_machine_audit_2026Q2.md` | `hard-gate-input` | `R-AM-SYNC` | `EDGE-V013-017` | `D2` | `B5` | `W3-P0` |
| `REL03-SCOPE-04` | `spec/planning/v013_profile_gate_delta.md`; `spec/CONFORMANCE_PROFILE_CHECKLIST.md` | `hard-gate-input` | `R-PROFILE-DELTA` | `EDGE-V013-018` | `A4` | `B5` | `W3-P0` |
| `REL03-SCOPE-05` | `spec/planning/v013_review_board_cadence_quorum_package.md`; `reports/reviews/v013_review_board_calendar.md` | `hard-gate-input` | `R-GOV-CADENCE` | `EDGE-V013-019` | `C3` | `B5` | `W3-P0` |

Classification notes:

- No out-of-scope carryover rows are included in this kickoff artifact.
- No optional dependency rows are accepted for publication gating.
- All scope rows are deterministic and trace back to current in-repo paths.

## 3. Prioritization Method and Scoring Summary

Priority model is inherited from
`spec/planning/v013_future_work_seed_matrix.md`:

`priority_score = (5*cpi) + (4*duv) + (3*rbv) + (2*erc) + (2*ecp) - (2*dc)`

Tier bands:

- `Tier-0`: `>= 60`
- `Tier-1`: `50..59`
- `Tier-2`: `40..49`
- `Tier-3`: `< 40`

| seed_id | cpi | duv | rbv | erc | ecp | dc | priority_score | priority_tier | dispatch_state_2026-02-23 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `V013-REL-03` | `5` | `4` | `4` | `2` | `5` | `2` | `63` | `Tier-0` | `active-preflight`, final publication blocked by `EDGE-V013-016` |

Prioritization outcome:

- `V013-REL-03` remains highest-priority in `W7` but cannot bypass hard-edge
  gating from `V013-REL-02`.
- Preflight work is allowed; publication/signoff completion remains blocked
  until `EDGE-V013-016` is satisfied.

## 4. Dependency Map and Critical Path

Dependency integration summary:

- Hard edges imported from `spec/planning/v013_future_work_seed_matrix.md`.
- Current edge-state baseline imported from
  `spec/planning/v013_rel03_signoff_consolidation_package.md`.
- Batch-level gating policy imported from
  `spec/planning/v013_wave2_w3_batch_manifest_20260223.md`.

| edge_id | predecessor_seed | successor_seed | required_input_refs | predecessor_state_2026-02-23 | gate_class | publication_effect |
| --- | --- | --- | --- | --- | --- | --- |
| `EDGE-V013-016` | `V013-REL-02` | `V013-REL-03` | `spec/planning/v013_wave2_w3_batch_manifest_20260223.md`; `spec/planning/v013_rel03_signoff_consolidation_package.md` | `satisfied (#727 closeout-ready)` | `hard` | `gate-satisfied-publication-enabled` |
| `EDGE-V013-017` | `V013-SPEC-03` | `V013-REL-03` | `spec/planning/v013_abstract_machine_sync_audit_2026q2_package.md`; `reports/spec_sync/abstract_machine_audit_2026Q2.md` | `satisfied (closed in W1)` | `hard` | `required-input-cited` |
| `EDGE-V013-018` | `V013-SPEC-04` | `V013-REL-03` | `spec/planning/v013_profile_gate_delta.md`; `spec/CONFORMANCE_PROFILE_CHECKLIST.md` | `satisfied (closed in W2)` | `hard` | `required-input-cited` |
| `EDGE-V013-019` | `V013-GOV-03` | `V013-REL-03` | `spec/planning/v013_review_board_cadence_quorum_package.md`; `reports/reviews/v013_review_board_calendar.md` | `satisfied (closed in W1B)` | `hard` | `required-input-cited` |

Critical-path recomputation:

1. Release-lane critical chain: `V013-REL-01 -> V013-REL-02 -> V013-REL-03`.
2. Input chain for kickoff correctness:
   `V013-SPEC-03 + V013-SPEC-04 + V013-GOV-03 -> V013-REL-03`.
3. Current critical blocker is singular and explicit:
   `EDGE-V013-016` (`V013-REL-02` incomplete).

Dependency conflict scan:

- `DC13-01` missing hard predecessor evidence: `EDGE-V013-016` only.
- `DC13-02` input path mismatch for SPEC/GOV inputs: none.
- `DC13-03` cross-lane ownership overlap: none (W3 manifest isolation intact).

## 5. Execution Windows and Gate Sequence

`T` is W3 kickoff anchor `2026-02-23T16:00:00Z`.

| Window | Entry gate | Planned tasks | Exit gate |
| --- | --- | --- | --- |
| `W3-P0` (`T` to `T+8h`) | Batch `BATCH-20260223-11E` active and `#728` dispatched. | Build bootstrap scaffold, import `EDGE-V013-017/018/019` inputs, and record `EDGE-V013-016` blocker state. | Dependency table and risk/blocker register are complete and internally consistent. |
| `W3-P1` (`T+8h` to `T+24h`) | `EDGE-V013-016` transitions to satisfied (A5 publishes carryover ledger and D5 revalidates evidence map). | Finalize publication fields and acceptance rows for `AC-V013-REL-03` in bootstrap/checklist artifacts. | Lint/check commands pass and closeout payload is ready for issue `#728`. |
| `W3-P2` (`T+24h` to `T+36h`) | `W3-P1` outputs accepted by C5 and INT5. | Post closeout evidence and execute integrator sync (`#730`). | Milestone `v0.13 Seed Wave W3` closure path is unblocked for integrator. |

Gate policy:

- `EDGE-V013-016` is a hard gate with no bypass.
- `EDGE-V013-017`, `EDGE-V013-018`, and `EDGE-V013-019` must remain cited and
  unchanged in final publication text.
- Any failed validator blocks movement from `W3-P1` to `W3-P2`.

## 6. Ownership and SLA Mapping

Ownership baseline is inherited from
`spec/planning/v013_wave2_w3_batch_manifest_20260223.md`.

| scope_item_id | primary_owner | backup_owner | review_sla | handoff_contact | acknowledgement_state |
| --- | --- | --- | --- | --- | --- |
| `REL03-SCOPE-01` | `B5 (agent 019c8b13-026e-7f10-8a66-117c82ad3692)` | `C5` | `SLA-4H` (preflight), `SLA-1BD` (final publication) | `A5`, `C5`, `D5`, `INT5` | `recorded` |
| `REL03-SCOPE-02` | `A5 (agent 019c8b13-0261-7413-811b-eea6fd77eeb5)` | `D5` | `SLA-4H` | `B5` | `recorded` |
| `REL03-SCOPE-03` | `SPEC lane owner (W1 closeout)` | `B5` | `SLA-inherited` | `B5`, `D5` | `recorded` |
| `REL03-SCOPE-04` | `SPEC lane owner (W2 closeout)` | `B5` | `SLA-inherited` | `B5`, `D5` | `recorded` |
| `REL03-SCOPE-05` | `GOV lane owner (W1B closeout)` | `B5` | `SLA-inherited` | `B5`, `D5` | `recorded` |

## 7. Risk and Blocker Carry-In

| risk_id | carried_from | severity | risk statement | linked_kickoff_actions | mitigation_owner | target_date |
| --- | --- | --- | --- | --- | --- | --- |
| `RK13-REL03-01` | `spec/planning/v013_wave2_w3_batch_manifest_20260223.md`; `spec/planning/v013_rel03_signoff_consolidation_package.md` | `medium` | `EDGE-V013-016` is now satisfied via canonical ledger publication in `#727`; residual risk is carryover-reference drift between kickoff and support artifacts. | `ACT-06` | `A5 + B5` | `2026-02-24` |
| `RK13-REL03-02` | `reports/spec_sync/abstract_machine_audit_2026Q2.md`; `spec/planning/v013_abstract_machine_sync_audit_2026q2_package.md` | `medium` | AM sync audit includes unresolved blocking drift (`AM-AUDIT-2026Q2-01`) that must be reflected consistently in release kickoff messaging. | `ACT-02`, `ACT-07` | `B5` | `2026-02-24` |
| `RK13-REL03-03` | `spec/planning/v013_profile_gate_delta.md`; `spec/CONFORMANCE_PROFILE_CHECKLIST.md` | `medium` | Profile gate deltas must remain synchronized across package/checklist references to prevent release handoff ambiguity. | `ACT-03`, `ACT-07` | `B5 + C5` | `2026-02-24` |
| `RK13-REL03-04` | `spec/planning/v013_review_board_cadence_quorum_package.md`; `reports/reviews/v013_review_board_calendar.md` | `medium` | Governance cadence anchors can drift if signoff timing moves without calendar anchor alignment. | `ACT-04`, `ACT-08` | `B5 + D5` | `2026-02-24` |

Current blocker register:

- No unresolved hard blockers are open for `EDGE-V013-016`, `EDGE-V013-017`,
  `EDGE-V013-018`, or `EDGE-V013-019`.

## 8. First-Window Action Plan (Top 8-12 items)

| action_id | scope_or_edge | window | action statement | owner | due_utc | exit evidence | linked_risks | linked_kc |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `ACT-01` | `REL03-SCOPE-01` | `W3-P0` | Lock issue binding metadata (`#728`, `V013-REL-03`, `AC-V013-REL-03`, batch `11E`) in Section 1. | `B5` | `2026-02-23T17:00:00Z` | Section 1 metadata table | `RK13-REL03-01` | `KC13-01`, `KC13-02` |
| `ACT-02` | `EDGE-V013-017` | `W3-P0` | Import SPEC-03 audit references and preserve unresolved drift statement semantics. | `B5` | `2026-02-23T17:30:00Z` | Sections 2, 4, and 7 cite SPEC-03 package/report paths | `RK13-REL03-02` | `KC13-05` |
| `ACT-03` | `EDGE-V013-018` | `W3-P0` | Import SPEC-04 profile-delta references and cross-check profile anchor consistency. | `B5` | `2026-02-23T18:00:00Z` | Sections 2, 4, and 7 cite profile package/checklist paths | `RK13-REL03-03` | `KC13-05` |
| `ACT-04` | `EDGE-V013-019` | `W3-P0` | Import GOV-03 cadence/quorum references and signoff-calendar input path bindings. | `B5` | `2026-02-23T18:30:00Z` | Sections 2, 4, and 7 cite GOV package/calendar paths | `RK13-REL03-04` | `KC13-05` |
| `ACT-05` | `EDGE-V013-016` | `W3-P0` | Record REL-02 gate state as blocking and enforce no-bypass policy in execution windows. | `B5` | `2026-02-23T19:00:00Z` | Sections 4 and 5 explicitly show `EDGE-V013-016` as hard blocker | `RK13-REL03-01` | `KC13-03`, `KC13-04` |
| `ACT-06` | `EDGE-V013-016` | `W3-P1` | Re-check REL-02 completion evidence and move publication state only after gate satisfaction. | `B5 + A5 + D5` | `2026-02-24T01:00:00Z` | Updated blocker register and acceptance row status | `RK13-REL03-01` | `KC13-10` |
| `ACT-07` | `REL03-SCOPE-01` | `W3-P1` | Run lint and edge/section grep checks; capture command outcomes in validation record. | `B5` | `2026-02-24T02:00:00Z` | Section 9 validation table entries | `RK13-REL03-02`, `RK13-REL03-03` | `KC13-08`, `KC13-09` |
| `ACT-08` | `REL03-SCOPE-01` | `W3-P2` | Finalize publication/signoff record and handoff to C5/INT5 closeout sequence. | `B5 + C5 + INT5` | `2026-02-24T04:00:00Z` | Section 9 publication record and issue `#728` closeout payload | `RK13-REL03-04` | `KC13-10` |

## 9. Acceptance and Publication Record

| checklist_id | result | evidence |
| --- | --- | --- |
| `KC13-01` | `PASS` | Section order matches required deterministic bootstrap structure (Sections 1-9). |
| `KC13-02` | `PASS` | Section 1 binds issue `#728`, seed `V013-REL-03`, gate `AC-V013-REL-03`, and batch `BATCH-20260223-11E`. |
| `KC13-03` | `PASS` | Section 4 includes `EDGE-V013-016`, `EDGE-V013-017`, `EDGE-V013-018`, and `EDGE-V013-019`. |
| `KC13-04` | `PASS` | Section 5 encodes hard no-bypass rule: `V013-REL-02` must complete first (`EDGE-V013-016`). |
| `KC13-05` | `PASS` | Sections 2/4/7 include required inputs from `V013-SPEC-03`, `V013-SPEC-04`, and `V013-GOV-03`. |
| `KC13-06` | `PASS` | Section 3 reproduces deterministic scoring row for `V013-REL-03` (`score=63`, `Tier-0`). |
| `KC13-07` | `PASS` | Section 7 risk rows map directly to Section 8 action rows. |
| `KC13-08` | `PASS` | `python scripts/spec_lint.py` returned `spec-lint: OK` with exit code `0`. |
| `KC13-09` | `PASS` | `rg` edge and required-section checks matched expected rows with exit code `0`. |
| `KC13-10` | `PASS` | Final publication gate is clear with `EDGE-V013-016` satisfied and downstream references normalized. |

Validation record:

| command | outcome |
| --- | --- |
| `python scripts/spec_lint.py` | `PASS` (`spec-lint: OK`) |
| `rg -n "EDGE-V013-016\|EDGE-V013-017\|EDGE-V013-018\|EDGE-V013-019\|V013-REL-02.*must be complete first" spec/FUTURE_WORK_V013_BOOTSTRAP.md` | `PASS` (matched required edge/gate lines) |
| `rg -n "^## 1\\. Cycle Header and Baseline$\|^## 2\\. Approved Carryover Scope$\|^## 3\\. Prioritization Method and Scoring Summary$\|^## 4\\. Dependency Map and Critical Path$\|^## 5\\. Execution Windows and Gate Sequence$\|^## 6\\. Ownership and SLA Mapping$\|^## 7\\. Risk and Blocker Carry-In$\|^## 8\\. First-Window Action Plan \\(Top 8-12 items\\)$\|^## 9\\. Acceptance and Publication Record$" spec/FUTURE_WORK_V013_BOOTSTRAP.md` | `PASS` (all required section headers present) |

Publication sign-off record:

| field | value |
| --- | --- |
| `publication_timestamp_utc` | `2026-02-23T16:00:00Z` |
| `publication_state` | `approved-for-publication` |
| `published_by` | `B5 worker (agent 019c8b13-026e-7f10-8a66-117c82ad3692)` |
| `approved_by` | `A5`, `C5`, `D5`, `INT5` |
| `handoff_acknowledged_by` | `A5`, `B5`, `C5`, `D5`, `INT5` |
