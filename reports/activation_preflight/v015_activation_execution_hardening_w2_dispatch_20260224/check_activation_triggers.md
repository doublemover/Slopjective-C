# Activation Trigger Check

- Mode: `offline-deterministic`
- Contract ID: `activation-seed-contract/v0.15`
- Fail closed: `true`
- Issues snapshot: `tmp/v015_m28_live_issues_snapshot.json`
- Milestones snapshot: `tmp/v015_m28_live_milestones_snapshot.json`
- Catalog JSON: `spec/planning/remaining_task_review_catalog.json`
- Open blockers JSON: _none_
- T4 governance overlay JSON: _none_
- Actionable statuses: `open`, `open-blocked`, `blocked`
- Trigger order: `T1-ISSUES`, `T2-MILESTONES`, `T3-ACTIONABLE-ROWS`, `T5-OPEN-BLOCKERS`
- Open blockers count: `0`
- Open blockers trigger fired: `false`
- Activation required: `true`
- T4 new scope publish: `false`
- T4 source: `default-false`
- Gate open: `true`
- Queue state: `dispatch-open`
- Exit code: `1`

## Snapshot Freshness

| Snapshot | Requested | Max age (s) | Generated at UTC | Age (s) | Fresh |
| --- | --- | --- | --- | --- | --- |
| Issues | `true` | `86400` | `2026-02-24T21:57:36Z` | `2` | `true` |
| Milestones | `true` | `86400` | `2026-02-24T21:57:36Z` | `2` | `true` |

## Trigger Results

| Trigger ID | Fired | Count | Condition |
| --- | --- | --- | --- |
| `T1-ISSUES` | `true` | 140 | open issues > 0 |
| `T2-MILESTONES` | `true` | 14 | open milestones > 0 |
| `T3-ACTIONABLE-ROWS` | `false` | 0 | actionable catalog rows > 0 |
| `T5-OPEN-BLOCKERS` | `false` | 0 | open blockers > 0 |

- Active triggers: `T1-ISSUES`, `T2-MILESTONES`
