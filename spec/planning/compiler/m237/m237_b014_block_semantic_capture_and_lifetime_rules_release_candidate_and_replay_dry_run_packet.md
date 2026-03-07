# M237-B014 Qualifier/Generic Semantic Inference Contract and Architecture Freeze Packet

Packet: `M237-B014`
Milestone: `M237`
Lane: `B`
Issue: `#5781`
Freeze date: `2026-03-05`
Dependencies: `M237-B013`

## Purpose

Freeze lane-B qualifier/generic semantic inference contract prerequisites for M237 so nullability, generics, and qualifier completeness governance remains deterministic and fail-closed, including code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m237_block_semantic_capture_and_lifetime_rules_release_candidate_and_replay_dry_run_b014_expectations.md`
- Checker:
  `scripts/check_m237_b014_block_semantic_capture_and_lifetime_rules_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m237_b014_block_semantic_capture_and_lifetime_rules_contract.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m237-b014-block-semantic-capture-and-lifetime-rules-release-candidate-and-replay-dry-run-contract`
  - `test:tooling:m237-b014-block-semantic-capture-and-lifetime-rules-release-candidate-and-replay-dry-run-contract`
  - `check:objc3c:m237-b014-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-replay-proof`
- `test:objc3c:parser-ast-extraction`

## Gate Commands

- `python scripts/check_m237_b014_block_semantic_capture_and_lifetime_rules_contract.py`
- `python -m pytest tests/tooling/test_check_m237_b014_block_semantic_capture_and_lifetime_rules_contract.py -q`
- `npm run check:objc3c:m237-b014-lane-b-readiness`

## Evidence Output

- `tmp/reports/m237/M237-B014/block_semantic_capture_and_lifetime_rules_release_candidate_and_replay_dry_run_summary.json`




























