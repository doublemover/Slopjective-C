# M243 CLI/Reporting and Output Contract Integration Modular Split/Scaffolding Expectations (D002)

Contract ID: `objc3c-cli-reporting-output-contract-integration-modular-split-scaffolding/m243-d002-v1`
Status: Accepted
Scope: lane-D modular split/scaffolding continuity for CLI/reporting output contracts and fail-closed diagnostics UX/fix-it output wiring.

## Objective

Fail closed unless lane-D CLI/reporting and output contract integration modular
split/scaffolding anchors remain explicit, deterministic, and traceable,
including code/spec anchors and milestone optimization improvements as
mandatory scope inputs.

## Dependency Scope

- Dependencies: `M243-D001`
- D001 prerequisite assets remain mandatory:
  - `docs/contracts/m243_cli_reporting_and_output_contract_integration_d001_expectations.md`
  - `scripts/check_m243_d001_cli_reporting_and_output_contract_integration_contract.py`
  - `tests/tooling/test_check_m243_d001_cli_reporting_and_output_contract_integration_contract.py`
- D002 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m243/m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_packet.md`
  - `scripts/check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py`

## Deterministic Invariants

1. `Objc3CliReportingOutputContractScaffold` remains the canonical modular
   split scaffold for lane-D CLI/reporting output contract gating.
2. `BuildObjc3CliReportingOutputContractScaffold(...)` remains the only
   canonical D002 scaffold builder for diagnostics schema, summary mode,
   deterministic output path contract shape, and stage-report output contract
   closure.
3. `objc3c_frontend_c_api_runner.cpp` wires
   `BuildObjc3CliReportingOutputContractScaffold(...)` and
   `IsObjc3CliReportingOutputContractScaffoldReady(...)` and fail-closes before
   summary artifacts are written.
4. `native/objc3c/src/io/objc3_diagnostics_artifacts.cpp` preserves
   deterministic diagnostics schema/artifact output contract continuity while
   D002 scaffold gating is enforced in runner wiring.
5. Architecture/spec anchors explicitly include M243 lane-D D002 modular split
   scaffold intent and deterministic metadata continuity wording.

## Code and Spec Anchors

- `native/objc3c/src/io/objc3_cli_reporting_output_contract_scaffold.h`
- `native/objc3c/src/io/objc3_diagnostics_artifacts.cpp`
- `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
- `native/objc3c/src/libobjc3c_frontend/objc3_cli_frontend.cpp`
- `native/objc3c/src/pipeline/frontend_pipeline_contract.h`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-d002-cli-reporting-output-contract-integration-modular-split-scaffolding-contract`.
- `package.json` includes
  `test:tooling:m243-d002-cli-reporting-output-contract-integration-modular-split-scaffolding-contract`.
- `package.json` includes `check:objc3c:m243-d002-lane-d-readiness`.

## Validation

- `python scripts/check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m243_d002_cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m243-d002-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D002/cli_reporting_and_output_contract_integration_modular_split_scaffolding_contract_summary.json`
