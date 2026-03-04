# M245-C004 Lowering/IR Portability Contracts Core Feature Expansion Packet

Packet: `M245-C004`
Milestone: `M245`
Lane: `C`
Issue: `#6639`
Freeze date: `2026-03-04`
Dependencies: `M245-C003`

## Purpose

Freeze lane-C lowering/IR portability contracts core-feature expansion
continuity for M245 so predecessor continuity remains explicit,
deterministic, and fail-closed. Code/spec anchors and milestone optimization
improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m245_lowering_ir_portability_contracts_core_feature_expansion_c004_expectations.md`
- Checker:
  `scripts/check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py`
- Dependency anchors (`M245-C003`):
  - `docs/contracts/m245_lowering_ir_portability_contracts_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m245/m245_c003_lowering_ir_portability_contracts_core_feature_implementation_packet.md`
  - `scripts/check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m245-c004-lowering-ir-portability-contracts-core-feature-expansion-contract`
  - `test:tooling:m245-c004-lowering-ir-portability-contracts-core-feature-expansion-contract`
  - `check:objc3c:m245-c004-lane-c-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py`
- `python scripts/check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m245-c004-lane-c-readiness`

## Evidence Output

- `tmp/reports/m245/M245-C004/lowering_ir_portability_contracts_core_feature_expansion_summary.json`
