# M317-E002 Packet: Backlog publication closeout matrix - Cross-lane integration sync

## Intent

Close `M317` by replaying the landed backlog realignment evidence, validating that the live GitHub backlog still matches the corrected planning model, and handing the cleanup-first sequence forward to `M313-A001`.

## Contract

- Source of truth:
  - `docs/contracts/m317_backlog_publication_closeout_matrix_cross_lane_integration_sync_e002_expectations.md`
  - `spec/planning/compiler/m317/m317_e002_backlog_publication_closeout_matrix_cross_lane_integration_sync_contract.json`
- Verification:
  - `scripts/check_m317_e002_backlog_publication_closeout_matrix_cross_lane_integration_sync.py`
  - `tests/tooling/test_check_m317_e002_backlog_publication_closeout_matrix_cross_lane_integration_sync.py`
  - `scripts/run_m317_e002_lane_e_readiness.py`

## Closeout focus

- predecessor proof-chain continuity from `M317-A001` through `M317-E001`
- live GitHub backlog metadata cleanliness
- preserved `M288` boundary notes and `M293-M308` post-cleanup dependency rewrites
- preserved template and seed-generator simplification surfaces
- pre-closeout milestone-open-issue condition for the final closeout issue

## Next issue

- Next issue: `M313-A001`.
