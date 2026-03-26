# M313-C001 Packet: Acceptance artifact schema and replay contract - Contract and architecture freeze

## Intent

Freeze the acceptance artifact schema before executable suite work begins so `M313-C002` and `M313-C003` emit one deterministic evidence format instead of growing parallel report families.

## Contract

- Source of truth:
  - `docs/contracts/m313_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m313/m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_schema.json`
- Verification:
  - `scripts/check_m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze.py`
  - `tests/tooling/test_check_m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze.py`
  - `scripts/run_m313_c001_lane_c_readiness.py`

## Schema focus

- canonical acceptance artifact envelope
- report-root layout under `tmp/reports/m313/`
- replay metadata required for deterministic re-execution
- compatibility-bridge fields reserved for `M313-C003`

## Next issue

- Next issue: `M313-C002`.
