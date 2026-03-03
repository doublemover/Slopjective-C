# M243 CLI/Reporting and Output Contract Integration Recovery and Determinism Hardening Expectations (D008)

Contract ID: `objc3c-cli-reporting-output-contract-integration-recovery-determinism-hardening/m243-d008-v1`
Status: Accepted
Scope: lane-D recovery/determinism hardening continuity for CLI/reporting output contract integration and deterministic fail-closed summary/diagnostics payload closure.

## Objective

Expand lane-D recovery and determinism closure on top of D007 so CLI/reporting
summary and diagnostics output contracts remain deterministic, fail-closed, and
traceable across recovery/determinism consistency/readiness guardrails and
deterministic recovery-determinism key synthesis.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-D007`
- M243-D007 diagnostics hardening anchors remain mandatory prerequisites:
  - `docs/contracts/m243_cli_reporting_and_output_contract_integration_diagnostics_hardening_d007_expectations.md`
  - `spec/planning/compiler/m243/m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_packet.md`
  - `scripts/check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m243_d007_cli_reporting_and_output_contract_integration_diagnostics_hardening_contract.py`
- Packet/checker/test assets for D008 remain mandatory:
  - `spec/planning/compiler/m243/m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_packet.md`
  - `scripts/check_m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract.py`

## Deterministic Invariants

1. `Objc3CliReportingOutputContractRecoveryDeterminismHardeningSurface`
   remains the canonical lane-D D008 surface for CLI/reporting output contract
   recovery/determinism closure.
2. `BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningKey(...)`
   and
   `BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurface(...)`
   remain the canonical D008 recovery/determinism builders for:
   - D007 diagnostics-hardening continuity
   - summary/diagnostics replay-path continuity
   - recovery and determinism key synthesis
3. `objc3c_frontend_c_api_runner.cpp` wires
   `BuildObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurface(...)`
   and
   `IsObjc3CliReportingOutputContractRecoveryDeterminismHardeningSurfaceReady(...)`
   and fail-closes before writing summary output.
4. Runner summary `output_contract` payload remains deterministic and includes:
   - `diagnostics_hardening_key`
   - `recovery_determinism_key`
   - `recovery_determinism_consistent`
   - `recovery_determinism_ready`
   - `core_feature_impl_ready`
5. Shared architecture/spec anchors explicitly include M243 lane-D D008:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_cli_reporting_output_contract_recovery_determinism_hardening_surface.h`
- `native/objc3c/src/io/objc3_cli_reporting_output_contract_diagnostics_hardening_surface.h`
- `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-d008-cli-reporting-output-contract-integration-recovery-determinism-hardening-contract`.
- `package.json` includes
  `test:tooling:m243-d008-cli-reporting-output-contract-integration-recovery-determinism-hardening-contract`.
- `package.json` includes `check:objc3c:m243-d008-lane-d-readiness`.

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m243_d008_cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m243-d008-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D008/cli_reporting_and_output_contract_integration_recovery_determinism_hardening_contract_summary.json`
