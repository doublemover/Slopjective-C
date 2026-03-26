# M313-A003 Packet: Subsystem acceptance-suite boundary and migration map - Source completion

## Intent

Freeze the subsystem acceptance-suite boundaries and migration ownership map so later `M313` implementation work lands against explicit suite targets instead of generic acceptance-suite placeholders.

## Contract

- Source of truth:
  - `docs/contracts/m313_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_a003_expectations.md`
  - `spec/planning/compiler/m313/m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion_map.json`
- Verification:
  - `scripts/check_m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion.py`
  - `tests/tooling/test_check_m313_a003_subsystem_acceptance_suite_boundary_and_migration_map_source_completion.py`
  - `scripts/run_m313_a003_lane_a_readiness.py`

## Boundary-map focus

- explicit subsystem suite IDs
- existing runtime/fixture roots feeding each suite
- migration-only feeder surfaces that should shrink over time
- owner issues for harness creation, quarantine, compatibility bridge, and CI aggregation

## Next issue

- Next issue: `M313-B001`.
