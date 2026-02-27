# RC v0.11 Toolchain Dry Run Report (`B-14`)

_Run execution date (UTC): 2026-02-23_

## 1. Run Metadata

| Field | Value |
| --- | --- |
| `dry_run_id` | `RC-DRY-20260223-001` |
| `release_label` | `v0.11` |
| `release_id` | `20260223-issue173-laneb-011` |
| `source_revision` | `7f3c2b1f4f2f0df1f2e35a5d7ef6f71f5ebbf3a1` |
| `dashboard_snapshot_generated_at` | `2026-02-23T18:30:00Z` |
| `run_window_start_utc` | `2026-02-23T18:35:00Z` |
| `run_window_end_utc` | `2026-02-23T19:05:00Z` |
| `triage_review_window_utc` | `2026-02-24T16:00:00Z` |
| `recommendation_review_window_utc` | `2026-02-24T18:00:00Z` |
| `b14_dri` | `B-LEAD` |
| `b14_triage` | `B-LEAD` |
| `b14_profile` | `B-LEAD` |
| `b14_evidence` | `B-LEAD` |
| `b14_gate` | `D-LEAD` |

## 2. Preflight Checklist Outcomes (`PFG-01`..`PFG-08`)

| Preflight ID | Gate class | Result | Evidence |
| --- | --- | --- | --- |
| `PFG-01` | `Blocking` | `PASS` | `reports/conformance/dashboard_v011.status.json` contains `release_id`, `source_revision`, and `generated_at`. |
| `PFG-02` | `Blocking` | `PASS` | Dashboard status references schema `objc3-conformance-dashboard-status/v1` and `reports/conformance/dashboard_v011.md` `DB-SEC-04` records schema artifact `ART-B13-DASHBOARD-SCHEMA` as `valid`. |
| `PFG-03` | `Blocking` | `PASS` | Profile rows present for `core`, `strict`, `strict-concurrency`, `strict-system` in `profiles[]`. |
| `PFG-04` | `Blocking` | `PASS` | Dependency rows present for `B-04`, `B-10`, `B-11`, `B-12` with `status` and `refreshed_at` fields. |
| `PFG-05` | `Blocking` | `PASS` | Every `artifacts[].artifact_path` referenced by dependency rows resolves in-repo (see Section 9 `VAL-179-05`). |
| `PFG-06` | `Blocking` | `PASS` | Required owner roles and backups are assigned in this report metadata and `spec/planning/issue_179_rc_toolchain_dry_run_package.md` Section 8. |
| `PFG-07` | `Blocking` | `PASS` | Run window and review windows are timestamped in Section 1. |
| `PFG-08` | `Advisory` | `PASS` | Prior-run continuity captured via `change_history[]` in `reports/conformance/dashboard_v011.status.json`. |

Preflight exit: `PFG-01`..`PFG-07` all `PASS`; dry-run execution proceeded.

## 3. Profile Verdict Matrix and Dependency Status Summary

### 3.1 Profile verdict matrix

| Profile | Verdict | `B-04` | `B-10` | `B-11` | `B-12` | Blockers |
| --- | --- | --- | --- | --- | --- | --- |
| `core` | `pass` | `pass` | `pass` | `pass` | `pass` | none |
| `strict` | `incomplete` | `pass` | `pass` | `pass` | `stale` | `BLK-B12-STALE-001` |
| `strict-concurrency` | `fail` | `pass` | `pass` | `fail` | `stale` | `BLK-B11-RESULT-FAIL-001`, `BLK-B12-STALE-001` |
| `strict-system` | `blocked` | `pass` | `blocked` | `pass` | `stale` | `BLK-B10-PROFILE-GAP-001`, `BLK-B12-STALE-001` |

### 3.2 Dependency summary

| Dependency | Status | Refreshed at (UTC) | Failure codes |
| --- | --- | --- | --- |
| `B-04` | `pass` | `2026-02-23T18:10:00Z` | none |
| `B-10` | `blocked` | `2026-02-23T18:05:00Z` | `DASH-B10-PROFILE-GAP` |
| `B-11` | `fail` | `2026-02-23T17:50:00Z` | `DASH-B11-RESULT-FAIL` |
| `B-12` | `stale` | `2026-02-22T08:30:00Z` | `DASH-B12-EXCEPTION-STALE` |

## 4. Execution Matrix Results (`RCM-01`..`RCM-04`)

