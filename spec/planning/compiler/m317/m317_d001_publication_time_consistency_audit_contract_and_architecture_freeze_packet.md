# M317-D001 Packet: Publication-time consistency audit contract - Contract and architecture freeze

## Intent

Freeze the audit contract for publication-time backlog tooling before `M317-E001/E002` rely on those audits as part of the closeout gate.

## Contract

- Source of truth:
  - `docs/contracts/m317_publication_time_consistency_audit_contract_and_architecture_freeze_d001_expectations.md`
  - `spec/planning/compiler/m317/m317_d001_publication_time_consistency_audit_contract_and_architecture_freeze_contract.json`
- Verification:
  - `scripts/check_m317_d001_publication_time_consistency_audit_contract_and_architecture_freeze.py`
  - `tests/tooling/test_check_m317_d001_publication_time_consistency_audit_contract_and_architecture_freeze.py`
  - `scripts/run_m317_d001_lane_d_readiness.py`

## Contract focus

- required audit facets
- required machine-readable report keys
- required implementation surfaces
- blocker-class failures for later publication work

## Next issue

- Next issue: `M317-E001`.
