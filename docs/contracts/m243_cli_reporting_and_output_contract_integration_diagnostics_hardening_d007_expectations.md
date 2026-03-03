# M243 CLI/Reporting and Output Contract Integration Diagnostics Hardening Expectations (D007)

Contract ID: `objc3c-cli-reporting-output-contract-integration-diagnostics-hardening/m243-d007-v1`
Status: Accepted
Scope: lane-D diagnostics hardening continuity for CLI/reporting output contract integration and deterministic fail-closed summary/diagnostics payload closure.

## Objective

Expand lane-D diagnostics closure on top of D006 so CLI/reporting summary and
diagnostics output contracts remain deterministic, fail-closed, and traceable
across diagnostics-hardening consistency/readiness guardrails and deterministic
diagnostics-hardening key synthesis.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-D006`
- M243-D006 edge-case expansion and robustness anchors remain mandatory prerequisites:
  - `docs/contracts/m243_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_d006_expectations.md`
  - `spec/planning/compiler/m243/m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m243_d006_cli_reporting_and_output_contract_integration_edge_case_expansion_and_robustness_contract.py`
- Packet/checker/test assets for D007 remain mandatory:
  - `spec/planning/compiler/m243/m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_packet.md`
  - `scripts/check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py`

## Deterministic Invariants

1. `Objc3CliReportingOutputContractDiagnosticsHardeningSurface`
   remains the canonical lane-D D007 surface for CLI/reporting output contract
   diagnostics-hardening closure.
2. `BuildObjc3CliReportingOutputContractDiagnosticsHardeningSurface(...)`
   remains the canonical D007 diagnostics-hardening builder for:
   - D006 edge-case robustness continuity
   - summary/diagnostics path contract continuity
   - diagnostics hardening key synthesis
3. `objc3c_frontend_c_api_runner.cpp` wires
   `BuildObjc3CliReportingOutputContractDiagnosticsHardeningSurface(...)`
   and
   `IsObjc3CliReportingOutputContractDiagnosticsHardeningSurfaceReady(...)`
   and fail-closes before writing summary output.
4. Runner summary `output_contract` payload remains deterministic and includes:
   - `edge_case_robustness_key`
   - `diagnostics_hardening_key`
   - `diagnostics_hardening_consistent`
   - `diagnostics_hardening_ready`
   - `core_feature_impl_ready`
5. Shared architecture/spec anchors explicitly include M243 lane-D D007:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_cli_reporting_output_contract_diagnostics_hardening_surface.h`
- `native/objc3c/src/io/objc3_cli_reporting_output_contract_edge_case_expansion_and_robustness_surface.h`
- `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-d007-cli-reporting-output-contract-integration-diagnostics-hardening-contract`.
- `package.json` includes
  `test:tooling:m243-d007-cli-reporting-output-contract-integration-diagnostics-hardening-contract`.
- `package.json` includes `check:objc3c:m243-d007-lane-d-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m243-d007-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D007/cli_reporting_and_output_contract_integration_diagnostics_hardening_contract_summary.json`
