# M243-C015 Lowering/Runtime Diagnostics Surfacing Cross-Lane Integration Sync Packet

Packet: `M243-C015`
Milestone: `M243`
Lane: `C`
Freeze date: `2026-03-03`
Dependencies: `M243-C014`

## Purpose

Freeze lane-C lowering/runtime diagnostics surfacing advanced core workpack (shard 1)
closure so C014 performance/quality guardrail outputs remain deterministic and
fail-closed on advanced-core-workpack-shard-1 consistency/readiness or
advanced-core-workpack-shard-1-key continuity drift.

## Scope Anchors

- Contract:
  `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_advanced_core_workpack_shard_1_c015_expectations.md`
- Checker:
  `scripts/check_m243_c015_lowering_runtime_diagnostics_surfacing_advanced_core_workpack_shard_1_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m243_c015_lowering_runtime_diagnostics_surfacing_advanced_core_workpack_shard_1_contract.py`
- Dependency anchors from `M243-C014`:
  - `docs/contracts/m243_lowering_runtime_diagnostics_surfacing_release_candidate_replay_dry_run_c014_expectations.md`
  - `spec/planning/compiler/m243/m243_c014_lowering_runtime_diagnostics_surfacing_release_candidate_replay_dry_run_packet.md`
  - `scripts/check_m243_c014_lowering_runtime_diagnostics_surfacing_release_candidate_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m243_c014_lowering_runtime_diagnostics_surfacing_release_candidate_replay_dry_run_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m243-c015-lowering-runtime-diagnostics-surfacing-advanced-core-workpack-shard-1-contract`
  - `test:tooling:m243-c015-lowering-runtime-diagnostics-surfacing-advanced-core-workpack-shard-1-contract`
  - `check:objc3c:m243-c015-lane-c-readiness`
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

- `python scripts/check_m243_c014_lowering_runtime_diagnostics_surfacing_release_candidate_replay_dry_run_contract.py`
- `python scripts/check_m243_c015_lowering_runtime_diagnostics_surfacing_advanced_core_workpack_shard_1_contract.py`
- `python scripts/check_m243_c015_lowering_runtime_diagnostics_surfacing_advanced_core_workpack_shard_1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_c015_lowering_runtime_diagnostics_surfacing_advanced_core_workpack_shard_1_contract.py -q`
- `npm run check:objc3c:m243-c015-lane-c-readiness`

## Evidence Output

- `tmp/reports/m243/M243-C015/lowering_runtime_diagnostics_surfacing_advanced_core_workpack_shard_1_contract_summary.json`




