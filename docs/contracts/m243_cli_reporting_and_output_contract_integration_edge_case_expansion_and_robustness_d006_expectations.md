# M243 CLI/Reporting and Output Contract Integration Edge-Case Expansion and Robustness Expectations (D006)

Contract ID: `objc3c-cli-reporting-output-contract-integration-edge-case-expansion-and-robustness/m243-d006-v1`
Status: Accepted
Scope: lane-D edge-case expansion and robustness continuity for CLI/reporting output contract integration and deterministic fail-closed summary/diagnostics payload closure.

## Objective

Expand lane-D edge-case closure on top of D005 so CLI/reporting summary and
diagnostics output contracts remain deterministic, fail-closed, and traceable
across output-path expansion and robustness guardrails, including deterministic
robustness key synthesis.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-D005`
- M243-D005 edge-case compatibility completion anchors remain mandatory prerequisites:
  - `docs/contracts/m243_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_d005_expectations.md`
  - `spec/planning/compiler/m243/m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_packet.md`
  - `scripts/check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m243_d005_cli_reporting_and_output_contract_integration_edge_case_compatibility_completion_contract.py`
- Packet/checker/test assets for D006 remain mandatory:
  - `spec/planning/compiler/m243/m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract.py`

## Deterministic Invariants

1. `Objc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface`
   remains the canonical lane-D D006 surface for CLI/reporting output contract
   edge-case expansion and robustness closure.
2. `BuildObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface(...)`
   remains the canonical D006 expansion/robustness builder for:
   - summary/diagnostics output parent-path presence
   - output-path length-budget consistency
   - output-path trailing-space hygiene
   - edge-case robustness key synthesis
3. `objc3c_frontend_c_api_runner.cpp` wires
   `BuildObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurface(...)`
   and
   `IsObjc3CliReportingOutputContractEdgeCaseExpansionAndRobustnessSurfaceReady(...)`
   and fail-closes before writing summary output.
4. Runner summary `output_contract` payload remains deterministic and includes:
   - `edge_case_compatibility_key`
   - `edge_case_robustness_key`
   - `edge_case_expansion_consistent`
   - `edge_case_robustness_consistent`
   - `edge_case_robustness_ready`
   - `core_feature_impl_ready`
5. Shared architecture/spec anchors explicitly include M243 lane-D D006:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_cli_reporting_output_contract_edge_case_expansion_and_robustness_surface.h`
- `native/objc3c/src/io/objc3_cli_reporting_output_contract_edge_case_compatibility_surface.h`
- `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-d006-cli-reporting-output-contract-integration-edge-case-expansion-and-robustness-contract`.
- `package.json` includes
  `test:tooling:m243-d006-cli-reporting-output-contract-integration-edge-case-expansion-and-robustness-contract`.
- `package.json` includes `check:objc3c:m243-d006-lane-d-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m243-d006-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D006/cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract_summary.json`
