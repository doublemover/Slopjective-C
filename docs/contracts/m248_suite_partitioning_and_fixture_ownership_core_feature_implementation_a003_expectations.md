# M248 Suite Partitioning and Fixture Ownership Core Feature Implementation Expectations (A003)

Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-core-feature-implementation/m248-a003-v1`
Status: Accepted
Scope: M248 lane-A core feature implementation continuity for suite partitioning and fixture ownership dependency wiring.

## Objective

Fail closed unless lane-A core feature implementation dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Dependencies: `M248-A002`
- M248-A002 modular split/scaffolding anchors remain mandatory prerequisites:
  - `docs/contracts/m248_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_a002_expectations.md`
  - `spec/planning/compiler/m248/m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_packet.md`
  - `scripts/check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py`
  - `tests/tooling/test_check_m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_contract.py`
- Checker/test assets for A003 remain mandatory:
  - `scripts/check_m248_a003_suite_partitioning_and_fixture_ownership_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m248_a003_suite_partitioning_and_fixture_ownership_core_feature_implementation_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` remains the authoritative code anchor via inherited M248-A001 lane-A suite partitioning/fixture ownership fail-closed anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` remains the authoritative spec anchor via inherited M248-A001 lane-A fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` remains the authoritative metadata/spec anchor via inherited M248-A001 deterministic lane-A metadata anchor wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-a003-suite-partitioning-fixture-ownership-core-feature-implementation-contract`.
- `package.json` includes
  `test:tooling:m248-a003-suite-partitioning-fixture-ownership-core-feature-implementation-contract`.
- `package.json` includes `check:objc3c:m248-a003-lane-a-readiness`.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m248_a003_suite_partitioning_and_fixture_ownership_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m248_a003_suite_partitioning_and_fixture_ownership_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m248-a003-lane-a-readiness`

## Evidence Path

- `tmp/reports/m248/M248-A003/suite_partitioning_and_fixture_ownership_core_feature_implementation_contract_summary.json`
