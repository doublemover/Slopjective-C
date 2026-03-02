# M228-C006 IR Emission Completeness Edge-Case Expansion and Robustness Packet

Packet: `M228-C006`
Milestone: `M228`
Lane: `C`
Freeze date: `2026-03-02`
Dependencies: `M228-C005`

## Purpose

Freeze lane-C IR-emission edge-case expansion and robustness closure for M228
so C005 edge-case compatibility remains deterministic and fail-closed on
expansion/robustness drift before direct LLVM IR emission.

## Scope Anchors

- Contract:
  `docs/contracts/m228_ir_emission_completeness_edge_case_expansion_and_robustness_c006_expectations.md`
- Checker:
  `scripts/check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py`
- Core feature surfaces and frontend integration:
  - `native/objc3c/src/pipeline/objc3_ir_emission_core_feature_implementation_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/ir/objc3_ir_emitter.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- Dependency anchors from `M228-C005`:
  - `docs/contracts/m228_ir_emission_completeness_edge_case_and_compatibility_completion_c005_expectations.md`
  - `scripts/check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py`
  - `spec/planning/compiler/m228/m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_packet.md`
- Shared-file deltas required (not lane-owned in this packet):
  - `package.json`
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Gate Commands

- `python scripts/check_m228_c005_ir_emission_completeness_edge_case_and_compatibility_completion_contract.py`
- `python scripts/check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m228_c006_ir_emission_completeness_edge_case_expansion_and_robustness_contract.py -q`

## Evidence Output

- `tmp/reports/m228/M228-C006/ir_emission_completeness_edge_case_expansion_and_robustness_contract_summary.json`
- `tmp/reports/m228/M228-C006/closeout_validation_report.md`
