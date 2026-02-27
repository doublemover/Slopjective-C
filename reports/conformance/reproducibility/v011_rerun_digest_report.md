# v0.11 Reproducibility Rerun Digest Report (`EV-04`) {#v011-rerun-digest-report}

_Generated for `V013-CONF-01` on 2026-02-23 (issue `#789`, batch `BATCH-20260223-11S`)._

## 1. Scope

This digest summarizes deterministic rerun outcomes used by the v0.13 evidence
pack (`EV-01`..`EV-05`) and downstream quality-gate consumers.

## 2. Rerun Matrix

| Rerun ID | Profile | Input set | Result | Deterministic signal |
| --- | --- | --- | --- | --- |
| `RR-V011-01` | `core` | `B-04`,`B-10`,`B-11`,`B-12` | `pass` | Profile outcome stable versus prior dashboard snapshot. |
| `RR-V011-02` | `strict` | `B-04`,`B-10`,`B-11`,`B-12` | `incomplete` | `B-12` stale dependency preserved and explicitly tracked. |
| `RR-V011-03` | `strict-concurrency` | `B-04`,`B-10`,`B-11`,`B-12` | `fail` | `BLK-B11-RESULT-FAIL-001` remains unresolved. |
| `RR-V011-04` | `strict-system` | `B-04`,`B-10`,`B-11`,`B-12` | `blocked` | `BLK-B10-PROFILE-GAP-001` remains unresolved. |

## 3. Blocker Carry-Forward Digest

| Blocker ID | Severity | State | Owner | Downstream impact |
| --- | --- | --- | --- | --- |
| `BLK-B10-PROFILE-GAP-001` | `high` | `open` | `lane:B10` | Blocks strict-system gate acceptance. |
| `BLK-B11-RESULT-FAIL-001` | `critical` | `open` | `lane:B11` | Blocks strict-concurrency gate acceptance. |
| `BLK-B12-STALE-001` | `high` | `open` | `lane:B12` | Keeps strict-family profiles in non-pass state until refresh. |

## 4. Downstream Dependency Binding

| Consumer seed | Dependency path | Required fields from rerun digest |
| --- | --- | --- |
| `V013-CONF-02` | `V013-CONF-01 -> V013-CONF-02` | `Rerun Matrix`, `Blocker Carry-Forward Digest` |
| `V013-REL-01` | `V013-CONF-01 -> V013-CONF-02 -> V013-REL-01` | Stable blocker ledger and profile outcome summary |

## 5. Validation Transcript

| Validation ID | Command | Exact output | Exit code | Result |
| --- | --- | --- | --- | --- |
| `V013-CONF01-VAL-01` | `python scripts/spec_lint.py` | `spec-lint: OK` | `0` | `PASS` |
| `V013-CONF01-VAL-02` | `python scripts/check_issue_checkbox_drift.py` | `issue-drift: OK (mismatch count: 0; checked row count: 28)` | `0` | `PASS` |

### 5.1 `python scripts/spec_lint.py`

```text
spec-lint: OK
```

Exit code: `0`

### 5.2 `python scripts/check_issue_checkbox_drift.py`

```text
issue-drift: OK (mismatch count: 0; checked row count: 28)
```

Exit code: `0`
