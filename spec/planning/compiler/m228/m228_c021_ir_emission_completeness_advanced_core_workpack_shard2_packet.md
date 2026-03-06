# M228-C021 IR Emission Completeness Advanced Core Workpack (Shard 2) Packet

Packet: `M228-C021`
Milestone: `M228`
Lane: `C`
Freeze date: `2026-03-06`
Issue: `#5247`
Dependencies: `M228-C020`

## Purpose

Freeze lane-C IR-emission advanced core workpack (shard 2)
closure so C020 advanced-conformance outputs remain deterministic and fail-closed on
parse/typed integration handoff drift before direct LLVM IR emission advances.

## Scope Anchors

- Contract:
  `docs/contracts/m228_ir_emission_completeness_advanced_core_workpack_shard2_c021_expectations.md`
- Checker:
  `scripts/check_m228_c021_ir_emission_completeness_advanced_core_workpack_shard2_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_c021_ir_emission_completeness_advanced_core_workpack_shard2_contract.py`
- Core feature surface and frontend integration:
  - `native/objc3c/src/pipeline/objc3_ir_emission_core_feature_implementation_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- Dependency anchors from `M228-C020`:
  - `docs/contracts/m228_ir_emission_completeness_advanced_performance_workpack_shard1_c020_expectations.md`
  - `scripts/check_m228_c020_ir_emission_completeness_advanced_performance_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m228_c020_ir_emission_completeness_advanced_performance_workpack_shard1_contract.py`
  - `spec/planning/compiler/m228/m228_c020_ir_emission_completeness_advanced_performance_workpack_shard1_packet.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-smoke`
- `test:objc3c:lowering-replay-proof`

## Gate Commands

- `python scripts/check_m228_c020_ir_emission_completeness_advanced_performance_workpack_shard1_contract.py`
- `python scripts/check_m228_c021_ir_emission_completeness_advanced_core_workpack_shard2_contract.py --summary-out tmp/reports/m228/M228-C021/ir_emission_completeness_advanced_core_workpack_shard2_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c021_ir_emission_completeness_advanced_core_workpack_shard2_contract.py -q`
- `npm run check:objc3c:m228-c021-lane-c-readiness`

## Shared-file deltas required for full lane-C readiness

- `package.json`
  - add `check:objc3c:m228-c021-ir-emission-completeness-advanced-core-workpack-shard2-contract`
  - add `test:tooling:m228-c021-ir-emission-completeness-advanced-core-workpack-shard2-contract`
  - add `check:objc3c:m228-c021-lane-c-readiness` chained from C020 -> C021
- `docs/runbooks/m228_wave_execution_runbook.md`
  - add M228 lane-C C021 contract and validation command sequence anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C021 advanced-integration shard1 anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C021 fail-closed advanced-integration shard1 wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C021 advanced-integration shard1 metadata anchors

## Evidence Output

- `tmp/reports/m228/M228-C021/ir_emission_completeness_advanced_core_workpack_shard2_contract_summary.json`
- `tmp/reports/m228/M228-C021/closeout_validation_report.md`







