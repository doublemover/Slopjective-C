# M317-C002 Packet: Issue template and seed-generator simplification implementation - Core feature implementation

## Intent

Apply the `M317-C001` contract to the current authoring surfaces so future issue seeding stops generating the old verbose default shape.

## Contract

- Source of truth:
  - `docs/contracts/m317_issue_template_and_seed_generator_simplification_implementation_core_feature_implementation_c002_expectations.md`
  - `spec/planning/compiler/m317/m317_c001_planning_packet_and_issue_template_simplification_contract_and_architecture_freeze_contract.json`
- Implementation surfaces:
  - `.github/ISSUE_TEMPLATE/roadmap_execution.yml`
  - `.github/ISSUE_TEMPLATE/conformance_execution.yml`
  - `tmp/planning/cleanup_acceleration_program/generate_cleanup_acceleration_program.py`
- Verification:
  - `scripts/check_m317_c002_issue_template_and_seed_generator_simplification_implementation.py`
  - `tests/tooling/test_check_m317_c002_issue_template_and_seed_generator_simplification_implementation.py`
  - `scripts/run_m317_c002_lane_c_readiness.py`

## Closeout evidence

- Updated GitHub issue templates
- Regenerated local cleanup-program seed proving `## Validation posture` is emitted and `## Required deliverables` is gone from generated issue bodies
- Checker summary over the implementation surfaces and regenerated seed

## Next issue

- Next issue: `M317-D001`.
