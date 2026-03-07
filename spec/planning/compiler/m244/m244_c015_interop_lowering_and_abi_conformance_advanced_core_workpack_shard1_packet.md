# m244-c015 Interop Lowering and ABI Conformance advanced core workpack (shard 1) Packet

Packet: `m244-c015`
Milestone: `M244`
Lane: `C`
Issue: `#6564`
Dependencies: `m244-c014`

## Purpose

Execute lane-C interop lowering and ABI conformance docs and operator runbook
synchronization governance on top of C014 docs/runbook synchronization assets so
downstream readiness and cross-lane conformance integration remain deterministic
and fail-closed.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_c015_expectations.md`
- Checker:
  `scripts/check_m244_c015_interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_c015_interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-c015-interop-lowering-abi-conformance-advanced-core-workpack-shard1-contract`
  - `test:tooling:m244-c015-interop-lowering-abi-conformance-advanced-core-workpack-shard1-contract`
  - `check:objc3c:m244-c015-lane-c-readiness`

## Dependency Anchors (m244-c014)

- `docs/contracts/m244_interop_lowering_and_abi_conformance_release_candidate_and_replay_dry_run_c014_expectations.md`
- `spec/planning/compiler/m244/m244_c014_interop_lowering_and_abi_conformance_release_candidate_and_replay_dry_run_packet.md`
- `scripts/check_m244_c014_interop_lowering_and_abi_conformance_release_candidate_and_replay_dry_run_contract.py`
- `tests/tooling/test_check_m244_c014_interop_lowering_and_abi_conformance_release_candidate_and_replay_dry_run_contract.py`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m244_c015_interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract.py`
- `python scripts/check_m244_c015_interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c015_interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m244-c015-lane-c-readiness`

## Evidence Output

- `tmp/reports/m244/m244-c015/interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract_summary.json`




