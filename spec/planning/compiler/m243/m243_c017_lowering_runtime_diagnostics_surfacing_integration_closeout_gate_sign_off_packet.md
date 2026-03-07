# M243-C017 Lowering/Runtime Diagnostics Surfacing Cross-Lane Integration Sync Packet

Packet: `M243-C017`
Milestone: `M243`
Lane: `C`
Freeze date: `2026-03-03`
Dependencies: `M243-C016`

## Purpose

Freeze lane-C lowering/runtime diagnostics surfacing integration closeout and gate sign-off
closure so C016 performance/quality guardrail outputs remain deterministic and
fail-closed on integration-closeout-gate-sign-off consistency/readiness or
integration-closeout-gate-sign-off-key continuity drift.

## Scope Anchors

- Contract:
  `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_c017_expectations.md`
- Checker:
  `scripts/check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py`
- Dependency anchors from `M243-C016`:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_c016_expectations.md`
  - `spec/planning/compiler/m243/m243_c016_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_packet.md`
  - `scripts/check_m243_c016_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_contract.py`
  - `tests/tooling/test_check_m243_c016_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m243-c017-lowering-runtime-diagnostics-surfacing-integration-closeout-gate-sign-off-contract`
  - `test:tooling:m243-c017-lowering-runtime-diagnostics-surfacing-integration-closeout-gate-sign-off-contract`
  - `check:objc3c:m243-c017-lane-c-readiness`
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

- `python scripts/check_m243_c016_lowering_runtime_diagnostics_surfacing_advanced_edge_compatibility_workpack_shard_1_contract.py`
- `python scripts/check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py`
- `python scripts/check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_c017_lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract.py -q`
- `npm run check:objc3c:m243-c017-lane-c-readiness`

## Evidence Output

- `tmp/reports/m243/M243-C017/lowering_runtime_diagnostics_surfacing_integration_closeout_gate_sign_off_contract_summary.json`






