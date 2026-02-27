# Fixture Package

| Field | Value |
| --- | --- |
| `seed_id` | `V013-GOV-04` |
| `wave_id` | `W1` |
| `batch_id` | `BATCH-20260223-11S` |
| `acceptance_gate_id` | `AC-V013-GOV-04` |

| Check ID | Marker |
| --- | --- |
| `SMT-V013-05` | coverage marker |
| `RT-V013-T4` (Low) | coverage marker |
| `FRL-V013-05` | `SMT-V013-05` |
| `AC-V013-GOV-04-06` | coverage marker |

## 10. M11 Macro Security Tabletop Ops W1 Addendum (`MSTP-DEP-M11-*`)

| Dependency ID | Type | Deterministic semantic rule |
| --- | --- | --- |
| `MSTP-DEP-M11-06` | `Soft` | Open remediation drift is advisory `HOLD` only when rows retain explicit owner, due date, and evidence hook while all hard controls remain intact. |
| `MSTP-DEP-M11-07` | `Hard` | M11 disposition semantics are fail-closed with explicit `PASS`/`HOLD`/`FAIL` rules and no waiver path for hard failures. |

1. `PASS`: coverage marker
2. `HOLD`: coverage marker
3. `FAIL`: coverage marker
4. Fail-closed default: if evidence is ambiguous, missing, or contradictory, disposition is `FAIL` until deterministic alignment is restored.
