# M245-C015 Lowering/IR Portability Contracts Advanced Core Workpack (Shard 1) Packet

Packet: `M245-C015`
Milestone: `M245`
Lane: `C`
Theme: `advanced core workpack (shard 1)`
Issue: `#6650`
Freeze date: `2026-03-04`
Dependencies: `M245-C014`

## Purpose

Freeze lane-C lowering/IR portability contracts advanced core workpack (shard
1) continuity for M245 so predecessor continuity remains explicit,
deterministic, and fail-closed.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m245_lowering_ir_portability_contracts_advanced_core_workpack_shard1_c015_expectations.md`
- Checker:
  `scripts/check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py`
- Dependency anchors (`M245-C014`):
  - `docs/contracts/m245_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_c014_expectations.md`
  - `spec/planning/compiler/m245/m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m245_c014_lowering_ir_portability_contracts_release_candidate_and_replay_dry_run_contract.py`
- Shared wiring handoff:
  - `native/objc3c/src/ARCHITECTURE.md` (shared-owner follow-up)
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` (shared-owner follow-up)
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md` (shared-owner follow-up)
  - `package.json` lane-C readiness chain (shared-owner follow-up)

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py`
- `python scripts/check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c015_lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract.py -q`

## Evidence Output

- `tmp/reports/m245/M245-C015/lowering_ir_portability_contracts_advanced_core_workpack_shard1_contract_summary.json`
