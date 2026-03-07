# M230-D006 Developer CLI and diagnostics ergonomics Contract and Architecture Freeze Packet

Packet: `M230-D006`
Milestone: `M230`
Lane: `A`
Issue: `#5445`
Freeze date: `2026-03-06`
Dependencies: `M230-D005`

## Purpose

Execute edge-case expansion and robustness governance for lane-D Developer CLI and diagnostics ergonomics so corpus boundary anchors remain deterministic and fail-closed before modular split/scaffolding expansion.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Expectations:
  `docs/contracts/m230_developer_cli_and_diagnostics_ergonomics_edge_case_expansion_and_robustness_d006_expectations.md`
- Checker:
  `scripts/check_m230_d006_developer_cli_and_diagnostics_ergonomics_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m230_d006_developer_cli_and_diagnostics_ergonomics_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- Readiness scripts (`package.json`):
  - `check:objc3c:m230-d006-developer-cli-and-diagnostics-ergonomics-edge-case-expansion-and-robustness-contract`
  - `test:tooling:m230-d006-developer-cli-and-diagnostics-ergonomics-edge-case-expansion-and-robustness-contract`
  - `check:objc3c:m230-d006-lane-d-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Gate Commands

- `python scripts/check_m230_d006_developer_cli_and_diagnostics_ergonomics_contract.py`
- `python -m pytest tests/tooling/test_check_m230_d006_developer_cli_and_diagnostics_ergonomics_contract.py -q`
- `npm run check:objc3c:m230-d006-lane-d-readiness`

## Evidence Output

- `tmp/reports/m230/M230-D006/developer_cli_and_diagnostics_ergonomics_edge_case_expansion_and_robustness_summary.json`











