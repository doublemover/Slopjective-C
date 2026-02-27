# Activation Trigger Check

- Mode: `offline-deterministic`
- Contract ID: `activation-seed-contract/v0.15`
- Fail closed: `true`
- Issues snapshot: `tests/tooling/fixtures/activation_triggers/release_scale_foundation_w4_happy/issues.json`
- Milestones snapshot: `tests/tooling/fixtures/activation_triggers/release_scale_foundation_w4_happy/milestones.json`
- Catalog JSON: `tests/tooling/fixtures/activation_triggers/release_scale_foundation_w4_happy/catalog.json`
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
| Issues | `false` | _none_ | _none_ | _none_ | _none_ |
| Milestones | `false` | _none_ | _none_ | _none_ | _none_ |

## Trigger Results

| Trigger ID | Fired | Count | Condition |
| --- | --- | --- | --- |
| `T1-ISSUES` | `false` | 0 | open issues > 0 |
| `T2-MILESTONES` | `false` | 0 | open milestones > 0 |
| `T3-ACTIONABLE-ROWS` | `true` | 1 | actionable catalog rows > 0 |
| `T5-OPEN-BLOCKERS` | `false` | 0 | open blockers > 0 |

- Active triggers: `T3-ACTIONABLE-ROWS`
