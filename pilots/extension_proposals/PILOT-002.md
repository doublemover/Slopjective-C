# Pilot Extension Proposal: PILOT-002

## 0. Pilot metadata

| Field | Value |
| --- | --- |
| `pilot_id` | `PILOT-002` |
| `task_id` | `C-12` |
| `proposal_id` | `EXT-PROP-PILOT-002` |
| `extension_id` | `vendor.northwind.meta.safe_demangle` |
| `target_transition_id` | `T-05` |
| `current_state` | `LS-2 Provisional` |
| `target_state` | `LS-1 Experimental` |
| `pilot_status` | `published-packet` |
| `owner` | `Lane C pilot operations owner` |
| `last_verified_date` | `2026-02-23` |

## 1. Intake evidence (`PW-02` / `PW-03`)

- Intake template contract: `templates/experimental_extension_proposal.md`
- Required evidence domains included: syntax, semantics, diagnostics, determinism, security.
- Intake readiness references:
  - `spec/planning/issue_152_extension_proposal_intake_template_package.md`
  - `spec/planning/evidence/issue_152/proposal_instance_ext_prop_0007.md`

## 2. Review evidence (`PW-04`)

- Rubric and scoring contract reference:
  - `spec/planning/issue_153_extension_review_rubric_package.md`
- Lifecycle rollback/deprecation linkage reference:
  - `spec/planning/issue_164_extension_lifecycle_states_package.md`
- Review board operating model reference:
  - `spec/planning/issue_170_review_board_operating_model_package.md`

## 3. Testing obligations evidence (`PW-05`)

- Seed traceability matrix (`EV-03`):
  - `tests/conformance/spec_open_issues/P0-P3-seed_traceability.csv`
- Seed summary (`EV-04`):
  - `reports/conformance/spec_open_issues_v011_summary.md`
- Dashboard and reproducibility inputs:
  - `reports/conformance/dashboard_v011.md`
  - `reports/conformance/dashboard_v011.status.json`
  - `reports/conformance/reproducibility/v011_rerun_digest_report.md`

## 4. Decision publication snapshot (`PW-07` / `PW-08`)

- Current gate decision publication:
  - `reports/releases/v011_quality_gate_decision.md`
  - `reports/releases/v011_quality_gate_decision.status.json`
- Current recommendation signal: `no-go` (`QG-04=fail`).
- Pilot disposition for this publication packet: `hold` pending blocker remediation and re-evaluation.

## 5. Pilot traceability block

trace.task_id: C-12
trace.dependency_ids: ["C-03", "C-04", "C-09", "C-11"]
trace.pilot_id: PILOT-002
trace.extension_id: vendor.northwind.meta.safe_demangle
trace.capability_ids: ["objc3.cap.demangle_roundtrip.v1"]
trace.transition_id: T-05
trace.gate_results:
- PG-01: pass
- PG-02: pass
- PG-03: fail
- PG-04: pass
- PG-05: pass
- PG-06: fail
- PG-07: fail
trace.board_decision_id: pending-board-decision-record
trace.registry_linkage_ref: pending-registry-update-after-gate-pass
trace.last_verified_date: 2026-02-23

## 6. Follow-ups

1. Clear hard blockers tracked in the quality-gate publication artifacts.
2. Publish pilot-specific decision record under `pilots/review_records/PILOT-002/decision_record.md` after board disposition is final.
