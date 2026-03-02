# M248-A001 Suite Partitioning and Fixture Ownership Contract Freeze Packet

Packet: `M248-A001`
Milestone: `M248`
Lane: `A`
Freeze date: `2026-03-02`
Dependencies: none

## Purpose

Freeze lane-A suite partitioning and fixture ownership prerequisites for M248 so
CI sharding governance and fixture isolation remain deterministic and fail-closed,
including code/spec anchors and milestone optimization improvements as mandatory
scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m248_suite_partitioning_and_fixture_ownership_contract_freeze_a001_expectations.md`
- Checker:
  `scripts/check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m248-a001-suite-partitioning-fixture-ownership-contract`
  - `test:tooling:m248-a001-suite-partitioning-fixture-ownership-contract`
  - `check:objc3c:m248-a001-lane-a-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py`
- `python -m pytest tests/tooling/test_check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py -q`
- `npm run check:objc3c:m248-a001-lane-a-readiness`

## Evidence Output

- `tmp/reports/m248/M248-A001/suite_partitioning_and_fixture_ownership_contract_summary.json`
