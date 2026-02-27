# Fixture Report

| Field | Value |
| --- | --- |
| `seed_id` | `V013-GOV-04` |
| `wave_id` | `W1` |
| `batch_id` | `BATCH-20260223-11S` |
| `acceptance_gate_id` | `AC-V013-GOV-04` |

Scenario matrix summary: `5/5` scenarios passed severity/tier determinism checks.

| Row | Value |
| --- | --- |
| `FRL-V013-05` | `SMT-V013-05` |
| `PBK-V013-03` | Added remediation ledger, metadata binding, and `AC-V013-GOV-04` mapping to playbook Section `12`. |
| `AC-V013-GOV-04-06` | Validation transcript for `python scripts/spec_lint.py` is recorded. |

| Evidence ID | Scenario | Evidence artifact | Result |
| --- | --- | --- | --- |
| `TTX-EV-04` | `SMT-V013-04` | Recovery hold until replay checks passed; `G-R1` deferred per policy. | `PASS` |

Ledger completeness check:

- rows with owner: `5/5`
- rows with due date: `5/5`
- rows with evidence hook: `5/5`
