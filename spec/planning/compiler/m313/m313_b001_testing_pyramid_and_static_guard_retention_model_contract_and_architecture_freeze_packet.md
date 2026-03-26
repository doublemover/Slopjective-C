# M313-B001 Packet: Testing pyramid and static-guard retention model - Contract and architecture freeze

## Intent

Freeze the testing pyramid and retained-static-guard model so later `M313` implementation issues can consolidate validation around executable suites without guessing which static checks may legitimately remain.

## Contract

- Source of truth:
  - `docs/contracts/m313_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze_b001_expectations.md`
  - `spec/planning/compiler/m313/m313_b001_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze_model.json`
- Verification:
  - `scripts/check_m313_b001_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze.py`
  - `tests/tooling/test_check_m313_b001_testing_pyramid_and_static_guard_retention_model_contract_and_architecture_freeze.py`
  - `scripts/run_m313_b001_lane_b_readiness.py`

## Model focus

- executable acceptance-suite-first truth
- retained static-guard classes
- migration-only validation classes
- prohibited new milestone-local validation patterns
- linkage to the A001 inventory, A002 ratchets, and A003 suite boundaries

## Next issue

- Next issue: `M313-B002`.
