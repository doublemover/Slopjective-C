# M317-B002 Packet: Existing milestone and issue amendments implementation - Core feature implementation

## Contract

- Contract ID: `objc3c-cleanup-existing-milestone-issue-amendments-implementation/m317-b002-v1`
- Dependency contract: `objc3c-cleanup-immediate-backlog-surgery-supersession-model/m317-b001-v1`
- Implementation target file: `spec/planning/compiler/m317/m317_b002_existing_milestone_and_issue_amendments_implementation_core_feature_implementation_targets.json`
- Apply script: `tmp/github-publish/cleanup_acceleration_program/apply_m317_b002_existing_amendments.py`

## Scope

- Execute the remaining current-backlog amendments defined by the `M317` model, rather than only describing them.
- Keep the implementation focused on the existing overlap set, especially `M288`, whose milestone and issue bodies still need explicit boundary preservation.
- Preserve the earlier corrected runtime/storage issues without widening them or changing ownership again.

## Implementation targets

1. `m288_milestone_boundary_preservation`
   - Amend milestone `373` so it explicitly states that structural cleanup, validation collapse, command-surface reduction, proof hygiene, and governance belong to `M313-M318`.
2. `m288_issue_boundary_preservation`
   - Amend issues `#7529-#7538` with the same boundary-preserving scope note.
3. `corrective_issue_preservation`
   - Keep the already corrected `#7399`, `#7421`, `#7425`, `#7428`, `#7434`, `#7438`, and `#7441` aligned with the tracked amendment map.

## Acceptance

- The tracked implementation targets name the exact milestone and issue set touched by the implementation.
- The apply script writes a replayable report under `tmp/reports/m317/M317-B002/`.
- Live GitHub validation proves the amended milestone and issue bodies now carry the required boundary-preserving wording.
- Next issue: `M317-B003`.
