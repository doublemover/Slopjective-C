# Remaining Task Backlog View

- Input: `tests/tooling/fixtures/remaining_tasks_status_integrity/catalog_valid_status.json`
- Group by: `lane`
- Status filters: `open`, `open-blocked`, `blocked`
- Lane filters: _all lanes_

## Summary

- Total tasks: **6**
- Status counts: `open`=3, `open-blocked`=1, `blocked`=2
- Lane counts: `A`=3, `B`=3
- Path counts: `docs/reference/legacy_spec_anchor_index.md`=6
- Capacity status counts: `pass`=4
- Dispatch-intake: `fail` (`no-go`)
- Overlap conflicts: 1 (max allowed: 0; status=`fail`; intake recommendation: `no-go`)

## Capacity Baseline

- Global capacity: 6/16 (load_ratio=0.3750, status=`pass`)
- Global dispatch state: `dispatch-open` (intake recommendation: `go`)

| lane | active_issue_count | lane_wip_cap | load_ratio | status | dispatch_state | intake_recommendation | escalations |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `A` | 3 | 4 | 0.7500 | `pass` | `dispatch-open` | `go` | _none_ |
| `B` | 3 | 5 | 0.6000 | `pass` | `dispatch-open` | `go` | _none_ |
| `C` | 0 | 4 | 0.0000 | `pass` | `dispatch-open` | `go` | _none_ |
| `D` | 0 | 3 | 0.0000 | `pass` | `dispatch-open` | `go` | _none_ |

## Overlap Conflicts

| path | active_issue_count | lane_count | lanes | escalation |
| --- | --- | --- | --- | --- |
| `docs/reference/legacy_spec_anchor_index.md` | 3 | 2 | `A`, `B` | `ESC-MERGE-01` |

## lane `A` (3 tasks)

| task_id | title | lane | status | path | line |
| --- | --- | --- | --- | --- | --- |
| `SPT-2007` | Open beta lane A | `A` | `open` | `docs/reference/legacy_spec_anchor_index.md` | 7 |
| `SPT-2003` | Open blocked alpha lane A | `A` | `open-blocked` | `docs/reference/legacy_spec_anchor_index.md` | 12 |
| `SPT-2002` | Blocked alpha lane A | `A` | `blocked` | `docs/reference/legacy_spec_anchor_index.md` | 30 |

## lane `B` (3 tasks)

| task_id | title | lane | status | path | line |
| --- | --- | --- | --- | --- | --- |
| `SPT-2008` | Blocked beta lane B | `B` | `blocked` | `docs/reference/legacy_spec_anchor_index.md` | 3 |
| `SPT-2001` | Open zeta lane B | `B` | `open` | `docs/reference/legacy_spec_anchor_index.md` | 8 |
| `SPT-2005` | Open alpha lane B | `B` | `open` | `docs/reference/legacy_spec_anchor_index.md` | 11 |
