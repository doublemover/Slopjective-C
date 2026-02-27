# Remaining Task Backlog View

- Input: `tests/tooling/fixtures/remaining_tasks_status_integrity/catalog_valid_status.json`
- Group by: `lane`
- Status filters: `open`, `open-blocked`, `blocked`
- Lane filters: _all lanes_

## Summary

- Total tasks: **6**
- Status counts: `open`=3, `open-blocked`=1, `blocked`=2
- Lane counts: `A`=3, `B`=3
- Path counts: `spec/planning/alpha.md`=3, `spec/planning/beta.md`=2, `spec/planning/zeta.md`=1
- Capacity status counts: `pass`=4
- Dispatch-intake: `pass` (`go`)
- Overlap conflicts: 0 (max allowed: 0; status=`pass`; intake recommendation: `go`)

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

_none_

## lane `A` (3 tasks)

| task_id | title | lane | status | path | line |
| --- | --- | --- | --- | --- | --- |
| `SPT-2003` | Open blocked alpha lane A | `A` | `open-blocked` | `spec/planning/alpha.md` | 12 |
| `SPT-2002` | Blocked alpha lane A | `A` | `blocked` | `spec/planning/alpha.md` | 30 |
| `SPT-2007` | Open beta lane A | `A` | `open` | `spec/planning/beta.md` | 7 |

## lane `B` (3 tasks)

| task_id | title | lane | status | path | line |
| --- | --- | --- | --- | --- | --- |
| `SPT-2005` | Open alpha lane B | `B` | `open` | `spec/planning/alpha.md` | 11 |
| `SPT-2008` | Blocked beta lane B | `B` | `blocked` | `spec/planning/beta.md` | 3 |
| `SPT-2001` | Open zeta lane B | `B` | `open` | `spec/planning/zeta.md` | 8 |
