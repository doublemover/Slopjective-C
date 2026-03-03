# M243 CLI/Reporting and Output Contract Integration Core Feature Implementation Expectations (D003)

Contract ID: `objc3c-cli-reporting-output-contract-integration-core-feature-implementation/m243-d003-v1`
Status: Accepted
Scope: lane-D core feature implementation continuity for CLI/reporting output contract integration and deterministic fail-closed output contracts.

## Objective

Implement lane-D core feature closure so CLI/reporting output contracts remain
deterministic, fail-closed, and traceable across summary payloads, diagnostics
paths, and stage-report continuity. Code/spec anchors and milestone
optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-D002`
- D002 modular split/scaffolding anchors remain mandatory prerequisites:
  - `docs/contracts/m243_cli_reporting_and_output_contract_integration_modular_split_scaffolding_d002_expectations.md`
  - `spec/planning/compiler/m243/m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_packet.md`
  - `scripts/check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py`
- Packet/checker/test assets for D003 remain mandatory:
  - `spec/planning/compiler/m243/m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_packet.md`
  - `scripts/check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py`

## Deterministic Invariants

1. `Objc3CliReportingOutputContractCoreFeatureSurface` remains the canonical
   lane-D core feature implementation surface for CLI/reporting output contract
   closure.
2. `BuildObjc3CliReportingOutputContractCoreFeatureSurface(...)` remains the
   canonical D003 core feature builder for scaffold continuity, summary output
   path determinism, diagnostics output path determinism, and stage-report
   output continuity.
3. `objc3c_frontend_c_api_runner.cpp` wires
   `BuildObjc3CliReportingOutputContractCoreFeatureSurface(...)` and
   `IsObjc3CliReportingOutputContractCoreFeatureSurfaceReady(...)` and
   fail-closes before writing summary output.
4. Runner summary payloads preserve deterministic output contract fields:
   diagnostics schema version, summary mode, scaffold key, and core feature
   key.

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_cli_reporting_output_contract_core_feature_surface.h`
- `native/objc3c/src/io/objc3_cli_reporting_output_contract_scaffold.h`
- `native/objc3c/src/io/objc3_diagnostics_artifacts.cpp`
- `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
- `native/objc3c/src/libobjc3c_frontend/objc3_cli_frontend.cpp`
- `native/objc3c/src/pipeline/frontend_pipeline_contract.h`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m243-d003-cli-reporting-output-contract-integration-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m243-d003-lane-d-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m243-d003-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D003/cli_reporting_and_output_contract_integration_core_feature_implementation_contract_summary.json`
