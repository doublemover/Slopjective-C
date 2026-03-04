# M245 Lowering/IR Portability Contracts Advanced Core Workpack (Shard 1) Expectations (C015)

Contract ID: `objc3c-lowering-ir-portability-contracts-advanced-core-workpack-shard1/m245-c015-v1`
Status: Accepted
Dependencies: `M245-C014`
Scope: M245 lane-C lowering/IR portability contracts advanced core workpack (shard 1) continuity with explicit `M245-C014` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts advanced core
workpack (shard 1) anchors remain explicit, deterministic, and traceable
across dependency surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6650` defines canonical lane-C advanced core workpack (shard 1) scope.
- Dependency token: `M245-C014`.
- Upstream C014 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_c014_expectations.md`
  - `spec/planning/compiler/m245/m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py`
- C015 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_packet.md`
  - `scripts/check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C015 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and M245-C014 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py`
- `python scripts/check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C015/lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract_summary.json`
