# M231 Declaration Grammar Expansion and Normalization Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-declaration-lowering-contract-updates/m231-c002-v1`
Status: Accepted
Owner: Objective-C 3 native lane-C
Issue: `#5515`
Dependencies: `M231-C001`

## Objective

Execute modular split and scaffolding governance for lane-C Declaration lowering contract updates, locking deterministic declaration-grammar boundaries before modular split/scaffolding work begins.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- `docs/contracts/m231_declaration_lowering_contract_updates_modular_split_and_scaffolding_c002_expectations.md`
- `spec/planning/compiler/m231/m231_c002_declaration_lowering_contract_updates_modular_split_and_scaffolding_packet.md`
- `scripts/check_m231_c002_declaration_lowering_contract_updates_contract.py`
- `tests/tooling/test_check_m231_c002_declaration_lowering_contract_updates_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m231-c002-lane-c-readiness`)

## Deterministic Invariants

1. Contract-freeze artifacts and packet anchors must remain synchronized and fail closed on drift.
2. Lane-A architecture/spec/package anchors must remain explicit and deterministic for `M231-C002`.
3. Readiness checks must preserve lane-C freeze validation and emit evidence under `tmp/reports`.

## Required Commands

- `check:objc3c:m231-c002-declaration-lowering-contract-updates-modular-split-and-scaffolding-contract`
- `test:tooling:m231-c002-declaration-lowering-contract-updates-modular-split-and-scaffolding-contract`
- `check:objc3c:m231-c002-lane-c-readiness`
- `python scripts/check_m231_c002_declaration_lowering_contract_updates_contract.py`
- `python -m pytest tests/tooling/test_check_m231_c002_declaration_lowering_contract_updates_contract.py -q`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m231/M231-C002/declaration_lowering_contract_updates_modular_split_and_scaffolding_summary.json`





