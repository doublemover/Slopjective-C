# v0.13 REL-03 Signoff and Evidence Consolidation Support Package (`V013-REL-03`) {#v013-rel03-signoff-consolidation-package}

## 2. Dependency Register

| dependency_id | producer_seed | consumer_seed | required upstream artifacts | normalization contract | signoff class |
| --- | --- | --- | --- | --- | --- |
| `EDGE-V013-016` | `V013-REL-02` | `V013-REL-03` | `spec/planning/future_work_v011_carryover.md` | Canonical ledger path is authoritative. | `blocking` |
| `EDGE-V013-017` | `V013-SPEC-03` | `V013-REL-03` | `reports/spec_sync/abstract_machine_audit_2026Q2.md` | Consume latest audit drift state and unresolved IDs verbatim; no reinterpretation in REL package. | `blocking` |
| `EDGE-V013-018` | `V013-SPEC-04` | `V013-REL-03` | `spec/planning/v013_profile_gate_delta.md`; `spec/CONFORMANCE_PROFILE_CHECKLIST.md` | Treat profile delta package as decision source. | `blocking` |
| `EDGE-V013-019` | `V013-GOV-03` | `V013-REL-03` | `spec/planning/v013_review_board_cadence_quorum_package.md`; `reports/reviews/v013_review_board_calendar.md` | Consume cadence/quorum contract and precomputed deadlines. | `blocking` |

## 3. Evidence Consolidation Matrix

| evidence_id | edge_id | artifact_path | pre_link_role | normalization rule | verification command | current_state_2026-02-23 |
| --- | --- | --- | --- | --- | --- | --- |
| `REL03-EV-03` | `EDGE-V013-017` | `reports/spec_sync/abstract_machine_audit_2026Q2.md` | SPEC-03 audit baseline and drift-status payload. | Import unresolved drift IDs/status into kickoff risk table unchanged. | `Test-Path reports/spec_sync/abstract_machine_audit_2026Q2.md` | `present (True)` |

Pre-link index for release handoff packet assembly:

1. Carryover lane input (`EDGE-V013-016`): `REL03-EV-01`, `REL03-EV-02`.
2. AM sync lane input (`EDGE-V013-017`): `REL03-EV-03`.

## 5. Exception Handling

| exception_id | trigger | classification | immediate action | escalation | resolution SLA |
| --- | --- | --- | --- | --- | --- |
| `REL03-EX-02` | `reports/spec_sync/abstract_machine_audit_2026Q2.md` missing or unreadable. | `blocking` | Freeze REL-03 signoff and request SPEC-03 republish. | Escalate to SPEC lane owner and release chair. | `<=24h` from detection. |

## 6. Acceptance Criteria (`AC-729-*`, mapped to `AC-V013-REL-03`)

| acceptance_id | criterion | deterministic verification | status_2026-02-23 |
| --- | --- | --- | --- |
| `AC-729-02` | Dependency register explicitly covers `EDGE-V013-016..019`. | Section `2` lists each edge with producer/consumer and required artifacts. | `PASS` |

## 8. Validation Transcript

| command_id | command | output_excerpt | exit_code | result |
| --- | --- | --- | ---: | --- |
| `REL03-CMD-03` | `rg -n "spec/planning/future_work_v011_carryover.md|reports/spec_sync/abstract_machine_audit_2026Q2.md|spec/planning/v013_profile_gate_delta.md|spec/CONFORMANCE_PROFILE_CHECKLIST.md|spec/planning/v013_review_board_cadence_quorum_package.md|reports/reviews/v013_review_board_calendar.md" spec/planning/v013_rel03_signoff_consolidation_package.md` | `matched all required upstream artifact references` | `0` | `PASS` |
