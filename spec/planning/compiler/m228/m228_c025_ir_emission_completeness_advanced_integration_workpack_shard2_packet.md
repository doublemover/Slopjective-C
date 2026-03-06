# M228-C025 IR Emission Completeness Advanced Integration Workpack (Shard 2) Packet

Packet: `M228-C025`
Milestone: `M228`
Lane: `C`
Freeze date: `2026-03-06`
Issue: `#5251`
Dependencies: `M228-C024`

## Purpose

Freeze lane-C IR-emission advanced integration workpack (shard 2)
closure so C024 advanced-conformance outputs remain deterministic and fail-closed on
parse/typed integration handoff drift before direct LLVM IR emission advances.

## Scope Anchors

- Contract:
  `docs/contracts/m228_ir_emission_completeness_advanced_integration_workpack_shard2_c025_expectations.md`
- Checker:
  `scripts/check_m228_c025_ir_emission_completeness_advanced_integration_workpack_shard2_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_c025_ir_emission_completeness_advanced_integration_workpack_shard2_contract.py`
- Core feature surface and frontend integration:
  - `native/objc3c/src/pipeline/objc3_ir_emission_core_feature_implementation_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- Dependency anchors from `M228-C024`:
  - `docs/contracts/m228_ir_emission_completeness_advanced_conformance_workpack_shard2_c024_expectations.md`
  - `scripts/check_m228_c024_ir_emission_completeness_advanced_conformance_workpack_shard2_contract.py`
  - `tests/tooling/test_check_m228_c024_ir_emission_completeness_advanced_conformance_workpack_shard2_contract.py`
  - `spec/planning/compiler/m228/m228_c024_ir_emission_completeness_advanced_conformance_workpack_shard2_packet.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-smoke`
- `test:objc3c:lowering-replay-proof`

## Gate Commands

- `python scripts/check_m228_c024_ir_emission_completeness_advanced_conformance_workpack_shard2_contract.py`
- `python scripts/check_m228_c025_ir_emission_completeness_advanced_integration_workpack_shard2_contract.py --summary-out tmp/reports/m228/M228-C025/ir_emission_completeness_advanced_integration_workpack_shard2_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c025_ir_emission_completeness_advanced_integration_workpack_shard2_contract.py -q`
- `npm run check:objc3c:m228-c025-lane-c-readiness`

## Shared-file deltas required for full lane-C readiness

- `package.json`
  - add `check:objc3c:m228-c025-ir-emission-completeness-advanced-integration-workpack-shard2-contract`
  - add `test:tooling:m228-c025-ir-emission-completeness-advanced-integration-workpack-shard2-contract`
  - add `check:objc3c:m228-c025-lane-c-readiness` chained from C024 -> C025
- `docs/runbooks/m228_wave_execution_runbook.md`
  - add M228 lane-C C025 contract and validation command sequence anchors
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C025 advanced-integration shard1 anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C025 fail-closed advanced-integration shard1 wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C025 advanced-integration shard1 metadata anchors

## Evidence Output

- `tmp/reports/m228/M228-C025/ir_emission_completeness_advanced_integration_workpack_shard2_contract_summary.json`
- `tmp/reports/m228/M228-C025/closeout_validation_report.md`











