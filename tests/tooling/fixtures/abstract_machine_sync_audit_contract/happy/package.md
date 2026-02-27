# Fixture: M14 Package Happy Path

## 8. M13 Abstract Machine Sync Audit W1 Hardening Addendum (`AMSA-DEP-M13-*`)

### 8.2 Dependency rows

| Dependency ID | Type | Deterministic semantic rule | Fail criteria | Escalation owner | Unblock condition | Linked acceptance row |
| --- | --- | --- | --- | --- | --- | --- |
| `AMSA-DEP-M13-01` | `Hard` | This package remains canonical lane-A authority for M13 abstract-machine sync audit dependency semantics and fail-closed baseline controls. | Canonical source omitted. | `lane-a-owner` | Restore source linkage. | `AC-V014-M13-01` |
| `AMSA-DEP-M13-02` | `Hard` | `reports/spec_sync/abstract_machine_audit_2026Q2.md` remains deterministic source authority for drift IDs/classes/status, REL-03 impact, and remediation-priority semantics. | Report source drifted. | `lane-a-owner` | Restore report-source linkage. | `AC-V014-M13-03` |
| `AMSA-DEP-M13-03` | `Hard` | `spec/planning/v013_seed_source_reconciliation_package.md` remains deterministic source authority for reconciled contract ID, Part 0/3/10 closure status, conflict decisions, and `V013-SPEC-03` consumer binding fields. | Reconciliation source drifted. | `lane-a-owner` | Restore reconciliation linkage. | `AC-V014-M13-04` |
| `AMSA-DEP-M13-04` | `Hard` | M13 matrix/evidence schema must publish stable `DEP/CMD/EVID/AC` IDs with deterministic command and evidence-anchor mapping. | Schema IDs unstable. | `lane-a-owner` | Restore schema IDs. | `AC-V014-M13-05` |
| `AMSA-DEP-M13-05` | `Hard` | Every M13 acceptance row must declare dependency type, fail criteria, escalation owner, and unblock condition. | Acceptance-row schema incomplete. | `lane-a-owner` | Restore required fields. | `AC-V014-M13-06` |
| `AMSA-DEP-M13-06` | `Soft` | Advisory drift handling is `HOLD` only when drift IDs/status/owner/ETA/evidence metadata remain explicit while all hard controls remain intact. | Advisory metadata missing. | `lane-a-owner` | Add metadata fields. | `AC-V014-M13-07` |
| `AMSA-DEP-M13-07` | `Hard` | M13 disposition semantics are fail-closed with explicit `PASS`/`HOLD`/`FAIL` rules and no waiver path for hard failures. | Hard failure bypassed by waiver. | `lane-a-owner` | Reinstate fail-closed semantics. | `AC-V014-M13-08` |
| `AMSA-DEP-M13-08` | `Hard` | Repository lint validator (`python scripts/spec_lint.py`) is blocking for lane-A-owned M13 artifacts. | Lint command fails. | `lane-a-owner` | Fix lint findings. | `AC-V014-M13-09` |

### 8.3 Deterministic disposition rule

1. `PASS`: hard rows (`AMSA-DEP-M13-01`, `AMSA-DEP-M13-02`, `AMSA-DEP-M13-03`, `AMSA-DEP-M13-04`, `AMSA-DEP-M13-05`, `AMSA-DEP-M13-07`, `AMSA-DEP-M13-08`) pass with linked evidence.
2. `HOLD`: only soft row `AMSA-DEP-M13-06` remains open with explicit owner, ETA, and bounded-risk note while all hard rows remain `PASS`.
3. `FAIL`: any hard row fails, required evidence mapping is missing, or soft-row drift is used to bypass hard controls.
4. Fail-closed default: if evidence is ambiguous, missing, or contradictory, disposition is `FAIL` until deterministic alignment is restored.
