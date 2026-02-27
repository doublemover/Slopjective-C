# Fixture: Activation Reopen Playbook (W2 Drift)

_Deterministic dispatch-reopen contract for issue `#778`._

## 3. Gate-Open Criteria (Canonical Reduction)

3. `gate_open = tooling_activation OR t4_new_scope_publish`

## 4. Runner-Based Preflight Contract (`AR-PREFLIGHT-RUNNER-V1`)

1. `AR-RUN-01`
2. `AR-RUN-08`
3. `AR-RUN-09`
4. `AR-RUN-02`
5. `AR-RUN-03`
6. `AR-RUN-04`
7. `AR-RUN-05`
8. `AR-RUN-06`
9. `AR-RUN-07`

### 4.1 Dependency Semantics Baseline (`AR-DEP-M03-*`)

| Dependency ID | Type | Deterministic semantic rule |
| --- | --- | --- |
| `AR-DEP-M03-01` | `Hard` | Required runner chain is `AR-RUN-01`, `AR-RUN-08`, `AR-RUN-09`, `AR-RUN-02`, `AR-RUN-03`, `AR-RUN-04`, `AR-RUN-05`, `AR-RUN-06`, `AR-RUN-07` in exactly that order. |

## 5. Required Evidence Captures

| Evidence ID | Required payload |
| --- | --- |
| `AR-CMD-08` | Snapshot refresh output plus exit code. |
| `AR-CMD-09` | Freshness assertion output plus exit code. |
| `AR-SNAPSHOT-01` | Snapshot refresh record linked to the active cycle. |
| `AR-DECISION-01` | Final reopen decision record with cycle linkage. |

## 6. Activation Governance Hard Gates (`G0`..`G8`)

| Gate ID | Dependency type | Hard-gate requirement |
| --- | --- | --- |
| `G0-TRIGGER` | `Hard` | Activation checker reduction reports `gate_open=true`. |
| `G1-PREFLIGHT` | `Hard` | Runner contract required steps complete with evidence. |
| `G2-OWNERSHIP` | `Hard` | Lane ownership boundaries are published with no overlaps. |
| `G3-DEPENDENCY` | `Hard` | Initial dependency map and merge sequence are accepted. |
| `G4-SEQUENCER` | `Hard` | Next activation tranche sequencing is explicit. |
| `G5-SEED-PARITY` | `Hard` | Seed-generation parity has no drift. |
| `G6-CANONICAL-PAYLOAD-PUBLICATION` | `Hard` | Canonical payload digest is captured. |
| `G7-SNAPSHOT-REFRESH` | `Hard` | Snapshot refresh evidence is linked to the cycle. |
| `G8-SNAPSHOT-FRESHNESS-EVIDENCE` | `Hard` | Snapshot freshness evidence proves live activation snapshot inputs are within hard freshness window at decision time. |

Deterministic fail-closed rule:

- If any `Hard` gate in Section 6 is `FAIL`, reopen state is `BLOCKED`.

## 8.2 Hard/Soft Gate Matrix (`AR-DRW1-*`)

| Gate ID | Class | Dependency link |
| --- | --- | --- |
| `AR-DRW1-G0-TRIGGER` | `Hard` | `AR-DEP-M17-01` |
| `AR-DRW1-G1-EVIDENCE-LINKAGE` | `Hard` | `AR-DEP-M17-02` |
| `AR-DRW1-G3-LANE-EVIDENCE` | `Hard` | `AR-DEP-M17-03` |
| `AR-DRW1-G2-ACCEPTANCE-MATRIX` | `Hard` | `AR-DEP-M17-03` |
| `AR-DRW1-G4-SPEC-LINT` | `Hard` | `AR-DEP-M17-03` |
| `AR-DRW1-S1-T4-PROVENANCE` | `Soft` | `AR-DEP-M17-04` |
| `AR-DRW1-S2-INTAKE-ORDER` | `Soft` | `AR-DEP-M17-05` |

### 8.3 Dispatch-Reopen State Reduction (`AR-DRW1-STATE-V1`)

1. If any `Hard` gate (`AR-DRW1-G0`..`AR-DRW1-G4`) is `FAIL`, `dispatch_reopen_state=BLOCKED`.
2. If all hard gates pass and unresolved soft controls remain, `dispatch_reopen_state=HOLD`.
3. If `AR-DRW1-S1-T4-PROVENANCE` fails in a `T4`-only cycle, `dispatch_reopen_state=BLOCKED`.
4. Dispatch may move to `READY` only when all required controls pass.

## 9. Anti-Fabrication Guardrail

This playbook does not create tasks, issues, or milestones. It defines only the deterministic reopen contract and evidence requirements.
