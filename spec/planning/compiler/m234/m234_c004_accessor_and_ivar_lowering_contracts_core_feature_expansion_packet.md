# M234-C004 Accessor and Ivar Lowering Contracts Core Feature Expansion Packet

Packet: `M234-C004`
Milestone: `M234`
Lane: `C`
Issue: `#5722`
Freeze date: `2026-03-05`
Dependencies: `M234-C003`

## Purpose

Freeze lane-C accessor/ivar lowering contracts core-feature expansion
continuity for M234 so predecessor continuity remains explicit,
deterministic, and fail-closed. Code/spec anchors and milestone optimization
improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m234_accessor_and_ivar_lowering_contracts_core_feature_expansion_c004_expectations.md`
- Checker:
  `scripts/check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py`
- Dependency anchors (`M234-C003`):
  - `docs/contracts/m234_accessor_and_ivar_lowering_contracts_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m234/m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_packet.md`
  - `scripts/check_m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m234-c004-accessor-and-ivar-lowering-contracts-core-feature-expansion-contract`
  - `test:tooling:m234-c004-accessor-and-ivar-lowering-contracts-core-feature-expansion-contract`
  - `check:objc3c:m234-c004-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py`
- `python scripts/check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m234-c004-lane-c-readiness`

## Evidence Output

- `tmp/reports/m234/M234-C004/accessor_and_ivar_lowering_contracts_core_feature_expansion_summary.json`
