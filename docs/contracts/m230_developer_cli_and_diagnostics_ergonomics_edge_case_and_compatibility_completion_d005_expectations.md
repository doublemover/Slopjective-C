# M230 Developer CLI and diagnostics ergonomics Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-developer-cli-and-diagnostics-ergonomics/m230-d005-v1`
Status: Accepted
Owner: Objective-C 3 native lane-D
Issue: `#5444`
Dependencies: `M230-D004`

## Objective

Execute edge-case and compatibility completion governance for lane-D Developer CLI and diagnostics ergonomics, locking deterministic conformance-corpus boundaries before modular split/scaffolding work begins.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- `docs/contracts/m230_developer_cli_and_diagnostics_ergonomics_edge_case_and_compatibility_completion_d005_expectations.md`
- `spec/planning/compiler/m230/m230_d005_developer_cli_and_diagnostics_ergonomics_edge_case_and_compatibility_completion_packet.md`
- `scripts/check_m230_d005_developer_cli_and_diagnostics_ergonomics_contract.py`
- `tests/tooling/test_check_m230_d005_developer_cli_and_diagnostics_ergonomics_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m230-d005-lane-d-readiness`)

## Deterministic Invariants

1. Contract-freeze artifacts and packet anchors must remain synchronized and fail closed on drift.
2. Lane-A architecture/spec/package anchors must remain explicit and deterministic for `M230-D005`.
3. Readiness checks must preserve lane-D freeze validation and emit evidence under `tmp/reports`.

## Required Commands

- `check:objc3c:m230-d005-developer-cli-and-diagnostics-ergonomics-edge-case-and-compatibility-completion-contract`
- `test:tooling:m230-d005-developer-cli-and-diagnostics-ergonomics-edge-case-and-compatibility-completion-contract`
- `check:objc3c:m230-d005-lane-d-readiness`
- `python scripts/check_m230_d005_developer_cli_and_diagnostics_ergonomics_contract.py`
- `python -m pytest tests/tooling/test_check_m230_d005_developer_cli_and_diagnostics_ergonomics_contract.py -q`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m230/M230-D005/developer_cli_and_diagnostics_ergonomics_edge_case_and_compatibility_completion_summary.json`









