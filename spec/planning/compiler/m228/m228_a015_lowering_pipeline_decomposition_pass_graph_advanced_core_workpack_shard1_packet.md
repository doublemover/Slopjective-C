# M228-A015 Lowering Pipeline Decomposition and Pass-Graph Advanced Core Workpack (Shard 1) Packet

Packet: `M228-A015`
Milestone: `M228`
Lane: `A`
Dependencies: `M228-A014`

## Purpose

Close lane-A advanced-core shard1 readiness for the lowering pipeline
decomposition/pass-graph by pinning deterministic toolchain/runtime GA
advanced-core consistency/readiness/key wiring and fail-closed diagnostics.

## Scope Anchors

- Contract:
  `docs/contracts/m228_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_a015_expectations.md`
- Checker:
  `scripts/check_m228_a015_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_a015_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_contract.py`
- Lane-A implementation surfaces:
  - `native/objc3c/src/pipeline/objc3_parse_lowering_readiness_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Operator runbook:
  `docs/runbooks/m228_wave_execution_runbook.md`
- Shared anchors:
  - `package.json`
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Gate Commands

- `python scripts/check_m228_a015_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_contract.py`
- `python -m pytest tests/tooling/test_check_m228_a015_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m228-a015-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Evidence Output

- `tmp/reports/m228/M228-A015/advanced_core_workpack_shard1_contract_summary.json`
