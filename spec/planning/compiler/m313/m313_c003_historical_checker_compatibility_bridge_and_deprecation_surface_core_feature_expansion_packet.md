# M313-C003 Packet: Historical checker compatibility bridge and deprecation surface - Core feature expansion

## Intent

Implement the compatibility bridge that lets later CI and governance work reason about historical checker families through explicit schema-compliant bridge artifacts instead of raw filename sprawl.

## Contract

- Source of truth:
  - `docs/contracts/m313_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion_c003_expectations.md`
  - `spec/planning/compiler/m313/m313_c003_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion_plan.json`
- Verification:
  - `scripts/m313_historical_checker_compatibility_bridge.py`
  - `scripts/check_m313_c003_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion.py`
  - `tests/tooling/test_check_m313_c003_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion.py`
  - `scripts/run_m313_c003_lane_c_readiness.py`

## Bridge focus

- route-family bridge definitions
- schema-compliant compatibility summaries
- measured legacy-wrapper counts per bridge family
- handoff to `M313-D001`

## Next issue

- Next issue: `M313-D001`.
