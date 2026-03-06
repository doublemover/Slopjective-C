# M230 Conformance Corpus Governance and Sharding Edge-Case and Compatibility Completion Expectations (A005)

Contract ID: `objc3c-conformance-corpus-governance-and-sharding-edge-case-and-compatibility-completion/m230-a005-v1`
Status: Accepted
Owner: Objective-C 3 native lane-A
Issue: `#5399`
Dependencies: `M230-A004`

## Objective

Execute edge-case and compatibility completion governance for lane-A conformance corpus governance and sharding so core-feature-expansion outputs from `M230-A004` are consumed deterministically and fail-closed before robustness workpacks begin.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Inputs (M230-A004)

- `docs/contracts/m230_conformance_corpus_governance_and_sharding_core_feature_expansion_a004_expectations.md`
- `spec/planning/compiler/m230/m230_a004_conformance_corpus_governance_and_sharding_core_feature_expansion_packet.md`
- `scripts/check_m230_a004_conformance_corpus_governance_and_sharding_core_feature_expansion_contract.py`
- `tests/tooling/test_check_m230_a004_conformance_corpus_governance_and_sharding_core_feature_expansion_contract.py`

## Scope Anchors

- `docs/contracts/m230_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_a005_expectations.md`
- `spec/planning/compiler/m230/m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_packet.md`
- `scripts/check_m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_contract.py`
- `tests/tooling/test_check_m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m230-a005-lane-a-readiness`)

## Deterministic Invariants

1. A005 readiness must chain from `M230-A004` readiness and fail closed when dependency continuity drifts.
2. Edge-case/compatibility docs and packet anchors remain synchronized with architecture/spec coverage.
3. Parser replay and execution-smoke optimization commands stay present as required lane-A optimization inputs.

## Required Commands

- `check:objc3c:m230-a005-conformance-corpus-governance-and-sharding-edge-case-and-compatibility-completion-contract`
- `check:objc3c:m230-a005-lane-a-readiness`
- `python scripts/check_m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m230_a005_conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m230-a005-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m230/M230-A005/conformance_corpus_governance_and_sharding_edge_case_and_compatibility_completion_summary.json`



