# M317-A002 Packet: Current GitHub issue amendment and supersession map - Source completion

## Contract

- Contract ID: `objc3c-cleanup-current-github-amendment-supersession-map/m317-a002-v1`
- Dependency contract: `objc3c-cleanup-backlog-overlap-correction-inventory/m317-a001-v1`
- Source map: `spec/planning/compiler/m317/m317_a002_current_github_issue_amendment_and_supersession_map_source_completion_map.json`

## Scope

- Turn the A001 overlap inventory into one explicit current-state amendment and supersession map.
- Freeze the exact issue-level and milestone-level backlog entries that later `M317-B*` work must preserve or rewrite.
- Keep this issue at the source-completion boundary: it records the current GitHub shape and expected wording, but does not perform new amendment work beyond landing the tracked map and validation bundle.

## Required map families

1. `current_issue_amendments`
   - The exact amended issue set and the wording each issue must now carry.
2. `milestone_boundary_entries`
   - The milestones whose descriptions now enforce cleanup-first blocking or boundary preservation.
3. `followup_handling`
   - The concept-only state of `#7764`.
4. `metadata_targets`
   - The open-backlog label and execution-order invariants.

## Acceptance

- The tracked map names the exact amended issues, milestone entries, snippets, and dependency relationships.
- Live GitHub validation proves the map still matches the current backlog state.
- Next issue: `M317-B001`.
