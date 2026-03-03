# M243 CLI/Reporting and Output Contract Integration Conformance Matrix Implementation Expectations (D009)

Contract ID: `objc3c-cli-reporting-output-contract-integration-conformance-matrix-implementation/m243-d009-v1`
Status: Accepted
Scope: lane-D conformance matrix implementation continuity for CLI/reporting output contract integration and deterministic fail-closed summary/diagnostics payload closure.

## Objective

Expand lane-D conformance closure on top of D008 so CLI/reporting summary and
diagnostics output contracts remain deterministic, fail-closed, and traceable
across conformance-matrix consistency/readiness guardrails and deterministic
conformance-matrix key synthesis.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-D008`
- M243-D008 recovery and determinism hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m243_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_d008_expectations.md`
  - `spec/planning/compiler/m243/m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_packet.md`
  - `scripts/check_m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract.py`
- Packet/checker/test assets for D009 remain mandatory:
  - `spec/planning/compiler/m243/m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_packet.md`
  - `scripts/check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py`

## Deterministic Invariants

1. `Objc3CliReportingOutputContractConformanceMatrixImplementationSurface`
   remains the canonical lane-D D009 surface for CLI/reporting output contract
   conformance-matrix closure.
2. `BuildObjc3CliReportingOutputContractConformanceMatrixImplementationKey(...)`
   and
   `BuildObjc3CliReportingOutputContractConformanceMatrixImplementationSurface(...)`
   remain the canonical D009 conformance-matrix builders for:
   - D008 recovery/determinism continuity
   - summary/diagnostics replay-path continuity
   - conformance matrix key synthesis
3. `objc3c_frontend_c_api_runner.cpp` wires
   `BuildObjc3CliReportingOutputContractConformanceMatrixImplementationSurface(...)`
   and
   `IsObjc3CliReportingOutputContractConformanceMatrixImplementationSurfaceReady(...)`
   and fail-closes before writing summary output.
4. Runner summary `output_contract` payload remains deterministic and includes:
   - `recovery_determinism_key`
   - `conformance_matrix_key`
   - `conformance_matrix_consistent`
   - `conformance_matrix_ready`
   - `conformance_matrix_key_ready`
   - `core_feature_impl_ready`
5. Shared architecture/spec anchors explicitly include M243 lane-D D009:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_cli_reporting_output_contract_conformance_matrix_implementation_surface.h`
- `native/objc3c/src/io/objc3_cli_reporting_output_contract_recovery_determinism_hardening_surface.h`
- `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-d009-cli-reporting-output-contract-integration-conformance-matrix-implementation-contract`.
- `package.json` includes
  `test:tooling:m243-d009-cli-reporting-output-contract-integration-conformance-matrix-implementation-contract`.
- `package.json` includes `check:objc3c:m243-d009-lane-d-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_d009_cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract.py -q`
- `npm run check:objc3c:m243-d009-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D009/cli_reporting_and_output_contract_integration_conformance_matrix_implementation_contract_summary.json`