| Matrix ID | Scenario | Deterministic result | Evidence |
| --- | --- | --- | --- |
| `RCM-01` | Baseline RC dry run on current candidate snapshot. | `COMPLETE` - snapshot evaluated; overall outcome `fail`. | Sections 1-3 of this report; `reports/conformance/dashboard_v011.status.json`. |
| `RCM-02` | Replay dry run on same snapshot (determinism check). | `COMPLETE` - replay on same snapshot yields identical profile/dependency/blocker counts (`1 pass`, `1 incomplete`, `1 fail`, `1 blocked`; `3` open blockers). | Section 3 plus `reports/conformance/dashboard_v011.status.json` `summary`. |
| `RCM-03` | Controlled stale-input simulation (negative path). | `COMPLETE` - stale `B-12` input drives expected non-pass outcomes (`strict` -> `incomplete`, `strict-system` -> `blocked`) and emits `high` blocker `BLK-B12-STALE-001`. | `reports/conformance/dashboard_v011.status.json` (`dependencies[B-12]=stale`, `blockers[]`). |
| `RCM-04` | Missing-artifact simulation (negative path). | `COMPLETE` - missing artifact probe returns `False`; dry-run classification is hard-block (`no-go`) under missing-artifact policy. | Section 9 `VAL-179-07` (`Test-Path reports/releases/rc_v011_missing_artifact_probe.txt`). |

## 5. Blocker Summary by Severity and State

| Severity | Open count | Blocker IDs |
| --- | --- | --- |
| `critical` | `1` | `BLK-B11-RESULT-FAIL-001` |
| `high` | `2` | `BLK-B10-PROFILE-GAP-001`, `BLK-B12-STALE-001` |
| `medium` | `0` | none |
| `low` | `0` | none |

| State | Count |
| --- | --- |
| `open` | `3` |
| `resolved` | `0` |

Detailed owner/SLA records are published in `reports/releases/rc_v011_blocker_triage.md` and `reports/releases/rc_v011_blocker_triage.status.json`.

## 6. Gate Recommendation

Recommendation: `no-go`

Rationale:

1. Profile hard-fail condition is met (`strict-concurrency=fail`, `strict-system=blocked`).
2. Unresolved `critical` blocker exists (`BLK-B11-RESULT-FAIL-001`).
3. Unresolved `high` blockers remain (`BLK-B10-PROFILE-GAP-001`, `BLK-B12-STALE-001`).
4. Release-level status in dashboard snapshot is `fail`.

## 7. Downstream Handoff Notes (`D-11`, `D-12`)

| Consumer | Handoff payload | State |
| --- | --- | --- |
| `D-11` conformance dress rehearsal | `reports/releases/rc_v011_dry_run.md`; `reports/releases/rc_v011_blocker_triage.md`; `reports/releases/rc_v011_blocker_triage.status.json` | `DELIVERED` |
| `D-12` readiness dossier | Same payload plus source dashboard snapshot (`reports/conformance/dashboard_v011.status.json`) | `DELIVERED WITH NO-GO FLAG` |

Handoff record token: `HANDOFF-B14-DLANE-20260223-01`.

## 8. Artifact Publication Record

| Artifact | Path | Publication state |
| --- | --- | --- |
| RC dry-run report | `reports/releases/rc_v011_dry_run.md` | `published` |
| RC blocker triage report | `reports/releases/rc_v011_blocker_triage.md` | `published` |
| RC blocker triage status export | `reports/releases/rc_v011_blocker_triage.status.json` | `published` |

## 9. Validation Commands Used for This Report

Run from repository root.

```powershell
python -c "import json, pathlib; d=json.loads(pathlib.Path('reports/conformance/dashboard_v011.status.json').read_text(encoding='utf-8')); print(d['release_id']); print(d['source_revision']); print(d['status'])"
python -c "import json, pathlib; d=json.loads(pathlib.Path('reports/conformance/dashboard_v011.status.json').read_text(encoding='utf-8')); paths=[a['artifact_path'] for a in d['artifacts'] if a['dependency_id'] in {'B-04','B-10','B-11','B-12'}]; missing=[p for p in paths if not pathlib.Path(p).exists()]; print('artifact-path-missing-count={0}'.format(len(missing)))"
Test-Path reports/releases/rc_v011_missing_artifact_probe.txt
```

Latest command outputs recorded during dry-run authoring:

- `VAL-179-05` (`artifact path check`) -> `artifact-path-missing-count=0`
- `VAL-179-07` (`missing artifact probe`) -> `False`
