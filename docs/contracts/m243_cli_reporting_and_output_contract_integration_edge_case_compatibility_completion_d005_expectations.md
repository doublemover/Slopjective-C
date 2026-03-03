# M243 CLI/Reporting and Output Contract Integration Edge-Case Compatibility Completion Expectations (D005)

Contract ID: `objc3c-cli-reporting-output-contract-integration-edge-case-compatibility-completion/m243-d005-v1`
Status: Accepted
Scope: lane-D edge-case compatibility completion continuity for CLI/reporting output contract integration and deterministic fail-closed summary/diagnostics payload closure.

## Objective

Complete lane-D edge-case compatibility closure on top of D004 so
CLI/reporting summary and diagnostics output contracts remain deterministic,
fail-closed, and traceable across case-folded path distinctness, output path
control-character hygiene, and deterministic edge-case compatibility key
synthesis.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-D004`
- M243-D004 core feature expansion anchors remain mandatory prerequisites:
  - `docs/contracts/m243_cli_reporting_and_output_contract_integration_core_feature_expansion_d004_expectations.md`
  - `spec/planning/compiler/m243/m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_packet.md`
  - `scripts/check_m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m243_d004_cli_reporting_and_output_contract_integration_core_feature_expansion_contract.py`
- Packet/checker/test assets for D005 remain mandatory:
  - `spec/planning/compiler/m243/m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_packet.md`
  - `scripts/check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py`

## Deterministic Invariants

1. `Objc3CliReportingOutputContractEdgeCaseCompatibilitySurface` remains the
   canonical lane-D D005 surface for CLI/reporting output contract edge-case
   compatibility closure.
2. `BuildObjc3CliReportingOutputContractEdgeCaseCompatibilitySurface(...)`
   remains the canonical D005 compatibility builder for:
   - summary output-path extension compatibility
   - diagnostics output-path suffix compatibility
   - case-folded summary/diagnostics path distinctness
   - output-path control-character hygiene
   - edge-case compatibility key synthesis
3. `objc3c_frontend_c_api_runner.cpp` wires
   `BuildObjc3CliReportingOutputContractEdgeCaseCompatibilitySurface(...)` and
   `IsObjc3CliReportingOutputContractEdgeCaseCompatibilitySurfaceReady(...)`
   and fail-closes before writing summary output.
4. Runner summary `output_contract` payload remains deterministic and includes:
   - `core_feature_expansion_key`
   - `edge_case_compatibility_key`
   - `edge_case_compatibility_consistent`
   - `edge_case_compatibility_ready`
   - `core_feature_impl_ready`
5. Shared architecture/spec anchors explicitly include M243 lane-D D005:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_cli_reporting_output_contract_edge_case_compatibility_surface.h`
- `native/objc3c/src/io/objc3_cli_reporting_output_contract_core_feature_expansion_surface.h`
- `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-d005-cli-reporting-output-contract-integration-edge-case-compatibility-completion-contract`.
- `package.json` includes
  `test:tooling:m243-d005-cli-reporting-output-contract-integration-edge-case-compatibility-completion-contract`.
- `package.json` includes `check:objc3c:m243-d005-lane-d-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m243-d005-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D005/cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract_summary.json`
