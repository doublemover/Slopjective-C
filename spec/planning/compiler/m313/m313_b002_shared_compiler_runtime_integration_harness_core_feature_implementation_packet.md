# M313-B002 Packet: Shared compiler-runtime integration harness - Core feature implementation

## Intent

Implement the shared harness layer that later `M313` work will use instead of proliferating more issue-local readiness and checker flows.

## Contract

- Source of truth:
  - `docs/contracts/m313_shared_compiler_runtime_integration_harness_core_feature_implementation_b002_expectations.md`
  - `spec/planning/compiler/m313/m313_b002_shared_compiler_runtime_integration_harness_core_feature_implementation_registry.json`
- Verification:
  - `scripts/shared_compiler_runtime_acceptance_harness.py`
  - `scripts/check_m313_b002_shared_compiler_runtime_integration_harness_core_feature_implementation.py`
  - `tests/tooling/test_check_m313_b002_shared_compiler_runtime_integration_harness_core_feature_implementation.py`
  - `scripts/run_m313_b002_lane_b_readiness.py`

## Harness focus

- shared suite registry derived from `M313-A003`
- stable CLI for suite listing, suite inspection, and root validation
- summary output under `tmp/reports/`
- no new milestone-local validation namespace invention

## Next issue

- Next issue: `M313-B003`.
