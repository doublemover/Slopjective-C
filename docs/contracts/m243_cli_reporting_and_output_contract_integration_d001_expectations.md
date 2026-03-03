# M243 CLI/Reporting and Output Contract Integration Contract and Architecture Freeze Expectations (D001)

Contract ID: `objc3c-cli-reporting-output-contract-integration-freeze/m243-d001-v1`
Status: Accepted
Scope: M243 lane-D CLI/reporting and output contract integration freeze for diagnostics UX and fix-it engine.

## Objective

Fail closed unless lane-D CLI/reporting and output contract integration anchors
remain explicit, deterministic, and traceable across code/spec anchors and
milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: none
- Packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m243/m243_d001_cli_reporting_and_output_contract_integration_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m243_d001_cli_reporting_and_output_contract_integration_contract.py`
  - `tests/tooling/test_check_m243_d001_cli_reporting_and_output_contract_integration_contract.py`

## Code, Architecture, and Spec Anchors

- `native/objc3c/src/libobjc3c_frontend/objc3_cli_frontend.cpp` preserves the
  CLI artifact handoff boundary.
- `native/objc3c/src/io/objc3_diagnostics_artifacts.cpp` preserves diagnostics
  text/json artifact output contract shape.
- `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp` preserves
  canonical summary mode, stage-report fields, and deterministic summary output
  path behavior.
- `native/objc3c/src/pipeline/frontend_pipeline_contract.h` preserves
  output-directory and emit-stage output path contract fields.
- `native/objc3c/src/ARCHITECTURE.md` includes explicit M243 lane-D D001
  contract and architecture freeze anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-D CLI/reporting and
  output contract governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-D
  CLI/reporting output metadata anchors for `M243-D001`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m243-d001-cli-reporting-output-contract-integration-contract`.
- `package.json` includes
  `test:tooling:m243-d001-cli-reporting-output-contract-integration-contract`.
- `package.json` includes `check:objc3c:m243-d001-lane-d-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m243_d001_cli_reporting_and_output_contract_integration_contract.py`
- `python -m pytest tests/tooling/test_check_m243_d001_cli_reporting_and_output_contract_integration_contract.py -q`
- `npm run check:objc3c:m243-d001-lane-d-readiness`

## Evidence Path

- `tmp/reports/m243/M243-D001/cli_reporting_and_output_contract_integration_contract_summary.json`
