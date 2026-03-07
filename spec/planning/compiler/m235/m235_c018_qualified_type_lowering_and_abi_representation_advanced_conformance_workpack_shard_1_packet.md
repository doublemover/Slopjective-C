# M235-C018 Qualified Type Lowering and ABI Representation Advanced Conformance Workpack (shard 1) Packet

Packet: `M235-C018`
Milestone: `M235`
Lane: `C`
Issue: `#5828`
Freeze date: `2026-03-05`
Dependencies: `M235-C017`

## Purpose

Freeze lane-C qualified type lowering and ABI representation edge-case and
compatibility completion continuity for M235 so dependency wiring remains deterministic and
fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m235_qualified_type_lowering_and_abi_representation_advanced_conformance_workpack_shard_1_c018_expectations.md`
- Checker:
  `scripts/check_m235_c018_qualified_type_lowering_and_abi_representation_advanced_conformance_workpack_shard_1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m235_c018_qualified_type_lowering_and_abi_representation_advanced_conformance_workpack_shard_1_contract.py`
- Dependency anchors from `M235-C017`:
  - `docs/contracts/m235_qualified_type_lowering_and_abi_representation_advanced_diagnostics_workpack_shard_1_c017_expectations.md`
  - `spec/planning/compiler/m235/m235_c017_qualified_type_lowering_and_abi_representation_advanced_diagnostics_workpack_shard_1_packet.md`
  - `scripts/check_m235_c017_qualified_type_lowering_and_abi_representation_advanced_diagnostics_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m235_c017_qualified_type_lowering_and_abi_representation_advanced_diagnostics_workpack_shard_1_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Readiness Chain

- `C017 readiness -> C018 checker -> C018 pytest`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `npm run check:objc3c:m235-c017-lane-c-readiness`
- `python scripts/check_m235_c018_qualified_type_lowering_and_abi_representation_advanced_conformance_workpack_shard_1_contract.py`
- `python -m pytest tests/tooling/test_check_m235_c018_qualified_type_lowering_and_abi_representation_advanced_conformance_workpack_shard_1_contract.py -q`
- `npm run check:objc3c:m235-c018-lane-c-readiness`

## Evidence Output

- `tmp/reports/m235/M235-C018/qualified_type_lowering_and_abi_representation_advanced_conformance_workpack_shard_1_contract_summary.json`














