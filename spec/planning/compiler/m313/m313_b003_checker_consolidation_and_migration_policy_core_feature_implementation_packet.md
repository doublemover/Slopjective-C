# M313-B003 Packet: Checker consolidation and migration policy - Core feature implementation

## Intent

Freeze the consolidation routes that later `M313` issues must consume while replacing milestone-local validation wrappers with shared executable acceptance surfaces.

## Contract

- Source of truth:
  - `docs/contracts/m313_checker_consolidation_and_migration_policy_core_feature_implementation_b003_expectations.md`
  - `spec/planning/compiler/m313/m313_b003_checker_consolidation_and_migration_policy_core_feature_implementation_policy.json`
- Verification:
  - `scripts/check_m313_b003_checker_consolidation_and_migration_policy_core_feature_implementation.py`
  - `tests/tooling/test_check_m313_b003_checker_consolidation_and_migration_policy_core_feature_implementation.py`
  - `scripts/run_m313_b003_lane_b_readiness.py`

## Policy focus

- consolidation routes for the major milestone-local validation families
- migration defaults that require shared-harness-first validation
- retained static-guard classes and compatibility bridge ownership
- immediate prohibited patterns for new milestone-local wrappers

## Next issue

- Next issue: `M313-B004`.
