# M248-A002 Suite Partitioning and Fixture Ownership Modular Split/Scaffolding Packet

Packet: `M248-A002`
Milestone: `M248`
Lane: `A`
Freeze date: `2026-03-02`
Dependencies: `M248-A001`

## Purpose

Freeze lane-A modular split/scaffolding prerequisites for M248 suite partitioning and fixture ownership continuity so dependency wiring remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m248_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_a002_expectations.md`
- Checker:
  `scripts/check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py`
- Dependency anchors from `M248-A001`:
  - `docs/contracts/m248_suite_partitioning_and_fixture_ownership_contract_freeze_a001_expectations.md`
  - `spec/planning/compiler/m248/m248_a001_suite_partitioning_and_fixture_ownership_contract_freeze_packet.md`
  - `scripts/check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py`
  - `tests/tooling/test_check_m248_a001_suite_partitioning_and_fixture_ownership_contract.py`
- Inherited architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m248-a002-lane-a-readiness`

## Evidence Output

- `tmp/reports/m248/M248-A002/suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract_summary.json`
