# M239-C012 Qualified Type Lowering and ABI Representation Cross-lane Integration Sync Packet

Packet: `M239-C012`
Milestone: `M239`
Lane: `C`
Issue: `#5811`
Freeze date: `2026-03-05`
Dependencies: none

## Purpose

Freeze lane-C qualified type lowering and ABI representation contract
prerequisites for M239 so nullability, generics, and qualifier completeness
lowering/ABI boundaries remain deterministic and fail-closed, including
code/spec anchors and milestone optimization improvements as mandatory scope
inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m239_cfg_ssa_lowering_and_phi_construction_cross_lane_integration_sync_c012_expectations.md`
- Checker:
  `scripts/check_m239_c012_cfg_ssa_lowering_and_phi_construction_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m239_c012_cfg_ssa_lowering_and_phi_construction_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m239-c012-cfg-ssa-lowering-and-phi-construction-contract`
  - `test:tooling:m239-c012-cfg-ssa-lowering-and-phi-construction-contract`
  - `check:objc3c:m239-c012-lane-c-readiness`
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

- `python scripts/check_m239_c012_cfg_ssa_lowering_and_phi_construction_contract.py`
- `python -m pytest tests/tooling/test_check_m239_c012_cfg_ssa_lowering_and_phi_construction_contract.py -q`
- `npm run check:objc3c:m239-c012-lane-c-readiness`

## Evidence Output

- `tmp/reports/m239/M239-C012/cfg_ssa_lowering_and_phi_construction_contract_summary.json`













