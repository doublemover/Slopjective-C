# v0.11 Quality Gate Exception Ledger (`V013-CONF-02`)

_Generated at 2026-02-23T22:00:00Z_

## Ledger Metadata

| Field | Value |
| --- | --- |
| `contract_id` | `V013-CONF-02-QUALITY-GATE-v2` |
| `seed_id` | `V013-CONF-02` |
| `acceptance_gate_id` | `AC-V013-CONF-02` |
| `task_id` | `D-05` |
| `release_label` | `v0.11` |
| `release_id` | `20260223-issue713-lanea-012` |
| `source_revision` | `50c106ed1e0392d5b7820672ce7c3f96f1f0f9c8` |
| `ledger_owner` | `worker-lane-b` |
| `ev_linkage` | `EV-06` |
| `decision_artifacts` | `reports/releases/v011_quality_gate_decision.md`; `reports/releases/v011_quality_gate_decision.status.json` |

## EV-06 Linkage and Decision Context

- `EV-06` publishes required exception-ledger fields for D-05 policy consumption.
- Active exception set is empty and matches `active_exception_ids` in EV-07/EV-08.
- `QG-04` remains `fail` due unresolved blockers, not due exception-budget overflow.
- `recommendation_signal` remains `no-go` in EV-07/EV-08 downstream contracts.

## Exception Records (Required D-05 Fields)

No exception requests are active for this publication window.

| `exception_id` | `gate_id` | `threshold_id` | `exception_class` | `severity` | `requested_by` | `approved_by` | `approved_at_utc` | `expires_at_utc` | `impact_summary` | `compensating_controls` | `remediation_owner` | `remediation_due_utc` | `status` | `closure_evidence_refs` |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `_none_` | `_none_` | `_none_` | `_none_` | `_none_` | `_none_` | `_none_` | `_none_` | `_none_` | `No approved or pending exceptions in this EV-06 snapshot.` | `Not applicable.` | `_none_` | `_none_` | `open=0, closed=0, expired=0, rejected=0` | `none` |

## Active Exception Set and Budget Check (`QG-04`)

| Field | Value |
| --- | --- |
| `active_exception_ids` | `[]` |
| `active_exception_count` | `0` |
| `active_exception_budget_max` | `2` |
| `max_per_gate_domain` | `1` |
| `active_critical_or_high_exceptions` | `0` |
| `budget_evaluation_result` | `pass` |

## Non-Exception Unresolved Blocker Posture

`BLK-189-*` entries are tracked as blockers (not exceptions) and keep EV-08 in a fail/no-go posture.

| Blocker ID | Status | Owner | Due date (UTC) | Due path |
| --- | --- | --- | --- | --- |
| `BLK-189-01` | `OPEN` | `D-LEAD` | `2026-02-24` | `reports/releases/v011_readiness_dossier.md#scope-and-baseline` |
| `BLK-189-02` | `OPEN` | `RELEASE-LIAISON` | `2026-02-24` | `reports/releases/v011_readiness_dossier.md#gate-decision-package` |
| `BLK-189-03` | `OPEN` | `D-OPS` | `2026-02-23` | `spec/planning/issue_189_readiness_dossier_package.md#issue-189-closeout-comment-template` |

## Downstream Handoff Notes

| Consumer seed | Required inputs | Handoff note |
| --- | --- | --- |
| `V013-CONF-03` | `EV-07`, `EV-08` | Consume `QG-04` and `recommendation_signal` from EV-07/EV-08 after `V013-CONF-02` close; finalize dress-rehearsal publication state. |
| `V013-REL-01` | `EV-07`, `EV-08`, `BLK-189 posture` | Carry forward no-go posture and unresolved blocker table into readiness dossier recommendation gating. |

## Acceptance Rollup (`AC-V013-CONF-02`)

| Acceptance ID | Status | Evidence |
| --- | --- | --- |
| `AC-V013-CONF-02-01` | `pass` | Required exception ledger fields are present in EV-06. |
| `AC-V013-CONF-02-02` | `pass` | EV-07/EV-08 generated deterministically via `scripts/generate_quality_gate_decision.py`. |
| `AC-V013-CONF-02-03` | `pass` | Active exception set is identical across EV-06/EV-07/EV-08 (`[]`). |
| `AC-V013-CONF-02-04` | `pass` | Explicit handoff lines for `V013-CONF-03` and `V013-REL-01` are recorded. |
| `AC-V013-CONF-02-05` | `pass` | Unresolved blocker posture is recorded alongside decision-state constraints. |
