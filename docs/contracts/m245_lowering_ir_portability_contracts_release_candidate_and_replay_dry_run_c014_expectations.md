# M245 Lowering/IR Portability Contracts Release-Candidate and Replay Dry-Run Expectations (C014)

Contract ID: `objc3c-lowering-ir-portability-contracts-release-candidate-and-replay-dry-run/m245-c014-v1`
Status: Accepted
Dependencies: `M245-C013`
Scope: M245 lane-C lowering/IR portability contracts release-candidate and replay dry-run continuity with explicit `M245-C013` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts release-candidate
and replay dry-run anchors remain explicit, deterministic, and traceable
across dependency surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6649` defines canonical lane-C release-candidate and replay dry-run scope.
- Dependency token: `M245-C013`.
- Upstream C013 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_c013_expectations.md`
  - `spec/planning/compiler/m245/m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_packet.md`
  - `scripts/check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py`
  - `tests/tooling/test_check_m245_c013_lowering_ir_portability_contracts_docs_and_operator_runbook_synchronization_contract.py`
- C014 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C014 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and M245-C013 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py`
- `python scripts/check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C014/lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract_summary.json`
