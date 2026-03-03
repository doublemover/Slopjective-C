# M228-B010 Ownership-Aware Lowering Behavior Conformance Corpus Expansion Packet

Packet: `M228-B010`
Milestone: `M228`
Lane: `B`
Freeze date: `2026-03-03`
Dependencies: `M228-B009`

## Purpose

Freeze lane-B conformance-corpus expansion closure for ownership-aware lowering
behavior so conformance-matrix continuity and parse-lowering conformance-corpus
evidence remain deterministic and fail-closed before LLVM IR emission.

## Scope Anchors

- Contract:
  `docs/contracts/m228_ownership_aware_lowering_behavior_conformance_corpus_expansion_b010_expectations.md`
- Checker:
  `scripts/check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py`
- Dependency anchors (`M228-B009`):
  - `docs/contracts/m228_ownership_aware_lowering_behavior_conformance_matrix_implementation_b009_expectations.md`
  - `scripts/check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py`
  - `spec/planning/compiler/m228/m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_packet.md`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m228-b010-ownership-aware-lowering-behavior-conformance-corpus-expansion-contract`
  - `test:tooling:m228-b010-ownership-aware-lowering-behavior-conformance-corpus-expansion-contract`
  - `check:objc3c:m228-b010-lane-b-readiness`
- Ownership-aware lowering conformance-corpus integration:
  - `native/objc3c/src/pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:lowering-replay-proof`

## Gate Commands

- `python scripts/check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py`
- `python scripts/check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m228-b010-lane-b-readiness`

## Evidence Output

- `tmp/reports/m228/M228-B010/ownership_aware_lowering_behavior_conformance_corpus_expansion_contract_summary.json`
