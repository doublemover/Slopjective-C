# M313-B004 Packet: Legacy validation quarantine and namespace policy - Core feature implementation

## Intent

Freeze the first real quarantine stage so later `M313` work can reduce historical milestone-local validation surfaces without ambiguity about what is active, what is retained, and what is fenced off.

## Contract

- Source of truth:
  - `docs/contracts/m313_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation_b004_expectations.md`
  - `spec/planning/compiler/m313/m313_b004_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation_policy.json`
- Verification:
  - `scripts/check_m313_b004_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation.py`
  - `tests/tooling/test_check_m313_b004_legacy_validation_quarantine_and_namespace_policy_core_feature_implementation.py`
  - `scripts/run_m313_b004_lane_b_readiness.py`

## Policy focus

- active cleanup-window validation namespaces
- retained non-milestone policy and operator surfaces
- quarantined historical milestone-local wrappers
- dynamic ratchet-stage measurement derived from the live tree

## Next issue

- Next issue: `M313-B005`.
