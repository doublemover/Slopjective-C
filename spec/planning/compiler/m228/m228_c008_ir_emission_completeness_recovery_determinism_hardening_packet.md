# M228-C008 IR Emission Completeness Recovery and Determinism Hardening Packet

Packet: `M228-C008`
Milestone: `M228`
Lane: `C`
Freeze date: `2026-03-03`
Dependencies: `M228-C007`

## Purpose

Freeze lane-C IR-emission recovery and determinism hardening closure so C007
diagnostics hardening outputs remain deterministic and fail-closed on replay
drift before direct LLVM IR emission.

## Scope Anchors

- Contract:
  `docs/contracts/m228_ir_emission_completeness_recovery_determinism_hardening_c008_expectations.md`
- Checker:
  `scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
- Core feature surfaces and frontend integration:
  - `native/objc3c/src/pipeline/objc3_ir_emission_core_feature_implementation_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- Dependency anchors from `M228-C007`:
  - `docs/contracts/m228_ir_emission_completeness_diagnostics_hardening_c007_expectations.md`
  - `scripts/check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py`
  - `spec/planning/compiler/m228/m228_c007_ir_emission_completeness_diagnostics_hardening_packet.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Gate Commands

- `python scripts/check_m228_c007_ir_emission_completeness_diagnostics_hardening_contract.py`
- `python scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py -q`

## Shared-file deltas required for full lane-C readiness

- `package.json`
  - add `check:objc3c:m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract`
  - add `test:tooling:m228-c008-ir-emission-completeness-recovery-determinism-hardening-contract`
  - add `check:objc3c:m228-c008-lane-c-readiness` chained from C007 -> C008
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C008 recovery/determinism anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C008 fail-closed recovery/determinism wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C008 recovery/determinism metadata anchors

## Evidence Output

- `tmp/reports/m228/M228-C008/ir_emission_completeness_recovery_determinism_hardening_contract_summary.json`
- `tmp/reports/m228/M228-C008/closeout_validation_report.md`
