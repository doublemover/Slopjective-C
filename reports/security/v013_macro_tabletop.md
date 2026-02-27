# V013 Macro Security Tabletop Report (`V013-GOV-04`) {#v013-macro-security-tabletop-report}

_Report version 1.1 - 2026-02-23_

This report publishes deterministic tabletop outputs for issue `#790` and records playbook patch evidence for `spec/governance/macro_security_incident_playbook_v1.md`.

## 1. Run Metadata

| Field | Value |
| --- | --- |
| `report_id` | `V013-MACRO-TTX-20260223-02` |
| `seed_id` | `V013-GOV-04` |
| `acceptance_gate_id` | `AC-V013-GOV-04` |
| `wave_id` | `W1` |
| `issue_id` | `#790` |
| `batch_id` | `BATCH-20260223-11S` |
| `tabletop_window_start_utc` | `2026-02-23T14:00:00Z` |
| `tabletop_window_end_utc` | `2026-02-23T16:05:00Z` |
| `incident_simulation_clock` | `compressed 1h = 10m` |
| `run_owner` | `WL-GOV owner` |
| `participants` | `incident commander`; `security owner`; `tooling owner`; `provenance owner`; `release owner`; `governance delegate` |
| `artifact_targets` | `spec/planning/v013_macro_security_tabletop_package.md`; `spec/governance/macro_security_incident_playbook_v1.md`; `reports/security/v013_macro_tabletop.md` |

## 2. Exercise Charter

Deterministic goals:

1. Execute all `SMT-V013-*` scenarios with expected severity and tier binding.
2. Validate response tier SLAs and CER gate decisions against playbook v1.2.
3. Publish a follow-up remediation ledger with owner and due-date coverage.
4. Produce closeout-ready acceptance mapping for `AC-V013-GOV-04-*`.

## 3. Timeline Trace

| Timeline ID | UTC timestamp | Event | Outcome |
| --- | --- | --- | --- |
| `TTX-TL-01` | `2026-02-23T14:00:00Z` | Tabletop kickoff and role assignment. | `PASS` |
| `TTX-TL-02` | `2026-02-23T14:12:00Z` | `SMT-V013-01` inject delivered (`INC-SIGN`). | `SEV-1`, `RT-V013-T1` selected |
| `TTX-TL-03` | `2026-02-23T14:30:00Z` | `SMT-V013-02` inject delivered (`INC-PAYLOAD`). | `SEV-2`, `RT-V013-T2` selected |
| `TTX-TL-04` | `2026-02-23T14:54:00Z` | `SMT-V013-03` inject delivered (`INC-SOURCE` + `INC-TRUST`). | `SEV-2`, `RT-V013-T2` selected |
| `TTX-TL-05` | `2026-02-23T15:18:00Z` | `SMT-V013-04` inject delivered (`INC-PROV`). | `SEV-2`, `RT-V013-T2` selected |
| `TTX-TL-06` | `2026-02-23T15:42:00Z` | `SMT-V013-05` inject delivered (`INC-COORD`). | `SEV-1`, `RT-V013-T1` selected |
| `TTX-TL-07` | `2026-02-23T16:05:00Z` | Consolidated findings and remediation ledger approval. | `PASS` |

## 4. Scenario Matrix Outcomes

| Scenario ID | Inject summary | Expected severity/tier | Observed severity/tier | SLA adherence | Required outputs present | Scenario verdict |
| --- | --- | --- | --- | --- | --- | --- |
| `SMT-V013-01` | Forged signer metadata in release candidate. | `SEV-1` / `RT-V013-T1` | `SEV-1` / `RT-V013-T1` | `PASS` | denylist, revocation, `COMM-T1`, `COMM-T2` | `PASS` |
| `SMT-V013-02` | Mirror payload drift during publish. | `SEV-2` / `RT-V013-T2` | `SEV-2` / `RT-V013-T2` | `PASS` | quarantine, gate freeze, remediation plan | `PASS` |
| `SMT-V013-03` | Source substitution plus stale trust roots. | `SEV-2` / `RT-V013-T2` | `SEV-2` / `RT-V013-T2` | `PASS` | source block, trust refresh, maintainer notification | `PASS` |
| `SMT-V013-04` | Replay mismatch during recovery. | `SEV-2` / `RT-V013-T2` | `SEV-2` / `RT-V013-T2` | `PASS` | `CER-R` hold, replay forensics, updated guidance | `PASS` |
| `SMT-V013-05` | Coordinated multi-vendor compromise. | `SEV-1` / `RT-V013-T1` | `SEV-1` / `RT-V013-T1` | `PASS` | coordinated disclosure plan and vendor rota | `PASS` |

Scenario matrix summary: `5/5` scenarios passed severity/tier determinism checks.

## 5. Response Tier Performance

| Tier ID | Scenarios exercised | SLA target | Worst observed delta | Compliance |
| --- | --- | --- | --- | --- |
| `RT-V013-T1` | `SMT-V013-01`, `SMT-V013-05` | Ack <= 15m; containment <= 30m | Ack `+3m`; containment `+7m` | `PASS` |
| `RT-V013-T2` | `SMT-V013-02`, `SMT-V013-03`, `SMT-V013-04` | Ack <= 30m; containment <= 60m | Ack `+5m`; containment `+11m` | `PASS` |
| `RT-V013-T3` | none in this run | Ack <= 4h; containment <= 8h | not exercised | `N/A` |
| `RT-V013-T4` | none in this run | Ack <= 1 business day; triage <= 2 business days | not exercised | `N/A` |

