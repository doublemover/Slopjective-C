# M243 CLI/Reporting and Output Contract Integration Core Feature Expansion Expectations (D004)

Contract ID: `objc3c-cli-reporting-output-contract-integration-core-feature-expansion/m243-d004-v1`
Status: Accepted
Scope: lane-D core feature expansion continuity for CLI/reporting output contract integration and deterministic summary/diagnostics output contract payload closure.

## Objective

Implement lane-D core feature expansion closure on top of D003 so
CLI/reporting summary and diagnostics output contracts remain deterministic,
fail-closed, and traceable across summary payload fields, diagnostics
output-path continuity, and emit-prefix filename contracts.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-D003`
- M243-D003 core feature implementation anchors remain mandatory prerequisites:
  - `docs/contracts/m243_cli_reporting_and_output_contract_integration_core_feature_implementation_d003_expectations.md`
  - `spec/planning/compiler/m243/m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_packet.md`
  - `scripts/check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m243_d003_cli_reporting_and_output_contract_integration_core_feature_implementation_contract.py`
- Packet/checker/test assets for D004 remain mandatory:
  - `spec/planning/compiler/m243/m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_packet.md`
  - `scripts/check_m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_contract.py`

## Deterministic Invariants

1. `Objc3CliReportingOutputContractCoreFeatureExpansionSurface` remains the
   canonical lane-D D004 surface for CLI/reporting output contract expansion
   closure.
2. `BuildObjc3CliReportingOutputContractCoreFeatureExpansionSurface(...)`
   remains the canonical D004 expansion builder for:
   - summary output-path payload consistency
   - diagnostics output-path payload consistency
   - diagnostics emit-prefix filename continuity
   - core feature expansion key synthesis
3. `objc3c_frontend_c_api_runner.cpp` wires
   `BuildObjc3CliReportingOutputContractCoreFeatureExpansionSurface(...)` and
   `IsObjc3CliReportingOutputContractCoreFeatureExpansionSurfaceReady(...)`
   and fail-closes before writing summary output.
4. Runner summary `output_contract` payload remains deterministic and includes:
   - `diagnostics_schema_version`
   - `summary_mode`
   - `scaffold_key`
   - `core_feature_key`
   - `core_feature_expansion_key`
   - `core_feature_expansion_ready`
   - `core_feature_impl_ready`
5. Shared architecture/spec anchors explicitly include M243 lane-D D004:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_cli_reporting_output_contract_core_feature_expansion_surface.h`
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
  `check:objc3c:m243-d004-cli-reporting-output-contract-integration-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m243-d004-cli-reporting-output-contract-integration-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m243-d004-lane-d-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m243-d004-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D004/cli_reporting_and_output_contract_integration_core_feature_expansion_contract_summary.json`
