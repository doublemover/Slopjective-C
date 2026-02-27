# Fixture Playbook

### 10.1 Scenario matrix contract (`SMT-V013-*`)

| Scenario ID | Incident class | Notes |
| --- | --- | --- |
| `SMT-V013-05` | `INC-COORD` | coverage marker |

### 11.1 Tier definitions

| Tier ID | Severity | Notes |
| --- | --- | --- |
| `RT-V013-T4` | `SEV-4` | coverage marker |

### 11.2 Tier transition and override rules

1. coverage marker
2. coverage marker
3. tier downgrade MUST NOT occur before `CER-C` completion criteria are met,

### 12.1 Remediation ledger contract (`FRL-V013-*`)

| Remediation ID | Linked scenario | Notes |
| --- | --- | --- |
| `FRL-V013-05` | `SMT-V013-05` | coverage marker |

### 12.4 Reseed metadata binding (`#790`, `BATCH-20260223-11S`)

| Field | Bound value |
| --- | --- |
| `acceptance_gate_id` | `AC-V013-GOV-04` |
