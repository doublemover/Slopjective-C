# M228-C009 IR Emission Completeness Conformance Matrix Implementation Packet

Packet: `M228-C009`
Milestone: `M228`
Lane: `C`
Freeze date: `2026-03-03`
Dependencies: `M228-C008`

## Purpose

Freeze lane-C IR-emission conformance matrix implementation closure so C008
recovery and determinism outputs remain deterministic and fail-closed on
conformance-matrix drift before direct LLVM IR emission.

## Scope Anchors

- Contract:
  `docs/contracts/m228_ir_emission_completeness_conformance_matrix_implementation_c009_expectations.md`
- Checker:
  `scripts/check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py`
- Core feature surface and frontend integration:
  - `native/objc3c/src/pipeline/objc3_ir_emission_core_feature_implementation_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- Dependency anchors from `M228-C008`:
  - `docs/contracts/m228_ir_emission_completeness_recovery_determinism_hardening_c008_expectations.md`
  - `scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
  - `spec/planning/compiler/m228/m228_c008_ir_emission_completeness_recovery_determinism_hardening_packet.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Gate Commands

- `python scripts/check_m228_c008_ir_emission_completeness_recovery_determinism_hardening_contract.py`
- `python scripts/check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py --summary-out tmp/reports/m228/M228-C009/ir_emission_completeness_conformance_matrix_implementation_contract_summary.json`
- `python -m pytest tests/tooling/test_check_m228_c009_ir_emission_completeness_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m228-c009-lane-c-readiness`

## Shared-file deltas required for full lane-C readiness

- `package.json`
  - add `check:objc3c:m228-c009-ir-emission-completeness-conformance-matrix-implementation-contract`
  - add `test:tooling:m228-c009-ir-emission-completeness-conformance-matrix-implementation-contract`
  - add `check:objc3c:m228-c009-lane-c-readiness` chained from C008 -> C009
- `native/objc3c/src/ARCHITECTURE.md`
  - add M228 lane-C C009 conformance matrix anchor text
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - add M228 lane-C C009 fail-closed conformance-matrix wiring text
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - add deterministic lane-C C009 conformance-matrix metadata anchors

## Evidence Output

- `tmp/reports/m228/M228-C009/ir_emission_completeness_conformance_matrix_implementation_contract_summary.json`
- `tmp/reports/m228/M228-C009/closeout_validation_report.md`
