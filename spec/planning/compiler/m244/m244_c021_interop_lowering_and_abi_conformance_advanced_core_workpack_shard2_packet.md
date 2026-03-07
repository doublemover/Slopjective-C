# m244-c021 Interop Lowering and ABI Conformance advanced core workpack (shard 2) Packet

Packet: `m244-c021`
Milestone: `M244`
Lane: `C`
Issue: `#6570`
Dependencies: `m244-c020`

## Purpose

Execute lane-C interop lowering and ABI conformance docs and operator runbook
synchronization governance on top of C020 docs/runbook synchronization assets so
downstream readiness and cross-lane conformance integration remain deterministic
and fail-closed.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m244_interop_lowering_and_abi_conformance_advanced_core_workpack_shard2_c021_expectations.md`
- Checker:
  `scripts/check_m244_c021_interop_lowering_and_abi_conformance_advanced_core_workpack_shard2_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m244_c021_interop_lowering_and_abi_conformance_advanced_core_workpack_shard2_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m244-c021-interop-lowering-abi-conformance-advanced-core-workpack-shard2-contract`
  - `test:tooling:m244-c021-interop-lowering-abi-conformance-advanced-core-workpack-shard2-contract`
  - `check:objc3c:m244-c021-lane-c-readiness`

## Dependency Anchors (m244-c020)

- `docs/contracts/m244_interop_lowering_and_abi_conformance_advanced_performance_workpack_shard1_c020_expectations.md`
- `spec/planning/compiler/m244/m244_c020_interop_lowering_and_abi_conformance_advanced_performance_workpack_shard1_packet.md`
- `scripts/check_m244_c020_interop_lowering_and_abi_conformance_advanced_performance_workpack_shard1_contract.py`
- `tests/tooling/test_check_m244_c020_interop_lowering_and_abi_conformance_advanced_performance_workpack_shard1_contract.py`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m244_c021_interop_lowering_and_abi_conformance_advanced_core_workpack_shard2_contract.py`
- `python scripts/check_m244_c021_interop_lowering_and_abi_conformance_advanced_core_workpack_shard2_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c021_interop_lowering_and_abi_conformance_advanced_core_workpack_shard2_contract.py -q`
- `npm run check:objc3c:m244-c021-lane-c-readiness`

## Evidence Output

- `tmp/reports/m244/m244-c021/interop_lowering_and_abi_conformance_advanced_core_workpack_shard2_contract_summary.json`










