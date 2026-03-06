# M230 Conformance Corpus Governance and Sharding Modular Split/Scaffolding Expectations (A002)

Contract ID: `objc3c-conformance-corpus-governance-and-sharding-modular-split-scaffolding/m230-a002-v1`
Status: Accepted
Owner: Objective-C 3 native lane-A
Issue: `#5396`
Dependencies: `M230-A001`

## Objective

Establish modular split/scaffolding governance for lane-A conformance corpus governance and sharding so the A001 freeze artifacts are consumed deterministically and fail-closed while preparing expansion workpacks.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Inputs (M230-A001)

- `docs/contracts/m230_conformance_corpus_governance_and_sharding_contract_and_architecture_freeze_a001_expectations.md`
- `spec/planning/compiler/m230/m230_a001_conformance_corpus_governance_and_sharding_contract_and_architecture_freeze_packet.md`
- `scripts/check_m230_a001_conformance_corpus_governance_and_sharding_contract.py`
- `tests/tooling/test_check_m230_a001_conformance_corpus_governance_and_sharding_contract.py`

## Scope Anchors

- `docs/contracts/m230_conformance_corpus_governance_and_sharding_modular_split_scaffolding_a002_expectations.md`
- `spec/planning/compiler/m230/m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_packet.md`
- `scripts/check_m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_contract.py`
- `tests/tooling/test_check_m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m230-a002-lane-a-readiness`)

## Deterministic Invariants

1. A002 readiness must chain from `M230-A001` readiness and fail closed when dependency continuity drifts.
2. Modular split/scaffolding docs and packet anchors remain synchronized with architecture/spec coverage.
3. Parser replay and execution-smoke optimization commands stay present as required lane-A optimization inputs.

## Required Commands

- `check:objc3c:m230-a002-conformance-corpus-governance-and-sharding-modular-split-scaffolding-contract`
- `check:objc3c:m230-a002-lane-a-readiness`
- `python scripts/check_m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_contract.py`
- `python -m pytest tests/tooling/test_check_m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_contract.py -q`
- `npm run check:objc3c:m230-a002-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m230/M230-A002/conformance_corpus_governance_and_sharding_modular_split_scaffolding_summary.json`

