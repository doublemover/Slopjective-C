# M317-A001 Backlog Overlap, Supersession, And Publication-Scope Inventory

- generated_at: `2026-03-31T00:04:35.837301+00:00`
- github_open_milestones: `20`
- github_open_issues: `197`
- github_open_m317_issues: `6`
- local_total_milestones: `20`
- local_total_issues: `197`

## Current measured facts
- `package.json` scripts: `144`
- `check_*.py` files: `82`
- `test_check_*.py` files: `0`
- `m2xx-*` refs in `native/objc3c/src`: `0`
- tracked `.ll` files: `76`
- tracked `*stub*.ll` files: `0`

## Active owner surfaces
- `unified_draft_program_generation`
  owner: `tmp/planning/regenerate_draft_programs.py`
  status: `active`
  reason: All three draft programs now share one manifest-driven generator; the cleanup-only generator is redundant and drift-prone.
  supersedes: `tmp/planning/cleanup_acceleration_program/generate_cleanup_acceleration_program.py`
- `manifest_driven_publication_pipeline`
  owner: `tmp/github-publish/publish_draft_backlog_programs.py + tmp/github-publish/finalize_draft_backlog_publication.py`
  status: `active`
  reason: The new backlog set is published from generated manifests and dependency graphs, not from milestone-specific hardcoded publisher scripts.
  supersedes: `tmp/github-publish/final_runtime_completion_program/publish_final_runtime_completion_program.py`, `tmp/github-publish/post_m292_refined_program/publish_post_m292_refined_program.py`
- `cleanup_tranche_live_reuse`
  owner: `live GitHub milestones M313-M318`
  status: `active`
  reason: These milestones already existed on GitHub and were reopened/updated in place instead of being recreated with invented numbers.
  milestones: `M313`, `M314`, `M315`, `M316`, `M317`, `M318`
- `runtime_envelope_publication`
  owner: `runtime_envelope_completion_program manifests`
  status: `active`
  reason: The runtime-envelope tranche is now structurally separated from cleanup and post-adoption work, with direct blocker metadata carried in the generated manifests.
  milestones: `M319`, `M320`, `M321`, `M322`, `M323`, `M324`
- `post_m324_adoption_publication`
  owner: `post_m324_adoption_program manifests`
  status: `active`
  reason: Post-closure durability and adoption work now has its own manifest-driven program and no longer relies on the older post-M292 hardcoded publication packet.
  milestones: `M325`, `M326`, `M327`, `M328`, `M329`, `M330`, `M331`, `M332`

## Live M317 issues
- `#7833` [M317][Lane-E][E001] Backlog realignment closeout gate
- `#7832` [M317][Lane-D][D001] Publish-artifact and consistency-audit implementation
- `#7830` [M317][Lane-C][C001] Planning packet, manifest, and blocker-graph contract
- `#7828` [M317][Lane-B][B002] Existing draft and backlog realignment implementation
- `#7827` [M317][Lane-B][B001] Supersession, blocker, and publication policy
- `#7825` [M317][Lane-A][A001] Backlog overlap, supersession, and publication-scope inventory

## Non-goals
- This inventory does not define the supersession policy itself; that belongs to M317-B001.
- This inventory does not change issue templates or publication helpers beyond identifying the active owner surfaces.
- This inventory does not re-open archived tmp/archive milestone-era artifacts; it classifies them as historical references only.

Next issue: `M317-B001`
