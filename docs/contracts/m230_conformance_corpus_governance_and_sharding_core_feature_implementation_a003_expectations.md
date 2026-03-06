# M230 Conformance Corpus Governance and Sharding Core Feature Implementation Expectations (A003)

Contract ID: `objc3c-conformance-corpus-governance-and-sharding-core-feature-implementation/m230-a003-v1`
Status: Accepted
Owner: Objective-C 3 native lane-A
Issue: `#5397`
Dependencies: `M230-A002`

## Objective

Execute core feature implementation governance for lane-A conformance corpus governance and sharding so modular split/scaffolding outputs from `M230-A002` are consumed deterministically and fail-closed before edge-case and compatibility workpacks begin.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Inputs (M230-A002)

- `docs/contracts/m230_conformance_corpus_governance_and_sharding_modular_split_scaffolding_a002_expectations.md`
- `spec/planning/compiler/m230/m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_packet.md`
- `scripts/check_m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_contract.py`
- `tests/tooling/test_check_m230_a002_conformance_corpus_governance_and_sharding_modular_split_scaffolding_contract.py`

## Scope Anchors

- `docs/contracts/m230_conformance_corpus_governance_and_sharding_core_feature_implementation_a003_expectations.md`
- `spec/planning/compiler/m230/m230_a003_conformance_corpus_governance_and_sharding_core_feature_implementation_packet.md`
- `scripts/check_m230_a003_conformance_corpus_governance_and_sharding_core_feature_implementation_contract.py`
- `tests/tooling/test_check_m230_a003_conformance_corpus_governance_and_sharding_core_feature_implementation_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m230-a003-lane-a-readiness`)

## Deterministic Invariants

1. A003 readiness must chain from `M230-A002` readiness and fail closed when dependency continuity drifts.
2. Core-feature implementation docs and packet anchors remain synchronized with architecture/spec coverage.
3. Parser replay and execution-smoke optimization commands stay present as required lane-A optimization inputs.

## Required Commands

- `check:objc3c:m230-a003-conformance-corpus-governance-and-sharding-core-feature-implementation-contract`
- `check:objc3c:m230-a003-lane-a-readiness`
- `python scripts/check_m230_a003_conformance_corpus_governance_and_sharding_core_feature_implementation_contract.py`
- `python -m pytest tests/tooling/test_check_m230_a003_conformance_corpus_governance_and_sharding_core_feature_implementation_contract.py -q`
- `npm run check:objc3c:m230-a003-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m230/M230-A003/conformance_corpus_governance_and_sharding_core_feature_implementation_summary.json`

