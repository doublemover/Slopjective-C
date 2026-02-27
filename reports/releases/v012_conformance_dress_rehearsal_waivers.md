# v0.12 Conformance Dress Rehearsal Waiver Ledger

## 1. Ledger Metadata

| Field | Value |
| --- | --- |
| `ledger_id` | `D11-WVR-LEDGER-20260223-012` |
| `release_id` | `v0.12-locked-snapshot` |
| `owner` | `Lane-C` |
| `report_path` | `reports/releases/v012_conformance_dress_rehearsal.md` |
| `blocker_ledger_path` | `reports/releases/v012_conformance_dress_rehearsal_blockers.md` |
| `captured_at_utc` | `2026-02-23T22:39:00Z` |

## 2. Waiver Decision Records

| `waiver_id` | `linked_blocker_id` | `severity` | `justification` | `workaround_summary` | `risk_statement` | `approver` | `approved_at_utc` | `expires_at_utc` | `revalidation_trigger` | `decision` | `status` |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `D11-WVR-20260223-001` | `D11-BLK-20260223-001` | `P0` | `Request to waive unresolved release blocker during rehearsal closeout.` | `No workaround is policy-compliant for unresolved P0.` | `Waiving this blocker would invalidate no-go safety constraints.` | `Release lead` | `2026-02-23T22:36:00Z` | `2026-02-23T22:36:00Z` | `Close D11-BLK-20260223-001 with verified remediation and rerun.` | `rejected` | `inactive` |

## 3. Policy Assertions

- Waiver is not permitted for any unresolved `P0` blocker.
- `D11-BLK-20260223-001` remains `open` and therefore non-waivable.

## 4. Summary Rollup

| Metric | Value |
| --- | --- |
| `waiver_requests_total` | `1` |
| `waivers_approved` | `0` |
| `waivers_rejected` | `1` |
| `waivers_active` | `0` |
| `waivers_expired` | `0` |
| `non_waivable_p0_blockers` | `1` |
