# M313-A002 Packet: Validation reduction targets and ratchet map - Source completion

## Intent

Freeze the numeric reduction targets and staged ratchets that later `M313` issues must consume while collapsing migration-only validation surfaces.

## Contract

- Source of truth:
  - `docs/contracts/m313_validation_reduction_targets_and_ratchet_map_source_completion_a002_expectations.md`
  - `spec/planning/compiler/m313/m313_a002_validation_reduction_targets_and_ratchet_map_source_completion_ratchet_map.json`
- Verification:
  - `scripts/check_m313_a002_validation_reduction_targets_and_ratchet_map_source_completion.py`
  - `tests/tooling/test_check_m313_a002_validation_reduction_targets_and_ratchet_map_source_completion.py`
  - `scripts/run_m313_a002_lane_a_readiness.py`

## Ratchet focus

- imported `M313-A001` baselines
- stage-by-stage numeric caps for migration-only surfaces
- no-growth enforcement until exception policy exists
- explicit separation between reduction targets and active acceptance inputs

## Next issue

- Next issue: `M313-A003`.
