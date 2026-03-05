# M233-D002 Runtime Metadata and Lookup Plumbing Modular Split and Scaffolding Packet

Packet: `M233-D002`
Milestone: `M233`
Lane: `D`
Freeze date: `2026-03-02`
Dependencies: `M233-D001`

## Purpose

Freeze lane-D runtime metadata and lookup plumbing modular split/scaffolding
prerequisites for M233 so dependency continuity stays deterministic and
fail-closed, including code/spec anchors and milestone optimization
improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_d002_expectations.md`
- Checker:
  `scripts/check_m233_d002_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m233_d002_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract.py`
- Prerequisite D001 assets:
  - `docs/contracts/m233_runtime_metadata_and_lookup_plumbing_contract_and_architecture_freeze_d001_expectations.md`
  - `spec/planning/compiler/m233/m233_d001_runtime_metadata_and_lookup_plumbing_contract_and_architecture_freeze_packet.md`
  - `scripts/check_m233_d001_runtime_metadata_and_lookup_plumbing_contract.py`
  - `tests/tooling/test_check_m233_d001_runtime_metadata_and_lookup_plumbing_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m233-d002-installer-runtime-operations-lookup-plumbing-modular-split-scaffolding-contract`
  - `test:tooling:m233-d002-installer-runtime-operations-lookup-plumbing-modular-split-scaffolding-contract`
  - `check:objc3c:m233-d002-lane-d-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Gate Commands

- `python scripts/check_m233_d002_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m233_d002_runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m233-d002-lane-d-readiness`

## Evidence Output

- `tmp/reports/m233/M233-D002/runtime_metadata_and_lookup_plumbing_modular_split_scaffolding_contract_summary.json`
