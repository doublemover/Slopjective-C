# M313-A001 Packet: Validation surface inventory and taxonomy - Contract and architecture freeze

## Intent

Freeze the measured validation surface and the taxonomy that later `M313` issues will use to reduce checker noise without losing coverage, replayability, or migration discipline.

## Contract

- Source of truth:
  - `docs/contracts/m313_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_a001_expectations.md`
  - `spec/planning/compiler/m313/m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze_inventory.json`
- Verification:
  - `scripts/check_m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze.py`
  - `tests/tooling/test_check_m313_a001_validation_surface_inventory_and_taxonomy_contract_and_architecture_freeze.py`
  - `scripts/run_m313_a001_lane_a_readiness.py`

## Inventory focus

- measured current validation counts
- taxonomy and namespace boundaries
- explicit migration-only surfaces instead of ad hoc checker sprawl
- active acceptance-suite roots that later issues must prefer

## Next issue

- Next issue: `M313-A002`.