## 6. CER and Communications Evidence

| Evidence ID | Scenario(s) | Evidence artifact | Result |
| --- | --- | --- | --- |
| `TTX-EV-01` | `SMT-V013-01` | `COMM-T1` internal advisory draft generated in-window. | `PASS` |
| `TTX-EV-02` | `SMT-V013-01`, `SMT-V013-05` | `COMM-T2` vendor coordination notice template completed with owner roster. | `PASS` |
| `TTX-EV-03` | `SMT-V013-02`, `SMT-V013-03` | `G-C1` containment decision records completed with IC + tooling owner approvals. | `PASS` |
| `TTX-EV-04` | `SMT-V013-04` | Recovery hold until replay checks passed; `G-R1` deferred per policy. | `PASS` |
| `TTX-EV-05` | `SMT-V013-05` | Coordinated disclosure timeline includes next-update timestamp commitment. | `PASS` |

## 7. Follow-Up Remediation Ledger (`FRL-V013-*`)

| Remediation ID | Linked scenario | Action | Owner | Due (UTC) | Status | Evidence hook |
| --- | --- | --- | --- | --- | --- | --- |
| `FRL-V013-01` | `SMT-V013-01` | Rotate signer keys and publish trust bundle version pinning guidance. | Security owner | `2026-03-02T18:00:00Z` | `in_progress` | Playbook `12.2` |
| `FRL-V013-02` | `SMT-V013-02` | Add deterministic mirror quarantine replay script runbook to containment ops. | Tooling owner | `2026-03-04T18:00:00Z` | `open` | Package `5.2` |
| `FRL-V013-03` | `SMT-V013-03` | Enforce trust-root freshness guardrail for all release runners. | Release owner | `2026-03-06T18:00:00Z` | `open` | Playbook `12.2` |
| `FRL-V013-04` | `SMT-V013-04` | Add replay mismatch stop condition to recovery gate checklist. | Provenance owner | `2026-03-03T18:00:00Z` | `in_progress` | Playbook `11.3` |
| `FRL-V013-05` | `SMT-V013-05` | Publish coordinated vendor contact rota with deterministic fallback owners. | Governance delegate | `2026-03-05T18:00:00Z` | `open` | Package `5.2` |

Ledger completeness check:

- rows with owner: `5/5`
- rows with due date: `5/5`
- rows with evidence hook: `5/5`

## 8. Deterministic Playbook Update Publication

| Patch ID | Update applied | Verification |
| --- | --- | --- |
| `PBK-V013-01` | Added scenario matrix addendum (`SMT-V013-*`) to playbook Section `10`. | Scenario IDs in report and package match playbook identifiers exactly. |
| `PBK-V013-02` | Added response tier policy (`RT-V013-*`) to playbook Section `11`. | Tier/SLA outcomes in Section `5` map directly to playbook rules. |
| `PBK-V013-03` | Added remediation ledger, metadata binding, and `AC-V013-GOV-04` mapping to playbook Section `12`. | `FRL-V013-*`, issue metadata tuple, and acceptance rows match package/report mappings. |

## 9. Acceptance Checklist Mapping (`AC-V013-GOV-04`)

### 9.1 Deterministic acceptance rows

| Acceptance row | Criterion | Report evidence | Status |
| --- | --- | --- | --- |
| `AC-V013-GOV-04-01` | Scenario matrix is published and class-complete. | Section `4` includes `SMT-V013-01`..`SMT-V013-05` outcomes. | `PASS` |
| `AC-V013-GOV-04-02` | Response tiers are deterministic and severity-bound. | Section `5` reports tier/SLA outcomes mapped to `RT-V013-*`. | `PASS` |
| `AC-V013-GOV-04-03` | Playbook updates codify tabletop outputs. | Section `8` verifies `PBK-V013-01`..`03` publication. | `PASS` |
| `AC-V013-GOV-04-04` | Follow-up remediation ledger is complete. | Section `7` includes owner, due date, status, evidence for all rows. | `PASS` |
| `AC-V013-GOV-04-05` | Tabletop output report includes timeline and deterministic decisions. | Sections `3`, `4`, and `6` capture timeline, scenario verdicts, and CER evidence. | `PASS` |
| `AC-V013-GOV-04-06` | Validation transcript for `python scripts/spec_lint.py` is recorded. | Section `10` includes command, output, and exit status. | `PASS` |

### 9.2 Closeout checklist

- [x] `AC-V013-GOV-04-01` Scenario matrix output published.
- [x] `AC-V013-GOV-04-02` Response tiers and SLA outcomes recorded.
- [x] `AC-V013-GOV-04-03` Playbook deterministic updates published.
- [x] `AC-V013-GOV-04-04` Follow-up remediation ledger published.
- [x] `AC-V013-GOV-04-05` Timeline and CER evidence captured.
- [x] `AC-V013-GOV-04-06` `spec_lint` transcript captured.

## 10. Validation Transcript (`python scripts/spec_lint.py`)

| Command | Observed output | Exit code | Status |
| --- | --- | --- | --- |
| `python scripts/spec_lint.py` | `spec-lint: OK` | `0` | `PASS` |
