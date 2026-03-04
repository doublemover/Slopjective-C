# M245-C007 Lowering/IR Portability Contracts Diagnostics Hardening Packet

Packet: `M245-C007`
Milestone: `M245`
Lane: `C`
Issue: `#6642`
Freeze date: `2026-03-04`
Dependencies: `M245-C006`

## Purpose

Freeze lane-C lowering/IR portability contracts diagnostics hardening
continuity for M245 so predecessor continuity remains explicit,
deterministic, and fail-closed.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m245_lowering_ir_portability_contracts_diagnostics_hardening_c007_expectations.md`
- Checker:
  `scripts/check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py`
- Dependency anchors (`M245-C006`):
  - `docs/contracts/m245_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_c006_expectations.md`
  - `spec/planning/compiler/m245/m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_contract.py`
- Shared wiring handoff:
  - `native/objc3c/src/ARCHITECTURE.md` (shared-owner follow-up)
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` (shared-owner follow-up)
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md` (shared-owner follow-up)
  - `package.json` lane-C readiness chain (shared-owner follow-up)

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`
- `test:objc3c:execution-replay-proof`

## Gate Commands

- `python scripts/check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py`
- `python scripts/check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py -q`

## Evidence Output

- `tmp/reports/m245/M245-C007/lowering_ir_portability_contracts_diagnostics_hardening_summary.json`
