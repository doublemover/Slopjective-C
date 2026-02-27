# v0.11 Quality Gate Decision (`V013-CONF-02`)

_Generated at 2026-02-23T22:00:00Z_

## Decision Summary

- `contract_id`: `V013-CONF-02-QUALITY-GATE-v2`
- `seed_id`: `V013-CONF-02`
- `acceptance_gate_id`: `AC-V013-CONF-02`
- `task_id`: `D-05`
- `release_label`: `v0.11`
- `release_id`: `20260223-issue713-lanea-012`
- `source_revision`: `50c106ed1e0392d5b7820672ce7c3f96f1f0f9c8`
- `overall_decision`: `hold`
- `QG-04`: `fail`
- `recommendation_signal`: `no-go`

## EV-06..EV-08 Evidence Register

| EV ID | Status | Summary | Blocking refs |
| --- | --- | --- | --- |
| `EV-06` | `pass` | Exception ledger is published with required D-05 fields and valid statuses. | none |
| `EV-07` | `pass` | Human-readable gate decision record is published with QG-04 and handoff notes. | none |
| `EV-08` | `fail` | Machine-readable gate decision remains no-go while BLK-189 blockers are open. | BLK-189-01, BLK-189-02, BLK-189-03 |

## Gate Results (`QG-01`..`QG-04`)

| Gate ID | Result | Rationale |
| --- | --- | --- |
| `QG-01` | `fail` | CT-04 failed: unresolved high/critical blocker count is 3 (threshold requires 0). |
| `QG-02` | `pass` | Diagnostics stability evidence is present; no active diagnostics exception is required. |
| `QG-03` | `pass` | Reproducibility rerun digest indicates stable replay outcomes for the locked snapshot. |
| `QG-04` | `fail` | Precedence result from `QG-01`..`QG-03` and active exception set (`0`). |

## EV Contract Mapping

| EV ID | Artifact path | Baseline status |
| --- | --- | --- |
| `EV-06` | `reports/releases/v011_quality_gate_exceptions.md` | `pass` |
| `EV-07` | `reports/releases/v011_quality_gate_decision.md` | `pass` |
| `EV-08` | `reports/releases/v011_quality_gate_decision.status.json` | `fail` |

## Threshold Results Snapshot

| Threshold ID | Gate ID | Observed metric | Pass threshold | Result |
| --- | --- | --- | --- | --- |
| `CT-04` | `QG-01` | `high_or_critical_open_blockers=3` | `==0` | `fail` |
| `FR-01` | `QG-01` | `dashboard_age_hours=2.25` | `<=24` | `pass` |
| `FR-02` | `QG-01` | `seeded_conformance_age_hours=4.17` | `<=24` | `pass` |
| `FR-03` | `QG-03` | `rerun_digest_age_hours=2.25` | `<=72` | `pass` |
| `FR-04` | `QG-04` | `exception_ledger_age_hours=0.00` | `<=24` | `pass` |
| `RT-05` | `QG-03` | `cross_run_verdict_consistency=100%` | `==100%` | `pass` |

## Active Exception Set (`EV-06` linkage)

- `exception_ledger_path`: `reports/releases/v011_quality_gate_exceptions.md`
- `active_exception_ids`: `[]`
- `active_exception_count`: `0`

## Unresolved Blocker Posture

| Blocker ID | Status | Owner | Due date (UTC) | Due path |
| --- | --- | --- | --- | --- |
| `BLK-189-01` | `OPEN` | `D-LEAD` | `2026-02-24` | `reports/releases/v011_readiness_dossier.md#scope-and-baseline` |
| `BLK-189-02` | `OPEN` | `RELEASE-LIAISON` | `2026-02-24` | `reports/releases/v011_readiness_dossier.md#gate-decision-package` |
| `BLK-189-03` | `OPEN` | `D-OPS` | `2026-02-23` | `spec/planning/issue_189_readiness_dossier_package.md#issue-189-closeout-comment-template` |

## Downstream Handoff Notes

| Consumer seed | Required inputs | Handoff state | Handoff note |
| --- | --- | --- | --- |
| `V013-CONF-03` | `EV-07, EV-08` | `ready-after-close` | Consume QG-04 + recommendation_signal from EV-07/EV-08 only after V013-CONF-02 is merged/closed; then finalize v0.12 dress rehearsal verdict publication. |
| `V013-REL-01` | `EV-07, EV-08, BLK-189 posture` | `ready-after-close` | Use no-go decision state and unresolved BLK-189 posture as hard inputs to readiness dossier final recommendation gating. |

## Acceptance Rollup (`AC-V013-CONF-02`)

| Acceptance ID | Status | Summary |
| --- | --- | --- |
| `AC-V013-CONF-02-01` | `pass` | EV-06 exception ledger exists with required D-05 fields. |
| `AC-V013-CONF-02-02` | `pass` | EV-07 and EV-08 are regenerated deterministically from tooling output. |
| `AC-V013-CONF-02-03` | `pass` | Decision state, blocker posture, and active exception set are internally consistent. |
| `AC-V013-CONF-02-04` | `pass` | Downstream handoff notes for V013-CONF-03 and V013-REL-01 are explicit. |
| `AC-V013-CONF-02-05` | `pass` | AC-V013-CONF-02 rollup and unresolved blocker posture are recorded. |

## Validation Command References

- `python scripts/spec_lint.py`
- `python scripts/generate_quality_gate_decision.py`
- `node -e "const fs=require('fs'); JSON.parse(fs.readFileSync('reports/releases/v011_quality_gate_decision.status.json','utf8')); console.log('status-json: OK');"`
- `rg -n "EV-06|EV-07|EV-08|QG-04|recommendation_signal" reports/releases/v011_quality_gate_decision.md reports/releases/v011_quality_gate_decision.status.json reports/releases/v011_quality_gate_exceptions.md`

## Deterministic Rule

- If any of `QG-01`, `QG-02`, or `QG-03` is `blocked`, then `QG-04=blocked`.
- Else if any of `QG-01`, `QG-02`, or `QG-03` is `fail`, then `QG-04=fail`.
- Else if all are `pass`, `QG-04=pass` when no active exceptions exist; otherwise `QG-04=conditional-pass`.
- `recommendation_signal` is derived from `QG-04` (`pass` -> `go-candidate`; `conditional-pass` -> `conditional-go-candidate`; `fail` -> `no-go`; `blocked` -> `hold`).
- `overall_decision` is `approve` only when `QG-04=pass`; otherwise `hold`.

