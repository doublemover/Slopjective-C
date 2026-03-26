# M313-B005 Packet: New-checker and temporary-validation exception policy - Edge case and compatibility completion

## Intent

Freeze the waiver policy that prevents the cleanup program from immediately regressing back into ad hoc checker and readiness growth.

## Contract

- Source of truth:
  - `docs/contracts/m313_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_b005_expectations.md`
  - `spec/planning/compiler/m313/m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_policy.json`
  - `spec/planning/compiler/m313/m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion_exception_registry.json`
- Verification:
  - `scripts/check_m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion.py`
  - `tests/tooling/test_check_m313_b005_new_checker_and_temporary_validation_exception_policy_edge_case_and_compatibility_completion.py`
  - `scripts/run_m313_b005_lane_b_readiness.py`

## Policy focus

- default-no-growth rule for new checker-style surfaces
- required waiver record fields and lifecycle
- empty-by-default registry for active waivers
- handoff into the lane-C artifact and bridge work

## Next issue

- Next issue: `M313-C001`.
